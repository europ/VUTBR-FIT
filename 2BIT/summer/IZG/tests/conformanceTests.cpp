#include<iostream>

#include"tests/conformanceTests.h"

#include"student/student_cpu.h"
#include"student/student_pipeline.h"
#include"student/student_shader.h"
#include"student/camera.h"
#include"student/mouseCamera.h"
#include"student/linearAlgebra.h"
#include"student/program.h"
#include"student/uniforms.h"

#define CATCH_CONFIG_RUNNER
#include"3rdParty/catch.hpp"

char const*groundTruthFile = 0;

bool equalFloats(float const&a,float const&b){
  return fabs(a-b)<=0.001f;
}

GPUVertexShaderOutput const*outputs[3];
GPUVertexShaderInput        inputs [3];
GPU                         gpus   [3];
GPUVertexPullerOutput       pullerOutputs[3];
size_t vsInvocationCounter = 0;

// dummy vertex shader for testing
void vs_test(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  if(vsInvocationCounter < 3){
    //write output
    outputs[vsInvocationCounter] = output;
    //write input pointer
    memcpy(
        inputs+vsInvocationCounter,
        input,
        sizeof(GPUVertexShaderInput));
    //write input attributes
    memcpy(
        pullerOutputs+vsInvocationCounter,
        input->attributes,
        sizeof(GPUVertexPullerOutput));
    //write gpu handler
    gpus   [vsInvocationCounter] = gpu   ;
  }
  //increment invocation counter
  vsInvocationCounter++;
}


#if !defined(SOLUTION)

TEST_CASE("gpu_computeGLVertexID should compute gl_VertexID."){
  WHEN(" using indexing"){
    VertexIndex const indices[] = {0,1,2,2,1,3};
    REQUIRE(gpu_computeGLVertexID(indices,0) == 0);
    REQUIRE(gpu_computeGLVertexID(indices,1) == 1);
    REQUIRE(gpu_computeGLVertexID(indices,2) == 2);
    REQUIRE(gpu_computeGLVertexID(indices,3) == 2);
    REQUIRE(gpu_computeGLVertexID(indices,4) == 1);
    REQUIRE(gpu_computeGLVertexID(indices,5) == 3);
  }
  WHEN(" not using indexing"){
    REQUIRE(gpu_computeGLVertexID(NULL,0) == 0);
    REQUIRE(gpu_computeGLVertexID(NULL,1) == 1);
    REQUIRE(gpu_computeGLVertexID(NULL,2) == 2);
    REQUIRE(gpu_computeGLVertexID(NULL,3) == 3);
    REQUIRE(gpu_computeGLVertexID(NULL,4) == 4);
    REQUIRE(gpu_computeGLVertexID(NULL,5) == 5);
  }
}

TEST_CASE("gpu_computeVertexAttributeDataPointer should compute data pointer for vertex attribute."){
  GPUVertexPullerHead head;
  head.offset  = 13;
  head.stride  = 20;
  head.enabled = 1;
  head.buffer  = (void*)1000;
  REQUIRE((size_t)gpu_computeVertexAttributeDataPointer(&head,444) == 9893);
}

TEST_CASE("gpu_runVertexPuller should construct vertex and fill vertex attributes."){
  GPUVertexPullerConfiguration puller;
  VertexIndex const indices[] = {0,1,4,4,1,5,1,2,5,5,2,6,2,3,6,6,3,7};
  puller.indices = indices;
  for(size_t a = 0; a < MAX_ATTRIBUTES; ++a)
    puller.heads[a].enabled = 0;
  puller.heads[0].offset  = 7;
  puller.heads[0].stride  = 21;
  puller.heads[0].enabled = 1;
  puller.heads[0].buffer  = (void*)100;

  puller.heads[2].offset  = 88;
  puller.heads[2].stride  = 99;
  puller.heads[2].enabled = 1;
  puller.heads[2].buffer  = (void*)10000;

  puller.heads[3].offset  = 15;
  puller.heads[3].stride  = 77;
  puller.heads[3].enabled = 1;
  puller.heads[3].buffer  = (void*)1000;

  GPUVertexPullerOutput vertex;

  gpu_runVertexPuller(&vertex,&puller,11);

  REQUIRE((size_t)vertex.attributes[0] == 233  );
  REQUIRE((size_t)vertex.attributes[1] == 0    );
  REQUIRE((size_t)vertex.attributes[2] == 10682);
  REQUIRE((size_t)vertex.attributes[3] == 1477 );
}

