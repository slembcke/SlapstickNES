#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"

#define BG_COLOR 0x1D
static const u8 PALETTE[] = {
	BG_COLOR, 0x13, 0x22, 0x33,
	BG_COLOR, 0x16, 0x27, 0x20,
	BG_COLOR, 0x16, 0x27, 0x20,
	BG_COLOR, 0x01, 0x11, 0x21,

	BG_COLOR, 0x18, 0x28, 0x38, // P1, BANANA
	BG_COLOR, 0x3d, 0x27, 0x30, // HAMMER, PIE, BOMB
	BG_COLOR, 0x1c, 0x2c, 0x3c,	// P2,
	BG_COLOR, 0x2d, 0x3d, 0x00, // BURNT
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

static const u8 airPuff_F1[] = {
	0, 0, 0xEB, 0,
	128,
};
static const u8 airPuff_F2[] = {
	-8, -8, 0xDB, 0,
	-8,  0, 0xF9, 0,
	-8,  8, 0xDB, PX_SPR_FLIPY,
	 0,  0, 0xE9, 0,
	128,
};
static const u8 airPuff_F3[] = {
	-16, -8, 0xDA, 0,
	-16,  0, 0xD9, 0,
	-24,  0, 0xFA, 0,
	-16,  8, 0xDA, PX_SPR_FLIPY,
	-8,  0, 0xE9, 0,
	 0,  0, 0xEA, 0,
	128,
};

static const u8 splosion[] = {
	-8, -8, 0xD6, 1,
	 0, -8, 0xD7, 1,
	 8, -8, 0xD8, 1,

	-8,  0, 0xE6, 1,
	 0,  0, 0xE7, 1,
	 8,  0, 0xE8, 1,

	-8,  8, 0xF6, 1,
	 0,  8, 0xf7, 1,
	 8,  8, 0xf8, 1,
	 128,
};

static const u8* anim_AIR_PUFF[] = {
	airPuff_F1,
	airPuff_F2,
	airPuff_F3,
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
	items_splosion,
	hazard_peel,
	hazard_pie,
};

static u8 pickupsX[5];
static u8 pickupsY[5];
static u8 pickupsT[5];
static u8 pickupsP[5];
static u8 pickupsR[5];
static u8 pickupsS[5];

#define HAZARD_COUNT 2 // [banana, pie]
static u8 hazardsT[HAZARD_COUNT]; // type
static u8 hazardsX[HAZARD_COUNT];
static u8 hazardsY[HAZARD_COUNT];
static u8 hazardsS[HAZARD_COUNT]; // sprite
static u8 hazardsP[HAZARD_COUNT]; // palette
static u8 hazardsA[HAZARD_COUNT]; // active

typedef struct {
	u16 x, y;
	bool walking, walkRight;
	bool holding, throw;
bool slipping;
	u8 throwFrameTimer, item;
	u8 pieFaceTimer;
	u8 panHitTimer, hammerHitTimer;
	u8 splodedTimer;
	u8 palette_base;
	u8 palette;
} Player;

static Player P1, P2;
static Player* player;
static u8 bossStage = 0; // 0 = teeth, 1 = cracked, 2 = uvula, 3 = done
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
		// Knock the player back from the clown during the slapstick phase
		if(player->x > 0xCC && 0x48 < player->y && player->y < 0xA0){
			player->x -= 16;
		}
	} else {
		if((0x4E < player->y && player->y < 0xA2) || bossStage >= 3){
			if(player->x > 0xCC){
				// Check if the player should be behind the mouth
				if(player->x < 0x118){
					behind = PX_SPR_BEHIND;
					player_flags |= PX_SPR_BEHIND;
				}

				// line up the player with the mouth
				if(player->x > 256) player->y = 0x90;
				if(player->y < 0x90) player->y += 1;
				if(player->y > 0x90) player->y -= 1;
			}
		} else {
			// Clamp to right edge
			if(player->x > 220){
				player->x = 220;
			}
		}
	}

	x = (u16)player->x - PX.scroll_x, y = player->y;

	// ITEMS
	if (player->item == items_bomb) {
		if (px_ticks/8%18 == 17) {
			player->holding = true;
			player->item = items_splosion;

			if (P1.x+8 > x-8 && P1.y > y-24-16) {
				P1.palette = 3;
				P1.splodedTimer = 128;
				smileScore += 16;
			}

			if (P2.x+8 > x-8 && P2.y > y-24-16) {
				P2.palette = 3;
				P2.splodedTimer = 128;
				smileScore += 16;
			}
		}
	}

	if (player-> item == items_splosion) {
		if (player->splodedTimer <= 116) {
			player->item = items_none;
			player->holding = false;
		}
	}


	// TIMERS
	if (player->hammerHitTimer > 1) {
		player->palette = 1;
		player->hammerHitTimer -= 1;
		if (player->hammerHitTimer == 1) {
			player->palette = player->palette_base;
		}
	}

	if (player->splodedTimer > 1) {
		player->palette = 3;
		player->splodedTimer -= 1;
		if (player->splodedTimer == 1) {
			player->palette = player->palette_base;
		}
	}

	// ACTIONS
	if (player->throw && player->item != items_splosion) {
		player->throwFrameTimer -= 1;
		if (player->throwFrameTimer == 0) {
			player->throwFrameTimer = 24;
			player->throw = false;
			player->holding = false;
			player->item = items_none;
		}
	}

	// GRAPHICS
	if (player->throw) {
		if (player->walkRight) {
			switch (player->item) {
				case items_hammer: 	px_spr(x+8, y-8, behind|pickupsP[1], 0xB1); break;
				case items_pie: 	px_spr(x+8, y-8, pickupsP[2], 0xC1); break;
				case items_banana: 	px_spr(x+8, y-8, pickupsP[3], 0xB3); break;
				case items_bomb: 	meta_spr(x+8, y-8, pickupsP[4], anim_BOMB_burn_DOWN[px_ticks/8%18]); break;
			}
		}
		else {
			switch (player->item) {
				case items_hammer: 	px_spr(x-16, y-8, behind|pickupsP[1]|PX_SPR_FLIPX, 0xB1); break;
				case items_pie: 	px_spr(x-16, y-8, pickupsP[2]|PX_SPR_FLIPX, 0xC1); break;
				case items_banana: 	px_spr(x-16, y-8, pickupsP[3]|PX_SPR_FLIPX, 0xB3); break;
				case items_bomb: 	meta_spr(x-16, y-8, pickupsP[4]|PX_SPR_FLIPX, anim_BOMB_burn_DOWN[px_ticks/8%18]); break;
			}
		}
	}
	else if (player->holding) {
		if (player->walkRight) {
			switch (player->item) {
				case items_hammer: 	px_spr(x-8, y-24, behind|pickupsP[1], 0xB0); break;
				case items_pie: 	px_spr(x-8, y-24, pickupsP[2], 0xC0); break;
				case items_banana: 	px_spr(x-8, y-24, pickupsP[3], 0xB2); break;
				case items_bomb:
					meta_spr(x-8, y-24, pickupsP[4], anim_BOMB_burn_DOWN[px_ticks/8%18]);
				break;
				case items_splosion: meta_spr(x, y-24, pickupsP[5], splosion); break;
			}
		}
		else {
			switch (player->item) {
				case items_hammer: 	px_spr(x, y-24, behind|pickupsP[1]|PX_SPR_FLIPX, 0xB0); break;
				case items_pie: 	px_spr(x, y-24, pickupsP[2]|PX_SPR_FLIPX, 0xC0); break;
				case items_banana: 	px_spr(x, y-24, pickupsP[3]|PX_SPR_FLIPX, 0xB2); break;
				case items_bomb:
					meta_spr(x, y-24, pickupsP[4]|PX_SPR_FLIPX, anim_BOMB_burn_DOWN[px_ticks/8%18]);
				break;
				case items_splosion: meta_spr(x, y-24, pickupsP[5], splosion); break;
			}
		}
	}

	if(player->pieFaceTimer > 0){
		px_spr(x + (player->walkRight ? 0 : -8), y - 8 - player->pieFaceTimer/8, 1 | (player->walkRight ? 0 : PX_SPR_FLIPX), 0xC2);
		player->pieFaceTimer--;
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

// super rushed hack:
	// reach into sprite memory and squish existing sprites down when hammered
	if(player->palette == 1){
		(OAM - 12)[px_sprite_cursor] += 4;
		(OAM - 16)[px_sprite_cursor] += 4;
		(OAM - 20)[px_sprite_cursor] += 8;
		(OAM - 24)[px_sprite_cursor] += 8;
		if(player->holding) (OAM - 28)[px_sprite_cursor] += 8;
	}

	for (idx = 1; idx < 5; idx++) {
		if (abs((s16)x-(s16)pickupsX[idx]) < 8 && abs((s16)y-(s16)pickupsY[idx]) < 8 && !player->holding) {
			pickupsX[idx] = -8;
			pickupsY[idx] = -8;
			pickupsR[idx] = 100;
			player->item = pickupsT[idx];
			player->holding = true;
		}
	}

	for(idx = 0; idx < HAZARD_COUNT; idx++){
		switch(hazardsT[idx]){
			case hazard_peel:
				if (hazardsA[idx] && abs((s16)x-(s16)hazardsX[idx]) < 8 && abs((s16)y-(s16)hazardsY[idx]) < 8) {
					player->slipping = true;
					hazardsA[idx] = false;
					hazardsX[idx] = -8;
					hazardsY[idx] = -8;
					smileScore += 16;
				}
				break;
			case hazard_pie:
				if (hazardsA[idx] && abs((s16)x-(s16)hazardsX[idx]) < 8 && abs((s16)y - (s16)hazardsY[idx] - 18) < 8) {
					player->pieFaceTimer = 60;
					hazardsA[idx] = false;
					hazardsX[idx] = -8;
					hazardsY[idx] = -8;
					smileScore += 16;
				}
				break;
			default: break;
		}
	}
}

