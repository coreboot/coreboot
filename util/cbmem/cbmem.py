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
import struct
import sys

def get_phys_mem(addr, size):
    '''Read size bytes from address addr by mmaping /dev/mem'''

    mf = open("/dev/mem")
    delta = addr % 4096
    mm = mmap.mmap(mf.fileno(), size + delta,
                   mmap.MAP_PRIVATE, offset=(addr - delta))
    buf = mm.read(size + delta)
    mf.close()
    return buf[delta:]

# This class and metaclass make it easier to define and access structures
# which live in physical memory. To use them, inherit from CStruct and define
# a class member called struct_members which is a tuple of pairs. The first
# item in the pair is the type format specifier that should be used with
# struct.unpack to read that member from memory. The second item is the name
# that member should have in the resulting object.

class MetaCStruct(type):
    def __init__(cls, name, bases, dct):
        struct_members = dct["struct_members"]
        cls.struct_fmt = "<"
        for char, name in struct_members:
            cls.struct_fmt += char
        cls.struct_len = struct.calcsize(cls.struct_fmt)
        super(MetaCStruct, cls).__init__(name, bases, dct)

class CStruct(object):
    __metaclass__ = MetaCStruct
    struct_members = ()

    def __init__(self, addr):
        self.raw_memory = get_phys_mem(addr, self.struct_len)
        values = struct.unpack(self.struct_fmt, self.raw_memory)
        names = (name for char, name in self.struct_members)
        for name, value in zip(names, values):
            setattr(self, name, value)

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

def process_timers(base):
    '''Scan the array of timestamps found in CBMEM at address base.

    For each timestamp print the timer ID and the value in microseconds.
    '''

    class TimestampHeader(CStruct):
        struct_members = (
            ("Q", "base_time"),
            ("L", "max_entr"),
            ("L", "entr")
        )

    class TimestampEntry(CStruct):
        struct_members = (
            ("L", "timer_id"),
            ("Q", "timer_value")
        )

    header = TimestampHeader(base)
    print('\ntime base %d, total entries %d' % (header.base_time, header.entr))
    clock_freq = get_cpu_freq()
    base = base + header.struct_len
    for i in range(header.entr):
        timestamp = TimestampEntry(base)
        print '%d:%s ' % (timestamp.timer_id,
            normalize_timer(timestamp.timer_value, clock_freq)),
        base = base + timestamp.struct_len
    print

def process_console(base):
    '''Dump the console log buffer contents found at address base.'''

    class ConsoleHeader(CStruct):
        struct_members = (
            ("L", "size"),
            ("L", "cursor")
        )

    header = ConsoleHeader(base)
    print 'cursor at %d\n' % header.cursor

    cons_addr = base + header.struct_len
    cons_length = min(header.cursor, header.size)
    cons_text = get_phys_mem(cons_addr, cons_length)
    print cons_text
    print '\n'

def ipchksum(buf):
    '''Checksumming function used on the coreboot tables. The buffer being
    checksummed is summed up as if it was an array of 16 bit unsigned
    integers. If there are an odd number of bytes, the last element is zero
    extended.'''

    size = len(buf)
    odd = size % 2
    fmt = "<%dH" % ((size - odd) / 2)
    if odd:
        fmt += "B"
    shorts = struct.unpack(fmt, buf)
    checksum = sum(shorts)
    checksum = (checksum >> 16) + (checksum & 0xffff)
    checksum += (checksum >> 16)
    checksum = ~checksum & 0xffff
    return checksum

def parse_tables(base, length):
    '''Find the coreboot tables in memory and process whatever we can.'''

    class CBTableHeader(CStruct):
        struct_members = (
            ("4s", "signature"),
            ("I", "header_bytes"),
            ("I", "header_checksum"),
            ("I", "table_bytes"),
            ("I", "table_checksum"),
            ("I", "table_entries")
        )

    class CBTableEntry(CStruct):
        struct_members = (
            ("I", "tag"),
            ("I", "size")
        )

    class CBTableForward(CBTableEntry):
        struct_members = CBTableEntry.struct_members + (
            ("Q", "forward"),
        )

    class CBMemTab(CBTableEntry):
        struct_members = CBTableEntry.struct_members + (
            ("L", "cbmem_tab"),
        )

    for addr in range(base, base + length, 16):
        header = CBTableHeader(addr)
        if header.signature == "LBIO":
            break
    else:
        return -1

    if header.header_bytes == 0:
        return -1

    if ipchksum(header.raw_memory) != 0:
        print "Bad header checksum"
        return -1

    addr += header.header_bytes
    table = get_phys_mem(addr, header.table_bytes)
    if ipchksum(table) != header.table_checksum:
        print "Bad table checksum"
        return -1

    for i in range(header.table_entries):
        entry = CBTableEntry(addr)
        if entry.tag == 0x11: # Forwarding entry
            return parse_tables(CBTableForward(addr).forward, length)
        elif entry.tag == 0x16: # Timestamps
            process_timers(CBMemTab(addr).cbmem_tab)
        elif entry.tag == 0x17: # CBMEM console
            process_console(CBMemTab(addr).cbmem_tab)

        addr += entry.size

    return 0

def main():
    for base, length in (0x00000000, 0x1000), (0x000f0000, 0x1000):
        if parse_tables(base, length):
            break
    else:
        print "Didn't find the coreboot tables"
        return 0

if __name__ == "__main__":
    sys.exit(main())