TEST_CASE("gpu_runPrimitiveAssembly should construct primitive."){
  GPU gpu = (GPU)13;
  GPUPrimitive primitive;
  primitive.nofUsedVertices = 0;
  size_t const nofPrimitiveVertices = 3;
  GPUVertexPullerConfiguration puller;
  VertexIndex const indices[8] = {0,0,0,0,0,3,12,31};
  puller.indices          = indices;
  puller.heads[0].buffer  = (void*)123;
  puller.heads[0].stride  = 32;
  puller.heads[0].offset  = 10000;
  puller.heads[0].enabled = 1;
  puller.heads[1].buffer  = (void*)3;
  puller.heads[1].stride  = 2;
  puller.heads[1].offset  = 10;
  puller.heads[1].enabled = 1;
  for(size_t a=2;a<MAX_ATTRIBUTES;++a){
    puller.heads[a].buffer  = (void*)0;
    puller.heads[a].stride  = 0;
    puller.heads[a].offset  = 0;
    puller.heads[a].enabled = 0;
  }

  VertexShaderInvocation const baseInvocation = 5;
  vsInvocationCounter = 0;

  gpu_runPrimitiveAssembly(
      gpu,
      &primitive,
      nofPrimitiveVertices,
      &puller,
      baseInvocation,
      vs_test);

  REQUIRE(vsInvocationCounter == 3);
  REQUIRE(primitive.nofUsedVertices == 3);
  REQUIRE(gpus[0] == gpu);
  REQUIRE(gpus[1] == gpu);
  REQUIRE(gpus[2] == gpu);
  REQUIRE(outputs[0] == primitive.vertices+0);
  REQUIRE(outputs[1] == primitive.vertices+1);
  REQUIRE(outputs[2] == primitive.vertices+2);

  REQUIRE(inputs[0].gl_VertexID == 3 );
  REQUIRE(inputs[1].gl_VertexID == 12);
  REQUIRE(inputs[2].gl_VertexID == 31);
  REQUIRE((size_t)pullerOutputs[0].attributes[0] == 10219);
  REQUIRE((size_t)pullerOutputs[1].attributes[0] == 10507);
  REQUIRE((size_t)pullerOutputs[2].attributes[0] == 11115);
  REQUIRE((size_t)pullerOutputs[0].attributes[1] == 19   );
  REQUIRE((size_t)pullerOutputs[1].attributes[1] == 37   );
  REQUIRE((size_t)pullerOutputs[2].attributes[1] == 75   );
  for(size_t i=2;i<MAX_ATTRIBUTES;++i){
    REQUIRE((size_t)pullerOutputs[0].attributes[i] == 0    );
    REQUIRE((size_t)pullerOutputs[1].attributes[i] == 0    );
    REQUIRE((size_t)pullerOutputs[2].attributes[i] == 0    );
  }
}

TEST_CASE("gpu_computeScreenSpaceBarycentrics should compute barycentric coordinates."){
  Vec3 coords;
  Vec2 pixelCenter;
  Vec2 vertices[VERTICES_PER_TRIANGLE];
  Vec3 lines[EDGES_PER_TRIANGLE];

  vertices[0].data[0] = 133.f;
  vertices[0].data[1] = 31.f ;
  vertices[1].data[0] = 760.f;
  vertices[1].data[1] = 433.f;
  vertices[2].data[0] = 555.f;
  vertices[2].data[1] = 777.f;
  for(size_t v=0;v<VERTICES_PER_TRIANGLE;++v)
    construct2DLine(lines+v,vertices+v,vertices+((v+1)%VERTICES_PER_TRIANGLE));

  init_Vec2(&pixelCenter,333.f,334.f);
  gpu_computeScreenSpaceBarycentrics(&coords,&pixelCenter,vertices,lines);
  REQUIRE(equalFloats(coords.data[0],0.560832f));
  REQUIRE(equalFloats(coords.data[1],0.071567f));
  REQUIRE(equalFloats(coords.data[2],0.367601f));

  init_Vec2(&pixelCenter,300.f,277.f);
  gpu_computeScreenSpaceBarycentrics(&coords,&pixelCenter,vertices,lines);
  REQUIRE(equalFloats(coords.data[0],0.638112f));
  REQUIRE(equalFloats(coords.data[1],0.069675f));
  REQUIRE(equalFloats(coords.data[2],0.292213f));
}

