# Вітаю у документація до NowaVM

Github repo: [click](https://github.com/Naharashu/NowaVM)

Issues: [click](https://github.com/Naharashu/NowaVM/issues)

## Вимоги

- x64 Linux ос (наприклад Ubuntu, Arch)
- GNU make
- asmjit (дивіться [asmjit build docs](https://asmjit.com/doc/group__asmjit__build.html))
- clang++
- git

## Встановлення(Linux)

1. Встановлення залежностей:

- Ubuntu/Debian:
```console
sudo apt install make clang git
```

- Arch:
```console
sudo pacman -S make clang git
```

2. Клонування репозиторію:

```console
git clone https://github.com/Naharashu/NowaVM.git && cd NowaVM
```

3. Збірка та інсталяція:

```console
make -j2 && make linker && make install
```

## Встановлення(Termux)

1. Встановлення залежностей:

```console
pkg install git make clang
```

2. Клонування репозиторію:

```console
git clone https://github.com/Naharashu/NowaVM.git && cd NowaVM
```

3. Запуск скрипту збірки:
	
```console
chmod +x termux.sh && ./termux.sh
```
