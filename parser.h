#ifndef CCOM_PARSER_H
#define CCOM_PARSER_H

#include "objects.h"

extern set(char)* whitespace;
extern set(char)* letters;
extern set(char)* digits;
extern set(char)* hexDigits;
extern set(char)* octDigits;
extern set(char)* specialChars;
extern set(char)* escChars;
extern set(char)* newLine;
extern set(char)* stringLiteral;
extern set(char)* charLiteral;
extern set(char)* notWhitespace;

void init(CPARSER);

bool cNext(void* context);
static inline bool cEof(void* context) {
    return ((cContext*)context)->text.len >= ((cContext*)context)->loc.cr;
}
static inline bool cNeof(void* context) {
    return ((cContext*)context)->loc.cr < ((cContext*)context)->text.len;
}
static inline bool cParseC(void* context, char ch) {
    return cNeof(((cContext*)context)) && ((cContext*)context)->text.items[((cContext*)context)->loc.cr] == ch && cNext(((cContext*)context));
}
static inline bool cParseCS(void* context, set(char)* cs) {
    return cNeof(((cContext*)context)) && charSetContains(cs, ((cContext*)context)->text.items[((cContext*)context)->loc.cr]) && cNext(((cContext*)context));
}
static inline bool cParseCSR(void* context, set(char)* cs, char* res) {
    if (res)
        *res = ((cContext*)context)->text.items[((cContext*)context)->loc.cr];
    return cNeof(((cContext*)context)) && charSetContains(cs, ((cContext*)context)->text.items[((cContext*)context)->loc.cr]) && cNext(((cContext*)context));
}
static inline bool cParseAllCS(void* context, set(char)* cs) {
    bool res = false;
    while (cNeof(((cContext*)context)) && charSetContains(cs, ((cContext*)context)->text.items[((cContext*)context)->loc.cr]) && cNext(((cContext*)context)))
        res = true;
    return res;
}
static inline bool cParseAllNot(void* context, set(char)* cs) {
    bool res = false;
    while (cNeof(((cContext*)context)) && !charSetContains(cs, ((cContext*)context)->text.items[((cContext*)context)->loc.cr]) && cNext(((cContext*)context)))
        res = true;
    return res;
}

#endif // CCOM_PARSER_H
