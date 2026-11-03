	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

	.include "asm/macros/gfx.inc"

	.include "gfx_offsets.inc"

@ helper variable
.ifdef JP
	JP_D::
.endif
.ifdef DEMO_JP
	JP_D::
.endif


gGfxGroup_1::
	gfx_raw src=offset_gGfx_1_0, unknown=0x7, dest=0x6000000, size=0xf60
	gfx_raw src=offset_gGfx_1_1, unknown=0x7, dest=0x600e800, size=0x500, terminator=1

gGfxGroup_2::
	gfx_raw src=offset_gGfx_2_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_2_1, unknown=0x7, dest=0x600f000, size=0x500
	gfx_raw src=offset_gGfx_2_2, unknown=0x7, dest=0x600e800, size=0x500
.ifdef JP_D
	gfx_raw src=offset_gGfx_2_3_JP, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_2_4_JP, dest=0x600e000, size=0x500
	gfx_raw src=offset_gGfx_2_5_JP, dest=0x6006000, size=0x500
.else
	gfx_raw src=offset_gGfx_2_3, unknown=0xf, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_2_4, unknown=0xf, dest=0x600e000, size=0x400
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_3::
.ifdef EU
	gfx_raw src=offset_gGfx_3_0, unknown=0x2, dest=0x6010000, size=0x300
	gfx_raw src=offset_gGfx_3_1, unknown=0x3, dest=0x6010000, size=0x300
	gfx_raw src=offset_gGfx_3_2, unknown=0x4, dest=0x6010000, size=0x300
	gfx_raw src=offset_gGfx_3_3, unknown=0x5, dest=0x6010000, size=0x300
	gfx_raw src=offset_gGfx_3_4, unknown=0x6, dest=0x6010000, size=0x300
	gfx_raw src=offset_gGfx_3_5, unknown=0xe, dest=0x6010400, size=0x400
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_3_0, dest=0x6010000, size=0x300
	gfx_raw src=offset_gGfx_3_5, dest=0x6010400, size=0x400
.else
	gfx_raw src=offset_gGfx_3_0, unknown=0x1, dest=0x6010000, size=0x300
	gfx_raw src=offset_gGfx_3_7, unknown=0x1, dest=0x6010400, size=0x400
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_4::
	gfx_raw src=offset_gGfx_4_0, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_128_1, unknown=0x7, dest=0x6004000, size=0x19e0
	gfx_raw src=offset_gGfx_4_2, unknown=0x7, dest=0x6014000, size=0xe00, terminator=1

gGfxGroup_5::
	gfx_raw src=offset_gGfx_5_0, unknown=0x7, dest=0x6004000, size=0x1be0
	gfx_raw src=offset_gGfx_5_1, unknown=0x7, dest=0x6014800, size=0xfa0
	gfx_raw src=offset_gGfx_5_2, unknown=0x7, dest=0x6008000, size=0x31a0
	gfx_raw src=offset_gGfx_5_3, unknown=0x7, dest=0x600f000, size=0x500, terminator=1

gGfxGroup_6::
.ifdef EU
	gfx_raw src=offset_gGfx_6_0, unknown=0x2, dest=0x6015800, size=0x2000
	gfx_raw src=offset_gGfx_6_1, unknown=0x3, dest=0x6015800, size=0x2000
	gfx_raw src=offset_gGfx_6_2, unknown=0x4, dest=0x6015800, size=0x2000
	gfx_raw src=offset_gGfx_6_3, unknown=0x5, dest=0x6015800, size=0x2000
	gfx_raw src=offset_gGfx_6_4, unknown=0x6, dest=0x6015800, size=0x2000
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_6_0_JP, dest=0x6015800, size=0x2000
.else
	gfx_raw src=offset_gGfx_6_0, unknown=0x1, dest=0x6015800, size=0x2000
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_7::
	gfx_raw src=offset_gGfx_7_0, unknown=0x7, dest=gBG2Buffer, size=0x500, terminator=1

gGfxGroup_8::
	gfx_raw src=offset_gGfx_8_0, unknown=0x7, dest=gBG2Buffer, size=0x500, terminator=1

gGfxGroup_12::
	gfx_raw src=offset_gGfx_12_0, unknown=0x7, dest=gBG1Buffer+0xC0, size=0x400
	gfx_raw src=offset_gGfx_12_1, unknown=0x7, dest=gBG2Buffer+0xC0, size=0x400, terminator=1

gGfxGroup_13::
	gfx_raw src=offset_gGfx_13_0, unknown=0x7, dest=gBG1Buffer+0x100, size=0x400
	gfx_raw src=offset_gGfx_13_1, unknown=0x7, dest=gBG2Buffer+0x100, size=0x400, terminator=1

