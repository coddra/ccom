#include "../diagnostics.h"

cDgnDscr EMISSINGTOKEN = { "missingToken", "'$' not found", LVLERROR };
cDgnDscr EMISSINGSYNTAX = { "missingSyntax", "missing $", LVLERROR };
cDgnDscr EUNRECESCSEQ = { "unrecEscSeq", "unrecognized escape sequence", LVLERROR };

void cAddDgnLoc(void* context, cDgnDscr* kind, cLoc loc, char* prm) {
    cDgn d = { kind, loc, prm };
    cDgnListAdd(&((cContext*)context)->dgns, d);
}
void cAddDgnEmpty(void* context, cDgnDscr* kind) {
    cAddDgnLoc(((cContext*)context), kind, ((cContext*)context)->loc, NULL);
}
void cAddDgnEmptyLoc(void* context, cDgnDscr* kind, cLoc loc) {
    cAddDgnLoc(((cContext*)context), kind, loc, NULL);
}
void cAddDgn(void* context, cDgnDscr* kind, char* prm) {
    cAddDgnLoc(((cContext*)context), kind, ((cContext*)context)->loc, prm);
}
string cDgnToString(void* context, cDgn dgn, bool standard) {
    string res = {0};
    if (standard) {
        if (dgn.loc.file < ((cContext*)context)->inputs.len) {
            stringAddRange(&res, ((cContext*)context)->inputs.items[dgn.loc.file]);
            stringAdd(&res, ':');
            concat(&res, utos(dgn.loc.ln));
            stringAdd(&res, ':');
            concat(&res, utos(dgn.loc.cl + 1));
            catCptr(&res, ": ");
        }
        catCptr(&res, dgn.kind->lvl == LVLERROR ? "error:" : dgn.kind->lvl == LVLWARNING ? "warning:" : "message:");
        catCptr(&res, dgn.kind->msg);
        if (dgn.prm != NULL)
            stringReplaceAll(&res, sstr("$"), sstr(dgn.prm));
    } else {
        res = str(dgn.kind->lvl == LVLERROR ? "error: " : dgn.kind->lvl == LVLWARNING ? "warning: " : "message: ");
        catCptr(&res, dgn.kind->msg);
        if (dgn.prm != NULL)
            stringReplaceAll(&res, sstr("$"), sstr(dgn.prm));
        catCptr(&res, "\n");
        if (dgn.loc.file < ((cContext*)context)->inputs.len) {
            catCptr(&res, "\tin file '");
            stringAddRange(&res, ((cContext*)context)->inputs.items[dgn.loc.file]);
            catCptr(&res, "', at line: ");
            concat(&res, utos(dgn.loc.ln));
            catCptr(&res, ", column: ");
            concat(&res, utos(dgn.loc.cl));
        }
        if ((dgn.kind->lvl & (LVLWARNING | LVLMESSAGE)) != 0) {
            catCptr(&res, " (-");
            catCptr(&res, dgn.kind->id);
            catCptr(&res, ")");
        }
    }
    return res;
}
CLVL cHighestLVL(void* context, bool (*includeDgn)(void* context, cDgn dgn, u highestLvl)) {
    CLVL res = LVLMESSAGE;
    for (u i = 0; i < ((cContext*)context)->dgns.len && res != LVLERROR; i++)
        if (((cContext*)context)->dgns.items[i].kind->lvl > res && includeDgn(context, ((cContext*)context)->dgns.items[i], LVLERROR + 1))
            res = ((cContext*)context)->dgns.items[i].kind->lvl;
    return res;
}
void cPrintDgns(void* context, bool (*includeDgn)(void* context, cDgn dgn, u highestLvl), bool standard) {
    CLVL h = cHighestLVL(((cContext*)context), includeDgn);
    for (u i = 0; i < ((cContext*)context)->dgns.len; i++)
        if (includeDgn(context, ((cContext*)context)->dgns.items[i], h))
            puts(cptr(cDgnToString(context, ((cContext*)context)->dgns.items[i], standard)));
}
