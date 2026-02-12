#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later

"""
This utility generates an anti-aliased (smoothed) font table from a TTF/OTF file.
The output is a C source file containing:
1. Font dimensions and range macros.
2. An 8-bit alpha map font table (0=transparent, 255=opaque).
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

    # RESTRICTION: Width and Height checks for firmware safety
    if canvas_width > 64 or canvas_height > 64:
        print(f"Warning: Large dimensions ({canvas_width}x{canvas_height}) "
              "will consume significant flash memory.", file=sys.stderr)

    # Validation: Basic sanity check for positive dimensions
    if canvas_width <= 0 or canvas_height <= 0:
        print("Error: Width and Height must be positive integers.", file=sys.stderr)
        sys.exit(1)

    # Use canvas_width as font size to maintain original calculation style
    font_size = canvas_width

    try:
        font = ImageFont.truetype(font_path, font_size)
    except OSError:
        print(f"Error: Could not open font file '{font_path}'.", file=sys.stderr)
        sys.exit(1)

    # Check for vertical clipping based on font metrics
    ascent, descent = font.getmetrics()
    total_font_height = ascent + descent
    if total_font_height > canvas_height:
        print(f"Warning: Font vertical size ({total_font_height}px) exceeds "
              f"canvas height ({canvas_height}px).", file=sys.stderr)
        y_offset = 0
    else:
        y_offset = (canvas_height - total_font_height) // 2

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

    print(f"const uint8_t font_table[FONT_NUM_CHARS][FONT_HEIGHT * FONT_WIDTH] = {{")

    clipped_glyphs = []

    for char_code in range(START_CHAR, END_CHAR + 1):
        char = chr(char_code)
        image = Image.new("L", (canvas_width, canvas_height), 0)
        draw = ImageDraw.Draw(image)
        draw.text((0, y_offset), char, font=font, fill=255)

        pixels = list(image.getdata())

        leftmost = canvas_width
        rightmost = 0
        has_pixels = False

        for y in range(canvas_height):
            for x in range(canvas_width):
                if pixels[y * canvas_width + x] > 0:
                    if x < leftmost: leftmost = x
                    if x > rightmost: rightmost = x
                    has_pixels = True

        if has_pixels:
            actual_width = rightmost - leftmost + 1
            if actual_width >= canvas_width:
                clipped_glyphs.append(char)
        else:
            actual_width = canvas_width // 3  # Default width for space

        widths.append(actual_width)

        char_repr = f"'{char}'" if char not in ["'", "\\"] else f"'\\{char}'"

        print(f"\t[0x{char_code:02x} - FONT_START_CHAR] = {{")

        # Format with line breaks every 8 entries
        for i in range(0, len(pixels), canvas_width):
            row = pixels[i : i + canvas_width]
            for j in range(0, len(row), 8):
                chunk = row[j : j + 8]
                line = ", ".join([f"0x{p:02x}" for p in chunk])
                print(f"\t\t{line},")

        print(f"\t}}, /* {char_repr} */")

    print("};\n")

    # 3. Output Width Table
    print(f"const uint8_t font_widths[FONT_NUM_CHARS] = {{")
    for i, w in enumerate(widths):
        char_code = i + START_CHAR
        print(f"\t[0x{char_code:02x} - FONT_START_CHAR] = {w},")
    print("};")

    if clipped_glyphs:
        print(f"/* Warning: Characters clipped: {' '.join(clipped_glyphs)} */", file=sys.stderr)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate anti-aliased font table.")
    parser.add_argument("font_path", help="Path to TTF/OTF font file")
    parser.add_argument("--width", type=int, default=24, help="Canvas width (default: 24)")
    parser.add_argument("--height", type=int, default=32, help="Canvas height (default: 32)")

    args = parser.parse_args()
    generate_c_table(args.font_path, args.width, args.height)
