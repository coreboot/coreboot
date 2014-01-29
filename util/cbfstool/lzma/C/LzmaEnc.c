/* LzmaEnc.c -- LZMA Encoder
2009-11-24 : Igor Pavlov : Public domain */

#include <string.h>

#include "LzmaEnc.h"

#include "LzFind.h"

#define kBlockSizeMax ((1 << LZMA_NUM_BLOCK_SIZE_BITS) - 1)

#define kBlockSize (9 << 10)
#define kUnpackBlockSize (1 << 18)
#define kMatchArraySize (1 << 21)
#define kMatchRecordMaxSize ((LZMA_MATCH_LEN_MAX * 2 + 3) * LZMA_MATCH_LEN_MAX)

#define kNumMaxDirectBits (31)

#define kNumTopBits 24
#define kTopValue ((uint32_t)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5
#define kProbInitValue (kBitModelTotal >> 1)

#define kNumMoveReducingBits 4
#define kNumBitPriceShiftBits 4
#define kBitPrice (1 << kNumBitPriceShiftBits)

void LzmaEncProps_Init(struct CLzmaEncProps *p)
{
  p->level = 5;
  p->dictSize = p->mc = 0;
  p->lc = p->lp = p->pb = p->algo = p->fb = p->btMode = p->numHashBytes = p->numThreads = -1;
  p->writeEndMark = 0;
}

void LzmaEncProps_Normalize(struct CLzmaEncProps *p)
{
  int level = p->level;
  if (level < 0) level = 5;
  p->level = level;
  if (p->dictSize == 0) p->dictSize = (level <= 5 ? (1 << (level * 2 + 14)) : (level == 6 ? (1 << 25) : (1 << 26)));
  if (p->lc < 0) p->lc = 3;
  if (p->lp < 0) p->lp = 0;
  if (p->pb < 0) p->pb = 2;
  if (p->algo < 0) p->algo = (level < 5 ? 0 : 1);
  if (p->fb < 0) p->fb = (level < 7 ? 32 : 64);
  if (p->btMode < 0) p->btMode = (p->algo == 0 ? 0 : 1);
  if (p->numHashBytes < 0) p->numHashBytes = 4;
  if (p->mc == 0)  p->mc = (16 + (p->fb >> 1)) >> (p->btMode ? 0 : 1);
  if (p->numThreads < 0)
    p->numThreads = 1;
}

uint32_t LzmaEncProps_GetDictSize(const struct CLzmaEncProps *props2)
{
  struct CLzmaEncProps props = *props2;
  LzmaEncProps_Normalize(&props);
  return props.dictSize;
}

#define kNumLogBits (9 + (int)sizeof(size_t) / 2)
#define kDicLogSizeMaxCompress ((kNumLogBits - 1) * 2 + 7)

static void LzmaEnc_FastPosInit(uint8_t *g_FastPos)
{
  int c = 2, slotFast;
  g_FastPos[0] = 0;
  g_FastPos[1] = 1;

  for (slotFast = 2; slotFast < kNumLogBits * 2; slotFast++)
  {
    uint32_t k = (1 << ((slotFast >> 1) - 1));
    uint32_t j;
    for (j = 0; j < k; j++, c++)
      g_FastPos[c] = (uint8_t)slotFast;
  }
}

#define BSR2_RET(pos, res) { uint32_t macro_i = 6 + ((kNumLogBits - 1) & \
  (0 - (((((uint32_t)1 << (kNumLogBits + 6)) - 1) - pos) >> 31))); \
  res = p->g_FastPos[pos >> macro_i] + (macro_i * 2); }
/*
#define BSR2_RET(pos, res) { res = (pos < (1 << (kNumLogBits + 6))) ? \
  p->g_FastPos[pos >> 6] + 12 : \
  p->g_FastPos[pos >> (6 + kNumLogBits - 1)] + (6 + (kNumLogBits - 1)) * 2; }
*/

#define GetPosSlot1(pos) p->g_FastPos[pos]
#define GetPosSlot2(pos, res) { BSR2_RET(pos, res); }
#define GetPosSlot(pos, res) { if (pos < kNumFullDistances) res = p->g_FastPos[pos]; else BSR2_RET(pos, res); }

#define LZMA_NUM_REPS 4

typedef unsigned CState;

struct COptimal
{
  uint32_t price;

  CState state;
  int prev1IsChar;
  int prev2;

  uint32_t posPrev2;
  uint32_t backPrev2;

  uint32_t posPrev;
  uint32_t backPrev;
  uint32_t backs[LZMA_NUM_REPS];
};

#define kNumOpts (1 << 12)

#define kNumLenToPosStates 4
#define kNumPosSlotBits 6
#define kDicLogSizeMin 0
#define kDicLogSizeMax 32
#define kDistTableSizeMax (kDicLogSizeMax * 2)


#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)
#define kAlignMask (kAlignTableSize - 1)

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumPosModels (kEndPosModelIndex - kStartPosModelIndex)

#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

typedef uint16_t CLzmaProb;


#define LZMA_PB_MAX 4
#define LZMA_LC_MAX 8
#define LZMA_LP_MAX 4

#define LZMA_NUM_PB_STATES_MAX (1 << LZMA_PB_MAX)


#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumMidBits 3
#define kLenNumMidSymbols (1 << kLenNumMidBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define kLenNumSymbolsTotal (kLenNumLowSymbols + kLenNumMidSymbols + kLenNumHighSymbols)

#define LZMA_MATCH_LEN_MIN 2
#define LZMA_MATCH_LEN_MAX (LZMA_MATCH_LEN_MIN + kLenNumSymbolsTotal - 1)

#define kNumStates 12

struct CLenEnc
{
  CLzmaProb choice;
  CLzmaProb choice2;
  CLzmaProb low[LZMA_NUM_PB_STATES_MAX << kLenNumLowBits];
  CLzmaProb mid[LZMA_NUM_PB_STATES_MAX << kLenNumMidBits];
  CLzmaProb high[kLenNumHighSymbols];
};

struct CLenPriceEnc
{
  struct CLenEnc p;
  uint32_t prices[LZMA_NUM_PB_STATES_MAX][kLenNumSymbolsTotal];
  uint32_t tableSize;
  uint32_t counters[LZMA_NUM_PB_STATES_MAX];
};

struct CRangeEnc
{
  uint32_t range;
  uint8_t cache;
  uint64_t low;
  uint64_t cacheSize;
  uint8_t *buf;
  uint8_t *bufLim;
  uint8_t *bufBase;
  struct ISeqOutStream *outStream;
  uint64_t processed;
  SRes res;
};

struct CSaveState
{
  CLzmaProb *litProbs;

  CLzmaProb isMatch[kNumStates][LZMA_NUM_PB_STATES_MAX];
  CLzmaProb isRep[kNumStates];
  CLzmaProb isRepG0[kNumStates];
  CLzmaProb isRepG1[kNumStates];
  CLzmaProb isRepG2[kNumStates];
  CLzmaProb isRep0Long[kNumStates][LZMA_NUM_PB_STATES_MAX];

  CLzmaProb posSlotEncoder[kNumLenToPosStates][1 << kNumPosSlotBits];
  CLzmaProb posEncoders[kNumFullDistances - kEndPosModelIndex];
  CLzmaProb posAlignEncoder[1 << kNumAlignBits];

  struct CLenPriceEnc lenEnc;
  struct CLenPriceEnc repLenEnc;

  uint32_t reps[LZMA_NUM_REPS];
  uint32_t state;
};

struct CLzmaEnc
{
  struct IMatchFinder matchFinder;
  void *matchFinderObj;

  struct CMatchFinder matchFinderBase;

  uint32_t optimumEndIndex;
  uint32_t optimumCurrentIndex;

  uint32_t longestMatchLength;
  uint32_t numPairs;
  uint32_t numAvail;
  struct COptimal opt[kNumOpts];

  #ifndef LZMA_LOG_BSR
  uint8_t g_FastPos[1 << kNumLogBits];
  #endif

  uint32_t ProbPrices[kBitModelTotal >> kNumMoveReducingBits];
  uint32_t matches[LZMA_MATCH_LEN_MAX * 2 + 2 + 1];
  uint32_t numFastuint8_ts;
  uint32_t additionalOffset;
  uint32_t reps[LZMA_NUM_REPS];
  uint32_t state;

  uint32_t posSlotPrices[kNumLenToPosStates][kDistTableSizeMax];
  uint32_t distancesPrices[kNumLenToPosStates][kNumFullDistances];
  uint32_t alignPrices[kAlignTableSize];
  uint32_t alignPriceCount;

  uint32_t distTableSize;

  unsigned lc, lp, pb;
  unsigned lpMask, pbMask;

  CLzmaProb *litProbs;

  CLzmaProb isMatch[kNumStates][LZMA_NUM_PB_STATES_MAX];
  CLzmaProb isRep[kNumStates];
  CLzmaProb isRepG0[kNumStates];
  CLzmaProb isRepG1[kNumStates];
  CLzmaProb isRepG2[kNumStates];
  CLzmaProb isRep0Long[kNumStates][LZMA_NUM_PB_STATES_MAX];

  CLzmaProb posSlotEncoder[kNumLenToPosStates][1 << kNumPosSlotBits];
  CLzmaProb posEncoders[kNumFullDistances - kEndPosModelIndex];
  CLzmaProb posAlignEncoder[1 << kNumAlignBits];

  struct CLenPriceEnc lenEnc;
  struct CLenPriceEnc repLenEnc;

  unsigned lclp;

  bool fastMode;

  struct CRangeEnc rc;

  bool writeEndMark;
  uint64_t nowPos64;
  uint32_t matchPriceCount;
  bool finished;
  bool multiThread;

  SRes result;
  uint32_t dictSize;
  uint32_t matchFinderCycles;

  int needInit;

  struct CSaveState saveState;
};

