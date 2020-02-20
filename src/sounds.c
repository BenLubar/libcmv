#include "context.h"
#include <errno.h>

cmv_status_t cmv_ensure_read_sounds(cmv_context ctx)
{
	cmv_status_t status;
	uint32_t frame, channel, i;
	size_t len;

	ctx->sounds = malloc(sizeof(uint32_t));
	if (!ctx->sounds)
	{
		ctx->error = 1;
		return CMV_STATUS_ERRNO;
	}

	if (ctx->header->version < CMV_VERSION_1)
	{
		ctx->sounds->count = 0;
		return CMV_STATUS_OK;
	}

	status = cmv_read(ctx, ctx->sounds, sizeof(uint32_t));
	if (status)
	{
		ctx->error = 1;
		return status;
	}

	ctx->sounds->count = le32toh(ctx->sounds->count);
	if (ctx->sounds->count)
	{
		cmv_sounds_t *sounds_realloc = realloc(ctx->sounds, sizeof(uint32_t) + sizeof(cmv_sound_name_t) * ctx->sounds->count);
		if (!sounds_realloc)
		{
			int stored_errno = errno;
			free(ctx->sounds);
			errno = stored_errno;
			ctx->sounds = NULL;
			ctx->error = 1;
			return CMV_STATUS_ERRNO;
		}
		ctx->sounds = sounds_realloc;

		status = cmv_read(ctx, &ctx->sounds->names[0], sizeof(cmv_sound_name_t) * ctx->sounds->count);
		if (status)
		{
			ctx->error = 1;
			return status;
		}

		for (i = 0; i < ctx->sounds->count; i++)
		{
			len = strnlen(ctx->sounds->names[i], sizeof(cmv_sound_name_t));
			if (len == 0 || len == sizeof(cmv_sound_name_t))
			{
				ctx->error = 1;
				return CMV_STATUS_OUT_OF_RANGE;
			}
		}
	}

	status = cmv_read(ctx, &ctx->sound_timings, sizeof(cmv_sound_timings_t));
	if (status)
	{
		ctx->error = 1;
		return status;
	}

	for (frame = 0; frame < 200; frame++)
	{
		for (channel = 0; channel < 16; channel++)
		{
			ctx->sound_timings[frame][channel] = le32toh(ctx->sound_timings[frame][channel]);
			if (ctx->sound_timings[frame][channel] >= ctx->sounds->count)
			{
				ctx->sound_timings[frame][channel] = CMV_SOUND_TIMING_UNUSED;
			}
		}
	}

	return CMV_STATUS_OK;
}

cmv_status_t cmv_read_sound_name(cmv_context ctx, cmv_sound_name_t *name, uint32_t index)
{
	cmv_status_t status;

	(*name)[0] = 0;

	status = cmv_ensure_read(ctx, CMV_READ_STATE_SOUNDS);
	if (status)
		return status;

	if (index < ctx->sounds->count)
	{
		memcpy(name, ctx->sounds->names[index], sizeof(cmv_sound_name_t));
	}

	return CMV_STATUS_OK;
}

cmv_status_t cmv_read_sound_timings(cmv_context ctx, cmv_sound_timings_t *timings)
{
	cmv_status_t status;

	status = cmv_ensure_read(ctx, CMV_READ_STATE_SOUNDS);
	if (status)
		return status;

	memcpy(timings, &ctx->sound_timings, sizeof(cmv_sound_timings_t));

	return CMV_STATUS_OK;
}
