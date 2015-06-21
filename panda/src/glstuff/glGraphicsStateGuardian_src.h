// Filename: glGraphicsStateGuardian_src.h
// Created by:  drose (02Feb99)
// Updated by: fperazzi, PandaSE (05May10) (added
//   get_supports_cg_profile)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "pandabase.h"

#include "graphicsStateGuardian.h"
#include "geomVertexColumn.h"
#include "texture.h"
#include "displayRegion.h"
#include "material.h"
#include "colorWriteAttrib.h"
#include "depthTestAttrib.h"
#include "textureAttrib.h"
#include "texMatrixAttrib.h"
#include "texGenAttrib.h"
#include "shaderAttrib.h"
#include "textureStage.h"
#include "antialiasAttrib.h"
#include "renderModeAttrib.h"
#include "colorBlendAttrib.h"
#include "pointerToArray.h"
#include "fog.h"
#include "graphicsWindow.h"
#include "pset.h"
#include "pmap.h"
#include "geomVertexArrayData.h"
#include "lightMutex.h"
#include "pStatGPUTimer.h"

class PlaneNode;
class Light;

#ifdef OPENGLES
// This is a cheesy hack to allow the following typedefs to compile
// for OpenGL ES.  We won't actually be calling any of the "double"
// functions in this case (and they don't exist anyway), but we need
// to be able to get past their declarations.
typedef double GLdouble;
#endif  // OPENGLES

// These typedefs are declared in glext.h, but we must repeat them
// here, mainly because they will not be included from glext.h if the
// system GL version matches or exceeds the GL version in which these
// functions are defined, and the system gl.h sometimes doesn't
// declare these typedefs.
#if !defined( __EDG__ ) || defined( __INTEL_COMPILER )  // Protect the following from the Tau instrumentor and expose it for the intel compiler.
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,GLvoid *userParam);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC) (GLDEBUGPROC callback, const void *userParam);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef void (APIENTRYP PFNGLOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint level, GLvoid *img);
typedef void (APIENTRYP PFNGLGENQUERIESPROC) (GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLBEGINQUERYPROC) (GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLENDQUERYPROC) (GLenum target);
typedef void (APIENTRYP PFNGLDELETEQUERIESPROC) (GLsizei n, const GLuint *ids);
typedef void (APIENTRYP PFNGLGETQUERYIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC) (GLuint id, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VPROC) (GLuint id, GLenum pname, GLuint64 *params);
typedef void (APIENTRYP PFNGLGETINTEGER64VPROC) (GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
// There is some trivial disagreement between different gl.h headers about this one, so we use our own typename.
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC_P) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FPROC) (GLenum target, const GLfloat s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FPROC) (GLenum target, const GLfloat s, const GLfloat t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FPROC) (GLenum target, const GLfloat s, const GLfloat t, const GLfloat r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FPROC) (GLenum target, const GLfloat s, const GLfloat t, const GLfloat r, const GLfloat q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DPROC) (GLenum target, const GLdouble s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DPROC) (GLenum target, const GLdouble s, const GLdouble t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DPROC) (GLenum target, const GLdouble s, const GLdouble t, const GLdouble r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DPROC) (GLenum target, const GLdouble s, const GLdouble t, const GLdouble r, const GLdouble q);
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLBLENDCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLCLEARBUFFERFVPROC) (GLenum buffer, GLint drawbuffer, const GLfloat *value);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRYP PFNGLACTIVESTENCILFACEEXTPROC) (GLenum face);
typedef void (APIENTRYP PFNGLWEIGHTPOINTERARBPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXBLENDARBPROC) (GLint count);
typedef void (APIENTRYP PFNGLWEIGHTFVARBPROC) (GLint size, const GLfloat *weights);
typedef void (APIENTRYP PFNGLWEIGHTDVARBPROC) (GLint size, const GLdouble *weights);
typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFEREXTPROC) (GLuint renderbuffer);
typedef void (APIENTRYP PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) (GLenum target, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFEREXTPROC) (GLuint framebuffer);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint *framebuffers);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
#ifdef OPENGLES_2
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DOES) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
#else
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
#endif
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREARBPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) (GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPEXTPROC) (GLenum target);
typedef void (APIENTRYP PFNGLCURRENTPALETTEMATRIXARBPROC) (GLint index);
typedef void (APIENTRYP PFNGLMATRIXINDEXUIVARBPROC) (GLint size, const GLuint *indices);
typedef void (APIENTRYP PFNGLMATRIXINDEXPOINTERARBPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFEREXTPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC) (GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLCURRENTPALETTEMATRIXOESPROC) (GLuint matrixpaletteindex);
typedef void (APIENTRYP PFNGLLOADPALETTEFROMMODELVIEWMATRIXOESPROC) (void);
typedef void (APIENTRYP PFNGLMATRIXINDEXPOINTEROESPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLWEIGHTPOINTEROESPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLTEXSTORAGE1DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLTEXSTORAGE2DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXSTORAGE3DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

#ifndef OPENGLES_1
// GLSL shader functions
typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar *name);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC_P) (GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC) (GLuint index, GLuint divisor);
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC) (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount);
#endif  // OPENGLES_1
#ifndef OPENGLES
typedef void (APIENTRYP PFNGLGENSAMPLERSPROC) (GLsizei count, GLuint *samplers);
typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC) (GLsizei count, const GLuint *samplers);
typedef void (APIENTRYP PFNGLBINDSAMPLERPROC) (GLuint unit, GLuint sampler);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC) (GLuint sampler, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC) (GLuint sampler, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIEXTPROC) (GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLCLEARTEXIMAGEPROC) (GLuint texture, GLint level, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARTEXSUBIMAGEPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLBINDTEXTURESPROC) (GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLBINDSAMPLERSPROC) (GLuint first, GLsizei count, const GLuint *samplers);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTUREPROC) (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTURESPROC) (GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEPROC) (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (APIENTRYP PFNGLMEMORYBARRIERPROC) (GLbitfield barriers);
typedef void (APIENTRYP PFNGLGETPROGRAMBINARYPROC) (GLuint program, GLsizei bufsize, GLsizei *length, GLenum *binaryFormat, void *binary);
typedef void (APIENTRYP PFNGLGETINTERNALFORMATIVPROC) (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params);
typedef GLuint64 (APIENTRYP PFNGLGETTEXTUREHANDLEPROC) (GLuint texture);
typedef GLuint64 (APIENTRYP PFNGLGETTEXTURESAMPLERHANDLEPROC) (GLuint texture, GLuint sampler);
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLERESIDENTPROC) (GLuint64 handle);
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLENONRESIDENTPROC) (GLuint64 handle);
typedef GLuint64 (APIENTRYP PFNGLGETIMAGEHANDLEPROC) (GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format);
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLERESIDENTPROC) (GLuint64 handle, GLenum access);
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLENONRESIDENTPROC) (GLuint64 handle);
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64PROC) (GLint location, GLuint64 value);
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64VPROC) (GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64PROC) (GLuint program, GLint location, GLuint64 value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64VPROC) (GLuint program, GLint location, GLsizei count, const GLuint64 *values);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREHANDLERESIDENTPROC) (GLuint64 handle);
typedef GLboolean (APIENTRYP PFNGLISIMAGEHANDLERESIDENTPROC) (GLuint64 handle);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1UI64PROC) (GLuint index, GLuint64EXT x);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1UI64VPROC) (GLuint index, const GLuint64EXT *v);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLUI64VPROC) (GLuint index, GLenum pname, GLuint64EXT *params);
#endif  // OPENGLES
#endif  // __EDG__

