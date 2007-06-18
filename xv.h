
#ifndef __XV_MAP_H
#define __XV_MAP_H

#include "common.h"

#define MODEL_XV5080			0x0010
#define MODEL_FANTOMS			0x000000
#define MODEL_FANTOMX			0x000000
#define MODEL_JUNOG			0x000015

enum xv_address_map {
	TEMP_PATCH_RHYTHM_PART1		= 0x1f000000,
};

/* Temporary Patch/Rhythm */
enum xv_temp_patch_rhythm {
	TEMP_PATCH			= 0x000000,
	TEMP_RHYTHM			= 0x100000,
};

/* Patch */
enum xv_patch {
	PATCH_COMMON			= 0x000000,
	PATCH_COMMON_MFX		= 0x000200,
	PATCH_COMMON_CHORUS		= 0x000400,
	PATCH_COMMON_REVERB		= 0x000600,
	PATCH_TMT			= 0x001000,
	PATCH_TONE_1			= 0x002000,
	PATCH_TONE_2			= 0x002200,
	PATCH_TONE_3			= 0x002400,
	PATCH_TONE_4			= 0x002600,
};

/* Patch Common */
enum xv_patch_common {
	PATCH_NAME_1			= 0x0000,
	PATCH_NAME_2,
	PATCH_NAME_3,
	PATCH_NAME_4,
	PATCH_NAME_5,
	PATCH_NAME_6,
	PATCH_NAME_7,
	PATCH_NAME_8,
	PATCH_NAME_9,
	PATCH_NAME_10,
	PATCH_NAME_11,
	PATCH_NAME_12,
	PATCH_CATEGORY,
	PATCH_LEVEL			= 0x000e,
	PATCH_PAN,
	PATCH_PRIORITY,
	PATCH_COARSE_TUNE,
	PATCH_FINE_TUNE,
	OCTAVE_SHIFT,
	STRETCH_TUNE_DEPTH,
	ANALOG_FEEL,
	MONO_POLY,
	LEGATO_SWITCH,
	LEGATO_RETRIGGER,
	PORTAMENTO_SWITCH,
	PORTAMENTO_MODE,
	PORTAMENTO_TYPE,
	PORTAMENTO_START,
	PORTAMENTO_TIME,
	CUTOFF_OFFSET			= 0x0022,
	RESONANCE_OFFSET,
	ATTACK_TIME_OFFSET,
	RELEASE_TIME_OFFSET,
	VELOCITY_SENS_OFFSET,
	PATCH_OUTPUT_ASSIGN		= 0x0027,
	TMT_CONTROL_SWITCH		= 0x0028,
	PITCH_BEND_RANGE_UP,
	PITCH_BEND_RANGE_DOWN,
	MATRIX_CTRL_1_SRC		= 0x002b,
	MATRIX_CTRL_1_DEST_1,
	MATRIX_CTRL_1_SENS_1,
	MATRIX_CTRL_1_DEST_2,
	MATRIX_CTRL_1_SENS_2,
	MATRIX_CTRL_1_DEST_3,
	MATRIX_CTRL_1_SENS_3,
	MATRIX_CTRL_1_DEST_4,
	MATRIX_CTRL_1_SENS_4,
	MATRIX_CTRL_2_SRC		= 0x0034,
	MATRIX_CTRL_2_DEST_1,
	MATRIX_CTRL_2_SENS_1,
	MATRIX_CTRL_2_DEST_2,
	MATRIX_CTRL_2_SENS_2,
	MATRIX_CTRL_2_DEST_3,
	MATRIX_CTRL_2_SENS_3,
	MATRIX_CTRL_2_DEST_4,
	MATRIX_CTRL_2_SENS_4,
	MATRIX_CTRL_3_SRC		= 0x003d,
	MATRIX_CTRL_3_DEST_1,
	MATRIX_CTRL_3_SENS_1,
	MATRIX_CTRL_3_DEST_2,
	MATRIX_CTRL_3_SENS_2,
	MATRIX_CTRL_3_DEST_3,
	MATRIX_CTRL_3_SENS_3,
	MATRIX_CTRL_3_DEST_4,
	MATRIX_CTRL_3_SENS_4,
	MATRIX_CTRL_4_SRC		= 0x0046,
	MATRIX_CTRL_4_DEST_1,
	MATRIX_CTRL_4_SENS_1,
	MATRIX_CTRL_4_DEST_2,
	MATRIX_CTRL_4_SENS_2,
	MATRIX_CTRL_4_DEST_3,
	MATRIX_CTRL_4_SENS_3,
	MATRIX_CTRL_4_DEST_4,
	MATRIX_CTRL_4_SENS_4,
};
#define PATCH_COMMON_SIZE_JG		0x004f
#define PATCH_COMMON_SIZE_XV		0x004f

