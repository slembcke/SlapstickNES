#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"

#define BG_COLOR 0x31
static const u8 PALETTE[] = {
	BG_COLOR, 0x00, 0x10, 0x20,
	BG_COLOR, 0x06, 0x16, 0x26,
	BG_COLOR, 0x09, 0x19, 0x29,
	BG_COLOR, 0x01, 0x11, 0x21,
	
	BG_COLOR, 0x00, 0x10, 0x20,
	BG_COLOR, 0x06, 0x16, 0x26,
	BG_COLOR, 0x09, 0x19, 0x29,
	BG_COLOR, 0x01, 0x11, 0x21,
};

Gamepad pad1, pad2;

void read_gamepads(void){
	pad1.prev = pad1.value;
	pad1.value = joy_read(0);
	pad1.press = pad1.value & (pad1.value ^ pad1.prev);
	pad1.release = pad1.prev & (pad1.value ^ pad1.prev);
	
	pad2.prev = pad2.value;
	pad2.value = joy_read(1);
	pad2.press = pad2.value & (pad2.value ^ pad2.prev);
	pad2.release = pad2.prev & (pad2.value ^ pad2.prev);
}

void wait_noinput(void){
	while(joy_read(0) || joy_read(1)) px_wait_nmi();
}

static void darken(register const u8* palette, u8 shift){
	for(idx = 0; idx < 32; idx++){
		ix = palette[idx];
		ix -= shift << 4;
		if(ix > 0x40 || ix == 0x0D) ix = 0x1D;
		px_buffer_set_color(idx, ix);
	}
}

void fade_from_black(const u8* palette, u8 delay){
	darken(palette, 4);
	px_wait_frames(delay);
	darken(palette, 3);
	px_wait_frames(delay);
	darken(palette, 2);
	px_wait_frames(delay);
	darken(palette, 1);
	px_wait_frames(delay);
	darken(palette, 0);
}

void meta_spr(u8 x, u8 y, u8 pal, const u8* data);
static const u8 META_R1[] = {
	// HEAD
	-8, -16, 0xD0, 0,
	 0, -16, 0xD1, 0,
	// TORSO
	-8, -8, 0xE0, 0,
	 0, -8, 0xE1, 0,
	//LEGS
	-8,  0, 0xF0, 0,
	 0,  0, 0xF1, 0,
	128,
};
static const u8 META_R2[] = {
	// HEAD
	-8, -16, 0xD0, 0,
	 0, -16, 0xD1, 0,
	// TORSO
	 0, -8, 0xE0, PX_SPR_FLIPX,
	-8, -8, 0xE1, PX_SPR_FLIPX,
	//LEGS
	 0,  0, 0xF0, PX_SPR_FLIPX,
	-8,  0, 0xF1, PX_SPR_FLIPX,
	128,
};

static const u8 META_R1_HOLDING[] = {
	// HEAD
	-8, -16, 0xD2, 0,
	 0, -16, 0xD1, 0,
	// TORSO
	-8, -8, 0xE2, 0,
	 0, -8, 0xE1, 0,
	//LEGS
	-8,  0, 0xF0, 0,
	 0,  0, 0xF1, 0,
	128,
};
static const u8 META_R2_HOLDING[] = {
	// HEAD
	-8, -16, 0xD2, 0,
	 0, -16, 0xD1, 0,
	// TORSO
	-8, -8, 0xE2, 0,
	 0, -8, 0xE0, PX_SPR_FLIPX,
	//LEGS
	 0,  0, 0xF0, PX_SPR_FLIPX,
	-8,  0, 0xF1, PX_SPR_FLIPX,
	128,
};

static const u8 META_R1_THROWING[] = {
	// HEAD
	-8, -16, 0xD0, 0,
	 0, -16, 0xD1, 0,
	// TORSO
	-8, -8, 0xE3, 0,
	 0, -8, 0xE4, 0,
	//LEGS
	-8,  0, 0xF0, 0,
	 0,  0, 0xF1, 0,
	128,
};
static const u8 META_R2_THROWING[] = {
	// HEAD
	-8, -16, 0xD0, 0,
	 0, -16, 0xD1, 0,
	// TORSO
	-8, -8, 0xE3, 0,
	 0, -8, 0xE4, 0,
	//LEGS
	 0,  0, 0xF0, PX_SPR_FLIPX,
	-8,  0, 0xF1, PX_SPR_FLIPX,
	128,
};

