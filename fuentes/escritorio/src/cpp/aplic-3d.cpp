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

#include "utilidades.h"
#include "camara.h"
#include "colecciones-objs.h"
#include "materiales-luces.h"
#include "animacion.h"
#include "aplic-3d.h"

// ---------------------------------------------------------------------

Aplicacion3D::Aplicacion3D( const unsigned major, const unsigned minor )

: AplicacionBase( major, minor ) 

{
   using namespace std ;

   // Crea el objeto Cauce (compila los 'shaders')
   //cauce = new Cauce3D() ;
      
   
   if ( major == 3 )
      cauce = new Cauce3D_ogl3() ;
   else if ( major == 4 )
      cauce = new Cauce3D_ogl4() ;
   else 
   {
      cout << "Error (en '" << __FUNCTION__ << "'): major no es 3 ni 4" << endl ;
      exit(1);
   }

   assert( cauce != nullptr );

   // Crea la pila de materiales 
   pila_materiales = new PilaMateriales();
   assert( pila_materiales != nullptr );

   // crear las colecciones de objejtos (de la 1 a la 5)
   colecciones_objs.push_back( new ColeccionObjs3D_1() );
   colecciones_objs.push_back( new ColeccionObjs3D_2() );
   colecciones_objs.push_back( new ColeccionObjs3D_3() );
   colecciones_objs.push_back( new ColeccionObjs3D_4() );
   colecciones_objs.push_back( new ColeccionObjs3D_5() );
   colecciones_objs.push_back( new ColeccionObjs3D_6() );
   
   cout << "Colecciones de objetos creadas." << endl ;

   // Dar un valor inicial adecuado a las variables de instancia 'col_fuentes' y 'material_ini'
   // (para 'col_fuentes', se usa una instancia de 'Col2Fuentes'). Los parámetros del material 
   // por defecto son: ka = 0.2, kd = 0.8, ks=1.5, exponente = 60.0
   
   col_fuentes  = new Col2Fuentes();
   cout << "Creadas las fuentes de luz." << endl ;

   material_ini = new Material( 0.2, 0.8, 1.5, 60.0 );
   cout << "Creado el material inicial." << endl ;

   // Añadir sentencias 'push_back' para añadir varias cámaras al vector 'camaras'.
   
   camaras.push_back( new Camara3Modos( true,  { 3.0, 3.0, 3.0 }, 1.0, { 0.0, 0.0, 0.0 }, 70.0 ) );
   camaras.push_back( new Camara3Modos( false, { 2.0, 2.0, 2.0 }, 1.0, { 0.0, 0.0, 0.0 }, 60.0 ) );
   camaras.push_back( new Camara3Modos( false, { 0.0, 0.0, 1.0 }, 1.0, { 0.0, 0.0, 0.0 }, 60.0 ) );
   camaras.push_back( new Camara3Modos( ) ); 
   camaras.push_back( new CamaraOrbitalSimple());    /// CUA: pruebo con la cámara orbital simple:
   cout << "Creado el vector de cámaras." << endl ;
}
// ---------------------------------------------------------------------

Aplicacion3D * Aplicacion3D::instancia()
{
   using namespace std ;
   if( aplBase == nullptr )
   {
      cout << "Error: se ha intentado obtener la aplicación 3D actual, pero no hay ninguna aplicación creada" << endl ;
      exit(1);
   }

   return dynamic_cast<Aplicacion3D *>( aplBase ); // null si no es una instancia de 'Aplicacion3D' o derivadas
}

// ---------------------------------------------------------------------

Aplicacion3D::~Aplicacion3D()
{
   using namespace std ;
   //cout << __FUNCTION__ << " inicio" << endl ;

   // eliminar el cauce
   delete cauce ;
   cauce = nullptr ;

   // eliminar la pila de materiales
   delete pila_materiales ;
   pila_materiales = nullptr ;

   // eliminar las colecciones de objetos
   for( ColeccionObjs * col : colecciones_objs )
   {
      assert( col != nullptr );
      delete col ;
   }
   colecciones_objs.clear() ;
   //cout << __FUNCTION__ << " fin." << endl ;

}
// ---------------------------------------------------------------------

ColeccionObjs * Aplicacion3D::coleccionActual()
{
   assert( ind_coleccion_act < colecciones_objs.size() );
   ColeccionObjs * col = colecciones_objs[ind_coleccion_act] ;
   assert( col != nullptr );
   return col ;
}

