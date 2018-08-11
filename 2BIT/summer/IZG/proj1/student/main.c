/**
 * @file 
 * @brief This file contains main function.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<SDL2/SDL.h>

#include"student/fwd.h"
#include"student/linearAlgebra.h"
#include"student/student_cpu.h"
#include"examples/triangleExample.h"
#include"student/mouseCamera.h"
#include"tests/conformanceTests.h"
#include"tests/performanceTest.h"

/**
 * @brief This function is entry point of appliaction
 *
 * @param argc   number of command line arguments
 * @param argv[] command line arguments
 *
 * @return return non zero on error
 */
int main(int argc,char*argv[]){
  char const*groundTruthFile = "../tests/output.bmp";
  int32_t windowWidth  = 500;
  int32_t windowHeight = 500;
  char const* applicationName = "izgProjekt2016";


  // run conformance tests
  if(argc>1 && strcmp(argv[1],"-c")==0){
    if(argc < 3)
      fprintf(stderr,"ERROR: expected path to reference image after -c command line argument. Image is located: tests/output.bmp\n");
    else
      groundTruthFile = argv[2];
    runConformanceTests(groundTruthFile);
    return EXIT_SUCCESS;
  }

  // run performance test
  if(argc>1 && strcmp(argv[1],"-p")==0){
    runPerformanceTest();
    return EXIT_SUCCESS;
  }


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

  typedef void(*ON_INIT)(int32_t,int32_t);
  typedef void(*ON_EXIT)();
  typedef void(*ON_DRAW)(SDL_Surface*);

  // pointers to methods
  ON_INIT const onInits[] = {
    phong_onInit,
    triangleExample_onInit,
  };

  ON_EXIT const onExits[] = {
    phong_onExit,
    triangleExample_onExit,
  };

  ON_DRAW const onDraws[] = {
    phong_onDraw,
    triangleExample_onDraw,
  };

  size_t const nofMethods = sizeof(onInits)/sizeof(onInits[0]);

  //id of method
  size_t method = 0;
  //id of last method (for detection of changes)
  size_t lastMethod = method;

  //inits first method
  onInits[method](windowWidth,windowHeight);

  SDL_Event event;
  int32_t running = 1;
  //main loop
  while(running){
    //get event
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT)
        running = 0;

      if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_n){//next method
          method++;
          if(method >= nofMethods)
            method = 0;
        }
        if(event.key.keysym.sym == SDLK_p){//previous method
          if(method == 0)
            method = nofMethods-1;
          else
            method--;
        }
      }

      if(event.type == SDL_MOUSEMOTION)
        onMouseMotion(event.motion.x,event.motion.y,event.motion.xrel,event.motion.yrel);

      if(event.type == SDL_MOUSEBUTTONDOWN)
        onMouseButtonDown(event.button.button);

      if(event.type == SDL_MOUSEBUTTONUP)
        onMouseButtonUp(event.button.button);
    }

    //is method switched?
    if(lastMethod != method){
      //destroy last method
      onExits[lastMethod]();
      onInits[method](windowWidth,windowHeight);
      lastMethod = method;
    }

    SDL_LockSurface(surface);
    onDraws[method](surface);
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  //destroy method
  onExits[method]();

  return EXIT_SUCCESS;
}


