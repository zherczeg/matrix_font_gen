=============================================
          DOT MATRIX FONT GENERATOR
=============================================

This project allows generating dot matrix fonts. The font
height must be between 4 and 8 dots but the width can be
variable length.

Script for compiling the generator: ./build.sh

------------------
   Input format
------------------

Note: font_5xN.txt contains an example font.

The first line must contains the following three numbers:

  HEIGHT FIRST_CHARACTER LAST_CHARACTER

The HEIGHT must be between 4 and 8 (inclusive) and it
specifies the height of the font. The FIRST_CHARACTER and
LAST_CHARACTER must be between 0 and 255 (inclusive) and
they specify the first and last character index. The font
must contain at least one character so FIRST_CHARACTER
must be less or equal than LAST_CHARACTER.

The following lines specify the dot matrix of characters.

The first line defines the character index of the next
character. This index can be a single character (e.g.
'a' represents index 97), or a colon followed by a number.
A colon followed by a newline is a colon character whose
index is 58.

The next HEIGHT lines defines the dot matrix of the
character. The length of each line must be the same and
only dot (.) and number-sign (#) characters are allowed.
The dot represents an unlit (0) pixel and number-sign
represents a lit (1) pixel.

-------------------
   Output format
-------------------

The output is two arrays: font_offset and font_data.

The font_data contains a bit matrix for each character.
One byte represents one column of the character bit
matrix so the size in bytes of a character is equal
to its width. The columns are ordered from left to
right. Bit 0 contains the status of the topmost pixel
(0: unlit, 1:lit), bit 1 contains the pixel below and
so on.

An example:

#.
.#
#.
#.
.#

The following two bytes are generated from this input:
0x0d (00001101 in binary) and 0x12 (00010010 in binary).
The unused bits of the byte are zeroed.

The font_offset contains the starting offset of each
character in the font_data array. The starting index
of character N can be computed as follows:

  start_index = font_offset[N - FIRST_CHARACTER]

The width of a character can be calculated by
substracting the starting offset of the next character
from the starting offset of the current character:

  width = font_offset[(N + 1) - FIRST_CHARACTER]
          - font_offset[N - FIRST_CHARACTER]

The size of the last character can also be computed
since the item count of font_offset is always:

  ((LAST_CHARACTER + 1) - FIRST_CHARACTER) + 1




