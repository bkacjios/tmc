	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
@    .align 2

gSpriteAnimations_WarpPoint_0:: @ 08121383
	.include "animations/gSpriteAnimations_WarpPoint_0.s"

gSpriteAnimations_WarpPoint_1:: @ 08121394
	.include "animations/gSpriteAnimations_WarpPoint_1.s"

gSpriteAnimations_WarpPoint:: @ 081213B8
	ptr gSpriteAnimations_WarpPoint_0
	ptr gSpriteAnimations_WarpPoint_1
	ptr 00000000
