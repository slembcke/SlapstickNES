#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"

#define BG_COLOR 0x1D
static const u8 PALETTE[] = {
	BG_COLOR, 0x14, 0x24, 0x35,
	BG_COLOR, 0x16, 0x27, 0x20,
	BG_COLOR, 0x16, 0x27, 0x20,
	BG_COLOR, 0x01, 0x11, 0x21,

	BG_COLOR, 0x18, 0x28, 0x38, // P1, BANANA
	BG_COLOR, 0x3d, 0x27, 0x30, // HAMMER, PIE, BOMB
	BG_COLOR, 0x1c, 0x2c, 0x3c,	// P2,
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


static const u8 BOMB_F1[] = {
	// FUSE
	 0, -8, 0xB4, PX_SPR_FLIPX,
	// BOMB
	 0, 0, 0xC4, PX_SPR_FLIPX,
	128,
};
static const u8 BOMB_F2[] = {
	// FUSE
	 0, -8, 0xB7, PX_SPR_FLIPX,
	// BOMB
	 0, 0, 0xC4, PX_SPR_FLIPX,
	128,
};
static const u8 BOMB_F3[] = {
	// FUSE
	 0, -8, 0xB5, PX_SPR_FLIPX,
	// BOMB
	 0, 0, 0xC4, PX_SPR_FLIPX,
	128,
};
static const u8 BOMB_F4[] = {
	// FUSE
	 0, -8, 0xB8, PX_SPR_FLIPX,
	// BOMB
	 0, 0, 0xC4, PX_SPR_FLIPX,
	128,
};
static const u8 BOMB_F5[] = {
	// FUSE
	 0, -8, 0xB6, PX_SPR_FLIPX,
	// BOMB
	 0, 0, 0xC4, PX_SPR_FLIPX,
	128,
};
static const u8 BOMB_F6[] = {
	// FUSE
	 //0, -8, 0xB5, PX_SPR_FLIPX,
	// BOMB
	 0, 0, 0xC4, PX_SPR_FLIPX,
	128,
};

static const u8* anim_BOMB_burn_DOWN[] = {
	BOMB_F1,
	BOMB_F2,
	BOMB_F1,
	BOMB_F2,
	BOMB_F1,
	BOMB_F2,

	BOMB_F3,
	BOMB_F4,
	BOMB_F3,
	BOMB_F4,
	BOMB_F3,
	BOMB_F4,

	BOMB_F5,
	BOMB_F6,
	BOMB_F5,
	BOMB_F6,
	BOMB_F5,
	BOMB_F6,
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
	items_none,
	items_hammer,
	items_pie,
	items_banana,
	items_bomb,
	hazard_peel,
};

static u8 pickupsX[5];
static u8 pickupsY[5];
static u8 pickupsT[5];
static u8 pickupsP[5];
static u8 pickupsR[5];

static u8 hazardsT[1];
static u8 hazardsX[1];
static u8 hazardsY[1];
static u8 hazardsS[1];
static u8 hazardsP[1];
static u8 hazardsA[1];

typedef struct {
	u16 x, y;
	bool walking, walkRight;
	bool holding, throw;
	u8 throwFrameTimer, item;
	u8 pieFaceTimer, bananaSlipTimer;
	u8 panHitTimer, hammerHitTimer;
	u8 palette_base;
	u8 palette;
} Player;

static Player P1, P2;
static Player* player;
static u8 bossStage = 0;
static u8 bossHits;
static bool boss_smack;
static u8 smileScore = 0; // 0 to 128
static u8 smileShown = 0; // 0 = frown, 1 = grin

static void tick_player(){
	static u8 player_flags, behind;
	static u16 x, y;

	behind = 0;
	player_flags = player->palette;

	if(player->x < 36 + PX.scroll_x) player->x = 36 + PX.scroll_x;
	if(player->x > 220 + PX.scroll_x) player->x = 220 + PX.scroll_x;
	if(player->y < 68) player->y = 68;
	if(player->y > 196) player->y = 196;

	if(smileScore < 128 || bossStage < 2){
		if(player->x > 0xCC && 0x48 < player->y && player->y < 0xA0){
			player->x -= 16;
		}
	} else {
		if(0x4E < player->y && player->y < 0xA2){
			if(player->x > 0xCC){
				if(player->x < 0x118){
					behind = PX_SPR_BEHIND;
					player_flags |= PX_SPR_BEHIND;
				}

				if(player->x > 256) player->y = 0x90;
				if(player->y < 0x90) player->y += 1;
				if(player->y > 0x90) player->y -= 1;
			}
		} else {
			if(player->x > 220){
				player->x = 220;
			}
		}
	}

	x = (u16)player->x - PX.scroll_x, y = player->y;

	if (player->throw) {
		player->throwFrameTimer -= 1;
		if (player->throwFrameTimer == 0) {
			player->throwFrameTimer = 24;
			player->throw = false;
			player->holding = false;
			player->item = items_none;
		}
	}

	if (player->hammerHitTimer > 1) {
		player->palette = 1;
		player->hammerHitTimer -= 1;
		if (player->hammerHitTimer == 1) {
			player->palette = player->palette_base;
		}
	}

	if (player->throw) {
		if (player->walkRight) {
			switch (player->item) {
				case items_hammer: 	px_spr(x+8, y-8, pickupsP[1], 0xB1); break;
				case items_pie: 	px_spr(x+8, y-8, pickupsP[2], 0xC1); break;
				case items_banana: 	px_spr(x+8, y-8, pickupsP[3], 0xB3); break;
				case items_bomb: 	meta_spr(x+8, y-8, pickupsP[4], anim_BOMB_burn_DOWN[px_ticks/8%18]); break;
			}
		}
		else {
			switch (player->item) {
				case items_hammer: 	px_spr(x-16, y-8, pickupsP[1]|PX_SPR_FLIPX, 0xB1); break;
				case items_pie: 	px_spr(x-16, y-8, pickupsP[2]|PX_SPR_FLIPX, 0xC1); break;
				case items_banana: 	px_spr(x-16, y-8, pickupsP[3]|PX_SPR_FLIPX, 0xB3); break;
				case items_bomb: 	meta_spr(x-16, y-8, pickupsP[4]|PX_SPR_FLIPX, anim_BOMB_burn_DOWN[px_ticks/8%18]); break;
			}
		}
	}
	else if (player->holding) {
		if (player->walkRight) {
			switch (player->item) {
				case items_hammer: 	px_spr(x-8, y-24, pickupsP[1], 0xB0); break;
				case items_pie: 	px_spr(x-8, y-24, pickupsP[2], 0xC0); break;
				case items_banana: 	px_spr(x-8, y-24, pickupsP[3], 0xB2); break;
				case items_bomb: 	meta_spr(x-8, y-24, pickupsP[4], anim_BOMB_burn_DOWN[px_ticks/8%18]); break;
			}
		}
		else {
			switch (player->item) {
				case items_hammer: 	px_spr(x, y-24, pickupsP[1]|PX_SPR_FLIPX, 0xB0); break;
				case items_pie: 	px_spr(x, y-24, pickupsP[2]|PX_SPR_FLIPX, 0xC0); break;
				case items_banana: 	px_spr(x, y-24, pickupsP[3]|PX_SPR_FLIPX, 0xB2); break;
				case items_bomb: 	meta_spr(x, y-24, pickupsP[4]|PX_SPR_FLIPX, anim_BOMB_burn_DOWN[px_ticks/8%18]); break;
			}
		}
	}

	// Draw a sprite.
	if (player->walking) {
		if (player->walkRight) {
			if (player->throw) {
				meta_spr(x, y, player_flags, anim_walk_right_throwing[px_ticks/8%2]);
			}
			else if (player->holding) {
				meta_spr(x, y, player_flags, anim_walk_right_holding[px_ticks/8%2]);
			}
			else {
				meta_spr(x, y, player_flags, anim_walk_right[px_ticks/8%2]);
			}
		}
		else {
			if (player->throw) {
				meta_spr(x, y, player_flags, anim_walk_left_throwing[px_ticks/8%2]);
			}
			else if (player->holding) {
				meta_spr(x, y, player_flags, anim_walk_left_holding[px_ticks/8%2]);
			}
			else {
				meta_spr(x, y, player_flags, anim_walk_left[px_ticks/8%2]);
			}
		}
	}
	else {
		if (player->walkRight) {
			if (player->throw) {
				meta_spr(x, y, player_flags, META_R1_THROWING);
			}
			else if (player->holding) {
				meta_spr(x, y, player_flags, META_R1_HOLDING);
			}
			else {
				meta_spr(x, y, player_flags, META_R1);
			}
		}
		else {
			if (player->throw) {
				meta_spr(x, y, player_flags, META_L1_THROWING);
			}
			else if (player->holding) {
				meta_spr(x, y, player_flags, META_L1_HOLDING);
			}
			else {
				meta_spr(x, y, player_flags, META_L1);
			}
		}
	}


	for (idx = 1; idx < 5; idx++) {
		if (pickupsR[idx] > 1) {
			pickupsR[idx] -= 1;
		}
		else {
			if (pickupsR[idx] <= 1) {
				pickupsR[idx] = 0;
				switch (pickupsT[idx]) {
					case items_hammer: 	if (player->item != items_hammer) { pickupsX[idx] = 48; pickupsY[idx] = 72; } break;
					case items_pie: 	if (player->item != items_pie) {pickupsX[idx] = 64; pickupsY[idx] = 72; } break;
					case items_banana: 	if (hazardsA[0] == false && player->item != items_banana) { pickupsX[idx] = 80; pickupsY[idx] = 72; } break;
					case items_bomb: 	pickupsX[idx] = 96; pickupsY[idx] = 72; break;
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
			case items_hammer : px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xB0); break;
			case items_pie : 	px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xC0); break;
			case items_banana : px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xB2); break;
			case items_bomb : 	meta_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],BOMB_F2); break;
		}
	}

	for (idx = 0; idx < 1; idx++) {
		px_spr(hazardsX[idx],hazardsY[idx],hazardsP[idx],hazardsS[idx]);
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
	if(JOY_BTN_A(pad1.press)) {
		if (P1.holding) {
			P1.throw = true;
			if (P1.item == items_hammer) {
				if (abs((s16)P1.x-(s16)P2.x) <= 24 && abs((s16)P1.y-(s16)P2.y) <= 24) {
					P2.hammerHitTimer = 24;
				}
			}
			else if (P1.item == items_banana) {
				hazardsA[0] = true;
				hazardsX[0] = P1.x;
				hazardsY[0] = P1.y;
			}
			sound_play(SOUND_JUMP);
		}
	}
	if(JOY_BTN_B(pad1.press)) { if (!P1.holding) { sound_play(SOUND_JUMP); }}

	if(JOY_LEFT (pad2.value)) { P2.x -= 1; P2.walking = true; P2.walkRight = false; }
	if(JOY_RIGHT(pad2.value)) { P2.x += 1; P2.walking = true; P2.walkRight = true; }
	if(JOY_DOWN (pad2.value)) { P2.y += 1; P2.walking = true; }
	if(JOY_UP   (pad2.value)) { P2.y -= 1; P2.walking = true; }
	if(JOY_BTN_A(pad2.press)) {
		if (P2.holding) {
			P2.throw = true;
			if (P2.item == items_hammer) {
				if (abs((s16)P2.x-(s16)P1.x) <= 24 && abs((s16)P2.y-(s16)P1.y) <= 24) {
					P1.hammerHitTimer = 24;
				}
			}
			sound_play(SOUND_JUMP);
		}
	}
	if(JOY_BTN_B(pad2.press)) { if (!P2.holding) { sound_play(SOUND_JUMP); }}
}

