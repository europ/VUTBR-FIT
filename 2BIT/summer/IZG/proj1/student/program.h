/*!
 * @file 
 * @brief This file contains structures and function declarations important for shader program object.
 *
 * A shader program is collection of shaders (vertex shader, fragment shader).
 * Shaders are represent by callbacks (function pointers).
 * In common graphics APIs, shaders are compiled a programs are linked.
 * Each program has its unique ID - ProgramID. This id can be reserved/obtained by cpu_createProgram.
 * A program can be delete by cpu_deleteProgram.
 * Shaders has to be attached to a program before it can be properly used on GPU.
 * Function cpu_useProgram has to be called before any draw call can be executed.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#pragma once

#include<stdlib.h>

#include"fwd.h"
#include"linearAlgebra.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This enum represents type of vertex/fragment attributes
 */
enum AttributeType{
  ATTRIB_FLOAT = 1u,///< 32-bit float 
  ATTRIB_VEC2  = 2u,///< 2d vector of 32-bit floats
  ATTRIB_VEC3  = 3u,///< 3d vector of 32-bit floats
  ATTRIB_VEC4  = 4u,///< 4f vector of 32-bit floats
  ATTRIB_EMPTY = 5u,///< empty attribute
};

/**
 * @brief Type of interpolation that is done in rasterization
 */
enum InterpolationType{
  FLAT         ,///< flat interpolation, fragment data are generated using only first vertex of primitive
  NOPERSPECTIVE,///< linear interpolation without perspective correction
  SMOOTH       ,///< linear interpolation with perspective correction
};

typedef enum AttributeType     AttributeType    ;///< shortcut
typedef enum InterpolationType InterpolationType;///< shortcut

/**
 * @brief This function interprets vertex attribute of input vertex of vertex shader as float.
 *
 * @param gpu GPU handle
 * @param vertex vertex shader input - output of vertex puller
 * @param attributeIndex attribute index
 *
 * @return pointer to const float
 */
float const* vs_interpretInputVertexAttributeAsFloat(
    GPU                        const gpu           ,
    GPUVertexShaderInput const*const vertex        ,
    AttribIndex                const attributeIndex);

/**
 * @brief This function interprets vertex attribute of input vertex of vertex shader as vec2.
 *
 * @param gpu GPU handle
 * @param vertex vertex shader input - output of vertex puller
 * @param attributeIndex attribute index
 *
 * @return pointer to const vec2
 */
Vec2  const* vs_interpretInputVertexAttributeAsVec2 (
    GPU                        const gpu           ,        
    GPUVertexShaderInput const*const vertex        ,
    AttribIndex                const attributeIndex);

/**
 * @brief This function interprets vertex attribute of input vertex of vertex shader as vec3.
 *
 * @param gpu GPU handle
 * @param vertex vertex shader input - output of vertex puller
 * @param attributeIndex attribute index
 *
 * @return pointer to const vec3
 */
Vec3  const* vs_interpretInputVertexAttributeAsVec3 (
    GPU                        const gpu           ,
    GPUVertexShaderInput const*const vertex        ,
    AttribIndex                const attributeIndex);

/**
 * @brief This function interprets vertex attribute of input vertex of vertex shader as vec4.
 *
 * @param gpu GPU handle
 * @param vertex vertex shader input - output of vertex puller output
 * @param attributeIndex attribute index
 *
 * @return pointer to const vec4
 */
Vec4  const* vs_interpretInputVertexAttributeAsVec4 (
    GPU                        const gpu           ,
    GPUVertexShaderInput const*const vertex        ,
    AttribIndex                const attributeIndex);

/**
 * @brief This function interprets vertex attribute of output vertex of vertex shader as float.
 *
 * @param gpu GPU handle
 * @param vertex output vertex of vertex shader
 * @param attributeIndex attribute index
 *
 * @return pointer to float
 */
float* vs_interpretOutputVertexAttributeAsFloat(
    GPU                    const gpu           ,
    GPUVertexShaderOutput *const vertex        ,
    AttribIndex            const attributeIndex);

/**
 * @brief This function interprets vertex attribute of output vertex of vertex shader as vec2.
 *
 * @param gpu GPU handle
 * @param vertex output vertex of vertex shader
 * @param attributeIndex attribute index
 *
 * @return pointer to vec2
 */
Vec2 * vs_interpretOutputVertexAttributeAsVec2 (
    GPU                    const gpu           ,
    GPUVertexShaderOutput *const vertex        ,
    AttribIndex            const attributeIndex);

/**
 * @brief This function interprets vertex attribute of output vertex of vertex shader as vec3.
 *
 * @param gpu GPU handle
 * @param vertex output vertex of vertex shader
 * @param attributeIndex attribute index
 *
 * @return pointer to vec3
 */
Vec3 * vs_interpretOutputVertexAttributeAsVec3 (
    GPU                    const gpu           ,
    GPUVertexShaderOutput *const vertex        ,
    AttribIndex            const attributeIndex);

/**
 * @brief This function interpres vertex attribute of output vertex of vertex shader as vec4.
 *
 * @param gpu GPU handle
 * @param vertex output vertex of vertex vertexShader
 * @param attributeIndex attribute index
 *
 * @return pointer to vec4
 */
Vec4 * vs_interpretOutputVertexAttributeAsVec4 (
    GPU                    const gpu           ,
    GPUVertexShaderOutput *const vertex        ,
    AttribIndex            const attributeIndex);

