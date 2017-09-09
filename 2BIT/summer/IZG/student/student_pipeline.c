/*!
 * @file
 * @brief This file contains implementation of rendering pipeline.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */


#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<math.h>

#include"student/student_pipeline.h"
#include"student/uniforms.h"

/// \addtogroup gpu_side Úkoly v gpu části
/// @{
VertexIndex gpu_computeGLVertexID(
    VertexIndex            const*const indices               ,
    VertexShaderInvocation       const vertexShaderInvocation){
  /// \todo Naimplementujte výpočet gl_VertexID z vertexShaderInvocation
  /// a případného indexování.
  /// vertexShaderInvocation obsahuje pořadové číslo spuštění (invokace)
  /// vertex shaderu v rámci jednoho vykreslovacího příkazu.
  /// gl_VertexID je index vertexu, který se má poslat do vertex shaderu - návratová hodnota této funkce.
  /// Pokud není aktivní indexing, index vertexu odpovída pořadovému číslu
  /// invokace vertex shaderu.
  /// Pokud je aktivní indexing, je potřeba zaadresovat správný gl_VertexID
  /// z bufferu indexů (indices) pomocí čísla invokace.
  /// Indexing je aktivní, pokud buffer indexů není NULL.
  (void)indices;
  (void)vertexShaderInvocation;

  return indices ? indices[vertexShaderInvocation] : vertexShaderInvocation;

  //return 0;
}

void const* gpu_computeVertexAttributeDataPointer(
    GPUVertexPullerHead const*const head       ,
    VertexIndex               const gl_VertexID){
  /// \todo Naimplementujte výpočet ukazatele pro daný vertex attribut a číslo vrcholu.
  /// Tato funkce počíta přesný ukazatel na data vertex atributu.
  /// Dejte si pozor na ukazatelovou aritmetiku, ukazatel musí být na byte přesně.
  /// Správná adresa se odvíjí od adresy bufferu, offsetu čtěcí hlavy, čísla vrcholu a kroku čtecí hlavy.
  assert(head != NULL);
  (void)head;
  (void)gl_VertexID;

  return (void const*)(head->offset+head->stride*gl_VertexID+(size_t)head->buffer);

  //return NULL;
}

void gpu_runVertexPuller(
    GPUVertexPullerOutput             *const output                ,
    GPUVertexPullerConfiguration const*const puller                ,
    VertexShaderInvocation             const vertexShaderInvocation){
  /// \todo Naimplementujte funkci vertex pulleru, využijte funkce
  /// gpu_computeGLVertexID a gpu_computeVertexAttrbuteDataPointer.
  /// Funkce vertex pulleru je nastavit správné adresy vertex attributů.
  /// Výstupem vertex pulleru je struktura obsahují seznam pointerů.
  /// Vašim úkolem je vypočítat správné adresy jednotlivých attributů.
  /// Pokud daný atribut/čtecí hlava není povolený/neexistuje nastavte jeho adresu na NULL.<br>
  /// <b>Seznam funkcí, které jistě využijete:</b>
  ///  - gpu_computeGLVertexID()
  ///  - gpu_computeVertexAttributeDataPointer()
  assert(output != NULL);
  assert(puller != NULL);
  (void)output;
  (void)puller;
  (void)vertexShaderInvocation;

  VertexIndex IDX_VERTEX = gpu_computeGLVertexID(puller->indices,vertexShaderInvocation);
  for (unsigned i=0; i<MAX_ATTRIBUTES; i++) {
    output->attributes[i] = (puller->heads[i].enabled) ? gpu_computeVertexAttributeDataPointer(&puller->heads[i],IDX_VERTEX) : NULL;
  }

}

void gpu_runPrimitiveAssembly(
    GPU                                const gpu                       ,
    GPUPrimitive                      *const primitive                 ,
    size_t                             const nofPrimitiveVertices      ,
    GPUVertexPullerConfiguration const*const puller                    ,
    VertexShaderInvocation             const baseVertexShaderInvocation,
    VertexShader                       const vertexShader              ){
  assert(primitive            != NULL);
  assert(nofPrimitiveVertices <= VERTICES_PER_TRIANGLE);
  assert(puller               != NULL);
  assert(vertexShader         != NULL);
  (void)gpu;
  (void)primitive;
  (void)nofPrimitiveVertices;
  (void)puller;
  (void)baseVertexShaderInvocation;
  (void)vertexShader;
  /// \todo Naimplementujte funkci jednotky sestavující primitiva.
  /// Vašim úkolem je spustit vertex puller a dodaný vertex shader nad každým vrcholem primitiva.
  /// Funkce by měla spustit vertex puller/vertex shader N krát (podle množství vrcholů primitiva).
  /// Výstupy z vertex shaderu vložte do parametru primitive.
  /// Počet vrcholů primitiva je udán v nofPrimitiveVertices.
  /// Čislo invokace vertex shaderu pro první vrchol primitiva je v baseVertexShaderInvocation proměnné.
  /// Nezapomeňte spustit nad každým vrcholem primitiva vertex shader.
  /// Nezapomeňte do každého vrcholu správně zapsat číslo vrcholu (gl_VertexID)<br>
  /// <b>Seznam funkcí, které jistě využijete:</b>
  ///  - gpu_runVertexPuller()
  ///  - gpu_computeGLVertexID()<br>
  /// <b>Seznam struktur, které jistě využijete:</b>
  ///  - GPUVertexPullerOutput()
  ///  - GPUVertexShaderInput()
  GPUVertexShaderInput  input;
  GPUVertexPullerOutput output;

  primitive->nofUsedVertices=nofPrimitiveVertices;

  for (size_t i=0; i<nofPrimitiveVertices; i++) {
    gpu_runVertexPuller(&output,puller,i+baseVertexShaderInvocation);
    input.attributes  = (GPUVertexPullerOutput const*)&output.attributes;
    input.gl_VertexID = gpu_computeGLVertexID(puller->indices,i+baseVertexShaderInvocation);
    vertexShader(&primitive->vertices[i],&input,gpu);
  }

}

