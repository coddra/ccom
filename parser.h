#ifndef CCOM_PARSER_H
#define CCOM_PARSER_H

#include "objects.h"

extern set(char)* whitespace;
extern set(char)* letters;
extern set(char)* digits;
extern set(char)* hexDigits;
extern set(char)* octDigits;
extern set(char)* escChars;
extern set(char)* newLine;
extern set(char)* stringLiteral;
extern set(char)* charLiteral;
extern set(char)* notWhitespace;

void init(CPARSER);

bool cNext(void* context);
bool cPrev(void* context);
bool cParseCptr(void* context, char* s);
bool cParseStr(void* context, string s);

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

bool cParseHex(void* context, u64* res, u8 digits);
bool cParseES(void* context, char* res);
bool cParseIL(void* context, i64* res);
bool cParseUL(void* context, u64* res);
bool cParseDL(void* context, d* res);
bool cParseSL(void* context, string* res);

static inline bool cParseCC(void* context, char* res, bool str) {
    return cParseES(context, res) || cParseCSR(context, str ? stringLiteral : charLiteral, res);
}
static inline bool cParseCL(void* context, char* res) {
    return cParseC(context, '\'') && cParseCC(context, res, false) && cParseC(context, '\'');
}

#endif // CCOM_PARSER_H
