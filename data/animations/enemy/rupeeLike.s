	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_RupeeLike_0:: @ 080CCC4A
	.include "animations/gSpriteAnimations_RupeeLike_0.s"

gSpriteAnimations_RupeeLike_1:: @ 080CCC5F
	.include "animations/gSpriteAnimations_RupeeLike_1.s"

gSpriteAnimations_RupeeLike_2:: @ 080CCC74
	.include "animations/gSpriteAnimations_RupeeLike_2.s"

gSpriteAnimations_RupeeLike_3:: @ 080CCC9C
	.include "animations/gSpriteAnimations_RupeeLike_3.s"

gSpriteAnimations_RupeeLike_4:: @ 080CCCB4
	.include "animations/gSpriteAnimations_RupeeLike_4.s"

gSpriteAnimations_RupeeLike_5:: @ 080CCCC9
	.include "animations/gSpriteAnimations_RupeeLike_5.s"

gSpriteAnimations_RupeeLike_6:: @ 080CCCDE
	.include "animations/gSpriteAnimations_RupeeLike_6.s"

gSpriteAnimations_RupeeLike_7:: @ 080CCD06
	.include "animations/gSpriteAnimations_RupeeLike_7.s"

gSpriteAnimations_RupeeLike:: @ 080CCD20
	ptr gSpriteAnimations_RupeeLike_0
	ptr gSpriteAnimations_RupeeLike_1
	ptr gSpriteAnimations_RupeeLike_2
	ptr gSpriteAnimations_RupeeLike_3
	ptr gSpriteAnimations_RupeeLike_4
	ptr gSpriteAnimations_RupeeLike_5
	ptr gSpriteAnimations_RupeeLike_6
	ptr gSpriteAnimations_RupeeLike_7
	ptr 00000000
