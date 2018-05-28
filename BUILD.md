# xmr-mine-suite
A suite with some tools and scripts to mine xmr

## Set common flags:

```
set FLAGS_COMPILER=-maes -Ofast -march=native -mtune=native
set FLAGS_COMPILER_C=
set FLAGS_COMPILER_CPP=
set FLAGS_LINKER=-static
```

########################
#Generate "miner" (C++):
#
# You call:

```
gcc %FLAGS_COMPILER% %FLAGS_COMPILER_C%   -c -I../src/ ../src/crypto/c_blake256.c ../src/crypto/c_groestl.c ../src/crypto/c_jh.c ../src/crypto/c_keccak.c ../src/crypto/c_skein.c
g++ %FLAGS_COMPILER% %FLAGS_COMPILER_CPP% -c -I../src/ ../src/crypto/CryptoNight.cpp ../src/net/Job.cpp ../src/Mem.cpp ../src/Mem_unix.cpp ../src/Mem_win.cpp ../src/miner.cpp
g++ %FLAGS_LINKER% c_blake256.o c_groestl.o c_jh.o c_keccak.o c_skein.o CryptoNight.o Job.o Mem.o Mem_unix.o Mem_win.o miner.o -o miner.exe
```

## You get:

`miner.exe` tool.

## Demo result

You call:

```
miner.exe 07079deab1d805e410406e6f8ae09b8392a3fb338700da850378889983dd3b19c86a9822219cfc0000000047fe7a15a44870c21862e6e96eab0208ce79a8f5bff4cd2469dc94ccdbe6485b02 e4a63d00
```

You get:

```
00000526
00001447
000017b9
0000185b
00001aff
00001df2
00001e64
00002452
00002c6f
00002e99
000032db
000035a6
000036b1
000038e2
00003fe7
00004224
0000423a
00004292
000042fb
00004759
000047a6
00004ae4
```
*etc*