static const u8 SMILE_FROWN[] = {
	      0x9B, 0x9C,
	      0xAB, 0xAC,
	0xBA, 0xBB, 0xBC, 0xBD,
	0xCA, 0xCB, 0xCC, 0xCD,
	0xDA, 0xDB, 0xDC, 0xDD,
	0xEA, 0xEB, 0xEC, 0xED,
	0xFA, 0xFB, 0xFC, 0xFD,
};

static const u8 SMILE_GRIN[] = {
	      0x9B, 0x9C,
	      0xAB, 0xAC,
	0xBA, 0xBB, 0x64, 0x65,
	0x72, 0x73, 0x74, 0x75,
	0x82, 0x83, 0x84, 0x85,
	0x92, 0x93, 0x94, 0x95,
	0xFA, 0xFB, 0xFC, 0xFD,
};

static const u8 SMILE_TOOTHY[] = {
	      0x60, 0x61,
	      0x70, 0x71,
	0x66, 0x67, 0x68, 0x69,
	0x76, 0x77, 0x78, 0x79,
	0x86, 0x87, 0x88, 0x89,
	0x96, 0x97, 0x98, 0x99,
	0xA6, 0xA7, 0xA8, 0xA9,
};

static const u8 SMILE_CRACKED[] = {
	      0x60, 0x61,
	      0x70, 0x71,
	0x66, 0x67, 0x68, 0x69,
	0xC2, 0xC3, 0xC4, 0x79,
	0xD2, 0xD3, 0xD4, 0x89,
	0xE2, 0xE3, 0xE4, 0x99,
	0xA6, 0xA7, 0xA8, 0xA9,
};

