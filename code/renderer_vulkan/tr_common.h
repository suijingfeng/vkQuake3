/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
#ifndef TR_COMMON_H
#define TR_COMMON_H

#include "../qcommon/q_shared.h"
#include "../renderercommon/tr_public.h"

static const float ORIGIN[3] = {0,0,0};
// outside of TR since it shouldn't be cleared during ref re-init

extern glconfig_t glConfig;	
// These variables should live inside glConfig but can't because of
// compatibility issues to the original ID vms.  If you release a stand-alone
// game and your mod uses tr_types.h from this build you can safely move them
// to the glconfig_t struct.

union uInt4bytes{
    unsigned int i;
    unsigned char uc[4];
};

union f32_u {
	float f;
	uint32_t ui;
    unsigned char uc[4];
	struct {
		unsigned int fraction:23;
		unsigned int exponent:8;
		unsigned int sign:1;
	} pack;
};

union f16_u {
	uint16_t ui;
	struct {
		unsigned int fraction:10;
		unsigned int exponent:5;
		unsigned int sign:1;
	} pack;
};


// any change in the LIGHTMAP_* defines here MUST be reflected in
// R_FindShader() in tr_bsp.c
#define LIGHTMAP_2D         -4	// shader is for 2D rendering
#define LIGHTMAP_BY_VERTEX  -3	// pre-lit triangle models
#define LIGHTMAP_WHITEIMAGE -2
#define LIGHTMAP_NONE       -1



float R_NoiseGet4f( float x, float y, float z, float t );
void  R_NoiseInit( void );


void R_IssuePendingRenderCommands( void );
//qhandle_t RE_RegisterShaderLightMap( const char *name, int lightmapIndex );
qhandle_t RE_RegisterShader( const char *name );
qhandle_t RE_RegisterShaderNoMip( const char *name );


// font stuff
void R_InitFreeType( void );
void R_DoneFreeType( void );
void RE_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font);


/*
=============================================================

//common subroutines for render

=============================================================
*/


void PointRotateAroundVector(float* dst, const float* dir, const float* p, const float degrees);
void RotateAroundUnitVector(float* res, const float* k, const float* p, const float degrees);

void FastNormalize1f(float v[3]);
char *getExtension( const char *name );
char *SkipPath(char *pathname);
void stripExtension(const char *in, char *out, int destsize);

char* R_ParseExt(char** data_p, qboolean allowLineBreaks);
int R_Compress( char *data_p );
int R_GetCurrentParseLine( void );
void R_BeginParseSession(const char* name);

void SetPlaneSignbits( struct cplane_s *out );
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *plane);
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs );


void VectorPerp( const vec3_t src, vec3_t dst );
float MakeTwoPerpVectors(const float forward[3], float right[3], float up[3]);


void ClearBounds( vec3_t mins, vec3_t maxs );
qboolean SkipBracedSection (char **program, int depth);



#define VectorCopy2(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1])
#define VectorSet2(v,x,y)       ((v)[0]=(x),(v)[1]=(y));

#define VectorCopy4(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])
#define VectorSet4(v,x,y,z,w)	((v)[0]=(x),(v)[1]=(y),(v)[2]=(z),(v)[3]=(w))
#define DotProduct4(a,b)        ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] + (a)[3]*(b)[3])
#define VectorScale4(a,b,c)     ((c)[0]=(a)[0]*(b),(c)[1]=(a)[1]*(b),(c)[2]=(a)[2]*(b),(c)[3]=(a)[3]*(b))

#define VectorCopy5(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3],(b)[4]=(a)[4])

#define OffsetByteToFloat(a)    ((float)(a) * 1.0f/127.5f - 1.0f)
#define FloatToOffsetByte(a)    (byte)((a) * 127.5f + 128.0f)
#define ByteToFloat(a)          ((float)(a) * 1.0f/255.0f)
#define FloatToByte(a)          (byte)((a) * 255.0f)

static inline void VectorNorm( float v[3] )
{
	float length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

    if(length != 0)
    {
        /* writing it this way allows gcc to recognize that rsqrt can be used */
        length = 1.0f / sqrtf (length);
        v[0] *= length;
        v[1] *= length;
        v[2] *= length;
    }

}

static inline void VectorNorm2(const float v[3], float out[3])
{
	float ilength = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

    if(ilength == 0)
    {
//		ri.Printf( PRINT_WARNING, "VectorNorm2: Trying to normalize zero vector!\n");
        return;
    }

	/* writing it this way allows gcc to recognize that rsqrt can be used */
	ilength = 1.0f / sqrtf(ilength);
	out[0] = v[0]*ilength;
	out[1] = v[1]*ilength;
	out[2] = v[2]*ilength;
}


static inline float VectorLen( const float v[3] )
{
	return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

qboolean SpheresIntersect(float origin1[3], float radius1, float origin2[3], float radius2);
void BoundingSphereOfSpheres(float origin1[3], float radius1, float origin2[3], float radius2, float origin3[3], float *radius3);

#ifndef SGN
#define SGN(x) (((x) >= 0) ? !!(x) : -1)
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef CLAMP
#define CLAMP(a,b,c) MIN(MAX((a),(b)),(c))
#endif


#endif