/*static void LzmaEnc_SaveState(CLzmaEncHandle pp)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  CSaveState *dest = &p->saveState;
  int i;
  dest->lenEnc = p->lenEnc;
  dest->repLenEnc = p->repLenEnc;
  dest->state = p->state;

  for (i = 0; i < kNumStates; i++)
  {
    memcpy(dest->isMatch[i], p->isMatch[i], sizeof(p->isMatch[i]));
    memcpy(dest->isRep0Long[i], p->isRep0Long[i], sizeof(p->isRep0Long[i]));
  }
  for (i = 0; i < kNumLenToPosStates; i++)
    memcpy(dest->posSlotEncoder[i], p->posSlotEncoder[i], sizeof(p->posSlotEncoder[i]));
  memcpy(dest->isRep, p->isRep, sizeof(p->isRep));
  memcpy(dest->isRepG0, p->isRepG0, sizeof(p->isRepG0));
  memcpy(dest->isRepG1, p->isRepG1, sizeof(p->isRepG1));
  memcpy(dest->isRepG2, p->isRepG2, sizeof(p->isRepG2));
  memcpy(dest->posEncoders, p->posEncoders, sizeof(p->posEncoders));
  memcpy(dest->posAlignEncoder, p->posAlignEncoder, sizeof(p->posAlignEncoder));
  memcpy(dest->reps, p->reps, sizeof(p->reps));
  memcpy(dest->litProbs, p->litProbs, (0x300 << p->lclp) * sizeof(CLzmaProb));
}*/

/*static void LzmaEnc_RestoreState(CLzmaEncHandle pp)
{
  CLzmaEnc *dest = (CLzmaEnc *)pp;
  const CSaveState *p = &dest->saveState;
  int i;
  dest->lenEnc = p->lenEnc;
  dest->repLenEnc = p->repLenEnc;
  dest->state = p->state;

  for (i = 0; i < kNumStates; i++)
  {
    memcpy(dest->isMatch[i], p->isMatch[i], sizeof(p->isMatch[i]));
    memcpy(dest->isRep0Long[i], p->isRep0Long[i], sizeof(p->isRep0Long[i]));
  }
  for (i = 0; i < kNumLenToPosStates; i++)
    memcpy(dest->posSlotEncoder[i], p->posSlotEncoder[i], sizeof(p->posSlotEncoder[i]));
  memcpy(dest->isRep, p->isRep, sizeof(p->isRep));
  memcpy(dest->isRepG0, p->isRepG0, sizeof(p->isRepG0));
  memcpy(dest->isRepG1, p->isRepG1, sizeof(p->isRepG1));
  memcpy(dest->isRepG2, p->isRepG2, sizeof(p->isRepG2));
  memcpy(dest->posEncoders, p->posEncoders, sizeof(p->posEncoders));
  memcpy(dest->posAlignEncoder, p->posAlignEncoder, sizeof(p->posAlignEncoder));
  memcpy(dest->reps, p->reps, sizeof(p->reps));
  memcpy(dest->litProbs, p->litProbs, (0x300 << dest->lclp) * sizeof(CLzmaProb));
}*/

SRes LzmaEnc_SetProps(CLzmaEncHandle pp, const struct CLzmaEncProps *props2)
{
  struct CLzmaEnc *p = (struct CLzmaEnc *)pp;
  struct CLzmaEncProps props = *props2;
  LzmaEncProps_Normalize(&props);

  if (props.lc > LZMA_LC_MAX || props.lp > LZMA_LP_MAX || props.pb > LZMA_PB_MAX ||
      props.dictSize > (1 << kDicLogSizeMaxCompress) || props.dictSize > (1 << 30))
    return SZ_ERROR_PARAM;
  p->dictSize = props.dictSize;
  p->matchFinderCycles = props.mc;
  {
    unsigned fb = props.fb;
    if (fb < 5)
      fb = 5;
    if (fb > LZMA_MATCH_LEN_MAX)
      fb = LZMA_MATCH_LEN_MAX;
    p->numFastuint8_ts = fb;
  }
  p->lc = props.lc;
  p->lp = props.lp;
  p->pb = props.pb;
  p->fastMode = (props.algo == 0);
  p->matchFinderBase.btMode = props.btMode;
  {
    uint32_t numHashBytes = 4;
    if (props.btMode)
    {
      if (props.numHashBytes < 2)
        numHashBytes = 2;
      else if (props.numHashBytes < 4)
        numHashBytes = props.numHashBytes;
    }
    p->matchFinderBase.numHashBytes = numHashBytes;
  }

  p->matchFinderBase.cutValue = props.mc;

  p->writeEndMark = props.writeEndMark;

  return SZ_OK;
}

static const int kLiteralNextStates[kNumStates] = {0, 0, 0, 0, 1, 2, 3, 4,  5,  6,   4, 5};
static const int kMatchNextStates[kNumStates]   = {7, 7, 7, 7, 7, 7, 7, 10, 10, 10, 10, 10};
static const int kRepNextStates[kNumStates]     = {8, 8, 8, 8, 8, 8, 8, 11, 11, 11, 11, 11};
static const int kShortRepNextStates[kNumStates]= {9, 9, 9, 9, 9, 9, 9, 11, 11, 11, 11, 11};

#define IsCharState(s) ((s) < 7)

#define GetLenToPosState(len) (((len) < kNumLenToPosStates + 1) ? (len) - 2 : kNumLenToPosStates - 1)

#define kInfinityPrice (1 << 30)

static void RangeEnc_Construct(struct CRangeEnc *p)
{
  p->outStream = 0;
  p->bufBase = 0;
}

#define RangeEnc_GetProcessed(p) ((p)->processed + ((p)->buf - (p)->bufBase) + (p)->cacheSize)

#define RC_BUF_SIZE (1 << 16)
static int RangeEnc_Alloc(struct CRangeEnc *p, struct ISzAlloc *alloc)
{
  if (p->bufBase == 0)
  {
    p->bufBase = (uint8_t *)alloc->Alloc(alloc, RC_BUF_SIZE);
    if (p->bufBase == 0)
      return 0;
    p->bufLim = p->bufBase + RC_BUF_SIZE;
  }
  return 1;
}

static void RangeEnc_Free(struct CRangeEnc *p, struct ISzAlloc *alloc)
{
  alloc->Free(alloc, p->bufBase);
  p->bufBase = 0;
}

static void RangeEnc_Init(struct CRangeEnc *p)
{
  /* Stream.Init(); */
  p->low = 0;
  p->range = 0xFFFFFFFF;
  p->cacheSize = 1;
  p->cache = 0;

  p->buf = p->bufBase;

  p->processed = 0;
  p->res = SZ_OK;
}

static void RangeEnc_FlushStream(struct CRangeEnc *p)
{
  size_t num;
  if (p->res != SZ_OK)
    return;
  num = p->buf - p->bufBase;
  if (num != p->outStream->Write(p->outStream, p->bufBase, num))
    p->res = SZ_ERROR_WRITE;
  p->processed += num;
  p->buf = p->bufBase;
}

static void RangeEnc_ShiftLow(struct CRangeEnc *p)
{
  if ((uint32_t)p->low < (uint32_t)0xFF000000 || (int)(p->low >> 32) != 0)
  {
    uint8_t temp = p->cache;
    do
    {
      uint8_t *buf = p->buf;
      *buf++ = (uint8_t)(temp + (uint8_t)(p->low >> 32));
      p->buf = buf;
      if (buf == p->bufLim)
        RangeEnc_FlushStream(p);
      temp = 0xFF;
    }
    while (--p->cacheSize != 0);
    p->cache = (uint8_t)((uint32_t)p->low >> 24);
  }
  p->cacheSize++;
  p->low = (uint32_t)p->low << 8;
}

static void RangeEnc_FlushData(struct CRangeEnc *p)
{
  int i;
  for (i = 0; i < 5; i++)
    RangeEnc_ShiftLow(p);
}

static void RangeEnc_EncodeDirectBits(struct CRangeEnc *p, uint32_t value, int numBits)
{
  do
  {
    p->range >>= 1;
    p->low += p->range & (0 - ((value >> --numBits) & 1));
    if (p->range < kTopValue)
    {
      p->range <<= 8;
      RangeEnc_ShiftLow(p);
    }
  }
  while (numBits != 0);
}

static void RangeEnc_EncodeBit(struct CRangeEnc *p, CLzmaProb *prob, uint32_t symbol)
{
  uint32_t ttt = *prob;
  uint32_t newBound = (p->range >> kNumBitModelTotalBits) * ttt;
  if (symbol == 0)
  {
    p->range = newBound;
    ttt += (kBitModelTotal - ttt) >> kNumMoveBits;
  }
  else
  {
    p->low += newBound;
    p->range -= newBound;
    ttt -= ttt >> kNumMoveBits;
  }
  *prob = (CLzmaProb)ttt;
  if (p->range < kTopValue)
  {
    p->range <<= 8;
    RangeEnc_ShiftLow(p);
  }
}

static void LitEnc_Encode(struct CRangeEnc *p, CLzmaProb *probs, uint32_t symbol)
{
  symbol |= 0x100;
  do
  {
    RangeEnc_EncodeBit(p, probs + (symbol >> 8), (symbol >> 7) & 1);
    symbol <<= 1;
  }
  while (symbol < 0x10000);
}

static void LitEnc_EncodeMatched(struct CRangeEnc *p, CLzmaProb *probs, uint32_t symbol, uint32_t matchuint8_t)
{
  uint32_t offs = 0x100;
  symbol |= 0x100;
  do
  {
    matchuint8_t <<= 1;
    RangeEnc_EncodeBit(p, probs + (offs + (matchuint8_t & offs) + (symbol >> 8)), (symbol >> 7) & 1);
    symbol <<= 1;
    offs &= ~(matchuint8_t ^ symbol);
  }
  while (symbol < 0x10000);
}

