#include "..\gameSetup\utility.h"
#include <nmmintrin.h>

#define STBI_MALLOC(sz)           new unsigned char[sz]
#define STBI_REALLOC(p,newsz)     Stbirealloc((p), (newsz))
#define STBI_FREE(p)              delete[] p


void *Stbirealloc(void *p, size_t newsz)
{
	unsigned char *newData = new unsigned char[newsz];
	memcpy(newData, p, newsz);
	delete[] p;
	return newData;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"