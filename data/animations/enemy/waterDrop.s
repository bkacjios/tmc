	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_WaterDrop_0:: @ 080CD044
	.include "animations/gSpriteAnimations_WaterDrop_0.s"

gSpriteAnimations_WaterDrop_1:: @ 080CD069
	.include "animations/gSpriteAnimations_WaterDrop_1.s"

gSpriteAnimations_WaterDrop:: @ 080CD070
	ptr gSpriteAnimations_WaterDrop_0
	ptr gSpriteAnimations_WaterDrop_1
	ptr 00000000
