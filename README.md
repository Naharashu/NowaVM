# NowaVM
 
NowaVM is small, jit vm that executed bytecode. 

Current Version: 0.8

Status: InDev


## Compiling

requirements: g++/clang++, asmjit, make

#### ArchLinux:
```
yay -S asmjit clang++ make
```

then 

```
git clone https://github.com/Naharashu/NowaVM.git
cd NowaVM
make
```


## Scripts

Installing nowavm on termux

```
./termux.sh 
```

Installing nowavm on linux


```
./install.sh
```

Test vm:

```
./examples.sh
```


## Preprocesor directives

`#include [filename]` - include file's code

`#extern [lable nams]` - declare label as extern(for linker only)

`#define [NAME] (value)` - defines NAME, value is optional

`#undef [NAME]` - undefined NAME

`#ifdef [NAME]` - if NAME is defined

`#ifndef [NAME]` - if NAME is not defined

`#ifeq [NAME] [NAME2]` - if NAME value equal to NAME2

`#ifneq [NAME] [NAME2]` - if NAME value not equal to NAME2

`#else` - else statement for preprocessor

`#endif` - reqiured to end if statements in preprocessor
