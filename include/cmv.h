#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#ifndef CMV_FUNC
#ifdef _WIN32
#ifdef COMPILING_LIBCMV
#define CMV_FUNC __declspec(dllexport)
#else
#define CMV_FUNC __declspec(dllimport)
#endif
#else
#if defined(COMPILING_LIBCMV) && defined(_GNUC)
#define CMV_FUNC __attribute__((visibility("default")))
#else
#define CMV_FUNC
#endif
#endif
#endif

#ifdef _GNUC
#define CMV_FUNC_ATTRS \
	__attribute__((nonnull, warn_unused_result, nothrow))
#define CMV_VOID_FUNC_ATTRS \
	__attribute__((nonnull, nothrow))
#define CMV_INLINE_FUNC_ATTRS \
	__attribute__((nonnull, warn_unused_result, nothrow, pure))
#define CMV_WRITE_ONLY(idx) \
	__attribute__((access(write_only, idx))
#else
#define CMV_FUNC_ATTRS
#define CMV_VOID_FUNC_ATTRS
#define CMV_INLINE_FUNC_ATTRS
#define CMV_WRITE_ONLY(idx)
#endif

/**
 * cmv_version_t - the version of a curses movie
 */
typedef enum cmv_version_t
{
	/** v0 - base version */
	CMV_VERSION_0 = 10000,
	/** v1 - adds sound support */
	CMV_VERSION_1 = 10001
} cmv_version_t;

/**
 * cmv_header_t - the header at the start of a curses movie
 */
typedef struct cmv_header_t
{
	union
	{
		/* ensure this field is 32 bits. */
		uint32_t magic;
		/** version number */
		cmv_version_t version;
	};
	/** number of columns on the screen */
	uint32_t columns;
	/** number of rows on the screen */
	uint32_t rows;
	/** number centiseconds per frame. delayrate = 2 means 50 FPS. */
	uint32_t delayrate;
} cmv_header_t;

/**
 * cmv_glyph_t - a character from IBM code page 437
 */
typedef uint8_t cmv_glyph_t;

/**
 * cmv_attr_t - describes the presentation of a glyph
 */
typedef union cmv_attr_t
{
	struct
	{
		/** foreground color */
		uint8_t fg : 3;
		/** background color */
		uint8_t bg : 3;
		/** intensity bit */
		uint8_t bright : 1;
	};
	uint8_t attr;
} cmv_attr_t;

/**
 * cmv_sound_name_t - sound file declaration
 *
 * null-terminated filename (not including extension or directory path)
 * of the sound file to be played
 */
typedef char cmv_sound_name_t[50];

/** value to set unused timing fields to */
#define CMV_SOUND_TIMING_UNUSED 0xffffffff

/** cmv_sound_id_t - sound file declaration index */
typedef uint32_t cmv_sound_id_t;

#define CMV_SOUND_FRAMES 200
#define CMV_SOUND_CHANNELS 16

/**
 * cmv_sound_timings_t - Sound IDs for the first 200 frames. 
 */
typedef cmv_sound_id_t cmv_sound_timings_t[CMV_SOUND_FRAMES][CMV_SOUND_CHANNELS];

#define CMV_CHUNK_COMPRESSED_LENGTH_MAX 1000000
typedef struct cmv_chunk_compressed_t
{
	uint32_t compressed_length;
	uint8_t compressed_data[CMV_CHUNK_COMPRESSED_LENGTH_MAX];
} cmv_chunk_compressed_t;

typedef union cmv_data_byte_t
#ifdef _GNUC
__attribute__((transparent_union))
#endif
{
	cmv_glyph_t glyph;
	cmv_attr_t attr;
} cmv_data_byte_t;

#define CMV_CHUNK_UNCOMPRESSED_LENGTH_MAX 800000
typedef struct cmv_chunk_uncompressed_t
{
	uint32_t columns, rows;
	uint32_t frame_size;

	uint32_t frame_count;

	cmv_data_byte_t data[CMV_CHUNK_UNCOMPRESSED_LENGTH_MAX];
} cmv_chunk_uncompressed_t;

/**
 * cmv_context - opaque library context type
 *
 * after cmv_read_chunk has returned, all other functions can safely be called concurrently
 */
typedef struct cmv_context_t *cmv_context;

/**
 * cmv_context_new_from_file - creates a cmv_context
 *
 * returns null if allocating space for the context fails.
 */
cmv_context cmv_context_new_from_file(FILE *) CMV_FUNC_ATTRS;
/**
 * cmv_context_free - cleans up a cmv_context
 *
 * after calling this function on a context, the context must no longer be used
 */
void cmv_context_free(cmv_context) CMV_VOID_FUNC_ATTRS;

/**
 * cmv_status_t - result value from functions in this library
 *
 * zero is success, negative values are failure, positive values are reserved
 */
typedef enum cmv_status_t
{
	/** no error occurred */
	CMV_STATUS_OK = 0,
	/** the error is accessible via the errno variable defined in errno.h */
	CMV_STATUS_ERRNO = -1,
	/** zlib-compressed data was invalid */
	CMV_STATUS_ZLIB = -2,
	/** a number was too large or too small to be part of a valid CMV */
	CMV_STATUS_OUT_OF_RANGE = -3,
	/** the CMV magic number was not a known CMV version */
	CMV_STATUS_BAD_VERSION = -4,
	/** attempting to read the file failed in some way */
	CMV_STATUS_BAD_FILE = -5,
	/** the context already returned an error that should have been handled */
	CMV_STATUS_BAD_CONTEXT = -6
} cmv_status_t;

/**
 * cmv_read_header - reads the header from the CMV file.
 */
cmv_status_t CMV_FUNC cmv_read_header(cmv_context, cmv_header_t *) CMV_FUNC_ATTRS CMV_WRITE_ONLY(2);
/**
 * cmv_read_sound_name - reads a sound name from the CMV file.
 */
cmv_status_t CMV_FUNC cmv_read_sound_name(cmv_context, cmv_sound_name_t *, cmv_sound_id_t) CMV_FUNC_ATTRS CMV_WRITE_ONLY(2);
/**
 * cmv_read_sound_timings - reads sound timing information from the CMV file.
 */
cmv_status_t CMV_FUNC cmv_read_sound_timings(cmv_context, cmv_sound_timings_t *) CMV_FUNC_ATTRS CMV_WRITE_ONLY(2);

/**
 * cmv_read_chunk - reads a chunk of compressed data from the CMV file.
 */
cmv_status_t CMV_FUNC cmv_read_chunk(cmv_context, cmv_chunk_compressed_t *) CMV_FUNC_ATTRS CMV_WRITE_ONLY(2);
/**
 * cmv_decompress_chunk - decompresses a chunk of frame data from the CMV file.
 */
cmv_status_t CMV_FUNC cmv_decompress_chunk(const cmv_context, cmv_chunk_uncompressed_t *, const cmv_chunk_compressed_t *) CMV_FUNC_ATTRS CMV_WRITE_ONLY(2);

static inline cmv_glyph_t cmv_frame_glyph(const cmv_chunk_uncompressed_t *chunk, uint32_t frame, uint32_t x, uint32_t y) CMV_INLINE_FUNC_ATTRS
{
	assert(chunk);
	assert(frame < chunk->frame_count);
	assert(x < chunk->columns);
	assert(y < chunk->rows);
	assert(chunk->frame_size == chunk->columns * chunk->rows * 2);

	return chunk->data[frame * chunk->frame_size + x * chunk->rows + y].glyph;
}

static inline cmv_attr_t cmv_frame_attr(const cmv_chunk_uncompressed_t *chunk, uint32_t frame, uint32_t x, uint32_t y) CMV_INLINE_FUNC_ATTRS
{
	assert(chunk);
	assert(frame < chunk->frame_count);
	assert(x < chunk->columns);
	assert(y < chunk->rows);
	assert(chunk->frame_size == chunk->columns * chunk->rows * 2);

	return chunk->data[frame * chunk->frame_size + (chunk->columns + x) * chunk->rows + y].attr;
}

/**
 * cmv_draw_tile - draws a tile onto an ARGB image buffer.
 */
void CMV_FUNC cmv_draw_tile(uint32_t *buf, size_t offset, size_t stride,
		cmv_glyph_t ch, cmv_attr_t attr) CMV_VOID_FUNC_ATTRS;

#undef CMV_FUNC_ATTRS
#undef CMV_VOID_FUNC_ATTRS
#undef CMV_INLINE_FUNC_ATTRS
#undef CMV_WRITE_ONLY