gGfxGroup_14::
	gfx_raw src=offset_gGfx_14_0, unknown=0x7, dest=gBG2Buffer, size=0x500, terminator=1

gGfxGroup_15::
.ifdef EU
	gfx_raw src=offset_gGfx_15_0, unknown=0x2, dest=0x6010800, size=0x1600
	gfx_raw src=offset_gGfx_15_1, unknown=0x3, dest=0x6010800, size=0x1600
	gfx_raw src=offset_gGfx_15_2, unknown=0x4, dest=0x6010800, size=0x1600
	gfx_raw src=offset_gGfx_15_3, unknown=0x5, dest=0x6010800, size=0x1600
	gfx_raw src=offset_gGfx_15_4, unknown=0x6, dest=0x6010800, size=0x1600
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_15_0_JP, dest=0x6010800, size=0x1600
.else
	gfx_raw src=offset_gGfx_15_0, unknown=0x1, dest=0x6010800, size=0x1600
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_9::
.ifdef EU
	gfx_raw src=offset_gGfx_9_0, unknown=0xe, dest=gBG3Buffer, size=0x500
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_9_0_JP, dest=gBG3Buffer, size=0x500
.else
	gfx_raw src=offset_gGfx_9_0, unknown=0x1, dest=gBG3Buffer, size=0x500
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_10::
.ifdef EU
	gfx_raw src=offset_gGfx_10_0, unknown=0xe, dest=gBG3Buffer, size=0x500
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_10_0_JP, dest=gBG3Buffer, size=0x500
.else
	gfx_raw src=offset_gGfx_10_0, unknown=0x1, dest=gBG3Buffer, size=0x500
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_11::
.ifdef JP_D
	gfx_raw src=offset_gGfx_11_1_JP, dest=gBG3Buffer, size=0x500
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_16::
	gfx_raw src=offset_gGfx_88_3, unknown=0x7, dest=0x6012000, size=0x1c0
	gfx_raw src=offset_gGfx_89_2, unknown=0x7, dest=0x600c000, size=0xe00, terminator=1

gGfxGroup_23::
	gfx_raw src=offset_gGfx_89_3, unknown=0x7, dest=0x6010000, size=0x2000, terminator=1

gGfxGroup_24::
	gfx_raw src=offset_gGfx_24_0, unknown=0x7, dest=0x6011800, size=0x100, terminator=1

gGfxGroup_25::
	gfx_raw src=offset_gGfx_25_0, unknown=0x7, dest=0x6011800, size=0x100, terminator=1

gGfxGroup_26::
	gfx_raw src=offset_gGfx_26_0, unknown=0x7, dest=0x6011c80, size=0x100, terminator=1

gGfxGroup_27::
	gfx_raw src=offset_gGfx_27_0, unknown=0x7, dest=0x6011aa0, size=0x80, terminator=1

gGfxGroup_28::
	gfx_raw src=offset_gGfx_28_0, unknown=0x7, dest=0x6011aa0, size=0x80, terminator=1

gGfxGroup_29::
	gfx_raw src=offset_gGfx_29_0, unknown=0x7, dest=0x6011920, size=0x180, terminator=1

gGfxGroup_36::
	gfx_raw src=offset_gGfx_36_0, unknown=0x7, dest=0x600a800, size=0x800
	gfx_raw src=offset_gGfx_36_1, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_37::
	gfx_raw src=offset_gGfx_38_0, unknown=0x7, dest=0x600a800, size=0x800
	gfx_raw src=offset_gGfx_37_1, unknown=0x7, dest=gBG3Buffer, size=0x1000, terminator=1

gGfxGroup_38::
	gfx_raw src=offset_gGfx_38_0, unknown=0x7, dest=0x600a400, size=0x800, terminator=1

gGfxGroup_39::
	gfx_raw src=offset_gGfx_39_0, unknown=0x7, dest=0x6000000, size=0xf000
	gfx_raw src=offset_gGfx_39_1, unknown=0x7, dest=0x600f000, size=0x1000, terminator=1

gGfxGroup_40::
	gfx_raw src=offset_gGfx_40_0, unknown=0x7, dest=0x6000000, size=0xf000
	gfx_raw src=offset_gGfx_40_1, unknown=0x7, dest=0x600f000, size=0x1000, terminator=1

gGfxGroup_41::
	gfx_raw src=offset_gGfx_41_0, unknown=0x7, dest=0x6000000, size=0xf000
	gfx_raw src=offset_gGfx_41_1, unknown=0x7, dest=0x600f000, size=0x1000, terminator=1

gGfxGroup_42::
	gfx_raw src=offset_gGfx_42_0, unknown=0x7, dest=0x6000000, size=0xf000
	gfx_raw src=offset_gGfx_42_1, unknown=0x7, dest=0x600f000, size=0x1000, terminator=1

