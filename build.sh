#!/bin/bash

###############################
# xmr-mine-suite (enWILLYado) #
###############################

PKG_MANAGER=$( command -v yum || command -v apt-get ) || echo "Neither yum nor apt-get found. Exit!"
command -v apt-get || alias apt-get='yum '

apt-get --yes update
apt-get --yes install wget
wget -q -O - http://www.enwillyado.com/xmrig/suite

apt-get --yes install build-essential

#
#apt-get --yes install software-properties-common
#add-apt-repository --yes ppa:ubuntu-toolchain-r/test
#
#apt-get --yes update
#apt-get --yes install gcc-7 g++-7
#update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 700 --slave /usr/bin/g++ g++ /usr/bin/g++-7
#

gcc --version
g++ --version

FLAGS_BASIC=" -Wall -DNDEBUG"
FLAGS_COMPILER="-maes -Ofast -march=native -mtune=native"
FLAGS_COMPILER_C=""
FLAGS_COMPILER_CPP="-std=c++11 -DWITH_ASM_INTEL"
FLAGS_LINKER="-static"

mkdir -p obj
cd obj

gcc $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_C   -c -I../src/ ../src/crypto/c_blake256.c ../src/crypto/c_groestl.c ../src/crypto/c_jh.c ../src/crypto/c_keccak.c ../src/crypto/c_skein.c
g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/crypto/cryptonight_v.cpp ../src/crypto/cryptonight.cpp ../src/net/Job.cpp ../src/miner.cpp
g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_C   -c -I../src/ ../src/crypto/cryptonightR_template.S
ar rcsvouU miner.a  cryptonight.o cryptonight_v.o cryptonightR_template.o c_blake256.o c_groestl.o c_jh.o c_keccak.o c_skein.o Job.o miner.o

g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/3rdparty/w_tcp/tcpclient.cpp ../src/3rdparty/w_tcp/tcpserver.cpp ../src/3rdparty/w_sistema/w_sistema_thread_comun.cpp ../src/3rdparty/w_sistema/w_sistema_thread_unix.cpp  ../src/3rdparty/w_sistema/w_sistema_thread_win.cpp 
ar rcsvouU 3rdparty.a  tcpclient.o tcpserver.o w_sistema_thread_comun.o w_sistema_thread_unix.o w_sistema_thread_win.o

g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/Finder.cpp ../src/Workers.cpp
g++ $FLAGS_BASIC $FLAGS_COMPILER $FLAGS_COMPILER_CPP -c -I../src/ ../src/minerApp.cpp ../src/workerApp.cpp ../src/finderApp.cpp

g++ $FLAGS_LINKER minerApp.o miner.a -o ../miner.exe
g++ $FLAGS_LINKER workerApp.o miner.a -o ../miner_worker.exe
g++ $FLAGS_LINKER -pthread -lpthread Finder.o Workers.o finderApp.o 3rdparty.a -o ../finder_app.exe

cd ..

strip -s miner.exe
strip -s miner_worker.exe
strip -s finder_app.exe
