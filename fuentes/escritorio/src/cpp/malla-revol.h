// *********************************************************************
// **
// ** Mallas indexadas (implementación)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Declaración de las clases 
// **    + MallaRevol: malla indexada de triángulos obtenida por 
// **      revolución de un perfil (derivada de MallaInd)
// **    + MallaRevolPLY: malla indexada de triángulos, obtenida 
// **      por revolución de un perfil leído de un PLY (derivada de MallaRevol)
// **    + algunas clases derivadas de MallaRevol
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

#include <vector>          // usar std::vector
#include <string>

#include "malla-ind.h"   // declaración de 'Objeto3D'
// ---------------------------------------------------------------------
// clase para mallas indexadas obtenidas a partir de la revolución de un perfil

class MallaRevol : public MallaInd
{
   private:

   //#inicio
   // (detalle de implementación que no está en la plantilla)
   unsigned nper , // numero de perfiles
            nvp  ; // numero de vertices por perfil
   unsigned indice( const unsigned iper, const unsigned iver ) const ;
   //#fin

   protected: //

   MallaRevol() {} // solo usable desde clases derivadas con constructores especificos

   // Método que crea las tablas de vértices, triángulos, normales y cc.de.tt.
   // a partir de un perfil y el número de copias que queremos de dicho perfil.
   void inicializar
   (
      const std::vector<glm::vec3> & perfil,     // tabla de vértices del perfil original
      const unsigned                 num_copias  // número de copias del perfil
   ) ;
} ;
// --------------------------------------------------------------------- 

class DonutRevol : public MallaRevol
{
   public:
      DonutRevol( const float r1, const float r2,
                  const unsigned nladospol, const unsigned ncopias );
} ;
// ---------------------------------------------------------------------

class MallaRevolPLY : public MallaRevol
{
   public:
   MallaRevolPLY( const std::string & nombre_arch,
                  const unsigned nperfiles ) ;
} ;
// ---------------------------------------------------------------------

class ConoRevol : public MallaRevol
{
   public:
   ConoRevol() ;
} ;
// ---------------------------------------------------------------------


