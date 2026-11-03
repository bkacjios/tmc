	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Melari_1:: @ 08111544
	.include "animations/gSpriteAnimations_Melari_1.s"

gSpriteAnimations_Melari_0:: @ 08111549
	.include "animations/gSpriteAnimations_Melari_0.s"

gSpriteAnimations_Melari_3:: @ 0811154E
	.include "animations/gSpriteAnimations_Melari_3.s"

gSpriteAnimations_Melari_8:: @ 08111553
	.include "animations/gSpriteAnimations_Melari_8.s"

gSpriteAnimations_Melari_9:: @ 0811158C
	.include "animations/gSpriteAnimations_Melari_9.s"

gSpriteAnimations_Melari:: @ 08111594
	ptr gSpriteAnimations_Melari_0
	ptr gSpriteAnimations_Melari_1
	ptr gSpriteAnimations_Melari_0
	ptr gSpriteAnimations_Melari_3
	ptr gSpriteAnimations_Melari_0
	ptr gSpriteAnimations_Melari_1
	ptr gSpriteAnimations_Melari_0
	ptr gSpriteAnimations_Melari_3
	ptr gSpriteAnimations_Melari_8
	ptr gSpriteAnimations_Melari_9
	ptr 00000000
    
