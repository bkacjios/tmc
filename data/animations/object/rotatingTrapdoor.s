	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_RotatingTrapdoor_0:: @ 08121B94
	.include "animations/gSpriteAnimations_RotatingTrapdoor_0.s"

gSpriteAnimations_RotatingTrapdoor:: @ 08121BD0
	ptr gSpriteAnimations_RotatingTrapdoor_0
	ptr 00000000
