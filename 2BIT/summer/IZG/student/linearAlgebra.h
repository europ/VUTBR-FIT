/*!
 * @file 
 * @brief This file contains structures and function declarations useful for linear algebra.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#pragma once

#include"fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This struct represents column 2D vector of 32-bit floats.
 */
struct Vec2{
  float data[2];///<components  of vector
};

/**
 * @brief This struct represents column 3D vector of 32-bt floats.
 */
struct Vec3{
  float data[3];///<components of vector
};

/**
 * @brief This struct represents column 4D vector of 32-bit floats
 */
struct Vec4{
  float data[4];///< components of vector
};

/**
 * @brief This struct represents 4x4 matrix - it is represented by 4 column vectors.
 */
struct Mat4{
  Vec4 column[4];///< columns of matrix
};

/**
 * @brief This function sets vector to zero vector.
 *
 * @param target target vector
 */
void zero_Vec2(
    Vec2 *const target);

/**
 * @brief This function sets vector to zero vector.
 *
 * @param target target vector
 */
void zero_Vec3(
    Vec3 *const target);

/**
 * @brief This function sets vector to zero vector.
 *
 * @param target target vector
 */
void zero_Vec4(
    Vec4 *const target);

/**
 * @brief This function initializes vector.
 *
 * @param target target vector
 * @param x x component
 * @param y y component
 */
void init_Vec2(
    Vec2  *const target,
    float  const x     ,
    float  const y     );

/**
 * @brief This function initializes vector.
 *
 * @param target target vector
 * @param x x component
 * @param y y component
 * @param z z component
 */
void init_Vec3(
    Vec3  *const target,
    float  const x     ,
    float  const y     ,
    float  const z     );

/**
 * @brief This function initializes vector.
 *
 * @param target target vector
 * @param x x component
 * @param y y component
 * @param z z component
 * @param w w component
 */
void init_Vec4(
    Vec4  *const target,
    float  const x     ,
    float  const y     ,
    float  const z     ,
    float  const w     );


/**
 * @brief This function copies one vector to another.
 *
 * @param target target vector
 * @param source source vector
 */
void copy_Vec2(
    Vec2      *const target,
    Vec2 const*const source);

/**
 * @brief This function copies one vector to another.
 *
 * @param target target vector
 * @param source source vector
 */
void copy_Vec3(
    Vec3      *const target,
    Vec3 const*const source);

/**
 * @brief This function copies one vector to another.
 *
 * @param target target vector
 * @param source source vector
 */
void copy_Vec4(
    Vec4      *const target,
    Vec4 const*const source);

/**
 * @brief This function copies larger vector to smaller vector.
 * This function omits components that are out of bounds.
 *
 * @param target target vector
 * @param source source vector
 */
void copy_Vec4_To_Vec2(
    Vec2      *const target,
    Vec4 const*const source);

/**
 * @brief This function copies larger vector to smaller vector.
 * This function omits components that are out of bounds.
 *
 * @param target target vector
 * @param source source vector
 */
void copy_Vec4_To_Vec3(
    Vec3      *const target,
    Vec4 const*const source);

/**
 * @brief This function copies larger vector to smaller vector.
 * This function omits components that are out of bounds.
 *
 * @param target target vector
 * @param source source vector
 */
void copy_Vec3_To_Vec2(
    Vec2      *const target,
    Vec3 const*const source);

/**
 * @brief This function copies vec3 and float to larger vector.
 *
 * @param target output vector
 * @param part0 x,y,z components
 * @param part1 w component
 */
void copy_Vec3Float_To_Vec4(
    Vec4       *const target,
    Vec3  const*const part0 ,
    float       const part1 );

/**
 * @brief This function copies vec2 and two floats to larger vector.
 *
 * @param target output vector
 * @param part0 x,y components
 * @param part1 z component
 * @param part2 w component
 */
void copy_Vec2FloatFloat_To_Vec4(
    Vec4       *const target,
    Vec2  const*const part0 ,
    float       const part1 ,
    float       const part2 );

/**
 * @brief This function copies two vec2 to larger vector.
 *
 * @param target output vector
 * @param part0 x,y components
 * @param part1 z,w components
 */
void copy_Vec2Vec2_To_Vec4(
    Vec4       *const target,
    Vec2  const*const part0 ,
    Vec2  const*const part1 );

/**
 * @brief This function copies two floats and vec2 to larger vector.
 *
 * @param target output vector
 * @param part0 x component
 * @param part1 y,z component
 * @param part2 w component
 */
void copy_FloatVec2Float_To_Vec4(
    Vec4       *const target,
    float       const part0 ,
    Vec2  const*const part1 ,
    float       const part2 );

/**
 * @brief This function copies two floats and vec2 to larger vector.
 *
 * @param target output vector
 * @param part0 x component
 * @param part1 y component
 * @param part2 z,w components
 */
