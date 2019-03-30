#ifndef MATRIX_MULTIPLICATION_H_
#define MATRIX_MULTIPLICATION_H_

#include <string.h>
#include <math.h>

void Mat4Identity( float out[4] );
void MatrixMultiply4x4_SSE(const float A[16], const float B[16], float out[16]);
void Mat4x1Transform_SSE( const float A[16], const float x[4], float out[4] );
void Mat4Transform( const float in1[16], const float in2[4], float out[4] );
void Mat4Translation( float vec[3], float out[4] );

void VectorPerp( const vec3_t src, vec3_t dst );
float MakeTwoPerpVectors(const float forward[3], float right[3], float up[3]);


static inline void Mat4Copy( const float in[64], float out[16] )
{
    memcpy(out, in, 64);
}

void Mat3x3Identity( float pMat[3][3] );

static inline void Mat3x3Copy( float dst[3][3], const float src[3][3] )
{
    memcpy(dst, src, 36);
}

static inline void VectorLerp( float a[3], float b[3], float lerp, float out[3])
{
	out[0] = a[0] + (b[0] - a[0]) * lerp;
	out[1] = a[1] + (b[1] - a[1]) * lerp;
	out[2] = a[2] + (b[2] - a[2]) * lerp;
}

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

static inline float VectorLen( const float v[3] )
{
	return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void TransformModelToClip( const float src[3], const float *modelMatrix, const float *projectionMatrix, float eye[4], float dst[4] );
void TransformModelToClip_SSE( const float src[3], const float pMatModel[16], const float pMatProj[16], float dst[4] );


#endif
