	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2


gSpriteAnimations_ArrowProjectile_0:: @ 08129A28
	.include "animations/gSpriteAnimations_ArrowProjectile_0.s"

gSpriteAnimations_ArrowProjectile_1:: @ 08129A2D
	.include "animations/gSpriteAnimations_ArrowProjectile_1.s"

gSpriteAnimations_ArrowProjectile_2:: @ 08129A32
	.include "animations/gSpriteAnimations_ArrowProjectile_2.s"

gSpriteAnimations_ArrowProjectile_3:: @ 08129A43
	.include "animations/gSpriteAnimations_ArrowProjectile_3.s"

gSpriteAnimations_ArrowProjectile:: @ 08129A54
	ptr gSpriteAnimations_ArrowProjectile_0
	ptr gSpriteAnimations_ArrowProjectile_1
	ptr gSpriteAnimations_ArrowProjectile_2
	ptr gSpriteAnimations_ArrowProjectile_3
