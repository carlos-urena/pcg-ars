// *********************************************************************
// **
// ** Máster en Desarrollo de Software (PCG+ARS)
// ** 
// ** Gestión de escenas (declaraciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de la clase 'ColeccionObjs' y derivadas 
// **
// **  + ColeccionObjs: clase con una colección de objetos para visualizar, las fuentes de luz
// **    y una cámara. En cada momento se visualiza uno de los objetos (se conmuta con 'o')
// **    Las clases derivadas incluyen cada una un constructor específico que crea una 
// **    colección distinta con diversos objetos, son estas:
// **
// **       + ColeccionObjs3D_1 : mallas indexadas sencillas
// **       + ColeccionObjs3D_2 : mallas indexadas generadas algoritmicamente
// **       + ColeccionObjs3D_3 : superficies paramétricas
// **       + ColeccionObjs3D_4 : grafos de escena
// **       + ColeccionObjs3D_5 : grafos de escena con materiales y texturas
// **       + ColeccionObjs3D_6 : grafos de escena con materiales, texturas e identificadores de selección.
// **       
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
#include "objeto-visu.h"

// *************************************************************************
// Clase ColeccionObjs
// -----------------
// Clase con un conjunto de objetos visualizables. 


class ColeccionObjs
{
   public:

   /// @brief crea una escena con una cámara orbital simple, materiales y luces por defecto.
   ///
   ColeccionObjs() ;

   /// @brief destructor
   ~ColeccionObjs() ;
   
   /// @brief pasa el objeto actual al siguiente
   ///
   void siguienteObjeto() ;

   /// @brief devuelve puntero al objeto actual (h)
   ///
   ObjetoVisu * objetoActual();

   /// @brief Escribe en el terminal una línea con info. del objeto actual 
   ///
   void imprimeInfoObjetoActual() ;

   /// @brief devuelve el nombre de la colección de objetos
   ///
   std::string nombre(); 

   /// @brief devuelve el número de objetos en la colección
   ///
   unsigned numObjetos() { return objetos.size(); }

   protected:

   // vector de objetos (alternativos: se visualiza uno de ellos nada más)
   std::vector<ObjetoVisu *> objetos ;

   // índice del objeto activo (en el vector 'objetos')
   unsigned ind_objeto_actual = 0 ;

   // nombre de la colección de objetos
   std::string nombreColeccion = "no asignado";
} ;


// -------------------------------------------------------------------------
// clases con diversas colecciones de objetos 

class ColeccionObjs3D_1 : public ColeccionObjs
{
   public:
      ColeccionObjs3D_1() ;
} ;
// -------------------------------------------------------------------------

class ColeccionObjs3D_2 : public ColeccionObjs
{
   public:
      ColeccionObjs3D_2() ;
} ;
// -------------------------------------------------------------------------

class ColeccionObjs3D_3 : public ColeccionObjs
{
   public:
      ColeccionObjs3D_3() ;
} ;
// -------------------------------------------------------------------------

class ColeccionObjs3D_4 : public ColeccionObjs
{
   public:
      ColeccionObjs3D_4() ;
} ;
// -------------------------------------------------------------------------


class ColeccionObjs3D_5 : public ColeccionObjs
{
   public:
      ColeccionObjs3D_5() ;
} ;


// ------------------------------------------------------------------------- 

class ColeccionObjs3D_6 : public ColeccionObjs
{
   public:
      ColeccionObjs3D_6() ;
} ;
// -------------------------------------------------------------------------

class ColeccionObjs2D : public ColeccionObjs
{
   public:
      ColeccionObjs2D() ;
} ; 


