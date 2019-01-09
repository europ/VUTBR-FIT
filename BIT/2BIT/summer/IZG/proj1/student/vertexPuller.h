/*!
 * @file 
 * @brief This file contains structures and function declarations important for vertex puller configuration.
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 * The main purpose of vertex puller is construction of vertices that are sent to vertex shader.
 * The vertex puller is composed of reading heads.
 * Each reading head constructs one vertex attribute.
 * A buffer, offset and stride are assigned to each reading head.
 * A reading head could be enable or disabled.
 *
 * \verbatim
Example:

A Vertex is composed of two vertex attributes: position and uv texture coordinates.
The position attribute has 3 components of type float.
The coord attribute has 2 components of type double.
Both attributes are stored in same buffer with some padding.

Notation:
P0x - x component of position of vertex 0      (gl_VertexID == 0)
P3z - z component of position of vertex 3      (gl_VertexID == 3)
C0u - u component of texture coord of vertex 0 (gl_VertexID == 0)
C8v - v component of texture coord of vertex 8 (gl_VertexID == 8)
000 - empty space (one uint8_t)
                             
              offset of coord                stride of coord        
        │◃─────────────────────────▹│◃─────────────────────────────▹│
        │                           │                               │
        ┌───────────────────────────────────────────────────────────────────────
buffer: │000│000│000│P0x│P0y│P0z│000│C0u│C0v│000│000│P1x│P1y│P1z│000│C1u│C1v│...
        └────────────────────────────────────────────────────────────────────────
        │           │                               │
        │ offset of │                               │
        │ position  │      stride of position       │
        │◃─────────▹│◃─────────────────────────────▹│

The vertex puller is composed of two enabled reading heads.

The reading head settings for position:
offset = sizeof(uint8_t)*3
stride = sizeof(float)*3 + sizeof(uint8_t) + sizeof(double)*2 + 2*sizeof(uint8_t)

The reading head settings for coord:
offset = offset of position + sizeof(float)*3 + sizeof(uint8_t)
stride = stride of position
\endverbatim
 *
 *
 */
#pragma once

#include<stdlib.h>

#include"fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This struct represents configuration for one reading head of vertex puller.
 *
 * A vertex is composed of N vertex attributes (typically position, normal, texture coord, ...).
 * A vertex attribute is composed of N components (1,2,3,4).
 * Type of each component is the same.
 * Each vertex has different data in its attributes (position of each vertex is different).
 * The data for vertex attributes are stored in buffers.
 */
struct GPUVertexPullerHead{
  size_t      offset ;///< offset in bytes where this attribute starts for first vertex
  size_t      stride ;///< distance in bytes between attributes for two consecutive vertices
  int         enabled;///< is this attribute enabled?
  void const* buffer ;///< buffer that contains this attribute
};

/**
 * @brief represents configuration for vertex puller
 */
struct GPUVertexPullerConfiguration{
  GPUVertexPullerHead heads[MAX_ATTRIBUTES];///< reading heads for each vertex attribute
  VertexIndex const*  indices              ;///< indices to vertices, if it is NULL -> indexing is not used
};

/**
 * @brief This struct represents output of vertex puller - vertex.
 *
 * An attribute could be set to zero pointer - this attribute must not be used in vertex shader
 * If an attribute is not set to zero pointer, it points to location of data for vertex attribute.
 * It is programmers duty to reinerpret data to suitable format.
 */
struct GPUVertexPullerOutput{
  void const* attributes[MAX_ATTRIBUTES];///< list of attributes
};

/**
 * @brief This function reserve vertex puller ids on GPU.
 *
 * Vertex puller id points to vertex puller configuration on GPU
 * Alternative function in OpenGL is <a href="https://www.opengl.org/sdk/docs/man4/html/glCreateVertexArrays.xhtml">glCreateVertexArrays</a>.
 *
 * @param gpu GPU handler
 * @param n number of vertex array ids that will be reserved
 * @param arrays resulting vertex array ids
 */
void cpu_createVertexPullers(
    GPU             const gpu   ,
    size_t          const n     ,
    VertexPullerID *const arrays);

/**
 * @brief This function sets vertex attribute in vertex puller.
 *
 * Its alternative functions in OpenGL is <a href="https://www.opengl.org/sdk/docs/man4/html/glBindVertexBuffer.xhtml">glVertexArrayVertexBuffer</a>.
 *
 * This function sets vertex attribute in vertex puller, \see GPUVertexPullerHead GPUVertexPullerConfiguration
 *
 * @param gpu GPU handler
 * @param puller id of vertex puller
 * @param headIndex index of reading head in vertex puller configuration
 * @param buffer id of buffer
 * @param offset offset in bytes
 * @param stride stride in bytes
 */
void cpu_setVertexPullerHead(
    GPU            const gpu      ,
    VertexPullerID const puller   ,
    size_t         const headIndex,
    BufferID       const buffer   ,
    size_t         const offset   ,
    size_t         const stride   );

/**
 * @brief This function sets indexing in vertex puller.
 *
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexArrayElementBuffer.xhtml">glVertexArrayElementBuffer</a>.
 * Indexing is used in indexed drawing (drawElements).
 *
 * \image html images/drawElements.svg "Difference between indexed and non-indexed drawing" width=10cm
 *
 * @param gpu GPU handler
 * @param puller id of vertex puller configuration
 * @param buffer id of buffer that contains indices
 * @param indexSize size in bytes of one index (1,2,4 are allowed)
 */
void cpu_setIndexing(
    GPU            const gpu      ,
    VertexPullerID const puller   ,
    BufferID       const buffer   ,
    size_t         const indexSize);

/**
 * @brief This function enables vertex puller head.
 *
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glEnableVertexAttribArray.xhtml">glEnableVertexArrayAttrib</a>.
 *
 * @param gpu    GPU handler
 * @param puller id of vertex puller configuration
 * @param head   index of vertex puller head
 */
void cpu_enableVertexPullerHead(
    GPU            const gpu   ,
    VertexPullerID const puller,
    size_t         const head  );
 
/**
 * @brief This function disables vertex puller head.
 *
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glEnableVertexAttribArray.xhtml">glDisableVertexArrayAttrib</a>.
 *
 * @param gpu GPU handler
 * @param puller id of vertex puller configuration
 * @param head   vertex attribute index
 */
void cpu_disableVertexPullerHead(
    GPU            const gpu   ,
    VertexPullerID const puller,
    size_t         const head  );

/**
 * @brief This function activates vertex puller.
 *
 * \verbatim
 * Example:
 * //setup puller
 *
 * cpu_bindVertexPuller(gpu,puller);
 * //draw
 * cpu_bindVertexPuller(gpu,0);
 * \endverbatim
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindVertexArray.xhtml">glBindVertexArray</a>.
 *
 * @param gpu GPU handler
 * @param id index to vertex puller description
 */
void cpu_bindVertexPuller(
    GPU            const gpu,
    VertexPullerID const id );

#ifdef __cplusplus
}
#endif
