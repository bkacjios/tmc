	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Din_0:: @ 0810F96C
	.include "animations/gSpriteAnimations_Din_0.s"

gSpriteAnimations_Din_3:: @ 0810F971
	.include "animations/gSpriteAnimations_Din_3.s"

gSpriteAnimations_Din_1:: @ 0810F976
	.include "animations/gSpriteAnimations_Din_1.s"

gSpriteAnimations_Din_4:: @ 0810F97B
	.include "animations/gSpriteAnimations_Din_4.s"

gSpriteAnimations_Din:: @ 0810F98C
	ptr gSpriteAnimations_Din_0
	ptr gSpriteAnimations_Din_1
	ptr gSpriteAnimations_Din_0
	ptr gSpriteAnimations_Din_3
	ptr gSpriteAnimations_Din_4
	ptr gSpriteAnimations_Din_4
	ptr gSpriteAnimations_Din_4
	ptr gSpriteAnimations_Din_4
	ptr 00000000
