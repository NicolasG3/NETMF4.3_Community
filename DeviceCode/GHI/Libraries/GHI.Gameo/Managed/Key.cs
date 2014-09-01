using Microsoft.SPOT.Hardware;
using Microsoft.SPOT;
using System;

namespace GHI.GameO
{
	/// <summary>
	/// Provides access to the keys on the Game-O.
	/// </summary>
    public static class Key
	{
		private const Cpu.Pin POWER_PIN = (Cpu.Pin)(0 * 16 + 0); //FEZCerberus.Pin.PA0; //BT1
		private const Cpu.Pin C_PIN = (Cpu.Pin)(2 * 16 + 1); //FEZCerberus.Pin.PC1;    // BT2
        private const Cpu.Pin B_PIN = (Cpu.Pin)(1 * 16 + 12); //FEZCerberus.Pin.PC2; // BT3
        private const Cpu.Pin A_PIN = (Cpu.Pin)(2 * 16 + 3); //FEZCerberus.Pin.PC3; // BT4
		private const Cpu.Pin START_PIN = (Cpu.Pin)(2 * 16 + 4); //FEZCerberus.Pin.PC4; // BT5
		private const Cpu.Pin UP_PIN = (Cpu.Pin)(2 * 16 + 5); //FEZCerberus.Pin.PC5; //BT6
		private const Cpu.Pin LEFT_PIN = (Cpu.Pin)(1 * 16 + 13); //FEZCerberus.Pin.PB13; // BT7
		private const Cpu.Pin DOWN_PIN = (Cpu.Pin)(1 * 16 + 14); //FEZCerberus.Pin.PB14; //BT8
		private const Cpu.Pin RIGHT_PIN = (Cpu.Pin)(1 * 16 + 15); //FEZCerberus.Pin.PB15; // BT9

        private static InterruptPort Up = new InterruptPort(Key.UP_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
		private static InterruptPort Left = new InterruptPort(Key.LEFT_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
		private static InterruptPort Down = new InterruptPort(Key.DOWN_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
		private static InterruptPort Right = new InterruptPort(Key.RIGHT_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);

		private static InterruptPort A = new InterruptPort(Key.A_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
		private static InterruptPort B = new InterruptPort(Key.B_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
		private static InterruptPort C = new InterruptPort(Key.C_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
		private static InterruptPort Power = new InterruptPort(Key.POWER_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
		private static InterruptPort Start = new InterruptPort(Key.START_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);

		/// <summary>
		/// The keys on the Game-O.
		/// </summary>
		public enum Keys
		{
			Left,
			Right,
			Up,
			Down,
			Power,
			C,
			B,
			A,
			Start,
		}

		/// <summary>
		/// Returns whether or not the Accelerometer is enabled.
		/// </summary>
		public static bool IsEnabled { get { return Key.Enabled; } }
		private static bool Enabled = true;

		/// <summary>
		/// Used to handle the KeyPressed event.
		/// </summary>
		/// <param name="key">The key that was pressed.</param>
		public delegate void KeyEventHandler(Keys key);

		/// <summary>
		/// The event that is raised when a key is pressed.
		/// </summary>
		public static event KeyEventHandler KeyPressed;

		/// <summary>
		/// Enables the key functionality if it was disabled.
		/// </summary>
        public static void Enable()
		{
			if (Key.IsEnabled)
				return;

			Key.Up = new InterruptPort(Key.UP_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
			Key.Left = new InterruptPort(Key.LEFT_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
			Key.Down = new InterruptPort(Key.DOWN_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
			Key.Right = new InterruptPort(Key.RIGHT_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);

			Key.A = new InterruptPort(Key.A_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
			Key.B = new InterruptPort(Key.B_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
			Key.C = new InterruptPort(Key.C_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
			Key.Power = new InterruptPort(Key.POWER_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);
			Key.Start = new InterruptPort(Key.START_PIN, false, Port.ResistorMode.PullDown, Port.InterruptMode.InterruptEdgeLow);

			Key.Up.OnInterrupt += new NativeEventHandler(OnKeyPress);
			Key.Left.OnInterrupt += new NativeEventHandler(OnKeyPress);
			Key.Down.OnInterrupt += new NativeEventHandler(OnKeyPress);
			Key.Right.OnInterrupt += new NativeEventHandler(OnKeyPress);

			Key.A.OnInterrupt += new NativeEventHandler(OnKeyPress);
			Key.B.OnInterrupt += new NativeEventHandler(OnKeyPress);
			Key.C.OnInterrupt += new NativeEventHandler(OnKeyPress);
			Key.Power.OnInterrupt += new NativeEventHandler(OnKeyPress);
			Key.Start.OnInterrupt += new NativeEventHandler(OnKeyPress);

			Key.Enabled = true;
        }

		/// <summary>
		/// Disables the key functionality.
		/// </summary>
        public static void Disable()
		{
			if (!Key.IsEnabled)
				return;

			Key.Left.Dispose();
			Key.Left = null;
			Key.Right.Dispose();
			Key.Right = null;
			Key.Up.Dispose();
			Key.Up = null;
			Key.Down.Dispose();
			Key.Down = null;
			Key.A.Dispose();
			Key.A = null;
			Key.B.Dispose();
			Key.B = null;
			Key.C.Dispose();
			Key.C = null;
			Key.Power.Dispose();
			Key.Power = null;
			Key.Start.Dispose();
			Key.Start = null;

			Key.Enabled = false;
        }

		/// <summary>
		/// Gets the state of a key.
		/// </summary>
		/// <param name="key">The key to test.</param>
		/// <returns>Whether or not the key is pressed.</returns>
		public static bool IsKeyPressed(Keys key)
		{
			if (!Key.IsEnabled)
				throw new Exception("You must enabled Keys first.");

			switch (key)
			{
				case Keys.Left: return Key.Left.Read();
				case Keys.Right: return Key.Right.Read();
				case Keys.Up: return Key.Up.Read();
				case Keys.Down: return Key.Down.Read();
				case Keys.A: return Key.A.Read();
				case Keys.B: return Key.B.Read();
				case Keys.C: return Key.C.Read();
				case Keys.Power: return Key.Power.Read();
				case Keys.Start: return Key.Start.Read();
			}

			return false;
		}

		private static void OnKeyPress(uint port, uint state, DateTime time)
		{
			Debug.Print(port.ToString());
			
			switch ((Cpu.Pin)port)
			{
				case Key.LEFT_PIN: Key.KeyPressed(Keys.Left); break;
				case Key.RIGHT_PIN: Key.KeyPressed(Keys.Right); break;
				case Key.UP_PIN: Key.KeyPressed(Keys.Up); break;
				case Key.DOWN_PIN: Key.KeyPressed(Keys.Down); break;
				case Key.A_PIN: Key.KeyPressed(Keys.A); break;
				case Key.B_PIN: Key.KeyPressed(Keys.B); break;
				case Key.C_PIN: Key.KeyPressed(Keys.C); break;
				case Key.START_PIN: Key.KeyPressed(Keys.Start); break;
				case Key.POWER_PIN: Key.KeyPressed(Keys.Power); break;
			}
		}
    }
}
