/*!
 * @file 
 * @brief This file contains function declarations important for cpu side.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#pragma once

#include<SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is called at start of application.
 * It should contain initialization of CPU side.
 * @param width width of window in pixels
 * @param height height of window in pixels
 */
void phong_onInit(int32_t width,int32_t height);

/**
 * @brief This function is called at end of application.
 */
void phong_onExit();

/**
 * @brief This function is called every frame and is responsible for drawing.
 *
 * @param surface SDL surface
 */
void phong_onDraw(SDL_Surface*surface);

#ifdef __cplusplus
}
#endif