gGfxGroup_30::
	gfx_raw src=offset_gGfx_33_0, unknown=0x7, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_30_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000
	gfx_raw src=offset_gGfx_30_2, unknown=0x7, dest=gMapDataTopSpecial+0x1000, size=0x1000
	gfx_raw src=offset_gGfx_30_3, unknown=0x7, dest=gMapDataTopSpecial+0x2000, size=0x1000
	gfx_raw src=offset_gGfx_30_4, unknown=0x7, dest=gMapDataTopSpecial+0x3000, size=0x1000, terminator=1

gGfxGroup_33::
	gfx_raw src=offset_gGfx_33_0, unknown=0x7, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_33_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000
	gfx_raw src=offset_gGfx_33_2, unknown=0x7, dest=gMapDataTopSpecial+0x1000, size=0x1000
	gfx_raw src=offset_gGfx_33_3, unknown=0x7, dest=gMapDataTopSpecial+0x2000, size=0x1000
	gfx_raw src=offset_gGfx_33_4, unknown=0x7, dest=gMapDataTopSpecial+0x3000, size=0x1000, terminator=1

gGfxGroup_32::
	gfx_raw src=offset_gGfx_33_0, unknown=0x7, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_32_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000
	gfx_raw src=offset_gGfx_32_2, unknown=0x7, dest=gMapDataTopSpecial+0x1000, size=0x1000
	gfx_raw src=offset_gGfx_32_3, unknown=0x7, dest=gMapDataTopSpecial+0x2000, size=0x1000
	gfx_raw src=offset_gGfx_32_4, unknown=0x7, dest=gMapDataTopSpecial+0x3000, size=0x1000, terminator=1

gGfxGroup_31::
	gfx_raw src=offset_gGfx_33_0, unknown=0x7, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_31_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000
	gfx_raw src=offset_gGfx_31_2, unknown=0x7, dest=gMapDataTopSpecial+0x1000, size=0x1000
	gfx_raw src=offset_gGfx_31_3, unknown=0x7, dest=gMapDataTopSpecial+0x2000, size=0x1000
	gfx_raw src=offset_gGfx_31_4, unknown=0x7, dest=gMapDataTopSpecial+0x3000, size=0x1000, terminator=1

gGfxGroup_35::
	gfx_raw src=offset_gGfx_35_0, unknown=0x7, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_35_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000
	gfx_raw src=offset_gGfx_35_2, unknown=0x7, dest=gMapDataTopSpecial+0x1000, size=0x1000
	gfx_raw src=offset_gGfx_35_3, unknown=0x7, dest=gMapDataTopSpecial+0x2000, size=0x1000
	gfx_raw src=offset_gGfx_35_4, unknown=0x7, dest=gMapDataTopSpecial+0x3000, size=0x1000, terminator=1

gGfxGroup_34::
	gfx_raw src=offset_gGfx_34_0, unknown=0x7, dest=0x6008000, size=0x4000
	gfx_raw src=offset_gGfx_34_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000
	gfx_raw src=offset_gGfx_34_2, unknown=0x7, dest=gMapDataTopSpecial+0x1000, size=0x1000
	gfx_raw src=offset_gGfx_34_3, unknown=0x7, dest=gMapDataTopSpecial+0x2000, size=0x1000
	gfx_raw src=offset_gGfx_34_4, unknown=0x7, dest=gMapDataTopSpecial+0x3000, size=0x1000, terminator=1

gGfxGroup_22::
	gfx_raw src=offset_gGfx_22_0, unknown=0x7, dest=0x6000000, size=0x4000
	gfx_raw src=offset_gGfx_22_1, unknown=0x7, dest=0x600e000, size=0x1000
	gfx_raw src=offset_gGfx_22_2, unknown=0x7, dest=0x6004000, size=0x2000
	gfx_raw src=offset_gGfx_22_3, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_17::
	gfx_raw src=offset_fixedTypeGfx_36, unknown=0x7, dest=0x6013800, size=0x1400
	gfx_raw src=offset_gGfx_17_1, unknown=0x7, dest=0x6014c00, size=0x1ae0, terminator=1

gGfxGroup_18::
	gfx_raw src=offset_gGfx_18_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_19::
	gfx_raw src=offset_gGfx_19_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_43::
	gfx_raw src=offset_gGfx_43_0, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_44::
	gfx_raw src=offset_gGfx_44_0, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_45::
	gfx_raw src=offset_gGfx_45_0, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_46::
	gfx_raw src=offset_gGfx_46_0, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_20::
	gfx_raw src=offset_gGfx_20_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_21::
	gfx_raw src=offset_gGfx_21_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_47::
	gfx_raw src=offset_gGfx_47_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_48::
	gfx_raw src=offset_gGfx_48_0, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000, terminator=1