void copy_FloatFloatVec2_To_Vec4(
    Vec4       *const target,
    float       const part0 ,
    float       const part1 ,
    Vec2  const*const part2 );

/**
 * @brief This function copies float and vec3 to larger vector.
 *
 * @param target output vector
 * @param part0 x component
 * @param part1 x,y,z components
 */
void copy_FloatVec3_To_Vec4(
    Vec4       *const target,
    float       const part0 ,
    Vec3  const*const part1 );

/**
 * @brief This function copies vec2 and float to larger vector.
 *
 * @param target output vector
 * @param part0 x,y components
 * @param part1 z component
 */
void copy_Vec2Float_ToVec3(
    Vec4       *const target,
    Vec2  const*const part0 ,
    float       const part1 );

/**
 * @brief This function copies float and vec2 to larger vector.
 *
 * @param target output vector
 * @param part0 x component
 * @param part1 y,z components
 */
void copy_FloatVec2_ToVec3(
    Vec4       *const target,
    float       const part0 ,
    Vec2  const*const part1 );


/**
 * @brief This function adds two vectors.
 *
 * @param output output vector, output = a + b
 * @param a first input vector
 * @param b second input vector
 */
void add_Vec2(
    Vec2      *const output,
    Vec2 const*const a     ,
    Vec2 const*const b     );

/**
 * @brief This function adds two vectors.
 *
 * @param output output vector, output = a + b
 * @param a first input vector
 * @param b second input vector
 */
void add_Vec3(
    Vec3      *const output,
    Vec3 const*const a     ,
    Vec3 const*const b     );

/**
 * @brief This function adds two vectors.
 *
 * @param output output vector, output = a + b
 * @param a first input vector
 * @param b second input vector
 */
void add_Vec4(
    Vec4      *const output,
    Vec4 const*const a     ,
    Vec4 const*const b     );

/**
 * @brief This function subtracts two vectors.
 *
 * @param output output vector, output = a - b 
 * @param a first vector
 * @param b second vector
 */
void sub_Vec2(
    Vec2      *const output,
    Vec2 const*const a     ,
    Vec2 const*const b     );

/**
 * @brief This function subtracts two vectors.
 *
 * @param output output vector, output = a - b 
 * @param a first vector
 * @param b second vector
 */
void sub_Vec3(
    Vec3      *const output,
    Vec3 const*const a     ,
    Vec3 const*const b     );

/**
 * @brief This function subtracts two vectors.
 *
 * @param output output vector, output = a - b 
 * @param a first vector
 * @param b second vector
 */
void sub_Vec4(
    Vec4      *const output,
    Vec4 const*const a     ,
    Vec4 const*const b     );

/**
 * @brief This function linearly mix two vectors.
 *
 * @param output output vector, output = (1-t)*a + t*b
 * @param a first vector
 * @param b second vector
 * @param t weight
 */
void mix_Vec2(
    Vec2       *const output,
    Vec2  const*const a     ,
    Vec2  const*const b     ,
    float       const t     );

/**
 * @brief This function linearly mix two vectors.
 *
 * @param output output vector, output = (1-t)*a + t*b
 * @param a first vector
 * @param b second vector
 * @param t weight
 */
void mix_Vec3(
    Vec3       *const output,
    Vec3  const*const a     ,
    Vec3  const*const b     ,
    float       const t     );

/**
 * @brief This function linearly mix two vectors.
 *
 * @param output output vector, output = (1-t)*a + t*b
 * @param a first vector
 * @param b second vector
 * @param t weight
 */
void mix_Vec4(
    Vec4       *const output,
    Vec4  const*const a     ,
    Vec4  const*const b     ,
    float       const t     );

/**
 * @brief This functions computes dot product of two vec2 vectors.
 *
 * @param left left side of multiplication
 * @param right right side of multiplication
 *
 * @return returns dot product
 */
float dot_Vec2(
    Vec2 const*const left ,
    Vec2 const*const right);

/**
 * @brief This function computes dot product of two vec3 vectors.
 *
 * @param left left side of multiplication
 * @param right right side of multiplication
 *
 * @return returns dot product
 */
float dot_Vec3(
    Vec3 const*const left ,
    Vec3 const*const right);

/**
 * @brief This function computes dot product of two vec4 vectors.
 *
 * @param left left side of multiplication
 * @param right right side of multiplication
 *
 * @return return dot product
 */
float dot_Vec4(
    Vec4 const*const left ,
    Vec4 const*const right);

/**
 * @brief This function multiplies vec2 and float.
 *
 * @param output resulting vector
 * @param left   vector
 * @param constant floating constant
 */
void multiply_Vec2_Float(
    Vec2      *const output  ,
    Vec2 const*const left    ,
    float      const constant);

/**
 * @brief This function multiplies vec3 and float.
 *
 * @param output resulting vector
 * @param left vector
 * @param constant floating constant
 */
void multiply_Vec3_Float(
    Vec3      *const output  ,
    Vec3 const*const left    ,
    float      const constant);

