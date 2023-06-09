/* aliType - some definitions for type of alignment. */

/* Copyright (C) 2011 The Regents of the University of California
 * See kent/LICENSE or http://genome.ucsc.edu/license/ for licensing information. */
#include "common.h"
#include "aliType.h"


char *gfTypeName(enum gfType type)
/* Return string representing type. */
{
if (type == gftDna) return "DNA";
if (type == gftRna) return "RNA";
if (type == gftProt) return "protein";
if (type == gftDnaX) return "DNAX";
if (type == gftRnaX) return "RNAX";
internalErr();
return NULL;
}

enum gfType gfTypeFromName(char *name)
/* Return type from string. */
{
if (sameWord(name, "DNA")) return gftDna;
if (sameWord(name, "RNA")) return gftRna;
if (sameWord(name, "protein")) return gftProt;
if (sameWord(name, "prot")) return gftProt;
if (sameWord(name, "DNAX")) return gftDnaX;
if (sameWord(name, "RNAX")) return gftRnaX;
errAbort("Unknown sequence type '%s'", name);
return 0;
}
