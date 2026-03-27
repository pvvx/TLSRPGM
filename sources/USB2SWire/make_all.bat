@set TLPATH=D:\MCU\TelinkIoTStudio
@set PATH=%TLPATH%\bin;%TLPATH%\opt\tc32\bin;%TLPATH%\mingw\bin;%TLPATH%\opt\tc32\tc32-elf\bin;%PATH%
make -s clean
make -s -j PROJECT_NAME=usb2sw_e180z6907 POJECT_DEF="-DSET_PLL=QUARTZ_12MHZ"

make -s clean
make -s -j PROJECT_NAME=usb2sw_et104bt10 POJECT_DEF="-DSET_PLL=QUARTZ_16MHZ"

