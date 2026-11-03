	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Whirlwind_0:: @ 08122800
	.include "animations/gSpriteAnimations_Whirlwind_0.s"

gSpriteAnimations_Whirlwind_1:: @ 08122819
	.include "animations/gSpriteAnimations_Whirlwind_1.s"

gSpriteAnimations_Whirlwind:: @ 08122834
	ptr gSpriteAnimations_Whirlwind_0
	ptr gSpriteAnimations_Whirlwind_1
