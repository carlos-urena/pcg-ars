// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Declaración de la clase 'Aplicacion2D'
// ** Copyright (C) 2016-2023 Carlos Ureña
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

#include "cauce-2d.h"
#include "vista-2d.h"
#include "aplic-base.h"

// --------------------------------------------------------------------
///
/// @brief Clase con una aplicación 2D (derivada de 'AplicacionBase')
/// @brief Es una aplicación pensada para objetos visualizables 2D, con  
/// @brief un cauce gráfico 2D y visualización de primitivas 2D con colores y 
/// @brief coordenadas de textura (sin normales ni iluminación)
/// 
class Aplicacion2D : public AplicacionBase
{
   public:
   
   /// @brief Constructor de Aplicacion2D: 
   /// 
   Aplicacion2D(); 

   /// @brief Destructor de Aplicacion2D:
   virtual ~Aplicacion2D() ;

   /// @brief Devuelve el cauce gráfico 2D de esta aplicación 
   Cauce2D * cauce2D();

   /// @brief Devuelve el cauce gráfico 2D para lineas (segmentos) de esta aplicación 
   Cauce2DLineas * cauce2DLineas();

   /// @brief Devuelve la instancia actual de la aplicación 3D (o derivada)
   ///
   static Aplicacion2D * instancia() ;

   /// @brief Devuelve la vista 2D asociada a esta aplicación (no puede ser nula)
   Vista2D * vista2D() ;

   // modos virtuales redefinidos
   virtual void visualizarFrame() override ;

   virtual bool animable() override ;
   virtual bool actualizarEstado( const float tiempo_seg ) override ;

   virtual void mgePulsarLevantarTecla( GLFWwindow* window, 
                                int key, int scancode, int action, int mods ) override ;

   virtual void mgeMovimientoRaton( GLFWwindow* window, 
                                    double xpos, double ypos ) override;

   virtual void mgeScroll( GLFWwindow* window, double xoffset, double yoffset  ) override ;

   virtual void mgePulsarLevantarBotonRaton( GLFWwindow* window, 
                                int button, int action, int mods ) override ;

   /// @brief Procesa una pulsación de un tecla con la tecla 'S' pulsada,
   /// @brief Incrementa o decrementa el 'uniform' 'S' en el cauce de contorno.
   /// @param key - código de la tecla pulsada
   /// @return 'true' si ha pulsado '+' o '-', false si no.
   ///
   virtual bool procesarTeclaS( int key ) override ;

   // ---------------------------------------------------------------------
   protected: 

   Cauce2D *       cauce2d = nullptr;
   Cauce2DLineas * cauce2d_lineas = nullptr ;
   ColeccionObjs * col_objetos = nullptr ;
   Vista2D *       vista2d = nullptr ;  

   // centro del cuadrado visible, en coordenadas de mundo
   glm::vec3 cv_centro_wcc = glm::vec3(0.0,0.0,0.0) ;

   // tamaño (lado) del cuadrado más grande actualmente  
   // visible en el viewport, en coordenadas del mundo 
   // (inicialmente va de -1 a +1 en X y en Y, así que el lado es 2)
   float cv_lado_wcc = 2.0f ;

   // modo de visualizacion de contornos (solidos o lineas, inicialmente solidos)
   bool contornosSolidos = true ;
   
}; // fin clase Aplicacion2D


