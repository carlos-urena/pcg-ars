// *********************************************************************
// **
// ** Conjunto de funciones auxiliares (declaraciones)
// **
// ** Copyright (C) 2014-2022 Carlos Ureña
// **
// ** This program is free software: you can redistribute it and/or modify
// ** it under the terms of the GNU General Public License as published by
// ** the Free Software Foundation, either version 3 of the License, or
// ** (at your option) any later version.
// **
// ** This program is distributed in the hope that it will be useful,
// ** but WITHOUT ANY WARRANTY; without even the implied warranty of
// ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// ** GNU General Public License for more details.
// **
// ** You should have received a copy of the GNU General Public License
// ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
// **
// *********************************************************************

#pragma once

// includes de la librería estandard y otras

#include <cctype>     // toupper
#include <algorithm>  // std::min y std::max
#include <cassert>    // assert
#include <cctype>     // toupper
#include <cstdlib>
#include <cmath>      // std::sin, std::cos, etc...
#include <fstream>    // std::ifstream
#include <iostream>   // std::cout
#include <iomanip>    // std::setprecision, std::fixed, etc....
#include <map>        // std::map<K,V>
#include <string>     // std::string
#include <vector>     // std::vector
#include <chrono>     // std::chrono::time_point, std::chrono::duration (para medir FPS)
#include <random>     // generadores de numeros aleatorios ( uniform_real_distribution)
#include <set>        // std::set

// includes de OpenGL/GLEW/GLFW (dependen del S.O.)
// ver: https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor

#ifdef __linux__
#include <GL/glew.h>   
#include <GLFW/glfw3.h>  
#include <GL/glu.h>
#define NOMBRE_OS "Linux"

#elif __APPLE__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" // ignora advertencias por uso de funciones obsoletas
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h> 
#define NOMBRE_OS "macOS"

#elif _WIN32
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <jpeglib.h>
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__ // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
#endif
#define NOMBRE_OS "Windows"

#else
#pragma error "No está definido el símbolo '__APPLE__' ni '__linux__', ni '_WIN32'"
#endif

//------------------------------------------------------
// por si acaso ...(en windows se usa)
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993751
#endif


// includes de GLM
#define GLM_ENABLE_EXPERIMENTAL       // ¿ necesario en macOs ?
#include <glm/glm.hpp>                // incluye cosas básicas de GLM (vectores y matrices)
#include <glm/gtc/type_ptr.hpp>       // conversion de vectores y matrices hacia/desde 'float *' con 'value_ptr' (https://glm.g-truc.net/0.9.9/api/a00305.html)
#include <glm/gtc/matrix_access.hpp>  // acceso a filas y columnas de una matriz, con glm::row( mat4 )
#include <glm/gtx/string_cast.hpp>    // función 'to_string'
#include <glm/gtx/transform.hpp>      // funciones 'scale', 'rotate', 'translate' sin especificar una matriz original (https://glm.g-truc.net/0.9.9/api/a00362.html)
#include <glm/gtx/norm.hpp>           // funciones de cálculo de longitud y longitud al cuadrado de un vector
#include <glm/gtc/matrix_inverse.hpp> // función inverse transpose


// includes en 'recursos/include'

//include "tup_mat.h"
#include "lector-jpg.h"
#include "fbo.h"
#include "cauce-3d.h"

//using namespace tup_mat ;

// definiciones para índices de los ejes cartesianos
constexpr unsigned X = 0, Y = 1, Z = 2 ;


// ---------------------------------------------------------------------
// número aleatorio entre 0 y 1
inline float alea01() { return float(std::rand())/float(RAND_MAX); }

// ---------------------------------------------------------------------
// quita el path de un nombre de archivo con path
std::string QuitarPath( const std::string & path_arch ) ;

// ---------------------------------------------------------------------
// traza la ejecución
#define traza() { std::cout << "archivo: " << __FILE__ << ", linea: " << __LINE__ << std::endl << std:: flush ;}

// ---------------------------------------------------------------------
// log macro log(a,b,c,..)
// see: https://gist.github.com/carlos-urena/6290b4fa0a5c50efd7bd2c4f0b5c54d8

inline void log_func_aux()
{
    using namespace std ;
    cout << endl ;
}

template<typename T, typename ...Tipos> 
inline void log_func_aux( const T & first, const Tipos & ...parametros )
{
   using namespace std ;
   cout << first << " ";
   log_func_aux( parametros... );
}

