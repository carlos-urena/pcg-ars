// *********************************************************************
// **
// ** Rutinas auxiliares:
// ** - Gestión de errores en general
// ** - Comprobación de errores de OpenGL (implementación)
// ** - Inicialización de GLEW,
// ** - Comprobación de la versión de OpenGL
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

#include <sys/types.h>  // para buscar carpetas con 'stat'
#include <sys/stat.h>   // para buscar carpetas con 'stat'

#ifndef _WIN32
#include <unistd.h>      // para 'getcwd', stat y otros ...
//#else 
//#include <sys/stat.h> // stat() en msvc
#endif 

#include "utilidades.h"
#include "vaos-vbos.h"
#include "aplic-3d.h"

// *********************************************************************
// gestion de errores

void GestionarError( const char * msg, const char * nomArchivo, int linea )
{
   std::cout
         << std::endl
         << "-----------------------------------------------------------" << std::endl
         << "Error detectado:" << std::endl
         << "   descripción : " << msg  << "." << std::endl
         << "   archivo     : " << nomArchivo << std::endl
         << "   línea       : " << linea << std::endl
         << "-----------------------------------------------------------" << std::endl
         << "programa abortado." << std:: endl
         << std::flush ;

    exit(1) ;
}



// -----------------------------------------------------------------------------
// códigos y descripciones de errores detectado por 'glGetError'

const unsigned numErrors = 6 ;

const GLenum errCodes[numErrors] =
{
   GL_NO_ERROR ,
   GL_INVALID_ENUM ,
   GL_INVALID_VALUE ,
   GL_INVALID_OPERATION ,
   GL_INVALID_FRAMEBUFFER_OPERATION ,  // REVISAR (pq no está definido con glfw?)
   GL_OUT_OF_MEMORY
} ;

const char * errDescr[numErrors] = 
{
   "Error when trying to report an error: no error has been recorded",
   "An unacceptable value is specified for an enumerated argument",
   "A numeric argument is out of range",
   "The specified operation is not allowed in the current state",
   "The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from 'glCheckFramebufferStatus' is not GL_FRAMEBUFFER_COMPLETE)",
   "There is not enough memory left to execute the command"
} ;

const char * errCodeString[numErrors] =
{
   "GL_NO_ERROR",
   "GL_INVALID_ENUM",
   "GL_INVALID_VALUE",
   "GL_INVALID_OPERATION",
   "GL_INVALID_FRAMEBUFFER_OPERATION",
   "GL_OUT_OF_MEMORY"
} ;

// ---------------------------------------------------------------------
// devuelve descripción de error dado el código de error opengl

std::string ErrorDescr( GLenum errorCode )
{
   int iErr = -1 ;
   for ( unsigned i = 0 ; i < numErrors ; i++ )
   {  if ( errCodes[i] == errorCode)
      {  iErr = i ;
         break ;
      }
   }
   if ( iErr == -1 )
      return std::string("Error when trying to report an error: error code is not a valid error code for 'glGetError'") ;
   return std::string( errDescr[iErr] ) ;
}

std::string ErrorCodeString( GLenum errorCode )
{
   int iErr = -1 ;
   for ( unsigned i = 0 ; i < numErrors ; i++ )
   {  if ( errCodes[i] == errorCode)
      {  iErr = i ;
         break ;
      }
   }
   if ( iErr == -1 )
      return std::string("** invalid error code **") ;
   return std::string( errCodeString[iErr] ) ;
}

//----------------------------------------------------------------------

void CompruebaErrorOpenGL( const char * nomArchivo, int linea )
{
   const GLint codigoError = glGetError() ;

   if ( codigoError != GL_NO_ERROR )
   {  std::cout
         << std::endl
         << "Detectado error de OpenGL. Programa abortado." << std::endl
         << "   archivo (linea) : " << QuitarPath(nomArchivo) << " (" << linea << ")" << std::endl
         << "   código error    : " << ErrorCodeString( codigoError )  << std::endl
         << "   descripción     : " << ErrorDescr( codigoError )  << "." << std::endl
         << std::endl << std::flush ;
      exit(1);
   }
}

//----------------------------------------------------------------------

void CalculaVersion(  const char *str, unsigned & major, unsigned & minor )
{
   // podría hacerse así, pero las primeras versiones (1.5-) al parecer no tienen GL_MAJOR_VERSION.... :-(
      // glGetIntegerv( GL_MAJOR_VERSION, &major ) ;
      // glGetIntegerv( GL_MINOR_VERSION, &minor ) ;

   using namespace std ;
   const
      std::string ver(str) ;
   size_t //auto
      blanco = ver.find_first_of(' ') ;
   if ( blanco == string::npos )
      blanco = ver.length() ;

   const size_t //auto
      punto = ver.find_first_of('.') ;
   assert( punto != string::npos ); // salta si no encuentra el punto
   assert( punto+1 < blanco );

   const string
      smajor = ver.substr( 0, punto ),
      sminor = ver.substr( punto+1, blanco-punto-1 );

   major = atoi(smajor.c_str());
   minor = atoi(sminor.c_str());
}

// ---------------------------------------------------------------------
// quita el path de un nombre de archivo con path

