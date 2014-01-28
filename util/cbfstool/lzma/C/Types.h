/* Types.h -- Basic types
2010-03-11 : Igor Pavlov : Public domain */

#ifndef __7Z_TYPES_H
#define __7Z_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#define SZ_OK 0

#define SZ_ERROR_DATA 1
#define SZ_ERROR_MEM 2
#define SZ_ERROR_CRC 3
#define SZ_ERROR_UNSUPPORTED 4
#define SZ_ERROR_PARAM 5
#define SZ_ERROR_INPUT_EOF 6
#define SZ_ERROR_OUTPUT_EOF 7
#define SZ_ERROR_READ 8
#define SZ_ERROR_WRITE 9
#define SZ_ERROR_PROGRESS 10
#define SZ_ERROR_FAIL 11
#define SZ_ERROR_THREAD 12

#define SZ_ERROR_ARCHIVE 16
#define SZ_ERROR_NO_ARCHIVE 17

typedef int SRes;
typedef int WRes; /* This was DWORD for _WIN32. That's uint32_t */

#ifndef RINOK
#define RINOK(x) { int __result__ = (x); if (__result__ != 0) return __result__; }
#endif

/* The following interfaces use first parameter as pointer to structure */

struct IByteIn
{
  uint8_t (*Read)(void *p); /* reads one byte, returns 0 in case of EOF or error */
};

struct IByteOut
{
  void (*Write)(void *p, uint8_t b);
};

struct ISeqInStream
{
  SRes (*Read)(void *p, void *buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
       (output(*size) < input(*size)) is allowed */
};

/* it can return SZ_ERROR_INPUT_EOF */
SRes SeqInStream_Read(struct ISeqInStream *stream, void *buf, size_t size);
SRes SeqInStream_Read2(struct ISeqInStream *stream, void *buf, size_t size, SRes errorType);
SRes SeqInStream_ReadByte(struct ISeqInStream *stream, uint8_t *buf);

struct ISeqOutStream
{
  size_t (*Write)(void *p, const void *buf, size_t size);
    /* Returns: result - the number of actually written bytes.
       (result < size) means error */
};

enum ESzSeek
{
  SZ_SEEK_SET = 0,
  SZ_SEEK_CUR = 1,
  SZ_SEEK_END = 2
};

struct ISeekInStream
{
  SRes (*Read)(void *p, void *buf, size_t *size);  /* same as ISeqInStream::Read */
  SRes (*Seek)(void *p, int64_t *pos, enum ESzSeek origin);
};

struct ILookInStream
{
  SRes (*Look)(void *p, const void **buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
       (output(*size) > input(*size)) is not allowed
       (output(*size) < input(*size)) is allowed */
  SRes (*Skip)(void *p, size_t offset);
    /* offset must be <= output(*size) of Look */

  SRes (*Read)(void *p, void *buf, size_t *size);
    /* reads directly (without buffer). It's same as ISeqInStream::Read */
  SRes (*Seek)(void *p, int64_t *pos, enum ESzSeek origin);
};

SRes LookInStream_LookRead(struct ILookInStream *stream, void *buf, size_t *size);
SRes LookInStream_SeekTo(struct ILookInStream *stream, uint64_t offset);

/* reads via ILookInStream::Read */
SRes LookInStream_Read2(struct ILookInStream *stream, void *buf, size_t size, SRes errorType);
SRes LookInStream_Read(struct ILookInStream *stream, void *buf, size_t size);

#define LookToRead_BUF_SIZE (1 << 14)

struct CLookToRead
{
  struct ILookInStream s;
  struct ISeekInStream *realStream;
  size_t pos;
  size_t size;
  uint8_t buf[LookToRead_BUF_SIZE];
};

void LookToRead_CreateVTable(struct CLookToRead *p, int lookahead);
void LookToRead_Init(struct CLookToRead *p);

struct CSecToLook
{
  struct ISeqInStream s;
  struct ILookInStream *realStream;
};

void SecToLook_CreateVTable(struct CSecToLook *p);

struct CSecToRead
{
  struct ISeqInStream s;
  struct ILookInStream *realStream;
};

void SecToRead_CreateVTable(struct CSecToRead *p);

struct ICompressProgress
{
  SRes (*Progress)(void *p, uint64_t inSize, uint64_t outSize);
    /* Returns: result. (result != SZ_OK) means break.
       Value (uint64_t)(int64_t)-1 for size means unknown value. */
};

struct ISzAlloc
{
  void *(*Alloc)(void *p, size_t size);
  void (*Free)(void *p, void *address); /* address can be 0 */
};

#define IAlloc_Alloc(p, size) (p)->Alloc((p), size)
#define IAlloc_Free(p, a) (p)->Free((p), a)

#endif
