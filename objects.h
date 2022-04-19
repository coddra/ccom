#ifndef CCOM_OBJECTS_H
#define CCOM_OBJECTS_H
#include "mcx/mcx.h"
#include "mcx/list.h"
#include "mcx/string.h"

typedef enum {
    LVLMESSAGE,
    LVLWARNING,
    LVLERROR,
} CLVL;//level

typedef struct cLoc_s {
    u cr;
    u ln;
    u cl;
    u file;
} cLoc;//location

typedef struct cDgnDscr_s {
    char* id;
    char* msg;
    CLVL  lvl;
} cDgnDscr;//diagnostic descriptor
typedef struct cDgn_s {
    cDgnDscr* kind;
    cLoc      loc;
    char*     prm;
} cDgn;//diagnostic

typedef struct cFile_s {
    string  name;
    list(u) uses;
} cFile;

listDeclare(cDgn);
listDeclare(cFile);
#define CCONTEXTFIELDS                     \
    string       text;                     \
    list(cDgn)   dgns;                     \
    list(cFile)  inputs;                   \
    cLoc         loc
typedef struct cContext_s {
    CCONTEXTFIELDS;
} cContext;

#endif // CCOM_OBJECTS_H
