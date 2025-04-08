#!/usr/bin/env python3
#
# SPDX-License-Identifier: GPL-2.0-only

import argparse


HEADER_OFFSET = 0x200
COOKIE = 0x17C3A6B4


def check_pi_img(pi_img):
    cookie_count = 0
    with open(pi_img, "rb") as f:
        f.seek(HEADER_OFFSET)
        while True:
            data = f.read(4)
            if not data:
                break
            value = int.from_bytes(data, byteorder="little", signed=False)
            if value == COOKIE:
                cookie_count += 1
    if cookie_count != 2:
        raise ValueError("Invalid PI_IMG {} (expected 2 cookies; found {})"
                         .format(pi_img, cookie_count))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("pi_img")
    args = parser.parse_args()
    check_pi_img(args.pi_img)

if __name__ == '__main__':
    main()
