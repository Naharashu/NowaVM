_add:
  ld r0 10
  ld r1 32
  add r1 r0
  ret

_start:
  call main
  hlt

main:
  call _add
  ret