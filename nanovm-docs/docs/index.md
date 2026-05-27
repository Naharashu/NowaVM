# Welcome to NowaVM docs

Github repo: [click](https://github.com/Naharashu/NowaVM)

## Requirements

- x64 Linux os (e.g. Ubuntu, Arch)
- GNU make
- asmjit (see [asmjit build docs](https://asmjit.com/doc/group__asmjit__build.html))
- clang++ or g++
- git

## Installing(Linux)

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
		git clone https://github.com/Naharashu/NowaVM.git && cd NowaVM
	```

3. Building and Installing

	```
		make -j2 && make linker && make install
	```

## Installing(Termux)

1. Installing reqiured packages:

	```
		pkg install git make clang
	```

2. Clonning repo:

	```
		git clone https://github.com/Naharashu/NowaVM.git && cd NowaVM
	```

3. Running building script:
	
	```
		chmod +x termux.sh && ./termux.sh
	```
