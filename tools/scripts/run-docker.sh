docker run -it --rm gitlab-tester:latest


cd root
git clone https://gitlab.com/asterion-project/chess-engine.git
cd chess-engine
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON -G "CodeBlocks - Unix Makefiles" ..
make
make BitmaskCoverage
make BoardCoverage
make ChessClockCoverage
make ChessMoveCoverage
make StringManipCoverage
make UCIOptionsCoverage
make EvalCoverage
lcov --add-tracefile BitmaskCoverage.info --add-tracefile BoardCoverage.info --add-tracefile ChessClockCoverage.info --add-tracefile ChessMoveCoverage.info --add-tracefile StringManipCoverage.info --add-tracefile UCIOptionsCoverage.info --add-tracefile EvalCoverage.info --output-file CompleteCoverage.info
lcov --remove CompleteCoverage.info '/usr/local/*' '/usr/include/*' -o CompleteCoverageFiltered.info

# At this point check the lcov output to get the code coverage
# - no guarantee yet that /usr/... corresponds with std::lib and spdlog in Gitlab (but most likely)