TEST_CASE("phong_vertexShader should project vertex to clip-space and move vertex position and normal to output."){
  Mat4 viewMatrix      ;
  Mat4 projectionMatrix;

  //init projection matrix
  perspective_Mat4(&projectionMatrix,MY_PI/2.f,1.f,1.f,1000.f);

  //init view matrix
  rotate_Mat4(&viewMatrix,0.f,1.f,0.f,1.1f);

  //create gpu
  GPU gpu = cpu_createGPU();

  //reserve uniform for view matrix
  cpu_reserveUniform(gpu,"viewMatrix"      ,UNIFORM_MAT4);

  //reserve uniform for projection matrix
  cpu_reserveUniform(gpu,"projectionMatrix",UNIFORM_MAT4);

  //upload matrices to gpu
  cpu_uniformMatrix4fv(gpu,getUniformLocation(gpu,"viewMatrix"      ),(float*)&viewMatrix      );
  cpu_uniformMatrix4fv(gpu,getUniformLocation(gpu,"projectionMatrix"),(float*)&projectionMatrix);

  //vertex data - position and normal
  Vec3 vertex[2];
  init_Vec3(vertex+0,10.f,10.f,10.f);
  init_Vec3(vertex+1,0.f,1.f,0.f);

  //create buffer
  BufferID vbo;
  cpu_createBuffers(gpu,1,&vbo);

  //upload data to buffer
  cpu_bufferData(gpu,vbo,sizeof(vertex),vertex);

  //create vertex puller
  VertexPullerID puller;
  cpu_createVertexPullers(gpu,1,&puller);

  //set position attribute
  cpu_setVertexPullerHead(gpu,puller,0,vbo,0              ,sizeof(float)*3);

  //set normal attribute
  cpu_setVertexPullerHead(gpu,puller,1,vbo,sizeof(float)*3,sizeof(float)*3);

  //activate puller
  cpu_bindVertexPuller(gpu,puller);

  //create program
  ProgramID prg = cpu_createProgram(gpu);
  cpu_attachVertexShader(gpu,prg,phong_vertexShader);

  //set interpolation
  cpu_setAttributeInterpolation(gpu,prg,0,ATTRIB_VEC3,SMOOTH);
  cpu_setAttributeInterpolation(gpu,prg,1,ATTRIB_VEC3,SMOOTH);

  //activate program
  cpu_useProgram(gpu,prg);

  //get puller configuration
  GPUVertexPullerConfiguration const*const pullerConf = gpu_getActiveVertexPuller(gpu);

  //set puller output
  GPUVertexPullerOutput pullerOutput;
  pullerOutput.attributes[0] = (uint8_t*)pullerConf->heads[0].buffer+sizeof(float)*0;
  pullerOutput.attributes[1] = (uint8_t*)pullerConf->heads[1].buffer+sizeof(float)*3;

  //set vertex shader input
  GPUVertexShaderInput vertexShaderInput;
  vertexShaderInput.attributes  = &pullerOutput;
  vertexShaderInput.gl_VertexID = 0;

  //run phong_vertexShader
  GPUVertexShaderOutput outputVertex;
  phong_vertexShader(&outputVertex,&vertexShaderInput,gpu);

  REQUIRE(equalFloats(outputVertex.gl_Position.data[0],1.3448034286e+01f));
  REQUIRE(equalFloats(outputVertex.gl_Position.data[1],1.0000000000e+01f));
  REQUIRE(equalFloats(outputVertex.gl_Position.data[2],2.3828723431e+00f));
  REQUIRE(equalFloats(outputVertex.gl_Position.data[3],4.3761134148e+00f));

  Vec3 const*const position = (Vec3*)outputVertex.attributes[0];
  Vec3 const*const normal   = (Vec3*)outputVertex.attributes[1];
  
  REQUIRE(position->data[0] == 10.f);
  REQUIRE(position->data[1] == 10.f);
  REQUIRE(position->data[2] == 10.f);

  REQUIRE(normal->data[0] == 0.f );
  REQUIRE(normal->data[1] == 1.f );
  REQUIRE(normal->data[2] == 0.f );
}