////////////////////////////////////////////////////////////////////
//       Class : GLGraphicsStateGuardian
// Description : A GraphicsStateGuardian specialized for rendering
//               into OpenGL contexts.  There should be no GL calls
//               outside of this object.
////////////////////////////////////////////////////////////////////
class EXPCL_GL CLP(GraphicsStateGuardian) : public GraphicsStateGuardian {
public:
  CLP(GraphicsStateGuardian)(GraphicsEngine *engine, GraphicsPipe *pipe);
  virtual ~CLP(GraphicsStateGuardian)();

  //#--- Zhao Nov/2011
  virtual string get_driver_vendor();
  virtual string get_driver_renderer();
  virtual string get_driver_version();
  virtual int get_driver_version_major();
  virtual int get_driver_version_minor();
  virtual int get_driver_shader_version_major();
  virtual int get_driver_shader_version_minor();

#ifndef OPENGLES_1
  static void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam);
#endif

  virtual void reset();

  virtual void prepare_display_region(DisplayRegionPipelineReader *dr);
  virtual void clear_before_callback();
  virtual CPT(TransformState) calc_projection_mat(const Lens *lens);
  virtual bool prepare_lens();

  virtual bool begin_frame(Thread *current_thread);
  virtual bool begin_scene();
  virtual void end_scene();
  virtual void end_frame(Thread *current_thread);

  virtual bool begin_draw_primitives(const GeomPipelineReader *geom_reader,
                                     const GeomMunger *munger,
                                     const GeomVertexDataPipelineReader *data_reader,
                                     bool force);
  virtual bool draw_triangles(const GeomPrimitivePipelineReader *reader,
                              bool force);
  virtual bool draw_tristrips(const GeomPrimitivePipelineReader *reader,
                              bool force);
  virtual bool draw_trifans(const GeomPrimitivePipelineReader *reader,
                            bool force);
  virtual bool draw_patches(const GeomPrimitivePipelineReader *reader,
                            bool force);
  virtual bool draw_lines(const GeomPrimitivePipelineReader *reader,
                          bool force);
  virtual bool draw_linestrips(const GeomPrimitivePipelineReader *reader,
                               bool force);
  virtual bool draw_points(const GeomPrimitivePipelineReader *reader,
                           bool force);
  virtual void end_draw_primitives();

#ifndef OPENGLES
  void issue_memory_barrier(GLbitfield barrier);
