/*
  Compatibility <intrin_x86.h> header for GCC -- GCC equivalents of intrinsic
  Microsoft Visual C++ functions. Originally developed for the ReactOS
  (<http://www.reactos.org/>) and TinyKrnl (<http://www.tinykrnl.org/>)
  projects.

  Copyright (c) 2006 KJK::Hyperion <hackbunny@reactos.com>
                2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
                2014 Sage Electronic Engineering, LLC
  All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#if defined (__GNUC__)

#if !defined(AMDLIB_OPTIMIZE)
	#define AMDLIB_OPTIMIZE
#endif

/* I/O intrin functions.  */
static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned char __inbyte(unsigned short Port)
{
  unsigned char value;

  __asm__ __volatile__ (
    "in  %1, %0"
    : "=a" (value)
    : "Nd" (Port)
    );

  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned short __inword(unsigned short Port)
{
  unsigned short value;

  __asm__ __volatile__ (
    "in  %1, %0"
    : "=a" (value)
    : "Nd" (Port)
    );

  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __indword(unsigned short Port)
{
  unsigned long value;

  __asm__ __volatile__ (
    "in  %1, %0"
    : "=a" (value)
    : "Nd" (Port)
    );
  return value;

}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __outbyte(unsigned short Port,unsigned char Data)
{
  __asm__ __volatile__ (
    "out  %0, %1"
    :
    : "a" (Data), "Nd" (Port)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __outword(unsigned short Port,unsigned short Data)
{
  __asm__ __volatile__ (
    "out  %0, %1"
    :
    : "a" (Data), "Nd" (Port)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __outdword(unsigned short Port,unsigned long Data)
{
  __asm__ __volatile__ (
    "out  %0, %1"
    :
    : "a" (Data), "Nd" (Port)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __inbytestring(unsigned short Port,unsigned char *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "rep ; insb"
    : "+D" (Buffer), "+c" (Count)
    : "d"(Port)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __inwordstring(unsigned short Port,unsigned short *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "rep ; insw"
    : "+D" (Buffer), "+c" (Count)
    : "d"(Port)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __indwordstring(unsigned short Port,unsigned long *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "rep ; insl"
    : "+D" (Buffer), "+c" (Count)
    : "d"(Port)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __outbytestring(unsigned short Port,unsigned char *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "rep ; outsb"
    : "+S" (Buffer), "+c" (Count)
    : "d"(Port)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __outwordstring(unsigned short Port,unsigned short *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "rep ; outsw"
    : "+S" (Buffer), "+c" (Count)
    : "d"(Port)
  );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __outdwordstring(unsigned short Port,unsigned long *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
   "rep ; outsl"
   : "+S" (Buffer), "+c" (Count)
   : "d"(Port)
   );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readdr0(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr0, %[value]"
    : [value] "=r" (value)
    );
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readdr1(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr1, %[value]"
    : [value] "=r" (value)
    );
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readdr2(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr2, %[value]"
    : [value] "=r" (value)
    );
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readdr3(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr3, %[value]"
    : [value] "=r" (value)
    );
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readdr7(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr7, %[value]"
    : [value] "=r" (value)
    );
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readdr(unsigned long reg)
{
  switch (reg){
    case 0:
      return __readdr0 ();
      break;

    case 1:
      return __readdr1 ();
      break;

    case 2:
      return __readdr2 ();
      break;

    case 3:
      return __readdr3 ();
      break;

    case 7:
      return __readdr7 ();
      break;

    default:
      return -1;
      break;
  }
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writedr0(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%dr0"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writedr1(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%dr1"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writedr2(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%dr2"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writedr3(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%dr3"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writedr7(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%dr7"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writedr(unsigned long reg, unsigned long Data)
{
  switch (reg){
    case 0:
      __writedr0 (Data);
      break;

    case 1:
      __writedr1 (Data);
      break;

    case 2:
      __writedr2 (Data);
      break;

    case 3:
      __writedr3 (Data);
      break;

    case 7:
      __writedr7 (Data);
      break;

    default:
      break;
  }
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readcr0(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr0, %[value]"
    : [value] "=r" (value));
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readcr2(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr2, %[value]"
    : [value] "=r" (value));
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readcr3(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr3, %[value]"
    : [value] "=r" (value));
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readcr4(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr4, %[value]"
    : [value] "=r" (value));
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readcr8(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr8, %[value]"
    : [value] "=r" (value));
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readcr(unsigned long reg)
{
  switch (reg){
    case 0:
      return __readcr0 ();
      break;

    case 2:
      return __readcr2 ();
      break;

    case 3:
      return __readcr3 ();
      break;

    case 4:
      return __readcr4 ();
      break;

    case 8:
      return __readcr8 ();
      break;

    default:
      return -1;
      break;
  }
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writecr0(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%cr0"
    :
    : "r" (Data)
    : "memory"
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writecr2(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%cr2"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writecr3(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%cr3"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writecr4(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%cr4"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writecr8(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %0, %%cr8"
    :
    : "r" (Data)
    );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writecr(unsigned long reg, unsigned long Data)
{
  switch (reg){
    case 0:
      __writecr0 (Data);
      break;

    case 2:
      __writecr2 (Data);
      break;

    case 3:
      __writecr3 (Data);
      break;

    case 4:
      __writecr4 (Data);
      break;

    case 8:
      __writecr8 (Data);
      break;

    default:
      break;
  }
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
UINT64 __readmsr(UINT32 msr)
{
  UINT64 retval;
  __asm__ __volatile__(
       "rdmsr"
       : "=A" (retval)
       : "c" (msr)
       );
   return retval;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writemsr (UINT32 msr, UINT64 Value)
{
  __asm__ __volatile__ (
     "wrmsr"
     :
     : "c" (msr), "A" (Value)
     );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
UINT64 __rdtsc(void)
{
  UINT64 retval;
  __asm__ __volatile__ (
     "rdtsc"
     : "=A" (retval));
  return retval;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __cpuid(int CPUInfo[], const int InfoType)
{
   __asm__ __volatile__(
     "cpuid"
     :"=a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3])
     : "a" (InfoType)
     );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void _disable(void)
{
  __asm__ __volatile__ ("cli");
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void _enable(void)
{
  __asm__ __volatile__ ("sti");
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __halt(void)
{
  __asm__ __volatile__ ("hlt");
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __debugbreak(void)
{
  __asm__ __volatile__ ("int3");
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __invd(void)
{
  __asm__ __volatile__ ("invd");
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __wbinvd(void)
{
  __asm__ __volatile__ ("wbinvd");
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __lidt(void *Source)
{
  __asm__ __volatile__("lidt %0" : : "m"(*(short*)Source));
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writefsbyte(const unsigned long Offset, const unsigned char Data)
{
  __asm__ ("movb %[Data], %%fs:%a[Offset]"
          :
          : [Offset] "ir" (Offset), [Data] "iq" (Data));
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writefsword(const unsigned long Offset, const unsigned short Data)
{
  __asm__ ("movw %[Data], %%fs:%a[Offset]"
          :
          : [Offset] "ir" (Offset), [Data] "ir" (Data));
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __writefsdword(const unsigned long Offset, const unsigned long Data)
{
  __asm__ ("movl %[Data], %%fs:%a[Offset]"
           :
           : [Offset] "ir" (Offset), [Data] "ir" (Data));
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned char __readfsbyte(const unsigned long Offset)
{
  unsigned char value;
  __asm__ ("movb %%fs:%a[Offset], %[value]"
          : [value] "=q" (value)
          : [Offset] "ir" (Offset));
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned short __readfsword(const unsigned long Offset)
{
  unsigned short value;
  __asm__ ("movw %%fs:%a[Offset], %[value]"
           : [value] "=q" (value)
           : [Offset] "ir" (Offset));
  return value;
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
unsigned long __readfsdword(unsigned long Offset)
{
  unsigned long value;
  __asm__ ("movl %%fs:%a[Offset], %[value]"
           : [value] "=r" (value)
           : [Offset] "ir" (Offset));
  return value;
}

#ifdef __SSE3__
typedef long long __v2di __attribute__ ((__vector_size__ (16)));
typedef long long __m128i __attribute__ ((__vector_size__ (16), __may_alias__));

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void _mm_stream_si128_fs2 (void *__A, __m128i __B)
{
  __asm__(".byte 0x64"); // fs prefix
  __builtin_ia32_movntdq ((__v2di *)__A, (__v2di)__B);
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void _mm_stream_si128_fs (void *__A, void *__B)
{
  __m128i data;
  data = (__m128i) __builtin_ia32_lddqu ((char const *)__B);
  _mm_stream_si128_fs2 (__A, data);
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void _mm_clflush_fs (void *__A)
{
  __asm__(".byte 0x64"); // fs prefix
  __builtin_ia32_clflush (__A);
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void _mm_mfence (void)
{
  __builtin_ia32_mfence ();
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void _mm_sfence (void)
{
  __builtin_ia32_sfence ();
}
#endif /* __SSE3__ */

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __stosb(unsigned char *dest, unsigned char data, size_t count)
{
   __asm__ __volatile__ (
    "rep ; stosb "
    : "+D" (dest), "+c" (count)
    : "a"(data)
  );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void __movsb(unsigned char *dest, unsigned char *data, size_t count)
{
   __asm__ __volatile__ (
    "movsb "
    : "+D" (dest), "+S"(data), "+c" (count)
  );
}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void debug_point ( unsigned short Port,  unsigned long Data )
{
   __outdword (Port, Data);
   __asm__ __volatile__ (".word 0xfeeb");

}

static
AMDLIB_OPTIMIZE
__inline__ __attribute__((always_inline))
void delay_point ( unsigned short Port, unsigned long Data, unsigned long delayTime )
{
  UINTN  Index;
  Index = 0;
  __outdword (Port, Data);
  while (Index < delayTime * 600000) {
    __outdword (0xE0, 0);
    Index ++;
  }
}
#endif /* defined (__GNUC__) */