TEST_CASE("phong_fragmentShader should compute phong color."){
  //create gpu
  GPU gpu = cpu_createGPU();

  //reserve uniforms
  cpu_reserveUniform(gpu,"cameraPosition",UNIFORM_VEC3);
  cpu_reserveUniform(gpu,"lightPosition" ,UNIFORM_VEC3);

  //upload uniform data
  cpu_uniform3f(gpu,getUniformLocation(gpu,"cameraPosition"),0.f,20.f,20.f);
  cpu_uniform3f(gpu,getUniformLocation(gpu,"lightPosition" ),0.f,100.f,0.f);

  //create program
  ProgramID prg = cpu_createProgram(gpu);
  cpu_attachFragmentShader(gpu,prg,phong_fragmentShader);

  //set interpolation
  cpu_setAttributeInterpolation(gpu,prg,0,ATTRIB_VEC3,SMOOTH);
  cpu_setAttributeInterpolation(gpu,prg,1,ATTRIB_VEC3,SMOOTH);

  //activate program
  cpu_useProgram(gpu,prg);

  //init input fragment
  GPUFragmentShaderInput  inputFragment;
  inputFragment.coords.data[0] = 10.f;
  inputFragment.coords.data[1] = 10.f;
  init_Vec3((Vec3*)inputFragment.attributes.attributes[0],10.f,10.f,10.f);
  init_Vec3((Vec3*)inputFragment.attributes.attributes[1],0.f ,1.f ,0.f );
  inputFragment.depth = 0.f;

  //init output fragment
  GPUFragmentShaderOutput outputFragment;
  zero_Vec4(&outputFragment.color);

  //run fragment shader
  phong_fragmentShader(&outputFragment,&inputFragment,gpu);

  REQUIRE(equalFloats(outputFragment.color.data[0],1.7394564955e-10f));
  REQUIRE(equalFloats(outputFragment.color.data[1],9.8787826300e-01f));
  REQUIRE(equalFloats(outputFragment.color.data[2],1.7394564955e-10f));
  REQUIRE(equalFloats(outputFragment.color.data[3],1.0000000000e+00f));
}

#endif// !defined(SOLUTION)

TEST_CASE("Application should render correct image."){
  int32_t windowWidth  = 500;
  int32_t windowHeight = 500;
  char const* applicationName = "izgProjekt2016 performance test";

  // enable logging
  SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

  // initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_Init fail: %s\n",SDL_GetError());
    exit(1);
  }

  // create window
  SDL_Window*window = SDL_CreateWindow(
      applicationName       ,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      windowWidth           ,
      windowHeight          ,
      SDL_WINDOW_SHOWN      );

  if(!window){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_CreateWindow fail: %s\n",SDL_GetError());
    exit(1);
  }

  // create surface
  SDL_Surface*surface = SDL_GetWindowSurface(window);
  if(!surface){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_GetWindowSurface fail: %s\n",SDL_GetError());
    exit(1);
  }

  // create renderer
  SDL_Renderer*renderer = SDL_CreateSoftwareRenderer(surface);
  if(!renderer){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_CreateSoftwareRenderer: %s\n",SDL_GetError());
    exit(1);
  }


  phong_onInit(500,500);

  extern Mat4 viewMatrix      ;
  extern Mat4 projectionMatrix;

  viewMatrix      .column[0].data[0] = +1.0000000000e+00f;
  viewMatrix      .column[0].data[1] = +0.0000000000e+00f;
  viewMatrix      .column[0].data[2] = +0.0000000000e+00f;
  viewMatrix      .column[0].data[3] = +0.0000000000e+00f;
  viewMatrix      .column[1].data[0] = +0.0000000000e+00f;
  viewMatrix      .column[1].data[1] = +1.0000000000e+00f;
  viewMatrix      .column[1].data[2] = +0.0000000000e+00f;
  viewMatrix      .column[1].data[3] = +0.0000000000e+00f;
  viewMatrix      .column[2].data[0] = +0.0000000000e+00f;
  viewMatrix      .column[2].data[1] = +0.0000000000e+00f;
  viewMatrix      .column[2].data[2] = +1.0000000000e+00f;
  viewMatrix      .column[2].data[3] = +0.0000000000e+00f;
  viewMatrix      .column[3].data[0] = +0.0000000000e+00f;
  viewMatrix      .column[3].data[1] = +0.0000000000e+00f;
  viewMatrix      .column[3].data[2] = -1.8800077438e+00f;
  viewMatrix      .column[3].data[3] = +1.0000000000e+00f;

  projectionMatrix.column[0].data[0] = +1.0000000000e+00f;
  projectionMatrix.column[0].data[1] = +0.0000000000e+00f;
  projectionMatrix.column[0].data[2] = +0.0000000000e+00f;
  projectionMatrix.column[0].data[3] = +0.0000000000e+00f;
  projectionMatrix.column[1].data[0] = +0.0000000000e+00f;
  projectionMatrix.column[1].data[1] = +1.0000000000e+00f;
  projectionMatrix.column[1].data[2] = +0.0000000000e+00f;
  projectionMatrix.column[1].data[3] = +0.0000000000e+00f;
  projectionMatrix.column[2].data[0] = +0.0000000000e+00f;
  projectionMatrix.column[2].data[1] = +0.0000000000e+00f;
  projectionMatrix.column[2].data[2] = -1.0000199080e+00f;
  projectionMatrix.column[2].data[3] = -1.0000000000e+00f;
  projectionMatrix.column[3].data[0] = +0.0000000000e+00f;
  projectionMatrix.column[3].data[1] = +0.0000000000e+00f;
  projectionMatrix.column[3].data[2] = -2.0000198483e-01f;
  projectionMatrix.column[3].data[3] = +0.0000000000e+00f;

  SDL_LockSurface(surface);

  phong_onDraw(surface);

  SDL_UnlockSurface(surface);
  SDL_UpdateWindowSurface(window);

  SDL_Surface*groundTruth = SDL_LoadBMP(groundTruthFile);

  if(groundTruth == nullptr){
    std::cerr<<"ERROR: reference image: \""<<groundTruthFile<<"\" is corrupt!"<<std::endl;
    REQUIRE(groundTruth != nullptr);
  }

  if(
      surface->w == groundTruth->w &&
      surface->h == groundTruth->h){
    float meanSquareError = 0;
    for(int32_t y = 0; y < surface->h; ++y)
      for(int32_t x = 0; x < surface->w; ++x)
        for(int32_t c = 0; c < 3; ++c){
          float diff = fabsf((float)((uint8_t*)surface->pixels)[(y*surface->w+x)*4+c] - (float)((uint8_t*)groundTruth->pixels)[(y*groundTruth->w+x)*3+c]);
          diff *= diff;
          meanSquareError += diff;
        }

    meanSquareError /= (float)(surface->w * surface->h * 3);
    REQUIRE(meanSquareError < 40.f);
  }


  SDL_FreeSurface(groundTruth);
  
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  phong_onExit();

}