/// @}

/**
 * @brief This function does clipping of an edge by frustum plane.
 *
 * A point P(t) on the edge is: P(t) = vertexA + t*(vertexB - vertexA), t in [0,1].
 * This function returns interval of possible values of parameter t for which P(t) is in front of frustum plane.
 * The interval is returned in arguments: minT and maxT.
 * If the edge lies in front of the frustum plane, this function return minT = 0.f and maxT = 1.f
 * If the edge lies behind frustum plane, this function returns minT = 1.f and maxT = 0.f
 * The axis selects frustum planes according to: x - LEFT,RIGHT, y - BOTTOM,TOP, z - NEAR,FAR
 * The positive flag selects frustum planes according to: positive = 1 - LEFT,BOTTOM,NEAR, positive = 0 - RIGHT,TOP,FAR.
 *
 * @param minT     minimal allowed value of parameter t
 * @param maxT     maximal allowed value of parameter t
 * @param vertexA  vertex A of the edge
 * @param vertexB  vertex B of the edge
 * @param axis     axis of frustum plane
 * @param positive positive flag of frustum plane
 */
void gpu_runFrustumPlaneClippingOnEdge(
    float       *const minT    ,
    float       *const maxT    ,
    Vec4   const*const vertexA ,
    Vec4   const*const vertexB ,
    size_t       const axis    ,
    size_t       const positive){
  // A point on a triangle edge:
  // A(a,b,t) = a + t*(b-a)
  // a,b are positions of triangle vertices
  // t in [0,1]
  //
  // -A(a,b,t)w <= (2p-1)*A(a,b,t)i
  // i - axis
  // p - positive flag
  //
  // solve for t
  // -aw - t*(bw-aw) <= (1-2p)*(ai + t*(bi-ai))
  // -t*(bw-aw) - (1-2p)*t*(bi-ai) <= (1-2p)*ai + aw
  // t*(-bw+aw-(1-2p)*(bi-ai)) <= (1-2p)*ai + aw
  // t*M <= N
  // M>0 -> t <= N/M
  // M<0 -> t >= N/M
  // M=0 and N>=0 -> t = any value
  // M=0 and N< 0 -> t = no value
  assert(vertexA  != NULL);
  assert(vertexB  != NULL);
  assert(axis     <= 2);
  assert(positive <= 1);
  *minT = 0.f;
  *maxT = 1.f;
  float const ai = vertexA->data[axis];
  float const aw = vertexA->data[3];
  float const bi = vertexB->data[axis];
  float const bw = vertexB->data[3];
  float const s = (2.f*(float)positive-1.f);
  float const M = -bw+aw-s*(bi-ai);
  float const N = s*ai + aw;
  if     (M>0.f)*maxT = fminf(*maxT,N/M);
  else if(M<0.f)*minT = fmaxf(*minT,N/M);
  else if(N<0.f){
    *maxT = 0.f;
    *minT = 1.f;
  }
}

/**
 * @brief This function writes one triangle when only one vertex is visible.
 *
 * @param triangles output list of clipped triangles
 * @param nofTriangles number of triangles
 * @param visible visibility mask for triangle vertices
 * @param tMin minimal t parameters
 * @param tMax maximal t parameters
 * @param triangle input triangle
 */
void gpu_writeClippedTriangle_OneVertexVisible(
    GPUTriangle      *const triangles   ,
    size_t           *const nofTriangles,
    size_t            const visible     ,
    float             const tMin[3]     ,
    float             const tMax[3]     ,
    GPUTriangle const*const triangle    ){
  assert(triangles    != NULL);
  assert(nofTriangles != NULL);
  assert(triangle     != NULL);
  size_t const vertex = visible>>1;
  size_t const prevVertex = (vertex+2)%VERTICES_PER_TRIANGLE;
  size_t const nextVertex = (vertex+1)%VERTICES_PER_TRIANGLE;
  float const prevT = tMin[prevVertex];
  float const nextT = tMax[vertex    ];

  Vec3      *const dstCoords    = triangles[*nofTriangles].coords;
  Vec3 const*const srcCoords    = triangle->coords;
  Vec4      *const dstPositions = triangles[*nofTriangles].positions;
  Vec4 const*const srcPositions = triangle->positions;

  copy_Vec3(
      dstCoords + 0     ,
      srcCoords + vertex);

  mix_Vec3(
      dstCoords + 1         ,
      srcCoords + vertex    ,
      srcCoords + nextVertex,
      nextT                 );

  mix_Vec3(
      dstCoords + 2         ,
      srcCoords + prevVertex,
      srcCoords + vertex    ,
      prevT                 );

  copy_Vec4(
      dstPositions + 0     ,
      srcPositions + vertex);

  mix_Vec4(
      dstPositions + 1         ,
      srcPositions + vertex    ,
      srcPositions + nextVertex,
      nextT                    );

  mix_Vec4(
      dstPositions + 2         ,
      srcPositions + prevVertex,
      srcPositions + vertex    ,
      prevT                    );

  (*nofTriangles)++;
}

