.syntax unified
   .cpu cortex-m4
   .thumb
.global delay_us


delay_us:
push {r4-r11,lr}

	movs r1,#20
	muls r0,r0,r1
	subs r0,r0,#20
L1: subs r0,r0,#1
	bgt L1
pop {r4-r11,pc}
