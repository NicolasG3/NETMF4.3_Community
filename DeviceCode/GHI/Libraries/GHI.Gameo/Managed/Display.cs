using System;
using System.Runtime.CompilerServices;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using Microsoft.SPOT.Presentation.Media;

namespace GHI.GameO
{
	/// <summary>
	/// Provides access to the display on the Game-O.
	/// </summary>
    public static class Display
	{
		private const ushort SCREEN_WIDTH = 240;
		private const ushort SCREEN_HEIGHT = 320;

        private static OutputPort ResetPin = new OutputPort((Cpu.Pin)(2 * 16 + 13), false); // PC13 
        private static OutputPort BacklightPin = new OutputPort((Cpu.Pin)(0 * 16 + 7), true); //PA7        

		private static Orientation CurrentOrientation;
		private static ushort Width;
		private static ushort Height;

		/// <summary>
		/// The current orientation of the display.
		/// </summary>
		/// <returns>The current orientation.</returns>
		public static Orientation GetCurrentOreintation()
		{
			return Display.CurrentOrientation;
		}

		/// <summary>
		/// The width of the display.
		/// </summary>
		/// <returns>The current width.</returns>
		public static ushort GetWidth()
		{
			return Display.Width;
		}

		/// <summary>
		/// The height of the display.
		/// </summary>
		/// <returns>The current height.</returns>
		public static ushort GetHeight()
		{
			return Display.Height;
		}

		/// <summary>
		/// The possible orientations of the display.
		/// </summary>
		public enum Orientation : byte
		{
			Portrait = 0,
			Landscape
		}

		/// <summary>
		/// Initializes the display so it is ready for use.
		/// </summary>
		public static void Initialize()
		{
			/// hard reset the display
			Display.ResetPin.Write(true);
			Thread.Sleep(20);
			Display.ResetPin.Write(false);
			Thread.Sleep(50);
			Display.ResetPin.Write(true);
			Thread.Sleep(500);

			SetOrientation(Orientation.Landscape);
			Thread.Sleep(200);
			SetDefaultConfiguration();
		}

		/// <summary>
		/// Changes the orientation of the device.
		/// </summary>
		/// <param name="orientation">The orietnation to change to.</param>
		/// <returns>Whether or not it was changed.</returns>
		public static bool SetOrientation(Orientation orientation)
		{
			Display.CurrentOrientation = orientation;

			if (orientation == Display.Orientation.Landscape)
			{
				Display.Width = Display.SCREEN_HEIGHT;
				Display.Height = Display.SCREEN_WIDTH;
			}
			else
			{
				Display.Width = Display.SCREEN_WIDTH;
				Display.Height = Display.SCREEN_HEIGHT;
			}

			return Initialize(orientation, Display.Width, Display.Height);
		}

		private static void SetDefaultConfiguration()
		{
			Display.WriteRegister(0x0000, 0x0001); Thread.Sleep(50);
			Display.WriteRegister(0x002b, 0x0003); Thread.Sleep(50);
			Display.WriteRegister(0x0007, 0x0000); Thread.Sleep(50);
			Display.WriteRegister(0x0012, 0x0000); Thread.Sleep(50);
			Display.WriteRegister(0x0060, 0xa700); Thread.Sleep(2);
			Display.WriteRegister(0x0008, 0x0405); Thread.Sleep(2);
			Display.WriteRegister(0x0030, 0x0002); Thread.Sleep(2);
			Display.WriteRegister(0x0031, 0x0307); Thread.Sleep(2);
			Display.WriteRegister(0x0032, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0035, 0x0606); Thread.Sleep(2);
			Display.WriteRegister(0x0036, 0x080e); Thread.Sleep(2);
			Display.WriteRegister(0x0037, 0x0204); Thread.Sleep(2);
			Display.WriteRegister(0x0038, 0x0203); Thread.Sleep(2);
			Display.WriteRegister(0x0039, 0x0707); Thread.Sleep(2);
			Display.WriteRegister(0x003c, 0x0400); Thread.Sleep(2);
			Display.WriteRegister(0x003d, 0x0005); Thread.Sleep(2);
			Display.WriteRegister(0x0010, 0x02c0); Thread.Sleep(2);
			Display.WriteRegister(0x0011, 0x0127); Thread.Sleep(30);
			Display.WriteRegister(0x0012, 0x011c); Thread.Sleep(30);
			Display.WriteRegister(0x0013, 0x1600); Thread.Sleep(2);
			Display.WriteRegister(0x0029, 0x0010); Thread.Sleep(30);
			Display.WriteRegister(0x0001, 0x0100); Thread.Sleep(2);
			Display.WriteRegister(0x0002, 0x0700); Thread.Sleep(2);
			Display.WriteRegister(0x0003, 0x0030); Thread.Sleep(2);
			Display.WriteRegister(0x0061, 0x0001); Thread.Sleep(2);
			Display.WriteRegister(0x0090, 0x001c); Thread.Sleep(2);
			Display.WriteRegister(0x0092, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0080, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0081, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0082, 0x013f); Thread.Sleep(2);
			Display.WriteRegister(0x0083, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0084, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0085, 0x013f); Thread.Sleep(2);
			Display.WriteRegister(0x0050, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0051, 0x00ef); Thread.Sleep(2);
			Display.WriteRegister(0x0052, 0x0000); Thread.Sleep(2);
			Display.WriteRegister(0x0053, 0x013f); Thread.Sleep(2);
			Display.WriteRegister(0x0091, 0x0300); Thread.Sleep(2);
			Display.WriteRegister(0x0098, 0x0303); Thread.Sleep(2);
			Display.WriteRegister(0x0010, 0x12C0); Thread.Sleep(20);
			Display.WriteRegister(0x0007, 0x0133); Thread.Sleep(20);
		}

