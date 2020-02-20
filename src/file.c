#include "context.h"

cmv_context cmv_context_new_from_file(FILE *file)
{
	cmv_context context;
	uint32_t frame, channel;

	if (!file)
		return NULL;

	context = (cmv_context)malloc(sizeof(struct cmv_context_t));
	if (!context)
		return NULL;

	context->file = file;
	context->header = NULL;
	context->sounds = NULL;
	for (frame = 0; frame < 200; frame++)
	{
		for (channel = 0; channel < 16; channel++)
		{
			context->sound_timings[frame][channel] = CMV_SOUND_TIMING_UNUSED;
		}
	}
	context->all_flags = 0;
	return context;
}

cmv_status_t cmv_read(cmv_context ctx, void *ptr, size_t size)
{
	if (ctx->eof)
		return CMV_STATUS_BAD_FILE;

	/* read entire buffer as one element for simplicity */
	if (fread(ptr, size, 1, ctx->file))
		return CMV_STATUS_OK;

	if (ferror(ctx->file))
		ctx->error = 1;
	if (feof(ctx->file))
		ctx->eof = 1;

	return CMV_STATUS_BAD_FILE;
}

