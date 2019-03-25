#include "tr_local.h"
#include "../renderercommon/tr_types.h"
// extern glconfig_t	glConfig;		// outside of TR since it shouldn't be cleared during ref re-init

glconfig_t glConfig;

void R_glConfigOut(glconfig_t* pOut)
{
	*pOut = glConfig;
}