static void handle_input(){
	read_gamepads();

	P1.walking = false;
	P2.walking = false;

	if(P1.slipping){
		P1.x += (P1.walkRight ? 2: -2);
		if(P1.x < 0x24 || 0xC0 < P1.x) P1.slipping = false;
	} else {
		if(JOY_LEFT (pad1.value)) { P1.x -= 1; P1.walking = true; P1.walkRight = false; }
		if(JOY_RIGHT(pad1.value)) { P1.x += 1; P1.walking = true; P1.walkRight = true; }
		if(JOY_DOWN (pad1.value)) { P1.y += 1; P1.walking = true; }
		if(JOY_UP   (pad1.value)) { P1.y -= 1; P1.walking = true; }
		if(JOY_BTN_A(pad1.press)) {
			if (P1.holding && P1.item != items_splosion) {
				P1.throw = true;
				if (P1.item == items_hammer) {
					if (abs((s16)P1.x-(s16)P2.x) <= 24 && abs((s16)P1.y-(s16)P2.y) <= 24) {
						P2.hammerHitTimer = 24;
						smileScore += 16;
					}
				}
				else if (P1.item == items_banana) {
					hazardsA[0] = true;
					hazardsX[0] = P1.x + (P1.walkRight ? 16 : -16);
					hazardsY[0] = P1.y;
				}
				else if (P1.item == items_bomb) {
					P1.throw = false;
					P1.holding = true;
					P1.item = items_splosion;
					P1.palette = 3;
					P1.splodedTimer = 128;
					smileScore += 16;

					if (P2.x+8 > P1.x-8 && P2.y > P1.y-24-16) {
						P2.palette = 3;
						P2.splodedTimer = 128;
						smileScore += 16;
					}
				}
				else if(P1.item == items_pie){
						hazardsA[1] = true;
						hazardsX[1] = P1.x + (P1.walkRight ? 16 : -16);
						hazardsY[1] = P1.y - 18;
						hazardsP[1] = 1 | (P1.walkRight ? PX_SPR_FLIPX : 0);
				}
				sound_play(SOUND_JUMP);
			}
		}
		if(JOY_BTN_B(pad1.press)) { if (!P1.holding) { sound_play(SOUND_JUMP); }}
	}

	if(P2.slipping){
		P2.x += (P2.walkRight ? 2: -2);
		if(P2.x < 0x24 || 0xC0 < P2.x) P2.slipping = false;
	} else {
		if(JOY_LEFT (pad2.value)) { P2.x -= 1; P2.walking = true; P2.walkRight = false; }
		if(JOY_RIGHT(pad2.value)) { P2.x += 1; P2.walking = true; P2.walkRight = true; }
		if(JOY_DOWN (pad2.value)) { P2.y += 1; P2.walking = true; }
		if(JOY_UP   (pad2.value)) { P2.y -= 1; P2.walking = true; }
		if(JOY_BTN_A(pad2.press)) {
			if (P2.holding && P2.item != items_splosion) {
				P2.throw = true;
				if (P2.item == items_hammer) {
					if (abs((s16)P2.x-(s16)P1.x) <= 24 && abs((s16)P2.y-(s16)P1.y) <= 24) {
						P1.hammerHitTimer = 24;
						smileScore += 16;
					}
				}
				else if (P2.item == items_banana) {
					hazardsA[0] = true;
					hazardsX[0] = P2.x + (P2.walkRight ? 16 : -16);
					hazardsY[0] = P2.y;
				}
				else if (P2.item == items_bomb) {
					P2.throw = false;
					P2.holding = true;
					P2.item = items_splosion;
					P2.palette = 3;
					P2.splodedTimer = 128;
					smileScore += 16;

					if (P1.x+8 > P2.x-8 && P1.y > P2.y-24-16) {
						P1.palette = 3;
						P1.splodedTimer = 128;
						smileScore += 16;
					}
				}
				else if(P2.item == items_pie){
					hazardsA[1] = true;
					hazardsX[1] = P2.x + (P2.walkRight ? 16 : -16);
					hazardsY[1] = P2.y - 18;
					hazardsP[1] = 1 | (P2.walkRight ? PX_SPR_FLIPX : 0);
				}
				sound_play(SOUND_JUMP);
			}
		}
		if(JOY_BTN_B(pad2.press)) { if (!P2.holding) { sound_play(SOUND_JUMP); }}
	}
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
	px_buffer_blit(0x21BB, smile +  0, 2);
	px_buffer_blit(0x21DB, smile +  2, 2);
	px_buffer_blit(0x21FA, smile +  4, 4);
	px_buffer_blit(0x221A, smile +  8, 4);
	px_buffer_blit(0x223A, smile + 12, 4);
	px_buffer_blit(0x225A, smile + 16, 4);
	px_buffer_blit(0x227A, smile + 20, 4);
	px_wait_nmi();
}

