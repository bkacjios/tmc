	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_MacroBook_0:: @ 08123CB8
	.include "animations/gSpriteAnimations_MacroBook_0.s"

gSpriteAnimations_MacroBook_1:: @ 08123CBD
	.include "animations/gSpriteAnimations_MacroBook_1.s"

gSpriteAnimations_MacroBook_2:: @ 08123CCE
	.include "animations/gSpriteAnimations_MacroBook_2.s"

gSpriteAnimations_MacroBook_3:: @ 08123CEB
	.include "animations/gSpriteAnimations_MacroBook_3.s"

gSpriteAnimations_MacroBook:: @ 08123D14
	ptr gSpriteAnimations_MacroBook_0
	ptr gSpriteAnimations_MacroBook_1
	ptr gSpriteAnimations_MacroBook_2
	ptr gSpriteAnimations_MacroBook_3
	ptr 00000000