static void LzmaEnc_InitPriceTables(uint32_t *ProbPrices)
{
  uint32_t i;
  for (i = (1 << kNumMoveReducingBits) / 2; i < kBitModelTotal; i += (1 << kNumMoveReducingBits))
  {
    const int kCyclesBits = kNumBitPriceShiftBits;
    uint32_t w = i;
    uint32_t bitCount = 0;
    int j;
    for (j = 0; j < kCyclesBits; j++)
    {
      w = w * w;
      bitCount <<= 1;
      while (w >= ((uint32_t)1 << 16))
      {
        w >>= 1;
        bitCount++;
      }
    }
    ProbPrices[i >> kNumMoveReducingBits] = ((kNumBitModelTotalBits << kCyclesBits) - 15 - bitCount);
  }
}


#define GET_PRICE(prob, symbol) \
  p->ProbPrices[((prob) ^ (((-(int)(symbol))) & (kBitModelTotal - 1))) >> kNumMoveReducingBits];

#define GET_PRICEa(prob, symbol) \
  ProbPrices[((prob) ^ ((-((int)(symbol))) & (kBitModelTotal - 1))) >> kNumMoveReducingBits];

#define GET_PRICE_0(prob) p->ProbPrices[(prob) >> kNumMoveReducingBits]
#define GET_PRICE_1(prob) p->ProbPrices[((prob) ^ (kBitModelTotal - 1)) >> kNumMoveReducingBits]

#define GET_PRICE_0a(prob) ProbPrices[(prob) >> kNumMoveReducingBits]
#define GET_PRICE_1a(prob) ProbPrices[((prob) ^ (kBitModelTotal - 1)) >> kNumMoveReducingBits]

static uint32_t LitEnc_GetPrice(const CLzmaProb *probs, uint32_t symbol, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  symbol |= 0x100;
  do
  {
    price += GET_PRICEa(probs[symbol >> 8], (symbol >> 7) & 1);
    symbol <<= 1;
  }
  while (symbol < 0x10000);
  return price;
}

static uint32_t LitEnc_GetPriceMatched(const CLzmaProb *probs, uint32_t symbol, uint32_t matchuint8_t, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  uint32_t offs = 0x100;
  symbol |= 0x100;
  do
  {
    matchuint8_t <<= 1;
    price += GET_PRICEa(probs[offs + (matchuint8_t & offs) + (symbol >> 8)], (symbol >> 7) & 1);
    symbol <<= 1;
    offs &= ~(matchuint8_t ^ symbol);
  }
  while (symbol < 0x10000);
  return price;
}


static void RcTree_Encode(struct CRangeEnc *rc, CLzmaProb *probs, int numBitLevels, uint32_t symbol)
{
  uint32_t m = 1;
  int i;
  for (i = numBitLevels; i != 0;)
  {
    uint32_t bit;
    i--;
    bit = (symbol >> i) & 1;
    RangeEnc_EncodeBit(rc, probs + m, bit);
    m = (m << 1) | bit;
  }
}

static void RcTree_ReverseEncode(struct CRangeEnc *rc, CLzmaProb *probs, int numBitLevels, uint32_t symbol)
{
  uint32_t m = 1;
  int i;
  for (i = 0; i < numBitLevels; i++)
  {
    uint32_t bit = symbol & 1;
    RangeEnc_EncodeBit(rc, probs + m, bit);
    m = (m << 1) | bit;
    symbol >>= 1;
  }
}

static uint32_t RcTree_GetPrice(const CLzmaProb *probs, int numBitLevels, uint32_t symbol, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  symbol |= (1 << numBitLevels);
  while (symbol != 1)
  {
    price += GET_PRICEa(probs[symbol >> 1], symbol & 1);
    symbol >>= 1;
  }
  return price;
}

static uint32_t RcTree_ReverseGetPrice(const CLzmaProb *probs, int numBitLevels, uint32_t symbol, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  uint32_t m = 1;
  int i;
  for (i = numBitLevels; i != 0; i--)
  {
    uint32_t bit = symbol & 1;
    symbol >>= 1;
    price += GET_PRICEa(probs[m], bit);
    m = (m << 1) | bit;
  }
  return price;
}


static void LenEnc_Init(struct CLenEnc *p)
{
  unsigned i;
  p->choice = p->choice2 = kProbInitValue;
  for (i = 0; i < (LZMA_NUM_PB_STATES_MAX << kLenNumLowBits); i++)
    p->low[i] = kProbInitValue;
  for (i = 0; i < (LZMA_NUM_PB_STATES_MAX << kLenNumMidBits); i++)
    p->mid[i] = kProbInitValue;
  for (i = 0; i < kLenNumHighSymbols; i++)
    p->high[i] = kProbInitValue;
}

static void LenEnc_Encode(struct CLenEnc *p, struct CRangeEnc *rc, uint32_t symbol, uint32_t posState)
{
  if (symbol < kLenNumLowSymbols)
  {
    RangeEnc_EncodeBit(rc, &p->choice, 0);
    RcTree_Encode(rc, p->low + (posState << kLenNumLowBits), kLenNumLowBits, symbol);
  }
  else
  {
    RangeEnc_EncodeBit(rc, &p->choice, 1);
    if (symbol < kLenNumLowSymbols + kLenNumMidSymbols)
    {
      RangeEnc_EncodeBit(rc, &p->choice2, 0);
      RcTree_Encode(rc, p->mid + (posState << kLenNumMidBits), kLenNumMidBits, symbol - kLenNumLowSymbols);
    }
    else
    {
      RangeEnc_EncodeBit(rc, &p->choice2, 1);
      RcTree_Encode(rc, p->high, kLenNumHighBits, symbol - kLenNumLowSymbols - kLenNumMidSymbols);
    }
  }
}

static void LenEnc_SetPrices(struct CLenEnc *p, uint32_t posState, uint32_t numSymbols, uint32_t *prices, uint32_t *ProbPrices)
{
  uint32_t a0 = GET_PRICE_0a(p->choice);
  uint32_t a1 = GET_PRICE_1a(p->choice);
  uint32_t b0 = a1 + GET_PRICE_0a(p->choice2);
  uint32_t b1 = a1 + GET_PRICE_1a(p->choice2);
  uint32_t i = 0;
  for (i = 0; i < kLenNumLowSymbols; i++)
  {
    if (i >= numSymbols)
      return;
    prices[i] = a0 + RcTree_GetPrice(p->low + (posState << kLenNumLowBits), kLenNumLowBits, i, ProbPrices);
  }
  for (; i < kLenNumLowSymbols + kLenNumMidSymbols; i++)
  {
    if (i >= numSymbols)
      return;
    prices[i] = b0 + RcTree_GetPrice(p->mid + (posState << kLenNumMidBits), kLenNumMidBits, i - kLenNumLowSymbols, ProbPrices);
  }
  for (; i < numSymbols; i++)
    prices[i] = b1 + RcTree_GetPrice(p->high, kLenNumHighBits, i - kLenNumLowSymbols - kLenNumMidSymbols, ProbPrices);
}

static void LenPriceEnc_UpdateTable(struct CLenPriceEnc *p, uint32_t posState, uint32_t *ProbPrices)
{
  LenEnc_SetPrices(&p->p, posState, p->tableSize, p->prices[posState], ProbPrices);
  p->counters[posState] = p->tableSize;
}

static void LenPriceEnc_UpdateTables(struct CLenPriceEnc *p, uint32_t numPosStates, uint32_t *ProbPrices)
{
  uint32_t posState;
  for (posState = 0; posState < numPosStates; posState++)
    LenPriceEnc_UpdateTable(p, posState, ProbPrices);
}

static void LenEnc_Encode2(struct CLenPriceEnc *p, struct CRangeEnc *rc, uint32_t symbol, uint32_t posState, bool updatePrice, uint32_t *ProbPrices)
{
  LenEnc_Encode(&p->p, rc, symbol, posState);
  if (updatePrice)
    if (--p->counters[posState] == 0)
      LenPriceEnc_UpdateTable(p, posState, ProbPrices);
}




static void MovePos(struct CLzmaEnc *p, uint32_t num)
{
  if (num != 0)
  {
    p->additionalOffset += num;
    p->matchFinder.Skip(p->matchFinderObj, num);
  }
}

static uint32_t ReadMatchDistances(struct CLzmaEnc *p, uint32_t *numDistancePairsRes)
{
  uint32_t lenRes = 0, numPairs;
  p->numAvail = p->matchFinder.GetNumAvailableBytes(p->matchFinderObj);
  numPairs = p->matchFinder.GetMatches(p->matchFinderObj, p->matches);
  if (numPairs > 0)
  {
    lenRes = p->matches[numPairs - 2];
    if (lenRes == p->numFastuint8_ts)
    {
      const uint8_t *pby = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
      uint32_t distance = p->matches[numPairs - 1] + 1;
      uint32_t numAvail = p->numAvail;
      if (numAvail > LZMA_MATCH_LEN_MAX)
        numAvail = LZMA_MATCH_LEN_MAX;
      {
        const uint8_t *pby2 = pby - distance;
        for (; lenRes < numAvail && pby[lenRes] == pby2[lenRes]; lenRes++);
      }
    }
  }
  p->additionalOffset++;
  *numDistancePairsRes = numPairs;
  return lenRes;
}


#define MakeAsChar(p) (p)->backPrev = (uint32_t)(-1); (p)->prev1IsChar = false;
#define MakeAsShortRep(p) (p)->backPrev = 0; (p)->prev1IsChar = false;
#define IsShortRep(p) ((p)->backPrev == 0)

static uint32_t GetRepLen1Price(struct CLzmaEnc *p, uint32_t state, uint32_t posState)
{
  return
    GET_PRICE_0(p->isRepG0[state]) +
    GET_PRICE_0(p->isRep0Long[state][posState]);
}