/**
 * @brief This function writes first clipped triangle when two vertices are visible.
 *
 * @param dstCoords pointer to destination barycentrics
 * @param srcCoords pointer to source barycentrics
 * @param dstPositions pointer to destination positions
 * @param srcPositions pointer to source positions
 * @param vertex id of not visible vertex
 * @param nextVertex id of next visible vertex in respect to vertex
 * @param prevVertex id of previous visible vertex in respect to vertex
 * @param nextT t parameter of next triangle edge
 * @param prevT t parameter of previous triangle edge
 */
void gpu_writeClippedTriangle_TwoVerticesVisible_FirstTriangle(
    Vec3        *const dstCoords   ,
    Vec3   const*const srcCoords   ,
    Vec4        *const dstPositions,
    Vec4   const*const srcPositions,
    size_t       const vertex      ,
    size_t       const nextVertex  ,
    size_t       const prevVertex  ,
    float        const nextT       ,
    float        const prevT       ){
  assert(dstCoords    != NULL);
  assert(srcCoords    != NULL);
  assert(dstPositions != NULL);
  assert(srcPositions != NULL);

  mix_Vec3(
      dstCoords + 0         ,
      srcCoords + vertex    ,
      srcCoords + nextVertex,
      nextT                 );

  copy_Vec3(
      dstCoords + 1         ,
      srcCoords + nextVertex);

  mix_Vec3(
      dstCoords + 2        ,
      srcCoords + prevVertex,
      srcCoords + vertex    ,
      prevT                 );

  mix_Vec4(
      dstPositions + 0         ,
      srcPositions + vertex    ,
      srcPositions + nextVertex,
      nextT                    );

  copy_Vec4(
      dstPositions + 1         ,
      srcPositions + nextVertex);

  mix_Vec4(
      dstPositions + 2         ,
      srcPositions + prevVertex,
      srcPositions + vertex    ,
      prevT                    );
}

/**
 * @brief This function writes second clipped triangle when two vertices are visible.
 *
 * @param dstCoords pointer to destination barycentrics
 * @param srcCoords pointer to source barycentrics
 * @param dstPositions pointer to destination positions
 * @param srcPositions pointer to source positions
 * @param vertex id of not visible vertex
 * @param nextVertex id of next visible vertex in respect to vertex
 * @param prevVertex id of previous visible vertex in respect to vertex
 * @param prevT t parameter of previous triangle edge
 */
void gpu_writeClippedTriangle_TwoVerticesVisible_SecondTriangle(
    Vec3        *const dstCoords   ,
    Vec3   const*const srcCoords   ,
    Vec4        *const dstPositions,
    Vec4   const*const srcPositions,
    size_t       const vertex      ,
    size_t       const nextVertex  ,
    size_t       const prevVertex  ,
    float        const prevT       ){
  assert(dstCoords    != NULL);
  assert(srcCoords    != NULL);
  assert(dstPositions != NULL);
  assert(srcPositions != NULL);
  mix_Vec3(
      dstCoords + 0         ,
      srcCoords + prevVertex,
      srcCoords + vertex    ,
      prevT                 );

  copy_Vec3(
      dstCoords + 1         ,
      srcCoords + nextVertex);

  copy_Vec3(
      dstCoords + 2         ,
      srcCoords + prevVertex);

  mix_Vec4(
      dstPositions + 0         ,
      srcPositions + prevVertex,
      srcPositions + vertex    ,
      prevT                    );

  copy_Vec4(
      dstPositions + 1         ,
      srcPositions + nextVertex);

  copy_Vec4(
      dstPositions + 2         ,
      srcPositions + prevVertex);
}

/**
 * @brief This function writes two clipped triangle when two vertices are visible.
 *
 * @param triangles triangle list
 * @param nofTriangles number of triangles
 * @param visible visibility mask of triangle vertices
 * @param tMin minimal t parameters
 * @param tMax maximal t parameters
 * @param triangle input triangle
 */
void gpu_writeClippedTriangle_TwoVerticesVisible(
    GPUTriangle      *const triangles   ,
    size_t           *const nofTriangles,
    size_t            const visible     ,
    float             const tMin[3]     ,
    float             const tMax[3]     ,
    GPUTriangle const*const triangle    ){
  assert(triangles    != NULL);
  assert(nofTriangles != NULL);
  assert(triangle     != NULL);

  size_t const vertex = (~visible & 0x7)>>1;
  size_t const prevVertex = (vertex+2)%VERTICES_PER_TRIANGLE;
  size_t const nextVertex = (vertex+1)%VERTICES_PER_TRIANGLE;
  float const prevT = tMax[prevVertex];
  float const nextT = tMin[vertex];

  gpu_writeClippedTriangle_TwoVerticesVisible_FirstTriangle(
      triangles[*nofTriangles].coords   ,triangle->coords   ,
      triangles[*nofTriangles].positions,triangle->positions,
      vertex,nextVertex,prevVertex,
      nextT,prevT);

  (*nofTriangles)++;

  gpu_writeClippedTriangle_TwoVerticesVisible_SecondTriangle(
      triangles[*nofTriangles].coords   ,triangle->coords   ,
      triangles[*nofTriangles].positions,triangle->positions,
      vertex,nextVertex,prevVertex,
      prevT);

  (*nofTriangles)++;
}

