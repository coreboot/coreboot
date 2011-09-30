#!/usr/bin/python
#
# cbmem.py - Linux space CBMEM contents parser
#
# Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#
'''
Parse and display CBMEM contents.

This module is meant to run on systems with coreboot based firmware.

When started, it determines the amount of DRAM installed on the system, and
then scans the top area of DRAM (right above the available memory size)
looking for the CBMEM base signature at locations aligned at 0x20000
boundaries.

Once it finds the CBMEM signature, the utility parses the contents, reporting
the section IDs/sizes and also reporting the contents of the tiemstamp and
console sections.
'''

import mmap
import re
import struct
import sys
import time

# These definitions follow src/include/cbmem.h
CBMEM_MAGIC = 0x434f5245
CBMEM_MAX_ENTRIES = 16

CBMEM_ENTRY_FORMAT = '@LLQQ'
CONSOLE_HEADER_FORMAT = '@LL'
TIMESTAMP_HEADER_FORMAT = '@QLL'
TIMESTAMP_ENTRY_FORMAT = '@LQ'

mf_fileno = 0  # File number of the file providing access to memory.

def align_up(base, alignment):
    '''Increment to the alignment boundary.

    Return the next integer larger than 'base' and divisible by 'alignment'.
    '''

    return base + alignment - base % alignment

def normalize_timer(value, freq):
    '''Convert timer reading into microseconds.

    Get the free running clock counter value, divide it by the clock frequency
    and multiply by 1 million to get reading in microseconds.

    Then convert the value into an ASCII string with groups of three digits
    separated by commas.

    Inputs:
      value: int, the clock reading
      freq: float, the clock frequency

    Returns:
      A string presenting 'value' in microseconds.
    '''

    result = []
    value = int(value * 1000000.0 / freq)
    svalue = '%d' % value
    vlength = len(svalue)
    remainder = vlength % 3
    if remainder:
        result.append(svalue[0:remainder])
    while remainder < vlength:
        result.append(svalue[remainder:remainder+3])
        remainder = remainder + 3
    return ','.join(result)

def get_cpu_freq():
    '''Retrieve CPU frequency from sysfs.

    Use /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq as the source.
    '''
    freq_str = open('/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq'
                    ).read()
    # Convert reading into Hertz
    return float(freq_str) * 1000.0

def get_mem_size():
    '''Retrieve amount of memory available to the CPU from /proc/meminfo.'''
    mult = {
        'kB': 1024
        }
    meminfo = open('/proc/meminfo').read()
    m = re.search('MemTotal:.*\n', meminfo)
    mem_string = re.search('MemTotal:.*\n', meminfo).group(0)
    (_, size, mult_name) = mem_string.split()
    return int(size) * mult[mult_name]

def parse_mem_at(addr, format):
    '''Read and parse a memory location.

    This function reads memory at the passed in address, parses it according
    to the passed in format specification and returns a list of values.

    The first value in the list is the size of data matching the format
    expression, and the rest of the elements of the list are the actual values
    retrieved using the format.
    '''

    size = struct.calcsize(format)
    delta = addr % 4096   # mmap requires the offset to be page size aligned.
    mm = mmap.mmap(mf_fileno, size + delta,
                   mmap.MAP_PRIVATE, offset=(addr - delta))
    buf = mm.read(size + delta)
    mm.close()
    rv = [size,] + list(struct.unpack(format, buf[delta:size + delta + 1]))
    return rv

def dprint(text):
    '''Debug print function.

    Edit it to get the debug output.
    '''

    if False:
        print text

def process_timers(base):
    '''Scan the array of timestamps found in CBMEM at address base.

    For each timestamp print the timer ID and the value in microseconds.
    '''

    (step, base_time, max_entr, entr) = parse_mem_at(
        base, TIMESTAMP_HEADER_FORMAT)

    print('\ntime base %d, total entries %d' % (base_time, entr))
    clock_freq = get_cpu_freq()
    base = base + step
    for i in range(entr):
        (step, timer_id, timer_value) = parse_mem_at(
            base, TIMESTAMP_ENTRY_FORMAT)
        print '%d:%s ' % (timer_id, normalize_timer(timer_value, clock_freq)),
        base = base + step
    print

def process_console(base):
    '''Dump the console log buffer contents found at address base.'''

    (step, size, cursor) = parse_mem_at(base, CONSOLE_HEADER_FORMAT)
    print 'cursor at %d\n' % cursor

    cons_string_format = '%ds' % min(cursor, size)
    (_, cons_text) = parse_mem_at(base + step, cons_string_format)
    print cons_text
    print '\n'

mem_alignment = 1024 * 1024 * 1024 # 1 GBytes
table_alignment = 128 * 1024

mem_size = get_mem_size()

# start at memory address aligned at 128K.
offset = align_up(mem_size, table_alignment)

dprint('mem_size %x offset %x' %(mem_size, offset))
mf = open("/dev/mem")
mf_fileno = mf.fileno()

while offset % mem_alignment: # do not cross the 1G boundary while searching
    (step, magic, mid, base, size) = parse_mem_at(offset, CBMEM_ENTRY_FORMAT)
    if magic == CBMEM_MAGIC:
        offset = offset + step
        break
    offset += table_alignment
else:
    print 'Did not find the CBMEM'
    sys.exit(0)

for i in (range(1, CBMEM_MAX_ENTRIES)):
    (_, magic, mid, base, size) = parse_mem_at(offset, CBMEM_ENTRY_FORMAT)
    if mid == 0:
        break

    print '%x, %x, %x' % (mid, base, size)
    if mid == 0x54494d45:
        process_timers(base)
    if mid == 0x434f4e53:
        process_console(base)

    offset = offset + step

mf.close()
