Prometheus
==========

This is a chess engine written in C++.

How to Use
----------
To download and build:
```shell
git clone https://gitlab.com/zborffs/Prometheus
mkdir -p Prometheus/build
pushd Prometheus/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```
At this point, you ought to have the Cloak-lib.a static library and the Cloak executable built for your host.

To run in terminal:
```shell
./prometheus
```
if the program immediately terminates, something went wrong. Try adding a logs/ directory in the project home directory. 
Otherwise, you'll be prompted with an empty terminal. Type "uci" for UCI mode. 
