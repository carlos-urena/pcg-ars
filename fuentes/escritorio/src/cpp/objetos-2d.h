// *********************************************************************
// **
// ** Asignatura: INFORMÁTICA GRÁFICA
// ** 
// ** Gestión de grafos de escena (declaraciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Declaraciones de: 
// **     + Clase 'NodoGrafoEscena' (derivada de 'Objeto3D')
// **     + Clase 'EntradaNGE' (una entrada de un nodo del grafo de escena)
// **     + Tipo enumerado 'TipoEntNGE' (tipo de entradas del nodo del grafo de escena)
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

#include "vaos-vbos.h"
#include "objeto-visu.h"

// ---------------------------------------------------------------------
///
/// @brief Clase para polígonos en 2D, 
/// @brief Cada objeto es una malla de triángulos (relleno) delimitados 
/// @brief por una secuencia de vértices que forma el contorno. 
///
class Poligono2D : public ObjetoVisu2D
{
   public:

   Poligono2D(  ) ;
   ~Poligono2D() ;

   // visualizar el objeto con OpenGL, métodos virtuales 

   /// @brief Visualiza el relleno del polígono con OpenGL
   ///   
   virtual void visualizarGL(  ) override ;

   /// @brief Visualiza el contorno del polígono con OpenGL
   ///
   virtual void visualizarSegmentosGL() override ;
   
   /// @brief Visualiza el relleno en modo selección.
   ///
   virtual void visualizarModoSeleccionGL() override;

   /// @brief Fija la nueva textura a usar para visualizar el relleno del objeto.
   /// @brief (si tiene textura, se usa para el relleno)
   /// @param nueva_textura (Textura*) -- puntero a la nueva textura a usar, null para que no tenga textura.
   ///
   void fijarTextura( Textura * nueva_textura ); 

   /// @brief Fija el valor lógico que determina si se visualiza el relleno o no 
   /// @param nuevo_visualizar_relleno (bool) -- 'true' si se visualiza el relleno, 'false' en otro caso
   ///   
   void visualizarRelleno( bool nuevo_visualizar_relleno );

   /// @brief Fija el valor lógico que determina si se visualizan los puntos del contorno
   /// @brief como discos o no se visualizan en absoluto
   /// @param nuevo_visualizar_puntos (bool) -- 'true' si se visualizan los puntos, 'false' en otro caso
   ///
   void visualizarPuntosContorno( bool nuevo_visualizar_puntos );

   /// @brief Fija el valor lógico que determina si se visualizan las lineas del contorno
   /// @brief como rectángulos o no se visualizan en absoluto
   /// @param nuevo_visualizar_lineas (bool) -- 'true' si se visualizan las lineas, 'false' en otro caso
   ///
   void visualizarLineasContorno( bool nuevo_visualizar_lineas );

   /// @brief Fijar el color a usar para la visualización de las lineas del contorno
   /// @param nuevo_color_lineas (vec3) -- nuevo color para las líneas
   ///
   void fijarColorLineasContorno( const glm::vec3 nuevo_color_lineas );

   /// @brief Fijar el color a usar para la visualización de los puntos del contorno
   /// @param nuevo_color_puntos (vec3) -- nuevo color para los puntos
   ///
   void fijarColorPuntosContorno( const glm::vec3 nuevo_color_puntos );

   /// @brief Fija el ancho de las lineas en WCC  
   /// @param nuevo_ancho_lineas (float) -- nuevo ancho de las lineas
   ///
   void fijarAnchoLineasWCC( const float nuevo_ancho_lineas_wcc );

   /// @brief Fija el radio de los puntos en WCC
   /// @param nuevo_radio_puntos (float) -- nuevo radio de los puntos
   ///
   void fijarRadioPuntosWCC( const float nuevo_radio_puntos_wcc );

   // --------------------------------------------------------------------- 
   protected:

   void crearVAOtriangulos();
   void crearVAOcontorno();
    
   std::vector<glm::vec2>  vertices ; // vector de coordenadas de vértices 
   std::vector<glm::vec3>  colores_ver ; // vector de colores de vértices
   std::vector<glm::vec2>  cct_ver ; // vector de coordenadas de textura de 
   std::vector<glm::uvec3> triangulos ; // vector de triángulos (índices de vértices)
   std::vector<unsigned>   contorno ; // vector de índices de vértices del contorno

   DescrVAO * dvao_tris = nullptr ; // vao de triángulos (para visualizarGL)
   DescrVAO * dvao_cont = nullptr ; // vao de lineas (para contorno, en visualizarSegmentosGL)

   Textura * textura = nullptr ; // puntero a la textura asignada al objeto (nullptr si no tiene)

   bool  visualizar_relleno = true , // determina si se visualiza el relleno
         visualizar_puntos = true ,  // determina si se visualizan los puntos del contorno 
         visualizar_lineas = true ;  // determina si se visualizan las líneas del contorno

   float ancho_lineas_wcc = 0.030,  // ancho inicial para las líneas del contorno
         radio_puntos_wcc = 0.020 ; // radio inicial para los puntos del contorno

   glm::vec3 color_lineas = { 0.0, 0.0, 0.0 }, // color inicial para las líneas del contorno
             color_puntos = { 0.0, 0.0, 0.0 }; // color inicial para los puntos del contorno
   
} ;

// ---------------------------------------------------------------------

/// @brief Clase para un cuadrado en 2D, tiene lado 2 y centro en el origen.

class Cuadrado : public Poligono2D
{
   public:

   /// @brief crear un cuadrado (posiblement con una textura)
   /// @param textura -- textura del cuadrado, si es nullptr no tiene textura
   Cuadrado( Textura * textura = nullptr ) ;
} ;

// ---------------------------------------------------------------------
///
/// @brief Clase para polígonos en 2D, con la secuencia de vértices leídos de un polígono 
/// @brief Cada objeto es una malla de triángulos (relleno) delimitados 
/// @brief por una secuencia de vértices que forma el contorno. 
///
class PoligonosDeTXT : public ObjetoVisu2D
{ 
   public:
   PoligonosDeTXT( const std::string & nombre_fichero ) ;

   virtual void visualizarGL(  ) override ;
   virtual void visualizarSegmentosGL() override ;
   virtual void visualizarModoSeleccionGL() override ;

   private:
   std::vector< std::vector<glm::vec2> >  vertices ; // vector de vectores de coordenadas de vértices
   std::vector<DescrVAO *> dvaos ; 
   bool dvaos_creados = false ;
   void crearVAOs();
   glm::vec2 centro, tamano ;
   Cuadrado * recuadro = nullptr ;
} ;

// ---------------------------------------------------------------------
///
/// @brief Clase para un polígono 2D con forma de estrella de N puntas
///
class Estrella : public Poligono2D 
{
   public: 
   Estrella( const unsigned n_puntas, const float radio1, const float radio2, Textura * textura = nullptr ) ;
} ;



