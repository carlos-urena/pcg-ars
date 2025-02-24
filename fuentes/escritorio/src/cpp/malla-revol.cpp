// *********************************************************************
// **
// ** Asignatura: INFORMÁTICA GRÁFICA
// ** 
// ** Mallas indexadas (implementación)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de las clases 
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

#include "utilidades.h"
#include "lector-ply.h"
#include "malla-revol.h"

using namespace std ;

// *****************************************************************************

unsigned MallaRevol::indice( const unsigned iper, const unsigned iver ) const
{
   assert( iper < nper );
   assert( iver < nvp );
   return iper*nvp + iver ;
}
// -----------------------------------------------------------------------------

// Método que crea las tablas de vértices, triángulos, normales y cc.de.tt.
// a partir de un perfil y el número de copias que queremos de dicho perfil.
void MallaRevol::inicializar
(
   const std::vector<glm::vec3> & perfil,     // tabla de vértices del perfil original
   const unsigned               num_copias  // número de copias del perfil
)
{
   using namespace glm ;
   
   // Algoritmo de creación de malla de revolución
   
   // inicializar y comprobar 'nvp' y 'nper'

   // comprobar los parámetros
   nvp  = perfil.size() ;  assert( 2 < nvp ); // numero de vertices del perfil
   nper = num_copias ;     assert( 2 < nper );

   // calcular las normales del perfil original

   std::vector<vec3> nor_ver_perfil ;

   for( int iver = 0 ; iver < int(nvp) ; iver++ )
   {
      const vec3 d = perfil[ std::min( iver+1, int(nvp)-1 )] -
                        perfil[ std::max( iver-1, 0 )] ;
      nor_ver_perfil.push_back( normalize( vec3( d.y, -d.x, 0.0 ) ) );
   }

   // calcular la tabla de distancias a lo largo del perfil:

   //glm::vec3           vant ;
   float               sum_distancias = 0.0 ;
   std::vector<float>  distancias ;

   for( unsigned iver = 0 ; iver < nvp ; iver++ )
   {
      const int iver_ant = std::max( 0, int(iver)-1 ) ;
      sum_distancias += length( perfil[iver]-perfil[iver_ant] ) ;
      distancias.push_back( sum_distancias );
   }
   assert( 0.0 < sum_distancias );
   for( unsigned iver = 0 ; iver < nvp ; iver++ )
      distancias[iver] = distancias[iver]/sum_distancias ;

   // ángulo en grados entre copias consecutivas de un perfil
   const float dang = 360.0/float(nper-1);

   // añadir vértices
   for( unsigned iper =  0 ; iper < nper ; iper++ )
   {
      const float    angulo_grad = dang*float(iper) ;
      const mat4     rot         = rotate( radians( angulo_grad ), vec3( 0.0, 1.0, 0.0 ));
      const float    coordt_s    = angulo_grad / 360.0 ;

      for( unsigned iver = 0 ; iver < nvp ; iver++ )
      {
         vertices.push_back( rot* vec4( perfil[iver], 1.0f ) );
         nor_ver.push_back( rot* vec4( nor_ver_perfil[iver], 0.0f ));
         cc_tt_ver.push_back( vec2( coordt_s, 1.0-distancias[iver] ) );
      }
   }

   // añadir caras entre copias del perfil
   for( unsigned iper0 =  0 ; iper0 < nper-1 ; iper0++ )
   {
      const unsigned iper1 = (iper0+1) % nper ; // el último 'iper0' es 'nper-2' luego el módulo aquí no hace nada
      for( unsigned iver0 = 0 ; iver0 < nvp-1 ; iver0++ )
      {  const unsigned
            iver1 = iver0+1 ,
            iv01  = indice( iper0, iver1 ), iv11  = indice( iper1, iver1 ),
            iv00  = indice( iper0, iver0 ), iv10  = indice( iper1, iver0 );
         triangulos.push_back( uvec3( iv00, iv11, iv01 ) );
         triangulos.push_back( uvec3( iv00, iv10, iv11 ) );
      }
   }
}

// -----------------------------------------------------------------------------
// constructor, a partir de un archivo PLY

MallaRevolPLY::MallaRevolPLY
(
   const std::string & nombre_arch,
   const unsigned      nperfiles
)
{
   ponerNombre( std::string("Malla de revolución, perfil en '"+ nombre_arch + "'" ));

   // Crear la malla de revolución
   // Leer los vértice del perfil desde un PLY, después llamar a 'inicializar'
   
   std::vector<glm::vec3> perfil_orig ;
   LeerVerticesPLY( nombre_arch, perfil_orig );

   inicializar( perfil_orig, nperfiles );
}


// *****************************************************************************

DonutRevol::DonutRevol( const float r1, const float r2,
              const unsigned nladospol, const unsigned ncopias )
{

   ponerNombre("Toroide generado por revolución");

   std::vector<glm::vec3> polig ;
   assert( 3 <= nladospol );

   for( unsigned iv = 0 ; iv < nladospol ; iv++ )
   {
      const float ang = 2.0*M_PI*float(iv)/float(nladospol-1) ,
                  x   = r1+r2*std::cos( ang ),
                  y   = r2*std::sin( ang );
      polig.push_back( { x, y, 0.0 } );
   }
   inicializar( polig, ncopias );
}

// *****************************************************************************

ConoRevol::ConoRevol()
{
   std::vector<glm::vec3> perfil ;
   constexpr int nv = 16 ;
   constexpr float h = 0.999 ;

   ponerNombre("Cono generado por revolución");

   for( int i = 0 ; i < nv ; i++ )
   {
      const float frac_h = h*float(i)/float(nv-1) ;
      perfil.push_back( glm::vec3{ float(1.0)-frac_h, frac_h, float(0.0) } );
   }
   inicializar( perfil, nv*3 );
}