template<typename ...Tipos> 
inline void log_func( const std::string & file, const std::string & func_name, const int line_num, 
           const Tipos & ...parametros )
{
   using namespace std ;
   cout << QuitarPath( file ) << ", " << func_name << "(...), " << line_num << ": " ; 
   log_func_aux<Tipos...>( parametros... );
}

#define log( ... )  log_func( __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__ )

// ---------------------------------------------------------------------
// comprobación de errores de OpenGL
#define CError() CompruebaErrorOpenGL(__FILE__,__LINE__)
void CompruebaErrorOpenGL( const char * nomArchivo, int linea ) ;

// ---------------------------------------------------------------------
/// @brief gestion de un error
#define Error(msg)   GestionarError(msg,__FILE__,__LINE__)
void GestionarError( const char * msg, const char * nomArchivo, int linea ) ;

// ----------------------------------------------------------------------
/// @brief calcula la versión de OpenGL a partir de la cadena obtenida con glGetString(GL_VERSION)
void CalculaVersion(  const char *str, unsigned & major, unsigned & minor );

// ---------------------------------------------------------------------
/// @brief true solo si el rendering context activo soporta al menos una versión de openGL
bool SoportaGL( const int min_major, const int min_minor ) ;

// ---------------------------------------------------------------------
/// @brief true solo si el rendering context activo soporta al menos una versión de GLSL
bool SoportaGLSL( const int min_major, const int min_minor ) ;

//----------------------------------------------------------------------
/// @brief aborta si no se soporta una versión de OpenGL
void ExigirVersionGL( const int min_major, const int min_minor, const char * msg ) ;

//----------------------------------------------------------------------
/// @brief comprobar si está GLEW, si está inicializarlo, en otro caso error y aborta
///
void InicializaGLEW(  ) ;

// ----------------------------------------------------------------------------
/// @brief fijar el icono que se usa para la ventana GLFW, usando imagen UGR
void FijarIconoVentanaGLFW( GLFWwindow * glfw_window ) ;

// ---------------------------------------------------------------------
/// @brief GLFW: especificar 'hints' para inicialización del rendering
/// @brief context de la ventana GLFW (se puede pedir uOpenGL 4 aquí)
void ConfigurarGLFW() ;

// ----------------------------------------------------------------------------------
/// @brief calcula posicion y tamaño de la ventana GLFW en función de la resolucion del escritorio
void TamPosVentanaGLFW( int & tamx, int & tamy, int & posx, int & posy );

// ----------------------------------------------------------------------------
/// @brief función que gestiona un error en GLFW, se fija con 'glfwSetErrorCallback'
/// @brief se imprimer código, mensaje, y se aborta
void ErrorGLFW( int codigo, const char * mensaje );

// ---------------------------------------------------------------------
/// @brief escribe en stdout características de opengl
void InformeOpenGL() ;

// ---------------------------------------------------------------------
/// @brief devuelve 'true' si está disponible el buffer de acumulación para
/// @brief usar super-sampling de 'fuerza bruta'
bool BufferAcumulacionDisponible( GLint & nb_accum_r, GLint & nb_accum_g, GLint & nb_accum_b );

// ---------------------------------------------------------------------
/// @brief crear una ventana glut centrada en el escritorio
void CrearVentanaGLUT( const char * titulo, const  float fr_alto ) ;

// ----------------------------------------------------------------------------
/// @brief funciones auxiliares para dibujar los ejes
void DibujarCono( GLdouble base, GLdouble height, GLdouble slices, GLdouble stacks );
void DibujarEsfera( GLdouble radius, GLdouble slices, GLdouble stacks ) ;
void DibujarCilindro( GLdouble radius, GLdouble height, int slices, int stacks );
void DibujarEjeZ( Cauce3D & cauce );
void DibujarEjesSolido( Cauce3D & cauce );
void DibujarEjesLineas( Cauce3D & cauce );

// -----------------------------------------------------------------------------
/// @brief imprimie en 'cout' una línea con información de la frecuencia con la que se llama
/// @brief a la función
void ImprimirFPS();

// -----------------------------------------------------------------------------
/// @brief dibujar una cruz
void DibujarCruz( Cauce3D & cauce, const float d );

// -----------------------------------------------------------------------------
/// @brief dibuja un FBO en un rectángulo en la parte superior izquierda
void DibujarFBO( Cauce3D & cauce, Framebuffer & fbo );

// -----------------------------------------------------------------------------
/// @brief calcula (la primera vez) y devuelve el path hasta la carpeta de materiales 

