// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// **
// ** Gestión del cauce gráfico 2D (clase 'Cauce2D') (declaración)
// **
// ** Copyright (C) 2017-2022 Carlos Ureña
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
#include "cauce-base.h"

// mapeo de atributos usados con índices de atributos enteros
// índices de los atributos en los shaders de este cauce
// (por ahora son los mismos que en el cauce base)

constexpr GLuint 
   numero_atributos_cauce_2d = numero_atributos_cauce_base  ;

// -------------------------------------------------------------------------------------


/// @brief Cauce adaptado para visualizar objetos 2D.
/// @brief Define un vertex y un fragment shader sencillos.

class Cauce2D : public CauceBase
{
   public:

   Cauce2D() ;
   virtual ~Cauce2D() ;
   virtual std::string descripcion()  override ;
} ;

// -------------------------------------------------------------------------------------

/// @brief Cauce 2D para visualizar segmentos de línea, pudiéndose 
/// @brief especificar el grosor y color de los segmentos.
/// @brief Unicamente visualiza el tipo de primitiva GL_LINE
/// @brief (sirve de ejemplo de uso de un geometry shader)
///
class Cauce2DLineas : public CauceBase
{
   public:

   Cauce2DLineas() ;
   virtual ~Cauce2DLineas() ;

   /// @brief fija el grosor de las líneas (en WCC)
   /// @param nuevo_ancho_wcc - nuevo ancho de líneas y diámetro de puntos
   ///
   void fijarGrosorLineasWCC( float nuevo_ancho_wcc );

   /// @brief fija el radio de los puntos en Wcc
   /// @param nuevo_ancho_wcc - nuevo ancho de líneas y diámetro de puntos
   ///
   void fijarRadioPuntosWCC( float nuevo_radio_wcc );

   /// @brief Fija el valor lógico que determina si se visualizan los puntos 
   /// @brief como discos o no se visualizan en absoluto 
   /// @param nuevo_visualizar_puntos 
   ///
   void fijarVisualizarPuntos( bool nuevo_visualizar_puntos );

   /// @brief Fija el valor lógico que determina si se visualizan los puntos 
   /// @brief como discos o no se visualizan en absoluto 
   /// @param nuevo_visualizar_puntos 
   ///
   void fijarVisualizarLineas( bool nuevo_visualizar_lineas );

   /// @brief lee el nombre o descripción del cauce
   virtual std::string descripcion(  )  ;

   protected:

   static constexpr float 
      grosor_lineas_wcc_inicial = 0.01f ,
      radio_puntos_wcc_inicial  = 0.01f ;
   
   void inicializarUniforms2DLineas();   // uniforms de 'Cauce2DSegmentos'

   // localizadores de los uniforms específicos de este cauce
   GLint loc_grosor_lineas_wcc         = -1 ;  // localizador del ancho de las líneas en WCC
   GLint loc_radio_puntos_wcc          = -1 ;  // localizador del radio de los puntos en WCC
   GLint loc_visualizar_puntos = -1 ;  // localizador del valor lógico que determina si se visualizan los puntos
   GLint loc_visualizar_lineas = -1 ;  // localizador del valor lógico que determina si se visualizan las líneas
} ; 

