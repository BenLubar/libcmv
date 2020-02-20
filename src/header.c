#include "context.h"

cmv_status_t cmv_ensure_read_header(cmv_context ctx)
{
	cmv_status_t status;
	uint64_t half_frame_size;

	ctx->header = malloc(sizeof(cmv_header_t));
	if (!ctx->header)
	{
		ctx->error = 1;
		return CMV_STATUS_ERRNO;
	}

	status = cmv_read(ctx, ctx->header, sizeof(cmv_header_t));
	if (status)
	{
		ctx->error = 1;
		return status;
	}

	ctx->header->magic = le32toh(ctx->header->magic);
	ctx->header->columns = le32toh(ctx->header->columns);
	ctx->header->rows = le32toh(ctx->header->rows);
	ctx->header->delayrate = le32toh(ctx->header->delayrate);

	if (ctx->header->version < CMV_VERSION_0 || ctx->header->version > CMV_VERSION_1)
	{
		ctx->error = 1;
		return CMV_STATUS_BAD_VERSION;
	}

	if (!ctx->header->columns || !ctx->header->rows)
	{
		ctx->error = 1;
		return CMV_STATUS_OUT_OF_RANGE;
	}

	/* cast to uint64 to avoid having to deal with overflow */
	half_frame_size = ((uint64_t)ctx->header->columns) * ((uint64_t)ctx->header->rows);
	if (half_frame_size > CMV_CHUNK_UNCOMPRESSED_LENGTH_MAX / 2)
	{
		ctx->error = 1;
		return CMV_STATUS_OUT_OF_RANGE;
	}

	if (ctx->header->delayrate == 0)
	{
		ctx->header->delayrate = 2;
	}

	return CMV_STATUS_OK;
}

cmv_status_t cmv_read_header(cmv_context ctx, cmv_header_t *header)
{
	cmv_status_t status;

	status = cmv_ensure_read(ctx, CMV_READ_STATE_HEADER);

	if (ctx->header)
		memcpy(header, ctx->header, sizeof(cmv_header_t));

	return status;
}