#endif

  virtual TextureContext *prepare_texture(Texture *tex, int view);
  virtual bool update_texture(TextureContext *tc, bool force);
  virtual void release_texture(TextureContext *tc);
  virtual bool extract_texture_data(Texture *tex);

#ifndef OPENGLES
  virtual SamplerContext *prepare_sampler(const SamplerState &sampler);
  virtual void release_sampler(SamplerContext *sc);
#endif

  virtual GeomContext *prepare_geom(Geom *geom);
  virtual void release_geom(GeomContext *gc);

  virtual ShaderContext *prepare_shader(Shader *shader);
  virtual void release_shader(ShaderContext *sc);

  void record_deleted_display_list(GLuint index);

  virtual VertexBufferContext *prepare_vertex_buffer(GeomVertexArrayData *data);
  bool apply_vertex_buffer(VertexBufferContext *vbc,
                           const GeomVertexArrayDataHandle *reader,
                           bool force);
  virtual void release_vertex_buffer(VertexBufferContext *vbc);

  bool setup_array_data(const unsigned char *&client_pointer,
                        const GeomVertexArrayDataHandle *data,
                        bool force);

  virtual IndexBufferContext *prepare_index_buffer(GeomPrimitive *data);
  bool apply_index_buffer(IndexBufferContext *ibc,
                          const GeomPrimitivePipelineReader *reader,
                          bool force);
  virtual void release_index_buffer(IndexBufferContext *ibc);
  bool setup_primitive(const unsigned char *&client_pointer,
                       const GeomPrimitivePipelineReader *reader,
                       bool force);

#ifndef OPENGLES
  virtual void begin_occlusion_query();
  virtual PT(OcclusionQueryContext) end_occlusion_query();
#endif

  virtual PT(TimerQueryContext) issue_timer_query(int pstats_index);

#ifndef OPENGLES
  virtual void dispatch_compute(int size_x, int size_y, int size_z);
#endif

  virtual PT(GeomMunger) make_geom_munger(const RenderState *state,
                                          Thread *current_thread);

  virtual PN_stdfloat compute_distance_to(const LPoint3 &point) const;

  virtual void clear(DrawableRegion *region);

  virtual bool framebuffer_copy_to_texture
    (Texture *tex, int view, int z, const DisplayRegion *dr, const RenderBuffer &rb);
  virtual bool framebuffer_copy_to_ram
    (Texture *tex, int view, int z, const DisplayRegion *dr, const RenderBuffer &rb);

#ifndef OPENGLES_2
  void apply_fog(Fog *fog);

  virtual void bind_light(PointLight *light_obj, const NodePath &light,
                          int light_id);
  virtual void bind_light(DirectionalLight *light_obj, const NodePath &light,
                          int light_id);
  virtual void bind_light(Spotlight *light_obj, const NodePath &light,
                          int light_id);
#endif

  void print_gfx_visual();

  LVecBase4 get_light_color(Light *light) const;

#ifdef SUPPORT_IMMEDIATE_MODE
  void draw_immediate_simple_primitives(const GeomPrimitivePipelineReader *reader, GLenum mode);
  void draw_immediate_composite_primitives(const GeomPrimitivePipelineReader *reader, GLenum mode);
#endif  // SUPPORT_IMMEDIATE_MODE

  INLINE bool report_errors(int line, const char *source_file);
  INLINE void report_my_errors(int line, const char *source_file);
  INLINE bool clear_errors(int line, const char *source_file);
  INLINE void clear_my_errors(int line, const char *source_file);

  INLINE const string &get_gl_vendor() const;
  INLINE const string &get_gl_renderer() const;
  INLINE const string &get_gl_version() const;
  INLINE int get_gl_version_major() const;
  INLINE int get_gl_version_minor() const;

  virtual void set_state_and_transform(const RenderState *state,
                                       const TransformState *transform);

  void bind_fbo(GLuint fbo);
  virtual bool get_supports_cg_profile(const string &name) const;
  void finish();

protected:
  void do_issue_transform();
  void do_issue_render_mode();
  void do_issue_antialias();
  void do_issue_rescale_normal();
  void do_issue_color_write();
  void do_issue_depth_test();
#ifndef OPENGLES_2
  void do_issue_alpha_test();
#endif
  void do_issue_depth_write();
  void do_issue_cull_face();
#ifndef OPENGLES_2
  void do_issue_fog();
#endif
  void do_issue_depth_offset();
  void do_issue_shade_model();
  void do_issue_shader(bool state_has_changed = false);
#ifndef OPENGLES_2
  void do_issue_material();
#endif
  void do_issue_texture();
  void do_issue_blending();
#ifndef OPENGLES_2
  void do_issue_tex_gen();
  void do_issue_tex_matrix();
