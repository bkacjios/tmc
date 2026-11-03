	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_RockChuchu_0:: @ 080CB96C
	.include "animations/gSpriteAnimations_RockChuchu_0.s"

gSpriteAnimations_RockChuchu_1:: @ 080CB995
	.include "animations/gSpriteAnimations_RockChuchu_1.s"

gSpriteAnimations_RockChuchu_2:: @ 080CB9D6
	.include "animations/gSpriteAnimations_RockChuchu_2.s"

gSpriteAnimations_RockChuchu:: @ 080CBA18
	ptr gSpriteAnimations_RockChuchu_0
	ptr gSpriteAnimations_RockChuchu_1
	ptr gSpriteAnimations_RockChuchu_2
	ptr 00000000
