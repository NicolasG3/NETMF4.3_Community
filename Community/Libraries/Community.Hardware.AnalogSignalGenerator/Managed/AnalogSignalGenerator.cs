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
    /// Generate an arbitrary analog signal using DAC.
    /// <para>Note that the channel count may not be the same as <see cref="AnalogOutput"/></para>
    /// </summary>
    public class AnalogSignalGenerator : IDisposable
        {
        /// <summary>
        /// Get the channel count for the hardware.
        /// </summary>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static public extern int GetChannelCount();
        /// <summary>
        /// Get the output pin for the specified channel
        /// </summary>
        /// <param name="channel">Analog output channel</param>
        /// <returns>The pin</returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static public extern Cpu.Pin GetPin(Cpu.AnalogOutputChannel channel);

        /// <summary>
        /// Get the minimum clock divisor applied to <see cref="Cpu.SystemClock"/> to define the max output sample for the hardware.
        /// </summary>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static public extern UInt16 GetMinClockDivisor();

        /// <summary>
        /// AnalogSignalGenerator status
        /// </summary>
        public enum SignalStatus
            {
            /// <summary>
            /// Signal pattern is not set (<see cref="RawLevels"/> is <c>null</c>)
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

        private readonly Cpu.AnalogOutputChannel _channel;
        /// <summary>
        /// Get the analog output channel
        /// </summary>
        public Cpu.AnalogOutputChannel Channel {
            get { return _channel; }
            }

        private Cpu.Pin _pin;
        /// <summary>
        /// Get the analog output pin
        /// </summary>
        public Cpu.Pin Pin {
            get { return _pin; }
            }
        /// <summary>
        /// Gets the signal repeat decounter. Returns the repeat remaining count, including the currently generating signal.
        /// <para>Returns 0 if signal generation is ended</para>
        /// <para>For never ending signal, count down from 0 and roll over endlessly</para>
        /// </summary>
        public int RepeatCount {
            get { return _signalInfo[0]; }
            }
        ///// <summary>
        ///// Gets the current signal generation index.
        ///// </summary>
        //public int LevelTimeIndex {
        //    get { return _signalInfo[1]; }
        //    }

        /// <summary>
        /// Indicates the signal generation status
        /// </summary>
        public SignalStatus Status {
            get {
                if (_signalInfo[2] == (int)SignalStatus.Uninitialized && _rawLevels != null)
                    _signalInfo[2] = (int)SignalStatus.Ready;
                return (SignalStatus)_signalInfo[2];
                }
            }

        private UInt16[] _rawLevels;
        /// <summary>
        /// Output binary levels for signal generation. The max usable value depends on the bit precision of the hardware DAC: 1023 for 10bits, 4095 for 12bits...
        /// <para>The ouptut signal is updated at <see cref="Cpu.SlowClock"/>/<see cref="ClockDivisor"/> rate.</para>
        /// <para>Any array value can be changed on the fly when the signal is generating (but not the array itself).</para>
        /// </summary>
        /// <exception cref="InvalidOperationException">Thrown if signal is started.</exception>
        /// <exception cref="ArgumentException">Thrown if array length is less than 2.</exception>
        public UInt16[] RawLevels {
            get { return _rawLevels; }
            set {
                if (Status > SignalStatus.Ready)
                    throw new InvalidOperationException("Stop signal generation before setting RawLevels array.");
                if (value != null && value.Length < 2)
                    throw new ArgumentException("Can not generate a signal with only one level.");
                _rawLevels = value;
                }
            }
        private UInt16 _clockDivisor;
        /// <summary>
        /// Get or set the divisor applied to <see cref="Cpu.SlowClock"/> to define the sample clock
        /// </summary>
        public UInt16 ClockDivisor {
            get { return _clockDivisor; }
            set {
                if (Status > SignalStatus.Ready)
                    throw new InvalidOperationException("Stop signal generation before setting ClockDivisor.");
                if (value < GetMinClockDivisor())
                    throw new ArgumentOutOfRangeException("ClockDivisor can not be less than " + GetMinClockDivisor().ToString());
                _clockDivisor = value;
                }
            }
        /// <summary>
        /// Get the output sample clock (defined by <see cref="ClockDivisor"/>)
        /// </summary>
        public uint SampleClock {
            get { return Cpu.SystemClock / ClockDivisor; }
            }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void NativeInitialize();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void NativeDispose();

        /// <summary>
        /// Initialize a new instance of <see cref="AnalogSignalGenerator"/> for the specified channel
        /// </summary>
        /// <param name="channel">The analog output channel</param>
        /// <param name="clockDivisor">The divisor applied to <see cref="Cpu.SlowClock"/> to define the sample clock</param>
        public AnalogSignalGenerator(Cpu.AnalogOutputChannel channel, int clockDivisor) {
            int channelCount = GetChannelCount();
            if ((int)channel < 0 || (int)channel >= channelCount)
                throw new ArgumentOutOfRangeException("channel");
            _channel = channel;
            _pin = GetPin(channel);
            if (!Port.ReservePin(_pin, true))
                throw new InvalidOperationException("Pin already reserved for another task");
            _signalInfo = new int[8];
            ClockDivisor = (UInt16)clockDivisor;
            NativeInitialize();
            }
        /// <summary>
        /// Dispose native resources
        /// </summary>
        public void Dispose() {
            Stop();
            NativeDispose();
            Port.ReservePin(_pin, false);
            _pin = Cpu.Pin.GPIO_NONE;
            _rawLevels = null;
            }
        /// <summary>
        /// Start the signal generation
        /// </summary>
        public void Start() {
            Start(0);
            }

        /// <summary>
        /// Start the signal generation with the specified repeat count
        /// <para>The signal pattern is specified by the <see cref="RawLevels"/> array.</para>
        /// </summary>
        ///<param name="repeatCount">The repeat count, 0 for never ending</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Start(int repeatCount);

        /// <summary>
        /// Start the signal generation  at specified time with specified repeat count
        /// <para>The signal pattern is specified by the <see cref="RawLevels"/> array.</para>
        /// </summary>
        /// <param name="startTime"></param>
        ///<param name="repeatCount">The repeat count, 0 for never ending</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Start(DateTime startTime, int repeatCount);

        /// <summary>
        /// Stop the signal generation
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Stop();

        /// <summary>
        /// Gets the string representation of this instance
        /// </summary>
        /// <returns></returns>
        public override string ToString() {
            return base.ToString() + "("+Status.ToString()+")";
            }

        }
    }
