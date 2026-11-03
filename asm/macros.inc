@ An address that C reads as a pointer: 4 bytes, 8 in the 64-bit PC build.
	.macro ptr values:vararg
	.ifdef PLATFORM_PC64
	.quad \values
	.else
	.4byte \values
	.endif
	.endm

	.include "asm/macros/asm.inc"
	.include "asm/macros/function.inc"
	.include "asm/macros/ram.inc"
	.include "asm/macros/m4a.inc"
	.include "asm/macros/map.inc"
	.include "asm/macros/entity.inc"
	.include "asm/macros/music_voice.inc"
	.include "asm/macros/gfx.inc"
