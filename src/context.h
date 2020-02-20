#pragma once

#define COMPILING_LIBCMV

#include <cmv.h>

/* common includes */
#include <stdlib.h>
#include <string.h>

#ifdef _GNUC
#define CMV_INTERNAL_FUNC __attribute__((visibility("hidden")))
#else
#define CMV_INTERNAL_FUNC
#endif

typedef enum cmv_read_state_t
{
	CMV_READ_STATE_NONE,
	CMV_READ_STATE_HEADER,
	CMV_READ_STATE_SOUNDS,
	CMV_READ_STATE_ALL_HEADERS = CMV_READ_STATE_SOUNDS
} cmv_read_state_t;

typedef struct cmv_sounds_t
{
	uint32_t count;
	cmv_sound_name_t names[];
} cmv_sounds_t;

struct cmv_context_t
{
	FILE *file;
	cmv_header_t *header;
	cmv_sounds_t *sounds;
	cmv_sound_timings_t sound_timings;
	union
	{
		struct
		{
			uint32_t error : 1;
			uint32_t eof : 1;
		};
		uint32_t all_flags;
	};
};

cmv_status_t CMV_INTERNAL_FUNC cmv_read(cmv_context, void *, size_t);
cmv_status_t CMV_INTERNAL_FUNC cmv_ensure_read(cmv_context, cmv_read_state_t);
cmv_status_t CMV_INTERNAL_FUNC cmv_ensure_read_header(cmv_context);
cmv_status_t CMV_INTERNAL_FUNC cmv_ensure_read_sounds(cmv_context);
