	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Fan_0:: @ 081243F4
	.include "animations/gSpriteAnimations_Fan_0.s"

gSpriteAnimations_Fan_1:: @ 08124465
	.include "animations/gSpriteAnimations_Fan_1.s"

gSpriteAnimations_Fan_2:: @ 081244D6
	.include "animations/gSpriteAnimations_Fan_2.s"

gSpriteAnimations_Fan_3:: @ 08124547
	.include "animations/gSpriteAnimations_Fan_3.s"

gSpriteAnimations_Fan_4:: @ 081245B8
	.include "animations/gSpriteAnimations_Fan_4.s"

gSpriteAnimations_Fan_5:: @ 081245DD
	.include "animations/gSpriteAnimations_Fan_5.s"

gSpriteAnimations_Fan_6:: @ 08124602
	.include "animations/gSpriteAnimations_Fan_6.s"

gSpriteAnimations_Fan_7:: @ 08124627
	.include "animations/gSpriteAnimations_Fan_7.s"

gSpriteAnimations_Fan:: @ 0812464C
	ptr gSpriteAnimations_Fan_0
	ptr gSpriteAnimations_Fan_1
	ptr gSpriteAnimations_Fan_2
	ptr gSpriteAnimations_Fan_3
	ptr gSpriteAnimations_Fan_4
	ptr gSpriteAnimations_Fan_5
	ptr gSpriteAnimations_Fan_6
	ptr gSpriteAnimations_Fan_7
	ptr 00000000
