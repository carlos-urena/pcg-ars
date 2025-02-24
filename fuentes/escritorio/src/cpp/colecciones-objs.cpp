// *********************************************************************
// **
// ** Máster en Desarrollo de Software (PCG+ARS)
// ** 
// ** Gestión de escenas (implementaciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de la clase 'ColeccionObjs' y derivadas 
// **
// **  + ColeccionObjs: clase con una colección de objetos para visualizar, las fuentes de luz
// **    y una cámara. En cada momento se visualiza uno de los objetos (se conmuta con 'o')
// **    Las clases derivadas incluyen cada una un constructor específico que crea una 
// **    colección distinta con diversos objetos, son estas:
// **
// **       + ColeccionObjs3D_1 : malla indexadas sencillas
// **       + ColeccionObjs3D_2 : malla indexadas generadas algoritmicamente
// **       + ColeccionObjs3D_3 : grafos de escena
// **       + ColeccionObjs3D_4 : grafos de escena con materiales y texturas
// **       + ColeccionObjs3D_5 : grafos de escena con materiales, texturas e identificadores de selección.
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

#include "objeto-visu.h"

#include "camara.h"
#include "materiales-luces.h"
#include "colecciones-objs.h"
#include "androide.h"
#include "latapeones.h" // para usar la colecciones de fuentes
#include "malla-ind.h"
#include "malla-revol.h"
#include "malla-sp.h"
#include "grafo-sp.h"
#include "objetos-2d.h"

// -----------------------------------------------------------------------------------------------

ColeccionObjs::ColeccionObjs()
{
   
}

// -----------------------------------------------------------------------------------------------

ColeccionObjs::~ColeccionObjs()
{
   for( ObjetoVisu * obj : objetos )
   {
      assert( obj != nullptr );
      obj->pendienteDestruccion();
   }

   objetos.clear();  // solamente elimina el array, no destruye los objetos
}

// -----------------------------------------------------------------------------------------------
/// @brief Devuelve el nombre de la colección de objetos
///
std::string ColeccionObjs::nombre() 
{
   return nombreColeccion ;
}
// -----------------------------------------------------------------------------------------------

void ColeccionObjs::siguienteObjeto()
{
   assert( 0 < objetos.size() );
   assert( ind_objeto_actual < objetos.size() );
   ind_objeto_actual = (ind_objeto_actual+1 ) % objetos.size();
   imprimeInfoObjetoActual() ;
}
// -----------------------------------------------------------------------------------------------

void ColeccionObjs::imprimeInfoObjetoActual() 
{
   using namespace std ;
   cout << "Objeto actual " << (ind_objeto_actual+1) << "/" << objetos.size() << " : " 
        << objetoActual()->leerNombre() << endl  ;
}
// -----------------------------------------------------------------------------------------------

/// @brief devuelve puntero al objeto actual
///
ObjetoVisu * ColeccionObjs::objetoActual()
{
   assert( 0 < objetos.size() );
   assert( ind_objeto_actual < objetos.size() );
   assert( objetos[ind_objeto_actual] != nullptr );
   return objetos[ind_objeto_actual] ;
}
// -----------------------------------------------------------------------------------------------

/// @brief Objetos a la coleccion de objetos 1
/// @brief (Mallas indexadas sencillas)
///
ColeccionObjs3D_1::ColeccionObjs3D_1()
{
   using namespace std ;
   nombreColeccion = "Mallas indexadas sencillas" ;
   cout << "Creando objetos de la colección 1: " << nombre() << "." << endl ;

   objetos.push_back( new CuboNorCol() );
   objetos.push_back( new CuboNor() );
   objetos.push_back( new Cubo() );
   objetos.push_back( new Cilindro(32,16) );
   objetos.push_back( new Esfera() );
   objetos.push_back( new EsferaBajaRes() );
   objetos.push_back( new Semiesfera() );
   objetos.push_back( new Cono() );
   objetos.push_back( new ConoTruncado() );
   objetos.push_back( new Tetraedro() );
   objetos.push_back( new Piramide() );
}
// -------------------------------------------------------------------------

