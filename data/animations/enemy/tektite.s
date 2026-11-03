	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2



gSpriteAnimations_Tektite_0:: @ 080CDEFC
	.include "animations/gSpriteAnimations_Tektite_0.s"

gSpriteAnimations_Tektite_1:: @ 080CDF09
	.include "animations/gSpriteAnimations_Tektite_1.s"

gSpriteAnimations_Tektite_2:: @ 080CDF1A
	.include "animations/gSpriteAnimations_Tektite_2.s"

gSpriteAnimations_Tektite_3:: @ 080CDF27
	.include "animations/gSpriteAnimations_Tektite_3.s"

gSpriteAnimations_Tektite_4:: @ 080CDF33
	.include "animations/gSpriteAnimations_Tektite_4.s"

gSpriteAnimations_Tektite:: @ 080CDF38
	ptr gSpriteAnimations_Tektite_0
	ptr gSpriteAnimations_Tektite_1
	ptr gSpriteAnimations_Tektite_2
	ptr gSpriteAnimations_Tektite_3
	ptr gSpriteAnimations_Tektite_4
	ptr 00000000