gGfxGroup_49::
	gfx_raw src=offset_gGfx_49_0, unknown=0x7, dest=gBG3Buffer, size=0x800, terminator=1

gGfxGroup_50::
	gfx_raw src=offset_gGfx_50_0, unknown=0x7, dest=gBG3Buffer, size=0x800, terminator=1

gGfxGroup_51::
	gfx_raw src=offset_gGfx_51_0, unknown=0x7, dest=gBG3Buffer, size=0x800, terminator=1

gGfxGroup_52::
	gfx_raw src=offset_gGfx_52_0, unknown=0x7, dest=gBG3Buffer, size=0x1000, terminator=1

gGfxGroup_53::
	gfx_raw src=offset_gGfx_53_0, unknown=0x7, dest=gBG3Buffer, size=0x1000, terminator=1

gGfxGroup_75::
	gfx_raw src=offset_gGfx_75_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_76::
	gfx_raw src=offset_gGfx_76_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_77::
	gfx_raw src=offset_gGfx_77_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_78::
	gfx_raw src=offset_gGfx_78_0, unknown=0x7, dest=0x600f000, size=0x500, terminator=1

gGfxGroup_79::
	gfx_raw src=offset_gGfx_79_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_80::
	gfx_raw src=offset_gGfx_80_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_81::
	gfx_raw src=offset_gGfx_82_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_82::
	gfx_raw src=offset_gGfx_82_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_83::
	gfx_raw src=offset_gGfx_83_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_84::
	gfx_raw src=offset_gGfx_84_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_85::
	gfx_raw src=offset_gGfx_85_0, unknown=0x7, dest=0x600f000, size=0x800, terminator=1

gGfxGroup_54::
	gfx_raw src=offset_gGfx_54_0, unknown=0x7, dest=0x6004000, size=0x48c0
	gfx_raw src=offset_gGfx_54_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000, terminator=1

gGfxGroup_55::
	gfx_raw src=offset_gGfx_55_0, unknown=0x7, dest=0x6004000, size=0x47c0
	gfx_raw src=offset_gGfx_55_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000, terminator=1

gGfxGroup_56::
	gfx_raw src=offset_gGfx_56_0, unknown=0x7, dest=0x6004000, size=0x3be0
	gfx_raw src=offset_gGfx_56_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000, terminator=1

gGfxGroup_57::
	gfx_raw src=offset_gGfx_57_0, unknown=0x7, dest=0x6004000, size=0x4700
	gfx_raw src=offset_gGfx_57_1, unknown=0x7, dest=gMapDataTopSpecial, size=0x1000, terminator=1

gGfxGroup_58::
	gfx_raw src=offset_gGfx_58_0, unknown=0x7, dest=0x6000000, size=0xc000
	gfx_raw src=offset_gGfx_58_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_59::
	gfx_raw src=offset_gGfx_59_0, unknown=0x7, dest=0x6000000, size=0xc000
	gfx_raw src=offset_gGfx_59_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_60::
	gfx_raw src=offset_gGfx_60_0, unknown=0x7, dest=0x6000000, size=0xc000
	gfx_raw src=offset_gGfx_60_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_61::
	gfx_raw src=offset_gGfx_61_0, unknown=0x7, dest=0x6000000, size=0xc000
	gfx_raw src=offset_gGfx_61_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_62::
	gfx_raw src=offset_gGfx_62_0, unknown=0x7, dest=0x6000000, size=0xc000
	gfx_raw src=offset_gGfx_62_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_63::
	gfx_raw src=offset_gGfx_63_0, unknown=0x7, dest=0x6000000, size=0xc000
	gfx_raw src=offset_gGfx_63_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_64::
	gfx_raw src=offset_gGfx_64_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_64_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_65::
	gfx_raw src=offset_gGfx_65_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_65_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_66::
	gfx_raw src=offset_gGfx_66_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_66_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_67::
	gfx_raw src=offset_gGfx_67_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_67_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_68::
	gfx_raw src=offset_gGfx_68_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_68_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_69::
	gfx_raw src=offset_gGfx_69_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_69_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_70::
	gfx_raw src=offset_gGfx_70_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_70_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_71::
	gfx_raw src=offset_gGfx_71_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_71_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_72::
	gfx_raw src=offset_gGfx_72_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_72_1, unknown=0x7, dest=0x600e800, size=0x800, terminator=1

gGfxGroup_73::
	gfx_raw src=offset_gGfx_73_0, unknown=0x7, dest=0x600e6c0, size=0x140, terminator=1

gGfxGroup_74::
	gfx_raw src=offset_gGfx_74_0, unknown=0x7, dest=0x600e6c0, size=0x140, terminator=1

