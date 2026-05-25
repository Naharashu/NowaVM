CXX=clang++
CXX_FLAGS=-O2 -g -pipe -flto=thin -std=c++20 -Wall
CXX_FLAGS_FASTER=-O3 -pipe -flto=thin -std=c++20 -Wall 
CXX_FLAGS_DEBUG=-O0 -g3 -pipe -std=c++20 -Wall -Wextra -fno-omit-frame-pointer -fsanitize=address

ASMJIT := $(wildcard /usr/include/asmjit/x86.h)
ifeq ($(ASMJIT),)
$(error "ERROR - ASMJIT not found on that pc, please install asmjit")
endif


all: main

linker:
	clang++ -O2 -march=x86-64 -pipe -std=c++20 -Wall src/linker/main.cpp -o nwld

main:
	$(CXX) $(CXX_FLAGS) src/*.cpp -lasmjit  -o nanovm

native:
	@echo "WARNING - -march=native makes executable only work with your cpu"
	@echo "INFO - if you want more optimized executable, use make faster"
	$(CXX) $(CXX_FLAGS) src/*.cpp -lasmjit -march=native -o nanovm

faster:
	$(CXX) $(CXX_FLAGS_FASTER) src/*.cpp -lasmjit -o nanovm

debug:
	$(CXX) $(CXX_FLAGS_DEBUG) src/*.cpp -lasmjit -o nanovm

install:
	mkdir -p ~/.local/bin/include_nanovm/
	cp entry0.asm ~/.local/bin/include_nanovm/entry0.asm
	cp nanovm ~/.local/bin/nanovm

clean:
	rm -f ./nanovm