static void deflate_uvula_and_sync(void){
	static const u8 uvula[] = {
		0x00, 0x91, 0x00,
		0xA0, 0xA1, 0xA2,
		0xB0, 0xB1, 0xB2,
		0xC0, 0xC1, 0x00,
		0b01010101,
	};

	px_buffer_blit(0x25C5, uvula + 0, 3);
	px_buffer_blit(0x25E5, uvula + 3, 3);
	px_buffer_blit(0x2605, uvula + 6, 3);
	px_buffer_blit(0x2625, uvula + 9, 3);
	px_buffer_blit(0x27D9, uvula + 12, 1);
	px_buffer_blit(0x27E1, uvula + 12, 1);
}

static void draw_humor_bar(){
	// gotta get this done!
	if(smileScore >= 1*16) px_spr(168 + 0*8, 22, 0, 0xB3);
	if(smileScore >= 2*16) px_spr(168 + 1*8, 22, 0, 0xB3);
	if(smileScore >= 3*16) px_spr(168 + 2*8, 22, 0, 0xB3);
	if(smileScore >= 4*16) px_spr(168 + 3*8, 22, 0, 0xB3);
	if(smileScore >= 5*16) px_spr(168 + 4*8, 22, 0, 0xB3);
	if(smileScore >= 6*16) px_spr(168 + 5*8, 22, 0, 0xB3);
	if(smileScore >= 7*16) px_spr(168 + 6*8, 22, 0, 0xB3);
	if(smileScore >= 8*16) px_spr(168 + 7*8, 22, 0, 0xB3);
}

