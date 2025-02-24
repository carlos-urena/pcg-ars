// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Declaración de la clase 'Aplicacion3D'
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** La clase 'AplicacionIG' contine
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

#include "aplic-base.h"

// --------------------------------------------------------------------
///
/// @brief Clase con una aplicación 3D (derivada de 'AplicacionBase')
/// @brief Es una aplicación pensada para objetos 3D con iluminación y cámaras en 3D
/// 
class Aplicacion3D : public AplicacionBase
{
   public:
   
   /// @brief Constructor de Aplicacion3D: inicialización específica de la aplicacion 3D, adicional a la base.
   ///
   /// @brief (3) crea el cauce, 
   /// @brief (2) crea la pila de materiales
   /// @brief (4) crea el vector de coleccions
   /// 
   /// @param major (unsigned) version de OpenGL requerida (major)
   /// @param minor (unsigned) version de OpenGL requerida (minor)
   ///
   Aplicacion3D( const unsigned major, const unsigned minor ); 

   /// @brief Destructor de Aplicacion3D: libera los recursos (cauce, pila de materiales, colecciones)
   ///
   virtual ~Aplicacion3D() ;

   /// @brief Devuelve la instancia actual de la aplicación 3D (o derivada)
   /// @brief Si no hay ninguna instancia, produce un error.
   /// @brief Si la aplicación actual no es una instancia de Aplicacion3D, devuelve nullptr
   /// @return (Aplicacion3D *) instancia actual de la aplicación 3D
   ///
   static Aplicacion3D * instancia() ;

   /// @brief Inicializa GLFW, crea la ventana, registra funciones gestoras de eventos.
   /// @brief (se llama desde el constructor)
   //
   void inicializarGLFW() ;

   // métodos redefinidos para aplicaciones 3D 

   virtual void visualizarFrame() override ;
   virtual bool actualizarEstado( const float tiempo_seg ) override ;

   // métodos gestores de eventos, redefinidos:

   /// @brief Método gestor de un evento de pulsar o levantar una tecla en el teclado
   /// @param window - ventana con el foco en el momento de pulsar o levantar la tecla
   /// @param key - código de tecla
   /// @param scancode - código de tecla (dependiente del teclado)
   /// @param action - indica si se ha pulsado o levantado la tecla
   /// @param mods - modificadores del teclado en el momento del evento
   ///
   virtual void mgePulsarLevantarTecla( GLFWwindow* window, int key, int scancode, int action, int mods ) override;
   
   /// @brief Método gestor de un evento de Scroll de la rueda del ratón
   /// @param window - ventana con el foco en el momento de producirse el scroll
   /// @param xoffset - (double) posicion del ratón en X
   /// @param yoffset - (double) posicion del ratón en Y
   ///
   virtual void mgeScroll( GLFWwindow* window, double xoffset, double yoffset  ) override ;
   
   /// @brief Método gestor de evento de pulsar o levantar un botón del ratón de esta clase
   /// @param window - ventana con el foco en el momento de levantar o pulsar el botón
   /// @param button - botón que ha sido pulsado o levantado (GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, )
   /// @param action - indica si se ha pulsado o levantado el botón (GLFW_PRESS, GLFW_RELEASE)
   /// @param mods - modificadores del teclado en el momento del evento
   ///
   virtual void mgePulsarLevantarBotonRaton( GLFWwindow* window, int button, int action, int mods ) override ;
   
   /// @brief Método gestor del evento de movimiento del ratón 
   /// @param window 
   /// @param xpos - (double) posición 
   /// @param ypos 
   ///
   virtual void mgeMovimientoRaton( GLFWwindow* window, double xpos, double ypos ) override;

   /// @brief Devuelve un puntero al cauce 3D usado en la aplicación 
   /// @brief Si es nulo, se produce un error.
   ///
   Cauce3D * cauce3D() ;

   /// @brief En general: indica si la aplicación es animable en el estado actual o no lo es.
   /// @brief En este caso, devuelve 'true' si el objeto actual de la colección actual tiene parámetros animables (leerNumParametros()>0)
   ///
   virtual bool animable() override ;