#if defined(SOLUTION)

TEST_CASE("SOLUTION_TEST: gpu_computeGLVertexID should compute gl_VertexID"){
  WHEN(" using indexing"){
    VertexIndex const ind[] = {3,10,20,20,10,30};
    REQUIRE(gpu_computeGLVertexID(ind,0) == 3 );
    REQUIRE(gpu_computeGLVertexID(ind,1) == 10);
    REQUIRE(gpu_computeGLVertexID(ind,2) == 20);
    REQUIRE(gpu_computeGLVertexID(ind,3) == 20);
    REQUIRE(gpu_computeGLVertexID(ind,4) == 10);
    REQUIRE(gpu_computeGLVertexID(ind,5) == 30);
  }
  WHEN(" not using indexing"){
    REQUIRE(gpu_computeGLVertexID(NULL,10) == 10);
    REQUIRE(gpu_computeGLVertexID(NULL,11) == 11);
    REQUIRE(gpu_computeGLVertexID(NULL,12) == 12);
    REQUIRE(gpu_computeGLVertexID(NULL,13) == 13);
    REQUIRE(gpu_computeGLVertexID(NULL,14) == 14);
    REQUIRE(gpu_computeGLVertexID(NULL,15) == 15);
  }
}

TEST_CASE("SOLUTION_TEST: gpu_computeVertexAttributeDataPointer should compute data pointer"){
  GPUVertexPullerHead head;
  head.offset  = 100;
  head.stride  = 21;
  head.enabled = 1;
  head.buffer  = (void*)110;
  REQUIRE((size_t)gpu_computeVertexAttributeDataPointer(&head,666) == 14196);
  REQUIRE((size_t)gpu_computeVertexAttributeDataPointer(&head,100) == 2310 );
  head.offset  = 10;
  head.stride  = 1;
  head.enabled = 1;
  head.buffer  = (void*)1;
  REQUIRE((size_t)gpu_computeVertexAttributeDataPointer(&head,0) == 11);
  REQUIRE((size_t)gpu_computeVertexAttributeDataPointer(&head,1) == 12);
}

TEST_CASE("SOLUTION_TEST: gpu_runVertexPuller should construct vertex"){
  GPUVertexPullerConfiguration puller;
  VertexIndex const i[] = {1,2,3,4,3,2,1,4,5,6,7,8,12,11,3,4,1,7};
  puller.indices = i;
  for(size_t a=0;a<MAX_ATTRIBUTES;++a)
    puller.heads[a].enabled = 0;
  puller.heads[0].offset  = 3;
  puller.heads[0].stride  = 2;
  puller.heads[0].enabled = 1;
  puller.heads[0].buffer  = (void*)10;

  puller.heads[1].offset  = 33;
  puller.heads[1].stride  = 13;
  puller.heads[1].enabled = 1;
  puller.heads[1].buffer  = (void*)1000;

  puller.heads[3].offset  = 5;
  puller.heads[3].stride  = 7;
  puller.heads[3].enabled = 1;
  puller.heads[3].buffer  = (void*)111;

  GPUVertexPullerOutput vertex;

  gpu_runVertexPuller(&vertex,&puller,10);

  REQUIRE((size_t)vertex.attributes[0] == 27  );
  REQUIRE((size_t)vertex.attributes[1] == 1124);
  REQUIRE((size_t)vertex.attributes[2] == 0   );
  REQUIRE((size_t)vertex.attributes[3] == 165 );

  gpu_runVertexPuller(&vertex,&puller,11);
  REQUIRE((size_t)vertex.attributes[0] == 29  );
  REQUIRE((size_t)vertex.attributes[1] == 1137);
  REQUIRE((size_t)vertex.attributes[2] == 0   );
  REQUIRE((size_t)vertex.attributes[3] == 172 );

}

