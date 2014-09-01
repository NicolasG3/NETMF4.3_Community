@echo off
echo Set Environnement variables for GCC
call c:\MicroFrameworkPK_v4_3\setenv_gcc 4.6.2 c:\gcc_4_6_2
call msbuild /p:flavor=debug /filelogger
pause