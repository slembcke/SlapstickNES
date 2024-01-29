.macro inclz4 symbol, file
	.export symbol
	symbol:
		.incbin file, 8
		.word 0 ; terminator
.endmacro

.rodata

.segment "PRG0"

inclz4 _CHRBG, "chr/bgchr.lz4"
inclz4 _CHRSM, "chr/smchr.lz4"

inclz4 _MAP0, "map/map0.lz4"
inclz4 _MAP1, "map/map1.lz4"
