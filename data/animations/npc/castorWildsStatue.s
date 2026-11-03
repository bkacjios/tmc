	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_CastorWildsStatue_0:: @ 08110E9C
	.include "animations/gSpriteAnimations_CastorWildsStatue_0.s"

gSpriteAnimations_CastorWildsStatue_4:: @ 08110EA1
	.include "animations/gSpriteAnimations_CastorWildsStatue_4.s"

gSpriteAnimations_CastorWildsStatue_1:: @ 08110EA6
	.include "animations/gSpriteAnimations_CastorWildsStatue_1.s"

gSpriteAnimations_CastorWildsStatue:: @ 08110EAC
	ptr gSpriteAnimations_CastorWildsStatue_0
	ptr gSpriteAnimations_CastorWildsStatue_1
	ptr gSpriteAnimations_CastorWildsStatue_0
	ptr gSpriteAnimations_CastorWildsStatue_1
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr gSpriteAnimations_CastorWildsStatue_0
	ptr gSpriteAnimations_CastorWildsStatue_1
	ptr gSpriteAnimations_CastorWildsStatue_0
	ptr gSpriteAnimations_CastorWildsStatue_1
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr gSpriteAnimations_CastorWildsStatue_4
	ptr 00000000
