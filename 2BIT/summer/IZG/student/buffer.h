/*!
 * @file 
 * @brief This file contains function declarations that are needed for manipulation with buffers on GPU.
 *
 * A buffer is linear memory on GPU.
 * It can be used for storing vertices and indices.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include<stdlib.h>
#include<stdint.h>
#include"gpu.h"
#include"fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function reserve buffer ids on GPU.
 *
 * Its alternative function in OpenGL is <a href="https://www.opengl.org/sdk/docs/man4/html/glCreateBuffers.xhtml">glCreateBuffers</a>
 *
 * @param gpu GPU handler
 * @param n number of buffer ids that will be reserved
 * @param buffers resulting buffers ids
*/
void cpu_createBuffers(
    GPU       const gpu    ,
    size_t    const n      ,
    BufferID *const buffers);

/**
 * @brief This function allocates/reallocates and uploads data to specific buffer on GPU.
 *
 * Its alternative function in OpenGL is <a href="https://www.opengl.org/sdk/docs/man4/html/glBufferData.xhtml">glNamedBufferData</a>
 *
 * @param gpu GPU handler
 * @param buffer buffer id
 * @param size size in bytes
 * @param data optional data pointer, if data==0 then no data is copied to GPU
 */
void cpu_bufferData(
    GPU             const gpu   ,
    BufferID        const buffer,
    size_t          const size  ,
    void      const*const data  );

#ifdef __cplusplus
}
#endif