/// @brief Objetos a la colección de objetos 2. 
/// @brief Mallas indexadas obtenidas por revolución, o generadas algoritmicamente en general 
///
ColeccionObjs3D_2::ColeccionObjs3D_2()
{
   using namespace std ;
   nombreColeccion = "Mallas indexadas generadas proceduralmente o de archivos PLY" ;
   cout << "Creando objetos de la colección 2: " << nombre() << "." << endl ;

   objetos.push_back( new ConoRevol() );
   objetos.push_back( new MallaPLY( "beethoven.ply") );
   objetos.push_back( new MallaPLY( "big_dodge.ply") );
   objetos.push_back( new MallaRevolPLY( "peon.ply", 17 ) );
   objetos.push_back( new DonutRevol( 1.2, 0.3, 32, 32 ) );
   objetos.push_back( new DonutRevol( 1.2, 0.3, 64, 32 ) );
   objetos.push_back( new DonutRevol( 1.2, 0.3, 256, 32 ) );
}

// -------------------------------------------------------------------------

/// @brief Objetos a la colección de objetos 3. 
/// @brief Mallas indexadas que aproximan Superficies paramétricas.
///
ColeccionObjs3D_3::ColeccionObjs3D_3()
{
   using namespace std ;
   nombreColeccion = "Mallas indexadas que aproximan superficies paramétricas." ;
   cout << "Creando objetos de la colección 3: " << nombre() << "." << endl ;

   constexpr unsigned ns = 32, nt = 32 ;

   objetos.push_back( new MallaSPEsfera( ns, nt ) );
   objetos.push_back( new MallaSPCilindro( ns, nt ) );
   objetos.push_back( new MallaSPCono( ns, nt ) );
   objetos.push_back( new MallaSPColumna( 3*ns, 3*nt ) );
   
}

// -------------------------------------------------------------------------

/// @brief Objetos a la colección de objetos 4.
/// @brief (grafos de escena)
///
ColeccionObjs3D_4::ColeccionObjs3D_4()
{
   using namespace std ;
   nombreColeccion = "Objetos modelados como grafos de escena" ;
   cout << "Creando objetos de la colección 3: " << nombre() << "." << endl ;

   objetos.push_back( new Cuadroide() );
   objetos.push_back( new FormacionDroides(20,20));
}
// -------------------------------------------------------------------------

/// @brief Objetos a la colección de objetos 5.
/// @brief (grafos de escena con materiales y texturas)
///
ColeccionObjs3D_5::ColeccionObjs3D_5()
{
   using namespace std ;
   nombreColeccion = "Objetos con materiales y texturas" ;
   cout << "Creando objetos de la colección 5: " << nombre() << "." << endl ;

   objetos.push_back( new LataPeones() );
   objetos.push_back( new NodoCubo24() ); // cubo con 24 vértices
   objetos.push_back( new GrafoSupPar() );
}
// -------------------------------------------------------------------------

/// @brief Objetos a la colección de objetos 6.
/// @brief (grafos de escena con identificadores de selección)
///
ColeccionObjs3D_6::ColeccionObjs3D_6()
{
   using namespace std ;
   nombreColeccion = "Objetos con identificadores de selección" ;
   cout << "Creando objetos de la colección 5: " << nombre() << "." << endl ;

   constexpr unsigned int ident_dado = 345 ;
   objetos.push_back( new VariasLatasPeones( ident_dado ) );
}

// -------------------------------------------------------------------------

ColeccionObjs2D:: ColeccionObjs2D() 
{
   objetos.push_back( new Estrella( 9, 0.5f,0.7f ) );
   
   auto * estrella2 = new Estrella( 32, 0.6f,0.7f, new Textura( "text-madera.jpg" ) ) ;
   estrella2->fijarAnchoLineasWCC( 0.02f );
   estrella2->fijarRadioPuntosWCC( 0.01f );
   estrella2->fijarColorLineasContorno({ 0.2, 0.3, 0.0}) ;
   objetos.push_back( estrella2 );

   objetos.push_back( new PoligonosDeTXT( "provincias-espana-v2.txt" ) );
}
