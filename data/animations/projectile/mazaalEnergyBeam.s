	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_MazaalEnergyBeam_0:: @ 08129A70
	.include "animations/gSpriteAnimations_MazaalEnergyBeam_0.s"

gSpriteAnimations_MazaalEnergyBeam_1:: @ 08129A89
	.include "animations/gSpriteAnimations_MazaalEnergyBeam_1.s"

gSpriteAnimations_MazaalEnergyBeam:: @ 08129AB0
	ptr gSpriteAnimations_MazaalEnergyBeam_0
	ptr gSpriteAnimations_MazaalEnergyBeam_1
	ptr 00000000
