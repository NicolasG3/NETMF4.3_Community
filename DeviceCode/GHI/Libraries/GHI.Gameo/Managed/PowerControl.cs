using System.Threading;
using Microsoft.SPOT.Hardware;

namespace GHI.GameO
{
	/// <summary>
	/// Provides access to the power control of the Game-O.
	/// </summary>
    public static class PowerControl
    {
		/// <summary>
		/// Turns off the device.
		/// </summary>
        public static void PowerOff()
        {
			Display.TurnBackLightOff();
			Sound.Disable();
			Accelerometer.Disable();

            while (Key.IsKeyPressed(Key.Keys.Power))
				;

            Thread.Sleep(300); //If any low to high signal is found on the wakeup pin, Game-O will wake up so we need to delay for 200-300ms.

            PowerState.Sleep(SleepLevel.Off, HardwareEvent.OEMReserved1);
        }
    }
}