TEST_CASE("SOLUTION_TEST: gpu_runPrimitiveAssembly should construct primitive"){
  GPU gpu = (GPU)17;
  GPUPrimitive primitive;
  primitive.nofUsedVertices = 0;
  size_t nofPrimitiveVertices = 3;
  GPUVertexPullerConfiguration puller;
  VertexIndex const indices[8] = {1,3,2,1,2,4,5,100};
  puller.indices = indices;
  puller.heads[0].buffer = (void*)1000;
  puller.heads[0].stride = 100;
  puller.heads[0].offset = 1000;
  puller.heads[0].enabled = 1;
  puller.heads[1].buffer = (void*)100;
  puller.heads[1].stride = 10;
  puller.heads[1].offset = 10;
  puller.heads[1].enabled = 1;
  for(size_t i=2;i<MAX_ATTRIBUTES;++i){
    puller.heads[i].buffer = (void*)0;
    puller.heads[i].stride = 0;
    puller.heads[i].offset = 0;
    puller.heads[i].enabled = 0;
  }

  VertexShaderInvocation baseInvocation = 0;
  vsInvocationCounter = 0;

  gpu_runPrimitiveAssembly(
      gpu,
      &primitive,
      nofPrimitiveVertices,
      &puller,
      baseInvocation,
      vs_test);

  REQUIRE(vsInvocationCounter == 3);
  REQUIRE(primitive.nofUsedVertices == 3);
  REQUIRE(gpus[0] == gpu);
  REQUIRE(gpus[1] == gpu);
  REQUIRE(gpus[2] == gpu);
  REQUIRE(outputs[0] == primitive.vertices+0);
  REQUIRE(outputs[1] == primitive.vertices+1);
  REQUIRE(outputs[2] == primitive.vertices+2);

  REQUIRE(inputs[0].gl_VertexID == 1 );
  REQUIRE(inputs[1].gl_VertexID == 3 );
  REQUIRE(inputs[2].gl_VertexID == 2 );
  REQUIRE((size_t)pullerOutputs[0].attributes[0] == 2100 );
  REQUIRE((size_t)pullerOutputs[1].attributes[0] == 2300 );
  REQUIRE((size_t)pullerOutputs[2].attributes[0] == 2200 );
  REQUIRE((size_t)pullerOutputs[0].attributes[1] == 120  );
  REQUIRE((size_t)pullerOutputs[1].attributes[1] == 140  );
  REQUIRE((size_t)pullerOutputs[2].attributes[1] == 130  );
  for(size_t i=2;i<MAX_ATTRIBUTES;++i){
    REQUIRE((size_t)pullerOutputs[0].attributes[i] == 0    );
    REQUIRE((size_t)pullerOutputs[1].attributes[i] == 0    );
    REQUIRE((size_t)pullerOutputs[2].attributes[i] == 0    );
  }

  baseInvocation = 3;
  vsInvocationCounter = 0;
  primitive.nofUsedVertices = 0;

  gpu_runPrimitiveAssembly(
      gpu,
      &primitive,
      nofPrimitiveVertices,
      &puller,
      baseInvocation,
      vs_test);

  REQUIRE(vsInvocationCounter == 3);
  REQUIRE(primitive.nofUsedVertices == 3);
  REQUIRE(gpus[0] == gpu);
  REQUIRE(gpus[1] == gpu);
  REQUIRE(gpus[2] == gpu);
  REQUIRE(outputs[0] == primitive.vertices+0);
  REQUIRE(outputs[1] == primitive.vertices+1);
  REQUIRE(outputs[2] == primitive.vertices+2);

  REQUIRE(inputs[0].gl_VertexID == 1 );
  REQUIRE(inputs[1].gl_VertexID == 2 );
  REQUIRE(inputs[2].gl_VertexID == 4 );
  REQUIRE((size_t)pullerOutputs[0].attributes[0] == 2100 );
  REQUIRE((size_t)pullerOutputs[1].attributes[0] == 2200 );
  REQUIRE((size_t)pullerOutputs[2].attributes[0] == 2400 );
  REQUIRE((size_t)pullerOutputs[0].attributes[1] == 120  );
  REQUIRE((size_t)pullerOutputs[1].attributes[1] == 130  );
  REQUIRE((size_t)pullerOutputs[2].attributes[1] == 150  );
  for(size_t i=2;i<MAX_ATTRIBUTES;++i){
    REQUIRE((size_t)pullerOutputs[0].attributes[i] == 0    );
    REQUIRE((size_t)pullerOutputs[1].attributes[i] == 0    );
    REQUIRE((size_t)pullerOutputs[2].attributes[i] == 0    );
  }

}

