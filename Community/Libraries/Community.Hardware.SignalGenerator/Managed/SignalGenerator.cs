////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using System;
using System.Runtime.CompilerServices;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace Community.Hardware
    {
    /// <summary>
    /// Allows the background generation of a binary signal on any available output pin.
    /// <para>SignalGenerator can modulate a carrier frequency as used in IR remote control.</para>
    /// <para>Easy settings for led pulse signal.</para>
    /// <para>The generation start can be delayed to a specified DateTime to synchronize multiple signals.</para>
    /// <para>Note that only one toggle can occur at a time for all signals. Toggles can not be closer than about 6.5탎 on STM32F4.</para>
    /// <para>The signal is stopped if it is too fast to be properly generated (<see cref="Status"/> is set to Error)</para>
    /// </summary>
    public class SignalGenerator : IDisposable
        {
        /// <summary>
        /// Create a new <see cref="SignalGenerator"/> generating a signal at the specified frequency with the specified duty cycle.
        /// <para>The signal is stopped when the returned object is disposed. You must store a reference to the returned object to keep the signal generating.</para>
        /// <para>Note that PWM is far more powerful as a simple frequency generator with no repeat count.</para>
        /// </summary>
        /// <param name="pin">Output pin</param>
        /// <param name="frequency">Signal frequency (Hz) (1 to 50000)</param>
        ///<param name="repeatCount">The repeat count, 0 for never ending</param>
        /// <param name="dutyCycle">Duty cycle in range 0.0 to 1.0 (excluded)</param>
        /// <returns></returns>
        /// <exception cref="ArgumentOutOfRangeException">frequency is greater than 50kHz or dutyCycle lead to a level time less than 5탎</exception>
        static public SignalGenerator StartNew(Cpu.Pin pin, double frequency, int repeatCount, double dutyCycle = 0.5) {
            if (frequency <= 0)
                throw new ArgumentOutOfRangeException("frequency", "frequency must be greater than 0");
            if (frequency > 50000)
                throw new ArgumentOutOfRangeException("frequency", "frequency must be lower than 50kHz");
            uint onTime = (uint)(dutyCycle * 1000000 / frequency);
            uint offTime = (uint)((1 - dutyCycle) * 1000000 / frequency);
            if (onTime < 5 || offTime < 5)
                throw new ArgumentOutOfRangeException("dutyCycle", "Level times less than 5탎");
            SignalGenerator sg = new SignalGenerator(pin, false);
            sg.LevelTimes = new uint[] { onTime, offTime };
            sg.Start(repeatCount);
            return sg;
            }
        /// <summary>
        /// Create a new <see cref="SignalGenerator"/> repeating the specified number of pulses. This is designed to show a numerical information with a simple led. 
        /// <para>Note that the signal is stopped when the returned object is disposed. You must store a reference to the returned object to keep the signal generating.</para>
        /// </summary>
        /// <param name="pin"></param>
        /// <param name="pulseCount">The number of pulses</param>
        /// <param name="burstInterval">Number of pulse period to separate the pulse bursts</param>
        /// <param name="pulsePeriod">Pulse period in seconds</param>
        /// <returns></returns>
        static public SignalGenerator StartNewLedPulse(Cpu.Pin pin, int pulseCount, int burstInterval = 3, double pulsePeriod = 0.25) {
            if (pulseCount <= 1)
                throw new ArgumentOutOfRangeException("pulseCount", "pulseCount must be greater than 1");
            SignalGenerator sg = new SignalGenerator(pin, false);
            sg.LevelTimes = new uint[2 * pulseCount];
            for (int i = 0; i < pulseCount; i++) {
                sg.LevelTimes[2 * i] = (uint)(pulsePeriod * 0.2 * 1000000);
                sg.LevelTimes[2 * i + 1] = (uint)(pulsePeriod * 0.8 * 1000000);
                }
            //add the burst interval to the last low level
            sg.LevelTimes[2 * pulseCount - 1] += (uint)(pulsePeriod * burstInterval * 1000000);
            sg.Start();
            return sg;

            }
        /// <summary>
        /// SignalGenerator status
        /// </summary>
        public enum SignalStatus
            {
            /// <summary>
            /// Signal pattern is not set (<see cref="LevelTimes"/> is <c>null</c>)
            /// </summary>
            Uninitialized,
            /// <summary>
            /// Signal is ready to start
            /// </summary>
            Ready,
            /// <summary>
            /// Signal is currently generating
            /// </summary>
            Active,
            /// <summary>
            /// Signal is waiting for start time (delayed start)
            /// </summary>
            Pending,
            /// <summary>
            /// Signal is too fast to be generated properly. Generation is cancelled.
            /// </summary>
            Error
            }
        /// <summary>
        /// Memory space used by native code
        /// </summary>
        private int[] _signalInfo;

        private Cpu.Pin _pin;
        /// <summary>
        /// Gets the output pin
        /// </summary>
        public Cpu.Pin Pin {
            get { return _pin; }
            }

        private bool _invert;

        /// <summary>
        /// Indicates wheter the output is inverted or not.
        /// <para>When inverted, the first signal level is low and the modulation applies on low levels.</para>
        /// </summary>
        public bool Invert {
            get { return _invert; }
            }

        /// <summary>
        /// Gets the signal repeat decounter. Returns the repeat remaining count, including the currently generating signal.
        /// <para>Returns 0 if signal generation is ended</para>
        /// <para>For never ending signal, count down from 0 and roll over endlessly</para>
        /// </summary>
        public int RepeatCount {
            get { return _signalInfo[0]; }
            }
        /// <summary>
        /// Gets the current signal generation index.
        /// </summary>
        public int LevelTimeIndex {
            get { return _signalInfo[1]; }
            }

        /// <summary>
        /// Indicates the signal generation status
        /// </summary>
        public SignalStatus Status {
            get {
                if (_signalInfo[2] == (int)SignalStatus.Uninitialized && _levelTimes != null)
                    _signalInfo[2] = (int)SignalStatus.Ready;
                return (SignalStatus)_signalInfo[2];
                }
            }

        private uint[] _levelTimes;
        /// <summary>
        /// Level times for signal generation (in 탎).
        /// <para>The signal start by a high level and then toggle after each specified level time.</para>
        /// <para>The first level is low if <see cref="Invert"/> is <c>true</c>.</para>
        /// <para>Any array value can be changed on the fly when the signal is generating (but not the array itself).</para>
        /// </summary>
        /// <exception cref="InvalidOperationException">Thrown if signal is started.</exception>
        /// <exception cref="ArgumentException">Thrown if array length is less than 2.</exception>
        public uint[] LevelTimes {
            get { return _levelTimes; }
            set {
                if (Status > SignalStatus.Ready)
                    throw new InvalidOperationException("Stop signal generation before setting LevelTimes array.");
                if (value != null && value.Length < 2)
                    throw new ArgumentException("Can not generate a signal with only one level time.");
                _levelTimes = value;
                }
            }

        /// <summary>
        /// Gets or set the pin state
        /// </summary>
        public extern bool PinState {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
            }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void NativeInitialize();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void NativeDispose();

        /// <summary>
        /// Initialize a new instance of <see cref="SignalGenerator"/> for the specified pin
        /// </summary>
        /// <param name="pin">Output pin</param>
        /// <param name="invert">Initial state applied to <paramref name="pin"/> when configured for output.</param>
        /// <exception cref="InvalidOperationException">Thrown if the specified pin is already reserved by another task</exception>
        public SignalGenerator(Cpu.Pin pin, bool invert = false) {
            if (pin == Cpu.Pin.GPIO_NONE)
                throw new ArgumentException("pin");
            if (!Port.ReservePin(pin, true))
                throw new InvalidOperationException("Pin already reserved for another task");
            _signalInfo = new int[30];
            _pin = pin;
            _invert = invert;
            NativeInitialize();
            }
        /// <summary>
        /// Release the output pin and native resources
        /// </summary>
        public void Dispose() {
            if (_pin != Cpu.Pin.GPIO_NONE) {
                Port.ReservePin(Pin, false);
                NativeDispose();
                _pin = Cpu.Pin.GPIO_NONE;
                _levelTimes = null;
                }
            }

        /// <summary>
        /// Start the signal generation. The signal is repeated until <see cref="Stop"/> or <see cref="Dispose"/>
        /// <para>The signal pattern is specified by <see cref="LevelTimes"/> array.</para>
        /// </summary>
        public void Start() {
            Start(0, 0, 0);
            }
        /// <summary>
        /// Start the signal generation at the specified time. The signal is repeated until <see cref="Stop"/> or <see cref="Dispose"/>
        /// <para>The signal pattern is specified by <see cref="LevelTimes"/> array.</para>
        /// </summary>
        /// <param name="startTime">The signal start time (any value lower than DateTime.Now will start immediately)</param>
        public void Start(DateTime startTime) {
            Start(startTime, 0, 0);
            }
        /// <summary>
        /// Start the signal generation for the specified repeat count.
        /// <para>The signal pattern is specified by <see cref="LevelTimes"/> array.</para>
        /// </summary>
        ///<param name="repeatCount">The repeat count, 0 for never ending</param>
        public void Start(int repeatCount) {
            Start(repeatCount, 0);
            }

        /// <summary>
        /// Start the signal generation with specified repetition count and optional carrier modulation.
        /// <para>The signal pattern is specified by <see cref="LevelTimes"/> array.</para>
        /// </summary>
        ///<param name="repeatCount">The repeat count, 0 for never ending</param>
        /// <param name="carrierFrequency">The carrier frequency (Hz) modulated by the signal, 0 if not used</param>
        /// <param name="carrierDutyCycle">The carrier duty cycle in range 0.0, 1.0 (excluded)</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Start(int repeatCount, double carrierFrequency, double carrierDutyCycle = 0.5);

        /// <summary>
        /// Start the signal generation at specified time with specified repetition count and optional carrier modulation.
        /// <para>The signal pattern is specified by <see cref="LevelTimes"/> array.</para>
        /// </summary>
        /// <param name="startTime">The signal start time (any value lower than DateTime.Now will start immediately)</param>
        ///<param name="repeatCount">The repeat count, 0 for never ending</param>
        /// <param name="carrierFrequency">The carrier frequency (Hz) modulated by the signal, 0 if not used</param>
        /// <param name="carrierDutyCycle">The carrier duty cycle in range 0.0, 1.0 (excluded)</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Start(DateTime startTime, int repeatCount, double carrierFrequency, double carrierDutyCycle = 0.5);

        /// <summary>
        /// Stops the signal generation
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Stop();

        /// <summary>
        /// Gets the string representation for this instance
        /// </summary>
        /// <returns></returns>
        public override string ToString() {
            String str = "Signal generator, Pin " + Pin;
            switch (Status) {
                case SignalStatus.Uninitialized:
                    str += " (Uninitialized)";
                    break;
                case SignalStatus.Ready:
                    str += " (Ready)";
                    break;
                case SignalStatus.Active:
                    str += " (Active)";
                    break;
                case SignalStatus.Pending:
                    str += " (Pending)";
                    break;
                case SignalStatus.Error:
                    str += " (Error)";
                    break;
                default:
                    break;
                }
            return str;
            }
        }
    }
