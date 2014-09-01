using System;
using System.IO;
using System.Runtime.CompilerServices;
using Microsoft.SPOT;

namespace Community.Graphics
    {
    /// <summary>
    /// Jpeg stream reader
    /// </summary>
    public class JpegReader : IDisposable
        {
        private object _jpegObj;
        private object _fileStream;

        private int _width = 0;
        /// <summary>
        /// Get the image width
        /// </summary>
        public int Width {
            get { return _width; }
            }
        private int _height = 0;
        /// <summary>
        /// Get the image height
        /// </summary>
        public int Height {
            get { return _height; }
            }
        private byte[] _line = null;
        /// <summary>
        /// Get the line buffer
        /// </summary>
        public byte[] Line {
            get { return _line; }
            }
        private int _lineIndex = -1;
        /// <summary>
        /// Get the current line number
        /// </summary>
        public int LineIndex {
            get { return _lineIndex; }
            }
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private void Initialize(String path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private void NativeDispose();

        /// <summary>
        /// Initialize a new instance of <see cref="JpegReader"/> for the specified file
        /// </summary>
        /// <param name="path"></param>
        public JpegReader(String path) {
            if (path == null)
                throw new ArgumentNullException();
            Initialize(path);
            _line = new byte[_width * 3];
            }

        /// <summary>
        /// Release resources
        /// </summary>
        public void Dispose() {
            NativeDispose();
            _width = 0;
            _height = 0;
            _line = null;
            _lineIndex = -1;
            }
        /// <summary>
        /// Read the next line in the line buffer.
        /// <para>Returns <c>false</c> if there is no more line to read</para>
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public bool ReadNextLine();

        }
    }
