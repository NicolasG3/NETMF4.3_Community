using System;
using System.Threading;
using GHI.OSHW.Hardware;
using Microsoft.SPOT.Hardware;

namespace GHI.GameO
{
	/// <summary>
	/// Provides access to sound on the Game-O.
	/// </summary>
    public static class Sound
    {
        private const Cpu.Pin AUDIO_POWER_CONTROL_PIN = (Cpu.Pin)(0 * 16 + 10); //FEZCerberus.Pin.PA10; 
		private const Cpu.AnalogOutputChannel AUDIO_OUTPUT_CHANNEL = Cpu.AnalogOutputChannel.ANALOG_OUTPUT_0;
		private const Cpu.PWMChannel PWM_CHANNEL = Cpu.PWMChannel.PWM_3;

		private static OutputPort AudioPowerControl = new OutputPort(Sound.AUDIO_POWER_CONTROL_PIN, false);
		private static PWM PWMOut;// = new PWM(Sound.PWM_CHANNEL, 261, 50, false);
        private static AnalogOutput AudioSwitch;

		/// <summary>
		/// Returns whether or not sound is enabled.
		/// </summary>
		public static bool IsEnabled { get { return Sound.Enabled; } }
		private static bool Enabled = false;

		/// <summary>
		/// Enables the sound functionality if it was disabled.
		/// </summary>
        public static void Enable()
        {
			if (Sound.IsEnabled)
				return;

			Sound.AudioPowerControl.Write(true);
			Sound.PWMOut = new PWM(Sound.PWM_CHANNEL, 261, 50, false);

			Sound.Enabled = true;
        }

		/// <summary>
		/// Disables the accelerometer to save power.
		/// </summary>
        public static void Disable()
		{
			if (!Sound.IsEnabled)
				return;

			Sound.AudioPowerControl.Write(false);
			Sound.PWMOut.Dispose();
			Sound.PWMOut = null;

			Sound.Enabled = false;
        }

		/// <summary>
		/// Plays an 8bit mono WAV file at 4,800 samples/second.
		/// </summary>
		/// <param name="data">The WAV file data array</param>
		/// <param name="offset">The offset into the array to begin playing</param>
		/// <param name="count">The amount of the array to play</param>
		public static void PlayWAVAudio(ref byte[] data, int offset, int count)
		{
			Util.PlayPCMAudio(Sound.AUDIO_OUTPUT_CHANNEL, data, offset, count, 4800);
		}

		/// <summary>
		/// Turns on the buzzer.
		/// </summary>
        public static void StartBuzzer()
        {
            if (!Sound.IsEnabled)
                throw new Exception("You must enable Sound first.");

            if (AudioSwitch == null)
            {
                AudioSwitch = new AnalogOutput(Cpu.AnalogOutputChannel.ANALOG_OUTPUT_0, 12);
                AudioSwitch.Write(0);
            }

            Sound.PWMOut.Start();
        }

		/// <summary>
		/// Plays a tone on the buzzer after you started it.
		/// </summary>
		/// <param name="frequency">The tone to play.</param>
		/// <param name="duration">How long to play the tone for. If 0, plays until StopBuzzer() is called.</param>
		public static void PlayTone(uint frequency, uint duration = 0)
		{
			if (!Sound.IsEnabled)
				throw new Exception("You must enable Sound first.");           
			
			Sound.PWMOut.Frequency = frequency;			
			Thread.Sleep((int)duration);
		}

		/// <summary>
		/// Turns off the buzzer.
		/// </summary>
		public static void StopBuzzer()
		{
			if (!Sound.IsEnabled)
				throw new Exception("You must enable Sound first.");

            Sound.PWMOut.Stop();

            if (AudioSwitch != null)
            {
                AudioSwitch.Dispose();
                AudioSwitch = null;
            }
			
		}
    }
}
