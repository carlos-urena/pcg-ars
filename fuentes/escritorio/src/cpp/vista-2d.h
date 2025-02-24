// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Gestión de vistas 2D (declaraciones)
// ** Copyright (C) 2016-2024 Carlos Ureña
// **
// ** Declaraciones de las clases 'Vista2D'
// **   (parámetros que determinan qué región de las WCC se ven en el viewport)
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

// ******************************************************************
//
/// @brief Clase con los parámetros necesarios para la transformación de coordenadas de mundo a 
/// @brief coordenadas normalizadas de dispositivo (sirven para calcular las matrices de 
/// @brief vista y proyección). 
/// @brief 
/// @brief Se especifica el tamaño (lado) y centro del cuadrado más grande completamente visible (CV),
/// @brief cuadrado que se especifica mediante su lado y centro en coordenadas de mundo.
/// @brief El CV se mapea al viewport con estos requerimientos:
/// @brief
/// @brief (a) el lado del cuadrado visible se mapea al lado menor del viewport y
/// @brief (b) el centro del cuadrado visible se mapea al centro del viewport,
/// @brief 
/// @brief de forma que el aspect ratio del cuadrado visible se mantiene en el mapeo
///
class Vista2D
{
   public: // ----------------------------

   /// @brief Constructor de Vista2D: inicialización a valores por defecto
   /// @brief (centro en el origen, lado 2, ángulo 0)
   ///
   Vista2D( const unsigned vp_nx_ini, const unsigned vp_ny_ini ) ;

   /// @brief fija las matrices modelview y projection en el cauce
   ///
   void activar( Cauce2D & cauce )  ;

   /// @brief devuelve una cadena con la descripción de la vista
   /// @return cadena (string) con la descripción de la vista
   ///
   std::string descripcion() ;

   /// @brief fijar las dimensiones del viewport 
   ///
   void fijarViewport( const unsigned nuevo_vp_nx, const unsigned nuevo_vp_ny ) ;

   /// @brief Hace zoom (amplia o reduce el tamaño aparente de los objetos), es decir,
   /// @brief multiplica el lado por (1+v) elevado a 'exp', donde 'v' es 0.05f,  
   /// @brief este valor 'v' se puede cambiar para ajustar la velocidad del zoom.   
   /// @param d - reduce cuando exp > 0, amplia cuando exp < 0, típicamente 'exp' = +1 o -1
   ///
   void zoom( const float d ) ;

   /// @brief Desplaza la vista (traslada el centro del cuadrado visible)
   /// @param npx - distancia a trasladar en X (en DC, es decir, en unidades de pixels)
   /// @param npy - distancia a trasladar en Y (en DC, es decir, en unidades de pixels)
   ///
   void desplazar( const float npx, const float npy );

   /// @brief Devuelve el lado de un pixel en coords. de mundo (WCC)
   /// @return lado del pixel en WCC 
   ///
   float ladoPixelWCC() { return lado_pixel_wcc ; }

   // ---------------------------------------------------------------------
   protected: 

   /// @brief recalcula las matrices de vista y proyección en función de los parámetros 
   /// @brief actuales de la vista 
   ///
   void actualizarMatrices() ;

   // centro del cuadrado más grande completamente visible en el viewport (CV), 
   // en coordenadas de mundo (inicialmente es el origen)
   glm::vec3 cv_centro_wcc = glm::vec3(0.0,0.0,0.0) ;

   // tamaño (lado) del cuadrado más grande actualmente  
   // visible en el viewport, en coordenadas del mundo 
   // (inicialmente va de -1 a +1 en X y en Y, así que el lado es 2)
   float cv_lado_wcc = 2.0f ;

   /// ángulo en grados de rotación del cuadrado visible entorno a Z (inicialmente 0)
   float cv_angulo_grad = 0.0f ;

   /// número de filas y columnas de pixels del viewport
   unsigned vp_nx = 0, vp_ny = 0 ;

   /// tamaño de un pixel en world coordinates en X y en Y (deben coincidir)
   float lado_pixel_wcc = 0.0f ;

   /// matriz de vista 
   glm::mat4 matriz_vista ;

   /// matriz de proyección
   glm::mat4 matriz_proyeccion ;

} ;

// ******************************************************************