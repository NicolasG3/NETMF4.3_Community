NETMF 4.3 Community features and GCC compatibility
==================================================

This code is intended to be merged to NETMF 4.3 and GHI open source hardware code.

All features are implemented for STM32F4 and a custom solution for FEZCerberus is provided.
This solution is compiled with GCC V4.6 (GCC V4.7 is not compatible).

GHI open source code is modified to be compatible with NETMF 4.3 and FEZCerberus class is completed to declare available hardware ports and channels.

Community features are provided in specific assemblies:
Community.Hardware.QuadratureEncoder.dll
----------------------------------------
  Quadrature encoder driver with output compare and input capture.
  The HAL driver is provided to allow implementation for other platforms. 
  
Community.Hardware.SignalGenerator.dll
--------------------------------------
  Software binary signal generator.
  
Community.Hardware.Settings.dll
-------------------------------
  Access to debug port settings (COM or USB), hardware serial and persistent user settings (internal Flash).
  HAL driver is provided for hardware serial.
  
Community.System.dll
--------------------
  BitConverter class implementation.
  
  

Nicolas Gallerand
