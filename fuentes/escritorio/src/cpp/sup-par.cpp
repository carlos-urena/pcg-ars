// *********************************************************************
// **
// ** Asignatura: Programación del Cauce Gráfico (MDS)
// ** 
// ** Funciones de parametrización para superficies paramétricas (declaraciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Declaración de las clases 
// **        + FuncionParam: clase base para funciones de parametrización de superficies
// **        + Diversas clases derivadas de 'FuncionParam' para superficies paramétricas concretas
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
#include <sup-par.h>

FuncionParam::FuncionParam( const std::string & nombre_inicial )
{
   fijarNombre( nombre_inicial ) ;
}
// ---------------------------------------------------------------------

void FuncionParam::fijarNombre( const std::string & nuevo_nombre ) 
{
   nombre = nuevo_nombre ;
}
// ---------------------------------------------------------------------
      
const std::string & FuncionParam::leerNombre( ) const 
{
   return nombre ;
}
// ---------------------------------------------------------------------


glm::vec3 FPEsfera::evaluarPosicion( const glm::vec2 & st ) const 
{
   assert( 0.0 <= st.s && st.s <= 1.0 );
   assert( 0.0 <= st.t && st.t <= 1.0 );

   const float
      a  = M_PI * (2.0*st.s),
      b  = M_PI * (st.t-0.5),
      ca = std::cos( a ),
      sa = std::sin( a ),
      cb = std::cos( b ),
      sb = std::sin( b ) ;

   return glm::vec3( sa*cb, sb, ca*cb );
}

// ---------------------------------------------------------------------

glm::vec3 FPCilindro::evaluarPosicion( const glm::vec2 &  st ) const 
{
   assert( 0.0 <= st.s && st.s <= 1.0 );
   assert( 0.0 <= st.t && st.t <= 1.0 );

   const float
      a  = M_PI * (2.0*st.s),
      ca = std::cos( a ),
      sa = std::sin( a ) ;

   return glm::vec3( sa, st.t, ca );
}

// ---------------------------------------------------------------------

glm::vec3 FPCono::evaluarPosicion( const glm::vec2 &  st ) const 
{
   assert( 0.0 <= st.s && st.s <= 1.0 );
   assert( 0.0 <= st.t && st.t <= 1.0 );

   const float
      a  = M_PI * (2.0*st.s),
      ca = std::cos( a ),
      sa = std::sin( a ),
      r  = 1.0-st.t ;

   return glm::vec3( r*sa, st.t, r*ca );
}
// ---------------------------------------------------------------------

glm::vec3 FPColumna::evaluarPosicion( const glm::vec2 & st ) const 
   
{
   assert( 0.0 <= st.s && st.s <= 1.0 );
   assert( 0.0 <= st.t && st.t <= 1.0 );

   const float
      a  = M_PI * (2.0*st.s),
      ca = std::cos( a ),
      sa = std::sin( a ),
      r  = 1.0+0.1*std::sin( 5.0* (a + 2.0*M_PI*st.t) ) ;

   return glm::vec3( r*sa, 10.0*(st.t-0.5), r*ca );
} ;
