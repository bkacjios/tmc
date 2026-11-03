	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_FallingBoulder_0:: @ 080CD5A4
	.include "animations/gSpriteAnimations_FallingBoulder_0.s"

gSpriteAnimations_FallingBoulder_1:: @ 080CD5B5
	.include "animations/gSpriteAnimations_FallingBoulder_1.s"

gSpriteAnimations_FallingBoulder:: @ 080CD5C8
	ptr gSpriteAnimations_FallingBoulder_0
	ptr gSpriteAnimations_FallingBoulder_1
	ptr 00000000