static const u8 SMILE_BROKEN[] = {
	      0x60, 0x61,
	      0x70, 0x71,
	0x66, 0x67, 0x68, 0x69,
	0xC6, 0xC7, 0xC8, 0x79,
	0xD6, 0xD7, 0xD8, 0x89,
	0xE6, 0xE7, 0xE8, 0x99,
	0xA6, 0xA7, 0xA8, 0xA9,
};

static void show_smile_and_sync(const u8* smile){
	px_buffer_blit(0x21BB, smile + 0x00, 2);
	px_buffer_blit(0x21DB, smile + 0x02, 2);
	px_buffer_blit(0x21FA, smile + 0x04, 4);
	px_buffer_blit(0x221A, smile + 0x08, 4);
	px_buffer_blit(0x223A, smile + 0x0C, 4);
	px_buffer_blit(0x225A, smile + 0x10, 4);
	px_buffer_blit(0x227A, smile + 0x14, 4);
	px_wait_nmi();

}

static void boss_loop(void);

static void game_loop(void){
	P1.x = 128, P1.y = 128;
	P1.throwFrameTimer = 24;
	P1.palette_base = 0;
	P1.palette = 0;//P1.palette_base;

	P2.x = 96, P2.y = 128;
	P2.throwFrameTimer = 24;
	P2.palette_base = 2;
	P2.palette = 2;//P2.palette_base;

	pickupsT[0] = items_none;
	pickupsX[0] = 0;
	pickupsY[0] = 0;
	pickupsP[0] = 0;
	pickupsR[0] = 0;

	pickupsT[1] = items_hammer;
	pickupsX[1] = 48;
	pickupsY[1] = 72;
	pickupsP[1] = 1;
	pickupsR[1] = 0;

	pickupsT[2] = items_pie;
	pickupsX[2] = 64;
	pickupsY[2] = 72;
	pickupsP[2] = 1;
	pickupsR[2] = 0;

	pickupsT[3] = items_banana;
	pickupsX[3] = 80;
	pickupsY[3] = 72;
	pickupsP[3] = 0;
	pickupsR[3] = 0;

	pickupsT[4] = items_bomb;
	pickupsX[4] = 96;
	pickupsY[4] = 72;
	pickupsP[4] = 1;
	pickupsR[4] = 0;

	hazardsT[0] = hazard_peel;
	hazardsX[0] = -8;
	hazardsY[0] = -8;
	hazardsS[0] = 0xB3;
	hazardsP[0] = 0;
	hazardsA[0] = false;

	px_ppu_sync_disable();{
		// load the palettes
		px_addr(PAL_ADDR);
		px_blit(sizeof(PALETTE), PALETTE);

		// load the tilemaps
		px_lz4_to_vram(NT_ADDR(0, 0, 0), MAP0);
		px_lz4_to_vram(NT_ADDR(1, 0, 0), MAP1);
	} px_ppu_sync_enable();

	show_smile_and_sync(SMILE_FROWN);

	while(true){
		PX.scroll_x = 0;
		handle_input();

		px_profile_start();
		player = &P1;
		tick_player();

		player = &P2;
		tick_player();
		px_profile_end();

		for (idx = 0; idx < 4; idx++) {
			if (pickupsR[idx] > 1) {
				pickupsR[idx] -= 1;
			}
			else {
				if (pickupsR[idx] == 1) {
					pickupsR[idx] = 0;
					switch (pickupsT[idx]) {
						case items_hammer: 	pickupsX[idx] = 48; pickupsY[idx] = 72; break;
						case items_pie: 	pickupsX[idx] = 64; pickupsY[idx] = 72; break;
						case items_banana: 	pickupsX[idx] = 80; pickupsY[idx] = 72; break;
						case items_bomb: 	pickupsX[idx] = 96; pickupsY[idx] = 72; break;
					}
				}
			}

			switch (pickupsT[idx]) {
				case items_hammer : px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xB0); break;
				case items_pie : 	px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xC0); break;
				case items_banana : px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xB2); break;
				case items_bomb : 	px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xC4); break;
			}
		}

		px_spr_end();
		px_wait_nmi();

		if(JOY_START(pad1.press)) smileScore += 64;
		if(smileScore/64 != smileShown){
			smileShown = smileScore/64;
			show_smile_and_sync(smileShown == 0 ? SMILE_FROWN : SMILE_GRIN);
		}

		if(smileScore >= 128){
			boss_loop();

			smileScore = 0;
			show_smile_and_sync(SMILE_FROWN);
		}
	}

	game_loop();
}

