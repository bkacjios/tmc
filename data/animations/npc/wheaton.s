	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Wheaton_0:: @ 0810C3C8
	.include "animations/gSpriteAnimations_Wheaton_0.s"

gSpriteAnimations_Wheaton_1:: @ 0810C3E1
	.include "animations/gSpriteAnimations_Wheaton_1.s"

gSpriteAnimations_Wheaton_2:: @ 0810C3F2
	.include "animations/gSpriteAnimations_Wheaton_2.s"

gSpriteAnimations_Wheaton_3:: @ 0810C403
	.include "animations/gSpriteAnimations_Wheaton_3.s"

gSpriteAnimations_Wheaton:: @ 0810C414
	ptr gSpriteAnimations_Wheaton_0
	ptr gSpriteAnimations_Wheaton_1
	ptr gSpriteAnimations_Wheaton_2
	ptr gSpriteAnimations_Wheaton_3
	ptr 00000000
