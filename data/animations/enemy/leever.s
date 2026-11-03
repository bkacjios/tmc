	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
	.align 2

gSpriteAnimations_Leever_0:: @ 080CA4D4
	.include "animations/gSpriteAnimations_Leever_0.s"

gSpriteAnimations_Leever_1:: @ 080CA500
	.include "animations/gSpriteAnimations_Leever_1.s"

gSpriteAnimations_Leever_2:: @ 080CA51C
	.include "animations/gSpriteAnimations_Leever_2.s"

gSpriteAnimations_Leever:: @ 080CA560
	ptr gSpriteAnimations_Leever_0
	ptr gSpriteAnimations_Leever_1
	ptr gSpriteAnimations_Leever_2
	ptr 00000000
