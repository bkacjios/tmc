	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_HurdyGurdyMan_8:: @ 0811453C
	.include "animations/gSpriteAnimations_HurdyGurdyMan_8.s"

gSpriteAnimations_HurdyGurdyMan_7:: @ 0811454D
	.include "animations/gSpriteAnimations_HurdyGurdyMan_7.s"

gSpriteAnimations_HurdyGurdyMan_4:: @ 0811455E
	.include "animations/gSpriteAnimations_HurdyGurdyMan_4.s"

gSpriteAnimations_HurdyGurdyMan_0:: @ 0811456F
	.include "animations/gSpriteAnimations_HurdyGurdyMan_0.s"

gSpriteAnimations_HurdyGurdyMan_1:: @ 08114574
	.include "animations/gSpriteAnimations_HurdyGurdyMan_1.s"

gSpriteAnimations_HurdyGurdyMan_3:: @ 08114579
	.include "animations/gSpriteAnimations_HurdyGurdyMan_3.s"

gSpriteAnimations_HurdyGurdyMan:: @ 08114580
	ptr gSpriteAnimations_HurdyGurdyMan_0
	ptr gSpriteAnimations_HurdyGurdyMan_1
	ptr gSpriteAnimations_HurdyGurdyMan_0
	ptr gSpriteAnimations_HurdyGurdyMan_3
	ptr gSpriteAnimations_HurdyGurdyMan_4
	ptr gSpriteAnimations_HurdyGurdyMan_4
	ptr gSpriteAnimations_HurdyGurdyMan_4
	ptr gSpriteAnimations_HurdyGurdyMan_7
	ptr gSpriteAnimations_HurdyGurdyMan_8
	ptr gSpriteAnimations_HurdyGurdyMan_8
	ptr gSpriteAnimations_HurdyGurdyMan_8
	ptr gSpriteAnimations_HurdyGurdyMan_8
	ptr 00000000
