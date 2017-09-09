/**
 * @file 
 * @brief This file contains implementation of simple triangle example.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#include<assert.h>
#include<math.h>

#include"examples/triangleExample.h"
#include"student/student_pipeline.h"
#include"student/linearAlgebra.h"
#include"student/uniforms.h"
#include"student/gpu.h"
#include"student/camera.h"
#include"student/vertexPuller.h"
#include"student/buffer.h"
#include"student/student_shader.h"
#include"student/mouseCamera.h"
#include"student/swapBuffers.h"

//! [EXTERNS]
///This variable contains projection matrix.
extern Mat4 projectionMatrix;//projection matrix
///This variable contains view matrix.
extern Mat4 viewMatrix      ;//view matrix
//! [EXTERNS]

//! [GLOBALS]
/**
 * @brief This structure contains all global variables for triangle example.
 */
struct TriangleExampleVariables{
  ///This value contains GPU handle.
  GPU            gpu     ;//gpu handle
  ///This variable contains shader program.
  ProgramID      program ;//program id
  ///This variable contains vertex puller.
  VertexPullerID puller  ;//puller id
  ///This variable contains buffer with vertex positions.
  BufferID       vertices;//buffer id
}triangleExample;///< instance of all global variables for triangle example.
//! [GLOBALS]

/**
 * @brief This function represents vertex shader for triangle example.
 *
 * @param output output vertex
 * @param input input vertex
 * @param gpu GPU handle
 */
void triangleExample_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  assert(output != NULL);
  assert(input  != NULL);
  assert(gpu    != NULL);

  //! [INTERPRET_UNIFORMS]
  //get handle to all uniforms
  Uniforms const uniformsHandle = gpu_getUniformsHandle(
      gpu);//gpu

  //get uniform location of view matrix
  UniformLocation const viewMatrixLocation = getUniformLocation(
      gpu         , //gpu handle
      "viewMatrix");//name of uniform variable

  //get pointer to view matrix
  Mat4 const*const view = shader_interpretUniformAsMat4(
      uniformsHandle    , //handle to all uniforms
      viewMatrixLocation);//location of view matrix

  //get uniform location of projection matrix
  UniformLocation const projectionMatrixLocation = getUniformLocation(
      gpu               , //gpu handle
      "projectionMatrix");//name of uniform variable

  //get pointer to projection matrix
  Mat4 const*const proj = shader_interpretUniformAsMat4(
      uniformsHandle          , //handle to all uniforms
      projectionMatrixLocation);//location of projection matrix
  //! [INTERPRET_UNIFORMS]
  
  //! [INPUT_ATTRIBUTES]
  Vec3 const*const position = vs_interpretInputVertexAttributeAsVec3(
      gpu  , //gpu
      input, //input vertex
      0    );//index of vertex attribute
  //! [INPUT_ATTRIBUTES]

  //! [PROJECT]
  Mat4 mvp;
  multiply_Mat4_Mat4(&mvp,proj,view);

  Vec4 pos4;
  copy_Vec3Float_To_Vec4(&pos4,position,1.f);

  multiply_Mat4_Vec4(
      &output->gl_Position, //output - position of output vertex in clip-space
      &mvp                , //projection view matrix
      &pos4               );//position of vertex in world-space
  //! [PROJECT]
  
  //! [VS_ATTRIBUTE]
  Vec3 *const colorAttribute = vs_interpretOutputVertexAttributeAsVec3(
      gpu   , //gpu
      output, //output vertex
      0     );//index of vertex attribute
  //! [VS_ATTRIBUTE]
  //! [VS_COLOR]
  init_Vec3(
      colorAttribute                  , //pointer to vertex attribute
      (float)(input->gl_VertexID == 0), //zeroth vertex will be red
      (float)(input->gl_VertexID == 1), //first vertex will be green
      (float)(input->gl_VertexID == 2));//second vertex will be blue
  //! [VS_COLOR]
}

/**
 * @brief This function represents fragment shader for triangle example.
 *
 * @param output output fragment
 * @param input input fragment
 * @param gpu GPU handle
 */
