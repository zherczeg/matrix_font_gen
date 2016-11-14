=============================================
          DOT MATRIX FONT GENERATOR
=============================================

This proejct allows generating dot matrix fonts. The font
height must be between 4 and 8 dots but the width can be
variable length.

Project compilation: run build.sh

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
'a' represents index 97), or a colon followed by a number
(colon followed by a newline is a colon).

The next HEIGHT lines defines the dot matrix of the
character. The length of each line must be the same and
only dot (.) and number-sign (#) characters are allowed.
The dot represents an unlit (0) pixel and number-sign
represents a lit (1) pixel.

-------------------
   Output format
-------------------

The output is two arrays. The first array contains the
starting offset of each character in the second array.
The width of a character can be calculated by
substracting the starting offset of the next character
from the starting offset of the current character. The
item count of this array is always:

  ((LAST_CHARACTER + 1) - FIRST_CHARACTER) + 1

Therefore the size of the last character can be computed
as well.

The second array contains a bit matrix for each character.
One byte is assigned for every column of the font so the
size in bytes of a character is equal to its width. Bit 0
contains the status of the topmost pixel (0: unlit, 1:lit),
bit 1 contains the pixel below and so on.



