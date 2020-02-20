#include "context.h"

#define ZLIB_CONST
#include <zlib.h>

cmv_status_t cmv_decompress_chunk(const cmv_context ctx, cmv_chunk_uncompressed_t *buffer, const cmv_chunk_compressed_t *chunk)
{
	size_t bytes_read;
	z_stream strm;
	int status;

	if (ctx->error || !ctx->header)
		return CMV_STATUS_BAD_CONTEXT;

	buffer->columns = ctx->header->columns;
	buffer->rows = ctx->header->rows;
	buffer->frame_size = buffer->columns * buffer->rows * 2;
	buffer->frame_count = 0;

	strm.next_in = (const Bytef*)&chunk->compressed_data[0];
	strm.avail_in = chunk->compressed_length;

	strm.next_out = (Bytef *)&buffer->data[0];
	strm.avail_out = sizeof(buffer->data);

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	status = inflateInit(&strm);
	if (status != Z_OK)
		return CMV_STATUS_ZLIB;

	status = inflate(&strm, Z_FINISH);
	if (status != Z_STREAM_END)
	{
		(void)inflateEnd(&strm);
		return CMV_STATUS_ZLIB;
	}

	bytes_read = sizeof(buffer->data) - strm.avail_out;
	buffer->frame_count = bytes_read / buffer->frame_size;
	if (bytes_read != buffer->frame_count * buffer->frame_size)
	{
		(void)inflateEnd(&strm);
		return CMV_STATUS_OUT_OF_RANGE;
	}

	status = inflateEnd(&strm);
	return status == Z_OK ? CMV_STATUS_OK : CMV_STATUS_ZLIB;
}
