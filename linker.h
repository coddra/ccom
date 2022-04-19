#ifndef CCOM_LINKER_H
#define CCOM_LINKER_H
#include "objects.h"

static inline string cCodeFrom(void* context, cLoc loc) {
    return stringGetRange(((cContext*)context)->text, loc.cr, ((cContext*)context)->loc.cr - loc.cr);
}

#endif // CCOM_LINKER_H
