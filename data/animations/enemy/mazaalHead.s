	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_MazaalHead_0:: @ 080CEDB0
	.include "animations/gSpriteAnimations_MazaalHead_0.s"

gSpriteAnimations_MazaalHead_1:: @ 080CEDB5
	.include "animations/gSpriteAnimations_MazaalHead_1.s"

gSpriteAnimations_MazaalHead_2:: @ 080CEDBA
	.include "animations/gSpriteAnimations_MazaalHead_2.s"

gSpriteAnimations_MazaalHead_3:: @ 080CEDCF
	.include "animations/gSpriteAnimations_MazaalHead_3.s"

gSpriteAnimations_MazaalHead_4:: @ 080CEE07
	.include "animations/gSpriteAnimations_MazaalHead_4.s"

gSpriteAnimations_MazaalHead_5:: @ 080CEE3F
	.include "animations/gSpriteAnimations_MazaalHead_5.s"

gSpriteAnimations_MazaalHead_6:: @ 080CEE57
	.include "animations/gSpriteAnimations_MazaalHead_6.s"

gSpriteAnimations_MazaalHead:: @ 080CEE70
	ptr gSpriteAnimations_MazaalHead_0
	ptr gSpriteAnimations_MazaalHead_1
	ptr gSpriteAnimations_MazaalHead_2
	ptr gSpriteAnimations_MazaalHead_3
	ptr gSpriteAnimations_MazaalHead_4
	ptr gSpriteAnimations_MazaalHead_5
	ptr gSpriteAnimations_MazaalHead_6
	ptr 00000000