gGfxGroup_86::
	gfx_raw src=offset_gGfx_88_0, unknown=0x7, dest=0x6008000, size=0x1d20
	gfx_raw src=offset_gGfx_86_1, unknown=0x7, dest=gBG3Buffer, size=0x500
	gfx_raw src=offset_gGfx_88_2, unknown=0x7, dest=0x6010000, size=0x700
	gfx_raw src=offset_gGfx_88_3, unknown=0x7, dest=0x6012000, size=0x1c0, terminator=1

gGfxGroup_87::
	gfx_raw src=offset_gGfx_88_0, unknown=0x7, dest=0x6008000, size=0x1d20
	gfx_raw src=offset_gGfx_87_1, unknown=0x7, dest=gBG3Buffer, size=0x500
	gfx_raw src=offset_gGfx_88_2, unknown=0x7, dest=0x6010000, size=0x700
	gfx_raw src=offset_gGfx_88_3, unknown=0x7, dest=0x6012000, size=0x1c0, terminator=1

gGfxGroup_88::
	gfx_raw src=offset_gGfx_88_0, unknown=0x7, dest=0x6008000, size=0x1d20
	gfx_raw src=offset_gGfx_88_1, unknown=0x7, dest=gBG3Buffer, size=0x500
	gfx_raw src=offset_gGfx_88_2, unknown=0x7, dest=0x6010000, size=0x700
	gfx_raw src=offset_gGfx_88_3, unknown=0x7, dest=0x6012000, size=0x1c0, terminator=1

gGfxGroup_89::
.ifdef JP_D
	gfx_raw src=offset_gGfx_89_0_JP, dest=gBG2Buffer, size=0x500
.else
	gfx_raw src=offset_gGfx_89_0, unknown=0xf, dest=gBG2Buffer, size=0x500
.endif
	gfx_raw src=offset_gGfx_89_1, unknown=0x7, dest=0x6000000, size=0x3bc0
	gfx_raw src=offset_gGfx_89_2, unknown=0x7, dest=0x6008000, size=0xe00
	gfx_raw src=offset_gGfx_89_3, unknown=0x7, dest=0x6010000, size=0x2000
	gfx_raw src=offset_gGfx_89_4, unknown=0x7, dest=0x6013000, size=0x640
	gfx_raw src=offset_RupeeKeyDigitsYellow, unknown=0x7, dest=0x6013c00, size=0x280
	gfx_raw src=offset_gGfx_89_6, unknown=0x7, dest=0x6014000, size=0x8c0, terminator=1

gGfxGroup_90::
	gfx_raw src=offset_gGfx_90_0, unknown=0x7, dest=0x6000000, size=0x2640
	gfx_raw src=offset_gGfx_90_1, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_90_2, unknown=0x7, dest=0x6014000, size=0x200
.ifdef EU
	gfx_raw src=offset_gGfx_90_3, unknown=0x2, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_4, unknown=0x2, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_90_5, unknown=0x3, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_6, unknown=0x3, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_90_7, unknown=0x4, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_8, unknown=0x4, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_90_9, unknown=0x5, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_10, unknown=0x5, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_90_11, unknown=0x6, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_12, unknown=0x6, dest=0x6015800, size=0x700
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_90_3_JP, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_4_JP, dest=0x6015800, size=0x700
.else
	gfx_raw src=offset_gGfx_90_3, unknown=0x1, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_4, unknown=0x1, dest=0x6015800, size=0x700
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_91::
	gfx_raw src=offset_gGfx_91_0, unknown=0x7, dest=0x6000000, size=0x3300
	gfx_raw src=offset_gGfx_91_1, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_91_2, unknown=0x7, dest=0x6014000, size=0x1780
.ifdef EU
	gfx_raw src=offset_gGfx_91_3, unknown=0x2, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_4, unknown=0x2, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_91_5, unknown=0x3, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_6, unknown=0x3, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_91_7, unknown=0x4, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_8, unknown=0x4, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_91_9, unknown=0x5, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_10, unknown=0x5, dest=0x6015800, size=0x700
	gfx_raw src=offset_gGfx_91_11, unknown=0x6, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_12, unknown=0x6, dest=0x6015800, size=0x700
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_91_3_JP, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_4_JP, dest=0x6015800, size=0x700
.else
	gfx_raw src=offset_gGfx_91_3, unknown=0x1, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_91_4, unknown=0x1, dest=0x6015800, size=0x700
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_92::
	gfx_raw src=offset_gGfx_92_0, unknown=0x7, dest=0x6000000, size=0x2000
	gfx_raw src=offset_gGfx_92_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_92_2, unknown=0x7, dest=gBG2Buffer, size=0x500