static void draw_hit_bar(){
	// gotta get this done!
	if(bossHits < 8*4) px_spr(168 + 0*8 - PX.scroll_x, 22, 0, 0xB3);
	if(bossHits < 7*4) px_spr(168 + 1*8 - PX.scroll_x, 22, 0, 0xB3);
	if(bossHits < 6*4) px_spr(168 + 2*8 - PX.scroll_x, 22, 0, 0xB3);
	if(bossHits < 5*4) px_spr(168 + 3*8 - PX.scroll_x, 22, 0, 0xB3);
	if(bossHits < 4*4) px_spr(168 + 4*8 - PX.scroll_x, 22, 0, 0xB3);
	if(bossHits < 3*4) px_spr(168 + 5*8 - PX.scroll_x, 22, 0, 0xB3);
	if(bossHits < 2*4) px_spr(168 + 6*8 - PX.scroll_x, 22, 0, 0xB3);
	if(bossHits < 1*4) px_spr(168 + 7*8 - PX.scroll_x, 22, 0, 0xB3);
}

static void boss_loop(void);

static void game_loop(void){
	rand_seed = 8347;
	
	P1.x = 128, P1.y = 128;
	P1.throwFrameTimer = 24;
	P1.palette_base = 0;
	P1.palette = 0;//P1.palette_base;

	P2.x = 96, P2.y = 128;
	P2.throwFrameTimer = 24;
	P2.palette_base = 2;
	P2.palette = 2;//P2.palette_base;

	pickupsT[1] = items_hammer;
	pickupsP[1] = 1;
	pickupsR[1] = 1;

	pickupsT[2] = items_pie;
	pickupsP[2] = 1;
	pickupsR[2] = 1;

	pickupsT[3] = items_banana;
	pickupsP[3] = 0;
	pickupsR[3] = 1;

	pickupsT[4] = items_bomb;
	pickupsP[4] = 1;
	pickupsR[4] = 1;

	pickupsT[5] = items_splosion;
	pickupsP[5] = 1;
	pickupsR[5] = 1;

	hazardsT[0] = hazard_peel;
	hazardsY[0] = -8;
	hazardsS[0] = 0xB3;
	hazardsP[0] = 0;
	hazardsA[0] = false;

	hazardsT[1] = hazard_pie;
	hazardsY[1] = -8;
	hazardsS[1] = 0xC2;
	hazardsP[1] = 1 | PX_SPR_FLIPX;
	hazardsA[1] = false;

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

		// px_profile_start();
		player = &P1;
		tick_player();

		player = &P2;
		tick_player();
		// px_profile_end();
		
		for (idx = 1; idx < 5; idx++) {
			if (pickupsR[idx] == 1){
				switch (pickupsT[idx]) {
					case items_hammer:
						if (P1.item != items_hammer && P2.item != items_hammer) {
							pickupsX[idx] = 48 + rand8()/2; pickupsY[idx] = 64 + rand8()/2;
							pickupsR[idx] = 0;
						}
						break;
					case items_banana:
						px_debug_hex(px_ticks);
						if (hazardsA[0] == false && P1.item != items_banana && P2.item != items_banana) {
							pickupsX[idx] = 48 + rand8()/2; pickupsY[idx] = 64 + rand8()/2;
							pickupsR[idx] = 0;
						}
						break;
					case items_pie:
						if (hazardsA[1] == false && P1.item != items_pie && P2.item != items_pie) {
							pickupsX[idx] = 48 + rand8()/2; pickupsY[idx] = 64 + rand8()/2;
							pickupsR[idx] = 0;
						}
						break;
					case items_bomb:
						if (P1.item != items_bomb && P2.item != items_bomb) {
							pickupsX[idx] = 48 + rand8()/2; pickupsY[idx] = 64 + rand8()/2;
							pickupsR[idx] = 0;
						}
						break;
				}
			}
			if (pickupsR[idx] > 1) pickupsR[idx] -= 1;

			// draw pickups
			switch (pickupsT[idx]) {
				case items_hammer : px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xB0); break;
				case items_pie : 	px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xC0); break;
				case items_banana : px_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],0xB2); break;
				case items_bomb : 	meta_spr(pickupsX[idx],pickupsY[idx],pickupsP[idx],BOMB_F2); break;
			}
		}

		for (idx = 0; idx < HAZARD_COUNT; idx++) {
			if(hazardsT[idx] == hazard_pie){
				hazardsX[idx] -= (hazardsP[idx] & PX_SPR_FLIPX ? -2 : 2);
				if(hazardsX[idx] < 0x20 || 0xC0 < hazardsX[idx]) hazardsY[idx] = -8;
			}

			// draw hazards
			px_spr(hazardsX[idx],hazardsY[idx],hazardsP[idx],hazardsS[idx]);
		}

		// HAZARDS


		//meta_spr(100,100,1,splosion);

		draw_humor_bar();

		px_spr_end();
		px_wait_nmi();

		if(JOY_START(pad1.press)) smileScore += 16;
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

	// Even more total hackery with the frame timer here...
	if(player->throwFrameTimer == 5 && player->walkRight){
		if(bossStage < 2 && 0xC0 < player->x && 0x80 < player->y && player->y < 0x94){
			bossHits++;
			boss_smack = true;
		}
		else if(bossStage >= 2 && player->x > 0x11C){
			bossHits++;
			boss_smack = true;
		}
	}
}

