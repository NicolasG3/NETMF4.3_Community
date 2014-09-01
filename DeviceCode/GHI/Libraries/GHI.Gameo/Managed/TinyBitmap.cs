using GHI.OSHW.Hardware;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace GHI.GameO
{
	/// <summary>
	/// A class that stores an image in the format the device expects.
	/// </summary>
	public class TinyBitmap
	{
		/// <summary>
		/// The raw byte data to be sent to the device.
		/// </summary>
		public byte[] Data;

		/// <summary>
		/// The width of the image.
		/// </summary>
		public uint Width;

		/// <summary>
		/// The height of the image.
		/// </summary>
		public uint Height;

		/// <summary>
		/// Constructs a TinyBitmap from a preexisting bitmap.
		/// </summary>
		/// <param name="bitmap">The bitmap to construct from.</param>
		public TinyBitmap(Bitmap bitmap)
		{
			this.Width = (uint)bitmap.Width;
			this.Height = (uint)bitmap.Height;
			this.Data = new byte[this.Width * this.Height * 2];
			Util.BitmapConvertBPP(bitmap.GetBitmap(), this.Data, Util.BPP_Type.BPP16_BGR_BE);
		}

		/// <summary>
		/// Constructs a TinyBitmap from the raw pre-decoded bytes.
		/// </summary>
		/// <param name="data">The bitmap data.</param>
		/// <param name="width">The width of the bitmap.</param>
		/// <param name="height">The height of the bitmap.</param>
		public TinyBitmap(byte[] data, uint width, uint height)
		{
			this.Width = width;
			this.Height = height;
			this.Data = data;
		}

		/// <summary>
		/// Constructs a TinyBitmap from a byte array with length and width ints at the beginning.
		/// </summary>
		/// <param name="data">The bitmap data. The first 4 bytes represent the width, the next four bytes represent the height, and the remainder represent the bitmap.</param>
		public TinyBitmap(byte[] data) : this(Utility.ExtractRangeFromArray(data, 8, data.Length - 8), Utility.ExtractValueFromArray(data, 0, 4), Utility.ExtractValueFromArray(data, 4, 4))
		{

		}
	}
}
