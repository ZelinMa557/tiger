# tiger
A compiler for tiger programming language.  
All features except nested functions are now supported. In fact, nested functions are not completely unsupported, but they cannot access local variables in the parent function.

# Dependency
LLVM 10.0.0

# How to run ?
``` bash
mkdir build && cd build && cmake .. && cd ..
bash initBinarys.sh
bash tiger.sh input.tig output
./output
```

# Other feats
``` bash
./tiger input.tig out.bc -bc # gen llvm bytecode
./tiger input.tig out.ir -ir # gen llvm ir
./tiger input.tig out.ast -ast # print ast
```