/**
 * @brief This function copies input triangle into output triangle list.
 *
 * @param triangles output triangle list
 * @param nofTriangles number of triangles in output triangle list
 * @param triangle input triangle
 */
void gpu_writeClippedTriangle_ThreeVerticesVisible(
    GPUTriangle             triangles   [MAX_CLIPPED_TRIANGLES],
    size_t           *const nofTriangles                       ,
    GPUTriangle const*const triangle                           ){
  assert(triangles    != NULL);
  assert(nofTriangles != NULL);
  assert(triangle     != NULL);

  for(size_t v = 0; v < VERTICES_PER_TRIANGLE; ++v){
    copy_Vec4(triangles[*nofTriangles].positions+v,triangle->positions+v);
    copy_Vec3(triangles[*nofTriangles].coords   +v,triangle->coords   +v);
  }
  (*nofTriangles)++;
}

/**
 * @brief This function performs frustum plane clipping on a triangle.
 * Triangles can be splitted into multiple triangle (0-2) if they intersect a frustum plane.
 *
 * @param triangles    output array of triangles
 * @param nofTriangles number of already clipped triangles in output array of triangles (this number will be increased by 0-2)
 * @param triangle     input triangle
 * @param plane        id of frustum plane
 */
void gpu_runFrustumPlaneClippingOnTriangle(
    GPUTriangle             triangles   [MAX_CLIPPED_TRIANGLES],
    size_t           *const nofTriangles                       ,
    GPUTriangle const*const triangle                           ,
    FrustumPlane      const plane                              ){
  assert(triangles    != NULL);
  assert(nofTriangles != NULL);
  assert(triangle     != NULL);
  // A point in clip-space P=(Px,Py,Pz,Pw) lies in camera view if and only if:
  // forall i in {x,y,z}: -Pw <= Pi <= +Pw
  // forall i in {x,y,z}: -Pw <= Pi and Pi <= +Pw
  // forall i in {x,y,z}: -Pw <= Pi and -Pi >= -Pw
  // forall i in {x,y,z}: -Pw <= Pi and -Pw <= -Pi
  //
  // i - axis {x,y,z}
  //
  // A axis refers to frustum planes:
  // x axis - LEFT  , RIGHT
  // y axis - BOTTOM, TOP
  // z axis - NEAR  , FAR
  //
  // A part of comparison refers to frustum planes:
  // -Pw <= +Pi - LEFT , BOTTOM, NEAR
  // -Pw <= -Pi - RIGHT, TOP   , FAR
  //
  // -Pw <= (2p-1)Pi
  // n - positive/negative {0,1}
  // p = 1 -> LEFT , BOTTOM, NEAR
  // p = 0 -> RIGHT, TOP   , FAR
  //
  // Comparison: -Pw <= Py refers to BOTTOM frustum plane

  size_t axis = 0;
  if     (plane == LEFT   || plane == RIGHT)axis = 0;
  else if(plane == BOTTOM || plane == TOP  )axis = 1;
  else if(plane == FAR    || plane == NEAR )axis = 2;
  size_t positive = 0;
  if     (plane == LEFT  || plane == BOTTOM || plane == NEAR)positive = 1;
  else if(plane == RIGHT || plane == TOP    || plane == FAR )positive = 0;

  float tMin[VERTICES_PER_TRIANGLE],tMax[VERTICES_PER_TRIANGLE];
  for(size_t v = 0; v < VERTICES_PER_TRIANGLE; ++v){
    size_t const nextV = (v+1)%VERTICES_PER_TRIANGLE;
    gpu_runFrustumPlaneClippingOnEdge(
        tMin+v,
        tMax+v,
        &triangle->positions[v],
        &triangle->positions[nextV],
        axis,
        positive);
  }

  // Binary mask of visible vertices
  uint32_t visible = 0u;
  for(size_t i=0;i<VERTICES_PER_TRIANGLE;++i)visible |= ((uint32_t)(tMin[i] == 0.f && tMin[i] <= tMax[i]))<<i;

  // no vertex is in front of frustum plane
  if(visible == 0x0){
    //fprintf(stderr,"zero\n");
    return;
  }

  // only one vertex is in front of frustum plane
  if(visible == 0x1 || visible == 0x2 || visible == 0x4){
    //fprintf(stderr,"one\n");
    gpu_writeClippedTriangle_OneVertexVisible(triangles,nofTriangles,visible,tMin,tMax,triangle);
    return;
  }

  // two vertrices are in front of frustum plane
  if(visible == 0x3 || visible == 0x5 || visible == 0x6){
    //fprintf(stderr,"two\n");
    gpu_writeClippedTriangle_TwoVerticesVisible(triangles,nofTriangles,visible,tMin,tMax,triangle);
    return;
  }

  // all vertices are in front of frustum plane
  if(visible == 0x7){
    //fprintf(stderr,"three\n");
    gpu_writeClippedTriangle_ThreeVerticesVisible(triangles,nofTriangles,triangle);
    return;
  }
}