#endif
  void do_issue_stencil();
  void do_issue_scissor();

  virtual void gl_flush() const;
  INLINE void maybe_gl_finish() const;
  virtual GLenum gl_get_error() const;

  static bool report_errors_loop(int line, const char *source_file,
                                 GLenum error_code, int &error_count);
  static string get_error_string(GLenum error_code);
  string show_gl_string(const string &name, GLenum id);
  virtual void query_gl_version();
  void save_extensions(const char *extensions);
  virtual void get_extra_extensions();
  void report_extensions() const;
  INLINE virtual bool has_extension(const string &extension) const;
  INLINE bool is_at_least_gl_version(int major_version, int minor_version) const;
  INLINE bool is_at_least_gles_version(int major_version, int minor_version) const;
  void *get_extension_func(const char *name);
  virtual void *do_get_extension_func(const char *name);

  virtual void reissue_transforms();

#ifndef OPENGLES_2
  virtual void enable_lighting(bool enable);
  virtual void set_ambient_light(const LColor &color);
  virtual void enable_light(int light_id, bool enable);
  virtual void begin_bind_lights();
  virtual void end_bind_lights();

  virtual void enable_clip_plane(int plane_id, bool enable);
  virtual void begin_bind_clip_planes();
  virtual void bind_clip_plane(const NodePath &plane, int plane_id);
  virtual void end_bind_clip_planes();
#endif

  virtual void free_pointers();

  INLINE void enable_multisample_antialias(bool val);
  INLINE void enable_multisample_alpha_one(bool val);
  INLINE void enable_multisample_alpha_mask(bool val);
  INLINE void enable_line_smooth(bool val);
  INLINE void enable_point_smooth(bool val);
  INLINE void enable_polygon_smooth(bool val);
  INLINE void setup_antialias_line();
  INLINE void setup_antialias_point();
  INLINE void setup_antialias_polygon();

  INLINE void enable_stencil_test(bool val);
  INLINE void enable_blend(bool val);
  INLINE void enable_depth_test(bool val);
  INLINE void enable_fog(bool val);
  INLINE void enable_alpha_test(bool val);
  INLINE void enable_polygon_offset(bool val);

  INLINE void set_color_write_mask(int mask);
  INLINE void clear_color_write_mask();

#ifndef OPENGLES_2
  INLINE void call_glFogfv(GLenum pname, const LColor &color);
  INLINE void call_glMaterialfv(GLenum face, GLenum pname, const LColor &color);
  INLINE void call_glLightfv(GLenum light, GLenum pname, const LVecBase4 &value);
  INLINE void call_glLightfv(GLenum light, GLenum pname, const LVecBase3 &value);
  INLINE void call_glLightModelfv(GLenum pname, const LVecBase4 &value);
  INLINE void call_glTexEnvfv(GLenum target, GLenum pname, const LVecBase4 &value);
#endif

  INLINE void call_glTexParameterfv(GLenum target, GLenum pname, const LVecBase4 &value);

#ifndef OPENGLES_2
  INLINE GLenum get_light_id(int index) const;
  INLINE GLenum get_clip_plane_id(int index) const;
#endif

  void set_draw_buffer(int rbtype);
  void set_read_buffer(int rbtype);

  static GLenum get_numeric_type(Geom::NumericType numeric_type);
  GLenum get_texture_target(Texture::TextureType texture_type) const;
  GLenum get_texture_wrap_mode(SamplerState::WrapMode wm) const;
  static SamplerState::WrapMode get_panda_wrap_mode(GLenum wm);
  static GLenum get_texture_filter_type(SamplerState::FilterType ft,
                                        bool ignore_mipmaps);
  static SamplerState::FilterType get_panda_filter_type(GLenum ft);
  GLenum get_component_type(Texture::ComponentType component_type);
  GLint get_external_image_format(Texture *tex) const;
  GLint get_internal_image_format(Texture *tex, bool force_sized=false) const;
  static bool is_mipmap_filter(GLenum min_filter);
  static bool is_compressed_format(GLenum format);
  static GLint get_texture_apply_mode_type(TextureStage::Mode am);
  static GLint get_texture_combine_type(TextureStage::CombineMode cm);
  GLint get_texture_src_type(TextureStage::CombineSource cs,
                             int last_stage, int last_saved_result,
                             int this_stage) const;
  static GLint get_texture_operand_type(TextureStage::CombineOperand co);
  static GLenum get_fog_mode_type(Fog::Mode m);
  static GLenum get_blend_equation_type(ColorBlendAttrib::Mode mode);
  static GLenum get_blend_func(ColorBlendAttrib::Operand operand);
  static GLenum get_usage(Geom::UsageHint usage_hint);

  void unbind_buffers();
#ifndef OPENGLES_2
  void disable_standard_vertex_arrays();
  bool update_standard_vertex_arrays(bool force);
  void disable_standard_texture_bindings();
  void update_standard_texture_bindings();
#endif

  void apply_white_texture();

#ifndef NDEBUG
  void update_show_usage_texture_bindings(int show_stage_index);
  void upload_usage_texture(int width, int height);