static uint32_t GetPureRepPrice(struct CLzmaEnc *p, uint32_t repIndex, uint32_t state, uint32_t posState)
{
  uint32_t price;
  if (repIndex == 0)
  {
    price = GET_PRICE_0(p->isRepG0[state]);
    price += GET_PRICE_1(p->isRep0Long[state][posState]);
  }
  else
  {
    price = GET_PRICE_1(p->isRepG0[state]);
    if (repIndex == 1)
      price += GET_PRICE_0(p->isRepG1[state]);
    else
    {
      price += GET_PRICE_1(p->isRepG1[state]);
      price += GET_PRICE(p->isRepG2[state], repIndex - 2);
    }
  }
  return price;
}

static uint32_t GetRepPrice(struct CLzmaEnc *p, uint32_t repIndex, uint32_t len, uint32_t state, uint32_t posState)
{
  return p->repLenEnc.prices[posState][len - LZMA_MATCH_LEN_MIN] +
    GetPureRepPrice(p, repIndex, state, posState);
}

static uint32_t Backward(struct CLzmaEnc *p, uint32_t *backRes, uint32_t cur)
{
  uint32_t posMem = p->opt[cur].posPrev;
  uint32_t backMem = p->opt[cur].backPrev;
  p->optimumEndIndex = cur;
  do
  {
    if (p->opt[cur].prev1IsChar)
    {
      MakeAsChar(&p->opt[posMem])
      p->opt[posMem].posPrev = posMem - 1;
      if (p->opt[cur].prev2)
      {
        p->opt[posMem - 1].prev1IsChar = false;
        p->opt[posMem - 1].posPrev = p->opt[cur].posPrev2;
        p->opt[posMem - 1].backPrev = p->opt[cur].backPrev2;
      }
    }
    {
      uint32_t posPrev = posMem;
      uint32_t backCur = backMem;

      backMem = p->opt[posPrev].backPrev;
      posMem = p->opt[posPrev].posPrev;

      p->opt[posPrev].backPrev = backCur;
      p->opt[posPrev].posPrev = cur;
      cur = posPrev;
    }
  }
  while (cur != 0);
  *backRes = p->opt[0].backPrev;
  p->optimumCurrentIndex  = p->opt[0].posPrev;
  return p->optimumCurrentIndex;
}

#define LIT_PROBS(pos, prevuint8_t) (p->litProbs + ((((pos) & p->lpMask) << p->lc) + ((prevuint8_t) >> (8 - p->lc))) * 0x300)

