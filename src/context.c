#include "context.h"
#include <endian.h>

void cmv_context_free(cmv_context ctx)
{
	if (ctx)
	{
		free(ctx->header);
		free(ctx->sounds);
	}
	free(ctx);
}

cmv_status_t cmv_ensure_read(cmv_context ctx, cmv_read_state_t goal)
{
	cmv_status_t status = ctx->error ? CMV_STATUS_BAD_CONTEXT : CMV_STATUS_OK;

#define DEFINE_STATE(field, state) \
	if (status || goal < state) \
		return status; \
	if (!ctx->field) \
		status = cmv_ensure_read_ ## field(ctx)

	DEFINE_STATE(header, CMV_READ_STATE_HEADER);
	DEFINE_STATE(sounds, CMV_READ_STATE_SOUNDS);

#undef DEFINE_STATE

	return status;
}

cmv_status_t cmv_read_chunk(cmv_context ctx, cmv_chunk_compressed_t *chunk)
{
	cmv_status_t status;

	status = cmv_ensure_read(ctx, CMV_READ_STATE_ALL_HEADERS);
	if (status)
		return status;

	status = cmv_read(ctx, &chunk->compressed_length, sizeof(uint32_t));
	if (status == CMV_STATUS_BAD_FILE && !ctx->error)
	{
		chunk->compressed_length = 0;
		return CMV_STATUS_OK;
	}
	else if (status)
	{
		return status;
	}

	chunk->compressed_length = le32toh(chunk->compressed_length);
	if (chunk->compressed_length > sizeof(chunk->compressed_data))
	{
		ctx->error = 1;
		return CMV_STATUS_OUT_OF_RANGE;
	}

	status = cmv_read(ctx, &chunk->compressed_data[0], chunk->compressed_length);
	if (status)
	{
		ctx->error = 1;
	}
	return status;
}
