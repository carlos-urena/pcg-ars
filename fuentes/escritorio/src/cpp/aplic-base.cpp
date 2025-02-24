// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Implementación de la clase 'AplicacionBase' y puntero a la aplicación actual.
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de los métodos de la clase
// ** Puntero a la aplicación actual, inicialmente nulo (la instancia se crea en la func. 'main')
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


#include <thread>
#include <chrono>

#include "objeto-visu.h"
#include "aplic-base.h"
#include "colecciones-objs.h"
#include "animacion.h"
#include "fbo.h"


// ---------------------------------------------------------------------
// 

void FGE_PulsarLevantarTecla( GLFWwindow* window, int key, int scancode, int action, int mods ) 
{
   AplicacionBase * apl = AplicacionBase::instancia();
   apl->mgePulsarLevantarTecla( window, key, scancode, action, mods );
}
// --------------------------------------------------------------------

void FGE_Scroll( GLFWwindow* window, double xoffset, double yoffset  )
{
   AplicacionBase * apl = AplicacionBase::instancia();
   apl->mgeScroll( window, xoffset, yoffset );
}
// --------------------------------------------------------------------
 
void FGE_PulsarLevantarBotonRaton( GLFWwindow* window, int button, int action, int mods )
{
   AplicacionBase * apl = AplicacionBase::instancia();
   apl->mgePulsarLevantarBotonRaton( window, button, action, mods );
}
// --------------------------------------------------------------------

void FGE_MovimientoRaton( GLFWwindow* window, double xpos, double ypos )
{
   AplicacionBase * apl = AplicacionBase::instancia();
   apl->mgeMovimientoRaton( window, xpos, ypos );
}

// ---------------------------------------------------------------------
// F.G. del evento de cambio de tamaño de la ventana (del framebuffer)

void FGE_CambioTamano( GLFWwindow* ventana_glfw, int nuevo_ancho_fb, int nuevo_alto_fb )
{
   AplicacionBase * apl = AplicacionBase::instancia();
   apl->cambioTamano( nuevo_ancho_fb, nuevo_alto_fb );
}
// ---------------------------------------------------------------------

AplicacionBase * AplicacionBase::aplBase = nullptr ; 

// ---------------------------------------------------------------------

AplicacionBase * AplicacionBase::instancia()
{
   assert( aplBase != nullptr );
   return aplBase ;
}
// ---------------------------------------------------------------------
                 
AplicacionBase::AplicacionBase( const unsigned major, const unsigned minor )
{
   using namespace std ;
   cout << "Constructor de 'AplicacionBase': inicio" << endl ;
   
   // asegurarnos de que no hay otra instancia de la aplicación
   if( aplBase != nullptr )
   {
      cout << "ERROR: ya hay creada una instancia de la aplicación" << endl ;
      exit(1);
   }
   
   // Inicializar GLFW y crear la ventana 
   inicializarGLFW( major, minor );

   // Inicialización de GLEW (dejar esta llamada siempre: en macOS no hace nada)
   InicializaGLEW();  

   // escribe características de OpenGL en pantalla (ver 'utilidades.cpp')
   InformeOpenGL() ; 

    cout << "ventana_glfw == " << ventana_glfw << endl ;

   // asignar la instancia actual de la aplicación
   aplBase = this ; 
   cout << "Constructor de 'AplicacionBase': fin." << endl ;
}
// ---------------------------------------------------------------------

AplicacionBase::~AplicacionBase()
{
   // desconectar gestores de eventos de GLFW
   using namespace std ;

   // finalizar la librería GLFW (se llamó a glfwInit desde el constructor, 
   // indirectamente, via 'inicializarGLFW')
   glfwTerminate(); 

   // poner a nullptr la 'aplBase', ya no apunta a ningún objeto.
   aplBase = nullptr ;

   using namespace std ;
   cout << "Aplicación base terminada." << endl ;

   // destruye objetos de tipo 'ObjetoVisu' pendientes de destruir
   ObjetoVisu::destruirPendientes(); 
}

// ---------------------------------------------------------------------
// gestiona un cambio de tamaño de la ventana de la aplicación

void AplicacionBase::cambioTamano(int nuevo_ancho_fb, int nuevo_alto_fb )
{
   // actualizar 'ventana_tam_x' y 'ventana_tam_y' (en la aplicación)
   ventana_tam_x = nuevo_ancho_fb ;
   ventana_tam_y = nuevo_alto_fb ;

   // forzar un nuevo evento de redibujado, para actualizar ventana
   revisualizar = true ;
}