static uint32_t GetOptimum(struct CLzmaEnc *p, uint32_t position, uint32_t *backRes)
{
  uint32_t numAvail, mainLen, numPairs, repMaxIndex, i, posState, lenEnd, len, cur;
  uint32_t matchPrice, repMatchPrice, normalMatchPrice;
  uint32_t reps[LZMA_NUM_REPS], repLens[LZMA_NUM_REPS];
  uint32_t *matches;
  const uint8_t *data;
  uint8_t curuint8_t, matchuint8_t;
  if (p->optimumEndIndex != p->optimumCurrentIndex)
  {
    const struct COptimal *opt = &p->opt[p->optimumCurrentIndex];
    uint32_t lenRes = opt->posPrev - p->optimumCurrentIndex;
    *backRes = opt->backPrev;
    p->optimumCurrentIndex = opt->posPrev;
    return lenRes;
  }
  p->optimumCurrentIndex = p->optimumEndIndex = 0;

  if (p->additionalOffset == 0)
    mainLen = ReadMatchDistances(p, &numPairs);
  else
  {
    mainLen = p->longestMatchLength;
    numPairs = p->numPairs;
  }

  numAvail = p->numAvail;
  if (numAvail < 2)
  {
    *backRes = (uint32_t)(-1);
    return 1;
  }
  if (numAvail > LZMA_MATCH_LEN_MAX)
    numAvail = LZMA_MATCH_LEN_MAX;

  data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
  repMaxIndex = 0;
  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t lenTest;
    const uint8_t *data2;
    reps[i] = p->reps[i];
    data2 = data - (reps[i] + 1);
    if (data[0] != data2[0] || data[1] != data2[1])
    {
      repLens[i] = 0;
      continue;
    }
    for (lenTest = 2; lenTest < numAvail && data[lenTest] == data2[lenTest]; lenTest++);
    repLens[i] = lenTest;
    if (lenTest > repLens[repMaxIndex])
      repMaxIndex = i;
  }
  if (repLens[repMaxIndex] >= p->numFastuint8_ts)
  {
    uint32_t lenRes;
    *backRes = repMaxIndex;
    lenRes = repLens[repMaxIndex];
    MovePos(p, lenRes - 1);
    return lenRes;
  }

  matches = p->matches;
  if (mainLen >= p->numFastuint8_ts)
  {
    *backRes = matches[numPairs - 1] + LZMA_NUM_REPS;
    MovePos(p, mainLen - 1);
    return mainLen;
  }
  curuint8_t = *data;
  matchuint8_t = *(data - (reps[0] + 1));

  if (mainLen < 2 && curuint8_t != matchuint8_t && repLens[repMaxIndex] < 2)
  {
    *backRes = (uint32_t)-1;
    return 1;
  }

  p->opt[0].state = (CState)p->state;

  posState = (position & p->pbMask);

  {
    const CLzmaProb *probs = LIT_PROBS(position, *(data - 1));
    p->opt[1].price = GET_PRICE_0(p->isMatch[p->state][posState]) +
        (!IsCharState(p->state) ?
          LitEnc_GetPriceMatched(probs, curuint8_t, matchuint8_t, p->ProbPrices) :
          LitEnc_GetPrice(probs, curuint8_t, p->ProbPrices));
  }

  MakeAsChar(&p->opt[1]);

  matchPrice = GET_PRICE_1(p->isMatch[p->state][posState]);
  repMatchPrice = matchPrice + GET_PRICE_1(p->isRep[p->state]);

  if (matchuint8_t == curuint8_t)
  {
    uint32_t shortRepPrice = repMatchPrice + GetRepLen1Price(p, p->state, posState);
    if (shortRepPrice < p->opt[1].price)
    {
      p->opt[1].price = shortRepPrice;
      MakeAsShortRep(&p->opt[1]);
    }
  }
  lenEnd = ((mainLen >= repLens[repMaxIndex]) ? mainLen : repLens[repMaxIndex]);

  if (lenEnd < 2)
  {
    *backRes = p->opt[1].backPrev;
    return 1;
  }

  p->opt[1].posPrev = 0;
  for (i = 0; i < LZMA_NUM_REPS; i++)
    p->opt[0].backs[i] = reps[i];

  len = lenEnd;
  do
    p->opt[len--].price = kInfinityPrice;
  while (len >= 2);

  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t repLen = repLens[i];
    uint32_t price;
    if (repLen < 2)
      continue;
    price = repMatchPrice + GetPureRepPrice(p, i, p->state, posState);
    do
    {
      uint32_t curAndLenPrice = price + p->repLenEnc.prices[posState][repLen - 2];
      struct COptimal *opt = &p->opt[repLen];
      if (curAndLenPrice < opt->price)
      {
        opt->price = curAndLenPrice;
        opt->posPrev = 0;
        opt->backPrev = i;
        opt->prev1IsChar = false;
      }
    }
    while (--repLen >= 2);
  }

  normalMatchPrice = matchPrice + GET_PRICE_0(p->isRep[p->state]);

  len = ((repLens[0] >= 2) ? repLens[0] + 1 : 2);
  if (len <= mainLen)
  {
    uint32_t offs = 0;
    while (len > matches[offs])
      offs += 2;
    for (; ; len++)
    {
      struct COptimal *opt;
      uint32_t distance = matches[offs + 1];

      uint32_t curAndLenPrice = normalMatchPrice + p->lenEnc.prices[posState][len - LZMA_MATCH_LEN_MIN];
      uint32_t lenToPosState = GetLenToPosState(len);
      if (distance < kNumFullDistances)
        curAndLenPrice += p->distancesPrices[lenToPosState][distance];
      else
      {
        uint32_t slot;
        GetPosSlot2(distance, slot);
        curAndLenPrice += p->alignPrices[distance & kAlignMask] + p->posSlotPrices[lenToPosState][slot];
      }
      opt = &p->opt[len];
      if (curAndLenPrice < opt->price)
      {
        opt->price = curAndLenPrice;
        opt->posPrev = 0;
        opt->backPrev = distance + LZMA_NUM_REPS;
        opt->prev1IsChar = false;
      }
      if (len == matches[offs])
      {
        offs += 2;
        if (offs == numPairs)
          break;
      }
    }
  }

  cur = 0;

  for (;;)
  {
    uint32_t numAvailFull, newLen, posPrev, state, startLen;
    uint32_t curPrice, curAnd1Price;
    bool nextIsChar;
    struct COptimal *curOpt;
    struct COptimal *nextOpt;

    cur++;
    if (cur == lenEnd)
      return Backward(p, backRes, cur);

    newLen = ReadMatchDistances(p, &numPairs);
    if (newLen >= p->numFastuint8_ts)
    {
      p->numPairs = numPairs;
      p->longestMatchLength = newLen;
      return Backward(p, backRes, cur);
    }
    position++;
    curOpt = &p->opt[cur];
    posPrev = curOpt->posPrev;
    if (curOpt->prev1IsChar)
    {
      posPrev--;
      if (curOpt->prev2)
      {
        state = p->opt[curOpt->posPrev2].state;
        if (curOpt->backPrev2 < LZMA_NUM_REPS)
          state = kRepNextStates[state];
        else
          state = kMatchNextStates[state];
      }
      else
        state = p->opt[posPrev].state;
      state = kLiteralNextStates[state];
    }
    else
      state = p->opt[posPrev].state;
    if (posPrev == cur - 1)
    {
      if (IsShortRep(curOpt))
        state = kShortRepNextStates[state];
      else
        state = kLiteralNextStates[state];
    }
    else
    {
      uint32_t pos;
      const struct COptimal *prevOpt;
      if (curOpt->prev1IsChar && curOpt->prev2)
      {
        posPrev = curOpt->posPrev2;
        pos = curOpt->backPrev2;
        state = kRepNextStates[state];
      }
      else
      {
        pos = curOpt->backPrev;
        if (pos < LZMA_NUM_REPS)
          state = kRepNextStates[state];
        else
          state = kMatchNextStates[state];
      }
      prevOpt = &p->opt[posPrev];
      if (pos < LZMA_NUM_REPS)
      {
        reps[0] = prevOpt->backs[pos];
        for (i = 1; i <= pos; i++)
          reps[i] = prevOpt->backs[i - 1];
        for (; i < LZMA_NUM_REPS; i++)
          reps[i] = prevOpt->backs[i];
      }
      else
      {
        reps[0] = (pos - LZMA_NUM_REPS);
        for (i = 1; i < LZMA_NUM_REPS; i++)
          reps[i] = prevOpt->backs[i - 1];
      }
    }
    curOpt->state = (CState)state;

    curOpt->backs[0] = reps[0];
    curOpt->backs[1] = reps[1];
    curOpt->backs[2] = reps[2];
    curOpt->backs[3] = reps[3];

    curPrice = curOpt->price;
    nextIsChar = false;
    data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
    curuint8_t = *data;
    matchuint8_t = *(data - (reps[0] + 1));

    posState = (position & p->pbMask);

    curAnd1Price = curPrice + GET_PRICE_0(p->isMatch[state][posState]);
    {
      const CLzmaProb *probs = LIT_PROBS(position, *(data - 1));
      curAnd1Price +=
        (!IsCharState(state) ?
          LitEnc_GetPriceMatched(probs, curuint8_t, matchuint8_t, p->ProbPrices) :
          LitEnc_GetPrice(probs, curuint8_t, p->ProbPrices));
    }

    nextOpt = &p->opt[cur + 1];

    if (curAnd1Price < nextOpt->price)
    {
      nextOpt->price = curAnd1Price;
      nextOpt->posPrev = cur;
      MakeAsChar(nextOpt);
      nextIsChar = true;
    }

    matchPrice = curPrice + GET_PRICE_1(p->isMatch[state][posState]);
    repMatchPrice = matchPrice + GET_PRICE_1(p->isRep[state]);

    if (matchuint8_t == curuint8_t && !(nextOpt->posPrev < cur && nextOpt->backPrev == 0))
    {
      uint32_t shortRepPrice = repMatchPrice + GetRepLen1Price(p, state, posState);
      if (shortRepPrice <= nextOpt->price)
      {
        nextOpt->price = shortRepPrice;
        nextOpt->posPrev = cur;
        MakeAsShortRep(nextOpt);
        nextIsChar = true;
      }
    }
    numAvailFull = p->numAvail;
    {
      uint32_t temp = kNumOpts - 1 - cur;
      if (temp < numAvailFull)
        numAvailFull = temp;
    }

    if (numAvailFull < 2)
      continue;
    numAvail = (numAvailFull <= p->numFastuint8_ts ? numAvailFull : p->numFastuint8_ts);

    if (!nextIsChar && matchuint8_t != curuint8_t) /* speed optimization */
    {
      /* try Literal + rep0 */
      uint32_t temp;
      uint32_t lenTest2;
      const uint8_t *data2 = data - (reps[0] + 1);
      uint32_t limit = p->numFastuint8_ts + 1;
      if (limit > numAvailFull)
        limit = numAvailFull;

      for (temp = 1; temp < limit && data[temp] == data2[temp]; temp++);
      lenTest2 = temp - 1;
      if (lenTest2 >= 2)
      {
        uint32_t state2 = kLiteralNextStates[state];
        uint32_t posStateNext = (position + 1) & p->pbMask;
        uint32_t nextRepMatchPrice = curAnd1Price +
            GET_PRICE_1(p->isMatch[state2][posStateNext]) +
            GET_PRICE_1(p->isRep[state2]);
        /* for (; lenTest2 >= 2; lenTest2--) */
        {
          uint32_t curAndLenPrice;
          struct COptimal *opt;
          uint32_t offset = cur + 1 + lenTest2;
          while (lenEnd < offset)
            p->opt[++lenEnd].price = kInfinityPrice;
          curAndLenPrice = nextRepMatchPrice + GetRepPrice(p, 0, lenTest2, state2, posStateNext);
          opt = &p->opt[offset];
          if (curAndLenPrice < opt->price)
          {
            opt->price = curAndLenPrice;
            opt->posPrev = cur + 1;
            opt->backPrev = 0;
            opt->prev1IsChar = true;
            opt->prev2 = false;
          }
        }
      }
    }

    startLen = 2; /* speed optimization */
    {
    uint32_t repIndex;
    for (repIndex = 0; repIndex < LZMA_NUM_REPS; repIndex++)
    {
      uint32_t lenTest;
      uint32_t lenTestTemp;
      uint32_t price;
      const uint8_t *data2 = data - (reps[repIndex] + 1);
      if (data[0] != data2[0] || data[1] != data2[1])
        continue;
      for (lenTest = 2; lenTest < numAvail && data[lenTest] == data2[lenTest]; lenTest++);
      while (lenEnd < cur + lenTest)
        p->opt[++lenEnd].price = kInfinityPrice;
      lenTestTemp = lenTest;
      price = repMatchPrice + GetPureRepPrice(p, repIndex, state, posState);
      do
      {
        uint32_t curAndLenPrice = price + p->repLenEnc.prices[posState][lenTest - 2];
        struct COptimal *opt = &p->opt[cur + lenTest];
        if (curAndLenPrice < opt->price)
        {
          opt->price = curAndLenPrice;
          opt->posPrev = cur;
          opt->backPrev = repIndex;
          opt->prev1IsChar = false;
        }
      }
      while (--lenTest >= 2);
      lenTest = lenTestTemp;

      if (repIndex == 0)
        startLen = lenTest + 1;

      /* if (_maxMode) */
        {
          uint32_t lenTest2 = lenTest + 1;
          uint32_t limit = lenTest2 + p->numFastuint8_ts;
          uint32_t nextRepMatchPrice;
          if (limit > numAvailFull)
            limit = numAvailFull;
          for (; lenTest2 < limit && data[lenTest2] == data2[lenTest2]; lenTest2++);
          lenTest2 -= lenTest + 1;
          if (lenTest2 >= 2)
          {
            uint32_t state2 = kRepNextStates[state];
            uint32_t posStateNext = (position + lenTest) & p->pbMask;
            uint32_t curAndLenCharPrice =
                price + p->repLenEnc.prices[posState][lenTest - 2] +
                GET_PRICE_0(p->isMatch[state2][posStateNext]) +
                LitEnc_GetPriceMatched(LIT_PROBS(position + lenTest, data[lenTest - 1]),
                    data[lenTest], data2[lenTest], p->ProbPrices);
            state2 = kLiteralNextStates[state2];
            posStateNext = (position + lenTest + 1) & p->pbMask;
            nextRepMatchPrice = curAndLenCharPrice +
                GET_PRICE_1(p->isMatch[state2][posStateNext]) +
                GET_PRICE_1(p->isRep[state2]);

            /* for (; lenTest2 >= 2; lenTest2--) */
            {
              uint32_t curAndLenPrice;
              struct COptimal *opt;
              uint32_t offset = cur + lenTest + 1 + lenTest2;
              while (lenEnd < offset)
                p->opt[++lenEnd].price = kInfinityPrice;
              curAndLenPrice = nextRepMatchPrice + GetRepPrice(p, 0, lenTest2, state2, posStateNext);
              opt = &p->opt[offset];
              if (curAndLenPrice < opt->price)
              {
                opt->price = curAndLenPrice;
                opt->posPrev = cur + lenTest + 1;
                opt->backPrev = 0;
                opt->prev1IsChar = true;
                opt->prev2 = true;
                opt->posPrev2 = cur;
                opt->backPrev2 = repIndex;
              }
            }
          }
        }
    }
    }
    /* for (uint32_t lenTest = 2; lenTest <= newLen; lenTest++) */
    if (newLen > numAvail)
    {
      newLen = numAvail;
      for (numPairs = 0; newLen > matches[numPairs]; numPairs += 2);
      matches[numPairs] = newLen;
      numPairs += 2;
    }
    if (newLen >= startLen)
    {
      uint32_t offs, curBack, posSlot;
      uint32_t lenTest;

      normalMatchPrice = matchPrice + GET_PRICE_0(p->isRep[state]);

      while (lenEnd < cur + newLen)
        p->opt[++lenEnd].price = kInfinityPrice;

      offs = 0;
      while (startLen > matches[offs])
        offs += 2;
      curBack = matches[offs + 1];
      GetPosSlot2(curBack, posSlot);
      for (lenTest = /*2*/ startLen; ; lenTest++)
      {
        uint32_t curAndLenPrice = normalMatchPrice + p->lenEnc.prices[posState][lenTest - LZMA_MATCH_LEN_MIN];
        uint32_t lenToPosState = GetLenToPosState(lenTest);
        struct COptimal *opt;
        if (curBack < kNumFullDistances)
          curAndLenPrice += p->distancesPrices[lenToPosState][curBack];
        else
          curAndLenPrice += p->posSlotPrices[lenToPosState][posSlot] + p->alignPrices[curBack & kAlignMask];

        opt = &p->opt[cur + lenTest];
        if (curAndLenPrice < opt->price)
        {
          opt->price = curAndLenPrice;
          opt->posPrev = cur;
          opt->backPrev = curBack + LZMA_NUM_REPS;
          opt->prev1IsChar = false;
        }

        if (/*_maxMode && */lenTest == matches[offs])
        {
          /* Try Match + Literal + Rep0 */
          const uint8_t *data2 = data - (curBack + 1);
          uint32_t lenTest2 = lenTest + 1;
          uint32_t limit = lenTest2 + p->numFastuint8_ts;
          uint32_t nextRepMatchPrice;
          if (limit > numAvailFull)
            limit = numAvailFull;
          for (; lenTest2 < limit && data[lenTest2] == data2[lenTest2]; lenTest2++);
          lenTest2 -= lenTest + 1;
          if (lenTest2 >= 2)
          {
            uint32_t state2 = kMatchNextStates[state];
            uint32_t posStateNext = (position + lenTest) & p->pbMask;
            uint32_t curAndLenCharPrice = curAndLenPrice +
                GET_PRICE_0(p->isMatch[state2][posStateNext]) +
                LitEnc_GetPriceMatched(LIT_PROBS(position + lenTest, data[lenTest - 1]),
                    data[lenTest], data2[lenTest], p->ProbPrices);
            state2 = kLiteralNextStates[state2];
            posStateNext = (posStateNext + 1) & p->pbMask;
            nextRepMatchPrice = curAndLenCharPrice +
                GET_PRICE_1(p->isMatch[state2][posStateNext]) +
                GET_PRICE_1(p->isRep[state2]);

            /* for (; lenTest2 >= 2; lenTest2--) */
            {
              uint32_t offset = cur + lenTest + 1 + lenTest2;
              while (lenEnd < offset)
                p->opt[++lenEnd].price = kInfinityPrice;
              curAndLenPrice = nextRepMatchPrice + GetRepPrice(p, 0, lenTest2, state2, posStateNext);
              opt = &p->opt[offset];
              if (curAndLenPrice < opt->price)
              {
                opt->price = curAndLenPrice;
                opt->posPrev = cur + lenTest + 1;
                opt->backPrev = 0;
                opt->prev1IsChar = true;
                opt->prev2 = true;
                opt->posPrev2 = cur;
                opt->backPrev2 = curBack + LZMA_NUM_REPS;
              }
            }
          }
          offs += 2;
          if (offs == numPairs)
            break;
          curBack = matches[offs + 1];
          if (curBack >= kNumFullDistances)
            GetPosSlot2(curBack, posSlot);
        }
      }
    }
  }
}

