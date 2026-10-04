_start:
	vldqw vr0 41
	vldqw vr1 1
	vaddqw vr0 vr1
	ld r0 3.14
	ld r1 0.01
	fadd r0 r1
`	hlt
