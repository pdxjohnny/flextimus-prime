	.cpu arm7tdmi
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.align	2
	.global	delay
	.type	delay, %function
delay:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	str	r0, [fp, #-8]
	mov	r0, r0	@ nop
.L2:
	ldr	r3, [fp, #-8]
	sub	r2, r3, #1
	str	r2, [fp, #-8]
	cmp	r3, #0
	bne	.L2
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
	.size	delay, .-delay
	.align	2
	.global	initClock
	.type	initClock, %function
initClock:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	ldr	r2, .L5
	ldr	r3, .L5
	ldr	r3, [r3]
	bic	r3, r3, #16777216
	str	r3, [r2]
	mov	r0, r0	@ nop
.L4:
	ldr	r3, .L5
	ldr	r3, [r3]
	and	r3, r3, #33554432
	cmp	r3, #0
	bne	.L4
	ldr	r2, .L5+4
	ldr	r3, .L5+4
	ldr	r3, [r3]
	orr	r3, r3, #1
	str	r3, [r2]
	ldr	r2, .L5+4
	ldr	r3, .L5+4
	ldr	r3, [r3]
	bic	r3, r3, #6
	str	r3, [r2]
	ldr	r2, .L5+4
	ldr	r3, .L5+4
	ldr	r3, [r3]
	orr	r3, r3, #16
	str	r3, [r2]
	ldr	r2, .L5+8
	ldr	r3, .L5+8
	ldr	r3, [r3]
	bic	r3, r3, #3932160
	str	r3, [r2]
	ldr	r2, .L5+8
	ldr	r3, .L5+8
	ldr	r3, [r3]
	orr	r3, r3, #2621440
	str	r3, [r2]
	ldr	r2, .L5+8
	ldr	r3, .L5+8
	ldr	r3, [r3]
	orr	r3, r3, #16384
	str	r3, [r2]
	ldr	r2, .L5
	ldr	r3, .L5
	ldr	r3, [r3]
	orr	r3, r3, #16777216
	str	r3, [r2]
	ldr	r2, .L5+8
	ldr	r3, .L5+8
	ldr	r3, [r3]
	orr	r3, r3, #2
	str	r3, [r2]
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L6:
	.align	2
.L5:
	.word	1073876992
	.word	1073881088
	.word	1073876996
	.size	initClock, .-initClock
	.align	2
	.global	configPins
	.type	configPins, %function
configPins:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	ldr	r2, .L8
	ldr	r3, .L8
	ldr	r3, [r3]
	orr	r3, r3, #131072
	str	r3, [r2]
	ldr	r2, .L8
	ldr	r3, .L8
	ldr	r3, [r3]
	orr	r3, r3, #262144
	str	r3, [r2]
	mov	r2, #1207959552
	mov	r3, #1207959552
	ldr	r3, [r3]
	orr	r3, r3, #1
	str	r3, [r2]
	mov	r2, #1207959552
	mov	r3, #1207959552
	ldr	r3, [r3]
	bic	r3, r3, #2
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	orr	r3, r3, #64
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	bic	r3, r3, #128
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	orr	r3, r3, #256
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	bic	r3, r3, #512
	str	r3, [r2]
	mov	r2, #1207959552
	mov	r3, #1207959552
	ldr	r3, [r3]
	orr	r3, r3, #16
	str	r3, [r2]
	mov	r2, #1207959552
	mov	r3, #1207959552
	ldr	r3, [r3]
	bic	r3, r3, #32
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	orr	r3, r3, #1
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	bic	r3, r3, #2
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	orr	r3, r3, #4
	str	r3, [r2]
	ldr	r2, .L8+4
	ldr	r3, .L8+4
	ldr	r3, [r3]
	bic	r3, r3, #8
	str	r3, [r2]
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L9:
	.align	2
.L8:
	.word	1073877012
	.word	1207960576
	.size	configPins, .-configPins
	.comm	PauseState,4,4
	.align	2
	.global	ButtonIRQ
	.type	ButtonIRQ, %function
ButtonIRQ:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r3, .L15
	ldr	r3, [r3]
	and	r3, r3, #8
	cmp	r3, #0
	beq	.L11
	mov	r0, r0	@ nop
.L12:
	ldr	r3, .L15
	ldr	r3, [r3]
	and	r3, r3, #8
	cmp	r3, #0
	bne	.L12
	bl	Pause
	b	.L10
.L11:
	mov	r3, #0
.L10:
	mov	r0, r3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, lr}
	bx	lr
.L16:
	.align	2
.L15:
	.word	1207960592
	.size	ButtonIRQ, .-ButtonIRQ
	.align	2
	.global	Pause
	.type	Pause, %function
Pause:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	ldr	r3, .L21
	ldr	r3, [r3]
	cmp	r3, #0
	bne	.L18
	ldr	r3, .L21
	mov	r2, #1
	str	r2, [r3]
	ldr	r2, .L21+4
	ldr	r3, .L21+4
	ldr	r3, [r3]
	orr	r3, r3, #1
	str	r3, [r2]
	b	.L19
.L18:
	ldr	r3, .L21
	mov	r2, #0
	str	r2, [r3]
	ldr	r2, .L21+4
	ldr	r3, .L21+4
	ldr	r3, [r3]
	bic	r3, r3, #1
	str	r3, [r2]
.L19:
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L22:
	.align	2
.L21:
	.word	PauseState
	.word	1207960596
	.size	Pause, .-Pause
	.align	2
	.global	main
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r3, .L26
	ldr	r3, [r3]
	cmp	r3, #1
	bne	.L24
	bl	initClock
.L24:
	bl	configPins
	ldr	r3, .L26+4
	mov	r2, #0
	str	r2, [r3]
.L25:
	bl	ButtonIRQ
	b	.L25
.L27:
	.align	2
.L26:
	.word	1073881088
	.word	PauseState
	.size	main, .-main
	.ident	"GCC: (15:4.9.3+svn231177-1) 4.9.3 20150529 (prerelease)"