#define ChangePair(smallDist, bigDist) (((bigDist) >> 7) > (smallDist))

static uint32_t GetOptimumFast(struct CLzmaEnc *p, uint32_t *backRes)
{
  uint32_t numAvail, mainLen, mainDist, numPairs, repIndex, repLen, i;
  const uint8_t *data;
  const uint32_t *matches;

  if (p->additionalOffset == 0)
    mainLen = ReadMatchDistances(p, &numPairs);
  else
  {
    mainLen = p->longestMatchLength;
    numPairs = p->numPairs;
  }

  numAvail = p->numAvail;
  *backRes = (uint32_t)-1;
  if (numAvail < 2)
    return 1;
  if (numAvail > LZMA_MATCH_LEN_MAX)
    numAvail = LZMA_MATCH_LEN_MAX;
  data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;

  repLen = repIndex = 0;
  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t len;
    const uint8_t *data2 = data - (p->reps[i] + 1);
    if (data[0] != data2[0] || data[1] != data2[1])
      continue;
    for (len = 2; len < numAvail && data[len] == data2[len]; len++);
    if (len >= p->numFastuint8_ts)
    {
      *backRes = i;
      MovePos(p, len - 1);
      return len;
    }
    if (len > repLen)
    {
      repIndex = i;
      repLen = len;
    }
  }

  matches = p->matches;
  if (mainLen >= p->numFastuint8_ts)
  {
    *backRes = matches[numPairs - 1] + LZMA_NUM_REPS;
    MovePos(p, mainLen - 1);
    return mainLen;
  }

  mainDist = 0; /* for GCC */
  if (mainLen >= 2)
  {
    mainDist = matches[numPairs - 1];
    while (numPairs > 2 && mainLen == matches[numPairs - 4] + 1)
    {
      if (!ChangePair(matches[numPairs - 3], mainDist))
        break;
      numPairs -= 2;
      mainLen = matches[numPairs - 2];
      mainDist = matches[numPairs - 1];
    }
    if (mainLen == 2 && mainDist >= 0x80)
      mainLen = 1;
  }

  if (repLen >= 2 && (
        (repLen + 1 >= mainLen) ||
        (repLen + 2 >= mainLen && mainDist >= (1 << 9)) ||
        (repLen + 3 >= mainLen && mainDist >= (1 << 15))))
  {
    *backRes = repIndex;
    MovePos(p, repLen - 1);
    return repLen;
  }

  if (mainLen < 2 || numAvail <= 2)
    return 1;

  p->longestMatchLength = ReadMatchDistances(p, &p->numPairs);
  if (p->longestMatchLength >= 2)
  {
    uint32_t newDistance = matches[p->numPairs - 1];
    if ((p->longestMatchLength >= mainLen && newDistance < mainDist) ||
        (p->longestMatchLength == mainLen + 1 && !ChangePair(mainDist, newDistance)) ||
        (p->longestMatchLength > mainLen + 1) ||
        (p->longestMatchLength + 1 >= mainLen && mainLen >= 3 && ChangePair(newDistance, mainDist)))
      return 1;
  }

  data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t len, limit;
    const uint8_t *data2 = data - (p->reps[i] + 1);
    if (data[0] != data2[0] || data[1] != data2[1])
      continue;
    limit = mainLen - 1;
    for (len = 2; len < limit && data[len] == data2[len]; len++);
    if (len >= limit)
      return 1;
  }
  *backRes = mainDist + LZMA_NUM_REPS;
  MovePos(p, mainLen - 2);
  return mainLen;
}

static void WriteEndMarker(struct CLzmaEnc *p, uint32_t posState)
{
  uint32_t len;
  RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][posState], 1);
  RangeEnc_EncodeBit(&p->rc, &p->isRep[p->state], 0);
  p->state = kMatchNextStates[p->state];
  len = LZMA_MATCH_LEN_MIN;
  LenEnc_Encode2(&p->lenEnc, &p->rc, len - LZMA_MATCH_LEN_MIN, posState, !p->fastMode, p->ProbPrices);
  RcTree_Encode(&p->rc, p->posSlotEncoder[GetLenToPosState(len)], kNumPosSlotBits, (1 << kNumPosSlotBits) - 1);
  RangeEnc_EncodeDirectBits(&p->rc, (((uint32_t)1 << 30) - 1) >> kNumAlignBits, 30 - kNumAlignBits);
  RcTree_ReverseEncode(&p->rc, p->posAlignEncoder, kNumAlignBits, kAlignMask);
}

static SRes CheckErrors(struct CLzmaEnc *p)
{
  if (p->result != SZ_OK)
    return p->result;
  if (p->rc.res != SZ_OK)
    p->result = SZ_ERROR_WRITE;
  if (p->matchFinderBase.result != SZ_OK)
    p->result = SZ_ERROR_READ;
  if (p->result != SZ_OK)
    p->finished = true;
  return p->result;
}

static SRes Flush(struct CLzmaEnc *p, uint32_t nowPos)
{
  /* ReleaseMFStream(); */
  p->finished = true;
  if (p->writeEndMark)
    WriteEndMarker(p, nowPos & p->pbMask);
  RangeEnc_FlushData(&p->rc);
  RangeEnc_FlushStream(&p->rc);
  return CheckErrors(p);
}

static void FillAlignPrices(struct CLzmaEnc *p)
{
  uint32_t i;
  for (i = 0; i < kAlignTableSize; i++)
    p->alignPrices[i] = RcTree_ReverseGetPrice(p->posAlignEncoder, kNumAlignBits, i, p->ProbPrices);
  p->alignPriceCount = 0;
}

static void FillDistancesPrices(struct CLzmaEnc *p)
{
  uint32_t tempPrices[kNumFullDistances];
  uint32_t i, lenToPosState;
  for (i = kStartPosModelIndex; i < kNumFullDistances; i++)
  {
    uint32_t posSlot = GetPosSlot1(i);
    uint32_t footerBits = ((posSlot >> 1) - 1);
    uint32_t base = ((2 | (posSlot & 1)) << footerBits);
    tempPrices[i] = RcTree_ReverseGetPrice(p->posEncoders + base - posSlot - 1, footerBits, i - base, p->ProbPrices);
  }

  for (lenToPosState = 0; lenToPosState < kNumLenToPosStates; lenToPosState++)
  {
    uint32_t posSlot;
    const CLzmaProb *encoder = p->posSlotEncoder[lenToPosState];
    uint32_t *posSlotPrices = p->posSlotPrices[lenToPosState];
    for (posSlot = 0; posSlot < p->distTableSize; posSlot++)
      posSlotPrices[posSlot] = RcTree_GetPrice(encoder, kNumPosSlotBits, posSlot, p->ProbPrices);
    for (posSlot = kEndPosModelIndex; posSlot < p->distTableSize; posSlot++)
      posSlotPrices[posSlot] += ((((posSlot >> 1) - 1) - kNumAlignBits) << kNumBitPriceShiftBits);

    {
      uint32_t *distancesPrices = p->distancesPrices[lenToPosState];
      for (i = 0; i < kStartPosModelIndex; i++)
        distancesPrices[i] = posSlotPrices[i];
      for (; i < kNumFullDistances; i++)
        distancesPrices[i] = posSlotPrices[GetPosSlot1(i)] + tempPrices[i];
    }
  }
  p->matchPriceCount = 0;
}

static void LzmaEnc_Construct(struct CLzmaEnc *p)
{
  RangeEnc_Construct(&p->rc);
  MatchFinder_Construct(&p->matchFinderBase);

  {
    struct CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    LzmaEnc_SetProps(p, &props);
  }

  #ifndef LZMA_LOG_BSR
  LzmaEnc_FastPosInit(p->g_FastPos);
  #endif

  LzmaEnc_InitPriceTables(p->ProbPrices);
  p->litProbs = 0;
  p->saveState.litProbs = 0;
}