TEST_CASE("SOLUTION_TEST: gpu_computeScreenSpaceBarycentrics should compute barycentric coordinates"){
  Vec3 coords;
  Vec2 pixelCenter;
  Vec2 vertices[VERTICES_PER_TRIANGLE];
  Vec3 lines[EDGES_PER_TRIANGLE];

  vertices[0].data[0]=3.f;
  vertices[0].data[1]=101.f ;
  vertices[1].data[0]=43.f;
  vertices[1].data[1]=133.f;
  vertices[2].data[0]=77.f;
  vertices[2].data[1]=477.f;
  for(size_t v=0;v<VERTICES_PER_TRIANGLE;++v)
    construct2DLine(lines+v,vertices+v,vertices+((v+1)%VERTICES_PER_TRIANGLE));
  init_Vec2(&pixelCenter,23.f,144.f);
  gpu_computeScreenSpaceBarycentrics(&coords,&pixelCenter,vertices,lines);
  REQUIRE(equalFloats(coords.data[0],0.572443f ));
  REQUIRE(equalFloats(coords.data[1],0.34233f  ));
  REQUIRE(equalFloats(coords.data[2],0.0852273f));

  init_Vec2(&pixelCenter,33.f,244.f);
  gpu_computeScreenSpaceBarycentrics(&coords,&pixelCenter,vertices,lines);
  REQUIRE(equalFloats(coords.data[0],0.569287f ));
  REQUIRE(equalFloats(coords.data[1],0.0550821f));
  REQUIRE(equalFloats(coords.data[2],0.375631f ));
}

TEST_CASE("SOLUTION_TEST: phong_vertexShader should project vertex to clip-space"){
  Mat4 viewMatrix      ;
  Mat4 projectionMatrix;

  //init projection matrix
  perspective_Mat4(&projectionMatrix,MY_PI/3.f,1.f,1.f,1000.f);

  //init view matrix
  rotate_Mat4(&viewMatrix,0.f,0.f,1.f,1.1f);

  //create gpu
  GPU gpu = cpu_createGPU();

  //reserve uniform for view matrix
  cpu_reserveUniform(gpu,"viewMatrix"      ,UNIFORM_MAT4);

  //reserve uniform for projection matrix
  cpu_reserveUniform(gpu,"projectionMatrix",UNIFORM_MAT4);

  //upload matrices to gpu
  cpu_uniformMatrix4fv(gpu,getUniformLocation(gpu,"viewMatrix"      ),(float*)&viewMatrix      );
  cpu_uniformMatrix4fv(gpu,getUniformLocation(gpu,"projectionMatrix"),(float*)&projectionMatrix);

  //vertex data - position and normal
  Vec3 vertex[2];
  init_Vec3(vertex+0,10.f,1.f,100.f);
  init_Vec3(vertex+1,1.f,0.f,0.f);

  //create buffer
  BufferID vbo;
  cpu_createBuffers(gpu,1,&vbo);

  //upload data to buffer
  cpu_bufferData(gpu,vbo,sizeof(vertex),vertex);

  //create vertex puller
  VertexPullerID puller;
  cpu_createVertexPullers(gpu,1,&puller);

  //set position attribute
  cpu_setVertexPullerHead(gpu,puller,0,vbo,0              ,sizeof(float)*3);

  //set normal attribute
  cpu_setVertexPullerHead(gpu,puller,1,vbo,sizeof(float)*3,sizeof(float)*3);

  //activate puller
  cpu_bindVertexPuller(gpu,puller);

  //create program
  ProgramID prg = cpu_createProgram(gpu);
  cpu_attachVertexShader(gpu,prg,phong_vertexShader);

  //set interpolation
  cpu_setAttributeInterpolation(gpu,prg,0,ATTRIB_VEC3,SMOOTH);
  cpu_setAttributeInterpolation(gpu,prg,1,ATTRIB_VEC3,SMOOTH);

  //activate program
  cpu_useProgram(gpu,prg);

  //get puller configuration
  GPUVertexPullerConfiguration const*const pullerConf = gpu_getActiveVertexPuller(gpu);

  //set puller output
  GPUVertexPullerOutput pullerOutput;
  pullerOutput.attributes[0] = (uint8_t*)pullerConf->heads[0].buffer+sizeof(float)*0;
  pullerOutput.attributes[1] = (uint8_t*)pullerConf->heads[1].buffer+sizeof(float)*3;

  //set vertex shader input
  GPUVertexShaderInput vertexShaderInput;
  vertexShaderInput.attributes  = &pullerOutput;
  vertexShaderInput.gl_VertexID = 0;

  //run phong_vertexShader
  GPUVertexShaderOutput outputVertex;
  phong_vertexShader(&outputVertex,&vertexShaderInput,gpu);


  REQUIRE(equalFloats(outputVertex.gl_Position.data[0],+6.3128976822e+00f));
  REQUIRE(equalFloats(outputVertex.gl_Position.data[1],+1.6221815109e+01f));
  REQUIRE(equalFloats(outputVertex.gl_Position.data[2],-1.0220220184e+02f));
  REQUIRE(equalFloats(outputVertex.gl_Position.data[3],-1.0000000000e+02f));

  Vec3 const*const position = (Vec3*)outputVertex.attributes[0];
  Vec3 const*const normal   = (Vec3*)outputVertex.attributes[1];
  
  REQUIRE(position->data[0] == 10.f);
  REQUIRE(position->data[1] == 1.f);
  REQUIRE(position->data[2] == 100.f);

  REQUIRE(normal->data[0] == 1.f );
  REQUIRE(normal->data[1] == 0.f );
  REQUIRE(normal->data[2] == 0.f );
}

