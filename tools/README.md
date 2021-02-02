Tools
=====
The tools directory contains different scripts and programs for facilitating chess engine related things that won't
directly affect the engine software.

bin
---
- contains c-chess-cli executable for regression testing and tuning

CloakTools.jl
-------------
- Julia code (not strictly speaking a package) for performing both ad hoc and automated statistical analysis on
  tournaments and test suites
  
data
----
- contains some .txt files of engine outputs, .hdf5 files for persistent data about engine statistics, and protobook
  data.

move-ordering
-------------
- Soon to be merged with CloakTools.jl, but currently performs ad hoc and automatic statistical analysis specifically
  as they relate to the Move ordering of some test suites

opening-book
------------
- C++ source code for building the opening book. It interacts with the engine's classes and functions to facilitate the
  construction of the book.
  
scripts
-------
- place for shell scripts that serve as reminders for what to execute to do certain things like automated testing

tune
----
- C++ source code for tuning the engine's evaluation parameters