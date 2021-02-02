Tools Readme
============
Thoughts on tools stuff

Todos
-----
- tuning functionality in tools/ (something akin to texel tuning).
    - Potentially NNUE functionality in the future, but this seems a long way off, and is trained with the classical
      eval, so as long as the classical eval is shit, what's the point?
    - tuning data can be acquired easily with c-chess-cli tool
- tournament functionality (https://github.com/lucasart/c-chess-cli)
    - Julia script that wraps up the c-chess-cli script
    - Julia script will pass some gauntlet parameters to the c-chess-cli, which will return different sorts of data
      for tuning etc.
    - Julia parses tournament output to determine engine elo / skill and shit
- test suite analysis (general engine analysis)
    - test suites will give easy way of marking progress
    - julia scripts can monitor how changes affect things like ordering, time, success rates, etc.
    - scripts can be aggregated into library, which in turn

Test Suite Analysis
-------------------
### Questions
What kind of information do I want?
- I want the overall ordering for the each test suite as well as for every test suite
    - table + graph for each test suite
    - single table + graph combined test suites
- I want the ordering broken down by game stage and by position type
    - covariance between game stage and ordering and game stage and position type
    - graph of overall ordering of all test suites for different game stages and different position types
- I want to know the relationship between total nodes and leaf nodes, and total nodes and time
- I want to know the effect of game stage and position type on time and on number of nodes
- I want to know time on success
- I want to know game stage and position type on success
- I want to know ordering on success
### Goals
- from the look of things, I should be able to consolidate the move-ordering
  scripts into a library files and then some scripts.
- the test suite scripts will be performing ordering analysis, so in a sense
  I can kill two birds with one stone and do ordering analysis at the same time
  I do test suite analysis
- But i'll save the move-ordering scripts for now, because they work and are
  good references
