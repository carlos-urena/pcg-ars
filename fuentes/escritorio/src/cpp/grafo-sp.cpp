// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Implementación de la clase 'GrafoSupPar'
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Clase con un nodo de grafo de escena con varias mallas 
// ** (superficies paramétricas) con texturas.
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


#include "malla-sp.h"
#include "grafo-sp.h"

// *****************************************************************************


GrafoSupPar::GrafoSupPar( ) 
{
   using namespace glm ;
   using namespace std ;

   ponerNombre("Varias superficies paramétricas con texturas");

   constexpr unsigned ns = 64, nt = 64 ;
   constexpr vec3     d = { 0.0, 0.0, 2.0f } ;
   constexpr float    s = 0.3f ;

   auto * tx = new Textura( "uv-checker-2-br.jpg" ) ;
   auto * n1 = new NodoGrafoEscena();
   auto * n2 = new NodoGrafoEscena();
   auto * n3 = new NodoGrafoEscena();
   auto * n4 = new NodoGrafoEscena();

   agregar( new Material( tx, 0.1, 1.0, 1.5, 40.0 ) );

   n1->agregar( translate( vec3{ 0.0f, 1.0f, 0.0f } ));
   n1->agregar( new MallaSPEsfera( ns, nt ) );

   agregar( n1 );
   
   n2->agregar( scale( vec3( 0.7, 2.0, 0.7 ) )) ;
   n2->agregar( new MallaSPCilindro( ns, nt ) );

   agregar( translate( d ));
   agregar( n2 ); 

   n3->agregar( scale( vec3( 1.0f, 2.0f, 1.0f ) )) ;
   n3->agregar( new MallaSPCono( ns, nt ) );

   agregar( translate( d ));
   agregar( n3 );


   n4->agregar( scale( vec3( s, s, s ) )) ;
   n4->agregar( translate( vec3{ 0.0f, 5.0f, 0.0f } ));
   n4->agregar( new MallaSPColumna( 3*ns, 3*nt ) );
   
   agregar( translate( d ));
   agregar( n4 );
}