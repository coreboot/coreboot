/*
 * minimal lzma implementation
 *
 * Copyright (C) 2002 Eric Biederman
 * Copyright (C) 2005 Joel Yliluoma
 * Copyright (C) 2007 coresystems GmbH
 * (Adapted by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 * Copyright (C) 2007 Patrick Georgi <patrick@georgi-clan.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include "C/Common/MyInitGuid.h"
#include "C/7zip/Compress/LZMA/LZMAEncoder.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>

#include <vector>
#include <algorithm>
#include <stdint.h>

const std::vector<unsigned char> LZMACompress
    (const std::vector<unsigned char>& buf);

const std::vector<unsigned char> LZMADeCompress
    (const std::vector<unsigned char>& buf);

static inline uint16_t R16(const void* p)
{
    const unsigned char* data = (const unsigned char*)p;
    return (data[0] << 0)  | (data[1] << 8);
}
static inline uint32_t R32(const void* p)
{
    const unsigned char* data = (const unsigned char*)p;
    return R16(data) | (R16(data+2) << 16);
}

#define L (uint64_t)

static inline uint64_t R64(const void* p)
{
    const unsigned char* data = (const unsigned char*)p;
    return (L R32(data)) | ((L R32(data+4)) << 32);
}

#undef L

static UInt32 SelectDictionarySizeFor(unsigned datasize)
{
   #if 1
    return datasize;
   #else
#ifdef __GNUC__
    /* gnu c can optimize this switch statement into a fast binary
     * search, but it cannot do so for the list of the if statements.
     */
    switch(datasize)
    {
        case 0 ... 512 : return 512;
        case 513 ... 1024: return 2048;
        case 1025 ... 4096: return 8192;
        case 4097 ... 16384: return 32768;
        case 16385 ... 65536: return 528288;
        case 65537 ... 528288: return 1048576*4;
        case 528289 ... 786432: return 1048576*16;
        default: return 1048576*32;
    }
#else
    if(datasize <= 512) return 512;
    if(datasize <= 1024) return 1024;
    if(datasize <= 4096) return 4096;
    if(datasize <= 16384) return 32768;
    if(datasize <= 65536) return 528288;
    if(datasize <= 528288) return 1048576*4;
    if(datasize <= 786432) reutrn 1048576*16;
    return 32*1048576;
#endif
   #endif
}


class CInStreamRam: public ISequentialInStream, public CMyUnknownImp
{
    const std::vector<unsigned char>& input;
    size_t Pos;
public:
    MY_UNKNOWN_IMP

    CInStreamRam(const std::vector<unsigned char>& buf) : input(buf), Pos(0)
    {
    }
    virtual ~CInStreamRam() {}

    STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
};

STDMETHODIMP CInStreamRam::Read(void *data, UInt32 size, UInt32 *processedSize)
{
    UInt32 remain = input.size() - Pos;
    if (size > remain) size = remain;

    std::memcpy(data, &input[Pos], size);
    Pos += size;

    if(processedSize != NULL) *processedSize = size;

    return S_OK;
}

class COutStreamRam: public ISequentialOutStream, public CMyUnknownImp
{
    std::vector<Byte> result;
    size_t Pos;
public:
    MY_UNKNOWN_IMP

    COutStreamRam(): result(), Pos(0) { }
    virtual ~COutStreamRam() { }

    void Reserve(unsigned n) { result.reserve(n); }
    const std::vector<Byte>& Get() const { return result; }

    HRESULT WriteByte(Byte b)
    {
        if(Pos >= result.size()) result.resize(Pos+1);
        result[Pos++] = b;
        return S_OK;
    }

    STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
};

STDMETHODIMP COutStreamRam::Write(const void *data, UInt32 size, UInt32 *processedSize)
{
    if(Pos+size > result.size()) result.resize(Pos+size);

    std::memcpy(&result[Pos], data, size);
    if(processedSize != NULL) *processedSize = size;
    Pos += size;
    return S_OK;
}

