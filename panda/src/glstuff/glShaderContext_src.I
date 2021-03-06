/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file glShaderContext_src.I
 * @author jyelon
 * @date 2005-09-01
 */

/**
 * Returns true if the shader is "valid", ie, if the compilation was
 * successful.  The compilation could fail if there is a syntax error in the
 * shader, or if the current video card isn't shader-capable, or if no shader
 * languages are compiled into panda.
 */
INLINE bool CLP(ShaderContext)::
valid() {
  if (_shader->get_error_flag()) return false;
  if (_shader->get_language() != Shader::SL_GLSL) {
    return false;
  }
  if (_glsl_program != 0) {
    return true;
  }
  return false;
}

/**
 * Returns true if the shader may need to access standard vertex attributes as
 * passed by glVertexPointer and the like.
 */
INLINE bool CLP(ShaderContext)::
uses_standard_vertex_arrays() {
  return _uses_standard_vertex_arrays;
}

/**
 * Always true, for now.
 */
INLINE bool CLP(ShaderContext)::
uses_custom_vertex_arrays() {
  return true;
}