TEST_CASE("SOLUTION_TEST: phong_fragmentShader should compute phong color."){
  //create gpu
  GPU gpu = cpu_createGPU();

  //reserve uniforms
  cpu_reserveUniform(gpu,"cameraPosition",UNIFORM_VEC3);
  cpu_reserveUniform(gpu,"lightPosition" ,UNIFORM_VEC3);

  //upload uniform data
  cpu_uniform3f(gpu,getUniformLocation(gpu,"cameraPosition"),0.f,20.f,20.f);
  cpu_uniform3f(gpu,getUniformLocation(gpu,"lightPosition" ),0.f,100.f,0.f);

  //create program
  ProgramID prg = cpu_createProgram(gpu);
  cpu_attachFragmentShader(gpu,prg,phong_fragmentShader);

  //set interpolation
  cpu_setAttributeInterpolation(gpu,prg,0,ATTRIB_VEC3,SMOOTH);
  cpu_setAttributeInterpolation(gpu,prg,1,ATTRIB_VEC3,SMOOTH);

  //activate program
  cpu_useProgram(gpu,prg);

  //init input fragment
  GPUFragmentShaderInput  inputFragment;
  inputFragment.coords.data[0] = 10.f;
  inputFragment.coords.data[1] = 10.f;
  init_Vec3((Vec3*)inputFragment.attributes.attributes[0],1.f,1.f,1.f);
  init_Vec3((Vec3*)inputFragment.attributes.attributes[1],1.f,3.f,3.f);
  inputFragment.depth = 0.f;

  //init output fragment
  GPUFragmentShaderOutput outputFragment;
  zero_Vec4(&outputFragment.color);

  //run fragment shader
  phong_fragmentShader(&outputFragment,&inputFragment,gpu);

  REQUIRE(equalFloats(outputFragment.color.data[0],1.6810902670e-09f));
  REQUIRE(equalFloats(outputFragment.color.data[1],6.7890864611e-01f));
  REQUIRE(equalFloats(outputFragment.color.data[2],1.6810902670e-09f));
  REQUIRE(equalFloats(outputFragment.color.data[3],1.0000000000e+00f));

  init_Vec3((Vec3*)inputFragment.attributes.attributes[0],1.f,2.f,1.f);
  init_Vec3((Vec3*)inputFragment.attributes.attributes[1],3.f,1.f,2.f);
  phong_fragmentShader(&outputFragment,&inputFragment,gpu);

  REQUIRE(equalFloats(outputFragment.color.data[0],0.0000000000e+00f));
  REQUIRE(equalFloats(outputFragment.color.data[1],2.5359907746e-01f));
  REQUIRE(equalFloats(outputFragment.color.data[2],0.0000000000e+00f));
  REQUIRE(equalFloats(outputFragment.color.data[3],1.0000000000e+00f));
}

#endif




void runConformanceTests(char const*gtFile){
  groundTruthFile = gtFile;
  int argc = 1;
  char const*argv[] = {"test"};
  int result = Catch::Session().run( argc, argv );
  std::cout<<"NUMBER OF FAILED TESTS: "<<result<<std::endl;
}
