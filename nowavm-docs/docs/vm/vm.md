# NowaVM

## Compilation

You can compile .asm files that contains vm assembly syntax into .bin.

For example:
```
ld r0 42
hlt
```

will compile into a.bin:
```
0x01 0x00 0x2A 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFF
```

but it also will produce a.sym that contains inforamation for nwld.

example b.asm:

```
func:
ld r0 42
ret

funcx:
hlt
```

b.sym will be:

```
EXPORT funcx 11
EXPORT func 0
```


## Standard library

### entry0.asm

entry0.asm is a file that contains only one loc:

```
jmp _start
```

Its main use case is just jump to the standard entry point.
