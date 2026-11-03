	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_SpikedBeetle_0:: @ 080CD31C
	.include "animations/gSpriteAnimations_SpikedBeetle_0.s"

gSpriteAnimations_SpikedBeetle_1:: @ 080CD32D
	.include "animations/gSpriteAnimations_SpikedBeetle_1.s"

gSpriteAnimations_SpikedBeetle_2:: @ 080CD33E
	.include "animations/gSpriteAnimations_SpikedBeetle_2.s"

gSpriteAnimations_SpikedBeetle_3:: @ 080CD34F
	.include "animations/gSpriteAnimations_SpikedBeetle_3.s"

gSpriteAnimations_SpikedBeetle_4:: @ 080CD360
	.include "animations/gSpriteAnimations_SpikedBeetle_4.s"

gSpriteAnimations_SpikedBeetle_5:: @ 080CD368
	.include "animations/gSpriteAnimations_SpikedBeetle_5.s"

gSpriteAnimations_SpikedBeetle_6:: @ 080CD370
	.include "animations/gSpriteAnimations_SpikedBeetle_6.s"

gSpriteAnimations_SpikedBeetle_7:: @ 080CD378
	.include "animations/gSpriteAnimations_SpikedBeetle_7.s"

gSpriteAnimations_SpikedBeetle_8:: @ 080CD380
	.include "animations/gSpriteAnimations_SpikedBeetle_8.s"

gSpriteAnimations_SpikedBeetle_9:: @ 080CD391
	.include "animations/gSpriteAnimations_SpikedBeetle_9.s"

gSpriteAnimations_SpikedBeetle:: @ 080CD398
	ptr gSpriteAnimations_SpikedBeetle_0
	ptr gSpriteAnimations_SpikedBeetle_1
	ptr gSpriteAnimations_SpikedBeetle_2
	ptr gSpriteAnimations_SpikedBeetle_3
	ptr gSpriteAnimations_SpikedBeetle_4
	ptr gSpriteAnimations_SpikedBeetle_5
	ptr gSpriteAnimations_SpikedBeetle_6
	ptr gSpriteAnimations_SpikedBeetle_7
	ptr gSpriteAnimations_SpikedBeetle_8
	ptr gSpriteAnimations_SpikedBeetle_9
	ptr 00000000
