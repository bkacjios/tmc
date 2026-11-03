	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Wisp_0:: @ 080CEBAC
	.include "animations/gSpriteAnimations_Wisp_0.s"

gSpriteAnimations_Wisp_1:: @ 080CEBBD
	.include "animations/gSpriteAnimations_Wisp_1.s"

gSpriteAnimations_Wisp:: @ 080CEBD0
	ptr gSpriteAnimations_Wisp_0
	ptr gSpriteAnimations_Wisp_1
	ptr 00000000
