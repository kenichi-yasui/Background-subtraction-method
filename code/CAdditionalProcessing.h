#ifndef _CADDITIONAL_PROCESSING_H_
#define _CADDITIONAL_PROCESSING_H_
#include <stdio.h>
#include "CDetectBase.h"

class CAdditionalProcessing
{
public:
	virtual int Postprocessing(CDetectBase *pDetect,unsigned char *pPixel);
	virtual int Preprocessing(CDetectBase *pDetect,unsigned char *pPixel);

};

#endif