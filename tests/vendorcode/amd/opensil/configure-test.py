#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Exercise incremental openSIL Make/Meson builds with a real tiny C library."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[4]
HELPER = ROOT / 'src/vendorcode/amd/opensil/configure.py'
MAKEFILE = ROOT / 'src/vendorcode/amd/opensil/Makefile.mk'


def run(argv, directory, log):
    env = os.environ.copy()
    if str(argv[0]) == "make":
        # The fixture owns its Make configuration and jobserver.
        for key in ("MAKEFLAGS", "MFLAGS", "MAKELEVEL", "MAKEOVERRIDES"):
            env.pop(key, None)
    result = subprocess.run([str(arg) for arg in argv], cwd=directory, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            timeout=90)
    log.write_bytes(result.stdout)
    if result.returncode:
        raise RuntimeError(f'{log.name}: exit {result.returncode}')


def fixture(directory, flags):
    source, build = directory / 'source', directory / 'build'
    source.mkdir(parents=True)
    build.mkdir()
    (source / 'meson.build').write_text(
        "project('opensil-incremental-test', 'c')\n"
        # openSIL installs its combined archive, so Meson makes a regular
        # archive whose members remain usable after coreboot copies it.
        "static_library('AMDopensil64', 'value.c', install: true)\n")
    (source / 'meson_options.txt').write_text(
        "option('PlatKcfgDir', type: 'string')\n"
        "option('PlatKcfg', type: 'string')\n")
    (source / 'value.c').write_text(
        '#ifndef REMOVED\n#define REMOVED 0\n#endif\n'
        'int value(void) { return VALUE; }\n'
        'int removed(void) { return REMOVED; }\n')
    (build / 'config.h').write_text('/* fixture config */\n')
    cross_file(build, flags)
    return source, build


def cross_file(build, flags):
    (build / 'meson_crosscompile').write_text(
        "[binaries]\nc = 'cc'\nar = 'ar'\n"
        "[host_machine]\nsystem = 'linux'\ncpu_family = 'x86_64'\n"
        "cpu = 'x86_64'\nendian = 'little'\n"
        '[built-in options]\nc_args = ' + repr(flags) + '\n')


def configure(source, build, log):
    run([sys.executable, '-B', HELPER, '--cross-file', build / 'meson_crosscompile',
         '--build-dir', build / 'opensil', '--config-dir', build,
         '--config-name', 'opensil_config'], source, log)


def check_library(library, expected, removed, log):
    probe = log.with_suffix('.c')
    binary = log.with_suffix('.elf')
    probe.write_text('int value(void); int removed(void);\n'
                     f'int main(void) {{ return value() != {expected} || '
                     f'removed() != {removed}; }}\n')
    run(['cc', probe, library, '-o', binary], library.parent, log)
    run([binary], library.parent, log.with_suffix('.run.log'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    out = Path(tempfile.mkdtemp(prefix="run-", dir=args.output.resolve()))
    result = dict(complete=False, checks=[],
                  helper_sha256=hashlib.sha256(HELPER.read_bytes()).hexdigest(),
                  makefile_sha256=hashlib.sha256(MAKEFILE.read_bytes()).hexdigest())
    try:
        source, build = fixture(out / 'flags', ['-DVALUE=7', '-DREMOVED=1'])
        configure(source, build, out / 'flags-initial.log')
        run(['meson', 'compile', '-C', build / 'opensil'], source, out / 'flags-build.log')
        check_library(build / 'opensil/libAMDopensil64.a', 7, 1, out / 'flags-before.log')
        cross_file(build, ['-DVALUE=11'])
        configure(source, build, out / 'flags-reconfigure.log')
        run(['meson', 'compile', '-C', build / 'opensil'], source, out / 'flags-rebuild.log')
        check_library(build / 'opensil/libAMDopensil64.a', 11, 0, out / 'flags-after.log')
        result['checks'].append('reconfigure replaces changed and removed compiler flags')

        source, build = fixture(out / 'substitutions', ['-DVALUE=1'])
        contents = MAKEFILE.read_text()
        prefix = contents.split('subdirs-$(CONFIG_SOC_AMD_OPENSIL_GENOA_POC)', 1)[0]
        initialization = prefix.split('OPENSIL_CONFIG=opensil_config', 1)[1]
        recipe_start = contents.index('$(OBJPATH)/$(OPENSIL_CONFIG):')
        recipe_end = contents.index('$(OBJPATH)/$(OPENSIL_CONFIG).h:', recipe_start)
        (source.parent / 'opensil_config.template').write_text('VALUE=@TEST_VALUE@\n')
        make = out / 'substitutions.mk'
        make.write_text(f'OBJPATH := {build}\nobj := {build}\n'
                        f'opensil_dir := {source}\nOPENSIL_CONFIG := opensil_config\n'
                        'opensil_config_sed := INVALID_INHERITED_VALUE\n' + initialization +
                        'TEST_VALUE := 2\n'
                        'opensil_config_sed += -e "s,@TEST_VALUE@,$(TEST_VALUE),"\n'
                        'CONFIG_PSP_APOB_DRAM_ADDRESS := 0x4000000\n'
                        'bios_base := 0xff000000\nbios_size := 0x10000\n' +
                        contents[recipe_start:recipe_end])
        run(['make', '--no-print-directory', '-f', make, build / 'opensil_config'],
            source, out / 'substitutions.log')
        generated = (build / 'opensil_config').read_text()
        if generated != 'VALUE=2\n':
            raise RuntimeError('configuration substitutions were not initialized and expanded')
        result['checks'].append('Makefile initializes and applies SoC configuration substitutions')

        source, build = fixture(out / 'dependency', ['-DVALUE=1'])
        fragment = MAKEFILE.read_text().split("# Don't set a meson buildtype", 1)[1]
        fragment = "# Don't set a meson buildtype" + fragment.split('romstage-libs +=', 1)[0]
        make = out / 'dependency.mk'
        make.write_text(f'OBJPATH := {build}\nobj := {build}\ntop := {ROOT}\n'
                        f'opensil_dir := {source}\nPYTHON := {sys.executable}\n'
                        'OPENSIL_CONFIG := opensil_config\n'
                        'opensil_target_name := AMDopensil64\n'
                        '.PHONY: all\nall: $(OBJPATH)/opensil.a\n' + fragment)
        make_command = ['make', '-j4', '--no-print-directory', '-f', make, 'all']
        run(make_command, source, out / 'make-first.log')
        if 'Could not initialize jobserver' in (out / 'make-first.log').read_text():
            raise RuntimeError('parallel openSIL build lost the Make jobserver')
        result['checks'].append('parallel Meson/Ninja build inherits the Make jobserver')
        check_library(build / 'opensil.a', 1, 0, out / 'source-before.log')
        (source / 'value.c').write_text('int value(void) { return 99; }\n'
                                        'int removed(void) { return 0; }\n')
        run(make_command, source, out / 'make-edited.log')
        check_library(build / 'opensil.a', 99, 0, out / 'source-after.log')
        result['checks'].append('source edit reaches the archive linked by coreboot')
        previous = (build / 'opensil.a').stat().st_mtime_ns
        run(make_command, source, out / 'make-unchanged.log')
        if (build / 'opensil.a').stat().st_mtime_ns != previous:
            raise RuntimeError('unchanged openSIL archive was copied again')
        result['checks'].append('unchanged rebuild preserves archive timestamp')
        result['complete'] = True
    except Exception as error:
        result['error'] = str(error)
    (out / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))
    return 0 if result['complete'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