#endif  // NDEBUG

  bool specify_texture(CLP(TextureContext) *gtc, const SamplerState &sampler);
  bool apply_texture(TextureContext *tc);
  bool apply_sampler(GLuint unit, const SamplerState &sampler, TextureContext *tc);
  bool upload_texture(CLP(TextureContext) *gtc, bool force, bool uses_mipmaps);
  bool upload_texture_image(CLP(TextureContext) *gtc, bool needs_reload,
                            bool uses_mipmaps, int mipmap_bias,
                            GLenum texture_target, GLenum page_target,
                            GLint internal_format, GLint external_format,
                            GLenum component_type,
                            bool one_page_only, int z,
                            Texture::CompressionMode image_compression);
  bool upload_simple_texture(CLP(TextureContext) *gtc);

  size_t get_texture_memory_size(Texture *tex);
  void check_nonresident_texture(BufferContextChain &chain);
  bool do_extract_texture_data(CLP(TextureContext) *gtc);
  bool extract_texture_image(PTA_uchar &image, size_t &page_size,
           Texture *tex, GLenum target, GLenum page_target,
           Texture::ComponentType type,
           Texture::CompressionMode compression, int n);

  void do_point_size();

  enum AutoAntialiasMode {
    AA_poly,
    AA_line,
    AA_point,
  };

  enum MultisampleMode {
    MM_antialias  = 0x0001,
    MM_alpha_one  = 0x0002,
    MM_alpha_mask = 0x0004,
  };

  int  _multisample_mode;
  bool _line_smooth_enabled;
  bool _point_smooth_enabled;
  bool _polygon_smooth_enabled;
  bool _stencil_test_enabled;
  bool _blend_enabled;
  bool _depth_test_enabled;
  bool _fog_enabled;
  bool _alpha_test_enabled;
  bool _polygon_offset_enabled;
  bool _flat_shade_model;
  int  _decal_level;
  int _active_color_write_mask;

  bool _dithering_enabled;

  int _viewport_x;
  int _viewport_y;
  int _viewport_width;
  int _viewport_height;
  int _draw_buffer_type;
  bool _auto_antialias_mode;
  RenderModeAttrib::Mode _render_mode;
  PN_stdfloat _point_size;
  bool _point_perspective;
  bool _vertex_blending_enabled;
  bool _scissor_enabled;
  bool _scissor_attrib_active;
  epvector<LVecBase4i> _scissor_array;

#ifndef OPENGLES_1
  PT(Shader) _current_shader;
  ShaderContext *_current_shader_context;
  PT(Shader) _vertex_array_shader;
  ShaderContext *_vertex_array_shader_context;
  PT(Shader) _texture_binding_shader;
  ShaderContext *_texture_binding_shader_context;
#endif
#ifdef OPENGLES_2
  static PT(Shader) _default_shader;
#endif

#ifdef HAVE_CG
  CGcontext _cg_context;
#endif

#ifdef SUPPORT_IMMEDIATE_MODE
  CLP(ImmediateModeSender) _sender;
  bool _use_sender;
#endif  // SUPPORT_IMMEDIATE_MODE

  bool _supports_vertex_attrib_divisor;

  // Cache the data necessary to bind each particular light each
  // frame, so if we bind a given light multiple times, we only have
  // to compute its data once.
  class DirectionalLightFrameData {
  public:
    LVector4 _neg_dir;
  };
  typedef pmap<NodePath, DirectionalLightFrameData> DirectionalLights;
  DirectionalLights _dlights;

  int _pass_number;
  GLuint _geom_display_list;
  GLuint _current_vbuffer_index;
  GLuint _current_ibuffer_index;
  GLuint _current_fbo;
  int _num_active_texture_stages;
  PN_stdfloat _max_anisotropy;
  bool _supports_anisotropy;
  GLint _max_image_units;
  bool _supports_multi_bind;
  bool _supports_get_program_binary;
  bool _supports_uniform_buffers;

#ifdef OPENGLES
  bool _supports_depth24;
  bool _supports_depth32;
#endif

  int _error_count;

  string _gl_vendor;
  string _gl_renderer;
  string _gl_version;
  int _gl_version_major, _gl_version_minor;
  //#--- Zhao Nov/2011
  int _gl_shadlang_ver_major, _gl_shadlang_ver_minor;

  pset<string> _extensions;

public:
  bool _supports_point_parameters;
  PFNGLPOINTPARAMETERFVPROC _glPointParameterfv;
  bool _supports_point_sprite;

#ifndef OPENGLES
  PFNGLPRIMITIVERESTARTINDEXPROC _glPrimitiveRestartIndex;
  bool _explicit_primitive_restart;
#endif

  bool _supports_vertex_blend;
  PFNGLWEIGHTPOINTERARBPROC _glWeightPointer;
  PFNGLVERTEXBLENDARBPROC _glVertexBlend;
  PFNGLWEIGHTFVARBPROC _glWeightfv;
  PFNGLWEIGHTDVARBPROC _glWeightdv;

  bool _supports_matrix_palette;
#ifdef OPENGLES_1
  PFNGLCURRENTPALETTEMATRIXOESPROC _glCurrentPaletteMatrix;
  PFNGLMATRIXINDEXPOINTEROESPROC _glMatrixIndexPointer;
