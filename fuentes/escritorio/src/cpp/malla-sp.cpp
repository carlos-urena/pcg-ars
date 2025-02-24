// *********************************************************************
// **
// ** Asignatura: Programación del Cauce Gráfico (MDS)
// ** 
// ** Mallas indexadas que aproximan superficies paramétricas (implementación)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de las clases 
// **        + MallaSupPar: malla indexada de triángulos, que aproxima una superficie paramétrica cualquiera
// **        + Diversas clases derivadas de MallaSupPar, que aproximan superficies paramétricas concretas
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

#include <limits>
#include <set>
#include "utilidades.h"
#include "malla-sp.h" 

// ---------------------------------------------------------------------

MallaSupPar::MallaSupPar( const FuncionParam * p_fp, const unsigned p_ns, const unsigned p_nt  )
{
   using namespace glm ;

   assert( 1 < p_nt && 1 < p_ns );
   assert( p_fp != nullptr );

   fp = p_fp ;
   ns = p_ns ;
   nt = p_nt ;

   ponerNombre( fp->leerNombre() + " generado como superf. parámetrica (" + std::to_string(ns) + " x " + std::to_string(nt) + ")");
   
   // agregar los vértices y triángulos, por filas
   
   for( unsigned it = 0 ; it < nt ; it++ )  
   for( unsigned is = 0 ; is < ns ; is++ )   
   {
      const vec2 c = vec2( float(is)/float(ns-1), float(it)/float(nt-1) );
      
      vertices.push_back( fp->evaluarPosicion( c ));
      cc_tt_ver.push_back( vec2( c.s, 1.0-c.t ));

      if ( is < ns-1 && it < nt-1 )
      {
         const unsigned 
            iv00 = (it+0)*ns + (is+0),
            iv01 = (it+1)*ns + (is+0), 
            iv10 = (it+0)*ns + (is+1),
            iv11 = (it+1)*ns + (is+1);

         triangulos.push_back( uvec3( iv00, iv11, iv01 ));
         triangulos.push_back( uvec3( iv00, iv10, iv11 ));

         // aquí arriba hay que tener en cuenta que la coordenada T crece de "arriba abajo"
         // y la coordenadas S crece de "izquierda a derecha", así que hay que dar la indices en
         // este orden para que las normales de las caras y vértices esten "hacia fuera" ....
         
      }
   }

   calcularNormales();
}
// ---------------------------------------------------------------------

void MallaSupPar::promediarNormalesCol()
{   
   using namespace glm ;

   for( unsigned it = 0 ; it < nt ; it++ )  
   {
      const unsigned  
         iv0 = it*ns,      // índice del 1er vértice de la fila
         iv1 = iv0+ns-1 ;  // índice del último vértice de la fila.

      const vec3 
         n_promedio = normalize( nor_ver[ iv0 ] + nor_ver[ iv1 ] );

      nor_ver[ iv0 ] = n_promedio ;
      nor_ver[ iv1 ] = n_promedio ;
   }
}
// ---------------------------------------------------------------------
    
MallaSPEsfera::MallaSPEsfera( const unsigned ns, const unsigned nt )

:  MallaSupPar( new FPEsfera(), ns, nt )
{
   promediarNormalesCol();
}
// ---------------------------------------------------------------------


MallaSPCilindro ::MallaSPCilindro( const unsigned ns, const unsigned nt )

:  MallaSupPar( new FPCilindro(), ns, nt )
{      
   promediarNormalesCol();
}
// ---------------------------------------------------------------------

MallaSPCono ::MallaSPCono( const unsigned ns, const unsigned nt )

:  MallaSupPar( new FPCono(), ns, nt )
{      
   promediarNormalesCol();
}
// ---------------------------------------------------------------------

MallaSPColumna::MallaSPColumna( const unsigned ns, const unsigned nt )

:  MallaSupPar( new FPColumna(), ns, nt )
{      
   promediarNormalesCol();
}
// ---------------------------------------------------------------------


