	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_LakituLightning_0:: @ 08129C08
	.include "animations/gSpriteAnimations_LakituLightning_0.s"

gSpriteAnimations_LakituLightning:: @ 08129C74
	ptr gSpriteAnimations_LakituLightning_0
	ptr 00000000
