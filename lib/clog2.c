/*
 * clog2 -- get a log2 in C. 
 * 
 *
 * Author(s) unknown, but this classic code was found by
 * Ronald G. Minnich somewhere. 
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */

/* Assume 8 bits per byte */
#define CHAR_BIT 8

unsigned long log2(unsigned long x)
{
        unsigned long i = 1ULL << (sizeof(x)* CHAR_BIT - 1ULL);
        unsigned long pow = sizeof(x) * CHAR_BIT - 1ULL;

        if (! x) {
                return -1;
        }
        for(; i > x; i >>= 1, pow--)
                ;

        return pow;
}
