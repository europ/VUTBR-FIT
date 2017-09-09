#include<iostream>
#include<iomanip>
#include<chrono>
#include<vector>
#include<algorithm>

#include"tests/performanceTest.h"
#include"student/student_cpu.h"
#include"student/linearAlgebra.h"
#include"student/mouseCamera.h"

void runPerformanceTest(){
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

  size_t const framesPerMeasurement = 10;
  auto start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < framesPerMeasurement; ++i)
    phong_onDraw(surface);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> elapsed = end - start;
  float const time = elapsed.count()/float(framesPerMeasurement);

  std::cout<<"Seconds per frame: "<<std::scientific<<std::setprecision(10)<<time<<std::endl;

  SDL_UnlockSurface(surface);
  SDL_UpdateWindowSurface(window);
  
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  phong_onExit();
}