// ---------------------------------------------------------------------
///
void Aplicacion3D::siguienteCamara()
{
   assert( ind_camara_actual < camaras.size() );
   ind_camara_actual = (ind_camara_actual+1 ) % camaras.size();
   using namespace std ;
   cout << "Cámara actual cambiada a: " << (ind_camara_actual+1) << "/" << camaras.size() << ": " << camaras[ind_camara_actual]->descripcion() << endl ;
}
// ---------------------------------------------------------------------

CamaraInteractiva * Aplicacion3D::camaraActual()
{
   assert( ind_camara_actual < camaras.size() );
   CamaraInteractiva * cam = camaras[ind_camara_actual] ;
   assert( cam != nullptr );
   return cam ;
}


// ---------------------------------------------------------------------

bool Aplicacion3D::animable() 
{
   return 0 < coleccionActual()->objetoActual()->leerNumParametros() ;
}

// ---------------------------------------------------------------------
  
bool Aplicacion3D::actualizarEstado( const float tiempo_seg ) 
{
   assert( animable() );
   coleccionActual()->objetoActual()->actualizarEstado( tiempo_seg );
   return true ;
}
// ---------------------------------------------------------------------

Cauce3D * Aplicacion3D::cauce3D() 
{
   using namespace std ;
   if ( cauce == nullptr )
      cout << "Error: se ha invocado 'Aplicacion3D::cauce3D' pero el cauce es nulo" << endl ;
   return cauce ;
}
// ---------------------------------------------------------------------

PilaMateriales * Aplicacion3D::pilaMateriales()
{
   assert( pila_materiales != nullptr ); 
   return pila_materiales ;
}

// ---------------------------------------------------------------------

void Aplicacion3D::visualizarFrame()
{
   using namespace std ;
   CError();

   // asegurarnos de que existe un cauce
   assert( cauce != nullptr );
  
   // Configuración de OpenGL:
   //    + habilitar test de comparación de profundidades para 3D (y 2D)
   //      (no está por defecto: https://www.opengl.org/wiki/Depth_Buffer)
   //    + deshabilitar filtrado de triangulos por su orientación:
   //    + dibujar los polígonos rellenos más atrás que las aristas (fijar el 'polygon offset')
   
   glEnable( GL_DEPTH_TEST );   
   glDisable( GL_CULL_FACE );  
   glEnable( GL_POLYGON_OFFSET_FILL ); 
   glPolygonOffset( 1.0, 1.0 );    
   CError();
 
   // fijar el viewport
   glViewport( 0, 0, ventana_tam_x, ventana_tam_y );
   CError();

   // Establecer color de fondo y limpiar la ventana
   if ( fondo_blanco )
      glClearColor( 0.75, 0.75, 0.75, 1.0 ) ;
   else
   glClearColor( 0.2, 0.25, 0.3, 1.0 );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   CError();

   // visualizar el objeto actual de la colección actual
   visualizarGL_OA() ;
   CError();

   // visualizar las normales del objeto.
   if ( visualizar_normales ) 
      visualizarNormalesGL(  );
   
   // si hay un FBO, dibujarlo
   if ( visualizar_fbo )
   {
      if ( fbo != nullptr )
         DibujarFBO( *cauce, *fbo );
   }
   
   // visualizar en pantalla el buffer trasero (donde se han dibujado las primitivas)
   glfwSwapBuffers( ventana_glfw );

   // si queremos imprimir los tiempos por cuadro, hacerlo.
   if ( imprimir_tiempos )
      ImprimirFPS();
}

// ---------------------------------------------------------------------

void  Aplicacion3D::imprimeInfoColeccionActual() 
{
   using namespace std ;
   cout  << endl 
         << "Colección actual " << (ind_coleccion_act+1) << "/" << colecciones_objs.size() << ": " 
         << coleccionActual()->nombre() 
         << " (" << coleccionActual()->numObjetos() << " objetos)." << endl ;
   coleccionActual()->imprimeInfoObjetoActual() ;
}

// ---------------------------------------------------------------------