#endif
#ifndef OPENGLES
  PFNGLCURRENTPALETTEMATRIXARBPROC _glCurrentPaletteMatrix;
  PFNGLMATRIXINDEXPOINTERARBPROC _glMatrixIndexPointer;
  PFNGLMATRIXINDEXUIVARBPROC _glMatrixIndexuiv;
#endif

#ifndef OPENGLES
  PFNGLDRAWRANGEELEMENTSPROC _glDrawRangeElements;
#endif

#ifndef OPENGLES_1
  PFNGLTEXIMAGE3DPROC_P _glTexImage3D;
  PFNGLTEXSUBIMAGE3DPROC _glTexSubImage3D;
  PFNGLCOPYTEXSUBIMAGE3DPROC _glCopyTexSubImage3D;
#endif

  bool _supports_tex_storage;
  PFNGLTEXSTORAGE1DPROC _glTexStorage1D;
  PFNGLTEXSTORAGE2DPROC _glTexStorage2D;
  PFNGLTEXSTORAGE3DPROC _glTexStorage3D;

#ifndef OPENGLES
  PFNGLTEXBUFFERPROC _glTexBuffer;
#endif

  bool _supports_clear_texture;
#ifndef OPENGLES
  PFNGLCLEARTEXIMAGEPROC _glClearTexImage;
#endif

  PFNGLCOMPRESSEDTEXIMAGE1DPROC _glCompressedTexImage1D;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC _glCompressedTexImage2D;
  PFNGLCOMPRESSEDTEXIMAGE3DPROC _glCompressedTexImage3D;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC _glCompressedTexSubImage1D;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC _glCompressedTexSubImage2D;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC _glCompressedTexSubImage3D;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC _glGetCompressedTexImage;

  bool _supports_bgr;
  bool _supports_rescale_normal;
  bool _supports_packed_dabc;
  bool _supports_packed_ufloat;

  PFNGLACTIVETEXTUREPROC _glActiveTexture;
#ifndef OPENGLES_2
  bool _supports_multitexture;
  PFNGLCLIENTACTIVETEXTUREPROC _glClientActiveTexture;
  PFNGLMULTITEXCOORD1FPROC _glMultiTexCoord1f;
  PFNGLMULTITEXCOORD2FPROC _glMultiTexCoord2f;
  PFNGLMULTITEXCOORD3FPROC _glMultiTexCoord3f;
  PFNGLMULTITEXCOORD4FPROC _glMultiTexCoord4f;
  PFNGLMULTITEXCOORD1DPROC _glMultiTexCoord1d;
  PFNGLMULTITEXCOORD2DPROC _glMultiTexCoord2d;
  PFNGLMULTITEXCOORD3DPROC _glMultiTexCoord3d;
  PFNGLMULTITEXCOORD4DPROC _glMultiTexCoord4d;
#endif

  bool _supports_buffers;
  PFNGLGENBUFFERSPROC _glGenBuffers;
  PFNGLBINDBUFFERPROC _glBindBuffer;
  PFNGLBUFFERDATAPROC _glBufferData;
  PFNGLBUFFERSUBDATAPROC _glBufferSubData;
  PFNGLDELETEBUFFERSPROC _glDeleteBuffers;

  PFNGLBLENDEQUATIONPROC _glBlendEquation;
  PFNGLBLENDCOLORPROC _glBlendColor;

  bool _supports_framebuffer_object;
  PFNGLISRENDERBUFFEREXTPROC _glIsRenderbuffer;
  PFNGLBINDRENDERBUFFEREXTPROC _glBindRenderbuffer;
  PFNGLDELETERENDERBUFFERSEXTPROC _glDeleteRenderbuffers;
  PFNGLGENRENDERBUFFERSEXTPROC _glGenRenderbuffers;
  PFNGLRENDERBUFFERSTORAGEEXTPROC _glRenderbufferStorage;
  PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC _glGetRenderbufferParameteriv;
  PFNGLISFRAMEBUFFEREXTPROC _glIsFramebuffer;
  PFNGLBINDFRAMEBUFFEREXTPROC _glBindFramebuffer;
  PFNGLDELETEFRAMEBUFFERSEXTPROC _glDeleteFramebuffers;
  PFNGLGENFRAMEBUFFERSEXTPROC _glGenFramebuffers;
  PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC _glCheckFramebufferStatus;
  PFNGLFRAMEBUFFERTEXTURE1DEXTPROC _glFramebufferTexture1D;
  PFNGLFRAMEBUFFERTEXTURE2DEXTPROC _glFramebufferTexture2D;
#ifdef OPENGLES_2
  PFNGLFRAMEBUFFERTEXTURE3DOES _glFramebufferTexture3D;
#else
  PFNGLFRAMEBUFFERTEXTURE3DEXTPROC _glFramebufferTexture3D;
