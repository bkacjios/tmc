	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_SittingPerson_0:: @ 0810CDA0
	.include "animations/gSpriteAnimations_SittingPerson_0.s"

gSpriteAnimations_SittingPerson_2:: @ 0810CDB1
	.include "animations/gSpriteAnimations_SittingPerson_2.s"

gSpriteAnimations_SittingPerson_3:: @ 0810CDC2
	.include "animations/gSpriteAnimations_SittingPerson_3.s"

gSpriteAnimations_SittingPerson:: @ 0810CDD4
	ptr gSpriteAnimations_SittingPerson_0
	ptr gSpriteAnimations_SittingPerson_0
	ptr gSpriteAnimations_SittingPerson_2
	ptr gSpriteAnimations_SittingPerson_3
	ptr gSpriteAnimations_SittingPerson_3
	ptr gSpriteAnimations_SittingPerson_3
	ptr gSpriteAnimations_SittingPerson_2
	ptr gSpriteAnimations_SittingPerson_0
	ptr 00000000
