.syntax unified
   .cpu cortex-m4
   .thumb
   .global max7219_send
   .equ RCC_AHB2ENR, 0x4002104C
  .equ GPIOA_MODER, 0x48000000
  .equ GPIOA_OTYPER,0x48000004
  .equ GPIOA_OSPEEDR,0x48000008
  .equ GPIOA_PUPDR, 0x4800000C
  .equ GPIOA_ODR, 	0x48000014
  .equ	DATA,		0x20//PA5
  .equ	LOAD,		0x40//PA6
  .equ	CLOCK,		0x80//PA7
  .equ	DECODE_MODE,0x09
  .equ	DISPLAY_TEST,0x0F
  .equ	SCAN_LIMIT,	0x0B
  .equ	INTENSITY,	0x0A
  .equ	SHUTDOWN,	0x0C
  .equ	GPIO_BSRR_OFFSET, 0x18
  .equ	GPIO_BRR_OFFSET, 0x28

  max7219_send:
//input parameter: r0 is ADDRESS , r1 is DATA
//TODO: Use this function to send a message to max7219
push {r4,r5,r6,r7,r8,r9,r10,r11,lr}
lsl r0, r0, #8
add r0, r0, r1
ldr r1, =#GPIOA_MODER
ldr r2, =#LOAD
ldr r3, =#DATA
ldr r4, =#CLOCK
ldr r5, =#GPIO_BSRR_OFFSET
ldr r6, =#GPIO_BRR_OFFSET
mov r7, #16//r7 = i

max7219send_loop:
mov r8, #1
sub r9, r7, #1
lsl r8, r8, r9 // r8 = mask
str r4, [r1,r6]//HAL_GPIO_WritePin(GPIOA, CLOCK, 0);
tst r0, r8
beq bit_not_set//bit not set
str r3, [r1,r5]
b if_done
bit_not_set:
str r3, [r1,r6]
if_done:
      str r4, [r1,r5]
subs r7, r7, #1
bgt max7219send_loop
str r2, [r1,r6]
str r2, [r1,r5]
pop {r4,r5,r6,r7,r8,r9,r10,r11,pc}
	BX LR

