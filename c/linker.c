#include "../linker.h"

string cCodeFrom(void* context, cLoc loc) {
    return stringGetRange(((cContext*)context)->text, loc.cr, ((cContext*)context)->loc.cr - loc.cr);
}
