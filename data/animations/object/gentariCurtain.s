	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_GentariCurtains_0:: @ 0812261C
	.include "animations/gSpriteAnimations_GentariCurtains_0.s"

gSpriteAnimations_GentariCurtains_1:: @ 08122631
	.include "animations/gSpriteAnimations_GentariCurtains_1.s"

gSpriteAnimations_GentariCurtains:: @ 08122638
	ptr gSpriteAnimations_GentariCurtains_0
	ptr gSpriteAnimations_GentariCurtains_1
	ptr 00000000