std::string QuitarPath( const std::string & path_arch )
{
   const size_t pos_barra = path_arch.find_last_of('/') ;

   if ( pos_barra == std::string::npos ) 
      return path_arch ;
   else
      return path_arch.substr( pos_barra+1 );
}

// ---------------------------------------------------------------------
// devuelve true si se soporta al menos una versión de OpenGL, se
// proporciona el número de version dividido en "major" y "minor"

bool SoportaGL( const unsigned min_major, const unsigned min_minor )
{
   static unsigned major,minor ;
   static bool primera = true ;

   if ( primera )
   {
      // parser de la cadena (GL_VERSION)
      using namespace std ;
      CalculaVersion( (const char *) glGetString(GL_VERSION), major, minor ) ;
      cout << "calculada version de OpenGL: " << major << "." << minor << " (" << glGetString(GL_VERSION) << ")" << endl << flush ;
      primera = false ;
   }

   if ( min_major < major )
      return true;
   if ( min_major == major )
      if ( min_minor <= minor )
         return true;
   return false ;
}

// ---------------------------------------------------------------------

bool SoportaGLSL( const unsigned min_major, const unsigned min_minor )
{
   static unsigned major,minor ;
   static bool primera = true ;

   if ( primera )
   {
      // parser de la cadena (GL_VERSION)
      using namespace std ;
      CalculaVersion( (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION), major, minor ) ;
      cout << "calculada version de GLSL: " << major << "." << minor << " (" << glGetString(GL_SHADING_LANGUAGE_VERSION) << ")" << endl << flush ;
      primera = false ;
   }

   if ( min_major < major )
      return true;
   if ( min_major == major )
      if ( min_minor <= minor )
         return true;
   return false ;
}


//----------------------------------------------------------------------

void InicializaGLEW(  ) // necesario para Linux y resto de SS.OO. excepto MAC
{
#ifndef __APPLE__
   static bool primera = true ;

   if ( ! primera )
      return ;
   primera = false ;

   using namespace std ;
#ifndef GLEW_OK
   Error("no se han incluido los headers de GLEW correctamente, usa '#include <utilidades.hpp>' para incluir símbolos de OpenGL/GLFW/GLEW") ;
#else
   GLenum codigoError = glewInit();
   if ( codigoError != GLEW_OK ) // comprobar posibles errores
   {
      const std::string errmsg =
         std::string( "Imposible inicializar ’GLEW’, mensaje recibido: ") +
         std::string( (const char *)glewGetErrorString(codigoError) ) ;
      Error(errmsg.c_str()) ;
   }
   else
      cout << "librería GLEW inicializada correctamente." << endl << flush ;
#endif
#else
   // en macOS (símbolo __APPLE__ definido) no es necesario usar 'glew'
#endif
}
// ----------------------------------------------------------------------------
// función que gestiona un error en GLFW, se fija con 'glfwSetErrorCallback'

void ErrorGLFW( int codigo, const char * mensaje )
{
   using namespace std ;
   cout << "Error en la librería GLFW: " << endl
        << "   Código  : " << codigo << endl
        << "   Mensaje : " << mensaje << endl
        << "(programa abortado)" << endl ;
   exit(1);
}

// ---------------------------------------------------------------------

void InformeOpenGL(  )
{
   CError();

   const GLubyte * vendor   = glGetString( GL_VENDOR );
   const GLubyte * renderer = glGetString( GL_RENDERER );
   const GLubyte * version  = glGetString( GL_VERSION );
   const GLubyte * glsl_ver = glGetString( GL_SHADING_LANGUAGE_VERSION );

   GLfloat rango_su[2], rango_ali[2] ;
   glGetFloatv( GL_SMOOTH_LINE_WIDTH_RANGE,  rango_su  );
   glGetFloatv( GL_ALIASED_LINE_WIDTH_RANGE, rango_ali );

   unsigned major, minor ;
   CalculaVersion( (const char *) glGetString(GL_VERSION), major, minor ) ;
   
   
   CError();

   using namespace std ;
   cout  << "Datos de versión e implementación de OpenGL" << endl
         << "    implementación de  : " << vendor  << endl
         << "    hardware           : " << renderer << endl
         << "    version de OpenGL  : " << version << " (major == " << major << ", minor == " << minor << ")" << endl
         << "    version de GLSL    : " << glsl_ver << endl
         << "    rango anchos línea : " << rango_su[0] << " - " << rango_su[1] << " (suave)" << endl 
         << "    rango anchos línea : " << rango_ali[0] << " - " << rango_ali[1] << " (aliased)" << endl 
         << flush ;

   CError();
}
// ----------------------------------------------------------------------------------
// calcula posicion y tamaño de la ventana en función de la resolución y tamaño de los 
// monitores disponibles (la posiciona dentro del monitor con mayor área).