#endif
  PFNGLFRAMEBUFFERTEXTUREARBPROC _glFramebufferTexture;
  PFNGLFRAMEBUFFERTEXTURELAYERPROC _glFramebufferTextureLayer;
  PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC _glFramebufferRenderbuffer;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC _glGetFramebufferAttachmentParameteriv;
  PFNGLGENERATEMIPMAPEXTPROC _glGenerateMipmap;
  PFNGLBINDPROGRAMARBPROC _glBindProgram;

  bool _supports_framebuffer_multisample;
  bool _supports_framebuffer_multisample_coverage_nv;
  INLINE bool get_supports_framebuffer_multisample();
  INLINE bool get_supports_framebuffer_multisample_coverage_nv();
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC _glRenderbufferStorageMultisample;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC _glRenderbufferStorageMultisampleCoverage;
  bool _supports_framebuffer_blit;
  INLINE bool get_supports_framebuffer_blit();
  PFNGLBLITFRAMEBUFFEREXTPROC _glBlitFramebuffer;
  PFNGLDRAWBUFFERSPROC _glDrawBuffers;
  PFNGLCLEARBUFFERFVPROC _glClearBufferfv;
  int _max_fb_samples;
  bool _supports_viewport_arrays;
  bool _supports_bindless_texture;

  PFNGLGENQUERIESPROC _glGenQueries;
  PFNGLBEGINQUERYPROC _glBeginQuery;
  PFNGLENDQUERYPROC _glEndQuery;
  PFNGLDELETEQUERIESPROC _glDeleteQueries;
  PFNGLGETQUERYIVPROC _glGetQueryiv;
  PFNGLGETQUERYOBJECTUIVPROC _glGetQueryObjectuiv;

#ifndef OPENGLES
  PFNGLQUERYCOUNTERPROC _glQueryCounter;
  PFNGLGETQUERYOBJECTI64VPROC _glGetQueryObjecti64v;
  PFNGLGETQUERYOBJECTUI64VPROC _glGetQueryObjectui64v;

  PFNGLGETINTEGER64VPROC _glGetInteger64v;
#endif

  PFNGLACTIVESTENCILFACEEXTPROC _glActiveStencilFaceEXT;

#ifndef OPENGLES_1
  // GLSL functions
  PFNGLATTACHSHADERPROC _glAttachShader;
  PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation;
  PFNGLCOMPILESHADERPROC _glCompileShader;
  PFNGLCREATEPROGRAMPROC _glCreateProgram;
  PFNGLCREATESHADERPROC _glCreateShader;
  PFNGLDELETEPROGRAMPROC _glDeleteProgram;
  PFNGLDELETESHADERPROC _glDeleteShader;
  PFNGLDETACHSHADERPROC _glDetachShader;
  PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray;
  PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray;
  PFNGLGETACTIVEATTRIBPROC _glGetActiveAttrib;
  PFNGLGETACTIVEUNIFORMPROC _glGetActiveUniform;
  PFNGLGETATTRIBLOCATIONPROC _glGetAttribLocation;
  PFNGLGETPROGRAMIVPROC _glGetProgramiv;
  PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog;
  PFNGLGETSHADERIVPROC _glGetShaderiv;
  PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog;
  PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation;
  PFNGLLINKPROGRAMPROC _glLinkProgram;
  PFNGLSHADERSOURCEPROC_P _glShaderSource;
  PFNGLUSEPROGRAMPROC  _glUseProgram;
  PFNGLUNIFORM4FPROC _glUniform4f;
  PFNGLUNIFORM1IPROC _glUniform1i;
  PFNGLUNIFORM1FVPROC _glUniform1fv;
  PFNGLUNIFORM2FVPROC _glUniform2fv;
  PFNGLUNIFORM3FVPROC _glUniform3fv;
  PFNGLUNIFORM4FVPROC _glUniform4fv;
  PFNGLUNIFORM1IVPROC _glUniform1iv;
  PFNGLUNIFORM2IVPROC _glUniform2iv;
  PFNGLUNIFORM3IVPROC _glUniform3iv;
  PFNGLUNIFORM4IVPROC _glUniform4iv;
  PFNGLUNIFORMMATRIX3FVPROC _glUniformMatrix3fv;
  PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv;
  PFNGLVALIDATEPROGRAMPROC _glValidateProgram;
  PFNGLVERTEXATTRIB4FVPROC _glVertexAttrib4fv;
  PFNGLVERTEXATTRIB4DVPROC _glVertexAttrib4dv;
  PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer;
  PFNGLVERTEXATTRIBIPOINTERPROC _glVertexAttribIPointer;
  PFNGLVERTEXATTRIBLPOINTERPROC _glVertexAttribLPointer;
  PFNGLVERTEXATTRIBDIVISORPROC _glVertexAttribDivisor;
  PFNGLDRAWARRAYSINSTANCEDPROC _glDrawArraysInstanced;
  PFNGLDRAWELEMENTSINSTANCEDPROC _glDrawElementsInstanced;
