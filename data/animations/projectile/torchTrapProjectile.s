	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_TorchTrapProjectile_0_0:: @ 0812A3E0
	.include "animations/gSpriteAnimations_TorchTrapProjectile_0_0.s"

gSpriteAnimations_TorchTrapProjectile_0:: @ 0812A3F4
	ptr gSpriteAnimations_TorchTrapProjectile_0_0
	ptr 00000000

gSpriteAnimations_TorchTrapProjectile_1_0:: @ 0812A3FC
	.include "animations/gSpriteAnimations_TorchTrapProjectile_1_0.s"

gSpriteAnimations_TorchTrapProjectile_1:: @ 0812A410
	ptr gSpriteAnimations_TorchTrapProjectile_1_0
	ptr 00000000