void TamPosVentanaGLFW( int & tamx, int & tamy, int & posx, int & posy )
{

   using namespace std ;

   // leer número de monitores y puntero al array de monitores
   int num_monitores ;
   GLFWmonitor ** monitores = glfwGetMonitors( &num_monitores );
   unsigned       ims       = 0 ; // índice del monitor seleccionado
   unsigned long  area_max  = 0 ;

   cout << "Monitores y ventana GLFW:" << endl ;

   // recorrer los monitores y calcular en 'ims' el índice del monitor con más área en pixels
   for( int i = 0 ; i < num_monitores ; i++ )
   {
      const GLFWvidmode   * modo = glfwGetVideoMode( monitores[i] ); 
      const unsigned long   area = (unsigned long) modo->width * (unsigned long) modo->height ; 
      int mxpos, mypos ;

      glfwGetMonitorPos( monitores[i], &mxpos, &mypos );
      
      cout << "   Monitor '" << glfwGetMonitorName( monitores[i] ) << "': " 
           << "posición: " << mxpos << " x " << mypos  << ", "
           << "tamaño: " << modo->width << " x " << modo->height << "." 
           << endl ;

      if ( area_max < area )
      {  area_max = area ;
         ims      = i ;
      }
   }
   cout << "   Usando monitor: '" << glfwGetMonitorName( monitores[ims] ) << "'." << endl ;
  
   // leer el tamaño y la posición del monitor
   const GLFWvidmode * 
      modo      = glfwGetVideoMode( monitores[ims] ); //modo act. del monitor de mayor área
   
   int 
      ancho_tot = modo->width,   // ancho total del escritorio en el modo actual
      alto_tot  = modo->height,  // alto total del escritorio en el modo actual
      mxpos,                     // posicion X del monitor dentro del escritorio virtual completo
      mypos ;                    // posicion Y del monitor dentro del escritorio virtual completo.

   glfwGetMonitorPos( monitores[ims], &mxpos, &mypos );

   // calcular posición y tamaño

   tamx  = (alto_tot*4)/5 ;     // ancho de la ventana
   tamy  = tamx ;               // alto de la ventana (cuadrada)
   posx  = mxpos+(ancho_tot-tamx)/2 ; // posicion X de la ventana
   posy  = mypos+(alto_tot-tamy)/2;   // posición Y de la ventana

   cout << "   Ventana: posición: " << posx << " x " << posy << ", tamaño: " << tamx << " x " << tamy << "." << endl ;
}

// ---------------------------------------------------------------------------
// crea una tabla de índices para una rejilla con topología cilindrica

void CrearIndicesTopologiaCilindrica( std::vector<glm::uvec3> & indices, 
                                      const unsigned na, const unsigned nb )
{
   indices.clear();

   for( unsigned i = 0 ; i < na ; i++ )
   for( unsigned j = 0 ; j < nb ; j++ )
   {        
      const unsigned int
         i00 = (i  )*nb + j,
         i10 = (i+1)*nb + j,
         i01 = (i  )*nb + ((j+1) % nb),
         i11 = (i+1)*nb + ((j+1) % nb);
   
      indices.push_back({ i00, i01, i11 });
      indices.push_back({ i00, i11, i10 });
   }
}

// ----------------------------------------------------------------------------
// dibuja un cilindro, con eje en el eje Z, entre Z=0 y Z=1, y radio unidad.
// (es neutral: no modifica el estado del cauce - únicamente crea el VAO (la primera vez))

void DibujarCilindroZ01( Cauce3D & cauce )
{
   using namespace glm ;
   constexpr unsigned int 
         nz = 5,     // número de 'slices'
         ns = 16 ;   // número de sectores

   // poblar las tablas de coordenadas de posiciones y de índices
   // (únicamente en la 1a llamada a esta función)

   static DescrVAO * vao = nullptr ;

   if ( vao == nullptr )
   {
      std::vector<vec3>  posiciones ;
      std::vector<uvec3> indices ;

      CrearIndicesTopologiaCilindrica( indices, nz, ns );

      for( unsigned iz = 0 ; iz <= nz ; iz++ )
      for( unsigned is = 0 ; is < ns ; is++ )
      {  
         const float alpha = (2.0f*M_PI*float(is))/float(ns);
         posiciones.push_back( { std::cos(alpha), std::sin(alpha), float(iz)/float(nz) } );
      }
      // Nota: este descriptor de VAO únicamente tiene 1 atributo (las posiciones)
      vao = new DescrVAO( 1, new DescrVBOAtribs( ind_atrib_posiciones, posiciones ));  assert( vao != nullptr );
      vao->agregar( new DescrVBOInds( indices ) );
   }
   
   vao->draw( GL_TRIANGLES  );
}

// ----------------------------------------------------------------------------
// dibuja un cono, con eje en el eje Z, entre Z=0 y Z=1, y radio unidad.
// (es neutral: no modifica el estado del cauce - únicamente crea el VAO (la primera vez))

