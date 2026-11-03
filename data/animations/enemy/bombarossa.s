	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Bombarossa_0:: @ 080CEB60
	.include "animations/gSpriteAnimations_Bombarossa_0.s"

gSpriteAnimations_Bombarossa:: @ 080CEB6C
	ptr gSpriteAnimations_Bombarossa_0
	ptr 00000000
