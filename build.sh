#!/bin/bash

FLAGS_COMPILER="-maes -Ofast -march=native -mtune=native"
FLAGS_COMPILER_C=""
FLAGS_COMPILER_CPP=""
FLAGS_LINKER="-static"

mkdir -p obj
cd obj

gcc $FLAGS_COMPILER $FLAGS_COMPILER_C   -c -I../src/ ../src/crypto/c_blake256.c ../src/crypto/c_groestl.c ../src/crypto/c_jh.c ../src/crypto/c_keccak.c ../src/crypto/c_skein.c
g++ $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/crypto/CryptoNight.cpp ../src/net/Job.cpp ../src/Mem.cpp ../src/Mem_unix.cpp ../src/Mem_win.cpp ../src/miner.cpp
g++ $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/minerApp.cpp ../src/minerWorker.cpp
ar rcs miner.a c_blake256.o c_groestl.o c_jh.o c_keccak.o c_skein.o CryptoNight.o Job.o Mem.o Mem_unix.o Mem_win.o miner.o

g++ $FLAGS_LINKER minerApp.o miner.a -o ../miner.exe
g++ $FLAGS_LINKER minerWorker.o miner.a -o ../miner_worker.exe

cd ..

strip -s miner.exe
