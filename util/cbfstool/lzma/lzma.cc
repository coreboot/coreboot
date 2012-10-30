#include "endian.hh" /* For R64 */

extern "C" {
#include "C/LzmaDec.h"
#include "C/LzmaEnc.h"
}

#include "lzma.hh"

#include <algorithm> // min,max,swap
#include <vector>
#include <string>
#include <cstring> // std::memcpy


#include <cstdio>

#include <stdint.h>

/* We don't want threads */
#ifdef linux
#include <sched.h>
#define ForceSwitchThread() sched_yield()
#else
#define ForceSwitchThread()
#endif


int LZMA_verbose = 0;

// -fb
unsigned LZMA_NumFastBytes = 273;
/*from lzma.txt:
          Set number of fast bytes - [5, 273], default: 273
          Usually big number gives a little bit better compression ratio
          and slower compression process.
  from anonymous:
This one is hard to explain... To my knowledge (please correct me if I
am wrong), this refers to the optimal parsing algorithm. The algorithm
tries many different combinations of matches to find the best one. If a
match is found that is over the fb value, then it will not be optimised,
and will just be used straight.
This speeds up corner cases such as pic.
*/

/* apparently, 0 and 1 are valid values. 0 = fast mode */
unsigned LZMA_AlgorithmNo  = 1;

unsigned LZMA_MatchFinderCycles = 0; // default: 0

// -pb
unsigned LZMA_PosStateBits = 0; // default: 2, range: 0..4
/*from lzma.txt:
          pb switch is intended for periodical data
          when period is equal 2^N.
*/


// -lp
unsigned LZMA_LiteralPosStateBits = 0; // default: 0, range: 0..4
/*from lzma.txt:
          lp switch is intended for periodical data when period is
          equal 2^N. For example, for 32-bit (4 bytes)
          periodical data you can use lp=2.
          Often it's better to set lc0, if you change lp switch.
*/

// -lc
unsigned LZMA_LiteralContextBits = 1; // default: 3, range: 0..8
/*from lzma.txt:
          Sometimes lc=4 gives gain for big files.
  from anonymous:
The context for the literal coder is 2^(lc) long. The longer it is, the
better the statistics, but also the slower it adapts. A tradeoff, which
is why 3 or 4 is reccommended.
*/

/*

Discoveries:

 INODES:
    Best LZMA for raw_inotab_inode(40->48): pb0 lp0 lc0
    Best LZMA for raw_root_inode(28->32): pb0 lp0 lc0

    Start LZMA(rootdir, 736 bytes)
    Yay result with pb0 lp0 lc0: 218
    Yay result with pb0 lp0 lc1: 217
    Best LZMA for rootdir(736->217): pb0 lp0 lc1

    Start LZMA(inotab, 379112 bytes)
    Yay result with pb0 lp0 lc0: 24504
    Best LZMA for inotab(379112->24504): pb0 lp0 lc0

 BLKTAB:
    Best LZMA for raw_blktab(10068->2940): pb2 lp2 lc0

    ---with fastbytes=128---
    Start LZMA(blktab, 12536608 bytes)
    Yay result with pb0 lp0 lc0: 1386141
    Yay result with pb0 lp1 lc0: 1308137
    Yay result with pb0 lp2 lc0: 1305403
    Yay result with pb0 lp3 lc0: 1303072
    Yay result with pb1 lp1 lc0: 1238990
    Yay result with pb1 lp2 lc0: 1227973
    Yay result with pb1 lp3 lc0: 1221205
    Yay result with pb2 lp1 lc0: 1197035
    Yay result with pb2 lp2 lc0: 1188979
    Yay result with pb2 lp3 lc0: 1184531
    Yay result with pb3 lp1 lc0: 1183866
    Yay result with pb3 lp2 lc0: 1172994
    Yay result with pb3 lp3 lc0: 1169048
    Best LZMA for blktab(12536608->1169048): pb3 lp3 lc0

    It seems, lc=0 and pb=lp=N is a wise choice,
    where N is 2 for packed blktab and 3 for unpacked.

 FBLOCKS:
    For SPC sound+code data, the best results
     are between:
      pb0 lp0 lc0 (10%)
      pb0 lp0 lc1 (90%)
     For inotab, these were observed:
      pb1 lp0 lc1
      pb2 lp0 lc0
      pb1 lp1 lc0
      pb3 lp1 lc0
      pb1 lp2 lc0
      pb2 lp1 lc0

    For C source code data, the best results
     are between:
      pb1 lp0 lc3 (10%)
      pb0 lp0 lc3 (90%)
     Occasionally:
      pb0 lp1 lc0
      pb0 lp0 lc3 (mostly)
      pb0 lp0 lc2
      pb0 lp0 lc4
     Occasionally 2:
      pb0 lp0 lc8
      pb0 lp0 lc4

    BUT:
    Best LZMA for fblock(204944->192060): pb0 lp4 lc8 -- surprise! (INOTAB PROBABLY)

*/

