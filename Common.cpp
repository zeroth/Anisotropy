#include "Common.h"

namespace zeroth {


int getByteorder()
{
    union {
        long l;
        char c[4];
    } test;
    test.l = 1;
    if (test.c[3] && !test.c[2] && !test.c[1] && !test.c[0])
        return TIFF_BI;

    if (!test.c[3] && !test.c[2] && !test.c[1] && test.c[0])
        return TIFF_LI;

    return TIFF_ORDER_UNKNOWN;
}

}
