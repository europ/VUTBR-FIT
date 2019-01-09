/*!
 * @file 
 * @brief This file contains structures and function declaration important for rendering pipeline.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#pragma once

#include<stdlib.h>
#include<SDL2/SDL.h>
#include<assert.h>
#include"linearAlgebra.h"
#include"buffer.h"
#include"vertexPuller.h"
#include"program.h"
#include"fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This struct represents primitive - output of primitive assembly unit.
 * A primitive is composed of N vertices, interpolation type for each vertex attribute
 * and type of each vertex attribute.
 */
struct GPUPrimitive{
  GPUVertexShaderOutput vertices[VERTICES_PER_TRIANGLE];///< vertices of the primitive
  size_t                nofUsedVertices                ;///< number of used vertices 1 - point, 2 - line segment, 3 - triangle
  AttributeType         types[MAX_ATTRIBUTES]          ;///< types of vertex attributes
  InterpolationType     interpolations[MAX_ATTRIBUTES] ;///< interpolation types of vertex attributes
};

/**
 * @brief This structure represents triangle in clip-space.
 * This structure is used in clipping.
 * Positions of vertices are in clip-space.
 * Barycentric coords are in respect to original triangle (output of primitive assembly unit).
 */
struct GPUTriangle{
  Vec4 positions[VERTICES_PER_TRIANGLE]   ;///<positions of triangle vertices
  Vec3 coords   [WEIGHTS_PER_BARYCENTRICS];///<barycentric coords of vertices with respect to original triangle
};

/**
 * @brief This structure represents triangle list - input and output of clipping.
 */
struct GPUTriangleList{
  GPUTriangle triangles[MAX_CLIPPED_TRIANGLES];///<triangle slots
  size_t      nofTriangles                    ;///<number of used triangle slots
};

/**
 * @brief This enum represents frustum planes.
 */
enum FrustumPlane{
  LEFT  ,///< left   frustum plane
  RIGHT ,///< right  frustum plane
  BOTTOM,///< bottom frustum plane
  TOP   ,///< top    frustum plane
  NEAR  ,///< near   frustum plane
  FAR   ,///< far    frustum plane
};

typedef enum FrustumPlane FrustumPlane;///<shortcut

/*!
 * @brief This function computes gl_VertexID from vertex shader invocation using indexing
 *
 * @param indices pointer to 
 * @param vertexShaderInvocation id of vertex shader invocation
 *
 * @return return index of vertex if indexing is used, otherwise it returns vertexShaderInvocation
 */
VertexIndex gpu_computeGLVertexID(
    VertexIndex            const*const indices               ,
    VertexShaderInvocation       const vertexShaderInvocation);

/*!
 * @brief This function computes data pointer for vertex attribute "attribute" for vertex with index "gl_VertexID"
 *
 * @param head reading head of vertex attribute
 * @param gl_VertexID vertex index
 */
void const* gpu_computeVertexAttributeDataPointer(
    GPUVertexPullerHead const*const head       ,
    VertexIndex               const gl_VertexID);

/*!
 * @brief this function represents vertex puller
 *
 * It should do vertex puller job.
 * This job is construction of vertex using buffers.
 * 
 * @param vertex output vertex that is sent to vertex shader stage
 * @param pullerConfiguration configuration of vertex puller
 * @param vertexShaderInvocation id of vertex shader invocation
 */
void gpu_runVertexPuller(
    GPUVertexPullerOutput             *const vertex                ,
    GPUVertexPullerConfiguration const*const pullerConfiguration   ,
    VertexShaderInvocation             const vertexShaderInvocation);

/**
 * @brief This function performs primitive assembly.
 * Primitive assembly unit constructs primitive using vertices.
 * It executes vertex puller for each vertex of primitive.
 * It also executes vertex shader on each vertex.
 *
 * @param gpu GPU handle
 * @param primitive output primitive
 * @param nofPrimitiveVertices number of primitive vertices (3 for triangle, 2 for line segment, 1 for point)
 * @param puller vertex puller configuration
 * @param baseVertexShaderInvocation vertex shader invocation number of first vertex of primitive
 * @param vertexShader function pointer to selected vertexShader
 */
void gpu_runPrimitiveAssembly(
    GPU                                const gpu                       ,
    GPUPrimitive                      *const primitive                 ,
    size_t                             const nofPrimitiveVertices      ,
    GPUVertexPullerConfiguration const*const puller                    ,
    VertexShaderInvocation             const baseVertexShaderInvocation,
    VertexShader                       const vertexShader              );