static UInt32 SelectDictionarySizeFor(unsigned datasize)
{
   #if 1
    if(datasize >= (1 << 30U)) return 1 << 30U;
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

static void *SzAlloc(void*, size_t size)
    { return new unsigned char[size]; }
static void SzFree(void*, void *address)
    { unsigned char*a = (unsigned char*)address; delete[] a; }
static ISzAlloc LZMAalloc = { SzAlloc, SzFree };

class MemReader: public ISeqInStream
{
public:
    const unsigned char* const indata;
    const size_t         inlength;
    size_t pos;
public:
    MemReader(const unsigned char* d, size_t l)
        : ISeqInStream(), indata(d), inlength(l), pos(0)
    {
        Read = ReadMethod;
    }
    static SRes ReadMethod(void *pp, void *buf, size_t *size)
    {
        MemReader& p = *(MemReader*)pp;
        size_t rem = p.inlength-p.pos;
        size_t read = *size;
        if(read > rem) read= rem;
        std::memcpy(buf, &p.indata[p.pos], read);
        *size = read;
        p.pos += read;
        return SZ_OK;
    }
};
class MemWriter: public ISeqOutStream
{
public:
    std::vector<unsigned char> buf;
public:
    MemWriter(): ISeqOutStream(), buf() { Write = WriteMethod; }

    static size_t WriteMethod(void*pp, const void* from, size_t size)
    {
        MemWriter& p = *(MemWriter*)pp;
        const unsigned char* i = (const unsigned char*)from;
        p.buf.insert(p.buf.end(), i, i+size);
        return size;
    }
};

const std::vector<unsigned char> LZMACompress(const unsigned char* data, size_t length,
    unsigned pb,
    unsigned lp,
    unsigned lc)
{
    return LZMACompress(data,length, pb,lp,lc,
        SelectDictionarySizeFor(length));
}

const std::vector<unsigned char> LZMACompress(
    const unsigned char* data, size_t length,
    unsigned pb,
    unsigned lp,
    unsigned lc,
    unsigned dictionarysize)
{
    if(!length) return std::vector<unsigned char>();

    CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    props.dictSize = dictionarysize;
    props.pb       = pb;
    props.lp       = lp;
    props.lc       = lc;
    props.fb       = LZMA_NumFastBytes;
    props.mc       = LZMA_MatchFinderCycles;
    props.algo     = LZMA_AlgorithmNo;
    props.numThreads = 1;

    switch(LZMA_AlgorithmNo)
    {
        case 0: // quick: HC4
            props.btMode = 0;
            props.level = 1;
            break;
        case 1: // full: BT4
        default:
            props.level = 9;
            props.btMode       = 1;
            props.numHashBytes = 4;
            break;
    }

    CLzmaEncHandle p = LzmaEnc_Create(&LZMAalloc);
    struct AutoReleaseLzmaEnc
    {
        AutoReleaseLzmaEnc(CLzmaEncHandle pp) : p(pp) { }
        ~AutoReleaseLzmaEnc()
        { LzmaEnc_Destroy(p, &LZMAalloc, &LZMAalloc); }
        CLzmaEncHandle p;


        AutoReleaseLzmaEnc(const AutoReleaseLzmaEnc&);
        void operator=(const AutoReleaseLzmaEnc&);

    } AutoReleaser(p); // Create a destructor that ensures
    // that the CLzmaEncHandle is not leaked, even if an
    // exception happens

    int res = LzmaEnc_SetProps(p, &props);
    if(res != SZ_OK)
    {
    Error:
        return std::vector<unsigned char> ();
    }

    unsigned char propsEncoded[LZMA_PROPS_SIZE + 8];
    size_t propsSize = sizeof propsEncoded;
    res = LzmaEnc_WriteProperties(p, propsEncoded, &propsSize);
    if(res != SZ_OK) goto Error;

    MemReader is(data, length);
    MemWriter os;
    put_64(propsEncoded+LZMA_PROPS_SIZE, length);
    os.buf.insert(os.buf.end(), propsEncoded, propsEncoded+LZMA_PROPS_SIZE+8);

    res = LzmaEnc_Encode(p, &os, &is, 0, &LZMAalloc, &LZMAalloc);
    if(res != SZ_OK) goto Error;

    return os.buf;
}

const std::vector<unsigned char> LZMACompress(const unsigned char* data, size_t length)
{
    return LZMACompress(data, length,
        LZMA_PosStateBits,
        LZMA_LiteralPosStateBits,
        LZMA_LiteralContextBits);
}

#undef RC_NORMALIZE

const std::vector<unsigned char> LZMADeCompress
    (const unsigned char* data, size_t length, bool& ok)
{
    if(length <= LZMA_PROPS_SIZE+8)
    {
    /*clearly_not_ok:*/
        ok = false;
        return std::vector<unsigned char> ();
    }

    uint_least64_t out_sizemax = get_64(&data[LZMA_PROPS_SIZE]);

    /*if(out_sizemax >= (size_t)~0ULL)
    {
        // cannot even allocate a vector this large.
        goto clearly_not_ok;
    }*/

    std::vector<unsigned char> result(out_sizemax);

    ELzmaStatus status;
    SizeT destlen = result.size();
    SizeT srclen = length-(LZMA_PROPS_SIZE+8);
    int res = LzmaDecode(
        &result[0], &destlen,
        &data[LZMA_PROPS_SIZE+8], &srclen,
        &data[0], LZMA_PROPS_SIZE,
        LZMA_FINISH_END,
        &status,
        &LZMAalloc);

    /*
    std::fprintf(stderr, "res=%d, status=%d, in_done=%d (buf=%d), out_done=%d (max=%d)\n",
        res,
        (int)status,
        (int)srclen, (int)length,
        (int)destlen, (int)out_sizemax);
    */

    ok = res == SZ_OK && (status == LZMA_STATUS_FINISHED_WITH_MARK
                       || status == LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK)
      && srclen == (length-(LZMA_PROPS_SIZE+8))
      && destlen == out_sizemax;
    return result;
}

const std::vector<unsigned char> LZMADeCompress
    (const unsigned char* data, size_t length)
{
    bool ok_unused;
    return LZMADeCompress(data, length, ok_unused);
}

#if 0
#include <stdio.h>
int main(void)
{
    char Buf[2048*2048];
    int s = fread(Buf,1,sizeof(Buf),stdin);
    std::vector<unsigned char> result = LZMADeCompress(std::vector<unsigned char>(Buf,Buf+s));
    fwrite(&result[0],1,result.size(),stdout);
}
#endif

const std::vector<unsigned char> LZMACompressHeavy(const unsigned char* data, size_t length,
    const char* why)
{
    std::vector<unsigned char> bestresult;
    char best[512];
    bool first = true;
    if(LZMA_verbose >= 1)
    {
        std::fprintf(stderr, "Start LZMA(%s, %u bytes)\n", why, (unsigned)length);
        std::fflush(stderr);
    }

    unsigned minresultsize=0, maxresultsize=0;
    unsigned sizemap[5][5][9] = {{{0}}};

    bool use_small_dict = false;

    for(int compress_mode = 0; compress_mode < (5*5*9); ++compress_mode)
    {
        const unsigned pb = compress_mode % 5;
        const unsigned lp = (compress_mode / 5) % 5;
        const unsigned lc = (compress_mode / 5 / 5) % 9;

        std::vector<unsigned char>
            result = use_small_dict
                ? LZMACompress(data,length,pb,lp,lc, 4096)
                : LZMACompress(data,length,pb,lp,lc);

       {
        sizemap[pb][lp][lc] = result.size();

        if(first || result.size() < minresultsize) minresultsize = result.size();
        if(first || result.size() > maxresultsize) maxresultsize = result.size();
        if(first || result.size() < bestresult.size())
        {
            sprintf(best, "pb%u lp%u lc%u",
                pb,lp,lc);
            if(LZMA_verbose >= 1)
                std::fprintf(stderr, "Yay result with %s: %u\n", best, (unsigned)result.size());
            bestresult.swap(result);
            first = false;
        }
        else
        {
            char tmp[512];
            sprintf(tmp, "pb%u lp%u lc%u",
                pb,lp,lc);
            if(LZMA_verbose >= 2)
                std::fprintf(stderr, "Blaa result with %s: %u\n", tmp, (unsigned)result.size());
        }
        if(LZMA_verbose >= 2)
        {
            std::fprintf(stderr, "%*s\n", (5 * (4+9+2)), "");
            /* Visualize the size map: */
            std::string lines[6] = {};
            for(unsigned pbt = 0; pbt <= 4; ++pbt)
            {
                char buf[64]; sprintf(buf, "pb%u:%11s", pbt,"");
                lines[0] += buf;

                for(unsigned lpt = 0; lpt <= 4; ++lpt)
                {
                    char buf[64]; sprintf(buf, "lp%u:", lpt);
                    std::string line;
                    line += buf;
                    for(unsigned lct = 0; lct <= 8; ++lct)
                    {
                        unsigned s = sizemap[pbt][lpt][lct];
                        char c;
                        if(!s) c = '.';
                        else c = 'a' + ('z'-'a'+1)
                                     * (s - minresultsize)
                                     / (maxresultsize-minresultsize+1);
                        line += c;
                    }
                    lines[1 + lpt] += line + "  ";
                }
            }
            for(unsigned a=0; a<6; ++a) std::fprintf(stderr, "%s\n", lines[a].c_str());
            std::fprintf(stderr, "\33[%uA", 7);
        }
       }
    }
    if(LZMA_verbose >= 2)
        std::fprintf(stderr, "\n\n\n\n\n\n\n\n");

    if(LZMA_verbose >= 1)
    {
        std::fprintf(stderr, "Best LZMA for %s(%u->%u): %s\n",
            why,
            (unsigned)length,
            (unsigned)bestresult.size(),
            best);
    }
    std::fflush(stderr);
    return bestresult;
}

/*

The LZMA compression power is controlled by these parameters:
  Dictionary size (we use the maximum)
  Compression algorithm (we use BT4, the heaviest available)
  Number of fast bytes (we use the maximum)
  pb (0..4), lp (0..4) and lc (0..8) -- the effect of these depends on data.

Since the only parameters whose effect depends on the data to be compressed
are the three (pb, lp, lc), the "auto" and "full" compression algorithms
only try to find the optimal values for those.

The "auto" LZMA compression algorithm is based on these two assumptions:
  - It is possible to find the best value for each component (pb, lp, lc)
    by individually testing the most effective one of them while keeping
    the others static.
    I.e.,    step 1: pb=<find best>, lp=0, lc=0
             step 2: pb=<use result>, lp=<find best>, lc=0
             step 3: pb=<use result>, lp=<use result>, lc=<find best>
             final: pb=<use result>, lp=<use result>, lc=<use result>
  - That the effect of each of these components forms a parabolic function
    that has a starting point, ending point, and possibly a mountain or a
    valley somewhere in the middle, but never a valley _and_ a mountain, nor
    two valleys nor two mountains.
These assumptions are not always true, but it gets very close to the optimum.

The ParabolicFinder class below finds the lowest point in a parabolic curve
with a small number of tests, determining the shape of the curve by sampling
a few cue values as needed.

The algorithm is like this:
  Never check any value more than once.
  Check the first two values.
  If they differ, then check the last in sequence.
    If not, then check everything in sequential order.
  If the first two values and the last form an ascending sequence, accept the first value.
    If they form a descending sequence, start Focus Mode
    such that the focus lower limit is index 2 and upper
    limit is the second last. Then check the second last.
      If they don't, then check the third value of sequence,
      and everything else in sequential order.
  If in Focus Mode, check if being in the lower or upper end of the focus.
    If in upper end, check if the current value is bigger than the next one.
      If it is, end the process, because the smallest value has already been found.
        If not, next check the value at focus_low, and increase focus_low.
    If in lower end, check if the current value is bigger than the previous one.
      If it is, end the process, because the smallest value has already been found.
        If not, next check the value at focus_high, and decrease focus_high.

For any sample space, it generally does 3 tests, but if it detects a curve
forming a valley, it may do more.

Note that ParabolicFinder does not _indicate_ the lowest value. It leaves that
to the caller. It just stops searching when it thinks that no lower value will
be found.

Note: The effect of pb, lp and lc depend also on the dictionary size setting
and compression algorithm. You cannot estimate the optimal value for those
parameters reliably using different compression settings than in the actual case.

*/
class ParabolicFinder
{
public:
    enum QueryState      { Unknown, Pending, Done };
    enum InstructionType { HereYouGo, WaitingResults, End };
public:
    ParabolicFinder(unsigned Start, unsigned End)
        : begin(Start),
          results(End-Start+1, 0),
          state  (End-Start+1, Unknown),
          LeftRightSwap(false)
    {
    }

    InstructionType GetNextInstruction(unsigned& attempt)
    {
      InstructionType result = End;

      const int Last  = begin + results.size()-1;

      #define RetIns(n) do{ result = (n); goto DoneCrit; }while(0)
      #define RetVal(n) do{ state[attempt = (n)] = Pending; RetIns(HereYouGo); }while(0)

      {
        /*
        std::fprintf(stderr, "NextInstruction...");
        for(unsigned a=0; a<state.size(); ++a)
            std::fprintf(stderr, " %u=%s", a,
                state[a]==Unknown?"??"
               :state[a]==Done?"Ok"
               :"..");
        std::fprintf(stderr, "\n");*/

        if(CountUnknown() == 0)
        {
            // No unassigned slots remain. Don't need more workers.
            RetIns(End);
        }

        if(1) // scope for local variables
        {
            // Alternate which side to do next if both are available.
            bool LeftSideFirst = LeftRightSwap ^= 1;

            // Check left side descend type
            int LeftSideNext = -1; bool LeftSideDoable = false;
            for(int c=0; c<=Last; ++c)
                switch(state[c])
                {
                    case Unknown: LeftSideNext = c; LeftSideDoable = true; goto ExitLeftSideFor;
                    case Pending: LeftSideNext = c; LeftSideDoable = false; goto ExitLeftSideFor;
                    case Done:
                        if(c == 0) continue;
                        if(results[c] > results[c-1])
                        {
                            // Left side stopped descending.
                            if(state[Last] != Unknown) RetIns(End);
                            goto ExitLeftSideFor;
                        }
                        else if(results[c] == results[c-1])
                            LeftSideFirst = true;
                }
        ExitLeftSideFor: ;

            // Check right side descend type
            int RightSideNext = -1; bool RightSideDoable = false;
            for(int c=Last; c>=0; --c)
                switch(state[c])
                {
                    case Unknown: RightSideNext = c; RightSideDoable = true; goto ExitRightSideFor;
                    case Pending: RightSideNext = c; RightSideDoable = false; goto ExitRightSideFor;
                    case Done:
                        if(c == Last) continue;
                        if(results[c] > results[c+1])
                        {
                            // Right side stopped descending.
                            if(state[0] != Unknown) RetIns(End);
                            goto ExitRightSideFor;
                        }
                        else if(results[c] == results[c+1])
                            LeftSideFirst = false;
                }
        ExitRightSideFor: ;

            if(!LeftSideFirst)
                 { std::swap(LeftSideDoable, RightSideDoable);
                   std::swap(LeftSideNext,   RightSideNext); }

            if(LeftSideDoable) RetVal(LeftSideNext);
            if(RightSideDoable) RetVal(RightSideNext);

            // If we have excess threads and work to do, give them something
            if(CountHandled() > 2) if(LeftSideNext >= 0) RetVal(LeftSideNext);
            if(CountHandled() > 3) if(RightSideNext >= 0) RetVal(RightSideNext);

            RetIns(WaitingResults);
        }

      DoneCrit: ;
      }
      return result;
    }

    void GotResult(unsigned attempt, unsigned value)
    {
      {
        results[attempt] = value;
        state[attempt]   = Done;
      }
    }

private:
    unsigned CountUnknown() const
    {
        unsigned result=0;
        for(size_t a=0, b=state.size(); a<b; ++a)
            if(state[a] == Unknown) ++result;
        return result;
    }
    unsigned CountHandled() const
    {
        return state.size() - CountUnknown();
    }
private:
    unsigned begin;
    std::vector<unsigned>   results;
    std::vector<QueryState> state;
    bool LeftRightSwap;
};

static void LZMACompressAutoHelper(
    const unsigned char* data, size_t length,
    bool use_small_dict,
    const char* why,
    unsigned& pb, unsigned& lp, unsigned& lc,
    unsigned& which_iterate, ParabolicFinder& finder,
    bool&first, std::vector<unsigned char>& bestresult)
{
    for(;;)
    {
        unsigned t=0;
        switch(finder.GetNextInstruction(t))
        {
            case ParabolicFinder::End:
                return;
            case ParabolicFinder::HereYouGo:
                break;
            case ParabolicFinder::WaitingResults:
                ForceSwitchThread();
                continue;
        }

        const unsigned try_pb = &which_iterate == &pb ? t : pb;
        const unsigned try_lp = &which_iterate == &lp ? t : lp;
        const unsigned try_lc = &which_iterate == &lc ? t : lc;

        if(LZMA_verbose >= 2)
            std::fprintf(stderr, "%s:Trying pb%u lp%u lc%u\n",
                why,try_pb,try_lp,try_lc);

        std::vector<unsigned char> result = use_small_dict
            ? LZMACompress(data,length,try_pb,try_lp,try_lc, 65536)
            : LZMACompress(data,length,try_pb,try_lp,try_lc);

        if(LZMA_verbose >= 2)
            std::fprintf(stderr, "%s:       pb%u lp%u lc%u -> %u\n",
                why,try_pb,try_lp,try_lc, (unsigned)result.size());

        finder.GotResult(t, result.size());

      {
        if(first || result.size() <= bestresult.size())
        {
            first    = false;
            bestresult.swap(result);
            which_iterate = t;
        }
      }
    }
}


const std::vector<unsigned char> LZMACompressAuto(const unsigned char* data, size_t length,
    const char* why)
{
    if(LZMA_verbose >= 1)
    {
        std::fprintf(stderr, "Start LZMA(%s, %u bytes)\n", why, (unsigned)length);
        std::fflush(stderr);
    }

    unsigned backup_algorithm = LZMA_AlgorithmNo;

    bool use_small_dict = false;//length >= 1048576;

    if(use_small_dict) LZMA_AlgorithmNo = 0;

    unsigned pb=0, lp=0, lc=0;

    std::vector<unsigned char> bestresult;

  {
    ParabolicFinder pb_finder(0,4);
    ParabolicFinder lp_finder(0,4);
    ParabolicFinder lc_finder(0,8);
    bool first=true;
   {
    /* Using parallelism here. However, we need barriers after
     * each step, because the comparisons are made based on the
     * result size, and if the pb/lp/lc values other than the
     * one being focused change, it won't work. Only one parameter
     * must change in the loop.
     */

    /* step 1: find best value in pb axis */
    LZMACompressAutoHelper(data,length,use_small_dict,why,
        pb, lp, lc,
        pb, pb_finder, first, bestresult);


    lp_finder.GotResult(lp, bestresult.size());

    /* step 2: find best value in lp axis */
    LZMACompressAutoHelper(data,length,use_small_dict,why,
        pb, lp, lc,
        lp, lp_finder, first, bestresult);

    lc_finder.GotResult(lc, bestresult.size());

    /* step 3: find best value in lc axis */
    LZMACompressAutoHelper(data,length,use_small_dict,why,
        pb, lp, lc,
        lc, lc_finder, first, bestresult);
   }
  }

    if(use_small_dict || LZMA_AlgorithmNo != backup_algorithm)
    {
        LZMA_AlgorithmNo = backup_algorithm;
        bestresult = LZMACompress(data,length, pb,lp,lc);
    }

    if(LZMA_verbose >= 1)
    {
        std::fprintf(stderr, "Best LZMA for %s(%u->%u): pb%u lp%u lc%u\n",
            why,
            (unsigned)length,
            (unsigned)bestresult.size(),
            pb,lp,lc);
    }
    std::fflush(stderr);

    return bestresult;
}

const std::vector<unsigned char>
    DoLZMACompress(int HeavyLevel,
        const unsigned char* data, size_t length,
        const char* why)
{
    if(HeavyLevel >= 2) return LZMACompressHeavy(data,length, why);
    if(HeavyLevel >= 1) return LZMACompressAuto(data,length, why);
    return LZMACompress(data,length);
}

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
	if (result.size() <= (SizeT)dst_len)
		std::memcpy(dst, &result[0], result.size());
	else
	{
		fprintf(stderr, "Not copying %d bytes to %d-byte buffer!\n",
			(unsigned int)result.size(), dst_len);
		exit(1);
	}
}

}