// ---------------------------------------------------------------------
// inicialización de GLFW: creación de la ventana, designar FGEs. Se llama desde el constructor.

void AplicacionBase::inicializarGLFW( const unsigned major, const unsigned minor )
{
   using namespace std ;

   // Tendrán la posicion y tamaño de la ventana y el framebuffer
   int fbx, fby, wx, wy, px, py ; 

   // Inicializacion y configuracion de la librería GLFW:
   glfwSetErrorCallback( ErrorGLFW ); // fijar función llamada ante error (aborta, ver 'utilidades.cpp')
   glfwInit() ;                       // inicializacion de GLFW

   // Especificar versión de OpenGL y parámetros de compatibilidad que se querrán
   // * En macOS, intentar abrir siempre OpenGL 4.1
   // * En Linux, respetar la version que se pasa como parámetro 
   // * En windows, pendiente de verificar.
   
   #ifdef __APPLE__
   const unsigned major_requerido = 4 ,
                  minor_requerido = 1 ;
   #else 
   const unsigned major_requerido = major ,
                  minor_requerido = minor ;
   #endif 

   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, major_requerido ); 
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, minor_requerido ); 
   glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );  // permitir versiones posteriores.
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE ); // no permitir versiones anteriores.
   
   // Crear y posicionar la ventana,
   TamPosVentanaGLFW( wx, wy, px, py ); // calcula pos. y tam., usando tam. escritorio (ver 'utilidades.cpp')
   
   ventana_glfw = glfwCreateWindow( wx, wy, "PCG-ARS 23-24", nullptr, nullptr ); // crea ventana
   assert( ventana_glfw != nullptr );

   glfwSetWindowPos( ventana_glfw, px, py ); // posiciona la ventana


   // Establecer el 'rendering context' de la ventana como el 'context' actual
   glfwMakeContextCurrent( ventana_glfw );

   // leer major y minor version de OpenGL que se ha abierto (no el string que se obtiene con glGetString(GL_VERSION))
   // si el obtenido es menor que el requerido, abortar.
  
   glGetIntegerv( GL_MAJOR_VERSION, &context_major );
   glGetIntegerv( GL_MINOR_VERSION, &context_minor );
   cout << "Versión de OpenGL: " << endl 
        << "    Requerida: " << major << "." << minor << endl 
        << "    Obtenida:  " << context_major << "." << context_minor << endl ;

   if ( context_major < (int)major_requerido || 
        (context_major == (int)major_requerido && context_minor < (int)minor_requerido) )
   {
      cout << "Se había pedido OpenGL " << major << "." << minor << ", pero se ha obtenido una versión anterior (" << context_major << "." << context_minor << ") (aborto)." << endl ;
      exit(1);  
   }


   // Leer el tamaño real actual de la ventana (ventana_tam_x/_y)
   // * Los eventos de ratón del gestor de ventanas usan el tamaño de la ventana.
   // * OpenGL usa el tamaño del framebuffer 
   // En algunos casos ambos tamaños pueden ser distintos, así que a veces es necesario 
   // usar un factor de conversión para las coordenadas de los eventos de ratón.

   glfwGetWindowSize( ventana_glfw, &wx, &wy );
   glfwGetFramebufferSize( ventana_glfw, &fbx, &fby );
   ventana_tam_x    = fbx > 0 ? fbx : wx ;
   ventana_tam_y    = fby > 0 ? fby : wy ;
   mouse_pos_factor = fbx>0 ? fbx/wx : 1 ;  // en displays macos retina, esto es 2
   cout << "Factor de posición del mouse == " << mouse_pos_factor << endl ;

   // define la imagen que se usará para el icono de la ventana glfw
   // (únicamente en Windows y Linux, en macOs no hace nada)
   FijarImagenIconoVentana( ventana_glfw );

   // Definir las diversas funciones gestoras de eventos que GLFW debe invocar
   // (lo hace en glfwPollEvents o en glfwWaitEvents)
   // (esto debe ser lo último)

   glfwSetFramebufferSizeCallback ( ventana_glfw, FGE_CambioTamano );
   glfwSetKeyCallback             ( ventana_glfw, FGE_PulsarLevantarTecla );
   glfwSetMouseButtonCallback     ( ventana_glfw, FGE_PulsarLevantarBotonRaton );
   glfwSetCursorPosCallback       ( ventana_glfw, FGE_MovimientoRaton );
   glfwSetScrollCallback          ( ventana_glfw, FGE_Scroll );
}





