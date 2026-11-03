	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_Spark_0:: @ 080CD258
	.include "animations/gSpriteAnimations_Spark_0.s"

gSpriteAnimations_Spark:: @ 080CD274
	ptr gSpriteAnimations_Spark_0
	ptr 00000000
