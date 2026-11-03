	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Flame_0:: @ 08120DD8
	.include "animations/gSpriteAnimations_Flame_0.s"

gSpriteAnimations_Flame:: @ 08120DEC
	ptr gSpriteAnimations_Flame_0
	ptr 00000000
