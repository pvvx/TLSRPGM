@Path=E:\Telink\SDK\bin;E:\Telink\SDK\opt\tc32\bin;E:\Telink\SDK\opt\tc32\tools;%Path%
@del /Q floader.bin
@del /Q floader.lst
make -s clean
make -s -j4