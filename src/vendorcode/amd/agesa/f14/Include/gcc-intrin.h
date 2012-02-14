/*
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
 *       its contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
 
#if defined (__GNUC__)

/* I/O intrin functions.  */
static __inline__ __attribute__((always_inline)) unsigned char __inbyte(unsigned short Port)
{
  unsigned char value;

  __asm__ __volatile__ (
    "in  %%dx, %%al"
    : "=a" (value)
    : "d" (Port)
    );

  return value;
}

static __inline__ __attribute__((always_inline)) unsigned short __inword(unsigned short Port)
{
  unsigned short value;

  __asm__ __volatile__ (
    "in  %%dx, %%ax"
    : "=a" (value)
    : "d" (Port)
    );

  return value;
}

static __inline__ __attribute__((always_inline)) unsigned long __indword(unsigned short Port)
{
  unsigned long value;
    
  __asm__ __volatile__ (
    "in  %%dx, %%eax"
    : "=a" (value)
    : "d" (Port)
    );
  return value;

}

static __inline__ __attribute__((always_inline)) void __outbyte(unsigned short Port,unsigned char Data)
{
  __asm__ __volatile__ (
    "out  %%al, %%dx"
    :
    : "a" (Data), "d" (Port)
    );
}

static __inline__ __attribute__((always_inline)) void __outword(unsigned short Port,unsigned short Data)
{
  __asm__ __volatile__ (
    "out  %%ax, %%dx"
    :
    : "a" (Data), "d" (Port)
    );
}

static __inline__ __attribute__((always_inline)) void __outdword(unsigned short Port,unsigned long Data)
{
  __asm__ __volatile__ (
    "out  %%eax, %%dx"
    :
    : "a" (Data), "d" (Port)
    );
}

static __inline__ __attribute__((always_inline)) void __inbytestring(unsigned short Port,unsigned char *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "cld ; rep ; insb " 
    : "=D" (Buffer), "=c" (Count)
    : "d"(Port), "0"(Buffer), "1" (Count)
    );
}

static __inline__ __attribute__((always_inline)) void __inwordstring(unsigned short Port,unsigned short *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "cld ; rep ; insw " 
    : "=D" (Buffer), "=c" (Count)
    : "d"(Port), "0"(Buffer), "1" (Count)
    );
}

static __inline__ __attribute__((always_inline)) void __indwordstring(unsigned short Port,unsigned long *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "cld ; rep ; insl " 
    : "=D" (Buffer), "=c" (Count)
    : "d"(Port), "0"(Buffer), "1" (Count)
    );
}

static __inline__ __attribute__((always_inline)) void __outbytestring(unsigned short Port,unsigned char *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "cld ; rep ; outsb " 
    : "=S" (Buffer), "=c" (Count)
    : "d"(Port), "0"(Buffer), "1" (Count)
    );
}

static __inline__ __attribute__((always_inline)) void __outwordstring(unsigned short Port,unsigned short *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
    "cld ; rep ; outsw " 
    : "=S" (Buffer), "=c" (Count)
    : "d"(Port), "0"(Buffer), "1" (Count)
  );
}

static __inline__ __attribute__((always_inline)) void __outdwordstring(unsigned short Port,unsigned long *Buffer,unsigned long Count)
{
  __asm__ __volatile__ (
   "cld ; rep ; outsl " 
   : "=S" (Buffer), "=c" (Count)
   : "d"(Port), "0"(Buffer), "1" (Count)
   );
}

static __inline__ __attribute__((always_inline)) unsigned long __readdr0(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr0, %[value]" 
    : [value] "=a" (value)
    );
  return value;
}
 
static __inline__ __attribute__((always_inline)) unsigned long __readdr1(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr1, %[value]" 
    : [value] "=a" (value)
    );
  return value;
}
 
static __inline__ __attribute__((always_inline)) unsigned long __readdr2(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr2, %[value]" 
    : [value] "=a" (value)
    );
  return value;
}
 