#endif  // !OPENGLES_1
#ifndef OPENGLES
  PFNGLGETACTIVEUNIFORMSIVPROC _glGetActiveUniformsiv;
  PFNGLGETACTIVEUNIFORMBLOCKIVPROC _glGetActiveUniformBlockiv;
  PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC _glGetActiveUniformBlockName;
  PFNGLGENSAMPLERSPROC _glGenSamplers;
  PFNGLDELETESAMPLERSPROC _glDeleteSamplers;
  PFNGLBINDSAMPLERPROC _glBindSampler;
  PFNGLSAMPLERPARAMETERIPROC _glSamplerParameteri;
  PFNGLSAMPLERPARAMETERIVPROC _glSamplerParameteriv;
  PFNGLSAMPLERPARAMETERFPROC _glSamplerParameterf;
  PFNGLSAMPLERPARAMETERFVPROC _glSamplerParameterfv;
  PFNGLPROGRAMPARAMETERIPROC _glProgramParameteri;
  PFNGLPATCHPARAMETERIPROC _glPatchParameteri;
  PFNGLBINDTEXTURESPROC _glBindTextures;
  PFNGLBINDSAMPLERSPROC _glBindSamplers;
  PFNGLBINDIMAGETEXTUREPROC _glBindImageTexture;
  PFNGLBINDIMAGETEXTURESPROC _glBindImageTextures;
  PFNGLDISPATCHCOMPUTEPROC _glDispatchCompute;
  PFNGLMEMORYBARRIERPROC _glMemoryBarrier;
  PFNGLGETPROGRAMBINARYPROC _glGetProgramBinary;
  PFNGLGETINTERNALFORMATIVPROC _glGetInternalformativ;
  PFNGLVIEWPORTARRAYVPROC _glViewportArrayv;
  PFNGLSCISSORARRAYVPROC _glScissorArrayv;
  PFNGLDEPTHRANGEARRAYVPROC _glDepthRangeArrayv;
  PFNGLGETTEXTUREHANDLEPROC _glGetTextureHandle;
  PFNGLGETTEXTURESAMPLERHANDLEPROC _glGetTextureSamplerHandle;
  PFNGLMAKETEXTUREHANDLERESIDENTPROC _glMakeTextureHandleResident;
  PFNGLMAKETEXTUREHANDLENONRESIDENTPROC _glMakeTextureHandleNonResident;
  PFNGLUNIFORMHANDLEUI64PROC _glUniformHandleui64;
  PFNGLUNIFORMHANDLEUI64VPROC _glUniformHandleui64v;
#endif  // !OPENGLES

  GLenum _edge_clamp;
  GLenum _border_clamp;
  GLenum _mirror_repeat;
  GLenum _mirror_clamp;
  GLenum _mirror_edge_clamp;
  GLenum _mirror_border_clamp;
#ifndef OPENGLES_1
  GLsizei _instance_count;
#endif

  LightMutex _lock;
  typedef pvector<GLuint> DeletedNames;
  DeletedNames _deleted_display_lists;
  DeletedNames _deleted_queries;

#ifndef OPENGLES
  // Stores textures for which memory bariers should be issued.
  typedef pset<TextureContext*> TextureSet;
  TextureSet _textures_needing_fetch_barrier;
  TextureSet _textures_needing_image_access_barrier;
  TextureSet _textures_needing_update_barrier;
  TextureSet _textures_needing_framebuffer_barrier;
#endif

  //RenderState::SlotMask _inv_state_mask;

  bool _check_errors;
  bool _force_flush;
  bool _supports_debug;

  bool _use_object_labels;
  PFNGLOBJECTLABELPROC _glObjectLabel;

  GLuint _white_texture;

#ifndef NDEBUG
  bool _show_texture_usage;
  int _show_texture_usage_max_size;
  int _show_texture_usage_index;

  class UsageTextureKey {
  public:
    INLINE UsageTextureKey(int x_size, int y_size);
    INLINE bool operator < (const UsageTextureKey &other) const;

    int _x_size;
    int _y_size;
  };
  typedef pmap<UsageTextureKey, GLuint> UsageTextures;
  UsageTextures _usage_textures;
#endif  // NDEBUG

  BufferResidencyTracker _renderbuffer_residency;

  static PStatCollector _load_display_list_pcollector;
  static PStatCollector _primitive_batches_display_list_pcollector;
  static PStatCollector _vertices_display_list_pcollector;
  static PStatCollector _vertices_immediate_pcollector;
  static PStatCollector _memory_barrier_pcollector;
  static PStatCollector _vertex_array_update_pcollector;
  static PStatCollector _texture_update_pcollector;
  static PStatCollector _fbo_bind_pcollector;
  static PStatCollector _check_error_pcollector;

public:
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

  static TypeHandle get_class_type() {
    return _type_handle;
  }

public:
  static void init_type() {
    GraphicsStateGuardian::init_type();
    register_type(_type_handle, CLASSPREFIX_QUOTED "GraphicsStateGuardian",
                  GraphicsStateGuardian::get_class_type());
  }

private:
  static TypeHandle _type_handle;

  friend class CLP(VertexBufferContext);
  friend class CLP(IndexBufferContext);
  friend class CLP(ShaderContext);
  friend class CLP(CgShaderContext);
  friend class CLP(GraphicsBuffer);
  friend class CLP(OcclusionQueryContext);
  friend class CLP(TimerQueryContext);
};

#include "glGraphicsStateGuardian_src.I"
