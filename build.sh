#!/bin/bash

FLAGS_COMPILER="-maes -Ofast -march=native -mtune=native"
FLAGS_COMPILER_C=""
FLAGS_COMPILER_CPP="-DWITH_ASM_INTEL -DNDEBUG"
FLAGS_LINKER="-static"

mkdir -p obj
cd obj

gcc -v -Wall $FLAGS_COMPILER $FLAGS_COMPILER_C   -c -I../src/ ../src/crypto/c_blake256.c ../src/crypto/c_groestl.c ../src/crypto/c_jh.c ../src/crypto/c_keccak.c ../src/crypto/c_skein.c
g++ -v -Wall $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/crypto/cryptonight_v.cpp ../src/crypto/cryptonight.cpp ../src/net/Job.cpp ../src/miner.cpp
g++ -v -Wall $FLAGS_COMPILER $FLAGS_COMPILER_C   -c -I../src/ ../src/crypto/cryptonightR_template.S
g++ -v -Wall $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/minerApp.cpp ../src/minerWorker.cpp
ar rcsvouU miner.a  cryptonight.o cryptonight_v.o cryptonightR_template.o c_blake256.o c_groestl.o c_jh.o c_keccak.o c_skein.o Job.o miner.o

g++ $FLAGS_LINKER minerApp.o miner.a -o ../miner.exe
g++ $FLAGS_LINKER minerWorker.o miner.a -o ../miner_worker.exe

cd ..

strip -s miner.exe
