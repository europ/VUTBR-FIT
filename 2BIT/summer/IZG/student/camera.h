/*!
 * @file 
 * @brief This file contains function declarations for orbit camera manipulator.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include"fwd.h"

/**
 * @brief This functions initializes matrix to general perspective matrix.
 *
 * \image html images/frustum.svg "General perspective frustum" width=10cm
 *
 * @param output resulting mat4 
 * @param left   left coord of frustum
 * @param right  right coord of frustum
 * @param bottom bottom coord of frustum
 * @param top    top coord of frustum
 * @param near   naer coord of frustum
 * @param far    far  coord of frustum
 */
void frustum_Mat4(
    Mat4  *const output,
    float  const left  ,
    float  const right ,
    float  const bottom,
    float  const top   ,
    float  const near  ,
    float  const far   );

/**
 * @brief This functions initializes matrix to perspective matrix.
 *
 * @param output resulting mat4
 * @param fovy   field of view in y direction
 * @param aspect aspect ratio - width/height of screen
 * @param near   near plane distance
 * @param far    far plane distance
 */
void perspective_Mat4(
    Mat4  *const output,
    float  const fovy  ,
    float  const aspect,
    float  const near  ,
    float  const far   );

/**
 * @brief This function computes view matrix of orbit camera manipulator.
 *
 * \image html images/orbit_camera.svg "orbit camera" width=10cm
 *
 * @param viewMatrix output matrix
 * @param angleX x angle
 * @param angleY y angle
 * @param distance distance between camera center and focus point
 */
void cpu_computeOrbitCamera(
    Mat4 *const viewMatrix,
    float const angleX    ,
    float const angleY    ,
    float const distance  );

/**
 * @brief This function gets camere position from view matrix
 *
 * @param cameraPosition output camera position
 * @param viewMatrix view matrix
 */
void cpu_getCameraPositionFromViewMatrix(
    Vec3      *const cameraPosition,
    Mat4 const*const viewMatrix    );

#ifdef __cplusplus
}
#endif
