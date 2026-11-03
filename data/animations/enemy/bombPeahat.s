	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_BombPeahat_0:: @ 080CD184
	.include "animations/gSpriteAnimations_BombPeahat_0.s"

gSpriteAnimations_BombPeahat_1:: @ 080CD195
	.include "animations/gSpriteAnimations_BombPeahat_1.s"

gSpriteAnimations_BombPeahat_2:: @ 080CD1D6
	.include "animations/gSpriteAnimations_BombPeahat_2.s"

gSpriteAnimations_BombPeahat_3:: @ 080CD217
	.include "animations/gSpriteAnimations_BombPeahat_3.s"

gSpriteAnimations_BombPeahat:: @ 080CD220
	ptr gSpriteAnimations_BombPeahat_0
	ptr gSpriteAnimations_BombPeahat_1
	ptr gSpriteAnimations_BombPeahat_2
	ptr gSpriteAnimations_BombPeahat_3
	ptr 00000000