/**
 * @brief This function performs frustum plane clipping on triangle list.
 * Triangles can be splitted into multiple triangles if they intersect a frustum plane.
 * This function performs clipping using only one selected frustum plane.
 * This function has to be called 6 times in order to do full frustum clipping.
 * Every triangle in input triangle list is subject of clipping.
 *
 * @param output output triangle list
 * @param input input triangle list
 * @param plane id of plane
 */
void gpu_runFrustumPlaneClippingOnTriangleList(
    GPUTriangleList      *const output,
    GPUTriangleList const*const input ,
    FrustumPlane          const plane ){
  assert(output != NULL);
  assert(input  != NULL);

  output->nofTriangles = 0;
  for(size_t i = 0; i < input->nofTriangles; ++i)
    gpu_runFrustumPlaneClippingOnTriangle(output->triangles,&output->nofTriangles,input->triangles+i,plane);
}

void gpu_runTriangleClipping(
    GPUTriangleList      *const output,
    GPUTriangle     const*const input ){
  assert(output != NULL);
  assert(input  != NULL);
  output->nofTriangles = 0;

  //full clipping
  //GPUTriangleList c;
  //c.nofTriangles = 0;
  //gpu_runFrustumPlaneClippingOnTriangle(c.triangles,&c.nofTriangles,input,NEAR);
  //gpu_runFrustumPlaneClippingOnTriangleList(output,&c    ,FAR   );
  //gpu_runFrustumPlaneClippingOnTriangleList(&c    ,output,LEFT  );
  //gpu_runFrustumPlaneClippingOnTriangleList(output,&c    ,RIGHT );
  //gpu_runFrustumPlaneClippingOnTriangleList(&c    ,output,BOTTOM);
  //gpu_runFrustumPlaneClippingOnTriangleList(output,&c    ,TOP   );

  //near plane clipping
  gpu_runFrustumPlaneClippingOnTriangle(output->triangles,&output->nofTriangles,input,NEAR);
}

void gpu_runPerspectiveDivision(
    GPUPrimitive *const primitive){
  assert(primitive != NULL);
  for(size_t v = 0; v < primitive->nofUsedVertices; ++v)
    for(size_t k = 0; k < 3; ++k)
      primitive->vertices[v].gl_Position.data[k] /= primitive->vertices[v].gl_Position.data[3];
}

void gpu_runViewportTransformation(
    GPUPrimitive      *const primitive,
    size_t             const width    ,
    size_t             const height   ){
  assert(primitive != NULL);
  for(size_t v=0;v<primitive->nofUsedVertices;++v){
    primitive->vertices[v].gl_Position.data[0] =
        (primitive->vertices[v].gl_Position.data[0]*.5f+.5f)*(float)width;
    primitive->vertices[v].gl_Position.data[1] =
        (primitive->vertices[v].gl_Position.data[1]*.5f+.5f)*(float)height;
  }
}

size_t gpu_roundDownPixelCoord(
    float  const coord){
  assert(coord>=0.f);
  float  const fractional = coord - truncf(coord);
  size_t const integer    = (size_t)truncf(coord);
  return integer + (size_t)(fractional > PIXEL_CENTER);
}

size_t gpu_roundUpPixelCoord(
    float  const coord){
  assert(coord>=0.f);
  float  const fractional = coord - truncf(coord);
  size_t const integer    = (size_t)truncf(coord);
  return integer + (size_t)(fractional >= PIXEL_CENTER);
}

void gpu_restrictLineBorders(
    float     *const minX    ,
    float     *const maxX    ,
    float      const y       ,
    Vec3 const*const edgeLine){
  assert(minX     != NULL);
  assert(maxX     != NULL);
  assert(edgeLine != NULL);
  // edgeLine (a,b,c): ax+by+c = 0
  // half space that contains triangle: ax+by+c>=0
  // ax+by+c>=0
  // ax>=-by-c
  // ax>=d
  // a>0 -> x >= d/a
  // a<0 -> x <= d/a
  // a=0 && d <= 0 -> no restriction
  // a=0 && d >  0 -> no values are possible
  float const a = edgeLine->data[0];
  float const b = edgeLine->data[1];
  float const c = edgeLine->data[2];
  float const d = -b*y-c;
  if     (a>0.f)
    *minX = fmaxf(*minX,d/a);
  else if(a<0.f)
    *maxX = fminf(*maxX,d/a);
  else if(d>0.f){
    *maxX = -INFINITY;
    *minX = +INFINITY;
  }
}

void gpu_computeLineBorders(
    float      *const minX                             ,
    float      *const maxX                             ,
    float       const y                                ,
    Vec3        const triangleLines[EDGES_PER_TRIANGLE]){
  assert(minX          != NULL);
  assert(maxX          != NULL);
  assert(triangleLines != NULL);
  *minX = -INFINITY;
  *maxX = +INFINITY;
  for(size_t edge = 0; edge < EDGES_PER_TRIANGLE; ++edge)
    gpu_restrictLineBorders(minX,maxX,y,triangleLines+edge);
}