void DibujarConoZ01( Cauce3D & cauce )
{
   using namespace glm ;
   using namespace std ;

   constexpr unsigned int 
         nz = 5,   // número de 'slices'
         ns = 16 ; // número de sectores

   // poblar las tablas de coordenadas de posiciones y de índices
   // (únicamente en la 1a llamada a esta función)

   
   static DescrVAO * vao = nullptr ;
   
   if ( vao == nullptr )
   {
      vector<vec3> posiciones ;
      vector<uvec3> indices ;

      CrearIndicesTopologiaCilindrica( indices, nz, ns );

      for( unsigned iz = 0 ; iz <= nz ; iz++ )
      for( unsigned is = 0 ; is < ns ; is++ )
      {  
         const float alpha = (2.0f*M_PI*float(is))/float(ns);
         const float z     = float(iz)/float(nz) ;
         posiciones.push_back( { (1.0f-z)*std::cos(alpha), (1.0f-z)*std::sin(alpha), z } );
      }
      
      vao = new DescrVAO( 1, new DescrVBOAtribs( ind_atrib_posiciones, posiciones ));  assert( vao != nullptr );
      vao->agregar( new DescrVBOInds( indices ) );
   }
   
   vao->draw( GL_TRIANGLES );
}

// ----------------------------------------------------------------------------
// dibuja un cilindro, con eje en el eje Z, entre Z=0 y Z=longitud_z, y radio 'radio'

void DibujarCilindroZ( Cauce3D & cauce, GLdouble radio, GLdouble longitud_z )
{
   using namespace glm ;
   CError();
   cauce.pushMM();
      cauce.compMM( scale( vec3( radio, radio, longitud_z )));
      DibujarCilindroZ01( cauce );
   cauce.popMM();
   CError(); 
}

// ----------------------------------------------------------------------------
// dibuja un cono, con eje en el eje Z, entre Z=z0 y Z=z1, 'radio' es el radio en z0, 
// (z0 < z1 )
void DibujarConoZ( Cauce3D & cauce,  const float radio, const float z0, const float z1 )
{
   using namespace glm ;
   CError();
   cauce.pushMM();
      cauce.compMM( translate( vec3( 0.0, 0.0, z0 )));
      cauce.compMM( scale( vec3( radio, radio, z1-z0 )));
      DibujarConoZ01( cauce );
   cauce.popMM();
   CError(); 
}

// ----------------------------------------------------------------------------
// dibuja el eje Z

void DibujarEjeZ( Cauce3D & cauce )
{
   using namespace glm ;
   CError();
   const GLfloat
   	zmed     = 0.93,  // z fin del cilindro
   	rad_fle  = 0.03 ;  // radio del cono

   // dibujar línea del eje (más finas y más alargadas)
   
   static DescrVAO * vao = nullptr ; 
   
   if ( vao == nullptr ) 
   {
      const std::vector<vec3> pos = { { 0.0f, 0.0f, -2.0f }, { 0.0f, 0.0f, 5.0f } };
      vao = new DescrVAO( 1, new DescrVBOAtribs( ind_atrib_posiciones, pos )); assert( vao != nullptr );
   }
   
   vao->draw( GL_LINES );
   CError();

   // dibujar cilindro y cono
   DibujarCilindroZ( cauce, rad_fle*0.4, zmed ); 
   DibujarConoZ( cauce, rad_fle, zmed, 1.0 );
      
   CError();
}

//----------------------------------------------------------------------
// dibuja los ejes X,Y y Z, usando cilindros, conos y líneas.

void DibujarEjesSolido( Cauce3D & cauce )
{
   using namespace glm ;
   //log("entra");
   CError();

   // no se usa iluminación ni texturas, se dibuja relleno. 
   cauce.fijarEvalMIL( false );
   cauce.fijarEvalText( false );
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

   // dibujar ejes:

   // eje X, color rojo
   cauce.fijarColor( 1.0, 0.2, 0.2 );
   cauce.pushMM();  
      cauce.compMM( rotate( radians(90.0f), vec3{ 0.0,1.0,0.0 }) );
      DibujarEjeZ( cauce ) ; 
   cauce.popMM();
   CError();
   
   // eje Y, color verde
   cauce.fijarColor( 0.3, 1.0, 0.3 );
   cauce.pushMM();
      cauce.compMM( rotate( radians(-90.0f), vec3{ 1.0, 0.0, 0.0 }) );
      DibujarEjeZ( cauce ) ;
   cauce.popMM(); 
   CError();

   // eje Z, color azul
   cauce.fijarColor( 0.1, 0.7, 1.0 );
   DibujarEjeZ( cauce ) ;
   CError();

   // bola en el origen, negra (temporalmente desactivado)
   cauce.fijarColor( 0.0, 0.0, 0.0 );
   //DibujarEsfera( 0.04, 16, 8 );

   CError();
}


// -----------------------------------------------------------------------------
// imprimie en 'cout' una línea con información de la frecuencia con la que se llama
// a la función

