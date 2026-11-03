	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_GyorgFemaleMouth_0:: @ 080D291C
	.include "animations/gSpriteAnimations_GyorgFemaleMouth_0.s"

gSpriteAnimations_GyorgFemaleMouth_1:: @ 080D292D
	.include "animations/gSpriteAnimations_GyorgFemaleMouth_1.s"

gSpriteAnimations_GyorgFemaleMouth_2:: @ 080D293E
	.include "animations/gSpriteAnimations_GyorgFemaleMouth_2.s"

gSpriteAnimations_GyorgFemaleMouth_3:: @ 080D294F
	.include "animations/gSpriteAnimations_GyorgFemaleMouth_3.s"

gSpriteAnimations_GyorgFemaleMouth:: @ 080D2960
	ptr gSpriteAnimations_GyorgFemaleMouth_0
	ptr gSpriteAnimations_GyorgFemaleMouth_1
	ptr gSpriteAnimations_GyorgFemaleMouth_2
	ptr gSpriteAnimations_GyorgFemaleMouth_3
	ptr 00000000
