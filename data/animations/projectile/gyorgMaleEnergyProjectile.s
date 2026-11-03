	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_GyorgMaleEnergyProjectile_0:: @ 0812A9FC
	.include "animations/gSpriteAnimations_GyorgMaleEnergyProjectile_0.s"

gSpriteAnimations_GyorgMaleEnergyProjectile_1:: @ 0812AA31
	.include "animations/gSpriteAnimations_GyorgMaleEnergyProjectile_1.s"

gSpriteAnimations_GyorgMaleEnergyProjectile:: @ 0812AA50
	ptr gSpriteAnimations_GyorgMaleEnergyProjectile_0
	ptr gSpriteAnimations_GyorgMaleEnergyProjectile_1
	ptr 00000000