const std::vector<unsigned char> LZMACompress(const std::vector<unsigned char>& buf)
{
    if(buf.empty()) return buf;

    const UInt32 dictionarysize = SelectDictionarySizeFor(buf.size());

    NCompress::NLZMA::CEncoder *encoderSpec = new NCompress::NLZMA::CEncoder;
    CMyComPtr<ICompressCoder> encoder = encoderSpec;
    const PROPID propIDs[] =
    {
        NCoderPropID::kAlgorithm,
        NCoderPropID::kDictionarySize,
        NCoderPropID::kNumFastBytes,
    };
    const unsigned kNumProps = sizeof(propIDs) / sizeof(propIDs[0]);
    PROPVARIANT properties[kNumProps];
    properties[0].vt = VT_UI4; properties[0].ulVal = (UInt32)2;
    properties[1].vt = VT_UI4; properties[1].ulVal = (UInt32)dictionarysize;
    properties[2].vt = VT_UI4; properties[2].ulVal = (UInt32)64;

    if (encoderSpec->SetCoderProperties(propIDs, properties, kNumProps) != S_OK)
    {
    Error:
        return std::vector<unsigned char> ();
    }

    COutStreamRam *const outStreamSpec = new COutStreamRam;
    CMyComPtr<ISequentialOutStream> outStream = outStreamSpec;
    CInStreamRam *const inStreamSpec = new CInStreamRam(buf);
    CMyComPtr<ISequentialInStream> inStream = inStreamSpec;

    outStreamSpec->Reserve(buf.size());

    if (encoderSpec->WriteCoderProperties(outStream) != S_OK) goto Error;

    for (unsigned i = 0; i < 8; i++)
    {
        UInt64 t = (UInt64)buf.size();
        outStreamSpec->WriteByte((Byte)((t) >> (8 * i)));
    }

    HRESULT lzmaResult = encoder->Code(inStream, outStream, 0, 0, 0);
    if (lzmaResult != S_OK) goto Error;

    return outStreamSpec->Get();
}

#undef RC_NORMALIZE

#include "C/7zip/Decompress/LzmaDecode.h"
#include "C/7zip/Decompress/LzmaDecode.c"

const std::vector<unsigned char> LZMADeCompress
    (const std::vector<unsigned char>& buf)
{
    if(buf.size() <= 5+8) return std::vector<unsigned char> ();

    uint_least64_t out_sizemax = R64(&buf[5]);

    std::vector<unsigned char> result(out_sizemax);

    CLzmaDecoderState state;
    LzmaDecodeProperties(&state.Properties, &buf[0], LZMA_PROPERTIES_SIZE);
    state.Probs = new CProb[LzmaGetNumProbs(&state.Properties)];

    SizeT in_done;
    SizeT out_done;
    LzmaDecode(&state, &buf[13], buf.size()-13, &in_done,
               &result[0], result.size(), &out_done);

    delete[] state.Probs;

    result.resize(out_done);
    return result;
}

#ifndef COMPACT
int main(int argc, char *argv[])
{
	char  *s;
	FILE  *f, *infile, *outfile;
	int    c;

	if (argc != 4) {
		std::fprintf(stderr, "'lzma e file1 file2' encodes file1 into file2.\n"
			"'lzma d file2 file1' decodes file2 into file1.\n");

		return EXIT_FAILURE;
	}
	if (argc == 4) {
		if ((s = argv[1], s[1] || strpbrk(s, "DEde") == NULL)
			|| (s = argv[2], (infile  = fopen(s, "rb")) == NULL)
			|| (s = argv[3], (outfile = fopen(s, "wb")) == NULL)) {
			std::fprintf(stderr, "??? %s\n", s);
			return EXIT_FAILURE;
		}
	}

	struct stat fs;
	int si;
	if (fstat(fileno(infile), &fs)) {
		std::perror(strerror(errno));
		return EXIT_FAILURE;
	}
	si=fs.st_size;

	char *Buf=(char *)malloc(si);
	fread(Buf,si, 1, infile);

	std::vector<unsigned char> result;
	if (toupper(*argv[1]) == 'E')
		result = LZMACompress(std::vector<unsigned char>(Buf,Buf+si));
	else
		result = LZMADeCompress(std::vector<unsigned char>(Buf,Buf+si));

	fwrite(&result[0], result.size(), 1, outfile);
	fclose(infile);
	fclose(outfile);
	return EXIT_SUCCESS;
}
#else
extern "C" {

/**
 * Compress a buffer with lzma
 * Don't copy the result back if it is too large.
 * @param in a pointer to the buffer
 * @param in_len the length in bytes
 * @param out a pointer to a buffer of at least size in_len
 * @param out_len a pointer to the compressed length of in
 */

void do_lzma_compress(char *in, int in_len, char *out, int *out_len) {
	std::vector<unsigned char> result;
	result = LZMACompress(std::vector<unsigned char>(in, in + in_len));
	*out_len = result.size();
	if (*out_len < in_len)
		std::memcpy(out, &result[0], *out_len);
}

void do_lzma_uncompress(char *dst, int dst_len, char *src, int src_len) {
	std::vector<unsigned char> result;
	result = LZMADeCompress(std::vector<unsigned char>(src, src + src_len));
	if (result.size() <= dst_len)
		std::memcpy(dst, &result[0], result.size());
	else
	{
		fprintf(stderr, "Not copying %d bytes to %d-byte buffer!\n",
			result.size(), dst_len);
		exit(1);
	}
}

}
#endif