void Aplicacion3D::mgePulsarLevantarTecla( GLFWwindow* window, int key, int scancode, int action, int mods )
{
  using namespace std ;

   if ( action == GLFW_PRESS ) // solo estamos interesados en el evento de levantar una tecla
      return ;                 // (ignoramos el evento generado al pulsar)

   bool redib = true ; // true sii al final de esta función es necesario redibujar

   // recuperar y comprobar camara y objeto actual
   ColeccionObjs *     coleccion = coleccionActual(); 
   CamaraInteractiva * camara    = camaraActual();            
   ObjetoVisu *        objeto    = coleccion->objetoActual();  

   // si está pulsada la tecla 'S', procesarla y salir
   if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
   {
      if ( procesarTeclaS( key ))  
         revisualizar = true ;
      return ; // finalizar la f.g.e, ya que si está la tecla S pulsada no se mira ninguna otra tecla.
   }         

   // si está pulsada la tecla 'L', actualizar la colección de fuentes de luz y terminar
   if ( glfwGetKey( window, GLFW_KEY_L) == GLFW_PRESS )
   {
      // Procesar la tecla 'key' para actualizar la colección de fuentes
      // de luz actual (usar método 'colFuentes' de la coleccion activa para obtener un puntero), llamar a
      // 'ProcesaTeclaFuenteLuz', si devuelve 'true', forzar revisualizar coleccion (asignar 'revisualizar_coleccion')
      
      assert( col_fuentes != nullptr );
      if ( ProcesaTeclaFuenteLuz( col_fuentes, key ) )
         revisualizar = true ;
      return ; // finalizar la f.g.e, ya que si está la tecla L pulsada no se mira ninguna otra tecla.
   }

   // si está pulsada la tecla 'A', la tecla se procesa en 'animacion.cpp'
   // actúa sobre el objeto que se está visualizando
   
   if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
   {
      // Procesar pulsación de una tecla de animación
      //
      // procesar la tecla en la variable 'key' para actualizar el estado de animación
      // del objeto actual ('objeto'), se debe usar 'ProcesarTeclaAnimacion': si devuelve
      // 'true', forzar revisualizar coleccion (asignando valor a 'revisualizar_coleccion')
      
      if ( ProcesarTeclaAnimacion( objeto, key ) )
         revisualizar = true ;

      // cout << __FUNCTION__  << ", revisualizar coleccion   == " << revisualizar_coleccion 
      //                       << ", animaciones activadas == " << AnimacionesActivadas() 
      //                       << endl ;
      
      return ; // finalizar la f.g.e, ya que si está la tecla A pulsada no se mira ninguna otra tecla.
   }

   constexpr float
      cam_d_incre_tecla   = 1.0,     // incremento de 'cam_d' por teclado (debe ser >= 1.0)
      cam_ab_incre_tecla  = 3.0;     // incremento de ángulos por teclado

   switch ( key )
   {
      // teclas de camara :

      case GLFW_KEY_LEFT:
         camara->desplRotarXY( +cam_ab_incre_tecla, 0.0 );
         break;

      case GLFW_KEY_RIGHT:
         camara->desplRotarXY( -cam_ab_incre_tecla, 0.0 );
         break;

      case GLFW_KEY_UP:
         camara->desplRotarXY( 0.0, +cam_ab_incre_tecla  );
         break;

      case GLFW_KEY_DOWN:
         camara->desplRotarXY( 0.0, -cam_ab_incre_tecla );
         break;

      case GLFW_KEY_KP_SUBTRACT :  // tecla '-' en el teclado numérico
         camara->moverZ( +cam_d_incre_tecla );
         break;

      case GLFW_KEY_KP_ADD :        // tecla '+' en el teclado numérico ¿?
         camara->moverZ( -cam_d_incre_tecla );
         break;

      case GLFW_KEY_C :
         camara->siguienteModo();
         break ;

      case GLFW_KEY_V :
         siguienteCamara() ;
         break ;

      // teclas para cambiar de coleccion y de objeto dentro de la coleccion

      case GLFW_KEY_O :
         coleccion->siguienteObjeto() ;
         break ;

      case GLFW_KEY_P :
         assert( ind_coleccion_act < colecciones_objs.size());
         ind_coleccion_act = (ind_coleccion_act+1) % colecciones_objs.size();
         imprimeInfoColeccionActual();
         break ;


      // tecla para terminar
      case GLFW_KEY_ESCAPE:
      case GLFW_KEY_Q:
         terminar_programa = true ;
         break ;

      // teclas variadas:

      case GLFW_KEY_E:
         dibujar_ejes = ! dibujar_ejes ;
         cout << "Dibujar ejes: " << (dibujar_ejes? "activado." : "desactivado.") << endl << flush ;
         break ;

      case GLFW_KEY_N :
         visualizar_normales = ! visualizar_normales ;
         cout << "Visualizar normales: " << (visualizar_normales ? "activado." : "desactivado.") << endl << flush ;
         break ;

      case GLFW_KEY_M :
         modo_visu = ModosVisu( (int(modo_visu)+1) % int(ModosVisu::num_modos) );
         cout << "Modo de visualización cambiado a: '" << nombreModoVisu[int(modo_visu)] << "'" << endl << flush ;
         break ;

      case GLFW_KEY_W :
         dibujar_aristas = ! dibujar_aristas ;
         cout << "Dibujar aristas: " << (dibujar_aristas? "activado" : "desactivado" ) << endl ;
         break ;

      case GLFW_KEY_I :
         iluminacion = ! iluminacion ;
         cout << "Iluminación : " << (iluminacion ? "activada" : "desactivada") << endl << flush ;
         break ;

      case GLFW_KEY_F :
          usar_normales_tri = ! usar_normales_tri ;
          cout << "La iluminación usa " << (usar_normales_tri ? "la normal del triángulo" : "las normales de vértices (interpoladas)") << endl << flush ;
          break ;

      case GLFW_KEY_R :   // conmutar 'invertir'
         fondo_blanco = ! fondo_blanco ;
         cout << "fondo blanco : " << (fondo_blanco ? "activado" : "desactivado") << endl << flush ;
         break ;
      
      case GLFW_KEY_Y :   // conmutar
         visualizar_fbo = ! visualizar_fbo ;
         cout << "visualizar FBO: " << (visualizar_fbo ? "activado" : "desactivado") << endl << flush ;
         break ;

      case GLFW_KEY_H :
         imprimir_tiempos = ! imprimir_tiempos ;
         cout << "imprimir tiempos : " << (imprimir_tiempos ? "activado" : "desactivado") << endl << flush ;
         break ;
      
      case GLFW_KEY_T :
         {
            auto * cauce4 = dynamic_cast<Cauce3D_ogl4 *>( cauce );
            if ( cauce4 != nullptr )
            {
               cauce4->fijarActivarTS( ! cauce4->leerActivarTS() );
               cout << "Tessellation shader (subdivisión): " << (cauce4->leerActivarTS() ? "activado" : "desactivado") << endl << flush ;
            }
            else 
               cout << "Error: no se puede activar/desactivar el tessellation shader, el cauce no es de tipo 'Cauce3D_ogl4'" << endl << flush ;
         }
         break ;

      case GLFW_KEY_G :
         {
            auto * cauce4 = dynamic_cast<Cauce3D_ogl4 *>( cauce );
            if ( cauce4 != nullptr )
            {
               cauce4->fijarActivarGS( ! cauce4->leerActivarGS() );
               cout << "Geometry shader (aristas): " << (cauce4->leerActivarGS() ? "activado" : "desactivado") << endl << flush ;
            }
            else 
               cout << "Error: no se puede activar/desactivar el geometry shader, el cauce no es de tipo 'Cauce3D_ogl4'" << endl << flush ;
         }
         break ;

      default:
         redib = false ; // para otras teclas, no es necesario redibujar
         break ;
   }
   // si se ha cambiado algo, forzar evento de redibujado
   if ( redib )
      revisualizar = true ;
}

