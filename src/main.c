#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"

#define BG_COLOR 0x1D
static const u8 PALETTE[] = {
	BG_COLOR, 0x01, 0x11, 0x21,
	BG_COLOR, 0x27, 0x37, 0x20,
	BG_COLOR, 0x09, 0x19, 0x29,
	BG_COLOR, 0x01, 0x11, 0x21,
	
	BG_COLOR, 0x18, 0x28, 0x38, // P1, BANANA
	BG_COLOR, 0x3d, 0x17, 0x30, // HAMMER, PIE
	BG_COLOR, 0x1c, 0x2c, 0x3c,	// P2, BOMB
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

enum {
	items_hammer,
	items_pie,
	items_banana,
	items_bomb,
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
static u8 pickupsT[4];
static u8 pickupsR[4];

typedef struct {
	u8 x, y;
	bool walking, walkRight;
	bool holding, throw;
	u8 throwFrameTimer, item;
	u8 palette;
} Player;

static Player P1, P2;
static Player* player;

static void tick_player(){
	static u8 x, y;
	
	x = player->x, y = player->y;
	
	if (player->throw) {
		player->throwFrameTimer -= 1;
		if (player->throwFrameTimer == 1) {
			player->throwFrameTimer = 24;
			player->throw = false;
			player->holding = false;
		}	
	}
	
	if (player->throw) {
		if (player->walkRight) {
			switch (player->item) {
				case items_hammer: 	px_spr(x+8, y-8, 1, 0xB1); break;
				case items_pie: 	px_spr(x+8, y-8, 1, 0xC1); break;
				case items_banana: 	px_spr(x+8, y-8, 0, 0xB3); break;
			}
		}
		else {
			switch (player->item) {
				case items_hammer: 	px_spr(x-16, y-8, 1|PX_SPR_FLIPX, 0xB1); break;
				case items_pie: 	px_spr(x-16, y-8, 1|PX_SPR_FLIPX, 0xC1); break;
				case items_banana: 	px_spr(x-16, y-8, 0|PX_SPR_FLIPX, 0xB3); break;
			}
		}
	} 
	else if (player->holding) {
		if (player->walkRight) {
			switch (player->item) {
				case items_hammer: 	px_spr(x-8, y-24, 1, 0xB0); break;
				case items_pie: 	px_spr(x-8, y-24, 1, 0xC0); break;
				case items_banana: 	px_spr(x-8, y-24, 0, 0xB2); break;
			}
		}
		else {
			switch (player->item) {
				case items_hammer: 	px_spr(x, y-24, 1|PX_SPR_FLIPX, 0xB0); break;
				case items_pie: 	px_spr(x, y-24, 1|PX_SPR_FLIPX, 0xC0); break;
				case items_banana: 	px_spr(x, y-24, 0|PX_SPR_FLIPX, 0xB2); break;
			}
		}
	}

	// Draw a sprite.
	if (player->walking) {
		if (player->walkRight) {
			if (player->throw) {
				meta_spr(x, y, player->palette, anim_walk_right_throwing[px_ticks/8%2]);
			}
			else if (player->holding) {
				meta_spr(x, y, player->palette, anim_walk_right_holding[px_ticks/8%2]);
			}
			else {
				meta_spr(x, y, player->palette, anim_walk_right[px_ticks/8%2]);
			}
		}
		else {
			if (player->throw) {
				meta_spr(x, y, player->palette, anim_walk_left_throwing[px_ticks/8%2]);
			}
			else if (player->holding) {
				meta_spr(x, y, player->palette, anim_walk_left_holding[px_ticks/8%2]);
			}
			else {
				meta_spr(x, y, player->palette, anim_walk_left[px_ticks/8%2]);
			}
		}
	}
	else {
		if (player->walkRight) {
			if (player->throw) {
				meta_spr(x, y, player->palette, META_R1_THROWING);
			}
			else if (player->holding) {
				meta_spr(x, y, player->palette, META_R1_HOLDING);
			}
			else {
				meta_spr(x, y, player->palette, META_R1);
			}
		}
		else {
			if (player->throw) {
				meta_spr(x, y, player->palette, META_L1_THROWING);
			}
			else if (player->holding) {
				meta_spr(x, y, player->palette, META_L1_HOLDING);
			}
			else {
				meta_spr(x, y, player->palette, META_L1);
			}
		}
	}
	
	
	for (idx = 0; idx < 4; idx++) {
		if (pickupsR[idx] > 1) {
			pickupsR[idx] -= 1;
		}
		else {
			if (pickupsR[idx] == 1) {
				pickupsR[idx] = 0;
				switch (pickupsT[idx]) {
					case items_hammer: pickupsX[idx] = 48; pickupsY[idx] = 72; break;
					case items_pie: pickupsX[idx] = 64; pickupsY[idx] = 72; break;
					case items_banana: pickupsX[idx] = 80; pickupsY[idx] = 72; break;
				}
			}
		}
		
		if (abs((s16)x-(s16)pickupsX[idx]) < 8 && abs((s16)y-(s16)pickupsY[idx]) < 8 && !player->holding) {
			pickupsX[idx] = -8;
			pickupsY[idx] = -8;
			pickupsR[idx] = 100;
			player->item = pickupsT[idx];
			player->holding = true;
		}
		switch (pickupsT[idx]) {
			case items_hammer : meta_spr(pickupsX[idx],pickupsY[idx],0,HAMMER_UP); break;
			case items_pie : meta_spr(pickupsX[idx],pickupsY[idx],0,PIE_UP); break;
			case items_banana : meta_spr(pickupsX[idx],pickupsY[idx],0,BANANA_UP); break;
			//case items_bomb : meta_spr(pickupsX[idx],pickupsY[idx],0,BOMB_UP); break;
		}
		
	}
}

static void handle_input(){
	read_gamepads();
	
	P1.walking = false;
	P2.walking = false;
	
	if(JOY_LEFT (pad1.value)) { P1.x -= 1; P1.walking = true; P1.walkRight = false; } 
	if(JOY_RIGHT(pad1.value)) { P1.x += 1; P1.walking = true; P1.walkRight = true; }
	if(JOY_DOWN (pad1.value)) { P1.y += 1; P1.walking = true; }
	if(JOY_UP   (pad1.value)) { P1.y -= 1; P1.walking = true; }
	if(JOY_BTN_A(pad1.press)) { if (P1.holding) { P1.throw = true; sound_play(SOUND_JUMP); }}
	if(JOY_BTN_B(pad1.press)) { if (!P1.holding) { sound_play(SOUND_JUMP); }}
	
	if(JOY_LEFT (pad2.value)) { P2.x -= 1; P2.walking = true; P2.walkRight = false; } 
	if(JOY_RIGHT(pad2.value)) { P2.x += 1; P2.walking = true; P2.walkRight = true; }
	if(JOY_DOWN (pad2.value)) { P2.y += 1; P2.walking = true; }
	if(JOY_UP   (pad2.value)) { P2.y -= 1; P2.walking = true; }
	if(JOY_BTN_A(pad2.press)) { if (P2.holding) { P2.throw = true; sound_play(SOUND_JUMP); }}
	if(JOY_BTN_B(pad2.press)) { if (!P2.holding) { sound_play(SOUND_JUMP); }}
}

static void boss_loop(void);

static void game_loop(void){
	P1.x = 32, P1.y = 32;
	P1.throwFrameTimer = 24;
	P1.palette = 0;

	P2.x = 64, P2.y = 64;
	P2.throwFrameTimer = 24;
	P2.palette = 2;

	pickupsX[0] = 48;
	pickupsY[0] = 72;
	pickupsT[0] = items_hammer;
	pickupsR[0] = 0;
	pickupsX[1] = 64;
	pickupsY[1] = 72;
	pickupsT[1] = items_pie;
	pickupsR[1] = 0;
	pickupsX[2] = 80;
	pickupsY[2] = 72;
	pickupsT[2] = items_banana;
	pickupsR[2] = 0;

	px_ppu_sync_disable();{
		// load the palettes
		px_addr(PAL_ADDR);
		px_blit(sizeof(PALETTE), PALETTE);
		
		// load the tilemaps
		px_lz4_to_vram(NT_ADDR(0, 0, 0), MAP0);
		px_lz4_to_vram(NT_ADDR(1, 0, 0), MAP1);
	} px_ppu_sync_enable();
	
	while(true){
		handle_input();
		
		px_profile_start();
		player = &P1;
		tick_player();
		
		player = &P2;
		tick_player();
		px_profile_end();
		
		px_spr_end();
		px_wait_nmi();
		
		if(JOY_START(pad1.value)) boss_loop();
	}
	
	game_loop();
}

static const SMILE1[] = {0x1};

static void boss_loop(){
	static u8 boss_hits;
	static bool smack;
	
	boss_hits = 0;
	
	px_buffer_blit(0x2A3A, SMILE1, 1);
	px_wait_nmi();
	
	while(true){
		handle_input();
		smack = false;
		
		P1.holding = true;
		P1.item = items_hammer;
		// Hack the timer to let the player rapidly tap
		if(P1.throwFrameTimer > 8) P1.throwFrameTimer = 8;
		if(JOY_BTN_A(pad1.press)){
			// px_debug_hex(P1.y);
			if(0xC0 < P1.x && 0x80 < P1.y && P1.y < 0xA0){
				boss_hits++;
				smack = true;
			}
		}
		
		player = &P1;
		tick_player();
		
		// px_debug_hex(boss_hits);
		px_buffer_blit(NT_ADDR(0, 3, 3), "BOSS", 4);
		
		px_spr_end();
		px_wait_nmi();
		
		if(smack){
			px_buffer_set_color(7, 0x06);
		} else{	
			px_buffer_set_color(7, ((px_ticks & 8) == 0) ? 0x3C : 0x36);
		}
		
		px_debug_hex((px_ticks & 1) == 0);
		if(boss_hits > 32) break;
	}
	
	px_buffer_set_color(7, PALETTE[7]);
	px_wait_nmi();
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
	px_lz4_to_vram(CHR_ADDR(0, 0), CHRBG);
	px_lz4_to_vram(CHR_ADDR(1, 0), CHRSM);

	sound_init(&SOUNDS);
	music_init(&MUSIC);
	
	// Jump to the splash screen state.
	game_loop();
}