.ifdef EU
	gfx_raw src=offset_gGfx_92_3, unknown=0x2, dest=0x6014800, size=0x800
	gfx_raw src=offset_gGfx_92_4_EU, unknown=0x3, dest=0x6014800, size=0x800
	gfx_raw src=offset_gGfx_92_5_EU, unknown=0x4, dest=0x6014800, size=0x800
	gfx_raw src=offset_gGfx_92_6, unknown=0x5, dest=0x6014800, size=0x800
	gfx_raw src=offset_gGfx_92_7, unknown=0x6, dest=0x6014800, size=0x800
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_92_3_JP, dest=0x6014800, size=0x800
.else
	gfx_raw src=offset_gGfx_92_3, unknown=0x1, dest=0x6014800, size=0x800
.endif
.endif
	gfx_raw src=offset_gGfx_92_4, unknown=0x7, dest=0x6014000, size=0x700
	gfx_raw src=offset_fixedTypeGfx_521, unknown=0x7, dest=0x6017000, size=0x1c0, terminator=1

gGfxGroup_129::
.ifdef EU
	gfx_raw src=offset_gGfx_129_0, unknown=0x2, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_129_1, unknown=0x3, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_129_2, unknown=0x4, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_129_3, unknown=0x5, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_129_4, unknown=0x6, dest=0x6010800, size=0x800
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_129_0_JP, dest=0x6010800, size=0x800
.else
	gfx_raw src=offset_gGfx_129_0, unknown=0x1, dest=0x6010800, size=0x800
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_127::
.ifdef EU
	gfx_raw src=offset_gGfx_127_0, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_128_1, unknown=0x7, dest=0x6004000, size=0x19e0, terminator=1
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_127_0_JP, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_127_1_JP, dest=0x6004000, size=0x2000
.else
	gfx_raw src=offset_gGfx_127_0, unknown=0xf, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_128_1, unknown=0xf, dest=0x6004000, size=0x19e0
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1
.endif

gGfxGroup_128::
.ifdef EU
	gfx_raw src=offset_gGfx_128_0, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_128_1, unknown=0x7, dest=0x6004000, size=0x19e0, terminator=1
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_128_0_JP, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_127_1_JP, dest=0x6004000, size=0x2000
.else
	gfx_raw src=offset_gGfx_128_0, unknown=0xf, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_128_1, unknown=0xf, dest=0x6004000, size=0x19e0
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1
.endif

gGfxGroup_93::
	gfx_raw src=offset_gGfx_94_0, unknown=0x7, dest=0x6000000, size=0x4000
	gfx_raw src=offset_gGfx_94_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_94_2, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_93_3, unknown=0x7, dest=gBG3Buffer, size=0x500
	gfx_raw src=offset_gGfx_94_3, unknown=0x7, dest=0x6014000, size=0x1300, terminator=1

gGfxGroup_94::
	gfx_raw src=offset_gGfx_94_0, unknown=0x7, dest=0x6000000, size=0x4000
	gfx_raw src=offset_gGfx_94_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_94_2, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_94_3, unknown=0x7, dest=0x6014000, size=0x1300, terminator=1

gGfxGroup_95::
	gfx_raw src=offset_gGfx_96_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_95_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_96::
	gfx_raw src=offset_gGfx_96_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_96_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_96_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_97::
	gfx_raw src=offset_gGfx_98_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_97_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_98::
	gfx_raw src=offset_gGfx_98_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_102_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_98_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_99::
	gfx_raw src=offset_gGfx_113_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_113_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_99_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_100::
	gfx_raw src=offset_gGfx_103_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_113_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_100_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_101::
	gfx_raw src=offset_gGfx_105_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_113_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_101_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_102::
	gfx_raw src=offset_gGfx_112_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_102_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_102_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_103::
	gfx_raw src=offset_gGfx_103_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_103_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_104::
	gfx_raw src=offset_gGfx_112_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_104_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_112::
	gfx_raw src=offset_gGfx_112_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_112_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_105::
	gfx_raw src=offset_gGfx_105_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_105_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_105_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_106::
	gfx_raw src=offset_gGfx_113_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_113_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_106_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_113::
	gfx_raw src=offset_gGfx_113_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_113_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_113_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_107::
	gfx_raw src=offset_gGfx_108_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_107_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_107_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_108::
	gfx_raw src=offset_gGfx_108_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_113_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_108_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_116::
	gfx_raw src=offset_gGfx_116_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_116_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_115::
	gfx_raw src=offset_gGfx_115_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_115_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_109::
	gfx_raw src=offset_gGfx_109_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_109_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_110::
	gfx_raw src=offset_gGfx_110_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_110_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_110_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_111::
	gfx_raw src=offset_gGfx_111_0, unknown=0x7, dest=0x6000000, size=0x8000
	gfx_raw src=offset_gGfx_116_1, unknown=0x7, dest=gBG1Buffer, size=0x500
	gfx_raw src=offset_gGfx_111_2, unknown=0x7, dest=gBG2Buffer, size=0x800
	gfx_raw src=offset_gGfx_116_3, unknown=0x7, dest=0x6014000, size=0xa00, terminator=1

