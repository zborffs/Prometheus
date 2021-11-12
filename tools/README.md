Tools
=====
The tools directory contains different scripts and programs for facilitating chess engine related things that won't
directly affect the engine software.

bin
---
- contains c-chess-cli executable for regression testing and tuning
  data

Data
----
- contains some .txt files of engine outputs, .hdf5 files for persistent data about engine statistics, and protobook
  data.


math
-------------
- optimization, hypothesis testing, summary statistics, distributions, hdf5 
  - heteroscedasticity-consitent standard errors
  - white test (test for  heteroscedasticity)
  - M-estimators (robust)
  - L-estimators
  - diff robust statistics
  - nonparametric skew

opening-book
------------
- C++ source code for building the opening book. It interacts with the engine's classes and functions to facilitate the
  construction of the book.
  

tune
----
- C++ source code for tuning the engine's evaluation parameters
