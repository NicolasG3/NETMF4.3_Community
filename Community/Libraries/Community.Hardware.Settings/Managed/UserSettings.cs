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
using Microsoft.SPOT;

namespace Community.Hardware
    {
    /// <summary>
    /// User persistent data.
    /// <para>This class is a container for persistent data stored in flash memory (NETMF config block).</para>
    /// </summary>
    /// <example>
    /// //Get user persistent settings
    /// byte byteValue = UserSettings.Default.Data[0];
    /// byte strLen = UserSettings.Default.Data[1];
    /// String text = null;
    /// if (strLen > 0) 
    ///     text = new String(Encoding.UTF8.GetChars(UserSettings.Default.Data, 2, strLen));
    /// //update values and save
    /// byteValue++;
    /// UserSettings.Default.Data[0] = byteValue;
    /// Byte[] bytes = Encoding.UTF8.GetBytes("Persistent text");
    /// strLen = (byte)bytes.Length;    //assume the string is not larger than the UserSettings buffer
    /// UserSettings.Default.Data[1] = strLen;
    /// bytes.CopyTo(UserSettings.Default.Data, 2);
    /// UserSettings.Default.Save();
    /// </example>
    public class UserSettings
        {
        static private UserSettings _default;
        /// <summary>
        /// Get the default <see cref="UserSettings"/>
        /// </summary>
        static public UserSettings Default {
            get {
                if (_default == null)
                    _default = new UserSettings("USER_DEFAULT", 256);
                return _default;
                }
            }
        private String _name;
        /// <summary>
        /// Gets the configuration unique identifier
        /// </summary>
        public String Name {
            get { return _name; }
            }

        private byte[] _data;
        /// <summary>
        /// Gets the configuration data
        /// </summary>
        public byte[] Data {
            get { return _data; }
            }
        /// <summary>
        /// Initialize a new instance of <see cref="UserSettings"/>
        /// <para>Note to implementers: do not change Size when a config block is already existing in flash. Erase the flash and restore the firmware to delete existing config blocks.</para>
        /// </summary>
        /// <param name="name">Unique name for config block (63 characters max)</param>
        /// <param name="size">Config block size (bytes)</param>
        protected UserSettings(String name, int size) {
            _name = name;
            _data = new byte[size];
            SystemSettings.ReadConfigBlock(_name, _data);
            }
        /// <summary>
        /// Gets the config Name
        /// </summary>
        /// <returns></returns>
        public override string ToString() {
            return "UserSettings[" + _name + "]";
            }
        /// <summary>
        /// Save the configuration data to flash memory
        /// </summary>
        /// <returns></returns>
        public bool Save() {
            return SystemSettings.WriteConfigBlock(_name, _data);
            }
        /// <summary>
        /// Load the configuration data from flash memory
        /// </summary>
        /// <returns></returns>
        public bool Load() {
            return SystemSettings.ReadConfigBlock(_name, _data);
            }

        }
    }