std::string PathCarpetaMateriales(  );

// -----------------------------------------------------------------------------
/// @brief calcula (la primera vez) y devuelve el path hasta la carpeta de fuentes de los shaders

std::string PathCarpetaFuentesShaders(  );

// -----------------------------------------------------------------------------
/// @brief buscar un archivo en la carpeta de materiales o en la carpeta de archivos de alumno

std::string BuscarArchivo( const std::string & nombre_arch, const std::string & subcarpeta );

// -----------------------------------------------------------------------------

inline constexpr int CodificaBytes( const unsigned char b0, const unsigned char b1, const unsigned char b2)
{
   return int(b0) + ( int(0x100U)*int(b1) ) + ( int(0x10000U)*int(b2) ) ;
}

// --------------------------------------------------------------------------
/// @brief define la imagen que se usará para el icono de la ventana glfw
/// @brief (únicamente en Windows y Linux, en macOs no hace nada)

void FijarImagenIconoVentana( GLFWwindow * ventana_glfw );

// ---------------------------------------------------------------------
/// @brief matriz del viewport  (deja las Z igual: entre -1 y 1) y su inversa
glm::mat4 MAT_Viewport( int org_x, int org_y, int ancho, int alto );
glm::mat4 MAT_Viewport_inv( int org_x, int org_y, int ancho, int alto );

// ---------------------------------------------------------------------
/// @brief matriz de vista:
glm::mat4 MAT_Vista( const glm::vec3 eje[3], const glm::vec3 & origen );
glm::mat4 MAT_Vista_Inv( const glm::vec3 eje[3], const glm::vec3 & origen );

// ---------------------------------------------------------------------
// Funciones auxiliares de compilar/enlazar shaders y objetos programa

// ---------------------------------------------------------------------
/// @brief Devuelve una cadena que describe el tipo de un shader
/// @param tipo_shader - tipo de shader (GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER)
/// @return cadena con el nombre del tipo.
///
std::string NombreTipoShader( GLenum tipo_shader );

// ---------------------------------------------------------------------
/// @brief compila el fuente de un shader y lo adjunta al programa, si 
/// @brief hay errores o warnings muuestra el log. Si hay errores aborta.
///
/// @param id_prog - identificador del programa al que se adjunta el shader
/// @param tipo_shader - tipo de shader (GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER)
/// @param fuente_shader_opc - código fuente del shader (o cadena vacía si se lee de un archivo)
/// @param nombre_archivo - nombre del archivo fuente (o cadena vacía si se pasa el código fuente en 'fuente_shader_opc')
/// @return identificador del shader, ya compilado y adjuntado al programa
///
GLuint CompilarAdjuntarShader( GLuint id_prog, GLenum tipo_shader,
                               const std::string & fuente_shader_opc,
                               const std::string & nombre_archivo ) ;

// ---------------------------------------------------------------------
/// @brief Enlaza un objeto programa con los shaders previamente adjuntados. 
/// @brief Si hay errores o warnings muestra el log. Si hay errores aborta.
///
/// @param id_prog - identificador del programa a enlazar
///
void EnlazarObjetoPrograma( GLuint id_prog ) ;

// ----------------------------------------------------------------------

/// @brief Crea un objeto programa, lo compila y lo enlaza a partir de los 
/// @brief fuentes del vertex shader y el fragment shader.
/// @param vs_src - cadena (std::string) con el código fuente del vertex shader 
/// @param fs_src - cadena (std::string) con el código fuente del fragment shader
/// @return 
GLuint CrearObjetoPrograma( const std::string & vs_src, const std::string & fs_src );

// ---------------------------------------------------------------------
// variables de gestión del buffer de errores o warnings al compilar/enlazar shaders...

constexpr GLsizei report_buffer_long_max = 1024L*10L ; // capacidad del buffer 
extern GLsizei    report_buffer_long ; // longitud del texto en el buffer
extern GLchar     report_buffer[ report_buffer_long_max ] ; // cadena con errores

// ---------------------------------------------------------------------

/// @brief visualiza un cuadrado de lado 2 centrado en 0, con coord de text entre 0 y 1.
/// @brief Usa el objeto programa y el framebuffer activo actualmente, tmb el modo de envío, color, etc...
///
void VisualizarCuadrado();

// ---------------------------------------------------------------------

/// @brief Devuelve true si el cauce necesita que se envíen parches en lugar de triángulos
///
bool SustituirTriangulosPorParches() ;
