	.macro song label, music_player, unknown
	ptr \label
	.2byte \music_player
	.2byte \unknown
	.ifdef PLATFORM_PC64
	.4byte 0 @ Song is 16 bytes
	.endif
	.endm

	.macro music_player info_struct, track_struct, unknown_1, unknown_2
	ptr \info_struct
	ptr \track_struct
	.byte \unknown_1
	.space 1
	.2byte \unknown_2
	.ifdef PLATFORM_PC64
	.4byte 0 @ MusicPlayer is 24 bytes
	.endif
	.endm
