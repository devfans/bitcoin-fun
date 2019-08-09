#!/bin/bash

set -e

workdir="/root/bitcoin-fun/build"
CPUS=`lscpu | grep '^CPU(s):' | awk '{print $2}'`

apt-get update
apt-get install -y build-essential autotools-dev libtool autoconf automake pkg-config cmake \
                   openssl libssl-dev libcurl4-openssl-dev libconfig++-dev \
                   libboost-all-dev libgmp-dev libmysqlclient-dev libzookeeper-mt-dev \
                   libzmq3-dev libgoogle-glog-dev libhiredis-dev zlib1g zlib1g-dev \
                   libprotobuf-dev protobuf-compiler

cd $workdir
git clone --single-branch --depth=1 https://github.com/libevent/libevent.git
cd libevent
./autogen.sh
./configure --disable-shared
make && make install

cd $workdir

wget https://github.com/edenhill/librdkafka/archive/0.9.1.tar.gz
tar zxvf 0.9.1.tar.gz
cd librdkafka-0.9.1
./configure && make && make install

cd $workdir
# if you want to keep static libraries only
rm -v /usr/local/lib/librdkafka*.so /usr/local/lib/librdkafka*.so.*


