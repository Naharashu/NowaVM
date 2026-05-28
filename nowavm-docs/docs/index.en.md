# Welcome to NowaVM docs

Github repo: [click](https://github.com/Naharashu/NowaVM)

Issues: [click](https://github.com/Naharashu/NowaVM/issues)

## Requirements

- x64 Linux os (e.g. Ubuntu, Arch)
- GNU make
- asmjit (see [asmjit build docs](https://asmjit.com/doc/group__asmjit__build.html))
- clang++
- git

## Installing(Linux)

1. Installing required packages:

- Ubuntu/Debian:
```console
sudo apt install make clang git
```

- Arch:
```console
sudo pacman -S make clang git
```

2. Clone repository:

```console
git clone https://github.com/Naharashu/NowaVM.git && cd NowaVM
```

3. Building and Installing

```console
make -j2 && make linker && make install
```

## Installing(Termux)

1. Installing required packages:

```console
pkg install git make clang
```

2. Clon repository:

```console
git clone https://github.com/Naharashu/NowaVM.git && cd NowaVM
```

3. Run building script:
	
```console
chmod +x termux.sh && ./termux.sh
```
