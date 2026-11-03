	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Carlov_0_0:: @ 08113258
	.include "animations/gSpriteAnimations_Carlov_0_0.s"

gSpriteAnimations_Carlov_0_8:: @ 0811325D
	.include "animations/gSpriteAnimations_Carlov_0_8.s"

gSpriteAnimations_Carlov_0_9:: @ 0811326E
	.include "animations/gSpriteAnimations_Carlov_0_9.s"

gSpriteAnimations_Carlov_0_10:: @ 0811329F
	.include "animations/gSpriteAnimations_Carlov_0_10.s"

gSpriteAnimations_Carlov_0:: @ 081132E4
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_8
	ptr gSpriteAnimations_Carlov_0_9
	ptr gSpriteAnimations_Carlov_0_10
	ptr 00000000

gSpriteAnimations_Carlov_1:: @ 08113314
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_8
	ptr gSpriteAnimations_Carlov_0_9
	ptr gSpriteAnimations_Carlov_0_10
	ptr gSpriteAnimations_Carlov_0_0
	ptr gSpriteAnimations_Carlov_0_8
	ptr gSpriteAnimations_Carlov_0_9
	ptr gSpriteAnimations_Carlov_0_10
	ptr 00000000
