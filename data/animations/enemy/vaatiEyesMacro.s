	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2



gSpriteAnimations_VaatiEyesMacro_0:: @ 080CDEA0
	.include "animations/gSpriteAnimations_VaatiEyesMacro_0.s"

gSpriteAnimations_VaatiEyesMacro_1:: @ 080CDEB1
	.include "animations/gSpriteAnimations_VaatiEyesMacro_1.s"

gSpriteAnimations_VaatiEyesMacro:: @ 080CDEC4
	ptr gSpriteAnimations_VaatiEyesMacro_0
	ptr gSpriteAnimations_VaatiEyesMacro_1
	ptr 00000000
