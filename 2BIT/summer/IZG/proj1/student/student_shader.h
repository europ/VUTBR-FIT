/*!
 * @file 
 * @brief This file contains function declarations of vertex and fragment shader for phong shading/lighting.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include"student/fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function represents vertex shader for phong lighting/shading.
 *
 * @param output output vertex
 * @param input  input vertex
 * @param gpu GPU handle
 */
void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   );

/**
 * @brief This function represents fragment shader for phong lighting/shading.
 *
 * @param output output fragment
 * @param input input fragment
 * @param gpu GPU handle
 */
void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   );

#ifdef __cplusplus
}
#endif
