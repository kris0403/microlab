.syntax unified
   .cpu cortex-m4
   .thumb
   .global max7219_init
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

   max7219_init:
  //TODO: Initialize max7219 registers
	push {r0, r1, r2, lr}
	ldr	 r0, =#DECODE_MODE
	ldr	 r1, =#0xFF
	BL	max7219_send
	ldr	 r0, =#DISPLAY_TEST
	ldr	 r1, =#0x0
	BL	max7219_send
	ldr	 r0, =#SCAN_LIMIT
	ldr	 r1, =#0x07
	BL	max7219_send
	ldr	 r0, =#INTENSITY
	ldr	 r1, =#0xA
	BL	max7219_send
	ldr	 r0, =#SHUTDOWN
	ldr	 r1, =#0x1
	BL	max7219_send
	pop {r0, r1, r2, pc}
  	BX LR
