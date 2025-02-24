// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Declaración de la clase 'AplicacionBase'
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** La clase 'AplicacionBase' contine
// **     
// **     + ventana GLFW
// **     + vector de coleccions e índice de la coleccion actual
// **     + variables de estado para gestionar el bucle de eventos (revisualizar_coleccion, terminar_programa)
// **     + variables de estado del ratón para gestionar eventos de mover con botón pulsado (arrastrar).
// **     + diversas variables de estado que determinan como se visualizan los objetos, las animaciones, etc ....
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

#include <vector>
#include "utilidades.h"

// declaraciones adelantadas de clases (para poder declarar punteros antes de "ver" la estructura de la clase)
class ColeccionObjs ;
class Cauce3D ;
class ColFuentesLuz ;
class ContextoVis ;
class CamaraInteractiva ;
class Material ;
class ObjetoVisu ;
class AplicacionBase ;
class Framebuffer ;
class PilaMateriales ;


// ---------------------------------------------------------------------
// tipo de datos enumerado para los modos de visualización:
enum class ModosVisu
{
   relleno,
   lineas,
   puntos,
   num_modos
} ;

const std::string nombreModoVisu[ int(ModosVisu::num_modos)+1 ] =
{
   "relleno",
   "líneas",
   "puntos",
   "*** NUM. MODOS. VIS. ***"
} ;

// ---------------------------------------------------------------------

/// @brief Función gestora del evento de pulsar o levantar una tecla
/// @brief (invoca el método correspondiente de la aplicación actual)
///
/// @param window   ventana con el foco en el momento de pulsar o levantar la tecla
/// @param key      código de tecla (ver: https://www.glfw.org/docs/3.3/group__keys.html)
/// @param scancode código de tecla (específico de la plataforma, no lo usamos)
/// @param action   indica si se ha pulsado o levantado la tecla (GLFW_PRESS o GLFW_RELEASE)
/// @param mods     modificadores (indica si las tecla alt, ctrl y shift están pulsadas o no)
///
void FGE_PulsarLevantarTecla( GLFWwindow* window, int key, int scancode, int action, int mods );

// --------------------------------------------------------------------
// 

/// @brief Función gestora del evento de hacer scroll (se registra con 'glfwSetScrollCallback')
/// @brief invoca el correspondiente método de la aplicación activa actualmente
/// @param window 
/// @param xoffset 
/// @param yoffset 
///
void FGE_Scroll( GLFWwindow* window, double xoffset, double yoffset  );

// --------------------------------------------------------------------

/// @brief Función gestora del evento de pulsar/levantar tecla del ratón 
/// @brief (se registra con 'glfwSetMouseButtonCallback').
/// @brief Invoca el correspondiente método de la aplicación activa actualmente.
///
/// @param window 
/// @param button 
/// @param action 
/// @param mods 
///
void FGE_PulsarLevantarBotonRaton( GLFWwindow* window, int button, int action, int mods );

// --------------------------------------------------------------------
/// @brief Función gestora del evento de movimiento del ratón 
/// @brief (se registra con 'glfwSetCursorPosCallback').
/// @brief Invoca el correspondiente método de la aplicación activa actualmente.
///
/// @param window 
/// @param xpos 
/// @param ypos 
///
void FGE_MovimientoRaton( GLFWwindow* window, double xpos, double ypos );

// --------------------------------------------------------------------
///
/// @brief Clase base para aplicaciones gráficas interactivas.
/// @brief Una instancia contiene la ventana GLFW, el cauce, 
/// @brief el vector de colecciones de objetos
/// @brief Se puede extender para aplicaciones concretas (aplic 2D, 3D, otras...)
/// 
class AplicacionBase
{
   public:

   
   /// @brief Constructor: inicializa GLFW, crea la ventana, 
   /// @brief inicializa Glew e inicializa OpenGL.
   ///
   /// @param vers_major (unsigned) - versión de openGL que se requiere (major), por defecto 3
   /// @param vers_minir (unisgned) - versión de openGL que se requiere (minor), por defecto 3
   ///
   AplicacionBase( const unsigned major, const unsigned minor ); 

   // Destructor 
   virtual ~AplicacionBase() ;

   /// @brief Inicializa GLFW, crea la ventana, registra funciones gestoras de eventos.
   /// @brief (se llama desde el constructor)
   ///
   /// @param major (unsigned) version requerida (major)
   /// @param minor (unsigned) version requerida (minor)
   ///
   void inicializarGLFW( const unsigned major = 3, const unsigned minor = 3 ) ;

   /// @brief Indica si la aplicación es animable en el estado actual o no lo es
   ///
   virtual bool animable() = 0 ;

   /// @brief Actualiza el estado de la aplicación durante una animación (solo se puede llamar cuando 'animable() es true).
   /// @param tiempo_seg - tiempo transcurrido desde la ultima  actualización o desde el inicio de las anmaciones
   /// @return 'true' si se ha cambiado algo del estado de la aplic. y es necesario visu., 'false' si no.
   ///
   virtual bool actualizarEstado( const float tiempo_seg ) = 0 ;

