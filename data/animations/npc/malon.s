	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Malon_0:: @ 0810FF70
	.include "animations/gSpriteAnimations_Malon_0.s"

gSpriteAnimations_Malon_1:: @ 0810FF75
	.include "animations/gSpriteAnimations_Malon_1.s"

gSpriteAnimations_Malon_2:: @ 0810FF7A
	.include "animations/gSpriteAnimations_Malon_2.s"

gSpriteAnimations_Malon_3:: @ 0810FF7F
	.include "animations/gSpriteAnimations_Malon_3.s"

gSpriteAnimations_Malon_5:: @ 0810FF84
	.include "animations/gSpriteAnimations_Malon_5.s"

gSpriteAnimations_Malon_6:: @ 0810FFA5
	.include "animations/gSpriteAnimations_Malon_6.s"

gSpriteAnimations_Malon_4:: @ 0810FFB6
	.include "animations/gSpriteAnimations_Malon_4.s"

gSpriteAnimations_Malon_12:: @ 0810FFD7
	.include "animations/gSpriteAnimations_Malon_12.s"

gSpriteAnimations_Malon_10:: @ 08110014
	.include "animations/gSpriteAnimations_Malon_10.s"

gSpriteAnimations_Malon:: @ 08110028
	ptr gSpriteAnimations_Malon_0
	ptr gSpriteAnimations_Malon_1
	ptr gSpriteAnimations_Malon_2
	ptr gSpriteAnimations_Malon_3
	ptr gSpriteAnimations_Malon_4
	ptr gSpriteAnimations_Malon_5
	ptr gSpriteAnimations_Malon_6
	ptr gSpriteAnimations_Malon_4
	ptr gSpriteAnimations_Malon_0
	ptr gSpriteAnimations_Malon_1
	ptr gSpriteAnimations_Malon_10
	ptr gSpriteAnimations_Malon_3
	ptr gSpriteAnimations_Malon_12
	ptr gSpriteAnimations_Malon_12
	ptr gSpriteAnimations_Malon_12
	ptr gSpriteAnimations_Malon_12
	ptr 00000000
