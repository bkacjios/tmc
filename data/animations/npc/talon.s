	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Talon_2:: @ 0810FEE0
	.include "animations/gSpriteAnimations_Talon_2.s"

gSpriteAnimations_Talon_0:: @ 0810FEE5
	.include "animations/gSpriteAnimations_Talon_0.s"

gSpriteAnimations_Talon_3:: @ 0810FEEA
	.include "animations/gSpriteAnimations_Talon_3.s"

gSpriteAnimations_Talon_1:: @ 0810FEEF
	.include "animations/gSpriteAnimations_Talon_1.s"

gSpriteAnimations_Talon_4:: @ 0810FEF4
	.include "animations/gSpriteAnimations_Talon_4.s"

gSpriteAnimations_Talon_5:: @ 0810FF05
	.include "animations/gSpriteAnimations_Talon_5.s"

gSpriteAnimations_Talon_6:: @ 0810FF16
	.include "animations/gSpriteAnimations_Talon_6.s"

gSpriteAnimations_Talon_7:: @ 0810FF27
	.include "animations/gSpriteAnimations_Talon_7.s"

gSpriteAnimations_Talon:: @ 0810FF38
	ptr gSpriteAnimations_Talon_0
	ptr gSpriteAnimations_Talon_1
	ptr gSpriteAnimations_Talon_2
	ptr gSpriteAnimations_Talon_3
	ptr gSpriteAnimations_Talon_4
	ptr gSpriteAnimations_Talon_5
	ptr gSpriteAnimations_Talon_6
	ptr gSpriteAnimations_Talon_7
	ptr 00000000