CLzmaEncHandle LzmaEnc_Create(struct ISzAlloc *alloc)
{
  void *p;
  p = alloc->Alloc(alloc, sizeof(struct CLzmaEnc));
  if (p != 0)
    LzmaEnc_Construct((struct CLzmaEnc *)p);
  return p;
}

static void LzmaEnc_FreeLits(struct CLzmaEnc *p, struct ISzAlloc *alloc)
{
  alloc->Free(alloc, p->litProbs);
  alloc->Free(alloc, p->saveState.litProbs);
  p->litProbs = 0;
  p->saveState.litProbs = 0;
}

static void LzmaEnc_Destruct(struct CLzmaEnc *p, struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  MatchFinder_Free(&p->matchFinderBase, allocBig);
  LzmaEnc_FreeLits(p, alloc);
  RangeEnc_Free(&p->rc, alloc);
}

void LzmaEnc_Destroy(CLzmaEncHandle p, struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  LzmaEnc_Destruct((struct CLzmaEnc *)p, alloc, allocBig);
  alloc->Free(alloc, p);
}

static SRes LzmaEnc_CodeOneBlock(struct CLzmaEnc *p, bool useLimits, uint32_t maxPackSize, uint32_t maxUnpackSize)
{
  uint32_t nowPos32, startPos32;
  if (p->needInit)
  {
    p->matchFinder.Init(p->matchFinderObj);
    p->needInit = 0;
  }

  if (p->finished)
    return p->result;
  RINOK(CheckErrors(p));

  nowPos32 = (uint32_t)p->nowPos64;
  startPos32 = nowPos32;

  if (p->nowPos64 == 0)
  {
    uint32_t numPairs;
    uint8_t curuint8_t;
    if (p->matchFinder.GetNumAvailableBytes(p->matchFinderObj) == 0)
      return Flush(p, nowPos32);
    ReadMatchDistances(p, &numPairs);
    RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][0], 0);
    p->state = kLiteralNextStates[p->state];
    curuint8_t = p->matchFinder.GetIndexByte(p->matchFinderObj, 0 - p->additionalOffset);
    LitEnc_Encode(&p->rc, p->litProbs, curuint8_t);
    p->additionalOffset--;
    nowPos32++;
  }

  if (p->matchFinder.GetNumAvailableBytes(p->matchFinderObj) != 0)
  for (;;)
  {
    uint32_t pos, len, posState;

    if (p->fastMode)
      len = GetOptimumFast(p, &pos);
    else
      len = GetOptimum(p, nowPos32, &pos);

    posState = nowPos32 & p->pbMask;
    if (len == 1 && pos == (uint32_t)-1)
    {
      uint8_t curuint8_t;
      CLzmaProb *probs;
      const uint8_t *data;

      RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][posState], 0);
      data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - p->additionalOffset;
      curuint8_t = *data;
      probs = LIT_PROBS(nowPos32, *(data - 1));
      if (IsCharState(p->state))
        LitEnc_Encode(&p->rc, probs, curuint8_t);
      else
        LitEnc_EncodeMatched(&p->rc, probs, curuint8_t, *(data - p->reps[0] - 1));
      p->state = kLiteralNextStates[p->state];
    }
    else
    {
      RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][posState], 1);
      if (pos < LZMA_NUM_REPS)
      {
        RangeEnc_EncodeBit(&p->rc, &p->isRep[p->state], 1);
        if (pos == 0)
        {
          RangeEnc_EncodeBit(&p->rc, &p->isRepG0[p->state], 0);
          RangeEnc_EncodeBit(&p->rc, &p->isRep0Long[p->state][posState], ((len == 1) ? 0 : 1));
        }
        else
        {
          uint32_t distance = p->reps[pos];
          RangeEnc_EncodeBit(&p->rc, &p->isRepG0[p->state], 1);
          if (pos == 1)
            RangeEnc_EncodeBit(&p->rc, &p->isRepG1[p->state], 0);
          else
          {
            RangeEnc_EncodeBit(&p->rc, &p->isRepG1[p->state], 1);
            RangeEnc_EncodeBit(&p->rc, &p->isRepG2[p->state], pos - 2);
            if (pos == 3)
              p->reps[3] = p->reps[2];
            p->reps[2] = p->reps[1];
          }
          p->reps[1] = p->reps[0];
          p->reps[0] = distance;
        }
        if (len == 1)
          p->state = kShortRepNextStates[p->state];
        else
        {
          LenEnc_Encode2(&p->repLenEnc, &p->rc, len - LZMA_MATCH_LEN_MIN, posState, !p->fastMode, p->ProbPrices);
          p->state = kRepNextStates[p->state];
        }
      }
      else
      {
        uint32_t posSlot;
        RangeEnc_EncodeBit(&p->rc, &p->isRep[p->state], 0);
        p->state = kMatchNextStates[p->state];
        LenEnc_Encode2(&p->lenEnc, &p->rc, len - LZMA_MATCH_LEN_MIN, posState, !p->fastMode, p->ProbPrices);
        pos -= LZMA_NUM_REPS;
        GetPosSlot(pos, posSlot);
        RcTree_Encode(&p->rc, p->posSlotEncoder[GetLenToPosState(len)], kNumPosSlotBits, posSlot);

        if (posSlot >= kStartPosModelIndex)
        {
          uint32_t footerBits = ((posSlot >> 1) - 1);
          uint32_t base = ((2 | (posSlot & 1)) << footerBits);
          uint32_t posReduced = pos - base;

          if (posSlot < kEndPosModelIndex)
            RcTree_ReverseEncode(&p->rc, p->posEncoders + base - posSlot - 1, footerBits, posReduced);
          else
          {
            RangeEnc_EncodeDirectBits(&p->rc, posReduced >> kNumAlignBits, footerBits - kNumAlignBits);
            RcTree_ReverseEncode(&p->rc, p->posAlignEncoder, kNumAlignBits, posReduced & kAlignMask);
            p->alignPriceCount++;
          }
        }
        p->reps[3] = p->reps[2];
        p->reps[2] = p->reps[1];
        p->reps[1] = p->reps[0];
        p->reps[0] = pos;
        p->matchPriceCount++;
      }
    }
    p->additionalOffset -= len;
    nowPos32 += len;
    if (p->additionalOffset == 0)
    {
      uint32_t processed;
      if (!p->fastMode)
      {
        if (p->matchPriceCount >= (1 << 7))
          FillDistancesPrices(p);
        if (p->alignPriceCount >= kAlignTableSize)
          FillAlignPrices(p);
      }
      if (p->matchFinder.GetNumAvailableBytes(p->matchFinderObj) == 0)
        break;
      processed = nowPos32 - startPos32;
      if (useLimits)
      {
        if (processed + kNumOpts + 300 >= maxUnpackSize ||
            RangeEnc_GetProcessed(&p->rc) + kNumOpts * 2 >= maxPackSize)
          break;
      }
      else if (processed >= (1 << 15))
      {
        p->nowPos64 += nowPos32 - startPos32;
        return CheckErrors(p);
      }
    }
  }
  p->nowPos64 += nowPos32 - startPos32;
  return Flush(p, nowPos32);
}

#define kBigHashDicLimit ((uint32_t)1 << 24)

static SRes LzmaEnc_Alloc(struct CLzmaEnc *p, uint32_t keepWindowSize, struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  uint32_t beforeSize = kNumOpts;
  if (!RangeEnc_Alloc(&p->rc, alloc))
    return SZ_ERROR_MEM;

  {
    unsigned lclp = p->lc + p->lp;
    if (p->litProbs == 0 || p->saveState.litProbs == 0 || p->lclp != lclp)
    {
      LzmaEnc_FreeLits(p, alloc);
      p->litProbs = (CLzmaProb *)alloc->Alloc(alloc, (0x300 << lclp) * sizeof(CLzmaProb));
      p->saveState.litProbs = (CLzmaProb *)alloc->Alloc(alloc, (0x300 << lclp) * sizeof(CLzmaProb));
      if (p->litProbs == 0 || p->saveState.litProbs == 0)
      {
        LzmaEnc_FreeLits(p, alloc);
        return SZ_ERROR_MEM;
      }
      p->lclp = lclp;
    }
  }

  p->matchFinderBase.bigHash = (p->dictSize > kBigHashDicLimit);

  if (beforeSize + p->dictSize < keepWindowSize)
    beforeSize = keepWindowSize - p->dictSize;

  {
    if (!MatchFinder_Create(&p->matchFinderBase, p->dictSize, beforeSize, p->numFastuint8_ts, LZMA_MATCH_LEN_MAX, allocBig))
      return SZ_ERROR_MEM;
    p->matchFinderObj = &p->matchFinderBase;
    MatchFinder_CreateVTable(&p->matchFinderBase, &p->matchFinder);
  }
  return SZ_OK;
}

