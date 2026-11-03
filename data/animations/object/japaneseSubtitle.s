	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_JapaneseSubtitle_0:: @ 08124B38
	.include "animations/gSpriteAnimations_JapaneseSubtitle_0.s"

gSpriteAnimations_JapaneseSubtitle_1:: @ 08124BA9
	.include "animations/gSpriteAnimations_JapaneseSubtitle_1.s"

gSpriteAnimations_JapaneseSubtitle:: @ 08124BFC
	ptr gSpriteAnimations_JapaneseSubtitle_0
	ptr gSpriteAnimations_JapaneseSubtitle_1
	ptr 00000000