static void player_boss_tick(){
	player->holding = true;
	player->item = items_hammer;
	// Hack the timer to let the player rapidly tap
	if(player->throwFrameTimer > 6) player->throwFrameTimer = 6;

	px_debug_hex_addr = NT_ADDR(0, 16, 2);
	px_debug_hex(P1.x);

	// Even more total hackery with the frame timer here...
	if(player->throwFrameTimer == 5 && player->walkRight){
		// px_debug_hex(P1.y);
		if(bossStage < 2 && 0xC0 < player->x && 0x80 < player->y && player->y < 0x94){
			bossHits++;
			boss_smack = true;
		} else if(bossStage >= 2 && player->x > 0x11C){
			bossHits++;
			boss_smack = true;
		}
	}
}

static void boss_loop(){
	bossHits = 0;
	switch(bossStage){
		case 0: show_smile_and_sync(SMILE_TOOTHY); break;
		case 1: show_smile_and_sync(SMILE_CRACKED); break;
		default: show_smile_and_sync(SMILE_BROKEN); break;
	}

	while(true){
		handle_input();
		boss_smack = false;

		if(bossStage >= 2){
			if(PX.scroll_x < 88) PX.scroll_x += 2;
		}

		player = &P1;
		tick_player();
		player_boss_tick();

		player = &P2;
		tick_player();
		player_boss_tick();

		px_buffer_blit(NT_ADDR(0, 3, 3), "BOSS", 4);

		px_spr_end();
		px_wait_nmi();

		if(boss_smack){
			px_buffer_set_color(10, 0x06);
		} else{
			px_buffer_set_color(10, ((px_ticks & 8) == 0) ? 0x3C : 0x36);
		}

		if(bossHits > 4){
			bossStage++;
			break;
		}
	}

	px_buffer_set_color(10, PALETTE[10]);
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
