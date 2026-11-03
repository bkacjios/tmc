	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_WizzrobeIce_0:: @ 080CE098
	.include "animations/gSpriteAnimations_WizzrobeIce_0.s"

gSpriteAnimations_WizzrobeIce_1:: @ 080CE0A5
	.include "animations/gSpriteAnimations_WizzrobeIce_1.s"

gSpriteAnimations_WizzrobeIce_2:: @ 080CE0B2
	.include "animations/gSpriteAnimations_WizzrobeIce_2.s"

gSpriteAnimations_WizzrobeIce_3:: @ 080CE0BF
	.include "animations/gSpriteAnimations_WizzrobeIce_3.s"

gSpriteAnimations_WizzrobeIce_4:: @ 080CE0CC
	.include "animations/gSpriteAnimations_WizzrobeIce_4.s"

gSpriteAnimations_WizzrobeIce_5:: @ 080CE0D9
	.include "animations/gSpriteAnimations_WizzrobeIce_5.s"

gSpriteAnimations_WizzrobeIce_6:: @ 080CE0E6
	.include "animations/gSpriteAnimations_WizzrobeIce_6.s"

gSpriteAnimations_WizzrobeIce_7:: @ 080CE0F3
	.include "animations/gSpriteAnimations_WizzrobeIce_7.s"

gSpriteAnimations_WizzrobeIce:: @ 080CE100
	ptr gSpriteAnimations_WizzrobeIce_0
	ptr gSpriteAnimations_WizzrobeIce_1
	ptr gSpriteAnimations_WizzrobeIce_2
	ptr gSpriteAnimations_WizzrobeIce_3
	ptr gSpriteAnimations_WizzrobeIce_4
	ptr gSpriteAnimations_WizzrobeIce_5
	ptr gSpriteAnimations_WizzrobeIce_6
	ptr gSpriteAnimations_WizzrobeIce_7
	ptr 00000000
