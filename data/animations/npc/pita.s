	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
 @   .align 2

gSpriteAnimations_Pita_0:: @ 0810C445
	.include "animations/gSpriteAnimations_Pita_0.s"

gSpriteAnimations_Pita_4:: @ 0810C44A
	.include "animations/gSpriteAnimations_Pita_4.s"

gSpriteAnimations_Pita:: @ 0810C468
	ptr gSpriteAnimations_Pita_0
	ptr gSpriteAnimations_Pita_0
	ptr gSpriteAnimations_Pita_0
	ptr gSpriteAnimations_Pita_0
	ptr gSpriteAnimations_Pita_4
	ptr gSpriteAnimations_Pita_4
	ptr gSpriteAnimations_Pita_4
	ptr gSpriteAnimations_Pita_4
	ptr 00000000