/// \addtogroup gpu_side
/// @{

void gpu_computeScreenSpaceBarycentrics(
    Vec3      *const coords                            ,
    Vec2 const*const pixelCenter                       ,
    Vec2       const vertices   [VERTICES_PER_TRIANGLE],
    Vec3       const lines      [EDGES_PER_TRIANGLE   ]){
  /// \todo V této funkci spočtěte barycentrické coordináty trojúhelníku v obrazovce.
  /// Coordináty zapište do coords.
  /// V proměnné vertices naleznete pozice vrcholů ve 2D v obrazovce.
  /// V proměnné lines naleznete rovnice přímek hran trojúhelníka.
  /// Rovnice přímek jsou normalizované (velikost normály je 1).
  /// Normála směřuje směrem dovnitř trojúhelníka.
  assert(coords      != NULL);
  assert(pixelCenter != NULL);
  assert(vertices    != NULL);
  assert(lines       != NULL);
  (void)coords;
  (void)pixelCenter;
  (void)vertices;
  (void)lines;
  //OBSAH[1,2,3] = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2)
  float area_triangle = 0.5f*((vertices[0].data[0]*(vertices[1].data[1]-vertices[2].data[1]))+(vertices[1].data[0]*(vertices[2].data[1]-vertices[0].data[1]))+(vertices[2].data[0]*(vertices[0].data[1]-vertices[1].data[1])));
  float area_part1 = 0.5f*((pixelCenter->data[0]*(vertices[1].data[1]-vertices[2].data[1]))+(vertices[1].data[0]*(vertices[2].data[1]-pixelCenter->data[1]))+(vertices[2].data[0]*(pixelCenter->data[1]-vertices[1].data[1])));
  float area_part2 = 0.5f*((vertices[0].data[0]*(pixelCenter->data[1]-vertices[2].data[1]))+(pixelCenter->data[0]*(vertices[2].data[1]-vertices[0].data[1]))+(vertices[2].data[0]*(vertices[0].data[1]-pixelCenter->data[1])));
  float area_part3 = 0.5f*((vertices[0].data[0]*(vertices[1].data[1]-pixelCenter->data[1]))+(vertices[1].data[0]*(pixelCenter->data[1]-vertices[0].data[1]))+(pixelCenter->data[0]*(vertices[0].data[1]-vertices[1].data[1])));
  if (area_triangle < 0) area_triangle *= -1;
  if (area_part1 < 0) area_part1 *= -1;
  if (area_part2 < 0) area_part2 *= -1;
  if (area_part3 < 0) area_part3 *= -1;
  coords->data[0] = area_part1 / area_triangle;
  coords->data[1] = area_part2 / area_triangle;
  coords->data[2] = area_part3 / area_triangle;
}

/// @}

void gpu_computeTriangleLines(
    Vec3       lines   [EDGES_PER_TRIANGLE   ],
    Vec2 const vertices[VERTICES_PER_TRIANGLE]){
  assert(lines    != NULL);
  assert(vertices != NULL);
  for(size_t vertex = 0; vertex < VERTICES_PER_TRIANGLE; ++vertex){
    size_t const nextVertex = (vertex+1)%VERTICES_PER_TRIANGLE;
    construct2DLine(lines+vertex,vertices+vertex,vertices+nextVertex);
  }
}

float gpu_noperspectiveInterpolate(
    float const values [WEIGHTS_PER_BARYCENTRICS],
    float const weights[WEIGHTS_PER_BARYCENTRICS]){
  float result = 0.f;
  for(size_t i=0;i<WEIGHTS_PER_BARYCENTRICS;++i)
    result += values[i]*weights[i];
  return result;
}

float gpu_smoothInterpolate(
    float const values           [WEIGHTS_PER_BARYCENTRICS],
    float const weights          [WEIGHTS_PER_BARYCENTRICS],
    float const homogeneousCoords[WEIGHTS_PER_BARYCENTRICS]){
  float dividend = 0.f;
  float divisor  = 0.f;
  for(size_t i=0;i<WEIGHTS_PER_BARYCENTRICS;++i){
    dividend += values[i]*weights[i]/homogeneousCoords[i];
    divisor  += weights[i]/homogeneousCoords[i];
  }
  return dividend/divisor;
}

