	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

@ text.c
gTranslations:: @ 08109214
	ptr translation
	ptr translation
	ptr translation
.ifdef EU
	ptr translationFrench
	ptr translationGerman
	ptr translationSpanish
	ptr translationItalian
.else
	ptr translation
	ptr translation
	ptr translation
	ptr translation
.endif

@ text.c
gUnk_08109230:: @ 08109230
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_08109230.inc"
.else
	.incbin "data_08108E6C/gUnk_08109230.bin"
.endif

gUnk_08109244:: @ 08109244
	.incbin "data_08108E6C/gUnk_08109244.bin"

@ text.c
gUnk_08109248:: @ 08109248
	ptr gUnk_08692F60
	ptr gUnk_08692F60
	ptr gUnk_086978E0
	ptr gUnk_08696F60
	ptr gUnk_086AAEE0
	ptr gUnk_086998E0
	ptr gUnk_086A18E0
	ptr gUnk_086A2A60
	ptr gUnk_086A2EE0

gUnk_0810926C:: @ 0810926C
	.incbin "data_08108E6C/gUnk_0810926C.bin"

@ text.c
gUnk_081092AC:: @ 081092AC
	ptr gUnk_086926A0
	ptr gUnk_08692780
	ptr gUnk_08692860
	ptr gUnk_08692940
	ptr gUnk_08692A20
	ptr gUnk_08692B00
	ptr gUnk_08692BE0
	ptr gUnk_08692CC0
	ptr gUnk_08692DA0
	ptr gUnk_08692E80

gUnk_081092D4:: @ 081092D4
	.incbin "data_08108E6C/gUnk_081092D4.bin"

gUnk_0810942E:: @ 0810942E
	.incbin "data_08108E6C/gUnk_0810942E.bin"

@ text.c
gUnk_081094CE:: @ 081094CE
	.incbin "data_08108E6C/gUnk_081094CE.bin"
