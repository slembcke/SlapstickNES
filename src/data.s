.macro inclz4 symbol, file
	.export symbol
	symbol:
		.incbin file, 8
		.word 0 ; terminator
.endmacro

.rodata

inclz4 _CHRBG, "chr/bgchr.lz4"
inclz4 _CHRSM, "chr/smchr.lz4"

inclz4 _MAP_SPLASH, "map/splash.lz4"
inclz4 _MAP0, "map/map0.lz4"