static const u8 META_L1[] = {
	// HEAD
	 0, -16, 0xD0, PX_SPR_FLIPX,
	-8, -16, 0xD1, PX_SPR_FLIPX,
	// TORSO
	 0, -8, 0xE0, PX_SPR_FLIPX,
	-8, -8, 0xE1, PX_SPR_FLIPX,
	//LEGS
	 0,  0, 0xF0, PX_SPR_FLIPX,
	-8,  0, 0xF1, PX_SPR_FLIPX,
	128,
};
static const u8 META_L2[] = {
	// HEAD
	 0, -16, 0xD0, PX_SPR_FLIPX,
	-8, -16, 0xD1, PX_SPR_FLIPX,
	// TORSO
	-8, -8, 0xE0, 0,
	 0, -8, 0xE1, 0,
	//LEGS
	-8,  0, 0xF0, 0,
	 0,  0, 0xF1, 0,
	128,
};

static const u8 META_L1_HOLDING[] = {
	// HEAD
	 0, -16, 0xD2, PX_SPR_FLIPX,
	-8, -16, 0xD1, PX_SPR_FLIPX,
	// TORSO
	 0, -8, 0xE2, PX_SPR_FLIPX,
	-8, -8, 0xE1, PX_SPR_FLIPX,
	//LEGS
	 0,  0, 0xF0, PX_SPR_FLIPX,
	-8,  0, 0xF1, PX_SPR_FLIPX,
	128,
};
static const u8 META_L2_HOLDING[] = {
	// HEAD
	 0, -16, 0xD2, PX_SPR_FLIPX,
	-8, -16, 0xD1, PX_SPR_FLIPX,
	// TORSO
	-8, -8, 0xE0, 0,
	 0, -8, 0xE2, PX_SPR_FLIPX,
	//LEGS
	-8,  0, 0xF0, 0,
	 0,  0, 0xF1, 0,
	128,
};

static const u8 META_L1_THROWING[] = {
	// HEAD
	 0, -16, 0xD0, PX_SPR_FLIPX,
	-8, -16, 0xD1, PX_SPR_FLIPX,
	// TORSO
	 0, -8, 0xE3, PX_SPR_FLIPX,
	-8, -8, 0xE4, PX_SPR_FLIPX,
	//LEGS
	 0,  0, 0xF0, PX_SPR_FLIPX,
	-8,  0, 0xF1, PX_SPR_FLIPX,
	128,
};
static const u8 META_L2_THROWING[] = {
	// HEAD
	 0, -16, 0xD0, PX_SPR_FLIPX,
	-8, -16, 0xD1, PX_SPR_FLIPX,
	// TORSO
	-8, -8, 0xE4, PX_SPR_FLIPX,
	 0, -8, 0xE3, PX_SPR_FLIPX,
	//LEGS
	-8,  0, 0xF0, 0,
	 0,  0, 0xF1, 0,
	128,
};


static const u8* anim_walk_right[] = {
	META_R1,
	META_R2
};

static const u8* anim_walk_left[] = {
	META_L1,
	META_L2,
};


static const u8* anim_walk_right_holding[] = {
	META_R1_HOLDING,
	META_R2_HOLDING,
};

static const u8* anim_walk_left_holding[] = {
	META_L1_HOLDING,
	META_L2_HOLDING,
};


static const u8* anim_walk_right_throwing[] = {
	META_R1_THROWING,
	META_R2_THROWING,
};

static const u8* anim_walk_left_throwing[] = {
	META_L1_THROWING,
	META_L2_THROWING,
};


static const u8 HAMMER_UP[] = {
	 0, 0, 0xB0, 0,
	128,
};
static const u8 HAMMER_THROW[] = {
	 0, 0, 0xB1, 0,
	128,
};
static const u8 PIE_UP[] = {
	 0, 0, 0xC0, 0,
	128,
};
static const u8 PIE_THROW[] = {
	 0, 0, 0xC1, 0,
	128,
};
static const u8 BANANA_UP[] = {
	 0, 0, 0xB2, 0,
	128,
};
static const u8 BANANA_THROW[] = {
	 0, 0, 0xB3, 0,
	128,
};

static u8 pickupsX[4];
static u8 pickupsY[4];
static u8 pickupsType[4];

