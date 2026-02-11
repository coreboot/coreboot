#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later

"""
This utility generates a compressed bitmapped font table from a TTF/OTF file.
The output is a C source file containing:
1. Font dimensions and range macros.
2. A normalized (left-aligned) bitmapped font table.
3. A character width table for proportional spacing.

Usage:
  python generate_font.py <path_to_ttf> --width 24 --height 32 > font_table.c
"""
import argparse
from PIL import Image, ImageFont, ImageDraw
import sys
import os

# PRINTABLE ASCII RANGE
START_CHAR = 32
END_CHAR = 126
NUM_CHARS = END_CHAR - START_CHAR + 1

def generate_c_table(font_path, canvas_width, canvas_height):
    if not os.path.exists(font_path):
        print(f"Error: Font file '{font_path}' not found.", file=sys.stderr)
        sys.exit(1)

    # Validation: Width must not exceed 32 bits for uint32_t storage
    if canvas_width > 32:
        print(f"Error: Requested width ({canvas_width}) exceeds 32-bit capacity. "
              "The current renderer supports up to 32 pixels wide.", file=sys.stderr)
        sys.exit(1)

    # Validation: Basic sanity check for positive dimensions
    if canvas_width <= 0 or canvas_height <= 0:
        print("Error: Width and Height must be positive integers.", file=sys.stderr)
        sys.exit(1)

    # Use canvas_width as font size to maintain original calculation style
    font_size = canvas_width
    y_offset = 0

    try:
        font = ImageFont.truetype(font_path, font_size)
    except OSError:
        print(f"Error: Could not open font file '{font_path}'.", file=sys.stderr)
        sys.exit(1)

    # Header for the generated C file
    print("/* SPDX-License-Identifier: GPL-2.0-or-later */\n")
    print("/*")
    print(" * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.")
    print(f" * Source: {os.path.basename(font_path)}")
    print(f" * Command: python {os.path.basename(__file__)} {font_path} --width {canvas_width} --height {canvas_height}")
    print(" */\n")
    print("#include <stdint.h>\n")

    # 1. Output the Macros for C code usage
    print(f"#define FONT_WIDTH      {canvas_width}")
    print(f"#define FONT_HEIGHT     {canvas_height}")
    print(f"#define FONT_START_CHAR {START_CHAR}")
    print(f"#define FONT_END_CHAR   {END_CHAR}")
    print(f"#define FONT_NUM_CHARS  (FONT_END_CHAR - FONT_START_CHAR + 1)\n")

    widths = []

    # Choose data type based on width
    data_type = "uint32_t"
    hex_format = "08x" if canvas_width > 16 else "04x"
    bit_depth = 32 if canvas_width > 16 else 16

    # 2. Generate packed font table
    print(f"const {data_type} font_table[FONT_NUM_CHARS][FONT_HEIGHT] = {{")

    for char_code in range(START_CHAR, END_CHAR + 1):
        char = chr(char_code)
        image = Image.new("1", (canvas_width, canvas_height), 0)
        draw = ImageDraw.Draw(image)
        draw.text((0, y_offset), char, font=font, fill=1)

        pixels = list(image.getdata())
        rows = []
        global_mask = 0

        for y in range(canvas_height):
            row_val = 0
            base = y * canvas_width
            for x in range(canvas_width):
                if pixels[base + x]:
                    # Map pixels to bits: Leftmost pixel is highest bit
                    row_val |= (1 << ((canvas_width - 1) - x))
            rows.append(row_val)
            global_mask |= row_val

        # Dead space removal / Normalization
        left_shift = 0
        actual_width = 0

        if global_mask > 0:
            # Find the leftmost pixel column
            leftmost_col = 0
            for i in range(canvas_width):
                if (global_mask >> (canvas_width - 1 - i)) & 1:
                    leftmost_col = i
                    break

            # Find the rightmost pixel column
            rightmost_col = 0
            for i in range(canvas_width):
                if (global_mask >> i) & 1:
                    rightmost_col = (canvas_width - 1) - i
                    break

            # Width is the horizontal span of active pixels
            actual_width = rightmost_col - leftmost_col + 1

            # left_shift ensures character is left-aligned to MSB of the data type
            left_shift = (bit_depth - canvas_width) + leftmost_col
        else:
            actual_width = canvas_width // 3  # Default width for space

        widths.append(actual_width)

        # Pre-shift values to be MSB-aligned and mask based on bit depth
        mask_val = 0xFFFFFFFF if bit_depth == 32 else 0xFFFF
        hex_values = [f"0x{(row << left_shift) & mask_val:{hex_format}}" for row in rows]
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
    parser = argparse.ArgumentParser(description="Generate bitmapped font table.")
    parser.add_argument("font_path", help="Path to TTF/OTF font file")
    parser.add_argument("--width", type=int, default=24, help="Canvas width (default: 24)")
    parser.add_argument("--height", type=int, default=32, help="Canvas height (default: 32)")

    args = parser.parse_args()
    generate_c_table(args.font_path, args.width, args.height)