void ImprimirFPS()
{
   using namespace std ; 
   using namespace std::chrono ;

   // tipo para duraciones en segundos
   typedef duration<float,ratio<1,1>> DuracionSegs ;

   
   constexpr int  num_cuadros = 20 ;   // numero de frames entre impresiones de los FPS
   static int     count       = 0 ;    // cuenta de frames
   static bool    primera_vez = true ;

   // instante final del cuadro anterior (excepto en la 1a llamada)
   static time_point<steady_clock> fin_cuadro_anterior ;

   // saltar 'num_cuadros' veces la impresión
   if ( count % num_cuadros != 0 )
   {
      count++ ;
      return ;
   }

   // registrar instante actual
   time_point<steady_clock> fin_cuadro_actual = steady_clock::now() ;


   // si no es la primera vez, calcular e imprimir
   if ( ! primera_vez )
   {
      const float segundos_cuadro = (DuracionSegs( fin_cuadro_actual - fin_cuadro_anterior )).count()/float(num_cuadros) ,
                  fps             = 1.0/segundos_cuadro ;

      // guardar flags de formato de 'cout'
      ios estado_anterior( nullptr );
      estado_anterior.copyfmt( cout );

      // imprimir cambiando los flags
      cout << fixed << setprecision( 1 )
           << setw(5) << fps << "FPS "
           << "(" << setw(5) <<  (segundos_cuadro*1000.0) << " ms)."
           << defaultfloat << endl ;

      // restaurar formato de 'cout'
      cout.copyfmt( estado_anterior );
   }
   count ++ ;
   primera_vez = false ;
   fin_cuadro_anterior = fin_cuadro_actual ;
}


// ----------------------------------------------------------------------------
//
void DibujarFBO( Cauce3D & cauce, Framebuffer & fbo )
{
   using namespace glm ;
   using namespace std ;

   CError();
   GLuint textId = fbo.leerTextId();

   // ponemos el color a blanco para que al multiplicar por textura salga la textura tal cual.
   cauce.fijarColor( 1.0, 1.0, 1.0 );

   cauce.fijarMatrizProyeccion( mat4( 1.0f ) );
   cauce.fijarMatrizVista( mat4( 1.0f ) );
   cauce.fijarEvalMIL( false );
   cauce.fijarEvalText( true, textId );
   cauce.fijarTipoGCT( 0 );
   CError();

   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
   glDisable( GL_CULL_FACE );
   glDisable( GL_DEPTH_TEST );
   CError();

   static DescrVAO * vao = nullptr ;

   if ( vao == nullptr )
   {
      const vector<vec3> posiciones = 
      {  
         { -0.95, +0.60, 0.0 }, { -0.60, +0.60, 0.0 }, 
         { -0.60, +0.95, 0.0 }, { -0.95, +0.95, 0.0 }
      } ;
      const vector<uvec3> indices = 
      { 
         { 0, 1, 2 }, { 0, 2, 3 } 
      } ;
      const vector<vec2> coords_text = 
      { 
         { 0.0, 0.0 }, { 1.0, 0.0 }, 
         { 1.0, 1.0 }, { 0.0, 1.0 } 
      } ;
   
      constexpr unsigned num_atribs = 1 ;
      vao = new DescrVAO( num_atribs, new DescrVBOAtribs( ind_atrib_posiciones, posiciones ));  assert( vao != nullptr );
      vao->agregar( new DescrVBOAtribs( ind_atrib_coord_text, coords_text )); 
      vao->agregar( new DescrVBOInds( indices ) );
   }
   
   vao->draw( GL_TRIANGLES );
   
   glEnable( GL_DEPTH_TEST );
}


//------
// Reemplazar todas las ocurrencias de 'oldString' por 'newString' dentro de la cadena 'line'
// adaptado de:
// https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
// ( se usa en 'ProcesarNombreArchivo', a continuación)

void FindReplace(std::string& line, const std::string& oldString, const std::string& newString)
{
  const size_t oldSize = oldString.length();

   // do nothing if line is shorter than the string to find
   if( oldSize > line.length() )
      return;

   const size_t newSize = newString.length();
   for( size_t pos = 0; ; pos += newSize )
   {
    // Locate the substring to replace
      pos = line.find( oldString, pos );
      if( pos == std::string::npos )
         return;
      if( oldSize == newSize )
      {
         // if they're same size, use std::string::replace
         line.replace( pos, oldSize, newString );
      }
      else
      {
         // if not same size, replace by erasing and inserting
         line.erase( pos, oldSize );
         line.insert( pos, newString );
      }
   }
}

// -----------------------------------------------------------------------------
// Si el nombre de un archivo tiene la barra invertida ('\') pero no estamos
// en windows, sustituir la barra invertida '\' por la barra normal '/'

// std::string ProcesarNombreArchivo( const std::string & sorg )
// {
// #ifdef WINDOWS
//    return sorg ;
// #else
//    std::string res = sorg ;
//    using namespace std ;
//    FindReplace(res,"\\","/");   // el segundo parámetro es un único caracter (\), escapado con \ para poder escribirlo en C/C++)
//    //cout << "[" << sorg << "] --> [" << res << "]" << endl ;
//    return res ;
// #endif
// }

// -----------------------------------------------------------------------------
// Devuelve el path relativo hasta una carpeta con cierto nombre ('carpeta')
// busca la carpeta en el directorio de trabajo, si no está busca en el padre, y 
// así sucesivamente hasta 'n' veces como mucho
// Es decir, busca: ./<carpeta>, ./../<carpeta>, ../../<carpeta>, ../../../<carpeta> y ../../../../<carpeta>
// (funciona tmb en windows)
//
// Basado en:
// https://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
// (respuesta de Adam Parson)