static void boss_loop(){
	u8 frame = 0;
	bossHits = 0;
	switch(bossStage){
		case 0: show_smile_and_sync(SMILE_TOOTHY); break;
		case 1: show_smile_and_sync(SMILE_CRACKED); break;
		default: deflate_uvula_and_sync(); // fallthrough
		case 2: show_smile_and_sync(SMILE_BROKEN); break;
	}

	while(true){
		handle_input();
		boss_smack = false;

		if(bossStage >= 2 && PX.scroll_x < 88) PX.scroll_x += 2;
		if(bossStage >= 3 && PX.scroll_x < 256) PX.scroll_x += 1;
		frame = px_ticks%96/4;
		if (frame < 8) {
			meta_spr(200-PX.scroll_x,120,2,anim_AIR_PUFF[px_ticks/8%3]);
		}

		player = &P1;
		tick_player();
		player_boss_tick();

		if (frame < 8 && bossStage < 2) {
			switch (px_ticks/8%3) {
			case 0:
				if (player->x > 200-PX.scroll_x-8 && player->y > 96 && player->y < 148) {
					player->x -= 8;
				}
			break;
			case 1:
				if (player->x > 200-PX.scroll_x-16 && player->y > 96 && player->y < 148) {
					player->x -= 16;
				}
			break;
			case 2:
				if (player->x > 200-PX.scroll_x-24 && player->y > 96 && player->y < 148) {
					player->x -= 24;
				}
			break;
			}
		}

		player = &P2;
		tick_player();
		player_boss_tick();

		if (frame < 8 && bossStage < 2) {
			switch (px_ticks/8%3) {
			case 0:
				if (player->x > 200-PX.scroll_x-8 && player->y > 96 && player->y < 148) {
					player->x -= 8;
				}
			break;
			case 1:
				if (player->x > 200-PX.scroll_x-16 && player->y > 96 && player->y < 148) {
					player->x -= 16;
				}
			break;
			case 2:
				if (player->x > 200-PX.scroll_x-24 && player->y > 96 && player->y < 148) {
					player->x -= 24;
				}
			break;
			}
		}

		draw_hit_bar();
		px_spr_end();
		px_wait_nmi();

		if(boss_smack){
			px_buffer_set_color(10, 0x06);
		} else{
			px_buffer_set_color(10, ((px_ticks & 8) == 0) ? 0x3C : 0x36);
		}

		if(bossHits > 32){
			bossStage++;
			break;
		}
	}
	
	P1.item = items_none;
	P1.holding = false;
	P2.item = items_none;
	P2.holding = false;
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
