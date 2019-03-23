/*
 * ==================================================================================
 *       Filename:  matrix_multiplication.c
 *    Description:  4*4 matrix 
 * ==================================================================================
 */

#include <xmmintrin.h>
#include <stdio.h>

#include "matrix_multiplication.h"


static const float s_Identity3x3[3][3] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};

static const float s_Identity4x4[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};


void Mat4Identity( float out[16] )
{
    memcpy(out, s_Identity4x4, 64);
}

void Mat4Translation( float vec[3], float out[16] )
{
    memcpy(out, s_Identity4x4, 64);

    out[12] = vec[0];
    out[13] = vec[1];
    out[14] = vec[2];
}
//
// NOTE; out = b * a ???
// a, b and c are specified in column-major order
//
void myGlMultMatrix(const float A[16], const float B[16], float out[16])
{
	int	i, j;

	for ( i = 0 ; i < 4 ; i++ )
    {
		for ( j = 0 ; j < 4 ; j++ )
        {
			out[ i * 4 + j ] =
				  A [ i * 4 + 0 ] * B [ 0 * 4 + j ]
				+ A [ i * 4 + 1 ] * B [ 1 * 4 + j ]
				+ A [ i * 4 + 2 ] * B [ 2 * 4 + j ]
				+ A [ i * 4 + 3 ] * B [ 3 * 4 + j ];
		}
	}
}


void MatrixMultiply4x4(const float A[16], const float B[16], float out[16])
{
    out[0] = A[0]*B[0] + A[1]*B[4] + A[2]*B[8] + A[3]*B[12];
    out[1] = A[0]*B[1] + A[1]*B[5] + A[2]*B[9] + A[3]*B[13];
    out[2] = A[0]*B[2] + A[1]*B[6] + A[2]*B[10] + A[3]*B[14];
    out[3] = A[0]*B[3] + A[1]*B[7] + A[2]*B[11] + A[3]*B[15];

    out[4] = A[4]*B[0] + A[5]*B[4] + A[6]*B[8] + A[7]*B[12];
    out[5] = A[4]*B[1] + A[5]*B[5] + A[6]*B[9] + A[7]*B[13];
    out[6] = A[4]*B[2] + A[5]*B[6] + A[6]*B[10] + A[7]*B[14];
    out[7] = A[4]*B[3] + A[5]*B[7] + A[6]*B[11] + A[7]*B[15];

    out[8] = A[8]*B[0] + A[9]*B[4] + A[10]*B[8] + A[11]*B[12];
    out[9] = A[8]*B[1] + A[9]*B[5] + A[10]*B[9] + A[11]*B[13];
    out[10] = A[8]*B[2] + A[9]*B[6] + A[10]*B[10] + A[11]*B[14];
    out[11] = A[8]*B[3] + A[9]*B[7] + A[10]*B[11] + A[11]*B[15];

    out[12] = A[12]*B[0] + A[13]*B[4] + A[14]*B[8] + A[15]*B[12];
    out[13] = A[12]*B[1] + A[13]*B[5] + A[14]*B[9] + A[15]*B[13];
    out[14] = A[12]*B[2] + A[13]*B[6] + A[14]*B[10] + A[15]*B[14];
    out[15] = A[12]*B[3] + A[13]*B[7] + A[14]*B[11] + A[15]*B[15];
}

/*
 * NOTE; out = B * A in math
 * a, b and c are specified in column-major order
 * out must be 16 byte aliagned
 */

