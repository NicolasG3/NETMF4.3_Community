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

namespace Community.Hardware
    {
    /// <summary>
    /// Extended system settings
    /// </summary>
    public class SystemSettings
        {
        /// <summary>
        /// Hardware communication ports
        /// </summary>
        public enum Port
            {
            /// <summary>
            /// None
            /// </summary>
            None = 0,
            /// <summary>
            /// Serial port 1
            /// </summary>
            COM1 = 257,
            /// <summary>
            /// Serial port 2
            /// </summary>
            COM2 = 258,
            /// <summary>
            /// Serial port 3
            /// </summary>
            COM3 = 259,
            /// <summary>
            /// Serial port 4
            /// </summary>
            COM4 = 260,
            /// <summary>
            /// Serial port 5
            /// </summary>
            COM5 = 261,
            /// <summary>
            /// Serial port 6
            /// </summary>
            COM6 = 262,
            /// <summary>
            /// USB port 1
            /// </summary>
            USB1 = 513,
            /// <summary>
            /// USB port 2
            /// </summary>
            USB2 = 514,
            /// <summary>
            /// Ethernet port 1
            /// </summary>
            Sockets1 = 769,
            /// <summary>
            /// Ethernet port 2
            /// </summary>
            Sockets2 = 770
            }

        /// <summary>
        /// Gets the current debug Interface
        /// </summary>
        public static extern Port DebugInterface {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
            }

        /// <summary>
        /// Set the debug interface to the specified port and restart.
        /// <para>No operation if the specified port is already used as debug interface.</para>
        /// <para>Note that the debug interface must be the same in your Visual Studio project properties</para>
        /// </summary>
        /// <param name="port">The new debug interface port</param>
        /// <example>
        /// //Use COM3 debug interface when then PA9 pin is tied to ground
        /// InputPort debugPort = new InputPort(FEZCerberus.Pin.PA9, false, Port.ResistorMode.PullUp);
        /// if (debugPort.Read()) {
        ///     //Restore the default debug interface if the pin is left unconnected
        ///     SystemSettings.SetDebugInterface(Settings.Port.USB1);
        ///     }
        /// else
        ///     SystemSettings.SetDebugInterface(SystemConfig.Port.COM3);
        /// //Configure USB client for your own use and debug on COM3
        /// //...
        /// </example>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetDebugInterface(Port port);

        private static byte[] _hardwareSerial;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void NativeGetHardwareSerial(byte[] data);

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //private static extern void GetHardwareSerial(out ulong high, out ulong low);


        /// <summary>
        /// Gets the hardware unique serial ID (16 bytes)
        /// <para>This array can be use to vreate a <see cref="Guid"/></para>
        /// </summary>
        public static byte[] HardwareSerial {
            get {
                if (_hardwareSerial == null) {
                    _hardwareSerial = new byte[16];
                    NativeGetHardwareSerial(_hardwareSerial);
                    }
                return _hardwareSerial;
                }
            }
        /// <summary>
        /// Create or update a user configuration block in flash memory 
        /// <para>The data buffer size must be constant for the specified name</para>
        /// </summary>
        /// <param name="name">Unique config ID (63 characters max)</param>
        /// <param name="data">Data buffer</param>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool WriteConfigBlock(String name, byte[] data);
        /// <summary>
        /// Read a user configuration block in flash memory
        /// <para>The data buffer size must be constant for the specified name</para>
        /// </summary>
        /// <param name="name">Unique config ID (63 characters max)</param>
        /// <param name="data">Data buffer to update</param>
        /// <returns></returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool ReadConfigBlock(String name, byte[] data);
        }
    }
