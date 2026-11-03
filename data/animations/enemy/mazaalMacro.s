	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_MazaalMacro_1:: @ 080CEF08
	.include "animations/gSpriteAnimations_MazaalMacro_1.s"

gSpriteAnimations_MazaalMacro_0:: @ 080CEF0C
	.include "animations/gSpriteAnimations_MazaalMacro_0.s"

gSpriteAnimations_MazaalMacro_2:: @ 080CEF1D
	.include "animations/gSpriteAnimations_MazaalMacro_2.s"

gSpriteAnimations_MazaalMacro:: @ 080CEF24
	ptr gSpriteAnimations_MazaalMacro_0
	ptr gSpriteAnimations_MazaalMacro_1
	ptr gSpriteAnimations_MazaalMacro_2
	ptr 00000000
