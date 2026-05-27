CXX:=clang++
CXX_FLAGS:=-O2 -g -pipe -flto=thin -std=c++20 -Wall -march=x86-64
CXX_FLAGS_NATIVE:=-O3 -pipe -flto=thin -std=c++20 -Wall -march=native -mtune=native 
CXX_FLAGS_DEBUG:=-O0 -g3 -pipe -std=c++20 -Wall -Wextra -fno-omit-frame-pointer -fsanitize=address
CXX_OBJECTS := build/assembly.o build/vm.o build/main.o 

clean:
	rm -f ./nowavm ./nwld
	rm -f build/*.o

ASMJIT := $(wildcard /usr/include/asmjit/x86.h)
CLANG := $(shell which clang 2>/dev/null)
ifeq ($(ASMJIT),)
$(error "ERROR - ASMJIT is not found on that pc, please install asmjit")
endif

ifeq ($(CLANG),)
$(error "ERROR - Clang is not found on that pc, please install clang")
endif

.PHONY: all linker debug native install clean main gcc

gcc:
        make CXX=g++ CXX_FLAGS="-O2 -g -pipe -std=c++20 -Wall -march=x86-64"

all: main

linker:
        $(CXX) -O2 -g1 -march=x86-64-v2 -pipe -std=c++20 -Wall src/linker/linker.cpp -o nwld

main: $(CXX_OBJECTS)
        $(CXX) $(CXX_FLAGS) $(CXX_OBJECTS) -lasmjit  -o nowavm

vpath %.cpp src src/assembly 

build/%.o: %.cpp
        $(CXX) $(CXX_FLAGS) -c $< -o $@        

native: $(CXX_OBJECTS)
        @echo "WARNING - -march=native makes executable only work with your cpu"
        @echo "INFO - if you want more optimized executable, use make faster"
        $(CXX) $(CXX_FLAGS_NATIVE) $(CXX_OBJECTS) -lasmjit  -o nowavm


debug: $(CXX_OBJECTS)
        $(CXX) $(CXX_FLAGS_DEBUG) $(CXX_OBJECTS) -lasmjit  -o nowavm

install:
        mkdir -p ~/.local/bin/include_nowavm/
        cp entry0.asm ~/.local/bin/include_nowavm/entry0.asm
        cp nanovm ~/.local/bin/nowavm
        cp nanovm ~/.local/bin/nwld

