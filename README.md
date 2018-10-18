# entropy_calc
2 programs that calculate entropy of a file (byte by byte).

`entropy.c` - tries to read all of the file, if not possible proceeds byte by byte.

`entropy_map.cpp` - a C++ implementation using std::unordered_map instead. The overhead here was biggger than I expected.