static void LzmaEnc_Init(struct CLzmaEnc *p)
{
  uint32_t i;
  p->state = 0;
  for (i = 0 ; i < LZMA_NUM_REPS; i++)
    p->reps[i] = 0;

  RangeEnc_Init(&p->rc);


  for (i = 0; i < kNumStates; i++)
  {
    uint32_t j;
    for (j = 0; j < LZMA_NUM_PB_STATES_MAX; j++)
    {
      p->isMatch[i][j] = kProbInitValue;
      p->isRep0Long[i][j] = kProbInitValue;
    }
    p->isRep[i] = kProbInitValue;
    p->isRepG0[i] = kProbInitValue;
    p->isRepG1[i] = kProbInitValue;
    p->isRepG2[i] = kProbInitValue;
  }

  {
    uint32_t num = 0x300 << (p->lp + p->lc);
    for (i = 0; i < num; i++)
      p->litProbs[i] = kProbInitValue;
  }

  {
    for (i = 0; i < kNumLenToPosStates; i++)
    {
      CLzmaProb *probs = p->posSlotEncoder[i];
      uint32_t j;
      for (j = 0; j < (1 << kNumPosSlotBits); j++)
        probs[j] = kProbInitValue;
    }
  }
  {
    for (i = 0; i < kNumFullDistances - kEndPosModelIndex; i++)
      p->posEncoders[i] = kProbInitValue;
  }

  LenEnc_Init(&p->lenEnc.p);
  LenEnc_Init(&p->repLenEnc.p);

  for (i = 0; i < (1 << kNumAlignBits); i++)
    p->posAlignEncoder[i] = kProbInitValue;

  p->optimumEndIndex = 0;
  p->optimumCurrentIndex = 0;
  p->additionalOffset = 0;

  p->pbMask = (1 << p->pb) - 1;
  p->lpMask = (1 << p->lp) - 1;
}

static void LzmaEnc_InitPrices(struct CLzmaEnc *p)
{
  if (!p->fastMode)
  {
    FillDistancesPrices(p);
    FillAlignPrices(p);
  }

  p->lenEnc.tableSize =
  p->repLenEnc.tableSize =
      p->numFastuint8_ts + 1 - LZMA_MATCH_LEN_MIN;
  LenPriceEnc_UpdateTables(&p->lenEnc, 1 << p->pb, p->ProbPrices);
  LenPriceEnc_UpdateTables(&p->repLenEnc, 1 << p->pb, p->ProbPrices);
}

static SRes LzmaEnc_AllocAndInit(struct CLzmaEnc *p, uint32_t keepWindowSize, struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  uint32_t i;
  for (i = 0; i < (uint32_t)kDicLogSizeMaxCompress; i++)
    if (p->dictSize <= ((uint32_t)1 << i))
      break;
  p->distTableSize = i * 2;

  p->finished = false;
  p->result = SZ_OK;
  RINOK(LzmaEnc_Alloc(p, keepWindowSize, alloc, allocBig));
  LzmaEnc_Init(p);
  LzmaEnc_InitPrices(p);
  p->nowPos64 = 0;
  return SZ_OK;
}

static SRes LzmaEnc_Prepare(CLzmaEncHandle pp, struct ISeqOutStream *outStream, struct ISeqInStream *inStream,
    struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  struct CLzmaEnc *p = (struct CLzmaEnc *)pp;
  p->matchFinderBase.stream = inStream;
  p->needInit = 1;
  p->rc.outStream = outStream;
  return LzmaEnc_AllocAndInit(p, 0, alloc, allocBig);
}

/*static SRes LzmaEnc_PrepareForLzma2(CLzmaEncHandle pp,
    ISeqInStream *inStream, uint32_t keepWindowSize,
    ISzAlloc *alloc, ISzAlloc *allocBig)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  p->matchFinderBase.stream = inStream;
  p->needInit = 1;
  return LzmaEnc_AllocAndInit(p, keepWindowSize, alloc, allocBig);
}*/

static void LzmaEnc_SetInputBuf(struct CLzmaEnc *p, const uint8_t *src, size_t srcLen)
{
  p->matchFinderBase.directInput = 1;
  p->matchFinderBase.bufferBase = (uint8_t *)src;
  p->matchFinderBase.directInputRem = srcLen;
}

static SRes LzmaEnc_MemPrepare(CLzmaEncHandle pp, const uint8_t *src, size_t srcLen,
    uint32_t keepWindowSize, struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  struct CLzmaEnc *p = (struct CLzmaEnc *)pp;
  LzmaEnc_SetInputBuf(p, src, srcLen);
  p->needInit = 1;

  return LzmaEnc_AllocAndInit(p, keepWindowSize, alloc, allocBig);
}

static void LzmaEnc_Finish(CLzmaEncHandle pp)
{
  (void)pp;
}

struct CSeqOutStreamBuf
{
  struct ISeqOutStream funcTable;
  uint8_t *data;
  size_t rem;
  bool overflow;
};

static size_t MyWrite(void *pp, const void *data, size_t size)
{
  struct CSeqOutStreamBuf *p = (struct CSeqOutStreamBuf *)pp;
  if (p->rem < size)
  {
    size = p->rem;
    p->overflow = true;
  }
  memcpy(p->data, data, size);
  p->rem -= size;
  p->data += size;
  return size;
}


/*static uint32_t LzmaEnc_GetNumAvailableBytes(CLzmaEncHandle pp)
{
  const CLzmaEnc *p = (CLzmaEnc *)pp;
  return p->matchFinder.GetNumAvailableBytes(p->matchFinderObj);
}*/

/*static const uint8_t *LzmaEnc_GetCurBuf(CLzmaEncHandle pp)
{
  const CLzmaEnc *p = (CLzmaEnc *)pp;
  return p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - p->additionalOffset;
}*/

/*static SRes LzmaEnc_CodeOneMemBlock(CLzmaEncHandle pp, bool reInit,
    uint8_t *dest, size_t *destLen, uint32_t desiredPackSize, uint32_t *unpackSize)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  uint64_t nowPos64;
  SRes res;
  CSeqOutStreamBuf outStream;

  outStream.funcTable.Write = MyWrite;
  outStream.data = dest;
  outStream.rem = *destLen;
  outStream.overflow = false;

  p->writeEndMark = false;
  p->finished = false;
  p->result = SZ_OK;

  if (reInit)
    LzmaEnc_Init(p);
  LzmaEnc_InitPrices(p);
  nowPos64 = p->nowPos64;
  RangeEnc_Init(&p->rc);
  p->rc.outStream = &outStream.funcTable;

  res = LzmaEnc_CodeOneBlock(p, true, desiredPackSize, *unpackSize);

  *unpackSize = (uint32_t)(p->nowPos64 - nowPos64);
  *destLen -= outStream.rem;
  if (outStream.overflow)
    return SZ_ERROR_OUTPUT_EOF;

  return res;
}*/

static SRes LzmaEnc_Encode2(struct CLzmaEnc *p, struct ICompressProgress *progress)
{
  SRes res = SZ_OK;

  for (;;)
  {
    res = LzmaEnc_CodeOneBlock(p, false, 0, 0);
    if (res != SZ_OK || p->finished != 0)
      break;
    if (progress != 0)
    {
      res = progress->Progress(progress, p->nowPos64, RangeEnc_GetProcessed(&p->rc));
      if (res != SZ_OK)
      {
        res = SZ_ERROR_PROGRESS;
        break;
      }
    }
  }
  LzmaEnc_Finish(p);
  return res;
}

SRes LzmaEnc_Encode(CLzmaEncHandle pp, struct ISeqOutStream *outStream, struct ISeqInStream *inStream, struct ICompressProgress *progress,
    struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  RINOK(LzmaEnc_Prepare(pp, outStream, inStream, alloc, allocBig));
  return LzmaEnc_Encode2((struct CLzmaEnc *)pp, progress);
}

SRes LzmaEnc_WriteProperties(CLzmaEncHandle pp, uint8_t *props, size_t *size)
{
  struct CLzmaEnc *p = (struct CLzmaEnc *)pp;
  int i;
  uint32_t dictSize = p->dictSize;
  if (*size < LZMA_PROPS_SIZE)
    return SZ_ERROR_PARAM;
  *size = LZMA_PROPS_SIZE;
  props[0] = (uint8_t)((p->pb * 5 + p->lp) * 9 + p->lc);

  for (i = 11; i <= 30; i++)
  {
    if (dictSize <= ((uint32_t)2 << i))
    {
      dictSize = (2 << i);
      break;
    }
    if (dictSize <= ((uint32_t)3 << i))
    {
      dictSize = (3 << i);
      break;
    }
  }

  for (i = 0; i < 4; i++)
    props[1 + i] = (uint8_t)(dictSize >> (8 * i));
  return SZ_OK;
}

SRes LzmaEnc_MemEncode(CLzmaEncHandle pp, uint8_t *dest, size_t *destLen, const uint8_t *src, size_t srcLen,
		       int writeEndMark, struct ICompressProgress *progress, struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  SRes res;
  struct CLzmaEnc *p = (struct CLzmaEnc *)pp;

  struct CSeqOutStreamBuf outStream;

  LzmaEnc_SetInputBuf(p, src, srcLen);

  outStream.funcTable.Write = MyWrite;
  outStream.data = dest;
  outStream.rem = *destLen;
  outStream.overflow = false;

  p->writeEndMark = writeEndMark;

  p->rc.outStream = &outStream.funcTable;
  res = LzmaEnc_MemPrepare(pp, src, srcLen, 0, alloc, allocBig);
  if (res == SZ_OK)
    res = LzmaEnc_Encode2(p, progress);

  *destLen -= outStream.rem;
  if (outStream.overflow)
    return SZ_ERROR_OUTPUT_EOF;
  return res;
}

SRes LzmaEncode(uint8_t *dest, size_t *destLen, const uint8_t *src, size_t srcLen,
    const struct CLzmaEncProps *props, uint8_t *propsEncoded, size_t *propsSize, int writeEndMark,
    struct ICompressProgress *progress, struct ISzAlloc *alloc, struct ISzAlloc *allocBig)
{
  struct CLzmaEnc *p = (struct CLzmaEnc *)LzmaEnc_Create(alloc);
  SRes res;
  if (p == 0)
    return SZ_ERROR_MEM;

  res = LzmaEnc_SetProps(p, props);
  if (res == SZ_OK)
  {
    res = LzmaEnc_WriteProperties(p, propsEncoded, propsSize);
    if (res == SZ_OK)
      res = LzmaEnc_MemEncode(p, dest, destLen, src, srcLen,
          writeEndMark, progress, alloc, allocBig);
  }

  LzmaEnc_Destroy(p, alloc, allocBig);
  return res;
}
