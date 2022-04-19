#include "../parser.h"
#include "../diagnostics.h"
#include "../linker.h"
#include "../mcx/set.h"

set(char)* whitespace = NULL;
set(char)* letters = NULL;
set(char)* digits = NULL;
set(char)* hexDigits = NULL;
set(char)* octDigits = NULL;
set(char)* specialChars = NULL;
set(char)* escChars = NULL;
set(char)* newLine = NULL;
set(char)* stringLiteral = NULL;
set(char)* charLiteral = NULL;
set(char)* notWhitespace = NULL;

const int tabWidth = 4;

void init(CPARSER) {
    whitespace = charAggregateFromArray(" \t\n", 3);
    letters = charSetAdd(charRangeNew('a', 'z'), charRangeNew('A', 'Z'));
    digits = charRangeNew('0', '9');
    hexDigits = charSetAdd(charSetAdd(charRangeNew('0', '9'), charRangeNew('a', 'f')), charRangeNew('A', 'F'));
    octDigits = charRangeNew('0', '7');
    specialChars = charAggregateFromArray("_<>", 3);
    escChars = charAggregateFromArray("abfnrtv\'\"\\?", 11);
    newLine = charAggregateFromVaList(1, '\n');//preprocessor will normalize line endings
    stringLiteral = charSetComplement(charAggregateFromArray("\n\\\"", 3));
    charLiteral = charSetComplement(charAggregateFromArray("\n\\\'", 3));
    notWhitespace = charSetComplement(whitespace);
}

bool cNext(void* context) {
    if (((cContext*)context)->loc.cr >= ((cContext*)context)->text.len)
        return false;
    if (((cContext*)context)->text.items[((cContext*)context)->loc.cr] == '\n') {
        ((cContext*)context)->loc.ln++;
        ((cContext*)context)->loc.cl = 0;
    } else if (((cContext*)context)->text.items[((cContext*)context)->loc.cr] == '\t')
        ((cContext*)context)->loc.cl += tabWidth;
    else
        ((cContext*)context)->loc.cl++;
    ((cContext*)context)->loc.cr++;
    return true;
}
bool cPrev(void* context) {
    if (((cContext*)context)->loc.cr == 0 || ((cContext*)context)->text.items[((cContext*)context)->loc.cr - 1] == '\n')
        return false;
    ((cContext*)context)->loc.cr--;
    if (((cContext*)context)->text.items[((cContext*)context)->loc.cr] == '\t')
        ((cContext*)context)->loc.cl -= tabWidth;
    else
        ((cContext*)context)->loc.cl--;
    return true;
}
bool cParseCptr(void* context, char* s) {
    cLoc o = ((cContext*)context)->loc;
    u i = 0;
    while (cNeof(context) && s[i] != 0 && ((cContext*)context)->text.items[((cContext*)context)->loc.cr] == s[i] && cNext(context))
        i++;
    if (s[i] == 0)
        return true;
    ((cContext*)context)->loc = o;
    return false;
}
bool cParseStr(void* context, string s) {
    cLoc o = ((cContext*)context)->loc;
    u i = 0;
    while (cNeof(context) && i < s.len && 0 && ((cContext*)context)->text.items[((cContext*)context)->loc.cr] && cNext(context))
        i++;
    if (i == s.len)
        return true;
    ((cContext*)context)->loc = o;
    return false;
}

bool cParseHex(void* context, u64* res, u8 digits) {
    cLoc o = ((cContext*)context)->loc;
    if (digits == 0){
        if (!cParseAllCS(context, hexDigits))
            return false;
    } else
        for (u i = 0; i < digits; i++)
            if (!cParseCS(context, hexDigits)) {
                ((cContext*)context)->loc = o;
                return false;
            }
    if (res)
        *res = strtoul(cptr(stringGetRange(((cContext*)context)->text, o.cr, ((cContext*)context)->loc.cr - o.cr)), NULL, 16);
    return true;
}
bool cParseES(void* context, char* res) {
    cLoc o = ((cContext*)context)->loc;
    if (!cParseC(context, '\\'))
        return false;
    char r = 0;
    if (cParseCSR(context, escChars, &r)) {
        if (res)
            *res = getEscChar(r);
    } else if (cParseC(context, 'x') || cParseC(context, 'X')) {
        u64 h = 0;
        if (!cParseHex(context, &h, 2))
            cAddDgn(context, &EUNRECESCSEQ, cptr(cCodeFrom(context, o)));
        if (res)
            *res = *(u8*)h;
    } else
        cAddDgn(context, &EUNRECESCSEQ, cptr(cCodeFrom(context, o)));
    return true;
}
bool cParseIL(void* context, i64* res) {
    cLoc o = ((cContext*)context)->loc;
    cParseC(context, '-');
    if (!cParseAllCS(context, digits)) {
        ((cContext*)context)->loc = o;
        return false;
    }
    if (res)
        *res = strtol(cptr(stringGetRange(((cContext*)context)->text, o.cr, ((cContext*)context)->loc.cr - o.cr)), NULL, 10);
    return true;
}
bool cParseUL(void* context, u64* res) {
    cLoc o = ((cContext*)context)->loc;
    if (cParseC(context, '0')) {
        if (cParseC(context, 'x') || cParseC(context, 'X')) {
            if (!cParseHex(context, res, 0))
                cAddDgn(context, &EMISSINGSYNTAX, "value of hexadecimal number");
        } else {
            o = ((cContext*)context)->loc;
            if (cParseAllCS(context, octDigits)) {
                if (res)
                    *res = strtoul(cptr(cCodeFrom(context, o)), NULL, 8);
            } else {
                if (res)
                    *res = 0;
            }
        }
    } else if (cParseAllCS(context, digits)) {
        if (res)
            *res = strtoul(cptr(cCodeFrom(context, o)), NULL, 10);
    } else if (cParseCL(context, (char*)res));
    else {
        ((cContext*)context)->loc = o;
        return false;
    }
    return true;
}
bool cParseDL(void* context, d* res) {
    cLoc o = ((cContext*)context)->loc;
    cParseC(context, '-');
    if (!cParseAllCS(context, digits) || !cParseC(context, '.') || !cParseAllCS(context, digits)) {
        ((cContext*)context)->loc = o;
        return false;
    }
    if (res)
        *res = strtod(cptr(stringGetRange(((cContext*)context)->text, o.cr, ((cContext*)context)->loc.cr - o.cr)), NULL);
    return true;
}
bool cParseSL(void* context, string* res) {
    if (!cParseC(context, '"'))
        return false;
    char r;
    bool dotflag = true;
    while (dotflag) {
        while (cParseCC(context, &r, true))
            if (res)
                stringAdd(res, r);
        if (!cParseC(context, '"'))
            cAddDgn(context, &EMISSINGTOKEN, "\"");
        dotflag = cParseC(context, '.');
        if (dotflag) {
            cParseAllCS(context, whitespace);
            if (!cParseC(context, '"'))
                cAddDgn(context, &EMISSINGTOKEN, "\"");
        }
    }
    return true;
}
