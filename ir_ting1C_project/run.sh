rm -rf build/
cmake -S . -B build/
cmake --build build/
rm -f ~/sample
cp build/sample ~

