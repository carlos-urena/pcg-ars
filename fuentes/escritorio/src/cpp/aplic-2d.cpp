// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Implementación de la clase 'Aplicacion3D' 
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

#include <cmath>
#include "utilidades.h"
#include "aplic-2d.h"
#include "colecciones-objs.h"

// ---------------------------------------------------------------------

Aplicacion2D::Aplicacion2D()

: AplicacionBase( 3, 3 ) // abrir OpenGL 3.3

{
   using namespace std ;
   cout << "Constructor de 'Aplicacion2D': inicio." << endl ;

   cauce2d = new Cauce2D() ;
   cauce2d_lineas = new Cauce2DLineas() ;

   col_objetos = new ColeccionObjs2D() ;

   vista2d = new Vista2D( ventanaTamX(), ventanaTamY() ) ;
   
   cout << "Constructor de 'Aplicacion2D': fin." << endl ;
}
// ---------------------------------------------------------------------

Aplicacion2D::~Aplicacion2D()
{
   using namespace std ;
   cout << "Constructor de 'Aplicacion2D': inicio." << endl ;

   delete cauce2d ;
   cauce2d = nullptr ;
   
   delete col_objetos ;
   col_objetos = nullptr ;

   cout << "Destructor de 'Aplicacion2D': fin." << endl ;
}
// ---------------------------------------------------------------------

Aplicacion2D * Aplicacion2D::instancia()
{
   using namespace std ;
   if( aplBase == nullptr )
   {
      cout << "Error: se ha intentado obtener la aplicación 2D actual, pero no hay ninguna aplicación creada" << endl ;
      exit(1);
   }

   Aplicacion2D * apl2D = dynamic_cast<Aplicacion2D *>( aplBase );
   if ( apl2D == nullptr )
   {
      cout << "Error: se ha intentado obtener la aplicación 2D actual, pero la aplicación actual no es una aplicación 2D" << endl ;
      exit(1);
   }
      
   return apl2D ;
}
// ---------------------------------------------------------------------

Cauce2D * Aplicacion2D::cauce2D() 
{
   using namespace std ;
   if ( cauce2d == nullptr )
      cout << "Error: se ha invocado 'Aplicacion2D::cauce2D' pero el cauce es nulo" << endl ;
   return cauce2d ;
}
// ---------------------------------------------------------------------

Vista2D * Aplicacion2D::vista2D() 
{
   using namespace std ;
   if ( vista2d == nullptr )
      cout << "Error: se ha invocado 'Aplicacion2D::vista2D' pero la vista es nula" << endl ;
   return vista2d ;
}
// ---------------------------------------------------------------------

Cauce2DLineas * Aplicacion2D::cauce2DLineas() 
{
   using namespace std ;
   if ( cauce2d_lineas == nullptr )
      cout << "Error: se ha invocado 'Aplicacion2D::cauce2DSegmentos' pero el cauce es nulo" << endl ;
   return cauce2d_lineas ;
}

// ---------------------------------------------------------------------

bool Aplicacion2D::animable() 
{
   return false ;
}
// ---------------------------------------------------------------------

bool Aplicacion2D::actualizarEstado( const float tiempo_seg ) 
{
   using namespace std ;
   cout << "NO SE DEBE LLAMAR A 'Aplicacion2D::actualizarEstado' (por ahora)" << endl ;
   return false ; 
}
// ---------------------------------------------------------------------

void Aplicacion2D::mgeMovimientoRaton( GLFWwindow* window, double xpos, double ypos )
{
   // Ignorar evento si no está pulsado el botón derecho
   if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) != GLFW_PRESS )
      return ;

   // Calcular el desplazamiento en pixels en X y en Y respecto de la última posición
   const unsigned
      x  = unsigned( xpos ),
      y  = unsigned( ypos );
   const float 
      dx = float(int(x) - int(x_ant_mabd)) ,
      dy = float(int(y) - int(y_ant_mabd)) ;

   // registrar (x,y) como la última posición del ratón
   x_ant_mabd = x ;
   y_ant_mabd = y ;

   // desplazar la vista
   assert( vista2d != nullptr ) ; 
   vista2d->desplazar( dx, dy ) ;

   // forzar que la escena se vuelva a visualizar
   revisualizar = true ;

}
// ---------------------------------------------------------------------

