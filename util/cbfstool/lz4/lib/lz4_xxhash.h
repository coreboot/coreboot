#include <commonlib/bsd/xxhash.h>

typedef struct xxh64_state XXH64_state_t;
#define XXH64_update xxh64_update
#define XXH64_reset  xxh64_reset
#define XXH64_digest xxh64_digest
#define XXH64 xxh64

typedef struct xxh32_state XXH32_state_t;
#define XXH32_update xxh32_update
#define XXH32_reset  xxh32_reset
#define XXH32_digest xxh32_digest
#define XXH32 xxh32
