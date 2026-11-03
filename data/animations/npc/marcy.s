	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Marcy_0:: @ 0810C360
	.include "animations/gSpriteAnimations_Marcy_0.s"

gSpriteAnimations_Marcy_4:: @ 0810C365
	.include "animations/gSpriteAnimations_Marcy_4.s"

gSpriteAnimations_Marcy_8:: @ 0810C382
	.include "animations/gSpriteAnimations_Marcy_8.s"

gSpriteAnimations_Marcy:: @ 0810C390
	ptr gSpriteAnimations_Marcy_0
	ptr gSpriteAnimations_Marcy_0
	ptr gSpriteAnimations_Marcy_0
	ptr gSpriteAnimations_Marcy_0
	ptr gSpriteAnimations_Marcy_4
	ptr gSpriteAnimations_Marcy_4
	ptr gSpriteAnimations_Marcy_4
	ptr gSpriteAnimations_Marcy_4
	ptr gSpriteAnimations_Marcy_8
	ptr gSpriteAnimations_Marcy_8
	ptr gSpriteAnimations_Marcy_8
	ptr gSpriteAnimations_Marcy_8
