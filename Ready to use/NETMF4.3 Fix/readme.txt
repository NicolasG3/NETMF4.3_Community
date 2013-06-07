A bug in NETMF 4.3 code must be corrected to compile interop project with name containing dot characters like "Community.Hardware".

To fix it, replace 
C:\Program Files (x86)\Microsoft .NET Micro Framework\v4.3\Tools\Microsoft.SPOT.tasks.dll
C:\Program Files (x86)\Microsoft .NET Micro Framework\v4.3\Tools\Microsoft.SPOT.tasks.pdb
with the provided files