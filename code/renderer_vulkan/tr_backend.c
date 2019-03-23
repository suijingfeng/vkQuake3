#include "../renderercommon/ref_import.h"
#include "tr_backend.h"

backEndState_t	backEnd;


void R_ClearBackendState(void)
{
    ri.Printf(PRINT_ALL, " backend state cleared. \n");
	// clear all our internal state
	memset( &backEnd, 0, sizeof( backEnd ) );
}