static __inline__ __attribute__((always_inline)) unsigned long __readdr3(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr3, %[value]" 
    : [value] "=a" (value)
    );
  return value;
}
 
static __inline__ __attribute__((always_inline)) unsigned long __readdr7(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%dr7, %[value]" 
    : [value] "=a" (value)
    );
  return value;
}
 
static __inline__ __attribute__((always_inline)) unsigned long __readdr(unsigned long reg)
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
  }      
}
 
static __inline__ __attribute__((always_inline)) void __writedr0(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%dr0"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writedr1(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%dr1"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writedr2(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%dr2"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writedr3(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%dr3"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writedr7(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%dr7"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writedr(unsigned long reg, unsigned long Data)
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
      ;
  } 
}
 
static __inline__ __attribute__((always_inline)) unsigned long __readcr0(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr0, %[value]" 
    : [value] "=a" (value)
    :
    : "memory");
  return value;
}

static __inline__ __attribute__((always_inline)) unsigned long __readcr2(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr2, %[value]" 
    : [value] "=a" (value));
  return value;
}

static __inline__ __attribute__((always_inline)) unsigned long __readcr3(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr3, %[value]" 
    : [value] "=a" (value));
  return value;
}

static __inline__ __attribute__((always_inline)) unsigned long __readcr4(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr4, %[value]" 
    : [value] "=a" (value));
  return value;
}

static __inline__ __attribute__((always_inline)) unsigned long __readcr8(void)
{
  unsigned long value;
  __asm__ __volatile__ (
    "mov %%cr8, %[value]" 
    : [value] "=a" (value));
  return value;
}

static __inline__ __attribute__((always_inline)) unsigned long __readcr(unsigned long reg)
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
  }      
}

static __inline__ __attribute__((always_inline)) void __writecr0(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%cr0"
    : 
    : "a" (Data)
    : "memory"
    );
}
 
static __inline__ __attribute__((always_inline)) void __writecr2(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%cr2"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writecr3(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%cr3"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writecr4(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%cr4"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writecr8(unsigned long Data)
{
  __asm__ __volatile__ (
    "mov %%eax, %%cr8"
    : 
    : "a" (Data)
    );
}
 
static __inline__ __attribute__((always_inline)) void __writecr(unsigned long reg, unsigned long Data)
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
      ;
  } 
}

static __inline__ __attribute__((always_inline)) UINT64 __readmsr(UINT32 msr)
{
  UINT64 retval;
  __asm__ __volatile__(
       "rdmsr\n\t"
       : "=A" (retval)
       : "c" (msr)
       );
   return retval;
}

static __inline__ __attribute__((always_inline)) void __writemsr (UINT32 msr, UINT64 Value)
{
  __asm__ __volatile__ (
     "wrmsr\n\t"
     :
     : "c" (msr), "A" (Value)
     );
}
 
static __inline__ __attribute__((always_inline)) UINT64 __rdtsc(void)
{
  UINT64 retval;
  __asm__ __volatile__ (
     "rdtsc" 
     : "=A" (retval));
  return retval;
}

static __inline__ __attribute__((always_inline)) void __cpuid(int CPUInfo[], const int InfoType)
{
   __asm__ __volatile__(
     "cpuid" 
     :"=a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3]) 
     : "a" (InfoType)
     );
}


static __inline__ __attribute__((always_inline)) void _disable(void)
{
  __asm__ __volatile__ ("cli");
}


static __inline__ __attribute__((always_inline)) void _enable(void)
{
  __asm__ __volatile__ ("sti");
}


static __inline__ __attribute__((always_inline)) void __halt(void)
{
  __asm__ __volatile__ ("hlt");
}


static __inline__ __attribute__((always_inline)) void __debugbreak(void)
{
  __asm__ __volatile__ ("int3");
}

static __inline__ __attribute__((always_inline)) void __invd(void)
{
  __asm__ __volatile__ ("invd");
}

