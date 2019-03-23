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
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
/*
** QGL_WIN.C
**
** This file implements the operating system binding of GL to QGL function
** pointers.  When doing a port of Quake3 you must implement the following
** two functions:
**
*/
#include "qgl.h"


// Placeholder functions to replace OpenGL calls when Vulkan renderer is active.

static void noglActiveTextureARB ( GLenum texture ) {}
static void noglClientActiveTextureARB( GLenum texture ) {}
static void noglMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t){}
static void noglAlphaFunc(GLenum func, GLclampf ref) {}
static void noglBegin(GLenum mode) {}
static void noglBindTexture(GLenum target, GLuint texture) {}
static void noglBlendFunc(GLenum sfactor, GLenum dfactor) {}
static void noglClear(GLbitfield mask) {}
static void noglClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {}
static void noglClipPlane(GLenum plane, const GLdouble *equation) {}
static void noglColor3f(GLfloat red, GLfloat green, GLfloat blue) {}
static void noglColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {}
static void noglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {}
static void noglCullFace(GLenum mode) {}
static void noglDeleteTextures(GLsizei n, const GLuint *textures) {}
static void noglDepthFunc(GLenum func) {}
static void noglDepthMask(GLboolean flag) {}
static void noglDepthRange(GLclampd zNear, GLclampd zFar) {}
static void noglDisable(GLenum cap) {}
static void noglDisableClientState(GLenum array) {}
static void noglDrawBuffer(GLenum mode) {}
static void noglDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {}
static void noglEnable(GLenum cap) {}
static void noglEnableClientState(GLenum array) {}
static void noglEnd(void) {}
static void noglFinish(void) {}
static GLenum noglGetError(void) { return GL_NO_ERROR; }
static void noglGetIntegerv(GLenum pname, GLint *params) {}
static void noglLineWidth(GLfloat width) {}
static void noglLoadIdentity(void) {}
static void noglLoadMatrixf(const GLfloat *m) {}
static void noglMatrixMode(GLenum mode) {}
static void noglOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {}
static void noglPolygonMode(GLenum face, GLenum mode) {}
static void noglPolygonOffset(GLfloat factor, GLfloat units) {}
static void noglPopMatrix(void) {}
static void noglPushMatrix(void) {}
static void noglReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) {}
static void noglScissor(GLint x, GLint y, GLsizei width, GLsizei height) {}
static void noglStencilFunc(GLenum func, GLint ref, GLuint mask) {}
static void noglStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {}
static void noglTexCoord2f(GLfloat s, GLfloat t) {}
static void noglTexCoord2fv(const GLfloat *v) {}
static void noglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {}
static void noglTexEnvf(GLenum target, GLenum pname, GLfloat param) {}
static void noglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {}
static void noglTexParameterf(GLenum target, GLenum pname, GLfloat param) {}
static void noglTexParameterfv(GLenum target, GLenum pname, const GLfloat *params) {}
static void noglTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {}
static void noglVertex2f(GLfloat x, GLfloat y) {}
static void noglVertex3f(GLfloat x, GLfloat y, GLfloat z) {}
static void noglVertex3fv(const GLfloat *v) {}
static void noglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {}
static void noglViewport(GLint x, GLint y, GLsizei width, GLsizei height) {}


void (* qglAlphaFunc )(GLenum func, GLclampf ref)
    = noglAlphaFunc;
void (* qglBegin )(GLenum mode)
    = noglBegin;
void (* qglBindTexture )(GLenum target, GLuint texture)
    = noglBindTexture;
void (* qglBlendFunc )(GLenum sfactor, GLenum dfactor)
    = noglBlendFunc;
void (* qglClear )(GLbitfield mask)
    = noglClear;
