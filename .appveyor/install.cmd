REM Windows install script

rmdir /s /q c:\MinGW
curl -LO %SMINGTOOLS%/MinGW.7z
7z -oC:\ x MinGW.7z

set

echo %SMING_ARCH%

REM ======

goto :%SMING_ARCH%

:Esp8266

	REM Old toolchain
	set TOOLCHAIN=esp-udk-win32.7z
	curl -LO %SMINGTOOLS%/%TOOLCHAIN%
	7z -o%UDK_ROOT% x %TOOLCHAIN%

	REM New toolchain
	mkdir %EQT_ROOT%
	set TOOLCHAIN=i686-w64-mingw32.xtensa-lx106-elf-a5c9861.1575819473.zip
	curl -LO https://github.com/earlephilhower/esp-quick-toolchain/releases/download/3.0.0-gnu5/%TOOLCHAIN%
	7z -o%EQT_ROOT% x %TOOLCHAIN%

	goto :EOF


:Host

	goto :EOF