std::string PathCarpeta( const std::string & carpeta, unsigned int n )
{
   using namespace std ;
   string prefijo = "" ; 
   
   for( unsigned i = 0 ; i < n ; i++ )
   {
      const string       path   = prefijo + carpeta ;
      const char * const c_str  = path.c_str();
      struct stat        info ;

      if ( stat( c_str, &info ) == 0 )
         // if ( S_ISDIR( info.st_mode )  ) // no va en windows, S_ISDIR no en 'stat.h' 
         if ( info.st_mode & S_IFDIR ) 
            return path ;
      
      if ( i < n-1 )
         prefijo = prefijo + "../" ;
   }

   cout << "No encuentro el path hasta la carpeta '" << carpeta << "' (aborto)" << endl ;
   exit(1);   
}

// -----------------------------------------------------------------------------
// calcula (la primera vez) y devuelve el path hasta la carpeta de materiales 

std::string PathCarpetaMateriales(  )
{
   static bool primera = true ;
   static std::string path ;

   if ( primera )
   {
      path = PathCarpeta( "materiales", 8 );
      primera = false ;
      std::cout << "Carpeta de materiales encontrada en: [" << path << "]" << std::endl ;
      return path ;
   }

   return path ;
}

// -----------------------------------------------------------------------------
// calcula (la primera vez) y devuelve el path hasta la carpeta de archivos varios

std::string PathCarpetaArchivosVarios(  )
{
   static bool        primera = true ;
   static std::string path ;

   if ( primera )
   {
      path = PathCarpeta( "materiales/varios", 8 );
      primera = false ;
      std::cout << "Carpeta de archivos 'varios' encontrada en: [" << path << "]" << std::endl ;
      return path ;
   }

   return path ;
}

// -----------------------------------------------------------------------------
// calcula (la primera vez) y devuelve el path hasta la carpeta de fuentes

std::string PathCarpetaFuentesShaders(  )
{
   static bool        primera = true ;
   static std::string path ;

   if ( primera )
   {
      path = PathCarpeta( "src/shaders", 8 );
      primera = false ;
      std::cout << "Carpeta de fuentes de shaders encontrada en: [" << path << "]" << std::endl ;
      return path ;
   }

   return path ;
}

// ---------------------------------------------------------------------------
// busca un archivo dado su nombre (nombre_arch) con extensión, sin path alguno.
//
// - en 1er lugar lo busca en la carpeta de materiales, dentro de la subcarpeta especificada 
// - si no está, lo busca en la carpeta de archivos del alumno
//
// devuelve el nombre del archivo con la ruta completa, listo para ser abierto.
// si el archivo no se encuentra en ninguna de ambas carpetas, se invoca 'error' (aborta)

std::string BuscarArchivo( const std::string & nombre_arch, const std::string & subcarpeta )
{
   using namespace std ;

   // quitar el path y quedarnos simplemente con el basename:

   const std::string basename = QuitarPath( nombre_arch );

   if ( basename != nombre_arch )
      cout << "Advertencia: no usar nombres de archivos con path, como: " << nombre_arch << ", se usa: " << basename <<  endl ;

   // buscar en la carpeta de materiales:

   const string nombre_path_mat = PathCarpetaMateriales() + "/" + subcarpeta + "/" + basename ;
   ifstream     archivo_mat ;

   archivo_mat.open( nombre_path_mat.c_str() ); // intentar abrirlo

   if ( archivo_mat.is_open() ) // si se ha podido abrir, cerrarlo y terminar
   {
      archivo_mat.close();
      return nombre_path_mat ;
   }

   // buscar en la carpeta de archivos del alumno:

   const string nombre_path_alu = PathCarpetaArchivosVarios() + "/" + basename  ;
   ifstream     archivo_alu ;

   archivo_alu.open( nombre_path_alu.c_str() ); // intentar abrirlo,

   if ( archivo_alu.is_open() ) // si se ha podido abrir, cerrarlo y terminar
   {
      archivo_alu.close();
      return nombre_path_alu ;
   }

   // no se ha encontrado: producir mensaje de error y abortar 

   cout << "No se encuentra el archivo: '" << nombre_arch << "'" << endl 
        << "Programa abortado." << endl ; 

   exit(1);
}

// --------------------------------------------------------------------------
// define la imagen que se usará para el icono de la ventana glfw
// (únicamente en Windows y Linux, en macOs no hace nada)
//
// Carga el archivo "icono-ventana.jpg", que debe estar en "materiales/imgs". 
// Pone transparentes (alpha==0) para los pixels de color negro o casi negro,
// el resto los pone opacos (alpha== 255)