/**
 * @brief This function performs frustum clipping on a single triangle.
 *
 * @param output triangle list
 * @param input input triangle
 */
void gpu_runTriangleClipping(
    GPUTriangleList      *const output,
    GPUTriangle     const*const input );

/**
 * @brief This function performs perspective division on primitive.
 * Perspective division divides x,y,z by homogeneous coord w.
 *
 * @param primitive input/output primitive
 */
void gpu_runPerspectiveDivision(
    GPUPrimitive *const primitive);

/**
 * @brief This function performs viewport transformation.
 * Viewport transformation scales primitive to resolution of screen.
 *
 * @param primitive input/output primitive
 * @param width width of screen in pixels
 * @param height height of screen in pixels
 */
void gpu_runViewportTransformation(
    GPUPrimitive      *const primitive,
    size_t             const width    ,
    size_t             const height   );

/**
 * @brief This function rounds down pixel coord to integer.
 * This function takes into account position of pixel center.
 * If fractional part of coord is <= pixel center -> this function returns integer part of coord.
 * If fractional part of coord is > pixel center -> this function return interger part of coord + 1.
 *
 * @param coord float coord of pixel
 *
 * @return closest integer
 */
size_t gpu_roundDownPixelCoord(
    float  const coord);

/**
 * @brief This function rounds up pixel coord to integer.
 * This function takes into account position of pixel center.
 * If fractional part of coord is < pixel center -> this function returns integer part of coord.
 * If fractional part of coord is >= pixel center -> this function return integer part of coord +1.
 *
 * @param coord float coord of pixel
 *
 * @return closest integer
 */
size_t gpu_roundUpPixelCoord(
    float  const coord);

/**
 * @brief This function computes screen-space barycentric coordinates of pixel.
 * This function expects triangle transformed by viewport transformation.
 *
 * @param coords output barycentric coords
 * @param pixelCenter coords of pixel
 * @param vertices triangle vertices
 * @param lines triangle lines
 */
void gpu_computeScreenSpaceBarycentrics(
    Vec3       *const coords                            ,
    Vec2  const*const pixelCenter                       ,
    Vec2        const vertices   [VERTICES_PER_TRIANGLE],
    Vec3        const lines      [EDGES_PER_TRIANGLE   ]);

/**
 * @brief This function computes triangle lines from triangle vertices.
 *
 * @param lines output lines
 * @param vertices input triangle vertices
 */
void gpu_computeTriangleLines(
    Vec3       lines   [EDGES_PER_TRIANGLE   ],
    Vec2 const vertices[VERTICES_PER_TRIANGLE]);

/**
 * @brief This function interpolates values using barycentric coords without perspective correction.
 *
 * @param values values
 * @param weights barycentric coords/weights
 *
 * @return 
 */
float gpu_noperspectiveInterpolate(
    float const values [WEIGHTS_PER_BARYCENTRICS],
    float const weights[WEIGHTS_PER_BARYCENTRICS]);

/**
 * @brief This function interpolates values using barycentric coords with perspective correction.
 *
 * @param values values (one value per triangle vertex)
 * @param weights barycentric coords/weights
 * @param homogeneousCoords homogeneous coords of each triangle vertex
 *
 * @return 
 */
float gpu_smoothInterpolate(
    float const values           [WEIGHTS_PER_BARYCENTRICS],
    float const weights          [WEIGHTS_PER_BARYCENTRICS],
    float const homogeneousCoords[WEIGHTS_PER_BARYCENTRICS]);

/**
 * @brief This function creates fragment and interpolates vertex attributes into fragment attributes according to selected interpolation type.
 *
 * @param fragment output fragment
 * @param primitive input primitive
 * @param barycentrics barycentric coords/weight of primitive
 * @param pixelCoord pixel coordinate of fragment
 */
void gpu_createFragment(
    GPUFragmentShaderInput      *const fragment    ,
    GPUPrimitive           const*const primitive   ,
    Vec3                   const*const barycentrics,
    Vec2                   const*const pixelCoord  );

/**
 * @brief This function restricts interval of possible values of x.
 *
 * @param minX minimal value of interval
 * @param maxX maximal value of interval
 * @param y y coordinate of line
 * @param edgeLine line of triangle edge
 */