   /// @brief Gestiona un evento de cambio de tamaño de la ventana
   ///
   /// @param nuevo_ancho_fb (int) - nuevo ancho de la ventana
   /// @param nuevo_alto_fb (int) - nuevo alto de la ventana
   ///
   void cambioTamano( int nuevo_ancho_fb, int nuevo_alto_fb );

   /// @brief Ejecuta el bucle principal  de gestion de eventos GLFW, hasta que se cierra ventana o
   /// @brief bien 'terminar_programa' se pone a 'true' en algún gestor de eventos.
   ///
   void buclePrincipalEventos( );

   /// @brief Visualiza el objeto actual en la ventana de la aplicación, se llama durante el bucle de 
   /// @brief de gestión de eventos, en cada iteración, si hay que refrescar los contenidos de la ventana
   /// @brief (debe ser redefinido en las clases derivadas)
   ///
   virtual void visualizarFrame() = 0;

   /// @brief Método gestor de un evento de pulsar o levantar una tecla en el teclado
   /// @param window - ventana con el foco en el momento de pulsar o levantar la tecla
   /// @param key - código de tecla
   /// @param scancode - código de tecla (dependiente del teclado)
   /// @param action - indica si se ha pulsado o levantado la tecla
   /// @param mods - modificadores del teclado en el momento del evento
   ///
   virtual void mgePulsarLevantarTecla( GLFWwindow* window, int key, int scancode, int action, int mods );

   /// @brief Método gestor de un evento de Scroll de la rueda del ratón
   /// @param window - ventana con el foco en el momento de producirse el scroll
   /// @param xoffset - (double) posicion del ratón en X
   /// @param yoffset - (double) posicion del ratón en Y
   ///
   virtual void mgeScroll( GLFWwindow* window, double xoffset, double yoffset  );

   /// @brief Método gestor de evento de pulsar o levantar un botón del ratón de esta clase
   /// @param window - ventana con el foco en el momento de levantar o pulsar el botón
   /// @param button - botón que ha sido pulsado o levantado (GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, )
   /// @param action - indica si se ha pulsado o levantado el botón (GLFW_PRESS, GLFW_RELEASE)
   /// @param mods - modificadores del teclado en el momento del evento
   ///
   virtual void mgePulsarLevantarBotonRaton( GLFWwindow* window, int button, int action, int mods );

   /// @brief Método gestor del evento de movimiento del ratón 
   /// @param window 
   /// @param xpos - (double) posición 
   /// @param ypos 
   ///
   virtual void mgeMovimientoRaton( GLFWwindow* window, double xpos, double ypos );

   /// @brief Devuelve la instancia actual de la aplicación (o derivada)
   ///
   static AplicacionBase * instancia() ;

   /// @brief devuelve el tamaño en X de la ventana actual (número de columnas)
   /// @return (int) tamaño en X de la ventana actual (>0)
   ///
   unsigned ventanaTamX() const { assert( ventana_tam_x > 0 ); return ventana_tam_x ; }

   /// @brief devuelve el tamaño en Y de la ventana actual (número de filas)
   /// @return (int) tamaño en Y de la ventana actual (>0)
   ///
   unsigned ventanaTamY() const { assert( ventana_tam_y > 0 ); return ventana_tam_y ; }

   /// @brief Devuelve el 'aspect ratio' de la ventana (tam.en Y/ tam. en X)
   /// @return (float) número de filas dividido por el número de columnas
   ///
   float aspectRatioVentanaYX() const { return float(ventanaTamY()) /float(ventanaTamX()) ; }

   /// @brief Procesa una pulsación de un tecla con la tecla 'S' pulsada,
   /// @brief (se redefine en las clases derivadas si se quiere procesar la tecla 'S'
   /// @brief por defecto, no hace nada y devuelve false)
   /// @param key - código de la tecla pulsada
   /// @return 'true' si se quiere revisualizar, false si no. 
   ///
   virtual bool procesarTeclaS( int key ) ;

   // --------------------------------------------------------------------
   protected:

   // puntero a la instancia actual de la clase 'AplicacionBase' (o derivadas)
   static AplicacionBase * aplBase ; 

   // numero de columnas (tam_x) y filas (tam_y) del viewport actual
   unsigned ventana_tam_x = 0,
            ventana_tam_y = 0 ;

    // puntero a la ventana GLFW que está usando la aplicación
   GLFWwindow * ventana_glfw = nullptr; 

   // variables de estado para gestionar el bucle principal de eventos.
   // se modifican en las funciones gestoras de eventos y se tienen en cuenta 
   // en el bucle principal de eventos.

   bool revisualizar      = true;    // true indica que hay que redibujar la coleccion
   bool terminar_programa = false;   // true indica que hay que cerrar la aplicación

   // factor de conversión para displays "retina" en macOS
   unsigned mouse_pos_factor = 1 ;      

   // versión de OpenGL (major y minor) - actualizado en 'InicializaGLFW', tras crear el contexto OpenGL.
   int context_major = -1 , 
       context_minor = -1 ;

   // variables de estado para gestionar eventos de arrastrar ratón 
   unsigned x_ant_mabd ;  // coord. X anterior del ratón en modo arrastrar con botón derecho pulsado
   unsigned y_ant_mabd ;  // coord. Y anterior del ratón en modo arrastrar con botón derecho pulsado

      
   // test de combinación de FBOs
   void testFBO();
};


