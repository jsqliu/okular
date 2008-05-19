//========================================================================
//
// SplashFTFontFile.cc
//
//========================================================================

#include <aconf.h>

#if HAVE_FREETYPE_FREETYPE_H || HAVE_FREETYPE_H

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "SplashFTFontEngine.h"
#include "SplashFTFont.h"
#include "SplashFTFontFile.h"
#include "GString.h"

//------------------------------------------------------------------------
// SplashFTFontFile
//------------------------------------------------------------------------

SplashFontFile *SplashFTFontFile::loadType1Font(SplashFTFontEngine *engineA,
						SplashFontFileID *idA,
						SplashFontSrc *src,
						char **encA) {
  FT_Face faceA;
  Gushort *codeToGIDA;
  char *name;
  int i;

  if (src->isFile) {
    if (FT_New_Face(engineA->lib, src->fileName->getCString(), 0, &faceA))
      return NULL;
  } else {
    if (FT_New_Memory_Face(engineA->lib, (const FT_Byte *)src->buf, src->bufLen, 0, &faceA))
      return NULL;
  }
  codeToGIDA = (Gushort *)gmallocn(256, sizeof(int));
  for (i = 0; i < 256; ++i) {
    codeToGIDA[i] = 0;
    if ((name = encA[i])) {
      codeToGIDA[i] = (Gushort)FT_Get_Name_Index(faceA, name);
    }
  }

  return new SplashFTFontFile(engineA, idA, src,
			      faceA, codeToGIDA, 256, gFalse);
}

SplashFontFile *SplashFTFontFile::loadCIDFont(SplashFTFontEngine *engineA,
					      SplashFontFileID *idA,
					      SplashFontSrc *src,
					      Gushort *codeToGIDA,
					      int codeToGIDLenA) {
  FT_Face faceA;

  if (src->isFile) {
    if (FT_New_Face(engineA->lib, src->fileName->getCString(), 0, &faceA))
      return NULL;
  } else {
    if (FT_New_Memory_Face(engineA->lib, (const FT_Byte *)src->buf, src->bufLen, 0, &faceA))
      return NULL;
  }

  return new SplashFTFontFile(engineA, idA, src,
			      faceA, codeToGIDA, codeToGIDLenA, gFalse);
}

SplashFontFile *SplashFTFontFile::loadTrueTypeFont(SplashFTFontEngine *engineA,
						   SplashFontFileID *idA,
						   SplashFontSrc *src,
						   Gushort *codeToGIDA,
						   int codeToGIDLenA,
						   int faceIndexA) {
  FT_Face faceA;

  if (src->isFile) {
    if (FT_New_Face(engineA->lib, src->fileName->getCString(), faceIndexA, &faceA))
      return NULL;
  } else {
    if (FT_New_Memory_Face(engineA->lib, (const FT_Byte *)src->buf, src->bufLen, faceIndexA, &faceA))
      return NULL;
  }

  return new SplashFTFontFile(engineA, idA, src,
			      faceA, codeToGIDA, codeToGIDLenA, gTrue);
}

SplashFTFontFile::SplashFTFontFile(SplashFTFontEngine *engineA,
				   SplashFontFileID *idA,
				   SplashFontSrc *src,
				   FT_Face faceA,
				   Gushort *codeToGIDA, int codeToGIDLenA,
				   GBool trueTypeA):
  SplashFontFile(idA, src)
{
  engine = engineA;
  face = faceA;
  codeToGID = codeToGIDA;
  codeToGIDLen = codeToGIDLenA;
  trueType = trueTypeA;
}

SplashFTFontFile::~SplashFTFontFile() {
  if (face) {
    FT_Done_Face(face);
  }
  if (codeToGID) {
    gfree(codeToGID);
  }
}

SplashFont *SplashFTFontFile::makeFont(SplashCoord *mat,
				       SplashCoord *textMat) {
  SplashFont *font;

  font = new SplashFTFont(this, mat, textMat);
  font->initCache();
  return font;
}

#endif // HAVE_FREETYPE_FREETYPE_H || HAVE_FREETYPE_H