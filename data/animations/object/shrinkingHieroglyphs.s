	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
@    .align 2

gSpriteAnimations_ShrinkingHieroglyphs_0:: @ 08123422
	.include "animations/gSpriteAnimations_ShrinkingHieroglyphs_0.s"

gSpriteAnimations_ShrinkingHieroglyphs_1:: @ 0812343B
	.include "animations/gSpriteAnimations_ShrinkingHieroglyphs_1.s"

gSpriteAnimations_ShrinkingHieroglyphs_2:: @ 0812344C
	.include "animations/gSpriteAnimations_ShrinkingHieroglyphs_2.s"

gSpriteAnimations_ShrinkingHieroglyphs:: @ 08123474
	ptr gSpriteAnimations_ShrinkingHieroglyphs_0
	ptr gSpriteAnimations_ShrinkingHieroglyphs_1
	ptr gSpriteAnimations_ShrinkingHieroglyphs_2
	ptr 00000000
