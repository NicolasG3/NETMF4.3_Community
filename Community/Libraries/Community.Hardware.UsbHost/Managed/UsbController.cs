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
//using USBC = Microsoft.SPOT.Hardware.UsbClient;

namespace Community.Hardware.UsbHost
    {
    /// <summary>
    /// USB host controller (mass storage only)
    /// </summary>
    public class UsbController
        {
        private NativeEventDispatcher _dispatcher;
        /// <summary>
        /// Private constructor
        /// </summary>
        private UsbController() {

            }
        static private UsbController _defaultController = new UsbController();
        /// <summary>
        /// Get the default USB host controller
        /// </summary>
        static public UsbController DefaultController {
            get { return _defaultController; }
            set { _defaultController = value; }
            }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool NativeStart();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool NativeStop();
        /// <summary>
        /// Start this controller
        /// </summary>
        /// <returns></returns>
        public bool Start() {
            if (_dispatcher != null) {
                return true;
                }
            _dispatcher = new NativeEventDispatcher("Community_Hardware_UsbHost_Driver", 0);
            _dispatcher.OnInterrupt += Dispatcher_OnInterrupt;
            return NativeStart();
            }

        /// <summary>
        /// Internal interrupt handler
        /// </summary>
        /// <param name="data1"></param>
        /// <param name="data2"></param>
        /// <param name="time"></param>
        private void Dispatcher_OnInterrupt(uint data1, uint data2, DateTime time) {
            uint deviceClass = data1 & 0xFF;
            bool connected = (data1 & 0xFF00) != 0;
            }
        /// <summary>
        /// Stop this controller
        /// </summary>
        /// <returns></returns>
        public bool Stop() {
            if (_dispatcher == null)
                return true;
            _dispatcher.OnInterrupt -= Dispatcher_OnInterrupt;
            _dispatcher.Dispose();
            _dispatcher = null;
            return NativeStop();
            }
        }
    }