// --------------------------------------------------------------------------------

void Aplicacion3D::mgeScroll( GLFWwindow* window, double xoffset, double yoffset  )
{
   if ( fabs( yoffset) < 0.05 ) // si hay poco movimiento vertical, ignorar el evento
      return ;

   constexpr float cam_d_z_incre_scroll = 1.0 ;
   const     float direccion            = 0.0 < yoffset ? +1.0 : -1.0 ;

   CamaraInteractiva * camara = camaraActual() ; 
   assert( camara!= nullptr );

   camara->moverZ( direccion*cam_d_z_incre_scroll ) ;
   revisualizar = true ;
}
// --------------------------------------------------------------------------------


void Aplicacion3D::mgePulsarLevantarBotonRaton( GLFWwindow* window, int button, int action, int mods )
{
   // solo estamos interesados en eventos de pulsar algún botón (no levantar)
   if ( action != GLFW_PRESS )
      return ;

   // leer la posición del puntero de ratón en x,y (enteros)
   double x_f,y_f ;
   glfwGetCursorPos( window, &x_f, &y_f );
   const int x = int(x_f), y = int(y_f);

   //cout << "click, tam ventana == " << apl->ventana_tam_x << " x " << apl->ventana_tam_y << endl ;
   //cout << "click, cursor pos  == " << x << ", " << y <<  endl ;

   if ( button == GLFW_MOUSE_BUTTON_LEFT )
   {
      // pulsar botón izquierdo: selección
      if ( seleccion( x*mouse_pos_factor, (ventana_tam_y - y*mouse_pos_factor) ) )
         revisualizar = true ;
   }
   else if ( button == GLFW_MOUSE_BUTTON_RIGHT )
   {
      // pulsar botón derecho: inicio de modo arrastrar con botón derecho pulsado:
      x_ant_mabd = x ; // registrar posición de inicio en X
      y_ant_mabd = y ; // registrar posición de inicio en Y
      revisualizar = true ;
   }
}
// --------------------------------------------------------------------