gGfxGroup_117::
	gfx_raw src=offset_gGfx_117_0, unknown=0x7, dest=0x6000000, size=0x2a80
	gfx_raw src=offset_fixedTypeGfx_494, unknown=0x7, dest=0x6015400, size=0x600
	gfx_raw src=offset_gGfx_117_2, unknown=0x7, dest=gBG2Buffer, size=0x500
.ifdef EU
	gfx_raw src=offset_gGfx_117_3, unknown=0x2, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_117_4_EU, unknown=0x3, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_117_5, unknown=0x4, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_117_6, unknown=0x5, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_117_7, unknown=0x6, dest=0x6010800, size=0x840
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_117_3_JP, dest=0x6010800, size=0x800
.else
	gfx_raw src=offset_gGfx_117_3, unknown=0x1, dest=0x6010800, size=0x800
.endif
.endif
	gfx_raw src=offset_gGfx_117_4, unknown=0x7, dest=0x6011c00, size=0x80, terminator=1

gGfxGroup_118::
	gfx_raw src=offset_gGfx_118_0, unknown=0x7, dest=gBG1Buffer, size=0x500, terminator=1

gGfxGroup_119::
	gfx_raw src=offset_gGfx_119_0, unknown=0x7, dest=gBG1Buffer, size=0x500, terminator=1

gGfxGroup_120::
	gfx_raw src=offset_gGfx_120_0, unknown=0x7, dest=gBG1Buffer, size=0x500, terminator=1

gGfxGroup_121::
	gfx_raw src=offset_gGfx_121_0, unknown=0x7, dest=gBG1Buffer, size=0x500, terminator=1

gGfxGroup_122::
	gfx_raw src=offset_gGfx_122_0, unknown=0x7, dest=gBG1Buffer, size=0x500, terminator=1

gGfxGroup_123::
	gfx_raw src=offset_gGfx_123_0, unknown=0x7, dest=gBG1Buffer, size=0x500, terminator=1

gGfxGroup_124::
	gfx_raw src=offset_gGfx_124_0, unknown=0x7, dest=gBG1Buffer, size=0x500, terminator=1

gGfxGroup_125::
	gfx_raw src=offset_gGfx_125_0, unknown=0x7, dest=0x6000000, size=0x3800
	gfx_raw src=offset_gGfx_125_1, unknown=0x7, dest=gBG2Buffer, size=0x500
.ifdef EU
	gfx_raw src=offset_gGfx_125_2, unknown=0x2, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_125_3_EU, unknown=0x3, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_125_4, unknown=0x4, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_125_5, unknown=0x5, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_125_6, unknown=0x6, dest=0x6010800, size=0x800
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_125_2_JP, dest=0x6010800, size=0x800
.else
	gfx_raw src=offset_gGfx_125_2, unknown=0x1, dest=0x6010800, size=0x800
.endif
.endif
	gfx_raw src=offset_gGfx_125_3, unknown=0x7, dest=0x6011000, size=0x80, terminator=1

gGfxGroup_126::
	gfx_raw src=offset_gGfx_126_0, unknown=0x7, dest=0x6000000, size=0x3940
	gfx_raw src=offset_gGfx_126_1, unknown=0x7, dest=gBG2Buffer, size=0x500
	gfx_raw src=offset_gGfx_126_2, unknown=0x7, dest=0x6014000, size=0x3100
	gfx_raw src=offset_gGfx_126_3, unknown=0x7, dest=0x6013000, size=0x200
	gfx_raw src=offset_gGfx_126_4, unknown=0x7, dest=0x6013400, size=0x200
.ifdef EU
	gfx_raw src=offset_gGfx_126_5, unknown=0x2, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_126_6, unknown=0x3, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_126_7, unknown=0x4, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_126_8, unknown=0x5, dest=0x6010800, size=0x800
	gfx_raw src=offset_gGfx_126_9, unknown=0x6, dest=0x6010800, size=0x800
.else
.ifdef JP_D
	gfx_raw src=offset_gGfx_126_5_JP, dest=0x6010800, size=0x800
.else
	gfx_raw src=offset_gGfx_126_5, unknown=0x1, dest=0x6010800, size=0x800
.endif
.endif
	gfx_raw src=offset_gPalette_0, unknown=0xd, terminator=1

gGfxGroup_130::
	gfx_raw src=offset_gGfx_130_0, unknown=0x7, dest=0x600bf20, size=0xe0, compressed=1, terminator=1

gGfxGroup_131::
	gfx_raw src=offset_gGfx_131_0, unknown=0x7, dest=0x600bf20, size=0xe0, compressed=1, terminator=1

