;#include header.asm
; #include header.asm ; not includes twice
;_start:
ld R0 5 ; load 5 into r0
ld R1 3 ; load 3 into r1
add R0 R1 ; r0 = r0 + r1
ld r3 1
;call add_r3_r3
#define hillo 42
_start:
ltf r0 2
ltf r1 3
ltf r2 5
fma r0 r1 r2 
ftl r0
ftl r1
ftl r2
arx r0 r1 r2 r3
ld r42 hillo
#ifdef hi
#warn hello
#else
#error not hi
#endif
hlt ; end program
