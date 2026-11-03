	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_AmbientClouds_0:: @ 08120DA0
	.include "animations/gSpriteAnimations_AmbientClouds_0.s"

gSpriteAnimations_AmbientClouds_1:: @ 08120DB1
	.include "animations/gSpriteAnimations_AmbientClouds_1.s"

gSpriteAnimations_AmbientClouds:: @ 08120DC4
	ptr gSpriteAnimations_AmbientClouds_0
	ptr gSpriteAnimations_AmbientClouds_1
	ptr 00000000