void gpu_createFragment(
    GPUFragmentShaderInput      *const fragment    ,
    GPUPrimitive           const*const primitive   ,
    Vec3                   const*const barycentrics,
    Vec2                   const*const pixelCoord  ){
  assert(fragment     != NULL);
  assert(primitive    != NULL);
  assert(barycentrics != NULL);
  float const homogeneousCoords[WEIGHTS_PER_BARYCENTRICS] = {
    primitive->vertices[0].gl_Position.data[3],
    primitive->vertices[1].gl_Position.data[3],
    primitive->vertices[2].gl_Position.data[3],
  };

  for(size_t attribute = 0; attribute < MAX_ATTRIBUTES; ++attribute){
    if(primitive->types[attribute] == ATTRIB_EMPTY)continue;
    size_t const nofComponents = (size_t)primitive->types[attribute];
    if   (primitive->interpolations[attribute] == FLAT){
      for(size_t component=0;component<nofComponents;++component)
        ((float*)fragment->attributes.attributes[attribute])[component] =
          ((float*)primitive->vertices[0].attributes[attribute])[component];
    }else if(primitive->interpolations[attribute] == NOPERSPECTIVE){
      for(size_t component=0;component<nofComponents;++component){
        float const values[WEIGHTS_PER_BARYCENTRICS] = {
          ((float*)primitive->vertices[0].attributes[attribute])[component],
          ((float*)primitive->vertices[1].attributes[attribute])[component],
          ((float*)primitive->vertices[2].attributes[attribute])[component],
        };
        ((float*)fragment->attributes.attributes[attribute])[component] = gpu_noperspectiveInterpolate(
          values,
          barycentrics->data);
      }
    }else if(primitive->interpolations[attribute] == SMOOTH){
      for(size_t component=0;component<nofComponents;++component){
        float const values[WEIGHTS_PER_BARYCENTRICS] = {
          ((float*)primitive->vertices[0].attributes[attribute])[component],
          ((float*)primitive->vertices[1].attributes[attribute])[component],
          ((float*)primitive->vertices[2].attributes[attribute])[component],
        };
        ((float*)fragment->attributes.attributes[attribute])[component] = gpu_smoothInterpolate(
          values,
          barycentrics->data,
          homogeneousCoords);
      }
    }
  }
  copy_Vec2(&fragment->coords,pixelCoord);

  float const depths[WEIGHTS_PER_BARYCENTRICS] = {
    primitive->vertices[0].gl_Position.data[3],
    primitive->vertices[1].gl_Position.data[3],
    primitive->vertices[2].gl_Position.data[3],
  };
  fragment->depth = gpu_smoothInterpolate(
      depths,
      barycentrics->data,
      homogeneousCoords);
}

void gpu_perFragmentOperations(
    GPU                           const gpu     ,
    GPUFragmentShaderOutput const*const fragment,
    size_t                        const x       ,
    size_t                        const y       ){
  assert(fragment != NULL);
  if(fragment->depth < gpu_getDepth(gpu,x,y)){
    gpu_setColor(gpu,x,y,&fragment->color);
    gpu_setDepth(gpu,x,y, fragment->depth);
  }
}

void gpu_rasterizeTriangle(
    GPU                const gpu      ,
    GPUPrimitive const*const primitive,
    size_t             const width    ,
    size_t             const height   ){
  assert(primitive != NULL);

  // bounding quad of primitive
  float yMin = +INFINITY;
  float yMax = -INFINITY;
  for(size_t v = 0; v < primitive->nofUsedVertices; ++v){
    yMin = fminf(yMin,primitive->vertices[v].gl_Position.data[1]);
    yMax = fmaxf(yMax,primitive->vertices[v].gl_Position.data[1]);
  }
  if(yMin<0.f)yMin = 0.f;
  if(yMax<0.f)yMax = 0.f;

  Vec3 triangleLines[EDGES_PER_TRIANGLE];
  Vec2 triangleVertices[VERTICES_PER_TRIANGLE];
  for(size_t vertex = 0; vertex < VERTICES_PER_TRIANGLE; ++vertex)
    copy_Vec4_To_Vec2(triangleVertices+vertex,&primitive->vertices[vertex].gl_Position);
  gpu_computeTriangleLines(triangleLines,triangleVertices);

  size_t yMinI = gpu_roundDownPixelCoord(yMin);
  size_t yMaxI = gpu_roundUpPixelCoord  (yMax);
  if(yMaxI>=height)yMaxI = height;


  FragmentShader const fragmentShader = gpu_getActiveFragmentShader(gpu);
  for(size_t y = yMinI; y < yMaxI; ++y){
    Vec2 pixelCoord;
    pixelCoord.data[1] = (float)y + PIXEL_CENTER;
    float xMin,xMax;
    gpu_computeLineBorders(&xMin,&xMax,pixelCoord.data[1],triangleLines);
    if(xMin<0.f)xMin = 0.f;
    if(xMax<0.f)xMax = 0.f;
    if(xMin >= xMax)continue;
    size_t xMinI = gpu_roundDownPixelCoord(xMin);
    size_t xMaxI = gpu_roundUpPixelCoord  (xMax);
    if(xMaxI>=width)xMaxI = width;
    for(size_t x = xMinI; x < xMaxI; ++x){
      GPUFragmentShaderInput fragmentShaderInput;
      GPUFragmentShaderOutput fragmentShaderOutput;
      pixelCoord.data[0] = (float)x + PIXEL_CENTER;
      Vec3 barycentrics;
      gpu_computeScreenSpaceBarycentrics(&barycentrics,&pixelCoord,triangleVertices,triangleLines);
      gpu_createFragment(&fragmentShaderInput,primitive,&barycentrics,&pixelCoord);
      fragmentShaderOutput.depth = fragmentShaderInput.depth;
      fragmentShader(&fragmentShaderOutput,&fragmentShaderInput,gpu);
      for(size_t i=0;i<4;++i){
        if(fragmentShaderOutput.color.data[i] < 0.f)
          fragmentShaderOutput.color.data[i] = 0.f;
        if(fragmentShaderOutput.color.data[i] > 1.f)
          fragmentShaderOutput.color.data[i] = 1.f;
      }
      gpu_perFragmentOperations(gpu,&fragmentShaderOutput,(size_t)pixelCoord.data[0],(size_t)pixelCoord.data[1]);
    }
  }
}

