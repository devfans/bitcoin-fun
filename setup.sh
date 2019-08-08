#!/bin/bash
set -e

apt-get update
apt-get install -y  vim curl wget zip git cmake

wget https://github.com/bitcoin/bitcoin/archive/v0.18.0.zip
unzip v0.18.0.zip 
cd bitcoin-0.18.0/

apt-get install build-essential libtool autotools-dev automake pkg-config bsdmainutils python3
apt-get install libzmq3-dev
apt-get install libqrencode-dev
./autogen.sh 
apt-get install libdb-dev
apt-get install libdb++-dev
apt-get install -y libssl-dev libevent-dev libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-test-dev libboost-thread-dev

./configure --without-gui --with-incompatible-bdb 
make
ls
git clone https://github.com/devfans/bitcoin-fun.git
cd bitcoin-fun/
cd build
bash ../init.sh 
bash ../build.sh 
