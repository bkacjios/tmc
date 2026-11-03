	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Vaati1Portal_0:: @ 08124878
	.include "animations/gSpriteAnimations_Vaati1Portal_0.s"

gSpriteAnimations_Vaati1Portal:: @ 08124890
	ptr gSpriteAnimations_Vaati1Portal_0
	ptr 00000000
