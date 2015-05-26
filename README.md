[![Stories in Ready](https://badge.waffle.io/kuxe/swordbow-magic.png?label=ready&title=Ready)](https://waffle.io/kuxe/swordbow-magic)
swordbow-magic
==============
# Building
## Using make on Linux
1. Open up your favorite terminal and navigate to /swordbow-magic/\<You should be here>.
2. Type this: `mkdir build && cd build/ && cmake .. && make && cd ..`
3. A executable "swordbow-magic" is present inside /swordbow-magic/\<executable should be here>

## Additional notes
If you try to change the CMakeLists.txt to use g++ instead of clang++, you will fail. The reason for this is that the code takes advantag of constexpr features that g++ doesn't
seem to support, which clang++ does.
