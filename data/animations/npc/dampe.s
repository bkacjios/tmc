	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Dampe_0:: @ 08113350
	.include "animations/gSpriteAnimations_Dampe_0.s"

gSpriteAnimations_Dampe_4:: @ 08113369
	.include "animations/gSpriteAnimations_Dampe_4.s"

gSpriteAnimations_Dampe_5:: @ 0811336E
	.include "animations/gSpriteAnimations_Dampe_5.s"

gSpriteAnimations_Dampe_7:: @ 08113377
	.include "animations/gSpriteAnimations_Dampe_7.s"

gSpriteAnimations_Dampe:: @ 08113380
	ptr gSpriteAnimations_Dampe_0
	ptr gSpriteAnimations_Dampe_0
	ptr gSpriteAnimations_Dampe_0
	ptr gSpriteAnimations_Dampe_0
	ptr gSpriteAnimations_Dampe_4
	ptr gSpriteAnimations_Dampe_5
	ptr gSpriteAnimations_Dampe_4
	ptr gSpriteAnimations_Dampe_7
	ptr 00000000
