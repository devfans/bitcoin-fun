#!/bin/bash/
set -e

BTC_SRC="/root/bitcoin-0.18.0"
CPUS=`lscpu | grep '^CPU(s):' | awk '{print $2}'`
echo "Build type $1"
cmake -DJOBS=$CPUS -DCMAKE_BUILD_TYPE=$1 -DCHAIN_SRC_ROOT=$BTC_SRC -DGENERATE_DEB_PACKAGE=ON ..
make -j $CPUS