void MatrixMultiply4x4_SSE(const float A[16], const float B[16], float out[16])
{
    __m128 row1 = _mm_load_ps(&B[0]);
    __m128 row2 = _mm_load_ps(&B[4]);
    __m128 row3 = _mm_load_ps(&B[8]);
    __m128 row4 = _mm_load_ps(&B[12]);
    
    int i;
    for(i=0; i<4; i++)
    {
        __m128 brod1 = _mm_set1_ps(A[4*i    ]);
        __m128 brod2 = _mm_set1_ps(A[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(A[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(A[4*i + 3]);
        
        __m128 row = _mm_add_ps(
            _mm_add_ps( _mm_mul_ps(brod1, row1), _mm_mul_ps(brod2, row2) ),
            _mm_add_ps( _mm_mul_ps(brod3, row3), _mm_mul_ps(brod4, row4) )
            );

        _mm_store_ps(&out[4*i], row);
    }
}


void Mat4Transform( const float in1[16], const float in2[4], float out[4] )
{
    // 16 mult, 12 plus

    float a = in2[0];
    float b = in2[1];
    float c = in2[2];
    float d = in2[3];

	out[0] = in1[0] * a + in1[4] * b + in1[ 8] * c + in1[12] * d;
	out[1] = in1[1] * a + in1[5] * b + in1[ 9] * c + in1[13] * d;
	out[2] = in1[2] * a + in1[6] * b + in1[10] * c + in1[14] * d;
	out[3] = in1[3] * a + in1[7] * b + in1[11] * c + in1[15] * d;
}



// unlucky, not faseter than Mat4Transform
void Mat4x1Transform_SSE( const float A[16], const float x[4], float out[4] )
{
    //   16 mult, 12 plus
	//out[ 0] = A[ 0] * x[ 0] + A[ 4] * x[ 1] + A[ 8] * x[ 2] + A[12] * x[ 3];
	//out[ 1] = A[ 1] * x[ 0] + A[ 5] * x[ 1] + A[ 9] * x[ 2] + A[13] * x[ 3];
	//out[ 2] = A[ 2] * x[ 0] + A[ 6] * x[ 1] + A[10] * x[ 2] + A[14] * x[ 3];
	//out[ 3] = A[ 3] * x[ 0] + A[ 7] * x[ 1] + A[11] * x[ 2] + A[15] * x[ 3];
    

    // 4 mult + 3 plus + 4 broadcast + 8 load (4 _mm_set1_ps + 4 _mm_set1_ps) + 1 store 
    __m128 row = _mm_add_ps(
            _mm_add_ps( _mm_mul_ps( _mm_set1_ps(x[0]), _mm_load_ps(A   ) ) ,
                        _mm_mul_ps( _mm_set1_ps(x[1]), _mm_load_ps(A+4 ) ) )
            ,
            _mm_add_ps( _mm_mul_ps( _mm_set1_ps(x[2]), _mm_load_ps(A+8 ) ) ,
                        _mm_mul_ps( _mm_set1_ps(x[3]), _mm_load_ps(A+12) ) )
            );

    _mm_store_ps(out, row);
}


void Mat3x3Identity( float pMat[3][3] )
{
    memcpy(pMat, s_Identity3x3, 36);
}



void TransformModelToClip_SSE( const float src[3], const float pMatModel[16], const float pMatProj[16], float dst[4] )
{
	float AugSrc[4]	= {src[0], src[1], src[2], 1.0f};


    __m128 row1 = _mm_load_ps(&pMatProj[0]);
    __m128 row2 = _mm_load_ps(&pMatProj[4]);
    __m128 row3 = _mm_load_ps(&pMatProj[8]);
    __m128 row4 = _mm_load_ps(&pMatProj[12]);
    
    __m128 res[4];
    int i;
    for(i=0; i<4; i++)
    {
        __m128 brod1 = _mm_set1_ps(pMatModel[4*i    ]);
        __m128 brod2 = _mm_set1_ps(pMatModel[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(pMatModel[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(pMatModel[4*i + 3]);
        
        __m128 scol = _mm_set1_ps(AugSrc[i]);

        res[i] =_mm_mul_ps( _mm_add_ps(
                                _mm_add_ps( _mm_mul_ps(brod1, row1), _mm_mul_ps(brod2, row2) ),
                                _mm_add_ps( _mm_mul_ps(brod3, row3), _mm_mul_ps(brod4, row4) )
                                ), scol);
    }


    _mm_store_ps(dst, _mm_add_ps( _mm_add_ps(res[0], res[1]),  _mm_add_ps(res[2], res[3]) ) );

//    print4f("AugSrc", AugSrc);
//    printMat4x4f("MatModel", pMatModel);
//    printMat4x4f("MatProj", pMatProj);
//    MatrixMultiply4x4_SSE(pMatModel, pMatProj, mvp);
//    Mat4x1Transform_SSE(mvp, AugSrc, dst);
//    print4f("dst", dst);
}




void TransformModelToClip_SSE2( const float x[3], const float pMatModel[16], const float pMatProj[16], float dst[4] )
{

    // 7/8 broadcaster, 8 load, 7/8 mult, 6 add

    __m128 row = _mm_add_ps(
            _mm_add_ps( _mm_mul_ps( _mm_set1_ps(x[0]), _mm_load_ps(pMatModel   ) ) ,
                        _mm_mul_ps( _mm_set1_ps(x[1]), _mm_load_ps(pMatModel+4 ) ) )
            ,
            _mm_add_ps( _mm_mul_ps( _mm_set1_ps(x[2]), _mm_load_ps(pMatModel+8 ) ) ,
                                                       _mm_load_ps(pMatModel+12) ) );
    _mm_store_ps(dst, _mm_add_ps(
            _mm_add_ps( _mm_mul_ps( _mm_set1_ps(row[0]), _mm_load_ps(pMatProj   ) ) ,
                        _mm_mul_ps( _mm_set1_ps(row[1]), _mm_load_ps(pMatProj+4 ) ) )
            ,
            _mm_add_ps( _mm_mul_ps( _mm_set1_ps(row[2]), _mm_load_ps(pMatProj+8 ) ) ,
                        _mm_mul_ps( _mm_set1_ps(row[3]), _mm_load_ps(pMatProj+12) ) ) ) 
                );


//    _mm_store_ps(dst, row);

//    Mat4x1Transform_SSE(pMatModel, AugSrc, eye);
//    Mat4x1Transform_SSE(pMatProj, eye, dst);
}



void TransformModelToClip( const float src[3], const float* pMatModel, const float* pMatProj, float eye[4], float dst[4])
{
	int i;

	for ( i = 0 ; i < 4 ; i++ )
    {
		eye[i] = 
			src[0] * pMatModel[ i + 0 * 4 ] +
			src[1] * pMatModel[ i + 1 * 4 ] +
			src[2] * pMatModel[ i + 2 * 4 ] +
                 1 * pMatModel[ i + 3 * 4 ];
	}

	for ( i = 0 ; i < 4 ; i++ )
    {
		dst[i] = 
			eye[0] * pMatProj[ i + 0 * 4 ] +
			eye[1] * pMatProj[ i + 1 * 4 ] +
			eye[2] * pMatProj[ i + 2 * 4 ] +
			eye[3] * pMatProj[ i + 3 * 4 ];
	}
}






// ===============================================
// not used now
// ===============================================

/*
void Mat4SimpleInverse( const float in[16], float out[16])
{
	float v[3];
	float invSqrLen;
 
	VectorCopy(in + 0, v);
	invSqrLen = 1.0f / DotProduct(v, v); VectorScale(v, invSqrLen, v);
	out[ 0] = v[0]; out[ 4] = v[1]; out[ 8] = v[2]; out[12] = -DotProduct(v, &in[12]);

	VectorCopy(in + 4, v);
	invSqrLen = 1.0f / DotProduct(v, v); VectorScale(v, invSqrLen, v);
	out[ 1] = v[0]; out[ 5] = v[1]; out[ 9] = v[2]; out[13] = -DotProduct(v, &in[12]);

	VectorCopy(in + 8, v);
	invSqrLen = 1.0f / DotProduct(v, v); VectorScale(v, invSqrLen, v);
	out[ 2] = v[0]; out[ 6] = v[1]; out[10] = v[2]; out[14] = -DotProduct(v, &in[12]);

	out[ 3] = 0.0f; out[ 7] = 0.0f; out[11] = 0.0f; out[15] = 1.0f;
}


void Mat4Dump( const float in[16] )
{
	printf( "%3.5f %3.5f %3.5f %3.5f\n", in[ 0], in[ 4], in[ 8], in[12]);
	printf( "%3.5f %3.5f %3.5f %3.5f\n", in[ 1], in[ 5], in[ 9], in[13]);
	printf( "%3.5f %3.5f %3.5f %3.5f\n", in[ 2], in[ 6], in[10], in[14]);
	printf( "%3.5f %3.5f %3.5f %3.5f\n", in[ 3], in[ 7], in[11], in[15]);
}

void Mat4View(vec3_t axes[3], vec3_t origin, mat4_t out)
{
	out[0]  = axes[0][0];
	out[1]  = axes[1][0];
	out[2]  = axes[2][0];
	out[3]  = 0;

	out[4]  = axes[0][1];
	out[5]  = axes[1][1];
	out[6]  = axes[2][1];
	out[7]  = 0;

	out[8]  = axes[0][2];
	out[9]  = axes[1][2];
	out[10] = axes[2][2];
	out[11] = 0;

	out[12] = -DotProduct(origin, axes[0]);
	out[13] = -DotProduct(origin, axes[1]);
	out[14] = -DotProduct(origin, axes[2]);
	out[15] = 1;
}
*/