static __inline__ __attribute__((always_inline)) void __wbinvd(void)
{
  __asm__ __volatile__ ("wbinvd");
}

static __inline__ __attribute__((always_inline)) void __lidt(void *Source)
{
  __asm__ __volatile__("lidt %0" : : "m"(*(short*)Source));
}

static __inline__ __attribute__((always_inline)) void __writefsbyte(const unsigned long Offset, const unsigned char Data)
{
  __asm__("movb %b[Data], %%fs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "iq" (Data));
}

static __inline__ __attribute__((always_inline)) void __writefsword(const unsigned long Offset, const unsigned short Data)
{
  __asm__("movw %w[Data], %%fs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "iq" (Data));
}

static __inline__ __attribute__((always_inline)) void __writefsdword(const unsigned long Offset, const unsigned long Data)
{
  __asm__("movl %k[Data], %%fs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "iq" (Data));
}

static __inline__ __attribute__((always_inline)) unsigned char __readfsbyte(const unsigned long Offset)
{
  unsigned char value;
  __asm__("movb %%fs:%a[Offset], %b[value]" : [value] "=q" (value) : [Offset] "irm" (Offset));
  return value;
}

static __inline__ __attribute__((always_inline)) unsigned short __readfsword(const unsigned long Offset)
{
  unsigned short value;
  __asm__("movw %%fs:%a[Offset], %w[value]" : [value] "=q" (value) : [Offset] "irm" (Offset));
  return value;
}

static __inline__ __attribute__((always_inline)) unsigned long long __readfsdword(unsigned long long Offset)
{
  unsigned long long value;
  __asm__("movl %%fs:%a[Offset], %k[value]" : [value] "=q" (value) : [Offset] "irm" (Offset));
  return value;
}

#ifdef __SSE3__
typedef long long __v2di __attribute__ ((__vector_size__ (16)));
typedef long long __m128i __attribute__ ((__vector_size__ (16), __may_alias__));

static __inline__ __attribute__((always_inline)) void _mm_stream_si128_fs2 (void *__A, __m128i __B)
{
  __asm__(".byte 0x64"); // fs prefix
  __builtin_ia32_movntdq ((__v2di *)__A, (__v2di)__B);
}

static __inline__ __attribute__((always_inline)) void _mm_stream_si128_fs (void *__A, void *__B)
{
  __m128i data;
  data = (__m128i) __builtin_ia32_lddqu ((char const *)__B);
  _mm_stream_si128_fs2 (__A, data);
}

static __inline__ __attribute__((always_inline)) void _mm_clflush_fs (void *__A)
{
  __asm__(".byte 0x64"); // fs prefix
  __builtin_ia32_clflush (__A);
}

static __inline __attribute__(( __always_inline__)) void _mm_mfence (void)
{
  __builtin_ia32_mfence ();
}

static __inline __attribute__(( __always_inline__)) void _mm_sfence (void)
{
  __builtin_ia32_sfence ();
}
#endif

static __inline__ __attribute__((always_inline)) void __stosb(unsigned char *dest, unsigned char data, size_t count)
{
   __asm__ __volatile__ (
    "cld ; rep ; stosb "
    : "=D" (dest), "=c" (count)
    : "a"(data), "0"(dest), "1" (count)
  );
}

static __inline__ __attribute__((always_inline)) void __movsb(unsigned char *dest, unsigned char *data, size_t count)
{
   __asm__ __volatile__ (
    "cld ; rep ; movsb "
    : "=D" (dest), "=S"(data), "=c" (count)
    : "S"(data), "0"(dest), "1" (count)
  );
}

static __inline__ __attribute__((always_inline)) 
void debug_point ( unsigned short Port,  unsigned long Data )
{
   __outdword (Port, Data);     
   __asm__ __volatile__ (".word 0xfeeb");

}

static __inline__ __attribute__((always_inline)) 
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
#endif // defined (__GNUC__)
