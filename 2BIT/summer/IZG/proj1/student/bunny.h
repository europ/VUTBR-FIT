/*!
 * @file 
 * @brief This file contains model of Standford bunny in static arrays.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include<stdint.h>

#include"student/fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This structure represents vertex that contains only position and normal.
 */
struct BunnyVertex {
	float position[3];///< position of vertex
	float normal[3];///< normal of vertex
};

typedef struct BunnyVertex BunnyVertex;///< shortcut

/// This variable contains vertices of Standford bunny.
extern struct BunnyVertex const bunnyVertices[1048];

/// This variable contains Standford bunny indices.
extern const VertexIndex bunnyIndices[2092][3];

#ifdef __cplusplus
}
#endif
