#include "../preprocessor.h"
#include "../objects.h"
#include "../mcx/string.h"

void normalize(void* context) {
    stringReplaceAll(&((cContext*)context)->text, sstr("\r\n"), sstr("\n"));
    stringReplaceAll(&((cContext*)context)->text, sstr("\r"), sstr("\n"));
    for (char i = '\0'; i < ' '; i++)
        if (i != '\n' && i != '\t')
            stringRemoveAll(&((cContext*)context)->text, i);
}
