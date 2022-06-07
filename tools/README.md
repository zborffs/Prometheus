tools
=====
this directory contains code for facilitating engine regression testing, opening book creation, and parameter tuning. 
none of the software in this directory is in the engine executable though, so it's kept in this separate directory.

data
----
### Regression 0
- Tournament 600 games with ```go movetime 1000``` (i.e. 1 second per move for each engine) btw Prometheus and vice
- Bratko-Kopec test suite with 30 seconds per position
- WAC test suite with 10 seconds per position

opening book
------------
- C++ source code for building the opening book. It interacts with the engine's classes and functions to facilitate the
construction of the book.

```shell
./c-chess-cli -games 200 -each movetime=1.0 -engine cmd=../../cmake-build-debug-sse/prometheus -engine cmd=./Loki3 -engine cmd=./vice -pgn out.pgn 3
```