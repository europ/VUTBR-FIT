/*!
 * @file 
 * @brief This file contains implementation of mouse handling and camera manipulation.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#include<SDL2/SDL.h>

#include"student/mouseCamera.h"
#include"student/fwd.h"
#include"student/linearAlgebra.h"
#include"student/camera.h"

/// This variable contains 1 if left mouse button was pressed.
int         leftMouseButtonDown   = 0        ;
/// This variable contains 1 if right mouse button was pressed.
int         rightMouseButtonDown  = 0        ;
/// This varible contains 1 if middle mouse button was pressed.
int         middleMouseButtonDown = 0        ;
/// This variable contains orbit camera angle X.
float       cameraAngleX          = 0.f      ;
/// This variable contains orbit camera angle Y.
float       cameraAngleY          = 0.f      ;
/// This variable contains orbit camera distance.
float       cameraDistance        = 10.f     ;
/// This variable contains sensitivity of mouse.
float const cameraSensitivity     = 0.01f    ;
/// This variable contains zooming speed.
float const cameraZoomSpeed       = 0.04f    ;
/// This variable contains minimal allowed distance of orbit camera.
float const cameraMinDistance     = 1.f      ;
/// This variable contains maximal allowed distance of orbit camera.
float const cameraMaxDistance     = 100.f    ;
/// This variable contains distance to near plane.
float const cameraNear            = 0.1f     ;
/// This variable contains distance to far plane.
float const cameraFar             = 10000.f  ;
/// This variable contains field of view in y axis.
float const cameraFovy            = MY_PI/2.f;



/// This variable contains view matrix of orbit camera.
Mat4 viewMatrix;
/// This variable contains projection matrix of orbit camera.
Mat4 projectionMatrix;
/// This variable contains camera position in world-space.
Vec3 cameraPosition;

void onMouseMotion(int x,int y,int xrel,int yrel){
  (void)x;
  (void)y;
  int32_t changed = 0;
  if(leftMouseButtonDown){
    cameraAngleX += ((float)yrel)*cameraSensitivity;
    cameraAngleY += ((float)xrel)*cameraSensitivity;
    if(cameraAngleX < -MY_PI/2.f)cameraAngleX = -MY_PI/2.f;
    if(cameraAngleX > +MY_PI/2.f)cameraAngleX = +MY_PI/2.f;
    changed = 1;
  }
  if(rightMouseButtonDown){
    cameraDistance += ((float)yrel)*cameraZoomSpeed;
    if(cameraDistance < cameraMinDistance)cameraDistance = cameraMinDistance;
    if(cameraDistance > cameraMaxDistance)cameraDistance = cameraMaxDistance;
    changed = 1;
  }
  if(changed){
    cpu_computeOrbitCamera(&viewMatrix,cameraAngleX,cameraAngleY,cameraDistance);
    cpu_getCameraPositionFromViewMatrix(&cameraPosition,&viewMatrix);
  }
}

void onMouseButtonDown(uint8_t button){
  if(button == SDL_BUTTON_LEFT  )leftMouseButtonDown   = 1;
  if(button == SDL_BUTTON_RIGHT )rightMouseButtonDown  = 1;
  if(button == SDL_BUTTON_MIDDLE)middleMouseButtonDown = 1;
}

void onMouseButtonUp(uint8_t button){
  if(button == SDL_BUTTON_LEFT  )leftMouseButtonDown   = 0;
  if(button == SDL_BUTTON_RIGHT )rightMouseButtonDown  = 0;
  if(button == SDL_BUTTON_MIDDLE)middleMouseButtonDown = 0;
}

void cpu_initMatrices(int32_t width,int32_t height){
  float const aspectRatio = (float)width/(float)height;
  perspective_Mat4(&projectionMatrix,cameraFovy,aspectRatio,cameraNear,cameraFar);
  cpu_computeOrbitCamera(&viewMatrix,cameraAngleX,cameraAngleY,cameraDistance);
  cpu_getCameraPositionFromViewMatrix(&cameraPosition,&viewMatrix);
}
