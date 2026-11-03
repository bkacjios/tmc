	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_BigVortex_0:: @ 08123698
	.include "animations/gSpriteAnimations_BigVortex_0.s"

gSpriteAnimations_BigVortex_1:: @ 081236A9
	.include "animations/gSpriteAnimations_BigVortex_1.s"

gSpriteAnimations_BigVortex_2:: @ 081236BA
	.include "animations/gSpriteAnimations_BigVortex_2.s"

gSpriteAnimations_BigVortex:: @ 081236CC
	ptr gSpriteAnimations_BigVortex_0
	ptr gSpriteAnimations_BigVortex_1
	ptr gSpriteAnimations_BigVortex_2
	ptr 00000000