/**
 * @brief This function multiplies vec4 and float.
 *
 * @param output resulting vector
 * @param left vector
 * @param constant floating constant
 */
void multiply_Vec4_Float(
    Vec4      *const output  ,
    Vec4 const*const left    ,
    float      const constant);

/**
 * @brief This function computes length of vec2.
 *
 * @param vec input vector
 *
 * @return length of vector
 */
float length_Vec2(
    Vec2 const*const vec);

/**
 * @brief This function computes length of vec3.
 *
 * @param vec input vector
 *
 * @return length of vector
 */
float length_Vec3(
    Vec3 const*const vec);

/**
 * @brief This function computes length of vec4.
 *
 * @param vec input vector
 *
 * @return length of vector
 */
float length_Vec4(
    Vec4 const*const vec);

/**
 * @brief This function computes normalized vec2.
 *
 * @param output resulting vector
 * @param input  input vector
 */
void normalize_Vec2(
    Vec2      *const output,
    Vec2 const*const input );

/**
 * @brief This function computes normalized vec3.
 *
 * @param output resulting vector
 * @param input  input vector
 */
void normalize_Vec3(
    Vec3      *const output,
    Vec3 const*const input );

/**
 * @brief This function computes normalized vec4.
 *
 * @param output resulting vector
 * @param input input vector
 */
void normalize_Vec4(
    Vec4      *const output,
    Vec4 const*const input );

/**
 * @brief This function multiplies mat4 and vec4 producing vec4
 *
 * @param output resulting vec4 - output = left * right
 * @param left matrix - left side of multiplication
 * @param right vector - right side of multiplication
 */
void multiply_Mat4_Vec4(
    Vec4      *const output,
    Mat4 const*const left  ,
    Vec4 const*const right );

/**
 * @brief This function multiplies mat4 and mat4 producing mat4
 *
 * @param output resulting mat4 - output = left * right
 * @param left matrix - left side of multiplication
 * @param right matrix - right side of multiplication
 */
void multiply_Mat4_Mat4(
    Mat4      *const output,
    Mat4 const*const left  ,
    Mat4 const*const right );

/**
 * @brief This function initializes matrix to identity
 *
 * @param output matrix that is going to be set to identity
 */
void identity_Mat4(
    Mat4 *const output);

/**
 * @brief This function initializes matrix to translate matrix.
 *
 * @param output resulting mat4
 * @param tx translation in x direction
 * @param ty translation in y direction
 * @param tz translation in z direction
 */
void translate_Mat4(
    Mat4 *const output,
    float const tx    ,
    float const ty    ,
    float const tz    );

/**
 * @brief This function initializes matrix to scale matrix.
 *
 * @param output resulting mat4
 * @param sx scale in x axis
 * @param sy scale in y axis
 * @param sz scale in z axis
 */
void scale_Mat4(
    Mat4 *const output,
    float const sx    ,
    float const sy    ,
    float const sz    );

/**
 * @brief This function copies mat4 to another mat4.
 *
 * @param output output matrix
 * @param input input matrix
 */
void copy_Mat4(
    Mat4      *const output,
    Mat4 const*const input);

/**
 * @brief This function initializes matrix to rotation matrix.
 *
 * @param output resulting mat4
 * @param u x component of rotation vector
 * @param v y component of rotation vector
 * @param w z component of rotation vector
 * @param angle angle in radians
 */
void rotate_Mat4(
    Mat4 *const output,
    float const u     ,
    float const v     ,
    float const w     ,
    float const angle );

/**
 * @brief This function transposes mat4.
 *
 * @param output output matrix
 * @param input input matrix
 */
void transpose_Mat4(
    Mat4      *const output,
    Mat4 const*const input );

/**
 * @brief This function inverts mat4.
 *
 * @param output output matrix
 * @param input input matrix
 */
void invert_Mat4(
    Mat4      *const output,
    Mat4 const*const input );

/**
 * @brief This function constructs 2D line.
 * The line normal is oriented towards triangle, if the triangle edge A->B
 * is part of counter clockwise ordering of triangle vertices.
 *
 * @param line output line
 * @param a coords of vertex A
 * @param b coords of vertex B
 */
void construct2DLine(
    Vec3      *const line,
    Vec2 const*const a   ,
    Vec2 const*const b   );

/**
 * @brief This function computes distance to 2D line.
 * The normal of the line is assumed to be to normalized.
 *
 * @param line 2D line 
 * @param vertex 2D vertex
 *
 * @return distance to line
 */
float distanceTo2DLine(
    Vec3 const*const line  ,
    Vec2 const*const vertex);

/**
 * @brief This function reflect incident vector using normal vector
 *
 * @param output output vector
 * @param incident incident vector
 * @param normal normal vector
 */
void reflect(
    Vec3      *const output  ,
    Vec3 const*const incident,
    Vec3 const*const normal  );

#ifdef __cplusplus
}
#endif
