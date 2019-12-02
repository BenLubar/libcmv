# CMV container

All integers are 32-bit unsigned little-endian integers unless otherwise
specified. "Strict" refers to compliant encoders. Compliant decoders should
handle cases where strict encoding practices are not followed.

## Magic Number (version 0+)

A single integer representing the version:

- **Version 0:** 10000 (`10 27 00 00`)
- **Version 1:** 10001 (`11 27 00 00`)

## Header (version 0+)

The header immediately follows the magic number.

- int **Columns** *The number of columns on the screen.*
- int **Rows** *The number of rows on the screen.*
- int **DelayRate** *The number of centiseconds (10 millisecond intervals) per frame.* (Strict: this must be nonzero)

## Sounds (version 1+)

The sounds block follows the header in version 1.

- int **SoundCount** *The number of sounds referenced by this CMV.*
- char **SoundFile\[SoundCount\]\[50\]** *Null terminated strings with the filename of the sound being referenced. The extension and directory path are omitted.*
- int **SoundTiming\[200\]\[16\]** *The sounds to play on the first 200 frames. There are 16 channels available. Numbers greater than or equal to SoundCount are unused.* (Strict: unused entries are set to UINT32\_MAX)

## Frames (version 0+)

Repeated until the end of the file:

- int **Size** *The size of this chunk.*
- **Data\[Size\]** *The zlib-compressed data.*

Each chunk contains up to 200 frames in the CMV codec. Chunks must have at least one frame and must not contain partial frames. (Strict: all chunks except the final chunk must contain exactly 200 frames)

# CMV codec

Each frame in the CMV codec is 2\*Columns\*Rows bytes. There is no versioning information in the CMV codec, but future versions of the CMV container may change the codec.

The X coordinate ranges from 0 to Columns-1 and the Y coordinate ranges from 0 to Rows-1.

## Glyph

Byte **X+\(Columns\*Y\)**. An index into IBM PC Code Page 437. The rendering of glyphs is implementation-defined.

## Attributes

Byte **Columns\*Rows+X+\(Columns\*Y\)**.

- Bits 0-2: color **Foreground**
- Bits 3-5: color **Background**
- Bit 6: int **Intensity**
- Bit 7: reserved (Strict: 0)

Or in text form: `0ibbbfff`

Colors are as follows:

- 0: Black
- 1: Blue
- 2: Green
- 3: Cyan
- 4: Red
- 5: Magenta
- 6: Brown
- 7: Light grey

With the intensity bit set, the colors are as follows:

- 0: Dark grey
- 1: Light blue
- 2: Light green
- 3: Light cyan
- 4: Light red
- 5: Fuchsia
- 6: Yellow
- 7: White

The exact RGB coordinates for each color is implementation-defined.
