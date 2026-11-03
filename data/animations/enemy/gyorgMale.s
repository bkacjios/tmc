	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_GyorgMale_0:: @ 080D1C70
	.include "animations/gSpriteAnimations_GyorgMale_0.s"

gSpriteAnimations_GyorgMale_1:: @ 080D1C91
	.include "animations/gSpriteAnimations_GyorgMale_1.s"

gSpriteAnimations_GyorgMale:: @ 080D1CBC
	ptr gSpriteAnimations_GyorgMale_0
	ptr gSpriteAnimations_GyorgMale_1
	ptr 00000000
