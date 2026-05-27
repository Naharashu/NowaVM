# NanoVM

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
