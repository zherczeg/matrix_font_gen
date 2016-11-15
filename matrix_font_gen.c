/* Copyright 2016 Zoltan Herczeg.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *data;

#define MAX_WIDTH 8

static int font_data[256];
static unsigned char font_bits[256][MAX_WIDTH];
static int first_char;
static int last_char;

int load_file(char *file_name)
{
  FILE *file = fopen(file_name, "r");
  long int size = 0;

  if (!file)
  {
    fprintf(stderr, "Cannot open file '%s'\n", file_name);
    return 1;
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);

  data = (char*)malloc(size + 1);

  if (!data)
  {
    fprintf(stderr, "Cannot allocate memory\n");
    fclose(file);
    return 1;
  }

  if (fread(data, 1, size, file) != size)
  {
    fprintf(stderr, "Cannot read file\n");
    free(data);
    fclose(file);
    return 1;
  }

  data[size] = '\0';
  fclose(file);

  printf("'%s' loaded (%d bytes)\n\n", file_name, (int)size);

  return 0;
}

int read_number(char **ptr_ref, int line)
{
  char *ptr = *ptr_ref;
  int result = 0;

  if (*ptr < '0' || *ptr > '9')
  {
    fprintf(stderr, "Line %d: decimal number expected.\n", line);
    return -1;
  }

  if (*ptr == '0')
  {
    ptr++;

    if (*ptr >= '0' && *ptr <= '9')
    {
      fprintf(stderr, "Line %d: leading zeroes are not allowed.\n", line);
      return -1;
    }

    *ptr_ref = ptr;
    return 0;
  }

  do
  {
    result = result * 10 + (*ptr - '0');

    if (result > 255)
    {
      fprintf(stderr, "Line %d: all numbers must be less than 256.\n", line);
      return -1;
    }

    ptr++;
  }
  while (*ptr >= '0' && *ptr <= '9');

  *ptr_ref = ptr;
  return result;
}

int get_width(char *ptr, int line)
{
  int width = 0;

  while (*ptr != '\n')
  {
    if (*ptr != '.' && *ptr != '#')
    {
      fprintf(stderr, "Line %d: character matrix must be a combination of dot (.) and number-sign (#) characters.\n", line);
      return -1;
    }

    width++;
    ptr++;
  }

  if (width < 1 || width > MAX_WIDTH)
  {
    fprintf(stderr, "Line %d: character width must be in [1..%d] range.\n", line, MAX_WIDTH);
    return -1;
  }

  return width;
}

int process_file()
{
  char *ptr = data;
  unsigned char *font_bits_ptr;
  int line = 1;
  int height;
  int width;
  int char_index;
  int x, y, bit;

  height = read_number(&ptr, 1);

  if (height < 0)
  {
    return -1;
  }

  if (height < 4 || height > 8)
  {
    fprintf(stderr, "Line 1: font height must be in [4..8] range.\n");
    return 1;
  }

  if (*ptr != ' ')
  {
    fprintf(stderr, "Line 1: invalid first line format.\n");
    return 1;
  }

  ptr++;

  first_char = read_number(&ptr, 1);

  if (*ptr != ' ')
  {
    fprintf(stderr, "Line 1: invalid first line format.\n");
    return 1;
  }

  ptr++;

  last_char = read_number(&ptr, 1);

  if (*ptr != '\n')
  {
    fprintf(stderr, "Line 1: invalid first line format.\n");
    return 1;
  }

  if (first_char > last_char)
  {
    fprintf(stderr, "Line 1: first character index must be lower or equal than last character index.\n");
    return 1;
  }

  ptr++;
  line++;

  while (*ptr)
  {
    if (ptr[0] == ':' && ptr[1] >= '0' && ptr[1] <= '9')
    {
      ptr++;
      char_index = read_number(&ptr, line);

      if (char_index < first_char || char_index > last_char)
      {
        fprintf(stderr, "Line %d: decimal number expected between [%d..%d]\n",
                line, first_char, last_char);
        return 1;
      }

      if (*ptr != '\n')
      {
        fprintf(stderr, "Line 1: newline expected after character index.\n");
        return 1;
      }

      char_index -= first_char;
      ptr++;
      line++;
    }
    else
    {
      if (ptr[0] < first_char || ptr[0] > last_char || ptr[1] != '\n')
      {
        fprintf(stderr, "Line %d: character expected between [%d..%d] range.\n",
                line, first_char, last_char);
        return 1;
      }

      char_index = ptr[0] - first_char;
      ptr += 2;
      line++;
    }

    font_bits_ptr = font_bits[char_index];
    memset(font_bits_ptr, 0, MAX_WIDTH);

    width = get_width(ptr, line);

    if (width == -1)
    {
      return 1;
    }

    font_data[char_index] = width;

    for (y = 0; y < height; y++)
    {
      bit = 1 << y;

      for (x = 0; x < width; x++)
      {
        if (*ptr == '#')
        {
          font_bits_ptr[x] |= bit;
        }
        else if (*ptr != '.')
        {
          fprintf(stderr, "Line %d: character matrix must be a combination of dot (.) and number-sign (#) characters.\n", line);
          return 1;
        }

        ptr++;
      }

      if (*ptr != '\n')
      {
        fprintf(stderr, "Line %d: newline expected after character matrix.\n", line);
        return 1;
      }
      line++;
      ptr++;
    }
  }

  return 0;
}

void print_matrix()
{
  int offset = 0;
  int i, j, idx;
  unsigned char byte;

  printf("\nint font_offset[%d - %d] = {\n", last_char + 1, first_char);

  for (i = 0; i < last_char + 1 - first_char; i++)
  {
    printf("  %d", offset);

    idx = i + first_char;

    if (idx < 32 || idx > 126)
    {
      printf(", /* #%d */\n", idx);
    }
    else
    {
      printf(", /* %c */\n", (char)idx);
    }

    offset += font_data[i];
  }

  printf("  %d /* END */\n};\n\n", offset);

  printf("\nint font_data[%d] = {\n", offset);

  for (i = 0; i < last_char + 1 - first_char; i++)
  {
    printf("  ");

    for (j = 0; j < font_data[i]; j++)
    {
      if (j > 0)
      {
        printf(", ");
      }

      byte = font_bits[i][j];
      if (byte < 16)
      {
        printf("0x0%x", byte);
      }
      else
      {
        printf("0x%x", byte);
      }
    }

    idx = i + first_char;

    if (font_data[i] > 0 && i < last_char - first_char)
    {
      printf(",");
    }

    if (idx < 32 || idx > 126)
    {
      printf(" /* #%d */\n", idx);
    }
    else
    {
      printf(" /* %c */\n", (char)idx);
    }
  }

  printf("};\n\n");
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Filename argument required.\n");
    return 1;
  }

  if (load_file (argv[1]))
  {
    return 1;
  }

  if (!process_file())
  {
    print_matrix();
  }

  free(data);
  return 0;
}
