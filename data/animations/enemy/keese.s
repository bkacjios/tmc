	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_Keese_0:: @ 080CB706
	.include "animations/gSpriteAnimations_Keese_0.s"

gSpriteAnimations_Keese_1:: @ 080CB71E
	.include "animations/gSpriteAnimations_Keese_1.s"

gSpriteAnimations_Keese:: @ 080CB728
	ptr gSpriteAnimations_Keese_0
	ptr gSpriteAnimations_Keese_1
	ptr 00000000
