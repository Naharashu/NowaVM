_start:
ld r1 10
ld r2 1
jmp _main

_main:
add r0 r2
cmp r0 r1
jl _main
hlt
