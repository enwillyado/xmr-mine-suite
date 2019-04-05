#!/bin/bash

FLAGS_BASIC=" -Wall"
FLAGS_COMPILER="-maes -Ofast -march=native -mtune=native"
FLAGS_COMPILER_C=""
FLAGS_COMPILER_CPP="-DWITH_ASM_INTEL -DNDEBUG"
FLAGS_LINKER="-static"

mkdir -p obj
cd obj

gcc $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_C   -c -I../src/ ../src/crypto/c_blake256.c ../src/crypto/c_groestl.c ../src/crypto/c_jh.c ../src/crypto/c_keccak.c ../src/crypto/c_skein.c
g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/crypto/cryptonight_v.cpp ../src/crypto/cryptonight.cpp ../src/net/Job.cpp ../src/miner.cpp
g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_C   -c -I../src/ ../src/crypto/cryptonightR_template.S
ar rcsvouU miner.a  cryptonight.o cryptonight_v.o cryptonightR_template.o c_blake256.o c_groestl.o c_jh.o c_keccak.o c_skein.o Job.o miner.o

g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/3rdparty/w_tcp/tcpclient.cpp ../src/3rdparty/w_tcp/tcpserver.cpp # ../src/3rdparty/w_sistema/w_sistema_thread_comun.cpp ../src/3rdparty/w_sistema/w_sistema_thread_unix.cpp  ../src/3rdparty/w_sistema/w_sistema_thread_win.cpp 
ar rcsvouU 3rdparty.a  tcpclient.o tcpserver.o # w_sistema_thread_comun.o w_sistema_thread_unix.o w_sistema_thread_win.o

g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/minerApp.cpp ../src/minerWorker.cpp
g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/Finder.cpp ../src/Workers.cpp ../src/finderApp.cpp

g++ $FLAGS_LINKER minerApp.o miner.a -o ../miner.exe
g++ $FLAGS_LINKER minerWorker.o miner.a -o ../miner_worker.exe
g++ $FLAGS_LINKER -pthread -lpthread Finder.o Workers.o finderApp.o 3rdparty.a -o ../finder_app.exe

cd ..

strip -s miner.exe
strip -s miner_worker.exe
strip -s finder_app.exe
