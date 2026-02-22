# TLSRPGM
TLSR SWire programmer


>** Option 1:
Use TB-04-KIT or TB-04 / 03F module (TLSR8253 chip) and USB-COM (PL2303HX or CH340 chip).

Pin PD3 (OUT) --> RESET, Pin PD4 (SWM) <-> SWS, Pin PB4 (OUT) --> Power (max 16 mA)

![TB-03F-KIT-TLSRPGM.png](https://github.com/pvvx/TLSRPGM/blob/main/TB-03F-KIT-TLSRPGM.png)   <img width="320" height="269" alt="image" src="https://github.com/user-attachments/assets/0dbcf502-898a-48b4-8801-2f276b43c327" />

>** Option 2:
Use EBYTE E104-BT10-G / N module or (TLSR8269 chip) for direct USB connection

Pin PB1 (OUT) --> RESET, Pin PA7 (SWM) <-> SWS, Pin PB4 (OUT) --> Power


The current version of the 'TlsrPgm.py' program is tested with chips only: TLSR8250, TLSR8251, TLSR8253, TLSR8258, TLSR8266, TLSR8269.
The firmware for the module works with all types of TLSR82xx chips via SWM-SWS connection.

1. [How to write TLSRPGM program to TB-03F-KIT or TB-04-KIT](https://github.com/pvvx/TLSRPGM/tree/main/sources/UART2SWire/tb-0x-pgm).
2. TLSRPGM Usage Examples:

#### Info: 
> **Options:** wsl /usr/bin/python3 TlsrPgm.py
```
usage: TlsrPgm [-h] [-p PORT] [-b BAUD] [-t TRST] [-a ACT] [-z ZW] [-s] [-c]
               [-r] [-g] [-m] [-w] [-u]
               {rf,we,wf,es,ea,fsr,fsw,rs,ws,wsb,wsw,wsd,ra,wa,i,ds,df,da,dc,sws} ...

TLSR82xx TlsrPgm version 03.12.25

positional arguments:
  {rf,we,wf,es,ea,fsr,fsw,rs,ws,wsb,wsw,wsd,ra,wa,i,ds,df,da,dc,sws}
                        Run TlsrPgm {command} -h for additional help
    rf                  Read Flash to binary file
    we                  Write file to Flash with sectors erases
    wf                  Write file to Flash without sectors erases
    es                  Erase Region (sectors) of Flash
    ea                  Erase All Flash
    fsr                 Read Flash Status Register
    fsw                 Write Flash Status Register
    rs                  Read SWire address to binary file
    ws                  Write file to Swire addres
    wsb                 Write byte to Swire addres
    wsw                 Write 16-bit word to Swire address
    wsd                 Write 32-bit word to Swire address
    ra                  Read Analog Registers to binary file
    wa                  Write file to Analog Registers
    i                   External Chip Information
    ds                  Dump Swire (Registers or SRAM)
    df                  Dump Flash
    da                  Dump Analog Registers
    dc                  Show uit32 register or SRAM addres
    sws                 SWS Printf (PGM version >= 0.0.0.4 is used)

options:
  -h, --help            show this help message and exit
  -p, --port PORT       Serial port device (default: COM1)
  -b, --baud BAUD       UART Baud Rate (default: 230400)
  -t, --trst TRST       Hard Reset Time ms (default: 0 ms)
  -a, --act ACT         Activation Time ms (0-off, default: 0 ms)
  -z, --zw ZW           Wait Start CPU (default: 0 sec) (before main
                        processing)
  -s, --stopcpu         CPU Stop (before main processing)
  -c, --cpustall        CPU Stall (before main processing)
  -r, --run             CPU Run (post main processing)
  -g, --go              CPU Go (post main processing)
  -m, --mrst            Soft Reset (MCU Reboot) (post main processing)
  -w, --wrktime         Show Worked Time
  -u, --u2b             Use 2 bytes swire address (TLSR826x)
```

#### Samples:
> **Chip info:** wsl /usr/bin/python3 TlsrPgm.py -s -p/dev/ttyS3 i
```
=======================================================
TLSR825x TlsrPgm version 25.11.20
-------------------------------------------------------
Open /dev/ttyS3, 230400 bit/s... ok
PGM: ChipID: 0x5562 (TLSR825x), ver: 0.0.0.1
swdiv 5, addrlen 3, swbuf [5a 00 06 02 00 05], pwr On
SWire bit rate: 0.9600 Mbits/s
=== PreProcess ========================================
CPU Stop... ok
=== Process ===========================================
Chip ID: 0x5562, rev: 0x02
CPU PC=0x000000 ([0x0602] = 0x05)
Flash JEDEC ID: 0xC86013, Size: 512 kbytes
-------------------------------------------------------
FLASH UID:
000000: 41 50 32 31 36 39 33 01 00 40 00 f1 fe e5 ff ff 
000010: c8 01 ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
-------------------------------------------------------
FLASH:
000000: 26 80 00 00 00 00 5d 02 4b 4e 4c 54 30 04 88 00 
000010: ae 80 00 00 00 00 00 00 a4 4f 01 00 00 00 00 00 
-------------------------------------------------------
ANALOG REGISTERS:
000000: 48 4c a4 4f 48 02 ff 1d 05 00 00 7b c4 80 00 00 
000010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 
000020: 54 00 00 00 00 00 00 00 00 00 00 2e 1f 15 00 00 
000030: 20 20 80 80 07 20 00 00 00 ff 00 00 00 00 00 00 
000040: 39 29 aa 00 80 00 00 00 00 00 00 00 ff ff ff 0f 
000050: 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
000060: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
000070: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 
000080: 48 4c a4 4f 48 02 ff 1d 05 00 00 7b c4 80 00 00 
000090: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 
0000a0: 54 00 00 00 00 00 00 00 00 00 00 2e 1f 15 00 00 
0000b0: 20 20 80 80 07 20 00 00 00 ff 00 00 00 00 00 00 
0000c0: 62 29 aa 00 80 00 00 00 00 00 00 00 ff ff ff 0f 
0000d0: 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0000e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0000f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 
-------------------------------------------------------
REGISTERS:
000060: 7c f7 c7 83 08 30 06 00 02 00 02 00 01 02 1f 00 
000070: 00 04 00 04 00 00 00 00 00 00 00 64 00 02 62 55 
```

> **Read Full Flash:** TlsrPgm.py -w -t100 -a200 -c -pCOM3 -b1500000 rf 0 0x80000 ff_512k.bin
```
=======================================================
TLSR825x TlsrPgm version 25.11.20
-------------------------------------------------------
Open COM3, 230400 bit/s... ok
PGM: ChipID: 0x5562 (TLSR825x), ver: 0.0.0.1
swdiv 5, addrlen 3, swbuf [5a 00 06 02 00 05], pwr On
SWire bit rate: 0.9600 Mbits/s
Set Speed UART PGM board 3000000 bits/s... ok
ReOpen COM3, 3000000 bit/s... ok
=== PreProcess ========================================
Hard reset Ext.MCU 5 ms... ok
Activate 20 ms... ok
CPU PC=0x000000
CPU Stall... ok
=== Process ===========================================
Outfile: ff_512k.bin
Read Flash from 0x000000 to 0x080000...
                               
 Worked Time: 11.305 sec
```

> **Read the dump of RAM (on the fly, waiting for the SoC to exit sleep):** TlsrPgm.py -pCOM6 -b1500000 -z10 -c -g ds 0x848000 0x10
```
=======================================================
TLSR82xx TlsrPgm version 03.12.25
-------------------------------------------------------
Open COM6, 230400 bit/s... ok
PGM: ChipID: 0x5562 (TLSR825x), ver: 0.0.0.5
swdiv 5, addrlen 3, swbuf [5a 00 06 02 00 05], pwr On
SWire bit rate: 0.9600 Mbits/s
Set Speed UART PGM board 1500000 bits/s... ok
ReOpen COM6, 1500000 bit/s... ok
=== PreProcess ========================================

CPU PC=0x00000574          
CPU Stall ([0x0602] = 0x06)
=== Process ===========================================
-------------------------------------------------------

REGISTERS:
848000: 9c ad 90 b0 f6 ef ad fe 68 6d 1a b1 fd ef fe ed 
=== Post-Process ======================================
CPU Go... ok
```

### SWS_Printf()

В FW UART2SWire версии 0.0.0.4 программатора и “TlsrPgm.py” добавлена функция вывода сообщений printf() через соединение SWM-SWS по протоколу Telink SWire.

Приложен пример c sws_printf() для SoC TLSR825x (в SampleSwsPrintf).

В “TlsrPgm.py” желательна установка COM-порта на 500000 или 921600 Baud. 

Но данный функционал полностью работоспособн и на стандартной скорости в 230400, устанавливаемой в “TlsrPgm.py” по умолчанию. 

При работе SWire на скорости 0.9600 Mbits/s номинальный выдаваемый поток находится в пределах около 500000 Baud, если переводить это на COM порт.

При меньших установках скорости COM-порта и непрерывной работе printf() будут вставляться межкадровые  задержки в опросе SWire...
 
Для скорости COM-порта ниже 115200 Baud уже следует увеличить время ожидания в “SWS_WAITING_TIMEOUT”.

### Ускорение обработки ожидания выхода из сна

В FW UART2SWire версии 0.0.0.5 программатора и “TlsrPgm.py” добавлена ускоренная функция чтения PC с остановкой или stall CPU.

Ранее наблюдались пропуски при быстрых пробуждениях и засыпаниях SoC. Теперь это обрабатывается на максимальной скорости SWire, без пауз между командами.

### Test вывода SWM

1. Соединить PD4(SWM) и SWS на PB-0xx
2. Выполнить (изменив номер порта на используемый модулем PB-0xx):
```
TlsrPgm.py -pCOM6 i
```

---

Диаграмма шины и описание регистров [Telink SWire](https://github.com/pvvx/TLSRPGM/tree/main/TelinkSWire) контроллера:  [TelinkSWire.pdf](https://github.com/pvvx/TLSRPGM/blob/main/TelinkSWire/TelinkSWire.pdf) 

Это вырезка из ранней документации на чип TLSR8266 выложеной на сайте Telink много лет назад. В текущих версиях datasheet  на чипы TRLS82xx этот раздел удален.
