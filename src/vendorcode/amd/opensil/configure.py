#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Keep Meson's persistent C options aligned with coreboot's generated file."""
import argparse
import ast
import configparser
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cross-file', required=True, type=Path)
    parser.add_argument('--build-dir', required=True, type=Path)
    parser.add_argument('--config-dir', required=True)
    parser.add_argument('--config-name', required=True)
    args = parser.parse_args()
    machine = configparser.ConfigParser(interpolation=None)
    with args.cross_file.open() as stream:
        machine.read_file(stream)
    c_args = ast.literal_eval(machine['built-in options']['c_args'])
    if not isinstance(c_args, list) or not all(isinstance(flag, str) for flag in c_args):
        raise ValueError('Expected a list of compiler arguments in the generated machine file')
    command = ['meson', 'setup']
    if (args.build_dir/'meson-private/coredata.dat').exists():
        command.append('--reconfigure')
    # Machine-file built-in defaults are otherwise retained from initial setup,
    # even after --reconfigure. Set the complete current list explicitly.
    command += ['--cross-file', str(args.cross_file), str(args.build_dir),
                '-Db_staticpic=false', '-DPlatKcfgDir='+args.config_dir,
                '-DPlatKcfg='+args.config_name, '-Dc_args='+repr(c_args)]
    subprocess.run(command, check=True)


if __name__ == '__main__':
    main()
