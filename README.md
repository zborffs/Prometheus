Cloak
=====
[![pipeline status](https://gitlab.com/zborffs/cloak/badges/master/pipeline.svg)](https://gitlab.com/zborffs/cloak/-/commits/master)
[![coverage report](https://gitlab.com/zborffs/cloak/badges/master/coverage.svg)](https://gitlab.com/zborffs/cloak/-/commits/master)
 
This is a chess engine written in C++.

How to Use
----------
To download and build:
```shell
git clone https://gitlab.com/zborffs/cloak
mkdir -p Cloak/build
pushd Cloak/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```
At this point, you ought to have the Cloak-lib.a static library and the Cloak executable built for your host.

To run in terminal:
```shell
./Cloak
```
if the program immediately terminates, something went wrong. Try adding a logs/ directory in the project home directory. 
Otherwise, you'll be prompted with an empty terminal. Type "uci" for UCI mode. 