void triangleExample_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){
  assert(output != NULL);
  assert(input  != NULL);
  assert(gpu    != NULL);

  //! [FS_ATTRIBUTE]
  Vec3 const*colorAttribute = fs_interpretInputAttributeAsVec3(
      gpu  , //gpu
      input, //input fragment
      0    );//index of fragment attribute
  //! [FS_ATTRIBUTE]

  //! [FS_COLOR]
  copy_Vec3Float_To_Vec4(
      &output->color, //output fragment color
      colorAttribute, //computed color
      1.f           );//alpha
  //! [FS_COLOR]
}

void triangleExample_onInit(int32_t width,int32_t height){
  //! [INITIALIZATION]
  triangleExample.gpu = cpu_createGPU();

  cpu_setViewportSize(
      triangleExample.gpu, //gpu
      (size_t)width      , //width of screen/framebuffer in pixels
      (size_t)height     );//height of screen/framebuffer in pixels

  cpu_initMatrices(width,height);
  //! [INITIALIZATION]

  //! [RESERVE]
  cpu_reserveUniform(
      triangleExample.gpu, //gpu
      "projectionMatrix" , //uniform name
      UNIFORM_MAT4       );//uniform type

  cpu_reserveUniform(
      triangleExample.gpu, //gpu
      "viewMatrix"       , //uniform name
      UNIFORM_MAT4       );//uniform type
  //! [RESERVE]

  //! [CREATE_PROGRAM]
  triangleExample.program = cpu_createProgram(
      triangleExample.gpu);//gpu
  //! [CREATE_PROGRAM]
  //! [ATTACH]
  cpu_attachVertexShader  (
      triangleExample.gpu           , //gpu
      triangleExample.program       , //program id
      triangleExample_vertexShader  );//pointer to function that represents vertex shader

  cpu_attachFragmentShader(
      triangleExample.gpu           , //gpu
      triangleExample.program       , //program id
      triangleExample_fragmentShader);//pointer to function that represents fragment shader
  //! [ATTACH]
  //! [INTERPOLATION]
  cpu_setAttributeInterpolation(
      triangleExample.gpu    , //gpu
      triangleExample.program, //program id
      0                      , //vertex attribute index
      ATTRIB_VEC3,SMOOTH     );//interpolation type - with perspective correction
  //! [INTERPOLATION]

  //! [BUFFER]
  cpu_createBuffers(
      triangleExample.gpu      , //gpu
      1                        , //number of buffer ids that will be reserved
      &triangleExample.vertices);//pointer to buffer id variable

  float const positions[9] = {//vertex positions
    -1.f,-1.f,+0.f,//triangle vertex A
    +1.f,-1.f,+0.f,//triangle vertex B
    -1.f,+1.f,+0.f,//triangle vertex C
  };

  cpu_bufferData(
      triangleExample.gpu     , //gpu
      triangleExample.vertices, //buffer id
      sizeof(float)*9         , //size of data that is going to be copied to buffer
      positions               );//pointer to data
  //! [BUFFER]

  //! [PULLER]
  cpu_createVertexPullers(
      triangleExample.gpu    , //gpu
      1                      , //number of puller ids that will be reserved
      &triangleExample.puller);//pointer to puller id variable
  //! [PULLER]
  
  //! [HEAD]
  cpu_setVertexPullerHead(
      triangleExample.gpu     , //gpu
      triangleExample.puller  , //puller id
      0                       , //id of head/vertex attrib
      triangleExample.vertices, //buffer id
      sizeof(float)*0         , //offset
      sizeof(float)*3         );//stride

  cpu_enableVertexPullerHead(
      triangleExample.gpu   , //gpu
      triangleExample.puller, //puller id
      0                     );//id of head/vertex attrib
  //! [HEAD]
}

void triangleExample_onExit(){
  //! [FREE]
  cpu_destroyGPU(triangleExample.gpu);
  //! [FREE]
}