/**
 * @brief This function interprets fragment attribute of input fragment of fragment shader as float.
 *
 * @param gpu GPU handle
 * @param fragment fragment shader input - output of rasterization
 * @param attributeIndex attribute index
 *
 * @return pointer to const float
 */
float const* fs_interpretInputAttributeAsFloat(
    GPU                          const gpu           ,
    GPUFragmentShaderInput const*const fragment      ,
    AttribIndex                  const attributeIndex);

/**
 * @brief This function interprets fragment attribute of input fragment of fragment shader as vec2.
 *
 * @param gpu GPU handle
 * @param fragment fragment shader input - output of rasterization
 * @param attributeIndex attribute index
 *
 * @return pointer to const vec2
 */
Vec2  const* fs_interpretInputAttributeAsVec2 (
    GPU                          const gpu           ,        
    GPUFragmentShaderInput const*const fragment      ,
    AttribIndex                  const attributeIndex);

/**
 * @brief This function interprets fragment attribute of input fragment of fragment shader as vec3.
 *
 * @param gpu GPU handle
 * @param fragment fragment shader input - output of raterization
 * @param attributeIndex attribute index
 *
 * @return pointer to const vec3
 */
Vec3  const* fs_interpretInputAttributeAsVec3 (
    GPU                          const gpu           ,
    GPUFragmentShaderInput const*const fragment      ,
    AttribIndex                  const attributeIndex);

/**
 * @brief This function interprets fragment attribute of input fragment of fragment shader as vec4.
 *
 * @param gpu GPU handle
 * @param fragment fragment shader input - output of rasterization
 * @param attributeIndex attribute index
 *
 * @return pointer to const vec4
 */
Vec4  const* fs_interpretInputAttributeAsVec4 (
    GPU                          const gpu           ,
    GPUFragmentShaderInput const*const fragment      ,
    AttribIndex                  const attributeIndex);

/**
 * @brief This struct represents input interface of vertex shader.
 */
struct GPUVertexShaderInput{
  GPUVertexPullerOutput const*attributes ;///< read only attributes
  VertexIndex                 gl_VertexID;///< vertex id
};


/**
 * @brief This struct represents fragment that is produced by rasterization.
 * Each fragment contains fragment attributes.
 * Each attribute is represented as array of bytes that has to be cast to requested type.
 */
struct GPUFragmentAttributes{
  AttributeData attributes[MAX_ATTRIBUTES];///< attributes
};

/**
 * @brief This struct represents input data to fragment shader.
 */
struct GPUFragmentShaderInput{
  GPUFragmentAttributes attributes;///< fragment attributes
  Vec2                  coords    ;///< screenspace coords
  float                 depth     ;///< depth of fragment
};

/**
 * @brief This struct represents output data of fragment shader.
 */
struct GPUFragmentShaderOutput{
  Vec4  color;///< color of the fragment
  float depth;///< depth of the fragment
};

/**
 * @brief This struct represents vertex that is output of vertex shader.
 */
struct GPUVertexShaderOutput{
  GPU           gpu                       ;///< gpu handle
  AttributeData attributes[MAX_ATTRIBUTES];///< attributes
  Vec4          gl_Position               ;///< position of vertex in clip-space
};

/**
 * @brief This function reserves id for new program.
 *
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/">glCreateProgram</a>.
 *
 * @param gpu GPU handler
 *
 * @return return id of program (0 i reserved for empty program)
 */
ProgramID cpu_createProgram(
    GPU const gpu);

/**
 * @brief This function deletes program.
 *
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/">glDeleteProgram</a>.
 *
 * @param gpu GPU handler
 * @param program id of program
 */
void cpu_deleteProgram(
    GPU       const gpu    ,
    ProgramID const program);

/**
 * @brief This function attachs vertex shader to program.
 *
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/">glAttachShader</a>.
 *
 * @param gpu GPU handler
 * @param program id of program
 * @param shader function pointer to vertex shader
 */
void cpu_attachVertexShader(
    GPU          const gpu    ,
    ProgramID    const program,
    VertexShader const shader );

/**
 * @brief This function attachs fragment shader to program.
 *
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/">glAttachShader</a>.
 *
 * @param gpu GPU handler
 * @param program id of program
 * @param shader function pointer to fragment shader
 */
void cpu_attachFragmentShader(
    GPU            const gpu    ,
    ProgramID      const program,
    FragmentShader const shader );

/**
 * @brief This function activates selected program.
 *
 * This function has to be called with valid program id before any draw call can be executed.
 * Its alternative function in OpenGL is <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/">glUseProgram</a>.
 *
 * @param gpu GPU handler
 * @param program id of program
 */
void cpu_useProgram(
    GPU       const gpu    ,
    ProgramID const program);

/**
 * @brief This function sets interpolation of vertex attributes of output vertex.
 *
 * Vertex attributes are interpolated into fragment attributes during rasterization.
 * This function does not exist in OpenGL - shader source analysis does its work automatically.
 *
 * @param gpu GPU handler
 * @param program id of program
 * @param attribIndex index of attribute
 * @param type type of attribute 
 * @param interpolation type of interpolation in rasterization for this vertex attribute
 */
void cpu_setAttributeInterpolation(
    GPU               const gpu          ,
    ProgramID         const program      ,
    AttribIndex       const attribIndex  ,
    AttributeType     const type         ,
    InterpolationType const interpolation);

#ifdef __cplusplus
}
#endif