// ---------------------------------------------------------------------
// bucle principal  de gestion de eventos GLFW

void AplicacionBase::buclePrincipalEventos(  )
{
   terminar_programa  = false ;
   revisualizar       = true ;   // forzar visualizacióimera vez

   while ( ! terminar_programa  )
   {
      // 1. visualizar coleccion 

      if ( revisualizar )      //  si hay que volver a visualizar
      {                        //
         visualizarFrame();    //     visualizar de nuevo la ventana
         revisualizar = false; //     evitar que se redibuje continuamente
      }

      // 2. determinar si hay animaciones activas
      //
      // hay una animación en curso si están las animaciones activdas por el usuario y
      // además la aplicación está en un estado animable (para una Aplic 3D: si el objeto actual tiene params animables)

      //ObjetoVisu * objeto = objetoActual() ; assert( objeto != nullptr );
      const bool animacion_activa = AnimacionesActivadas() && animable() ;
      
      // 3. procesar eventos

      if ( animacion_activa )                  // si hay alguna animación en curso
      {                                        //
         glfwPollEvents();                     // procesar todos los eventos pendientes, y llamar a la función correspondiente, si está definida
         if ( ! revisualizar )                 // si no es necesario redibujar la ventana
            if ( ActualizarEstado())           // actualizar el estado de la aplicación
               revisualizar = true ;           // si se ha cambiado algo, redibujar.        
      }                                        //
      else                                     // si no hay una animacion en curso
         glfwWaitEvents();                     //   esperar hasta que haya un evento y llamar a la función correspondiente, si está definida

      // 4. determinar si se debe terminar el programa
      //
      // actualiza 'terminar_programa' si GLFW indica que se debe cerrar la ventana
      
      terminar_programa = terminar_programa || glfwWindowShouldClose( ventana_glfw ) ;
   }
}



void AplicacionBase::mgePulsarLevantarTecla( GLFWwindow* window, int key, int scancode, int action, int mods )
{
   using namespace std ;
   cout << "Se ha invocado el método " << __FUNCTION__ << " de la clase 'AplicacionBase'" << endl ;
}


void AplicacionBase::mgeScroll( GLFWwindow* window, double xoffset, double yoffset  )
{
   using namespace std ;
   cout << "Se ha invocado el método " << __FUNCTION__ << " de la clase 'AplicacionBase'" << endl ;
}


void AplicacionBase::mgePulsarLevantarBotonRaton( GLFWwindow* window, int button, int action, int mods )
{
   using namespace std ;
   cout << "Se ha invocado el método " << __FUNCTION__ << " de la clase 'AplicacionBase'" << endl ;
}


void AplicacionBase::mgeMovimientoRaton( GLFWwindow* window, double xpos, double ypos )
{
   using namespace std ;
   cout << "Se ha invocado el método " << __FUNCTION__ << " de la clase 'AplicacionBase'" << endl ;
}

// ----------------------------

void AplicacionBase::testFBO()
{
   using namespace std ;

   // on

   cout << "test de combinacion y visualización de FBOs -inicio" << endl ;

   const unsigned 
      nx = ventanaTamX(), 
      ny = ventanaTamY() ;
   Framebuffer 
      * fbo0 = new Framebuffer( nx, ny ),
      * fbo1 = new Framebuffer( nx, ny ),
      * fbo2 = new Framebuffer( nx, ny );
   
   glViewport( 0,0,nx,ny ); //
   
   // poner el fbo0 a un color
   fbo0->activar( nx, ny );
   glClearColor( 1.0f, 1.0f, 0.0f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT ); 

   // poner el fbo1 a otro color
   fbo1->activar( nx, ny );
   glClearColor( 0.0f, 1.0f, 1.0f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT ); 
   
   // hacer la suma ponderada de los dos fbo en el fbo2 (la media)
   fbo2->sumaPonderada( fbo0, 0.5f, fbo1, 0.5f);

   // visualizar el fbo2 en el viewport (framebuffer 0)
   fbo2->visualizar( 0, 0, nx, ny );
   glfwSwapBuffers( ventana_glfw );

   // ya está, esperar para verlo
   cout << "test de combinacion y visualización de FBOs - fin.(espero 3 seg)" << endl ;
   this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
   cout << "ya!" << endl ;
}





bool AplicacionBase::procesarTeclaS( int key ) 
{
   using namespace std ;
   cout << "Este tipo de aplicación no procesa pulsaciones de teclas estando 'S' pulsada." << endl ;
   return false ;
}
