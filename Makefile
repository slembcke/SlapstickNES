PROJECT_NAME = Slapstick
ROM = $(PROJECT_NAME).nes

CC65_ROOT = "C:/Users/Mycroft/Desktop/GGJ202X/wintools/cc65"
CC = $(CC65_ROOT)/bin/cc65
AS = $(CC65_ROOT)/bin/ca65
LD = $(CC65_ROOT)/bin/ld65

CFLAGS += \
	-t nes -Oirs \
	--register-space 16 \
	-I ext/pixler/lib \

ASMINC = \
	-I ext/pixler/lib \

SRC = \
	src/main.c \

ASM = \
	src/data.s \
	src/misc.s \
	audio/audio.s \
	ext/famitone2/famitone2.s \

OBJS = \
	$(SRC:.c=.o) \
	$(ASM:.s=.o) \

CHR = \
	chr/bgchr.png \
	chr/smchr.png \

SONGS = \
	audio/after_the_rain.txt \

default: $(ROM)
rom: $(ROM)

PX_TOOLS_PATH = "C:/Users/Mycroft/Desktop/GGJ202X/wintools"
PX_LIB_PATH = ext/pixler/lib
PX_LIB = $(PX_LIB_PATH)/px.lib
$(PX_LIB):
	wintools/make CC65_ROOT=$(CC65_ROOT) -C $(PX_LIB_PATH)

px-tools:
# wintools/make -C $(PX_TOOLS_PATH)
# touch px-tools

FT2_TOOLS_PATH = "C:/Users/Mycroft/Desktop/GGJ202X/wintools"
ft2-tools:
# wintools/make -C $(FT2_TOOLS_PATH)
# touch ft2-tools

run-mac: rom
	open -a Nestopia $(ROM)

run-linux: rom
	~/Applications/Mesen $(ROM)

run-win: rom
	wintools/Mesen.exe $(ROM)

BIN = $(ROM:.nes=.bin)
$(BIN): $(ROM)
	dd if=$< ibs=1 skip=16 > $@	

romviz.png: $(BIN)
	$(PX_TOOLS_PATH)/chr2png "1D 16 1A 11" $(BIN) $@

$(ROM): ld65.cfg $(OBJS) $(PX_LIB)
	$(LD) -C ld65.cfg --dbgfile $(ROM:.nes=.dbg) $(OBJS) $(PX_LIB) nes.lib -m link.log -o $@

%.s: %.c
	$(CC) -g $(CFLAGS) $< --add-source $(INCLUDE) -o $@

%.s %.o: %.c
	$(CC65_ROOT)/bin/cl65 -c -g $(CFLAGS) $(INCLUDE) $< -o $@

%.o: %.s
	$(AS) -g $< $(ASMINC) -o $@

%.chr: %.png px-tools
	$(PX_TOOLS_PATH)/png2chr $< $@

%.lz4: %.chr px-tools
	$(PX_TOOLS_PATH)/lz4x -f9 $< $@
# touch $@

%.bin: %.tmx
	python $(PX_TOOLS_PATH)/tmx2bin.py $< $@

%.lz4: %.bin px-tools
	$(PX_TOOLS_PATH)/lz4x -f9 $< $@
# touch $@

src/data.o: $(CHR:.png=.lz4) map/splash.lz4 map/map0.lz4 map/map1.lz4 

tiles: chr/bgchr.chr
	$(PX_TOOLS_PATH)/chr2png "1D 01 11 21" $< $(<:.chr=-pal0.png)
	$(PX_TOOLS_PATH)/chr2png "1D 16 27 20" $< $(<:.chr=-pal1.png)
	$(PX_TOOLS_PATH)/chr2png "1D 16 24 20" $< $(<:.chr=-pal2.png)
	$(PX_TOOLS_PATH)/chr2png "1D 02 02 02" $< $(<:.chr=-pal3.png)

audio/sounds.s: audio/sounds.nsf ft2-tools
	$(FT2_TOOLS_PATH)/nsf2data $< -ca65 -ntsc

audio/%.s: audio/%.txt ft2-tools
	$(FT2_TOOLS_PATH)/text2data -ca65 $<

audio/audio.o: $(SONGS:.txt=.s) audio/sounds.s

clean:
	-rm $(OBJS) $(CHR:.png=.chr) $(CHR:.png=.lz4)
	-rm map/splash.bin map/splash.lz4
	-rm $(SONGS:.txt=.s)
	-rm px-tools
	make -C $(PX_TOOLS_PATH) clean
	make -C $(PX_LIB_PATH) clean
	-rm ft2-tools
	make -C $(FT2_TOOLS_PATH) clean

.phony: default rom tiles clean