void gpu_createSubPrimitive(
    GPUPrimitive      *const subPrimitive   ,
    GPUPrimitive const*const primitive      ,
    GPUTriangle  const*const clippedTriangle){
  assert(subPrimitive    != NULL);
  assert(primitive       != NULL);
  assert(clippedTriangle != NULL);
  subPrimitive->nofUsedVertices = VERTICES_PER_TRIANGLE;
  for(size_t a = 0; a < MAX_ATTRIBUTES; ++a){
    subPrimitive->interpolations[a] = primitive->interpolations[a];
    subPrimitive->types         [a] = primitive->types         [a];
  }

  //interpolate vertex attributes to subPrimitive
  for(size_t vertexIndex = 0; vertexIndex < VERTICES_PER_TRIANGLE; ++vertexIndex){
    // interpolate positions of vertices
    for(size_t componentIndex = 0; componentIndex<4; ++componentIndex){
      float const values[WEIGHTS_PER_BARYCENTRICS] = {
        primitive->vertices[0].gl_Position.data[componentIndex],
        primitive->vertices[1].gl_Position.data[componentIndex],
        primitive->vertices[2].gl_Position.data[componentIndex]
      };
      subPrimitive->vertices[vertexIndex].gl_Position.data[componentIndex] =
        gpu_noperspectiveInterpolate(values,clippedTriangle->coords[vertexIndex].data);
    }

    // interpolate vertex attributes
    for(size_t attributeIndex = 0; attributeIndex < MAX_ATTRIBUTES; ++attributeIndex){
      if(primitive->types[attributeIndex] == ATTRIB_EMPTY)continue;
      size_t const dimension = (size_t)primitive->types[attributeIndex];
      for(size_t componentIndex=0;componentIndex<dimension;++componentIndex){
        float const values[WEIGHTS_PER_BARYCENTRICS] = {
          ((float*)primitive->vertices[0].attributes[attributeIndex])[componentIndex],
          ((float*)primitive->vertices[1].attributes[attributeIndex])[componentIndex],
          ((float*)primitive->vertices[2].attributes[attributeIndex])[componentIndex]
        };
        ((float*)subPrimitive->vertices[vertexIndex].attributes[attributeIndex])[componentIndex] =
          gpu_noperspectiveInterpolate(values,clippedTriangle->coords[vertexIndex].data);
      }
    }
  }
}

void gpu_initPrimitive(
    GPUPrimitive *const primitive,
    GPU           const gpu      ){
  assert(primitive != NULL);
  for(size_t a = 0; a < MAX_ATTRIBUTES; ++a){
    primitive->interpolations[a] = gpu_getAttributeInterpolation(gpu,a);
    primitive->types[a] = gpu_getAttributeType(gpu,a);
  }
}

void gpu_initTriangle(
    GPUTriangle       *const triangle ,
    GPUPrimitive const*const primitive){
  assert(triangle  != NULL);
  assert(primitive != NULL);
  for(size_t v = 0; v < VERTICES_PER_TRIANGLE; ++v){
    copy_Vec4(triangle->positions+v,&primitive->vertices[v].gl_Position);
    for(size_t k = 0; k < WEIGHTS_PER_BARYCENTRICS; ++k)
      triangle->coords[v].data[k] = (float)(v==k);
  }
}

void cpu_drawTriangles(
    GPU    const gpu        ,
    size_t const nofVertices){
  GPUVertexPullerConfiguration const*const puller       = gpu_getActiveVertexPuller(gpu);
  VertexShader                       const vertexShader = gpu_getActiveVertexShader(gpu);
  size_t                             const width        = gpu_getViewportWidth     (gpu);
  size_t                             const height       = gpu_getViewportHeight    (gpu);
  // loop over all triangles
  for(size_t base = 0; base+VERTICES_PER_TRIANGLE-1<nofVertices; base += VERTICES_PER_TRIANGLE){
    GPUPrimitive primitive;
    gpu_initPrimitive(&primitive,gpu);
    // assembly primitive
    gpu_runPrimitiveAssembly(
        gpu                  ,
        &primitive           ,
        VERTICES_PER_TRIANGLE,
        puller               ,
        base                 ,
        vertexShader         );

    //perform primitive clipping
    GPUTriangle triangle;
    gpu_initTriangle(&triangle,&primitive);
    GPUTriangleList clippedTriangles;
    gpu_runTriangleClipping(&clippedTriangles,&triangle);

    //draw sub primitives
    for(size_t c = 0; c < clippedTriangles.nofTriangles; ++c){
      //create sub primitive using clipped triangle and original primitive
      GPUPrimitive subPrimitive;
      gpu_createSubPrimitive(&subPrimitive,&primitive,clippedTriangles.triangles+c);
      gpu_runPerspectiveDivision(&subPrimitive);
      gpu_runViewportTransformation(&subPrimitive,width,height);
      gpu_rasterizeTriangle(gpu,&subPrimitive,width,height);
    }
  }
}


