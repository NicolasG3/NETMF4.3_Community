##NETMF 4.3 Community features and GCC compatibility
This project allows the compilation of any STM32F4 solution with GCC 4.6.  
Porting NETMF 4.3 ont STM32F4 is thus totally free and open source.

GCC compatibility was achieved to allow adding interop features in native code.  
Those interop features are released here to the community.

###Usage
***
This code is intended to be merged to [NETMF 4.3 porting kit](http://netmf.codeplex.com/releases/view/81000) and [GHI's Open Source NETMF Ports](http://ghiopensource.codeplex.com/).

All features are implemented for STM32F4 and a custom solution for *FEZCerberus* is provided.
This solution is compiled with GCC V4.6 (GCC V4.7 is not compatible).

###Content
***
Part of GHI open source code is modified to be compatible with NETMF 4.3 and the **GHI.OSHW.FEZCerberus** class is completed to declare available hardware ports and channels.

Community features are provided in specific assemblies:
####Community.Hardware.QuadratureEncoder.dll
Quadrature encoder hardware driver with output compare and input capture capability.
The HAL driver is provided to allow implementation for other platforms. 
  
####Community.Hardware.SignalGenerator.dll
Software binary signal generator.  Implemented at HAL level, this feature is platform independant and can be added to any solution.
  
####Community.Hardware.Settings.dll
Access to debug port settings (COM or USB), hardware serial and persistent user settings (internal Flash).
HAL driver is provided for porting hardware serial to other platforms. Others features are platform independant.
  
####Community.System.dll
BitConverter class implementation.
  
###Ready to use binary
***
The compiled firmware and the managed assemblies are availables in 'Ready to use' directory.
  
  
  
-------------  

**Nicolas Gallerand**
