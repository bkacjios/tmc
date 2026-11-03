	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_V1EyeLaser_0:: @ 0812A63C
	.include "animations/gSpriteAnimations_V1EyeLaser_0.s"

gSpriteAnimations_V1EyeLaser_1:: @ 0812A65C
	.include "animations/gSpriteAnimations_V1EyeLaser_1.s"

gSpriteAnimations_V1EyeLaser_2:: @ 0812A665
	.include "animations/gSpriteAnimations_V1EyeLaser_2.s"

gSpriteAnimations_V1EyeLaser_3:: @ 0812A66E
	.include "animations/gSpriteAnimations_V1EyeLaser_3.s"

gSpriteAnimations_V1EyeLaser_4:: @ 0812A677
	.include "animations/gSpriteAnimations_V1EyeLaser_4.s"

gSpriteAnimations_V1EyeLaser_5:: @ 0812A680
	.include "animations/gSpriteAnimations_V1EyeLaser_5.s"

gSpriteAnimations_V1EyeLaser_6:: @ 0812A689
	.include "animations/gSpriteAnimations_V1EyeLaser_6.s"

gSpriteAnimations_V1EyeLaser:: @ 0812A694
	ptr gSpriteAnimations_V1EyeLaser_0
	ptr gSpriteAnimations_V1EyeLaser_1
	ptr gSpriteAnimations_V1EyeLaser_2
	ptr gSpriteAnimations_V1EyeLaser_3
	ptr gSpriteAnimations_V1EyeLaser_4
	ptr gSpriteAnimations_V1EyeLaser_5
	ptr gSpriteAnimations_V1EyeLaser_6
	ptr 00000000