   /// @brief Método principal de selección, se llama al hacer click con el botón izquierdo
   /// @brief usa la posición donde se ha hecho click en coordenadas del dispositivo del FBO (enteras).
   /// @brief Si en ese pixel hay un objeto con identificador >0, ejecuta 'cuandoClick' del objeto.
   /// @brief y pone la camara actual de la coleccion actual mirando hacia el centro del objeto.
   ///
   /// @param x (int) posición en X del click (coords. de dispoitivo)
   /// @param y (int) posición en Y del click (coords. de dispositivo)
   /// @return (bool) true si se ha seleccionado algún objeto, false si no
   ///
   bool seleccion( int x, int y );

   /// @brief devuelve la pila de materiales actual (nunca nula)
   ///
   PilaMateriales * pilaMateriales();

   /// @brief devuelve 'true' si la iluminación está activada, 'false' si no
   ///
   bool iluminacionActiva() { return iluminacion ; } ; 

   /// @brief Imprime información sobre la colección actual del objeto
   ///
   void imprimeInfoColeccionActual() ;

   /// @brief Procesa una pulsación de un tecla con la tecla 'S' pulsada,
   /// @brief Incrementa o decrementa el 'uniform' 'S' en el cauce de contorno.
   /// @param key - código de la tecla pulsada
   /// @return 'true' si ha pulsado '+' o '-', false si no.
   ///
   virtual bool procesarTeclaS( int key ) override ;
   
   // ------------------------------------------------------------- 
   protected:


   // -------------------------------------------------------------
   // Métodos no públicos 

   /// @brief devuelve la cámara actual (nunca nula
   /// @return cámara actual (nunca nula) 
   ///
   CamaraInteractiva * camaraActual();


   /// @brief pasa la cámara actual a la siguiente
   ///
   void siguienteCamara();

   /// @brief Visualiza un objeto (3D) usando una cámara, poniendo el cauce en el estado adecuado
   ///    
   void visualizarGL_OA( );

   /// @brief visualiza el objeto actual de la cámara, pero en modo selección 
   ///
   void visualizarGL_OA_Seleccion(  );
   
   /// @brief Devuelve la colección de objetos actual (nunca nula)
   ColeccionObjs * coleccionActual();

   /// @brief visualiza las normales del objeto actual de la escena
   void visualizarNormalesGL(  );

   /// -------------------------------------------------------------
   /// Variables de instancia no públicas

   // vector de cámaras (se usa una en cada momento)
   std::vector<CamaraInteractiva *> camaras ;

   // material por defecto que se activa antes de dibujar (con iluminación activada)
   Material * material_ini = nullptr ;

   // colección de fuentes de luz que se usarán para dibujar, (con iluminación activada)
   ColFuentesLuz * col_fuentes = nullptr ;

   // índice de la cámara activa (en el vector 'camaras')
   unsigned ind_camara_actual = 0 ;


   // puntero a la pila de materiales actual (incluye material actual)
   PilaMateriales * pila_materiales = nullptr ;

   // vector con punteros a las distintas colecciones de objetos que gestiona la aplicación
   std::vector<ColeccionObjs *> colecciones_objs;                   
   
   // índice de la coleccion actual en el vector de coleccions
   unsigned ind_coleccion_act = 0;  

   // modo de visualización actual
   ModosVisu modo_visu = ModosVisu::relleno ;

   // Tipo de normales para Phong shading:
   // true  --> usar normales de triángulos 
   // false -->  usar normales de vértices interpoladas (tradicional)
   bool usar_normales_tri = false ;  

   // indica si durante la visualización se usa iluminación y materiales (true), 
   // o simplemente se usa un color plano (false)
   bool iluminacion = true ;

   // Indica si queremos dibujar aristas al visualizar sólido (true) o no
   bool dibujar_aristas = false ;

   // Indica si queremos visualizar normales al visualizar los objetos (true) o no.
   bool visualizar_normales = false ;

   // true cuando se está en proceso de visualizar normales durante una llamada a 'coleccion::visualizarNormales'
   //bool visualizando_normales = false ;

   // 'true' para dibujar ejes, 'false' -> no dibujar ejes
   bool dibujar_ejes = true ;

    // 'true' --> fondo blanco color negro, 'false' --> fondo oscuro
   bool fondo_blanco = false ;

    // 'true' -> visualizar FBO de seleccion, 'false' -> no visualizarlo
   bool visualizar_fbo = false ;

   // puntero al framebuffer usado para selección
   Framebuffer * fbo = nullptr ;

   // 'true' imprimir tiempo por frame, 'false', no imprimir 
   bool imprimir_tiempos  = false;  

   // puntero al cauce activo actualmente
   Cauce3D * cauce = nullptr ;  

}; // fin clase Aplicacion3D