void gpu_restrictLineBorders(
    float     *const minX    ,
    float     *const maxX    ,
    float      const y       ,
    Vec3 const*const edgeLine);
/**
 * @brief This function computes screen line start and end during rasterization of triangle.
 *
 * @param minX line start
 * @param maxX line end
 * @param y y coord of line
 * @param triangleLines edge lines of a triangle
 */
void gpu_computeLineBorders(
    float      *const minX                             ,
    float      *const maxX                             ,
    float       const y                                ,
    Vec3  const       triangleLines[EDGES_PER_TRIANGLE]);

/**
 * @brief This function performs per-fragment operations.
 * Depth test is only per-fragment operation in this project.
 *
 * @param gpu GPU handle
 * @param fragment fragment
 * @param x x coord of pixel
 * @param y y coord of pixel
 */
void gpu_perFragmentOperations(
    GPU                           const gpu     ,
    GPUFragmentShaderOutput const*const fragment,
    size_t                        const x       ,
    size_t                        const y       );

/**
 * @brief This function inits primitive.
 *
 * @param primitive A primitive that will be initialized.
 * @param gpu GPU handle
 */
void gpu_initPrimitive(
    GPUPrimitive *const primitive,
    GPU           const gpu      );

/**
 * @brief This functions creates sub primitive using clipped triangle and original triangle.
 *
 * @param subPrimitive output sub primitive
 * @param primitive original primitive
 * @param clippedTriangle clipped triangle
 */
void gpu_createSubPrimitive(
    GPUPrimitive      *const subPrimitive   ,
    GPUPrimitive const*const primitive      ,
    GPUTriangle  const*const clippedTriangle);

/**
 * @brief This function inits triangle using primitive.
 *
 * @param triangle output triangle
 * @param primitive input primitive
 */
void gpu_initTriangle(
    GPUTriangle       *const triangle ,
    GPUPrimitive const*const primitive);

/**
 * @brief This function rasterizes one triangle.
 *
 * @param gpu GPU handle
 * @param primitive input primitive
 * @param width screen width in pixels
 * @param height screen height in pixels
 */
void gpu_rasterizeTriangle(
    GPU                const gpu      ,
    GPUPrimitive const*const primitive,
    size_t             const width    ,
    size_t             const height   );

/**
 * @brief This function draw array of triangles.
 * This function invokes whole rendering pipeline.
 * It is necessary to active selected vertex puller and to active selected shader program before this function is called.
 *
 * @param gpu GPU handle
 * @param nofVertices number of vertices that will be drawn.
 */
void cpu_drawTriangles(
    GPU    const gpu        ,
    size_t const nofVertices);

#ifdef __cplusplus
}
#endif

