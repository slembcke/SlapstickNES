.import FamiToneInit
.import FamiToneSfxInit
.import FamiToneUpdate
.import FamiToneMusicPlay
.import FamiToneMusicPause
.import FamiToneMusicStop
.import FamiToneSfxPlay

; .code

.export _music_init
.proc _music_init ; u16 addr
	; ax -> xy
	pha
	txa
	tay
	pla
	tax
	lda #1 ; TODO Hardcoded NTSC
	jsr FamiToneInit
	
	rts
.endproc

.export _sound_init
.proc _sound_init ; u16 addr
	; ax -> xy
	pha
	txa
	tay
	pla
	tax
	jsr FamiToneSfxInit
	
	rts
.endproc

.export _music_play = FamiToneMusicPlay
.export _music_pause = FamiToneMusicPause
.export _music_stop = FamiToneMusicStop
.export _sound_play = FamiToneSfxPlay

.export _px_nmi_callback = FamiToneUpdate

.rodata

.segment "PRG0"

FT_DPCM_PTR = (FT_DPCM_OFF & $3fff) >> 6
FT_DPCM_OFF:
	.incbin "danger_streets.dmc"

.export _MUSIC
_MUSIC:
	.include "danger_streets.s"

.export _SOUNDS
_SOUNDS:
	.include "sounds.s"