		/// <summary>
		/// Turns the display backlight on.
		/// </summary>
        public static void TurnBackLightOn()
        {
			Display.BacklightPin.Write(true);
        }

		/// <summary>
		/// Turns the backlight off.
		/// </summary>
        public static void TurnBackLightOff()
        {
			Display.BacklightPin.Write(false);
        }

		/// <summary>
		/// Draws a string to the display.
		/// </summary>
		/// <param name="text">The text to write.</param>
		/// <param name="x">The x coordinate where to draw.</param>
		/// <param name="y">The y coordinate where to draw.</param>
		/// <param name="font">The font to use.</param>
		/// <param name="foreColor">The color of the text.</param>
		/// <param name="backColor">The background color of the text.</param>
		public static void Draw(string text, int x, int y, Font font, Color foreColor, Color backColor)
        {
            int width = 0;
            for (int i = 0; i < text.Length; i++)
            {
                width += font.CharWidth((char)(text[i]));
            }

            Bitmap bmp = new Bitmap(width, font.Height);
			bmp.DrawRectangle(backColor, 100, 0, 0, width, font.Height, 0, 0, backColor, 0, 0, backColor, 0, 0, 0);
			bmp.DrawText(text, font, foreColor, 0, 0);

			Display.Draw(new TinyBitmap(bmp), x, y);
        }

		/// <summary>
		/// Draws a TinyBitmap to the display.
		/// </summary>
		/// <param name="tb">The TinyBitmap to display.</param>
		/// <param name="xDst">The x coordinate where to draw.</param>
		/// <param name="yDst">The y coordinate where to draw.</param>
        public static void Draw(TinyBitmap tb, int xDst, int yDst)
        {
			Display.Draw(tb, xDst, yDst, 0, 0, tb.Width, tb.Height);
        }

		/// <summary>
		/// Draws a TinyBitmap to the display.
		/// </summary>
		/// <param name="tb">The TinyBitmap to display.</param>
		/// <param name="xDst">The x coordinate where to draw.</param>
		/// <param name="yDst">The y coordinate where to draw.</param>
		/// <param name="xSrc">The x coordinate of the portion of the bitmap you wish to start drawing from.</param>
		/// <param name="ySrc">The y coordinate of the portion of the bitmap you wish to start drawing from.</param>
		/// <param name="width">The width of the TinyBitmap you wish to draw.</param>
		/// <param name="height">The height of the TinyBitmap you wish to draw.</param>
		public static void Draw(TinyBitmap tb, int xDst, int yDst, int xSrc, int ySrc, uint width, uint height)
        {
			Display.DrawImage(tb.Data, (ushort)xDst, (ushort)yDst, (ushort)tb.Width, (ushort)tb.Height, (ushort)xSrc, (ushort)ySrc, (ushort)width, (ushort)height);
        }

		/// <summary>
		/// Draws a rectangle on the screen.
		/// </summary>
		/// <param name="x">The x coordinate where to draw.</param>
		/// <param name="y">The y coordinate where to draw.</param>
		/// <param name="width">The width of the rectangle.</param>
		/// <param name="height">The height of the rectangle.</param>
		/// <param name="color">The color of the rectangle.</param>
        public static void FillRect(int x, int y, int width, int height, ushort color)
        {
            Display.FillRect((ushort)x, (ushort)y, (ushort)width, (ushort)height, color);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static Boolean Initialize(Orientation screenmode, ushort screenwidt, ushort screenheight);     

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern private static Boolean WriteRaw(byte[] data, int offset, int count);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern private static Boolean WriteRaw(ushort[] data, int offset, int count);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern private static Boolean WriteRegister(ushort register, ushort value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static Boolean WriteRegister(ushort register);
      
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static Boolean FillRect(ushort x, ushort y, ushort width, ushort height, ushort color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static Boolean DrawImage(byte[] image, ushort xDst, ushort yDst, ushort originalWidth, ushort originalHeight, ushort xSrc, ushort ySrc, ushort width, ushort height);
    }
}