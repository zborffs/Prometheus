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

THIS IS WHAT I WAS DOING LAST TIME
==================================
- trying to integrate hashing
- first need to integrate move ordering basics
- did basic MVVLVA move ordering into the search
- created a new test suite full of positions from other test suites for different parts of games
- need to create a c++ program that automatically searches all of those positions
  - then rather than printing to the screen all of the output from the engine saying "info score ...", save it to a file. better yet save only a list of move orderings to the file with some other meta-data like (game stage, number of nodes, and depth related to a specific position)
  - see how long this will take and make sure it takes <120 seconds for everything (that's less than 2 seconds per position)
  - then write a julia script to read that .csv or whatever it will be;
  - analyze: we want to know:
    - how depth affects move ordering (if it does)
    - how game stage affects move ordering (if it does)
    - how color affects move ordering (if it does)
    - overall statistics for all of those subcategories and in total
    - draw graphs and show basic stats
  - then implement new move ordering (with MVVLVA) and redo the entire test automatically (potentially all called by a shell script and the images are automatically saved somewhere)
  - then make changes to the move ordering until we've reached a peak or whatever
