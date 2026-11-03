	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_UnusedSkull_0:: @ 0811F918
	.include "animations/gSpriteAnimations_UnusedSkull_0.s"

gSpriteAnimations_UnusedSkull_1:: @ 0811F945
	.include "animations/gSpriteAnimations_UnusedSkull_1.s"

gSpriteAnimations_UnusedSkull:: @ 0811F954
	ptr gSpriteAnimations_UnusedSkull_0
	ptr gSpriteAnimations_UnusedSkull_1
	ptr 00000000
