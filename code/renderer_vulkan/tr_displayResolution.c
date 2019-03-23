#include "tr_local.h"
#include "tr_displayResolution.h"
#include "../renderercommon/ref_import.h"

cvar_t* r_mode;



static cvar_t* r_customwidth;
static cvar_t* r_customheight;
static cvar_t* r_customaspect;

typedef struct vidmode_s
{
    const char *description;
    int         width, height;
	float		pixelAspect;		// pixel width / height
} vidmode_t;


static const vidmode_t r_vidModes[] =
{
	{ "Mode  0: 320x240",		320,	240,	1 },
	{ "Mode  1: 400x300",		400,	300,	1 },
	{ "Mode  2: 512x384",		512,	384,	1 },
	{ "Mode  3: 640x480 (480p)",	640,	480,	1 },
	{ "Mode  4: 800x600",		800,	600,	1 },
	{ "Mode  5: 960x720",		960,	720,	1 },
	{ "Mode  6: 1024x768",		1024,	768,	1 },
	{ "Mode  7: 1152x864",		1152,	864,	1 },
	{ "Mode  8: 1280x1024",		1280,	1024,	1 },
	{ "Mode  9: 1600x1200",		1600,	1200,	1 },
	{ "Mode 10: 2048x1536",		2048,	1536,	1 },
	{ "Mode 11: 856x480",		856,	480,	1 }, // Q3 MODES END HERE AND EXTENDED MODES BEGIN
	{ "Mode 12: 1280x720 (720p)",	1280,	720,	1 },
	{ "Mode 13: 1280x768",		1280,	768,	1 },
	{ "Mode 14: 1280x800",		1280,	800,	1 },
	{ "Mode 15: 1280x960",		1280,	960,	1 },
	{ "Mode 16: 1360x768",		1360,	768,	1 },
	{ "Mode 17: 1366x768",		1366,	768,	1 }, // yes there are some out there on that extra 6
	{ "Mode 18: 1360x1024",		1360,	1024,	1 },
	{ "Mode 19: 1400x1050",		1400,	1050,	1 },
	{ "Mode 20: 1400x900",		1400,	900,	1 },
	{ "Mode 21: 1600x900",		1600,	900,	1 },
	{ "Mode 22: 1680x1050",		1680,	1050,	1 },
	{ "Mode 23: 1920x1080 (1080p)",	1920,	1080,	1 },
	{ "Mode 24: 1920x1200",		1920,	1200,	1 },
	{ "Mode 25: 1920x1440",		1920,	1440,	1 },
    { "Mode 26: 2560x1080",		2560,	1080,	1 },
    { "Mode 27: 2560x1600",		2560,	1600,	1 },
	{ "Mode 28: 3840x2160 (4K)",	3840,	2160,	1 }
};
static const int s_numVidModes = 29;


void R_DisplayResolutionList_f( void )
{
	int i;

	ri.Printf( PRINT_ALL, "\n" );
	for ( i = 0; i < s_numVidModes; i++ )
	{
		ri.Printf( PRINT_ALL, "%s\n", r_vidModes[i].description );
	}
	ri.Printf( PRINT_ALL, "\n" );
}


void R_GetModeInfo(unsigned int *width, unsigned int *height, float *windowAspect, int mode)
{
	const vidmode_t	*vm;

    if ( mode < -2 || mode >= s_numVidModes) {
         mode = 3;
	}

	if ( mode == -1 ) {
		*width = r_customwidth->integer;
		*height = r_customheight->integer;
		*windowAspect = r_customaspect->value;
	}
    else
    {
        vm = &r_vidModes[mode];

        *width  = vm->width;
        *height = vm->height;
        *windowAspect = (float)vm->width / ( (float)vm->height * vm->pixelAspect );
    }
}


void R_InitDisplayResolution( void )
{
    // leilei - -2 is so convenient for modern day PCs

    r_mode = ri.Cvar_Get( "r_mode", "-2", CVAR_ARCHIVE | CVAR_LATCH );
    r_customwidth = ri.Cvar_Get( "r_customwidth", "960", CVAR_ARCHIVE | CVAR_LATCH );
    r_customheight = ri.Cvar_Get( "r_customheight", "540", CVAR_ARCHIVE | CVAR_LATCH );
    r_customaspect = ri.Cvar_Get( "r_customaspect", "1.78", CVAR_ARCHIVE | CVAR_LATCH );
}
