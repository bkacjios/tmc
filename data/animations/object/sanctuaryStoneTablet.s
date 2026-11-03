	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_SanctuaryStoneTablet_0:: @ 08124808
	.include "animations/gSpriteAnimations_SanctuaryStoneTablet_0.s"

gSpriteAnimations_SanctuaryStoneTablet:: @ 0812481C
	ptr gSpriteAnimations_SanctuaryStoneTablet_0
	ptr 00000000
