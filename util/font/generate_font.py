#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later

"""
This utility generates a compressed bitmapped font table from a TTF/OTF file.
The output is a C source file containing:
1. Font dimensions and range macros.
2. A normalized (left-aligned) bitmapped font table.
3. A character width table for proportional spacing.

Usage:
  python generate_font.py <path_to_ttf> > font_table.c
"""

from PIL import Image, ImageFont, ImageDraw
import sys
import os

# FONT DIMENSIONS
FONT_SIZE = 19
CANVAS_WIDTH = 16
CANVAS_HEIGHT = 24
Y_OFFSET = -2

# PRINTABLE ASCII RANGE
START_CHAR = 32
END_CHAR = 126
NUM_CHARS = END_CHAR - START_CHAR + 1

def generate_c_table(font_path):
    if not os.path.exists(font_path):
        print(f"Error: Font file '{font_path}' not found.", file=sys.stderr)
        sys.exit(1)

    try:
        font = ImageFont.truetype(font_path, FONT_SIZE)
    except OSError:
        print(f"Error: Could not open font file '{font_path}'.", file=sys.stderr)
        sys.exit(1)

    # Header for the generated C file
    print("/* SPDX-License-Identifier: GPL-2.0-or-later */\n")
    print("/*")
    print(" * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.")
    print(f" * Source: {os.path.basename(font_path)}")
    print(f" * Command: python util/font/{os.path.basename(__file__)} {font_path}")
    print(" */\n")
    print("#include <stdint.h>\n")

    # 1. Output the Macros for C code usage
    print(f"#define FONT_WIDTH      {CANVAS_WIDTH}")
    print(f"#define FONT_HEIGHT     {CANVAS_HEIGHT}")
    print(f"#define FONT_START_CHAR {START_CHAR}")
    print(f"#define FONT_END_CHAR   {END_CHAR}")
    print(f"#define FONT_NUM_CHARS  (FONT_END_CHAR - FONT_START_CHAR + 1)\n")

    widths = []

    # 2. Generate packed font table with block formatting
    print(f"const uint16_t font_table[FONT_NUM_CHARS][FONT_HEIGHT] = {{")

    for char_code in range(START_CHAR, END_CHAR + 1):
        char = chr(char_code)
        image = Image.new("1", (CANVAS_WIDTH, CANVAS_HEIGHT), 0)
        draw = ImageDraw.Draw(image)
        draw.text((0, Y_OFFSET), char, font=font, fill=1)

        pixels = list(image.getdata())
        rows = []
        global_mask = 0

        for y in range(CANVAS_HEIGHT):
            row_val = 0
            base = y * CANVAS_WIDTH
            for x in range(CANVAS_WIDTH):
                if pixels[base + x]:
                    row_val |= (1 << ((CANVAS_WIDTH - 1) - x))
            rows.append(row_val)
            global_mask |= row_val

        # Dead space removal / Normalization
        left_shift = 0
        actual_width = 0
        if global_mask > 0:
            temp_mask = global_mask
            while not (temp_mask & 0x8000):
                temp_mask <<= 1
                left_shift += 1

            rightmost_bit = 0
            for i in range(CANVAS_WIDTH):
                if (global_mask >> i) & 1:
                    rightmost_bit = i
                    break
            actual_width = ((CANVAS_WIDTH - 1) - left_shift) - rightmost_bit + 1
        else:
            actual_width = 6 # Default space width

        widths.append(actual_width)

        # Format the pre-shifted hex rows into blocks of 8
        hex_values = [f"0x{(row << left_shift) & 0xFFFF:04x}" for row in rows]
        char_repr = f"'{char}'" if char not in ["'", "\\"] else f"'\\{char}'"

        print(f"\t[0x{char_code:02x} - FONT_START_CHAR] = {{")
        for i in range(0, len(hex_values), 8):
            line = ", ".join(hex_values[i:i+8])
            print(f"\t\t{line},")
        print(f"\t}}, /* {char_repr} */")

    print("};\n")

    # 3. Output Width Table
    print(f"const uint8_t font_widths[FONT_NUM_CHARS] = {{")
    for i, w in enumerate(widths):
        char_code = i + START_CHAR
        print(f"\t[0x{char_code:02x} - FONT_START_CHAR] = {w},")
    print("};")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: python3 {sys.argv[0]} <path_to_ttf_file>")
        sys.exit(1)
    generate_c_table(sys.argv[1])
