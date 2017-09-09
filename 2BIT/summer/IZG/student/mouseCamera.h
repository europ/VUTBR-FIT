/*!
 * @file 
 * @brief This file contains function declarations for mouse handling and camera manipulation.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#pragma once

#include<stdint.h>

#include"student/linearAlgebra.h"

#ifdef __cplusplus
extern "C" {
#endif

/// This variable contains view matrix of orbit camera.
extern Mat4 viewMatrix;
/// This variable contains projection matrix of orbit camera.
extern Mat4 projectionMatrix;
/// This variable contains camera position in world-space.
extern Vec3 cameraPosition;

/**
 * @brief This function is called when the mouse is moving.
 *
 * @param x x position of cursor
 * @param y y position of cursor
 * @param xrel relative x position to previous x position
 * @param yrel relative y position to previous y position
 */
void onMouseMotion(int x,int y,int xrel,int yrel);

/**
 * @brief This function is called when a mouse button is pressed.
 *
 * @param button id of pressed button, it can be SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT or SDL_BUTTON_MIDDLE.
 */
void onMouseButtonDown(uint8_t button);

/**
 * @brief This function is called when a mouse button is released.
 *
 * @param button id of released button, it can be SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT or SDL_BUTTON_MIDDLE.
 */
void onMouseButtonUp(uint8_t button);

/**
 * @brief This function initializes viewMatrix and projectionMatrix.
 * This function should be called once in init stage.
 *
 * @param width width of screen in pixels
 * @param height height of screen in pixels
 */
void cpu_initMatrices(int32_t width,int32_t height);

#ifdef __cplusplus
}
#endif