/* Patch Common MFX */
enum xv_patch_common_mfx {
	MFX_TYPE			= 0x0000,
};

#define PATCH_COMMON_MFX_SIZE_JG	0x0111
#define PATCH_COMMON_MFX_SIZE_XV	0x0111

/* Patch Common Chorus */
enum xv_patch_common_chorus {
	CHORUS_TYPE			= 0x0000,
};
#define PATCH_COMMON_CHORUS_SIZE_JG	0x0054
#define PATCH_COMMON_CHORUS_SIZE_XV	0x0034

/* Patch Common Reverb */
enum xv_patch_common_reverb {
	REVERB_TYPE			= 0x0000,
};
#define PATCH_COMMON_REVERB_SIZE_JG	0x0053
#define PATCH_COMMON_REVERB_SIZE_XV	0x0053

/* Patch TMT */
enum xv_patch_tmt {
	STRUCTURE_TYPE_1_2		= 0x0000,
	BOOSTER_1_2,
	STRUCTURE_TYPE_3_4,
	BOOSTER_3_4,
	TMT_VELOCITY_CONTROL		= 0x0004,
	TMT1_TONE_SWITCH		= 0x0005,
	TMT1_KEYB_RANGE_LOWER,
	TMT1_KEYB_RANGE_UPPER,
	TMT1_KEYB_FWIDTH_LOWER,
	TMT1_KEYB_FWIDTH_UPPER,
	TMT1_VEL_RANGE_LOWER,
	TMT1_VEL_RANGE_UPPER,
	TMT1_VEL_FWIDTH_LOWER,
	TMT1_VEL_FWIDTH_UPPER,
	TMT2_TONE_SWITCH		= 0x000e,
	TMT2_KEYB_RANGE_LOWER,
	TMT2_KEYB_RANGE_UPPER,
	TMT2_KEYB_FWIDTH_LOWER,
	TMT2_KEYB_FWIDTH_UPPER,
	TMT2_VEL_RANGE_LOWER,
	TMT2_VEL_RANGE_UPPER,
	TMT2_VEL_FWIDTH_LOWER,
	TMT2_VEL_FWIDTH_UPPER,
	TMT3_TONE_SWITCH		= 0x0017,
	TMT3_KEYB_RANGE_LOWER,
	TMT3_KEYB_RANGE_UPPER,
	TMT3_KEYB_FWIDTH_LOWER,
	TMT3_KEYB_FWIDTH_UPPER,
	TMT3_VEL_RANGE_LOWER,
	TMT3_VEL_RANGE_UPPER,
	TMT3_VEL_FWIDTH_LOWER,
	TMT3_VEL_FWIDTH_UPPER,
	TMT4_TONE_SWITCH		= 0x0020,
	TMT4_KEYB_RANGE_LOWER,
	TMT4_KEYB_RANGE_UPPER,
	TMT4_KEYB_FWIDTH_LOWER,
	TMT4_KEYB_FWIDTH_UPPER,
	TMT4_VEL_RANGE_LOWER,
	TMT4_VEL_RANGE_UPPER,
	TMT4_VEL_FWIDTH_LOWER,
	TMT4_VEL_FWIDTH_UPPER,
};
#define PATCH_TMT_SIZE_JG		0x0029
#define PATCH_TMT_SIZE_XV		0x0029

/* Patch Tone */
enum xv_patch_tone {
	TONE_LEVEL			= 0x0000,
	WAVE_GROUP_TYPE			= 0x0027,
	WAVE_GROUP_ID,
	WAVE_NUMBER_L			= 0x002c,
	WAVE_NUMBER_R			= 0x0030,
	WAVE_GAIN,
};
#define PATCH_TONE_SIZE_JG		0x011a
#define PATCH_TONE_SIZE_XV		0x0109

#define DATA_SIZE(x) (((x)>0x7f)?(0x80+((x)&0xff)):(x))

struct xv_patch_category {
	char *short_name;
	char *long_name;
};

extern struct xv_patch_category patch_category[];
extern char *xv5080_wave[];
extern char *fantom_wave[];
extern char *fantomx_wave[];
extern char *junog_wave[];

int checksum	(int, uint8 *);
int send_sysex	(int, int, uint8 *);
int recv_sysex	(int, int, uint8 *);

#endif