gGfxGroup_132::
	gfx_raw src=offset_gGfx_132_0, unknown=0x7, dest=0x6004000, size=0x100, compressed=1, terminator=1
gGfxGroups::
	ptr 0
	ptr gGfxGroup_1
	ptr gGfxGroup_2
	ptr gGfxGroup_3
	ptr gGfxGroup_4
	ptr gGfxGroup_5
	ptr gGfxGroup_6
	ptr gGfxGroup_7
	ptr gGfxGroup_8
	ptr gGfxGroup_9
	ptr gGfxGroup_10
	ptr gGfxGroup_11
	ptr gGfxGroup_12
	ptr gGfxGroup_13
	ptr gGfxGroup_14
	ptr gGfxGroup_15
	ptr gGfxGroup_16
	ptr gGfxGroup_17
	ptr gGfxGroup_18
	ptr gGfxGroup_19
	ptr gGfxGroup_20
	ptr gGfxGroup_21
	ptr gGfxGroup_22
	ptr gGfxGroup_23
	ptr gGfxGroup_24
	ptr gGfxGroup_25
	ptr gGfxGroup_26
	ptr gGfxGroup_27
	ptr gGfxGroup_28
	ptr gGfxGroup_29
	ptr gGfxGroup_30
	ptr gGfxGroup_31
	ptr gGfxGroup_32
	ptr gGfxGroup_33
	ptr gGfxGroup_34
	ptr gGfxGroup_35
	ptr gGfxGroup_36
	ptr gGfxGroup_37
	ptr gGfxGroup_38
	ptr gGfxGroup_39
	ptr gGfxGroup_40
	ptr gGfxGroup_41
	ptr gGfxGroup_42
	ptr gGfxGroup_43
	ptr gGfxGroup_44
	ptr gGfxGroup_45
	ptr gGfxGroup_46
	ptr gGfxGroup_47
	ptr gGfxGroup_48
	ptr gGfxGroup_49
	ptr gGfxGroup_50
	ptr gGfxGroup_51
	ptr gGfxGroup_52
	ptr gGfxGroup_53
	ptr gGfxGroup_54
	ptr gGfxGroup_55
	ptr gGfxGroup_56
	ptr gGfxGroup_57
	ptr gGfxGroup_58
	ptr gGfxGroup_59
	ptr gGfxGroup_60
	ptr gGfxGroup_61
	ptr gGfxGroup_62
	ptr gGfxGroup_63
	ptr gGfxGroup_64
	ptr gGfxGroup_65
	ptr gGfxGroup_66
	ptr gGfxGroup_67
	ptr gGfxGroup_68
	ptr gGfxGroup_69
	ptr gGfxGroup_70
	ptr gGfxGroup_71
	ptr gGfxGroup_72
	ptr gGfxGroup_73
	ptr gGfxGroup_74
	ptr gGfxGroup_75
	ptr gGfxGroup_76
	ptr gGfxGroup_77
	ptr gGfxGroup_78
	ptr gGfxGroup_79
	ptr gGfxGroup_80
	ptr gGfxGroup_81
	ptr gGfxGroup_82
	ptr gGfxGroup_83
	ptr gGfxGroup_84
	ptr gGfxGroup_85
	ptr gGfxGroup_86
	ptr gGfxGroup_87
	ptr gGfxGroup_88
	ptr gGfxGroup_89
	ptr gGfxGroup_90
	ptr gGfxGroup_91
	ptr gGfxGroup_92
	ptr gGfxGroup_93
	ptr gGfxGroup_94
	ptr gGfxGroup_95
	ptr gGfxGroup_96
	ptr gGfxGroup_97
	ptr gGfxGroup_98
	ptr gGfxGroup_99
	ptr gGfxGroup_100
	ptr gGfxGroup_101
	ptr gGfxGroup_102
	ptr gGfxGroup_103
	ptr gGfxGroup_104
	ptr gGfxGroup_105
	ptr gGfxGroup_106
	ptr gGfxGroup_107
	ptr gGfxGroup_108
	ptr gGfxGroup_109
	ptr gGfxGroup_110
	ptr gGfxGroup_111
	ptr gGfxGroup_112
	ptr gGfxGroup_113
	ptr gGfxGroup_109
	ptr gGfxGroup_115
	ptr gGfxGroup_116
	ptr gGfxGroup_117
	ptr gGfxGroup_118
	ptr gGfxGroup_119
	ptr gGfxGroup_120
	ptr gGfxGroup_121
	ptr gGfxGroup_122
	ptr gGfxGroup_123
	ptr gGfxGroup_124
	ptr gGfxGroup_125
	ptr gGfxGroup_126
	ptr gGfxGroup_127
	ptr gGfxGroup_128
	ptr gGfxGroup_129
	ptr gGfxGroup_130
	ptr gGfxGroup_131
	ptr gGfxGroup_132