void FijarImagenIconoVentana( GLFWwindow * ventana_glfw )
{
#if defined (_WIN32) || defined (__linux__)

   assert( ventana_glfw != nullptr );

   unsigned ancho, alto ;
   unsigned char * pixels = LeerArchivoJPEG( "icono-ventana.jpg", ancho, alto );

   const size_t icon_sx   = size_t(ancho), 
                    icon_sy   = size_t(alto),
                    num_bytes = size_t(4)*icon_sx*icon_sy ;

   GLFWimage * img = new GLFWimage ;
   img->width  = int(icon_sx) ;
   img->height = int(icon_sy) ;
   img->pixels = new unsigned char [num_bytes] ;

   assert( img->pixels != nullptr );

   for( size_t iy = 0 ; iy < icon_sy ; iy++ )
   for( size_t ix = 0 ; ix < icon_sx ; ix++ )
   {
      const size_t offset_out  = size_t(4)*(iy*icon_sx + ix) ;
      const size_t offset_in   = size_t(3)*(iy*icon_sx + ix) ;

      unsigned char r = pixels[offset_in+0] ,
                    g = pixels[offset_in+1] ,
                    b = pixels[offset_in+2] ,
                    a = 0xFF ;

      if ( r < 10 && g < 10 && b < 10 )
         a = 0 ;

      img->pixels[offset_out+0] = pixels[offset_in+0] ;
      img->pixels[offset_out+1] = pixels[offset_in+1] ;
      img->pixels[offset_out+2] = pixels[offset_in+2] ;
      img->pixels[offset_out+3] = a ;   
   } 

   glfwSetWindowIcon(  ventana_glfw, 1, img );
#endif
}


// ---------------------------------------------------------------------
// matriz del viewport  (deja las Z igual: entre -1 y 1)

glm::mat4 MAT_Viewport( int org_x, int org_y, int ancho, int alto )
{
   using namespace glm ;
   return translate( vec3{ float(org_x), float(org_y), 0.0 } )*
          scale( vec3( float(ancho), float(alto), 1.0 ) )*
          scale( vec3( 0.5, 0.5, 1.0 ))*
          translate( vec3{ 1.0, 1.0, 1.0 }) ;
}
// ---------------------------------------------------------------------
// matriz inversa de la matriz del viewport

glm::mat4 MAT_Viewport_inv( int org_x, int org_y, int ancho, int alto )
{
   using namespace glm ;
   return translate( vec3{ -1.0, -1.0, -1.0 } ) *
          scale( vec3( 2.0, 2.0, 1.0 ))*
          scale( vec3( 1.0/float(ancho), 1.0/float(alto), 1.0 ))*
          translate( vec3{ -float(org_x), -float(org_y), 0.0 } ) ;
}


// ---------------------------------------------------------------------
// matriz de vista y su inversa

glm::mat4 MAT_Vista( const glm::vec3 eje[3], const glm::vec3 & origen )
{
   using namespace glm ;
   auto rot = mat4(1.0);
   
   for( unsigned i = 0 ; i < 3 ; i++ )
   for( unsigned j = 0 ; j < 3 ; j++ )
      //rot[i][j] = eje[i][j] ;    // CUA: mal en glm
      rot[i][j] = eje[j][i] ;      // CUA: ok en glm, pq rot[i][j] es la columna 'i', fila 'j' de 'rot'

   return rot * translate( -origen );
}

glm::mat4 MAT_Vista_inv( const glm::vec3 eje[3], const glm::vec3 & origen )
{
   using namespace glm ;
   auto rot_inv = mat4(1.0);
   
   for( unsigned i = 0 ; i < 3 ; i++ )
   for( unsigned j = 0 ; j < 3 ; j++ )
      //rot_inv[i][j] = eje[j][i] ;    // inversa == traspuesta, pero esto mal en glm
      rot_inv[i][j] = eje[i][j] ;    // inversa == traspuesta (lo hacemos al revés que en MAT_Vista)

   return translate( origen ) * rot_inv ; 
}
// --------------------------------------------------------------------- 

// lee un archivo completo como texto  y devuelve una cadena terminada
// en cero con el contenido
// (el resultado está en el heap y debería borrarse con 'delete [] p')

char * LeerArchivo( const std::string & nombreArchivo )
{
	using namespace std ;

	ifstream file( nombreArchivo.c_str(), ios::in|ios::binary|ios::ate );

	if ( ! file.is_open())
	{  cout << "imposible abrir archivo para lectura (" << nombreArchivo << ") - termino" << endl << flush ;
		exit(0);
	}
	size_t	numBytes	= file.tellg();
	char *	bytes		= new char [numBytes+2];

	file.seekg (0, ios::beg);
	file.read (bytes, numBytes);
	file.close();

	bytes[numBytes]=0 ;
	bytes[numBytes+1]=0 ;

	return bytes ;
}

// -------------------------------------------------------------------------------
// buffer para log de errores/warnings al compilar/enlazar shaders u objetos programa:

GLsizei report_buffer_long ; // longitud del texto en el buffer
GLchar  report_buffer[ report_buffer_long_max ] ; // cadena con errores

// -------------------------------------------------------------------------------

std::string NombreTipoShader( GLenum tipo_shader )
{
   const char * cstr = 
      ( tipo_shader == GL_VERTEX_SHADER   )        ? "GL_VERTEX_SHADER"   :
      ( tipo_shader == GL_GEOMETRY_SHADER )        ? "GL_GEOMETRY_SHADER" :
      ( tipo_shader == GL_FRAGMENT_SHADER )        ? "GL_FRAGMENT_SHADER" : 
      ( tipo_shader == GL_TESS_CONTROL_SHADER)     ? "GL_TESS_CONTROL_SHADER" : 
      ( tipo_shader == GL_TESS_EVALUATION_SHADER ) ? "GL_TESS_EVALUATION_SHADER" : 
      "shader de tipo desconocido" ;

   return std::string( cstr );
}
// -------------------------------------------------------------------------------

