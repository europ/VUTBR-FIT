/*!
 * @file 
 * @brief This file contains implementation of swapBuffers function.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#include<assert.h>
#include"student/swapBuffers.h"
#include"student/gpu.h"

uint8_t floatColorToUint32(float const value){
  return (uint8_t)(value*255.f);
}

void cpu_swapBuffers(
    SDL_Surface *const surface,
    GPU          const gpu    ){
  assert(surface != NULL);
  assert(gpu     != NULL);
  size_t   const w = (size_t)surface->w;
  size_t   const h = (size_t)surface->h;
  uint8_t *const pixels = (uint8_t*)surface->pixels;
  for(size_t y = 0; y < h; ++y){
    size_t const reversedY = h-y-1;
    size_t const rowStart = reversedY*w;
    for(size_t x = 0; x < w; ++x){
      size_t const pixelCoord = rowStart + x;
      Vec4 const*const color = cpu_getColor(gpu,x,y);
      for(size_t c = 0; c < CHANNELS_PER_COLOR; ++c)
        pixels[pixelCoord*CHANNELS_PER_COLOR+c] = floatColorToUint32(color->data[c]);
    }
  }
}


