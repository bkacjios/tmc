	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_FireballGuy_0:: @ 080D1818
	.include "animations/gSpriteAnimations_FireballGuy_0.s"

gSpriteAnimations_FireballGuy_1:: @ 080D1839
	.include "animations/gSpriteAnimations_FireballGuy_1.s"

gSpriteAnimations_FireballGuy:: @ 080D185C
	ptr gSpriteAnimations_FireballGuy_0
	ptr gSpriteAnimations_FireballGuy_1
	ptr 00000000
