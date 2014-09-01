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
    /// Event handler for quadrature encoder input or output pin change
    /// </summary>
    /// <param name="IOIndex">Compare/Capture index</param>
    /// <param name="count">Count</param>
    /// <param name="time">Time</param>
    /// <param name="overlap">Indicates whether one or more input was too close to be captured, or wether the previous compare pulse was still active</param>
    public delegate void QuadratureEncoderEventHandler(int IOIndex, int count, DateTime time, bool overlap);


    /// <summary>
    /// Hardware counter for quadrature encoder
    /// </summary>
    public class QuadratureEncoder : IDisposable
        {
        /// <summary>
        /// Quadrature count mode
        /// </summary>
        public enum CountMode
            {
            /// <summary>
            /// Undefined count mode
            /// </summary>
            None,
            /// <summary>
            /// X1 mode
            /// </summary>
            X1,
            /// <summary>
            /// X2 mode
            /// </summary>
            X2,
            /// <summary>
            /// X4 mode
            /// </summary>
            X4
            }
        /// <summary>
        /// IO capture/compare reservation status
        /// </summary>
        public enum IOStatus
            {
            /// <summary>
            /// None
            /// </summary>
            None,
            /// <summary>
            /// Input capture
            /// </summary>
            InputCapture,
            /// <summary>
            /// Output compare
            /// </summary>
            OutputCompare
            }
        private NativeEventDispatcher _dispatcher;

        private readonly QuadratureEncoderChannel _channel;
        /// <summary>
        /// Get the QuadratureEncoder channel
        /// </summary>
        public QuadratureEncoderChannel Channel {
            get { return _channel; }
            }

        private readonly Cpu.Pin _pin_A;
        /// <summary>
        /// Get the input pin for quadrature channel A
        /// </summary>
        public Cpu.Pin Pin_A {
            get { return _pin_A; }
            }

        private readonly Cpu.Pin _pin_B;

        /// <summary>
        /// Get the input pin for quadrature channel B
        /// </summary>
        public Cpu.Pin Pin_B {
            get { return _pin_B; }
            }

        private readonly CountMode _mode;
        /// <summary>
        /// Get the count mode
        /// </summary>
        public CountMode Mode {
            get { return _mode; }
            }

        /// <summary>
        /// Get the number of pins that can be used for compare/capture IO by this QuadratureEncoder
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetIOPinCount();

        /// <summary>
        /// Get the pin used for for the specified compare/capture IO.
        /// </summary>
        /// <param name="IOIndex">Compare/Capture IO index</param>
        /// <returns>The pin</returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Cpu.Pin GetIOPin(int IOIndex);

        /// <summary>
        /// Get the specified IO capture/compare reservation status
        /// </summary>
        /// <param name="IOIndex">Compare/Capture IO index</param>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern IOStatus GetIOStatus(int IOIndex);
        /// <summary>
        /// Initialize the specified output pin to generate a pulse when <see cref="Count"/> value equals the compare value
        /// </summary>
        /// <param name="IOIndex">Compare/Capture index</param>
        /// <param name="value">Compare value</param>
        /// <param name="invert">Indicates to generate a negative pulse</param>
        /// <param name="pulseLength">Output pulse length in µs (0 for infinite pulse)</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void InitOutputCompare(int IOIndex, int value, bool invert = false, int pulseLength = 100);

        /// <summary>
        /// Initialize the counter capture on the specified pin rising edgde
        /// </summary>
        /// <param name="IOIndex">Compare/Capture IO index</param>
        /// <param name="invert">Indicates to capture on falling edge</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void InitInputCapture(int IOIndex, bool invert = false);

        /// <summary>
        /// Release the specified IO pin reserve by <see cref="InitOutputCompare"/> or <see cref="InitInputCapture"/>
        /// </summary>
        /// <param name="IOIndex">Compare/Capture index</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ReleaseIO(int IOIndex);

        /// <summary>
        /// Get the channel count for the hardware.
        /// </summary>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static public extern int GetChannelCount();
        /// <summary>
        /// Get the input pins for the specified channel
        /// </summary>
        /// <param name="channel">Channel</param>
        /// <param name="A">A input pin</param>
        /// <param name="B">B input pin</param>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static public extern void GetQuadratureEncoderPinsForChannel(QuadratureEncoderChannel channel, out Cpu.Pin A, out Cpu.Pin B);

        /// <summary>
        /// Initialize hardware. Channel and Mode must be set before call.
        /// </summary>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void Initialize();

        /// <summary>
        /// Release hardware
        /// </summary>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void Uninitialize();

        /// <summary>
        /// Initialize a new <see cref="QuadratureEncoder"/>
        /// </summary>
        public QuadratureEncoder(QuadratureEncoderChannel channel, CountMode mode) {
            //Do not use HardwareProvider to keep the feature independant :
            //no HardwareProvider.HwProvider.GetQuadratureEncoderChannelCount(); 
            //no HardwareProvider.HwProvider.GetQuadratureEncoderPinsForChannel(...); 
            //-> if a solution doesn't need this feature, no stub is required
            _channel = channel;
            _mode = mode;
            GetQuadratureEncoderPinsForChannel(channel, out _pin_A, out _pin_B);
            Initialize();
            _dispatcher = new NativeEventDispatcher("Community_QuadratureEncoder_Driver", 0);
            _dispatcher.OnInterrupt += Dispatcher_OnInterrupt;
            }

        /// <summary>
        /// Dispose native resources
        /// </summary>
        public void Dispose() {
            if (_dispatcher == null)
                return;
            _dispatcher.OnInterrupt -= Dispatcher_OnInterrupt;
            _dispatcher.Dispose();
            _dispatcher = null;
            Uninitialize();
            }

        /// <summary>
        /// Get or set the counter value
        /// </summary>
        public extern int Count {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
            }

        /// <summary>
        /// Gets or sets a value indicating whether the counter is updated according to the encoder inputs
        /// <para>When not enabled, the counter retains its value regardless of the encoder inputs</para>
        /// <para>Default value is <c>true</c></para>
        /// </summary>
        public extern bool CountEnabled {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
            }

        /// <summary>
        /// Relay native interrupt to strong typed event handler
        /// </summary>
        /// <param name="data1"></param>
        /// <param name="data2"></param>
        /// <param name="time"></param>
        private void Dispatcher_OnInterrupt(uint data1, uint data2, DateTime time) {
            if (IOChanged != null) {
                int IOIndex = (int)(data1 & 0xF);
                int count = (int)data2;
                bool overlap = (data1 & 0x10) == 0x10;
                IOChanged(IOIndex, count, time, overlap);
                }
            }
        /// <summary>
        /// Event raised by an input capture or an output compare 
        /// </summary>
        public event QuadratureEncoderEventHandler IOChanged;
        }
    }
