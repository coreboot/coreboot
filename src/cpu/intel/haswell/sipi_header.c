#include <rmodule.h>


extern void *ap_start;

DEFINE_RMODULE_HEADER(sipi_vector_header, ap_start, RMODULE_TYPE_SIPI_VECTOR);
