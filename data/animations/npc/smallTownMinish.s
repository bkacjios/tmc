	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_SmallTownMinish_0:: @ 081144C8
	.include "animations/gSpriteAnimations_SmallTownMinish_0.s"

gSpriteAnimations_SmallTownMinish_1:: @ 081144CD
	.include "animations/gSpriteAnimations_SmallTownMinish_1.s"

gSpriteAnimations_SmallTownMinish_2:: @ 081144D2
	.include "animations/gSpriteAnimations_SmallTownMinish_2.s"

gSpriteAnimations_SmallTownMinish_3:: @ 081144D7
	.include "animations/gSpriteAnimations_SmallTownMinish_3.s"

gSpriteAnimations_SmallTownMinish:: @ 081144DC
	ptr gSpriteAnimations_SmallTownMinish_0
	ptr gSpriteAnimations_SmallTownMinish_1
	ptr gSpriteAnimations_SmallTownMinish_2
	ptr gSpriteAnimations_SmallTownMinish_3
	ptr 00000000