void triangleExample_onDraw(SDL_Surface*surface){
  assert(surface != NULL);

  //! [CLEAR]
  float const depth = (float)(+INFINITY);//infinity depth
  cpu_clearDepth(triangleExample.gpu,depth);

  Vec4 const color = {{.1f,.1f,.1f,.1f}};//dark gray color
  cpu_clearColor(triangleExample.gpu,&color);
  //! [CLEAR]

  //! [USE_PROGRAM]
  cpu_useProgram(
      triangleExample.gpu    , //gpu
      triangleExample.program);//program id
  //! [USE_PROGRAM]
  //! [BIND_PULLER]
  cpu_bindVertexPuller(
      triangleExample.gpu   , //gpu
      triangleExample.puller);//program id
  //! [BIND_PULLER]
  //! [SET_UNIFORMS]
  UniformLocation const viewMatrixUniform = getUniformLocation(
      triangleExample.gpu, //gpu
      "viewMatrix"       );//name of uniform variable

  cpu_uniformMatrix4fv(
      triangleExample.gpu, //gpu
      viewMatrixUniform  , //location of uniform variable
      (float*)&viewMatrix);//pointer to data

  UniformLocation const projectionMatrixUniform = getUniformLocation(
      triangleExample.gpu, //gpu
      "projectionMatrix" );//name of uniform variable

  cpu_uniformMatrix4fv(
      triangleExample.gpu      , //gpu
      projectionMatrixUniform  , //location of uniform variable
      (float*)&projectionMatrix);//pointer to data of
  //! [SET_UNIFORMS]

  //! [DRAW]
  cpu_drawTriangles(
      triangleExample.gpu, //gpu
      3                  );//number of vertices
  //! [DRAW]

  //! [SWAP]
  cpu_swapBuffers(surface,triangleExample.gpu);
  //! [SWAP]
}

/**
 * \example TriangleExample triangleExample.c
 * \image html images/triangle.png "The figure shows the output of the triangle example." width=10cm
 * If rendering pipeline works, this example renders one colored triangle. <br/>
 * \section Initialization Initialization
 * All global variables are stored in one place - in a structure.
 * \snippet triangleExample.c GLOBALS
 * View and projection matrices are stored in extern variables:
 * \snippet triangleExample.c EXTERNS
 * When a mouse is moving, these matrices are recomputed.<br/>
 * When example is started, function triangleExample_onInit() is called.
 * This function creates gpu, initializes viewport and initializes matrices:
 * \snippet triangleExample.c INITIALIZATION
 * Then, the function reserves uniforms on gpu:
 * \snippet triangleExample.c RESERVE
 * Next, the function creates shader program
 * \snippet triangleExample.c CREATE_PROGRAM
 * Shaders has to be attach to program:
 * \snippet triangleExample.c ATTACH
 * One vertex attribute (color) is interpolated during rasterization, we have to set its type, location and type of interpolation:
 * \snippet triangleExample.c INTERPOLATION
 * Next, we need to create buffer with 3 vertices.
 * \snippet triangleExample.c BUFFER
 * Last thing that needs to be done in initialization is creation of vertex puller,
 * \snippet triangleExample.c PULLER
 * and setting its zeroth head
 * \snippet triangleExample.c HEAD
 * \section Deinitialization Deinitialization
 * At the end of application, all allocated resource has to be released in function triangleExample_onExit():
 * \snippet triangleExample.c FREE
 * \section Drawing Drawing
 * Function triangleExample_onDraw() is called every frame.
 * This function clears depth buffer and color buffer:
 * \snippet triangleExample.c CLEAR
 * Function then activates program:
 * \snippet triangleExample.c USE_PROGRAM
 * Then, it activates vertex puller:
 * \snippet triangleExample.c BIND_PULLER
 * Next, it has to upload data to uniform values on gpu:
 * \snippet triangleExample.c SET_UNIFORMS
 * Now, it is possible to execute draw call (3 vertices are going to be drawn):
 * \snippet triangleExample.c DRAW
 * Lastly, the function has to swap framebuffer into window surface:
 * \snippet triangleExample.c SWAP
 * \section VertexShader Vertex Shader
 * Vertex shader starts with creation of pointers to uniform matrices:
 * \snippet triangleExample.c INTERPRET_UNIFORMS
 * Next, pointer to position attribute is created:
 * \snippet triangleExample.c INPUT_ATTRIBUTES
 * Then, it projects vertex position into clip-space:
 * \snippet triangleExample.c PROJECT
 * Then, it gets pointer handle to zeroth output vertex attribute:
 * \snippet triangleExample.c VS_ATTRIBUTE
 * Finaly, it writes generated color into this output vertex attribute:
 * \snippet triangleExample.c VS_COLOR
 * \section FragmentShader Fragment Shader
 * Fragment shader starts with creation of pointers to zeroth input fragment attribute:
 * \snippet triangleExample.c FS_ATTRIBUTE
 * This attribute contains interpolated color.
 * At the end, fragment shader writes color to output fragment.
 * \snippet triangleExample.c FS_COLOR
 */
