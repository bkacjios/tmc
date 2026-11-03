	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_VaatiProjectile_0:: @ 080D0688
	.include "animations/gSpriteAnimations_VaatiProjectile_0.s"

gSpriteAnimations_VaatiProjectile_1:: @ 080D06A1
	.include "animations/gSpriteAnimations_VaatiProjectile_1.s"

gSpriteAnimations_VaatiProjectile_2:: @ 080D06B2
	.include "animations/gSpriteAnimations_VaatiProjectile_2.s"

gSpriteAnimations_VaatiProjectile:: @ 080D06D0
	ptr gSpriteAnimations_VaatiProjectile_0
	ptr gSpriteAnimations_VaatiProjectile_1
	ptr gSpriteAnimations_VaatiProjectile_2
	ptr 00000000
