	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Teachers_0:: @ 081139B0
	.include "animations/gSpriteAnimations_Teachers_0.s"

gSpriteAnimations_Teachers_3:: @ 081139CD
	.include "animations/gSpriteAnimations_Teachers_3.s"

gSpriteAnimations_Teachers_1:: @ 081139EA
	.include "animations/gSpriteAnimations_Teachers_1.s"

gSpriteAnimations_Teachers:: @ 08113A08
	ptr gSpriteAnimations_Teachers_0
	ptr gSpriteAnimations_Teachers_1
	ptr gSpriteAnimations_Teachers_0
	ptr gSpriteAnimations_Teachers_3
	ptr 00000000