GLuint CompilarAdjuntarShader( GLuint id_prog, GLenum tipo_shader,
                               const std::string & fuente_shader_opc,
                               const std::string & nombre_archivo )
{  
   using namespace std ;
   assert( tipo_shader == GL_VERTEX_SHADER || 
           tipo_shader == GL_GEOMETRY_SHADER || 
           tipo_shader == GL_FRAGMENT_SHADER || 
           tipo_shader == GL_TESS_EVALUATION_SHADER || 
           tipo_shader == GL_TESS_CONTROL_SHADER ) ;

   assert( id_prog > 0 );
   CError();

   const bool leer_arch = fuente_shader_opc == "" ;

   const std::string descr_ts        = NombreTipoShader( tipo_shader );
   const std::string descr_fuente    = leer_arch ? "archivo '" + QuitarPath(nombre_archivo) + "'" : "cadena" ;
   const GLchar *    fuente_shader   = leer_arch ? LeerArchivo( nombre_archivo ) : fuente_shader_opc.c_str() ;
   const GLuint      id_shader       = glCreateShader( tipo_shader );
   GLint             longitud_fuente = strlen( fuente_shader );

   glShaderSource( id_shader, 1, (const GLchar **) &fuente_shader, &longitud_fuente ) ;
   glCompileShader( id_shader ) ;

   glGetShaderInfoLog( id_shader, report_buffer_long_max, &report_buffer_long, report_buffer );
   if ( report_buffer_long > 0 )
   {
      cout << "Log de compilación de " << descr_fuente << " (" << descr_ts << ") : " << endl ;
      cout << report_buffer << endl ;
   }
   GLint compile_status ;
   glGetShaderiv( id_shader, GL_COMPILE_STATUS, &compile_status );
   if ( compile_status != GL_TRUE )
   {
      cout << "Errores al compilar. Aborto." << endl ;
      exit(1);
   }
   glAttachShader( id_prog, id_shader );
   CError();
   cout << "Shader tipo " << descr_ts << " compilado correctamente." << endl ;

   return id_shader ;
}
// -------------------------------------------------------------------------------


void EnlazarObjetoPrograma( GLuint id_prog ) 
{
   using namespace std ;
   assert( id_prog > 0 ); 
   CError();

	// enlazar programa y ver errores
	glLinkProgram( id_prog );
   GLint resultado ;
   glGetProgramiv( id_prog, GL_LINK_STATUS, &resultado );

   // si ha habido errores, abortar
   if ( resultado != GL_TRUE )
   {  
      glGetProgramInfoLog( id_prog, report_buffer_long_max, &report_buffer_long, report_buffer);
      cout << "Error al enlazar el objeto programa:" << endl
           << report_buffer << flush
           << "Programa abortado." << endl << std::flush ;
      exit(1);
   }
   CError();
   cout << "Objeto programa creado y enlazado correctamente." << endl ;
}

//------------------------------------------------------------------------------

GLuint CrearObjetoPrograma( const std::string & vs_src, const std::string & fs_src )
{
   GLuint id_prog = glCreateProgram(); 
   assert( id_prog > 0 ); 
   
   CompilarAdjuntarShader( id_prog, GL_FRAGMENT_SHADER, fs_src.c_str(), "" );
   CompilarAdjuntarShader( id_prog, GL_VERTEX_SHADER, vs_src.c_str(), "" );
   EnlazarObjetoPrograma( id_prog );
   return id_prog ;
}
// ------------------------------------------------------------------------------ 

void VisualizarCuadrado()
{
   CError();

   static const std::vector<glm::vec2> vertices_cuadrado  = { {-1.0,-1.0},  {+1.0,-1.0},  {-1.0,+1.0},  {+1.0,+1.0} };
   static const std::vector<glm::vec2> cc_textura_cuadrado = { {+0.0,+0.0},  {+1.0,+0.0},  {+0.0,+1.0},  {+1.0,+1.0} };
   
   static DescrVAO * dvao_cuadrado = nullptr ;

   if ( dvao_cuadrado == nullptr )
   {
      dvao_cuadrado = new DescrVAO( 2, new DescrVBOAtribs( 0, vertices_cuadrado ) ); 
      assert( dvao_cuadrado != nullptr );
      dvao_cuadrado->agregar( new DescrVBOAtribs( 1, cc_textura_cuadrado ));
   }   
   dvao_cuadrado->draw( GL_TRIANGLE_STRIP );
   CError();
}

// ------------------------------------------------------------------------------ 


bool SustituirTriangulosPorParches() 
{
   const auto apl3d   = Aplicacion3D::instancia() ; 
   const auto cauce3d = (apl3d != nullptr) ? apl3d->cauce3D() : nullptr ;
   const bool sust    = (cauce3d != nullptr) ? cauce3d->sustituirTriangulosPorParches() : false ;
   return sust ;

}

