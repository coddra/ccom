#ifndef CCOM_DIAGNOSTICS_H
#define CCOM_DIAGNOSTICS_H

#include "objects.h"

extern cDgnDscr EMISSINGTOKEN;
extern cDgnDscr EMISSINGSYNTAX;
extern cDgnDscr EUNRECESCSEQ;

void cAddDgnLoc(void* context, cDgnDscr* kind, cLoc loc, char* prm);
void cAddDgnEmpty(void* context, cDgnDscr* kind);
void cAddDgnEmptyLoc(void* context, cDgnDscr* kind, cLoc loc);
void cAddDgn(void* context, cDgnDscr* kind, char* prm);
string cDgnToString(void* context, cDgn dgn, bool standard);
bool includeDgn(cContext* context, cDgn dgn, u highestLvl);
CLVL cHighestLVL(void* context);
void cPrintDgns(void* context, bool standard);

#endif // CCOM_DIAGNOSTICS_H
