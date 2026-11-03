	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_BoneProjectile_0:: @ 08129634
	.include "animations/gSpriteAnimations_BoneProjectile_0.s"

gSpriteAnimations_BoneProjectile:: @ 08129648
	ptr gSpriteAnimations_BoneProjectile_0
	ptr 00000000