static void splash_screen(void){
	register u8 x = 32, y = 32;
	register s16 sin = 0, cos = 0x3FFF;
	bool P1walking = false;
	bool P1walkRight = true;
	bool P1holding = false;
	bool P1throw = false;
	u8 throwFrameTimer = 24;
	bool P1item = false;

	pickupsX[0] = 64;
	pickupsY[0] = 128;
	pickupsType[0] = 0;

	px_ppu_sync_disable();{
		// Load the splash tilemap into nametable 0.
		px_lz4_to_vram(NT_ADDR(0, 0, 0), MAP0);
	} px_ppu_sync_enable();
	
	// music_play(0);
	
	fade_from_black(PALETTE, 4);
	
	while(true){
		read_gamepads();
		P1walking = false;

		if (P1throw) {
			throwFrameTimer -= 1;
			if (throwFrameTimer == 1) {
				throwFrameTimer = 24;
				P1throw = false;		
				P1holding = false;
			}	
		}

		if(JOY_LEFT (pad1.value)) { x -= 1; P1walking = true; P1walkRight = false; } 
		if(JOY_RIGHT(pad1.value)) { x += 1; P1walking = true; P1walkRight = true; }
		if(JOY_DOWN (pad1.value)) { y += 1; P1walking = true; }
		if(JOY_UP   (pad1.value)) { y -= 1; P1walking = true; }
		if(JOY_BTN_A(pad1.press)) { sound_play(SOUND_JUMP); P1throw = true; }
		// if(JOY_BTN_B(pad1.press)) sound_play(SOUND_JUMP);

		

		if (P1throw) {
			if (P1walkRight) {
				meta_spr(x+8, y-8, 2, HAMMER_THROW);
			}
			else {
				meta_spr(x-16, y-8, 2|PX_SPR_FLIPX, HAMMER_THROW);
			}
		} 
		else if (P1holding) {
			if (P1walkRight) {
				meta_spr(x-8, y-24, 2, HAMMER_UP);
			}
			else {
				meta_spr(x, y-24, 2, HAMMER_UP);
			}
		}

		// Draw a sprite.
		if (P1walking) {
			if (P1walkRight) {
				if (P1throw) {
					meta_spr(x, y, 0, anim_walk_right_throwing[px_ticks/8%2]);
				}
				else if (P1holding) {
					meta_spr(x, y, 0, anim_walk_right_holding[px_ticks/8%2]);
				}
				else {
					meta_spr(x, y, 0, anim_walk_right[px_ticks/8%2]);
				}
			}
			else {
				if (P1throw) {
					meta_spr(x, y, 0, anim_walk_left_throwing[px_ticks/8%2]);
				}
				else if (P1holding) {
			 		meta_spr(x, y, 0, anim_walk_left_holding[px_ticks/8%2]);
				}
				else {
			 		meta_spr(x, y, 0, anim_walk_left[px_ticks/8%2]);
				}
			}
		}
		else {
			if (P1walkRight) {
				if (P1throw) {
					meta_spr(x, y, 0, META_R1_THROWING);
				}
				else if (P1holding) {
					meta_spr(x, y, 0, META_R1_HOLDING);
				}
				else {
					meta_spr(x, y, 0, META_R1);
				}
			}
			else {
				if (P1throw) {
					meta_spr(x, y, 0, META_L1_THROWING);
				}
				else if (P1holding) {
					meta_spr(x, y, 0, META_L1_HOLDING);
				}
				else {
					meta_spr(x, y, 0, META_L1);
				}
			}
		}
		
		// meta_spr(16, 16, 2, HAMMER_UP);
		// meta_spr(32, 16, 2, PIE_UP);
		// meta_spr(48, 16, 2, BANANA_UP);
		for (idx = 0; idx < 4; idx++) {
			if (abs((s16)x-(s16)pickupsX[idx]) < 8 && abs((s16)y-(s16)pickupsY[idx]) < 8) {
				pickupsX[idx] = -8;
				pickupsY[idx] = -8;
				P1holding = true;
			}
			meta_spr(pickupsX[idx],pickupsY[idx],0,HAMMER_UP);
		}
		
		px_spr_end();
		px_wait_nmi();
	}
	
	splash_screen();
}

void main(void){
	// Set up CC65 joystick driver.
	joy_install(nes_stdjoy_joy);
	
	// Set which tiles to use for the background and sprites.
	px_bg_table(0);
	px_spr_table(1);
	
	// Not using bank switching, but a good idea to set a reliable value at boot.
	px_uxrom_select(0);
	
	// Black out the palette.
	for(idx = 0; idx < 32; idx++) px_buffer_set_color(idx, 0x1D);
	px_wait_nmi();
	
	// Decompress the tileset into character memory.
	px_lz4_to_vram(CHR_ADDR(0, 0), MOCK);
	px_lz4_to_vram(CHR_ADDR(1, 0), CHRSM);

	sound_init(&SOUNDS);
	music_init(&MUSIC);
	
	// Jump to the splash screen state.
	splash_screen();
}