void Aplicacion3D::mgeMovimientoRaton( GLFWwindow* window, double xpos, double ypos )
{
   // Ignorar evento si no está pulsado el botón derecho
   if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) != GLFW_PRESS )
      return ;

   // Estamos en modo arrastrar con el botón derecho pulsado:
   // Actualizar la cámara actual.

   constexpr float delta = 1.0; // incremento de ángulos con el ratón (por pixel)

   // calcular el desplazamiento en pixels en X y en Y respecto de la última posición
   const int
      x  = int( xpos ),
      y  = int( ypos ),
      dx = x - x_ant_mabd ,
      dy = y - y_ant_mabd ;

   // usar desplazamientos para desplazar/rotar la cámara actual en X y en Y
   CamaraInteractiva * camara = camaraActual() ;
   camara->desplRotarXY( -dx*delta, dy*delta );

   // registrar (x,y) como la última posición del ratón
   x_ant_mabd = x ;
   y_ant_mabd = y ;

   // forzar que la escena se vuelva a visualizar
   revisualizar = true ;

}
// -----------------------------------------------------------------------------------------------

void Aplicacion3D::visualizarGL_OA(  )
{
   CamaraInteractiva * camara = camaras[ind_camara_actual] ;
   ObjetoVisu * objeto = coleccionActual()->objetoActual() ;
   
   assert( cauce != nullptr );
   assert( objeto != nullptr );
   assert( camara != nullptr );
   assert( col_fuentes != nullptr );
   assert( material_ini != nullptr );

   using namespace std ;
   CError();
   
   // activar el cauce
   cauce->activar() ;
   CError();

   //const float ratio_vp = apl->;
   
   camara->fijarRatioViewport( aspectRatioVentanaYX()  );
   camara->activar( *cauce ) ;
   CError();

   // dibujar los ejes, si procede
   if ( dibujar_ejes  )
      DibujarEjesSolido( *cauce ) ; // ver 'utilidades.cpp' para la definición.

   // fijar el color por defecto (inicial) en el cauce para dibujar los objetos 
   // (es blanco debido a que el fondo se rellena con un color oscuro y debe contrastar).
   cauce->fijarColor( 1.0, 1.0, 1.0 );
   
   // fijar el modo de normales 
   cauce->fijarUsarNormalesTri( usar_normales_tri );

   // Usar 'glPolygonMode' en función del modo guardado en 'apl->modo_visu', 
   // que puede ser: puntos,lineas o relleno.
   const int modo_pol = 
      (modo_visu == ModosVisu::puntos) ?  GL_POINT :
      (modo_visu == ModosVisu::lineas) ?  GL_LINE  :
                                          GL_FILL  ;  // (modo_visu == modo relleno)
   glPolygonMode( GL_FRONT_AND_BACK, modo_pol );
   
   CError();

   if ( iluminacion )
   {
      // Activar evaluación del MIL (y desactivar texturas)
      //
      // * habilitar evaluación del MIL en el cauce (fijarEvalMIL)
      // * activar la colección de fuentes de la escena
      // * activar el material inicial (usando 'pila_materiales')
      
      cauce->fijarEvalMIL( true );   // evaluar el MIL
      cauce->fijarEvalText( false ); // inicialmente no hay textura activa

      //assert( col_fuentes != nullptr );
      col_fuentes->activar(  );

      pila_materiales->activar( material_ini );
      
   }
   else // si la iluminación no está activada, deshabilitar MIL y texturas
   {  
      cauce->fijarEvalMIL( false );
      cauce->fijarEvalText( false );
   }

   //log("recupero objeto");
   CError();

   
   // Visualizar el objeto actual ('objeto')
   // dibujar el objeto (raíz) actual de esta escena (simplemente invocar 'visualizarGL')
   objeto->visualizarGL( );

   // Visualizar las aristas del objeto, si procede (es decir: en modo relleno, con 
   // visualización de aristas activada, y si se trata de un objeto 3D)

   auto * objeto3D = dynamic_cast<ObjetoVisu3D *>( objeto );

   if ( objeto3D != nullptr && dibujar_aristas && modo_visu == ModosVisu::relleno ) 
   {
      // desactivar iluminación y texturas (podrían estarlo a partir de prác. 4)
      cauce->fijarEvalMIL( false );
      cauce->fijarEvalText( false );

      // Visualizar únicamente las aristas del objeto actual
      //
      // - hay que invocar 'visualizarGeomGL' para el objeto actual de la escena ('objeto')
      // - antes de eso debemos de poner el cauce en un estado adecuado:
      //      - fijar el color a negro
      //      - fijar el modo de polígonos a modo 'lineas'
      
      cauce->fijarColor( 0.0, 0.0, 0.0 );
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // lineas
      objeto3D->visualizarGeomGL(  );
   }
}
// -----------------------------------------------------------------------------------------------

