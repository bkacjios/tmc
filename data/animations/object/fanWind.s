	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_FanWind_0:: @ 081249B8
	.include "animations/gSpriteAnimations_FanWind_0.s"

gSpriteAnimations_FanWind_1:: @ 081249C9
	.include "animations/gSpriteAnimations_FanWind_1.s"

gSpriteAnimations_FanWind_2:: @ 081249DA
	.include "animations/gSpriteAnimations_FanWind_2.s"

gSpriteAnimations_FanWind_3:: @ 081249EB
	.include "animations/gSpriteAnimations_FanWind_3.s"

gSpriteAnimations_FanWind:: @ 081249FC
	ptr gSpriteAnimations_FanWind_0
	ptr gSpriteAnimations_FanWind_1
	ptr gSpriteAnimations_FanWind_2
	ptr gSpriteAnimations_FanWind_3
	ptr 00000000
