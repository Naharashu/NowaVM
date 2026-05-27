# Welcome to NowaVM docs

Github repo: [click](https://github.com/Naharashu/NowaVM)

## Requirements

- x64 linux os
- GNU make
- asmjit
- clang++ (g++ is acceptable but not recomended)
- git

## Installetion

1. Installing reqiured packages:

	- Ubuntu/Debian:
	```
		sudo apt install make clang git
	```

	- Arch:
	```
		sudo pacman -S make clang git
	```

2. Clonning repo:

	```
		git clome https://github.com/Naharashu/NowaVM.git && cd NowaVM
	```

3. Building and Installing

	```
		make -j2 && make linker && make install
	```

