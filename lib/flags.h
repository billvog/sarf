#ifndef SARF_FLAGS
#define SARF_FLAGS

#include <inttypes.h>

typedef uint32_t sarf_flags_t;

/* archive flags */
#define LSARF_CREATE			1u
#define LSARF_TRUNC				2u
#define LSARF_RDONLY			3u
#define LSARF_RECURSIVE 		4u

#endif