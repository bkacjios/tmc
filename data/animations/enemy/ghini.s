	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Ghini_0:: @ 080D0988
	.include "animations/gSpriteAnimations_Ghini_0.s"

gSpriteAnimations_Ghini_2:: @ 080D09A4
	.include "animations/gSpriteAnimations_Ghini_2.s"

gSpriteAnimations_Ghini_1:: @ 080D09C9
	.include "animations/gSpriteAnimations_Ghini_1.s"

gSpriteAnimations_Ghini_3:: @ 080D09EE
	.include "animations/gSpriteAnimations_Ghini_3.s"

gSpriteAnimations_Ghini_4:: @ 080D0A32
	.include "animations/gSpriteAnimations_Ghini_4.s"

gSpriteAnimations_Ghini_5:: @ 080D0A6B
	.include "animations/gSpriteAnimations_Ghini_5.s"

gSpriteAnimations_Ghini_6:: @ 080D0A6F
	.include "animations/gSpriteAnimations_Ghini_6.s"

gSpriteAnimations_Ghini_7:: @ 080D0A73
	.include "animations/gSpriteAnimations_Ghini_7.s"

gSpriteAnimations_Ghini_8:: @ 080D0A7F
	.include "animations/gSpriteAnimations_Ghini_8.s"

gSpriteAnimations_Ghini_9:: @ 080D0A8B
	.include "animations/gSpriteAnimations_Ghini_9.s"

gSpriteAnimations_Ghini:: @ 080D0A90
	ptr gSpriteAnimations_Ghini_0
	ptr gSpriteAnimations_Ghini_1
	ptr gSpriteAnimations_Ghini_2
	ptr gSpriteAnimations_Ghini_3
	ptr gSpriteAnimations_Ghini_4
	ptr gSpriteAnimations_Ghini_5
	ptr gSpriteAnimations_Ghini_6
	ptr gSpriteAnimations_Ghini_7
	ptr gSpriteAnimations_Ghini_8
	ptr gSpriteAnimations_Ghini_9
	ptr 00000000