void Aplicacion3D::visualizarGL_OA_Seleccion(  )
{
   // Comprobar algunas precondiciones y recuperar el cauce (para acortar la anotación)
   Aplicacion3D * apl   = Aplicacion3D::instancia() ;
   Cauce3D *      cauce = apl->cauce3D() ;
   CError();

   // Visualizar el objeto raíz de esta escena en modo selección
   // Pasos:

   // (1) Configurar estado de OpenGL:
   //       + fijar el viewport (con 'glViewport') usando el tamaño de la ventana (guardado en 'apl'), 
   //       + fijar el modo de polígonos a 'relleno', con 'glPolygonMode'
   //
   glViewport( 0,0, apl->ventanaTamX(), apl->ventanaTamY() );
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
   CError();
   
   // (2) Activar  y configurar el cauce:
   //      + Activar el cauce (con el método 'activar')
   //      + Desactivar iluminación y texturas en el cauce
   //      + Poner el color actual del cauce a '0' (por defecto los objetos no son seleccionables)
   cauce->activar();
   cauce->fijarEvalMIL( false );
   cauce->fijarEvalText( false );
   cauce->fijarColor( 0.0, 0.0, 0.0 ); 
   CError();
   
   // (3) Limpiar el framebuffer (color y profundidad) con color (0,0,0) (para indicar que en ningún pixel hay nada seleccionable)
   glClearColor( 0.0, 0.0, 0.0, 1.0 );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   CError();
   
   // (4) Recuperar la cámara actual (con 'camaraActual') y activarla en el cauce, 
   CamaraInteractiva * camara = camaras[ind_camara_actual] ;  assert( camara != nullptr );
   camara->activar( *cauce );
   CError();
   
   // (5) Recuperar (con 'objetoActual') el objeto raíz actual de esta escena y 
   //     visualizarlo con 'visualizarModoSeleccionGL'.
   ObjetoVisu * objeto_raiz = coleccionActual()->objetoActual() ; 
   objeto_raiz->visualizarModoSeleccionGL(); 
   CError();
}
// -----------------------------------------------------------------------------------------------

void Aplicacion3D::visualizarNormalesGL(  )
{
   auto * objeto3D = dynamic_cast<ObjetoVisu3D *>( coleccionActual()->objetoActual() );

   if ( objeto3D == nullptr )
      return ; // si el objeto actual no es 3D, no se visualizan las normales

   // comprobar precondiciones
   Aplicacion3D * apl = Aplicacion3D::instancia() ;
   Cauce3D * cauce = apl->cauce3D() ; 

   // Visualizar normales del objeto actual de la escena 
   // 1. Configurar el cauce de la forma adecuada, es decir:
   //      * Desactivar la iluminación (con 'fijarEvalMIL')
   //      * Desactivar el uso de texturas (con 'fijarEvalText')
   //      * fijar el color (con 'fijarColor') 
   // 2. Visualizar las normales del objeto actual de la escena (con el método 'visualizarNormalesGL')

   cauce->fijarEvalMIL( false );
   cauce->fijarEvalText( false );
   cauce->fijarColor( 1.0, 0.7, 0.4 );

   
   objeto3D->visualizarNormalesGL( );
}
// -----------------------------------------------------------------------------------------------

bool Aplicacion3D::procesarTeclaS( int key )
{
   using namespace std ;

   CauceBase * cauce = cauce3D() ; assert( cauce != nullptr );
   
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