void Aplicacion2D::mgePulsarLevantarTecla( GLFWwindow* window, int key, int scancode, int action, int mods )
{
  using namespace std ;

   if ( action == GLFW_PRESS ) // solo estamos interesados en el evento de levantar una tecla
      return ;                 // (ignoramos el evento generado al pulsar)

   // si está pulsada la tecla 'S', procesarla y salir
   if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
   {
      if ( procesarTeclaS( key ))  
         revisualizar = true ;
      return ; // finalizar la f.g.e, ya que si está la tecla S pulsada no se mira ninguna otra tecla.
   }

   bool redib = true ; // true sii al final de esta función es necesario redibujar

   switch ( key )
   {
      case GLFW_KEY_LEFT:
         break;

      case GLFW_KEY_RIGHT:
         break;

      case GLFW_KEY_UP:
         break;

      case GLFW_KEY_DOWN:
         break;

      case GLFW_KEY_KP_SUBTRACT :  // tecla '-' en el teclado numérico
         break;

      case GLFW_KEY_KP_ADD :        // tecla '+' en el teclado numérico ¿?
         break;

      // teclas para cambiar de coleccion y de objeto dentro de la coleccion
      case GLFW_KEY_O :
         col_objetos->siguienteObjeto() ;
         break ;

      // tecla para terminar
      case GLFW_KEY_ESCAPE:
      case GLFW_KEY_Q:
         terminar_programa = true ;
         break ;

      case GLFW_KEY_T: // test de fbos
         testFBO();
         break ;

      case GLFW_KEY_W: 
         // cambiar el modo de visualización de los contornos
         contornosSolidos = ! contornosSolidos ;
         cout << "Modo de visualización de contornos: " << (contornosSolidos ? "sólidos" : "líneas") << endl ;
         break ;
   }

   // si se ha cambiado algo, forzar evento de redibujado
   if ( redib )
      revisualizar = true ;
}
// --------------------------------------------------------------------------------

void Aplicacion2D::mgeScroll( GLFWwindow* window, double xoffset, double yoffset  )
{
    // si hay poco movimiento vertical, ignorar el evento
   if ( fabs( yoffset) < 0.05 )
      return ;

   // hacer zoom de la vista
   assert( vista2d != nullptr ) ;
   vista2d->zoom( yoffset ) ;
   
   revisualizar = true ;
}
// --------------------------------------------------------------------------------

void Aplicacion2D::mgePulsarLevantarBotonRaton( GLFWwindow* window, int button, int action, int mods )
{
   // solo estamos interesados en eventos de pulsar el botón derecho
   if ( action != GLFW_PRESS || button != GLFW_MOUSE_BUTTON_RIGHT )
      return ;

   // leer la posición del puntero de ratón en x_ant_mabd, y_ant_mabd (enteros sin signo)
   double x_f,y_f ;
   glfwGetCursorPos( window, &x_f, &y_f );
   x_ant_mabd = unsigned(x_f);
   y_ant_mabd = unsigned(y_f);
}
// --------------------------------------------------------------------

void Aplicacion2D::visualizarFrame() 
{
   using namespace std ;
   using namespace glm ;

   assert( cauce2d != nullptr );
   assert( cauce2d_lineas != nullptr );
   assert( col_objetos != nullptr );
   
   // fijar estado de OpenGL: definir el viewport, limpiar la ventana, inicializar el estado de OpenGL
   glViewport( 0, 0, ventanaTamX(), ventanaTamY() );
   glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   glEnable( GL_DEPTH_TEST );
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  
   glDisable( GL_CULL_FACE );

   // activar el cauce 2D y configurar y activar la vista 2D
   cauce2d->activar() ;
   vista2d->fijarViewport( ventanaTamX(), ventanaTamY() );
   vista2d->activar( *cauce2d ) ;

   // visualizar el objeto actual
   cauce2d->fijarColor( 0.5f, 0.5f, 0.5f) ; // gris medio para el relleno..
   col_objetos->objetoActual()->visualizarGL() ;

   // visualizar el contorno del objeto con el cauce de segmentos (si es un ObjetoVisu2D)

   auto * objeto2d = dynamic_cast<ObjetoVisu2D *>( col_objetos->objetoActual() ); 
   if ( objeto2d != nullptr )
   {
      // configurar OpenGL (el modo de visu.)
      const GLenum modo = contornosSolidos ? GL_FILL : GL_LINE ;
      glPolygonMode( GL_FRONT_AND_BACK, modo ) ; CError();

      // Configurar el cauce y la vista: 
      //
      // 1. activar la vista 2D en el cauce lineas (como si fuera de su clase base 'Cauce2D')
      // 2. mover hacia adelante en Z los contornos para que tapen los rellenos

      cauce2d_lineas->activar() ; // activar el cauce, 
      vista2d->activar( * ((Cauce2D*) cauce2d_lineas) ) ;  
      cauce2d_lineas->compMM( translate( vec3( 0.0f, 0.0f, -0.01f ) ));

      // visualizar el objeto.
      objeto2d->visualizarSegmentosGL();
   }

   // mostrar el buffer en la ventana
   glfwSwapBuffers( ventana_glfw );
}

// --------------------------------------------------------------------

bool Aplicacion2D::procesarTeclaS( int key )
{
   using namespace std ;

   CauceBase * cauce = cauce2DLineas() ; assert( cauce != nullptr );
   
   if ( key == GLFW_KEY_KP_ADD || key == GLFW_KEY_RIGHT_BRACKET )
   {
      cauce->modificarParametroS( +1.0f ) ; 
      return true ;
   }
   else if ( key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_SLASH )
   {
      cauce->modificarParametroS( -1.0f ) ;
      return true ;
   }
   else 
      return false ;
}


