	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_LikeLike_0:: @ 080CC734
	.include "animations/gSpriteAnimations_LikeLike_0.s"

gSpriteAnimations_LikeLike_1:: @ 080CC759
	.include "animations/gSpriteAnimations_LikeLike_1.s"

gSpriteAnimations_LikeLike_2:: @ 080CC76E
	.include "animations/gSpriteAnimations_LikeLike_2.s"

gSpriteAnimations_LikeLike:: @ 080CC780
	ptr gSpriteAnimations_LikeLike_0
	ptr gSpriteAnimations_LikeLike_1
	ptr gSpriteAnimations_LikeLike_2
	ptr 00000000
