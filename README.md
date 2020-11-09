Asterion Engine
===============
[![pipeline status](https://gitlab.com/asterion-project/chess-engine/badges/master/pipeline.svg)](https://gitlab.com/asterion-project/chess-engine/-/commits/master)
[![coverage report](https://gitlab.com/asterion-project/chess-engine/badges/master/coverage.svg)](https://gitlab.com/asterion-project/chess-engine/-/commits/master)
 
This is a chess engine written in C++.

How to Use
----------
To download and build:
```shell
git clone https://gitlab.com/asterion-project/asterion-engine.git
cd asterion-engine
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```
At this point, you ought to have the Prometheus-lib.a static library and the Prometheus executable built for your host.

To run in terminal:
```shell
./Prometheus
```
if the program immediately terminates, something went wrong. Try adding a logs/ directory in the project home directory. 
Otherwise, you'll be prompted with an empty terminal. Type "console" for the custom console mode and "uci" for UCI mode. 
