	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Percy_0:: @ 08112E5C
	.include "animations/gSpriteAnimations_Percy_0.s"

gSpriteAnimations_Percy_4:: @ 08112EA5
	.include "animations/gSpriteAnimations_Percy_4.s"

gSpriteAnimations_Percy_7:: @ 08112EAA
	.include "animations/gSpriteAnimations_Percy_7.s"

gSpriteAnimations_Percy_5:: @ 08112EAF
	.include "animations/gSpriteAnimations_Percy_5.s"

gSpriteAnimations_Percy_8:: @ 08112EB4
	.include "animations/gSpriteAnimations_Percy_8.s"

gSpriteAnimations_Percy_12:: @ 08112F25
	.include "animations/gSpriteAnimations_Percy_12.s"

gSpriteAnimations_Percy_15:: @ 08112F2A
	.include "animations/gSpriteAnimations_Percy_15.s"

gSpriteAnimations_Percy_13:: @ 08112F2F
	.include "animations/gSpriteAnimations_Percy_13.s"

gSpriteAnimations_Percy:: @ 08112F34
	ptr gSpriteAnimations_Percy_0
	ptr gSpriteAnimations_Percy_0
	ptr gSpriteAnimations_Percy_0
	ptr gSpriteAnimations_Percy_0
	ptr gSpriteAnimations_Percy_4
	ptr gSpriteAnimations_Percy_5
	ptr gSpriteAnimations_Percy_4
	ptr gSpriteAnimations_Percy_7
	ptr gSpriteAnimations_Percy_8
	ptr gSpriteAnimations_Percy_8
	ptr gSpriteAnimations_Percy_8
	ptr gSpriteAnimations_Percy_8
	ptr gSpriteAnimations_Percy_12
	ptr gSpriteAnimations_Percy_13
	ptr gSpriteAnimations_Percy_12
	ptr gSpriteAnimations_Percy_15
	ptr 00000000
