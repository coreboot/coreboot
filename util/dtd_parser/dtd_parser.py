#!/usr/bin/python
# dtd_parser.py - DTD structure parser
#
# Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

'''
DTD string parser/generator.

Detailed timing descriptor (DTD) is an 18 byte array describing video mode
(screen resolution, display properties, etc.) in EDID and used by Intel Option
ROM. Option ROM can support multiple video modes, specific mode is picked by
the BIOS through the appropriate Option ROM callback function.

This program allows to interpret the 18 byte hex DTD dump, and/or modify
certain values and generate a new DTD.
'''

import sys

#
# The DTD array format description can be found in
# http://en.wikipedia.org/wiki/Extended_display_identification_data, (see the
# EDID Detailed Timing Descriptor section).
#
# The below dictionary describes how different DTD parameters are laid out in
# the array. Note that many parameters span multiple bit fields in the DTD.
#
# The keys in the dictionary are stings (field names), the values are tuples
# of either numbers or tri-tuples. If the element of the tuple is a number, it
# is the offset in DTD, and the entire byte is used in this field. If the
# element is a tri-tuple, its components are (DTD offset, bit shift, field
# width).
#
# The partial values are extracted from the DTD fields and concatenated
# together to form actual parameter value.
#

dtd_descriptor  = {
    'dclck' : (1, 0),
    'hor_active' : ((4, 4, 4), 2),
    'hor_blank' : ((4, 0, 4), 3),
    'vert_act' : ((7, 4, 4), 5),
    'vert_blank' : ((7, 0, 4), 6),
    'hsync_offset' : ((11, 6, 2), 8),
    'hsync_pulse_width' : ((11, 4, 2), 9),
    'vsync_offset' : ((11, 2, 2), (10, 4, 4)),
    'vsync_pulse_width' : ((11, 0, 2), (10, 0, 4)),
    'hor_image_size' : ((14, 4, 4), 12),
    'vert_image_size' : ((14, 0, 4), 13),
    'hor_border' : (15,),
    'vert_border' : (16,),
    'interlaced' : ((17, 7, 1),),
    'reserved' : ((17, 5, 2), (17, 0, 1)),
    'digital_separate' : ((17, 3, 2),),
    'vert_polarity' : ((17, 2, 1),),
    'hor_polarity' : ((17, 1, 1),),
    }

PREFIX = 'attr_'

class DTD(object):
    '''An object containing all DTD information.

    The attributes are created dynamically when the input DTD string is
    parsed. For each element of the above dictionary two attributes are added:

    'attr_<param>' to hold the actual parameter value
    'max_attr_<param>' to hold the maximum allowed value for this parameter.
    '''

    def __init__(self):
        for name in dtd_descriptor:
            setattr(self, PREFIX + name, 0)

    def init(self, sarray):
        '''Initialize the object with values from a DTD array.

        Inputs:

        sarray: a string, an array of ASCII hex representations of the 18 DTD
                bytes.

        Raises: implicitly raises ValueError or IndexError exceptions in case
                the input string has less than 18 elements, or some of the
                elements can not be converted to integer.
        '''

        harray = [int(x, 16) for x in sarray]
        for name, desc in dtd_descriptor.iteritems():
            attr_value = 0
            total_width = 0
            for tup in desc:
                if isinstance(tup, tuple):
                    offset, shift, width = tup
                else:
                    offset, shift, width = tup, 0, 8

                mask = (1 << width) - 1
                attr_value = (attr_value << width) + (
                    (harray[offset] >> shift) & mask)
                total_width += width
            setattr(self, PREFIX + name, attr_value)
            setattr(self, 'max_' + PREFIX + name, (1 << total_width) - 1)

    def __str__(self):
        text = []
        for name in sorted(dtd_descriptor.keys()):
            text.append('%20s: %d' % (name, getattr(self, PREFIX + name)))
        return '\n'.join(text)

    def inhex(self):
        '''Generate contents of the DTD as a 18 byte ASCII hex array.'''

        result = [0] * 18
        for name, desc in dtd_descriptor.iteritems():
            attr_value = getattr(self, PREFIX + name)
            rdesc = list(desc)
            rdesc.reverse()
            for tup in rdesc:
                if isinstance(tup, tuple):
                    offset, shift, width = tup
                else:
                    offset, shift, width = tup, 0, 8

                mask = (1 << width) - 1
                value = attr_value & mask
                attr_value = attr_value >> width
                result[offset] = (result[offset] & ~(
                        mask << shift)) | (value << shift)

        return ' '.join('%2.2x' % x for x in result)

    def handle_input(self, name):
        '''Get user input and set a new parameter value if required.

        Display the parameter name, its current value, and prompt user for a
        new value.

        If the user enters a dot, stop processing (return True).

        Empty user input means that this parameter does not have to change,
        but the next parameter should be prompted.

        If input is non-empty, it is interpreted as a hex number, checked if
        it fits the parameter and the new parameter value is set if checks
        pass.

        Inputs:

        name - a string, parameter name, a key in dtd_descriptor

        Returns:

        Boolean, True meaning no more field are required to be modified, False
                 meaning that more field mods need to be prompted..
        '''

        param = PREFIX + name
        vmax = getattr(self, 'max_' + param)
        new_value = raw_input('%s : %d '  % (name, getattr(self, param)))
        if new_value == '':
            return False
        if new_value == '.':
            return True
        new_int = int(new_value)
        if new_int > vmax:
            print '%s exceeds maximum for %s (%d)' % (new_value, name, vmax)
        else:
            setattr(self, param, new_int)
        return False

def main(args):
    if args[0] == '-m':
        modify = True
        base = 1
    else:
        modify = False
        base = 0

    d = DTD()
    d.init(args[base:])
    if modify:
        for line in str(d).splitlines():
            if d.handle_input(line.split(':')[0].strip()):
                break
    print d
    if modify:
        print d.inhex()


if __name__ == '__main__':
    try:
        main(sys.argv[1:])
    except (ValueError, IndexError):
        print """
A string of 18 byte values in hex is required.
'-m' preceding the string will allow setting new parameter values.
"""
        sys.exit(1)