void (* qglClearColor )(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
    = noglClearColor;
void (* qglClipPlane )(GLenum plane, const GLdouble *equation)
    = noglClipPlane;
void (* qglColor3f )(GLfloat red, GLfloat green, GLfloat blue)
    = noglColor3f;
void (* qglColorMask )(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
    = noglColorMask;
void (* qglColorPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
    = noglColorPointer;
void (* qglCullFace )(GLenum mode)
    = noglCullFace;
void (* qglDeleteTextures )(GLsizei n, const GLuint *textures)
    = noglDeleteTextures;
void (* qglDepthFunc )(GLenum func)
    = noglDepthFunc;
void (* qglDepthMask )(GLboolean flag)
    = noglDepthMask;
void (* qglDepthRange )(GLclampd zNear, GLclampd zFar)
    = noglDepthRange;
void (* qglDisable )(GLenum cap)
    = noglDisable;
void (* qglDisableClientState )(GLenum array)
    = noglDisableClientState;
void (* qglDrawBuffer )(GLenum mode)
    = noglDrawBuffer;
void (* qglDrawElements )(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
    = noglDrawElements;

void (* qglEnable )(GLenum cap)
    = noglEnable;

void (* qglEnableClientState )(GLenum array)
    = noglEnableClientState;
void (* qglEnd )(void)
    = noglEnd;
void (* qglFinish )(void)
    = noglFinish;
GLenum (* qglGetError )(void)
    = noglGetError;
void (* qglGetIntegerv )(GLenum pname, GLint *params)
    = noglGetIntegerv;

void (* qglLineWidth )(GLfloat width)
    = noglLineWidth;
void (* qglLoadIdentity )(void)
    = noglLoadIdentity;

void (* qglLoadMatrixf )(const GLfloat *m)
    = noglLoadMatrixf;

void (* qglMatrixMode )(GLenum mode)
    = noglMatrixMode;

void (* qglOrtho )(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
    = noglOrtho;

void (* qglPolygonMode )(GLenum face, GLenum mode)
    = noglPolygonMode;

void (* qglPolygonOffset )(GLfloat factor, GLfloat units)
    = noglPolygonOffset;

void (* qglPopMatrix )(void)
    = noglPopMatrix;
    
void (* qglPushMatrix )(void)
    = noglPushMatrix;
void (* qglReadPixels )(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
    = noglReadPixels;
void (* qglScissor )(GLint x, GLint y, GLsizei width, GLsizei height)
    = noglScissor;

void (* qglStencilFunc )(GLenum func, GLint ref, GLuint mask)
    = noglStencilFunc;
void (* qglStencilOp )(GLenum fail, GLenum zfail, GLenum zpass)
    = noglStencilOp;
void (* qglTexCoord2f )(GLfloat s, GLfloat t)
    = noglTexCoord2f;
void (* qglTexCoord2fv )(const GLfloat *v)
    = noglTexCoord2fv;
void (* qglTexCoordPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
        = noglTexCoordPointer;
void (* qglTexEnvf )(GLenum target, GLenum pname, GLfloat param)
    = noglTexEnvf;
void (* qglTexImage2D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
    = noglTexImage2D;
void (* qglTexParameterf )(GLenum target, GLenum pname, GLfloat param)
    = noglTexParameterf;
void (* qglTexParameterfv )(GLenum target, GLenum pname, const GLfloat *params)
    = noglTexParameterfv;
void (* qglTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
    = noglTexSubImage2D;
void (* qglVertex2f )(GLfloat x, GLfloat y)
    = noglVertex2f;
void (* qglVertex3f )(GLfloat x, GLfloat y, GLfloat z)
    = noglVertex3f;
void (* qglVertex3fv )(const GLfloat *v)
    = noglVertex3fv;
void (* qglVertexPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
    = noglVertexPointer;
void (* qglViewport )(GLint x, GLint y, GLsizei width, GLsizei height)
    = noglViewport;


void (* qglActiveTextureARB) (GLenum texture)
    = noglActiveTextureARB;
void (* qglClientActiveTextureARB) (GLenum texture) 
    = noglClientActiveTextureARB;
void (* qglMultiTexCoord2fARB) (GLenum target, GLfloat s, GLfloat t)
    = noglMultiTexCoord2fARB;