/**
 * \mainpage Izg project 2016 - 2017.
 * 
 * \section zadani Zadání projektu do předmětu IZG 2016 - 2017.
 *
 * \tableofcontents
 *
 * \image html images/bunny.png "Výstup projektu" width=5cm
 *
 * Vašim úkolem je naimplementovat softwarový vykreslovací řetězec (pipeline).
 * Pomocí vykreslovacího řetězce vizualizovat model králička s phongovým osvělovacím modelem a phongovým stínováním.
 * V tomto projektu nebudeme pracovat s GPU, ale budeme se snažit simulovat její práci.
 * Cílem je pochopit jak vykreslovací řetěc funguje, z čeho je složený a jaká data se v něm pohybují.
 *
 * Váš úkol je složen ze tří částí: napsat kódy pro CPU stranu, pro virtuální GPU stranu a napsat shadery.
 * Musíte doplnit implementace několika funkcí a rozchodit kreslení modelu králička.
 * Funkce mají pevně daný interface (Vstupy a výstupy).
 * Seznam všech úkolů naleznete zde \link todo.html \endlink.
 * Úkoly týkající se pouze CPU strany naleznete zde \link cpu_side CPU\endlink.
 * Úkoly týkající se pouze GPU strany naleznete zde \link gpu_side GPU\endlink.
 * Úkoly týkající se pouze shaderů naleznete zde \link shader_side Shadery\endlink.
 *
 * Každý úkol má přiřazen akceptační test, takže si můžete snadno ověřit funkčnosti vaší implementace.
 *
 * V projektu je přítomen i příklad vykreslení jednoho trojúhelníku: v \link TriangleExample Triangle Example\endlink.
 * Tento příklad můžete využít pro inspiraci a návod jak napsat cpu stranu a shadery.
 *
 * Pro implementace třetí části - gpu části využijte teorii na této stránce, doxygen dokumentaci a teorii probíranou na přednáškách.
 *
 * \section rozdeleni Rozdělení
 *
 * Projekt je rozdělen do několika podsložek:
 *
 * <b>student/</b> Tato složka obsahuje soubory, které využijete při implementaci projektu.
 * Složka obsahuje soubory, které budete odevzávat a podpůrné knihovny.
 * Všechny soubory v této složce jsou napsány v jazyce C abyste se mohli podívat jak jednotlivé části fungují.
 *
 * <b>tests/</b> Tato složka obsahuje akceptační a performanční testy projektu.
 * Akceptační testy jsou napsány s využitím knihovny catch.
 * Testy jsou rozděleny do testovacích případů (TEST_CASE). Daný TEST_CASE testuje jednu podčást projektu.
 *
 * <b>examples/</b> Tato složka obsahuje kompletní příklad kreslení jednoho trojúhelníku pomocí virtuální gpu API.
 *
 * <b>doc/</b> Tato složka obsahuje doxygen dokumentaci projektu.
 * Můžete ji přegenerovat pomocí příkazu doxygen spuštěného v root adresáři projektu.
 *
 * <b>3rdParty/</b> Tato složka obsahuje hlavičkový soubor pro unit testy - catch.hpp.
 * Z pohledu projektu je nezajímavá.
 * Catch je knihovna složená pouze z hlavičkového souboru napsaného v jazyce C++.
 * Poskytuje několik užitečných maker pro svoji obsluhu.
 * TEST_CASE - testovací případ (například pro testování jedné funkce).
 * WHEN - toto makro popisuje způsob použití (například volání funkce s parametery nastavenými na krajní hodnoty).
 * REQUIRE - toto makro vyhodnotí podmínku a případně vypíše chybu (například chcete ověřit, že vaše funkce vrátila správnou hodnotu).
 *
 * <b>CMakeModules/</b> Tato složka obsahuje skripty pro CMake.
 * Z pohledu projektu je nezajímavá.
 *
 * <b>gpu/</b> Tato složka obsahuje implementace virtuální GPU (hlavně management paměti).
 * Z pohledu projektu je nezajímavá. Virtuální GPU je napsáno v jazyce C++.
 *
 * <b>images/</b> Tato složka obsahuje doprovodné obrázky pro dokumentaci v doxygenu.
 * Z pohledu projektu je nezajímavá.
 *
 * Složka student/ obsahuje soubory, které se vás přímo týkají:
 *
 * <b>student_cpu.c</b> obsahuje cpu stranu vykreslování - tento soubor budete editovat.
 *
 * <b>student_pipeline.c</b> obsahuje zobrazovací řetězec - tento soubor budete editovat.
 *
 * <b>student_shader.c</b> obsahuje shadery - tento soubor budete editovat.
 *
 * <b>buffer.h</b> slouží pro práci s buffery.
 *
 * <b>program.h</b> slouží pro práci s shader programy.
 *
 * <b>vertexPuller.h</b> slouží pro práci s vertex puller objekty.
 *
 * <b>uniforms.h</b> slouží pro práci s uniformními proměnnými.
 *
 * <b>linearAlgebra.h</b> slouží pro vektorové a maticové operace.
 *
 * <b>bunny.h</b> obsahuje model králíčka.
 * 
 * <b>gpu.h</b> obsahuje interface pro virtuální GPU (gpu paměť).
 *
 * Soubory, které se týkají projektu, ale ne přímo studentské práce:
 *
 * <b>main.c</b> obsahuje main funkci.
 *
 * <b>camera.h</b> obsahuje funkce pro výpočet orbit manipulátoru kamery.
 *
 * <b>mouseCamera.h</b> obsahuje manipulaci kamery pomoci myši.
 *
 * <b>swapBuffers.h</b> obsahuje funkci pro přepnutí vykreslovacích snímků.
 *
 * <b>fwd.h</b> obsahuje forward deklarace.
 *
 * Funkce s předponou cpu_ můžou být volány pouze na straně CPU, v souboru student_cpu.c.
 * Funkce s předponou gpu_ můžou být volány pouze na straně GPU, v souboru student_pipeline.c.
 * Funkce s předponou shader_ můžou být volány pouze v rámci shaderů (vertex i fragment), v souboru student_shader.c.
 * Funkce s předponou vs_ můžou být volány pouze v rámci vertex shaderu, v souboru student_shader.c.
 * Funkce s předponou fs_ můžou být volány pouze v rámci fragment shaderu v souboru student_shader.c.
 * Funkce bez předpony můžou být volány na CPU, GPU tak v rámci shaderu.
 *
 * Struktury, které se vyskytují pouze na GPU straně jsou uvozeny prefixem GPU.
 * Struktury bez předpony lze využít jak na CPU tak GPU straně či v shaderu.
 *
 * Projekt je postaven nad filozofií OpenGL. Spousta funkcí má podobné jméno.
 *
 * \section teorie Teorie
 *
 * Typické grafické API (OpenGL/Vulkan/DirectX) je složeno ze 2 částí: CPU a GPU strany.
 *
 * CPU strana se obvykle stará o tyto úkoly:
 *  - Příprava dat pro kreslení (modely, textury, matice, ...)
 *  - Upload dat na GPU a nastavení GPU
 *  - Spuštění vykreslení
 *
 * GPU strana je složena ze dvou částí: video paměti a zobrazovacího řetězce.
 * Vykreslovací řetězec se skládá z několika částí:
 * - Vertex Puller
 * - Vertex Processor
 * - Primitive Assembly
 * - Clipping
 * - Rasterize
 * - Fragment Processor
 * - Per-Fragment Operations
 *
 * \subsection terminologie Terminologie
 *
 * \b Vertex je kolekce několika vertex atributů.
 * Tyto atributy mají svůj typ a počet komponent.
 * Každý vertex atribut má nějaký význam (pozice, hmotnost, texturovací koordináty), které mu přiřadí programátor/modelátor.
 * Z několika vrcholů je složeno primitivum (trojúhelník, úsečka, ...)
 *
 * <b>Vertex atribut</b> je jedna vlastnost vrcholu (pozice, normála, texturovací koordináty, hmotnost, ...).
 * Atribut je složen z 1,2,3 nebo 4 komponent daného typu (FLOAT, INT, ...).
 * Sémantika atributu není pevně stanovena (atributy mají pouze pořadové číslo - attribIndex) a je na každém programátorovi/modelátorovi, jakou sémantiku atributu přidělí.
 * \image html images/primitive.svg "Vertex attributes" width=10cm
 *
 * \b Fragment je kolekce několika atributů (podobně jako Vertex).
 * Tyto atributy mají svůj typ a počet komponent.
 * Fragmenty jsou produkovány resterizací, kde jsou atributy fragmetů vypočítány z vertex atributů pomocí interpolace.
 * Fragment si lze představit jako útržek původního primitiva.
 *
 * <b>Fragment atribut</b> je jedna vlastnost fragmentu (podobně jako vertex atribut).
 *
 * <b>Interpolace</b> Při přechodu mezi vertex atributem a fragment atributem dochází k interpolaci atributů.
 * Atributy jsou váhovány podle pozice fragmentu v trojúhelníku (barycentrické koordináty).
 * \image html images/interpolation.svg "Vertex attribute interpolation" width=10cm
 *
 * <b>Vertex Processor</b> (často označován za Vertex Shader) je funkční blok, který je vykonáván nad každým vertexem.
 * Jeho vstup i výstup je Vertex. Výstupní vertex má obvykle jiné vertex atributy než vstupní vertex.
 * Výstupní vertex má vždy atribut - gl_Position (pozice vertexu v clip-space).
 * Vertex Processor se obvykle stará o transformace vrcholů modelu (posuny, rotace, projekce).
 * Jelikož Vertex Processor pracuje po vrcholech, je vhodný pro efekty jako vlnění na vodní hladině, displacement mapping apod.
 * Vertex Processor má informace pouze o jednom vrcholu v daném čase (neví nic o sousednostech vrcholů).
 * Vertex processor je programovatelný.
 *
 * <b>Fragment Processor</b> (často označován za Fragment Shader/Pixel Shader) je funkční blok, který je vykonáván nad každým fragmentem.
 * Jeho vstup i výstup je Fragment. Výstupní fragment má obykle jiné attributy.
 * Fragment processor je programovatelný.
 *
 * <b>Shader</b> je program/funkce, který běží na některé z programovatelných částí zobrazovacího řetezce.
 * Shader má vstupy a výstupy, které se mění s každou jeho invokací.
 * Shader má také vstupy, které zůstávají konstantní a nejsou závislé na číslu invokace shaderu.
 * Shaderů je několik typů, v tomto projektu se používají pouze 2 - vertex shader a fragment shader.
 * V tomto projektu jsou shadery reprezentovány pomocí standardních Cčkovských funkcí.
 *
 * <b>Vertex Shader</b> je program, který běží na vertex processoru.
 * Jeho vstupní interface obsahuje: vertex, uniformní proměnné a další proměnné (číslo vrcholu gl_VertexID, ...).
 * Jeho výstupní inteface je vertex, který vždy obsahuje proměnnou gl_Position - pozici vertexu v clip-space.
 *
 * <b>Fragment Shader</b> je program, který běží na fragment processoru.
 * Jeho vstupní interface obsahuje: fragment, uniformní proměnné a proměnné (souřadnici fragmentu ve screen-space gl_FragCoord, ...).
 * Jeho výstupní interface je fragment.
 *
 * <b>Shader Program</b> je kolekce programů, které běží na programovatelných částech zobrazovacího řetězce.
 * Obsahuje vždy maximálně jeden shader daného typu.
 * V tompto projektu je program reprezentován pomocí dvou ukazatelů na funkce.
 * \image html images/shader_program.svg "This shader program is composed of vertex shader and fragment shader" width=10cm
 *
 * <b>Buffer</b> je lineární pole dat ve video paměti na GPU.
 * Do bufferů se ukládají vertex attributy vextexů modelů nebo indexy na vrcholy pro indexované vykreslování.
 *
 * <b>Uniformní proměnná</b> je proměná uložená v konstantní paměti GPU. Všechny programovatelné bloky zobrazovacího řetězce z nich mohou pouze číst.
 * Jejich hodnota zůstává stejná v průběhu kreslení (nemění se v závislosti na číslu vertexu nebo fragmentu). Jejich hodnodu lze změnit z CPU strany pomocí funkcí jako je
 * uniform1f, uniform1i, uniform2f, uniformMatrix4fv apod. Uniformní proměnné jsou vhodné například pro uložení transformačních matic nebo uložení času.
 *
 * <b>Vertex Puller</b> se stará o přípravů vrcholů.
 * K tomuto účelu má tabulku s nastavením.
 * Vertex puller si můžete představit jako sadu čtecích hlav.
 * Každá čtecí hlava se stará o přípravu jednoho vertex atributu.
 * Mezi nastavení čtecí hlavy patří: ukazatel na začátek bufferu, offset a krok.
 * Vertex puller může obsahovat indexování.
 *
 * <b>Zobrazovací řetězec</b> je obvykle kus hardware na grafické kartě, který se stará o vyreslování.
 * Grafická karta je složena ze dvou částí: paměti a zobrazovacího řetězce.
 * V paměti se nacházejí buffery, textury, uniformní proměnné, programy, nastavení vertex pulleru a framebuffery.
 * Pokud se spustí kreslení N vrcholů, je vertex puller spuštěn N krát a sestaví N vrcholů.
 * Nad každým vrcholem je puštěn vertex shader.
 * Výstupem vertex shaderu je nový vrchol.
 * Blok sestavení primitiv "si počká" na 3 vrcholy z vertex shaderu (pro trojúhelník) a vloží je do jedné struktury.
 * Blok clipping ořeže trojúhelníky pohledovým jehlanem.
 * Následuje perspektivní dělení, které vydělí pozice vertexů homogenní složkou.
 * Poté následuje viewport transformace, která podělené vrcholy transformuje do rozlišení obrazovky.
 * Rasterizace trojúhelníky nařeže na fragmenty a interpoluje vertex atributy.
 * Nad každým fragmentem je spuštěn fragment shader.
 * Než jsou fragmenty zapsány zpět do paměti GPU (framebufferu) jsou provedeny per-fragment operace (zjištění viditelnosti fragmentů podle hloubky uchované v depth bufferu).
 * \image html images/rendering_pipeline.svg "Simplified rendering pipeline" width=10cm
 *
 * <b>ObjectID</b> je číslo odkazující se na jeden konkrétní objekt na GPU.
 * Každy buffer, program, vertex puller má přiřazeno/rezervováno takové číslo (BufferID, ProgramID, VertexPullerID).
 *
 * <b>Uniformní lokace</b> je číslo, které reprezentuje jednu uniformní proměnnou.
 *
 * <b>VertexShaderInvocation</b> je pořadové číslo vyvolání vertex shaderu.
 *
 * <b>gl_VertexID</b> je číslo vrcholu, které je vypočítáno pomocí indexování a pořadového čísla vyvolání vertex shaderu.
 *
 * <b>Indexované kreslení</b> je způsob snížení redundance dat s využitím indexů na vrcholy.
 * \image html images/drawElements.svg "Difference between indexed and non-indexed drawing" width=10cm
 *
 * \section sestaveni Sestavení
 *
 * Projekt byl testován na Ubuntu 14.04, Ubuntu 16.04, Visual Studio 2013, Visual Studio 2015.
 * Projekt vyžaduje 64 bitové sestavení.
 * Projekt využívá build systém <a href="https://cmake.org/">CMAKE</a>.
 * CMake je program, který na základně konfiguračních souborů "CMakeLists.txt" vytvoří "makefile" v daném vývojovém prostředí.
 * Dokáže generovat makefile pro Linux, mingw, solution file pro Microsoft Visual Studio, a další.
 * Postup:
 * -# stáhnout projekt
 * -# rozbalit projekt
 * -# ve složce build spusťte "cmake-gui .." případně "ccmake .."
 * -# vyberte si překladovou platformu (64 bit).
 * -# configure
 * -# generate
 * -# make nebo otevřete vygenerovnou Microsoft Visual Studio Solution soubor.
 *
 * Projekt vyžaduje pro sestavení knihovnu <a href="https://www.libsdl.org/download-2.0.php">SDL2</a>.
 * Ta je pro 64bit build ve Visual Studiu přibalena.
 * Cesty na hlavičkové soubory a libs jsou nastaveny pomocí checkboxu USE_PREBUILD_LIB_PACKAGE.
 * Pod Linuxem (debiánovské distribuce) můžete knihovnu nainstalovat pomocí: "sudo apt-get install libsdl2-dev"
 *
 * \section spousteni Spouštění
 *
 * Projekt je možné po úspěšném přeložení pustit přes aplikaci <b>izgProject2016</b>.
 * Projekt akceptuje několik argumentů příkazové řádky:
 * - <b>-c ../tests/output.bmp</b> spustí akceptační testy, soubor odkazuje na obrázek s očekávaným výstupem.
 * - <b>-p</b> spustí performanční test.
 * 
 * \section ovladani Ovládání
 * Program se ovládá pomocí myši a klávesnice:
 * - stisknuté levé tlačítko myši + pohyb myší - rotace kamery
 * - stisknuté pravé tlačítko myši + pohyb myší - přiblížení kamery
 * - "n" - přepne na další scénu/metodu
 *   "p" - přepne na předcházející scénu/metodu
 *
 * \section odevzdavani Odevzdávání
 *
 * Odevzdávejte pouze soubory student_pipeline.c, student_cpu.c a student_shader.c. Soubory zabalte do archivu proj.zip. Po rozbalení archivu se <b>NESMÍ</b> vytvořit žádná složka. Příkazy pro ověření na Linuxu: zip proj.zip student_pipeline.c student_cpu.c student_shader.c, unzip proj.zip.
 * Studenti pracují na řešení projektu samostatně a každý odevzdá své vlastní řešení.
 * Poraďte si, ale řešení vypracujte samostatně!
 *
 * \section hodnoceni Hodnocení
 *
 * Množství bodů, které dostanete, je odvozeno od množství splněných akceptačních testů a podle toho, zda vám to kreslí správně (s jistou tolerancí kvůli nepřesnosti floatové aritmetiky).
 * Automatické opravování má k dispozici větší množství akceptačních testů (kdyby někoho napadlo je obejít).
 * Pokud vám aplikace spadne v rámci testů, dostanete 0 bodů.
 * Pokud aplikace nepůjde přeložit, dostanete 0 bodů.
 *
 * \section soutez Soutěž
 *
 * Pokud váš projekt obdrží plný počet bodů, bude zařazen do soutěže o nejrychlejší implementaci zobrazovacího řetězce.
 * Můžete přeimplementovat cokoliv v odevzdávaných souborech pokud to projde akceptačními testy a kompilací.
 *
 * \section zaver Závěrem
 * Ať se dílo daří a ať vás grafika alespoň trochu baví!
 * V případě potřeby se nebojte zeptat (na fóru nebo napište přímo vedoucímu projektu imilet@fit.vutbr.cz).
 */


