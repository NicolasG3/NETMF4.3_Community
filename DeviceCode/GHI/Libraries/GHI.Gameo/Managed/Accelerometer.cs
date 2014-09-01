using System;
using GHI.OSHW.Hardware;
using Microsoft.SPOT.Hardware;

namespace GHI.GameO
{
	/// <summary>
	/// Provides access to the accelerometer on the Game-O.
	/// </summary>
    public static class Accelerometer
    {
        private static SoftwareI2CBus I2CBus;
        private static SoftwareI2CBus.I2CDevice I2CDevice;

		private const Cpu.Pin I2C_CLK = (Cpu.Pin)(1 * 16 + 6); //clk: PB6
		private const Cpu.Pin I2C_DATA = (Cpu.Pin)(1 * 16 + 7);  //data: PB7
		private const int I2C_ADDRESS = 0x1C;  //data: PB7

		/// <summary>
		/// Returns whether or not the accelerometer is enabled.
		/// </summary>
		public static bool IsEnabled { get { return Accelerometer.Enabled; } }
		private static bool Enabled = false;

		/// <summary>
		/// Enables the accelerometer functionality if it was disabled.
		/// </summary>
        public static void Enable()
		{
			if (Accelerometer.IsEnabled)
				return;

			Accelerometer.I2CBus = new SoftwareI2CBus(Accelerometer.I2C_CLK, Accelerometer.I2C_DATA);
			Accelerometer.I2CDevice = Accelerometer.I2CBus.CreateI2CDevice(Accelerometer.I2C_ADDRESS, 400);
           
            //bit 0 is 1 - active
            //bit 0 is 0 - standby
            //bit 1 = 1: 8 bit accurate - fast mode read
            //bit 1 = 0: 10 bit accurate - normal mode read
            
            WriteToRegister(0x2A, 1);

			Accelerometer.Enabled = true;
        }

		/// <summary>
		/// Disables the accelerometer to save power.
		/// </summary>
        public static void Disable()
		{
			if (!Accelerometer.IsEnabled)
				return;

            Accelerometer.WriteToRegister(0x2A, 0);

			Accelerometer.I2CDevice = null;
			Accelerometer.I2CBus.Dispose();
			Accelerometer.I2CBus = null;

			Accelerometer.Enabled = false;
        }

		/// <summary>
		/// Gets the x, y, and z reading from the accelerometer.
		/// </summary>
		/// <param name="x">The x value.</param>
		/// <param name="y">The y value.</param>
		/// <param name="z">The z value.</param>
        public static void GetXYZ(out int x, out int y, out int z)
        {
			if (!Accelerometer.IsEnabled)
				throw new Exception("You must enable the acceleromter first.");

            byte[] data = Accelerometer.ReadFromRegister(0x1, 6);

			x = data[0] << 2 | data[1] >> 6 & 0x3F;
			y = data[2] << 2 | data[3] >> 6 & 0x3F;
			z = data[4] << 2 | data[5] >> 6 & 0x3F;

            if (x > 511) 
				x -= 1024;
            if (y > 511) 
				y -= 1024;
            if (z > 511) 
				z -= 1024;           
        }

		private static void WriteToRegister(byte register, byte value)
		{

			byte[] toSend = new byte[2] { register, value };
			Accelerometer.I2CDevice.Write(toSend, 0, 2);
		}

		private static byte[] ReadFromRegister(byte register, int readCount)
		{
			byte[] toSend = new byte[1] { register };
			byte[] toRead = new byte[readCount];
			int written = 0;
			int read = 0;

			Accelerometer.I2CDevice.WriteRead(toSend, 0, 1, toRead, 0, readCount, out written, out read);

			return toRead;
		}
    }
}