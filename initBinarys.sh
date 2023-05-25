cd build && make && cd ..
clang -c runtime/runtime.c -o runtime.o
if [ -f "tiger" ]; then
    rm tiger
fi
cp build/tiger .