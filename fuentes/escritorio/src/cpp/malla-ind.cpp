// *********************************************************************
// **
// ** Asignatura: Programación del Cauce Gráfico (MDS)
// ** 
// ** Mallas indexadas (implementación)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de las clases 
// **        + MallaInd: malla indexada de triángulos (derivada de ObjetoVisu)
// **        + MallaPLY: malla indexada de triángulos, leída de un PLY (derivada de MallaInd)
// **        + algunas clases derivadas de MallaInd.
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
#include "aplic-3d.h"
#include "malla-ind.h"   // declaración de 'ContextoVis'
#include "lector-ply.h"
#include "seleccion.h"   // para 'ColorDesdeIdent' 

// *****************************************************************************
// funciones auxiliares

// *****************************************************************************
// métodos de la clase MallaInd.

MallaInd::MallaInd()
{
   // nombre por defecto
   ponerNombre("malla indexada, anónima");
}
// -----------------------------------------------------------------------------

MallaInd::MallaInd( const std::string & nombreIni )
{
   // 'identificador' puesto a 0 por defecto, 'centro_oc' puesto a (0,0,0)
   ponerNombre(nombreIni) ;
}
// -----------------------------------------------------------------------------

MallaInd::~MallaInd()
{
   using namespace std ;
   //cout << "Invocado desructor de MallaInd (borra tablas) '" << leerNombre() << "'" << endl ;

   vertices.clear();
   triangulos.clear();
   col_ver.clear();
   nor_ver.clear();
   nor_tri.clear();
   cc_tt_ver.clear();
   segmentos_normales.clear();
   
   delete dvao ;
   delete dvao_normales ;
}

//-----------------------------------------------------------------------------
// calcula la tabla de normales de triángulos una sola vez, si no estaba calculada

void MallaInd::calcularNormalesTriangulos()
{

   
   // si ya está creada la tabla de normales de triángulos, no es necesario volver a crearla
   const unsigned nt = triangulos.size() ;
   assert( 1 <= nt );
   if ( 0 < nor_tri.size() )
   {
      assert( nt == nor_tri.size() );
      return ;
   }

   // Creación de la tabla de normales de triángulos
   nor_tri.resize( nt ) ;
   unsigned long nt_mal = 0 ;

   for( unsigned it = 0 ; it < nt ; it++ )
   {
      const glm::vec3
         & v0 = vertices[triangulos[it][0]],
         & v1 = vertices[triangulos[it][1]],
         & v2 = vertices[triangulos[it][2]],
         e1   = v1-v0,
         e2   = v2-v0,
         n    = cross(e1,e2) ;
      const float ln = length( n ) ; //sqrt(n.lengthSq());
      if  ( ln > 1e-8 )
         nor_tri[it] = n/ln ;
      else
      {
         nor_tri[it] = glm::vec3(0.0,0.0,0.0);
         nt_mal++ ;
      }
   }
}


// -----------------------------------------------------------------------------
// calcula las dos tablas de normales

void MallaInd::calcularNormales()
{
   using namespace glm ;
   
   // Calculo de las normales de la malla
   // se debe invocar en primer lugar 'calcularNormalesTriangulos'
   
   if ( nor_tri.size() == 0 )
      calcularNormalesTriangulos();

   assert( nor_tri.size() == triangulos.size() );

   if ( nor_ver.size() > 0 )
   {         
      assert( nor_ver.size() == vertices.size() );
      return ;
   }
   const unsigned nv = vertices.size(),
                  nt = triangulos.size() ;

   assert( 2 <= nv );
   assert( 1 <= nt );

   // calcular normales de vértices:
   assert( nor_ver.size() == 0 );
   nor_ver.resize( nv );

   for( unsigned iv = 0 ; iv < nv ; iv++ )
      nor_ver[iv] = glm::vec3(0.0,0.0,0.0);

   for( unsigned it = 0 ; it < nt ; it++ )
   {
      vec3
         & nt  = nor_tri[it],
         & nv0 = nor_ver[triangulos[it][0]],
         & nv1 = nor_ver[triangulos[it][1]],
         & nv2 = nor_ver[triangulos[it][2]] ;

      nv0 = nv0+nt ;
      nv1 = nv1+nt ;
      nv2 = nv2+nt ;
   }

   for( unsigned iv = 0 ; iv < nv ; iv++ )
   {
      const float ln = length( nor_ver[iv] );
      if ( ln > 1e-5 )
         nor_ver[iv] = nor_ver[iv]/ln ;
      else
         nor_ver[iv] = glm::vec3(0.0,1.0,0.0);
   }
}


// --------------------------------------------------------------------------------------------

void MallaInd::visualizarGL( )
{
   using namespace std ;
   
   Cauce3D * cauce = Aplicacion3D::instancia()->cauce3D() ;
   
   CError();

   // si la malla no vértices o no tiene triángulos, imprimir advertencia y salir.
   if ( triangulos.size() == 0 || vertices.size() == 0 )
   {  cout << "advertencia: intentando dibujar malla vacía '" << leerNombre() << "'" << endl << flush ;
      return ;
   }

   // Cambiar color del cauce
   //
   // Si el objeto tiene un color asignado (se comprueba con 'tieneColor')
   //    - hacer push del color actual del cauce
   //    - fijar el color en el cauce usando el color del objeto (se lee con 'leerColor()')
   if ( tieneColor() )
   {
      cauce->pushColor();
      cauce->fijarColor( leerColor() );
   }
   
   // Crear el descriptor de VAO, si no está creado
   //  Si el puntero 'dvao' es nulo, crear el descriptor de VAO (se usan las tablas de vértices, triángulos y atributos de la malla)
   //  Si el VAO ya está creado, (dvao no nulo), no hay que hacer nada.
   
   if ( dvao == nullptr ) 
      dvao = new DescrVAO({ .posiciones_3d = vertices, 
                            .colores       = col_ver, 
                            .normales      = nor_ver,  
                            .coord_text    = cc_tt_ver, 
                            .triangulos    = triangulos });
   
   CError();
   
   // Visualizar el VAO usando el método 'draw' de 'DescrVAO'
   dvao->draw( GL_TRIANGLES );

   // Restaurar color anterior del cauce:
   // Si el objeto tiene un color asignado (se comprueba con 'tieneColor')
   //    - hacer 'pop' del color actual del cauce
   if ( tieneColor() )
      cauce->popColor();
}


// -----------------------------------------------------------------------------
// Visualizar el objeto con OpenGL
// usa las tablas de normales, colores y coordenadas de textura, si no están vacías.
      
void MallaInd::visualizarGeomGL( )
{
   // Comprobar que el descriptor de VAO ya está creado
   // (es decir, este método únicamente se podrá invocar después de que 
   // se haya llamado a 'visualizaGL')
   
   assert( dvao != nullptr );

   // Visualizar únicamente la geometría del objeto 
   // 
   //    1. Desactivar todas las tablas de atributos del VAO (que no estén vacías)
   //    2. Dibujar la malla (únicamente visualizará los triángulos), se usa el método 'draw' del VAO (dvao)
   //    3. Volver a activar todos los atributos para los cuales la tabla no esté vacía
   
   if ( 0 < col_ver.size() )   dvao->habilitarAtrib( ind_atrib_colores,    false );
   if ( 0 < nor_ver.size() )   dvao->habilitarAtrib( ind_atrib_normales,   false );
   if ( 0 < cc_tt_ver.size() ) dvao->habilitarAtrib( ind_atrib_coord_text, false );

   dvao->draw( GL_TRIANGLES );

   if ( 0 < col_ver.size() )   dvao->habilitarAtrib( ind_atrib_colores,    true );
   if ( 0 < nor_ver.size() )   dvao->habilitarAtrib( ind_atrib_normales,   true );
   if ( 0 < cc_tt_ver.size() ) dvao->habilitarAtrib( ind_atrib_coord_text, true );
   
}

// -----------------------------------------------------------------------------
// Visualizar las normales del objeto, si no tiene tabla de normales imprime 
// advertencia y no hace nada.

void MallaInd::visualizarNormalesGL(  )
{
   using namespace std ;
   //Aplicacion3D * apl = Aplicacion3D::instancia() ;
   //Cauce3D * cauce = apl->cauce3D() ;

   if ( nor_ver.size() == 0 )
   {
      cout << "Advertencia: intentando dibujar normales de una malla que no tiene tabla (" << leerNombre() << ")." << endl ;
      return ;
   }  

   if( nor_ver.size() != vertices.size() )
   {
      cout << "Error visu. normales: tabla de normales no vacía y de tamaño distinto a la de vértices." << endl ;
      cout << "Nombre del objeto        : " << leerNombre() << endl ;
      cout << "Tamaño tabla vértices    : " << vertices.size() << endl ;
      cout << "Tamaño tabla de normales : " << nor_ver.size() << endl ;
      exit(1);
   }
   CError();

   // Visualizar las normales del objeto MallaInd
   // 
   // *1* Si el puntero al descriptor de VAO de normales ('dvao_normales') es nulo, 
   //    debemos de crear dicho descriptor, con estos pasos:
   //
   //       * Para cada posición 'v_i' de un vértice en el vector 'vertices':
   //             - Leer la correspondiente normal 'n_i' del vector de normales ('nor_ver').
   //             - Añadir 'v_i' al vector 'segmentos_normales'.
   //             - Añadir 'v_i+a*n_i' al vector 'segmentos_normales'.
   //
   //       * Crear el objeto descriptor del VAO de normales, para ello se usa el vector 
   //          'segmentos_normales' y se tiene en cuenta que esa descriptor únicamente gestiona 
   //          una tabla de atributos de vértices (la de posiciones, ya que las otras no se 
   //          necesitan).
   // 
   // *2* Visualizar el VAO de normales, usando el método 'draw' del descriptor, con el 
   //       tipo de primitiva 'GL_LINES'.
   
   if ( dvao_normales == nullptr )
   {  
      assert( segmentos_normales.size() == 0 );

      for( unsigned i = 0 ; i < vertices.size() ; i++ )
      {  
         segmentos_normales.push_back( vertices[i] );
         segmentos_normales.push_back( vertices[i]+ 0.35f*(nor_ver[i]) );
      }
      constexpr unsigned num_atribs = 1 ;
      dvao_normales = new DescrVAO( num_atribs, new DescrVBOAtribs( ind_atrib_posiciones, segmentos_normales )); 
      
      assert( dvao_normales != nullptr );
   }
   
   dvao_normales->draw( GL_LINES );
}

// -----------------------------------------------------------------------------
// visualizar el objeto en 'modo seleccion', es decir, sin iluminación y con los colores 
// basados en los identificadores de los objetos
void MallaInd::visualizarModoSeleccionGL() 
{

   using namespace std ;
   Aplicacion3D * apl = Aplicacion3D::instancia() ;
   Cauce3D * cauce = apl->cauce3D() ;

   // Visualizar la malla en modo selección 
   //
   // Se debe escribir código para visualizar únicamente la geometría, pero usando el color 
   // obtenido a partir del identificador. El código da estos pasos:
   // 
   // 1. Leer el identificador del objeto (con 'leerIdentificador'). Si el objeto tiene 
   //    identificador (es decir, si su identificador no es -1)
   //       + Hacer push del color del cauce, con 'pushColor'.
   //       + Fijar el color del cauce (con 'fijarColor') usando un color obtenido a 
   //         partir del identificador (con 'ColorDesdeIdent'). 
   // 2. Invocar 'visualizarGeomGL' para visualizar la geometría.
   // 3. Si tiene identificador: hacer pop del color, con 'popColor'.
   
   const int ident_nodo = leerIdentificador();
   if ( ident_nodo != -1 )
   {
      cauce->pushColor();
      cauce->fijarColor( ColorDesdeIdent( ident_nodo ) );
   }
   visualizarGeomGL();

   if ( ident_nodo != -1 )
      cauce->popColor();
}


// ****************************************************************************
// Clase 'MallaPLY'

MallaPLY::MallaPLY( const std::string & nombre_arch )
{
   ponerNombre( std::string("Malla en archivo PLY (") + nombre_arch + ")" );
   LeerPLY( nombre_arch, vertices, triangulos );
   calcularNormales(); // calcular la tabla de normales
}

// ****************************************************************************
// Clase 'Cubo

Cubo::Cubo()
:  MallaInd( "Cubo de 8 vértices" )
{

   vertices =
      {  { -1.0, -1.0, -1.0 }, // 0
         { -1.0, -1.0, +1.0 }, // 1
         { -1.0, +1.0, -1.0 }, // 2
         { -1.0, +1.0, +1.0 }, // 3
         { +1.0, -1.0, -1.0 }, // 4
         { +1.0, -1.0, +1.0 }, // 5
         { +1.0, +1.0, -1.0 }, // 6
         { +1.0, +1.0, +1.0 }, // 7
      } ;

   
   for( glm::vec3 v : vertices )
      nor_ver.push_back( normalize(v) );
   

   triangulos =
      {  {0,1,3}, {0,3,2}, // X-
         {4,7,5}, {4,6,7}, // X+ (+4)

         {0,5,1}, {0,4,5}, // Y-
         {2,3,7}, {2,7,6}, // Y+ (+2)

         {0,6,4}, {0,2,6}, // Z-
         {1,5,7}, {1,7,3}  // Z+ (+1)
      } ;

}
// -----------------------------------------------------------------------------------------------


CuboTejado::CuboTejado()
:  MallaInd( "Cubo tejado" )
{

   vertices =
      {  { -1.0, -1.0, -1.0 }, // 0
         { -1.0, -1.0, +1.0 }, // 1
         { -1.0, +1.0, -1.0 }, // 2
         { -1.0, +1.0, +1.0 }, // 3
         { +1.0, -1.0, -1.0 }, // 4
         { +1.0, -1.0, +1.0 }, // 5
         { +1.0, +1.0, -1.0 }, // 6
         { +1.0, +1.0, +1.0 }, // 7
         { +0.0, +2.0, +0.0 }, // 8
      } ;

   for( glm::vec3 & v : vertices )
      v = 2.0f*v +glm::vec3({0.0,2.0,0.0});

   
   for( glm::vec3 v : vertices )
      nor_ver.push_back( normalize(v) );
   

   triangulos =
      {  {0,1,3}, {0,3,2}, // X-
         {4,7,5}, {4,6,7}, // X+ (+4)

         {0,5,1}, {0,4,5}, // Y-
         //{2,3,7}, {2,7,6}, // Y+ (+2)   // quito cara superior
         {2,3,8}, {3,7,8}, {7,6,8}, {6,2,8}, // añado tejado 

         {0,6,4}, {0,2,6}, // Z-
         {1,5,7}, {1,7,3}  // Z+ (+1)
      } ;

}
// -----------------------------------------------------------------------------------------------


CuboNor::CuboNor()
{

   ponerNombre("Cubo 24 vértices");


   // crear el objeto:
   vertices =
      {
         // Cara X-
         { -1.0, -1.0, -1.0 },  // 0
         { -1.0, -1.0, +1.0 },  // 1
         { -1.0, +1.0, -1.0 },  // 2
         { -1.0, +1.0, +1.0 },  // 3

         // Cara X+
         { +1.0, -1.0, -1.0 },  // 4
         { +1.0, -1.0, +1.0 },  // 5
         { +1.0, +1.0, -1.0 },  // 6
         { +1.0, +1.0, +1.0 },  // 7

         // Cara Y-
         { -1.0, -1.0, -1.0 },  // 8
         { -1.0, -1.0, +1.0 },  // 9
         { +1.0, -1.0, -1.0 },  // 10
         { +1.0, -1.0, +1.0 },  // 11

         // Cara Y+
         { -1.0, +1.0, -1.0 },  // 12
         { -1.0, +1.0, +1.0 },  // 13
         { +1.0, +1.0, -1.0 },  // 14
         { +1.0, +1.0, +1.0 },  // 15


         // Cara Z-
         { -1.0, -1.0, -1.0 },  // 16
         { -1.0, +1.0, -1.0 },  // 17
         { +1.0, -1.0, -1.0 },  // 18
         { +1.0, +1.0, -1.0 },  // 19

         // Cara Z+
         { -1.0, -1.0, +1.0 },  // 20
         { -1.0, +1.0, +1.0 },  // 21
         { +1.0, -1.0, +1.0 },  // 22
         { +1.0, +1.0, +1.0 },  // 23

      } ;

   triangulos =
      {
         // Cara X-
         { 1, 3, 2 },
         { 0, 1, 2 },

         // Cara X+
         { 4, 6, 5 },
         { 5, 6, 7 },

         // Cara Y-
         { 8, 10,  9 },
         { 9, 10, 11 },

         // Cara Y+
         { 13, 15, 14 },
         { 12, 13, 14 },

         // Cara Z-
         { 17, 19, 18 },
         { 16, 17, 18 },

         // Cara Z+
         { 20, 22, 21 },
         { 21, 22, 23 }

      } ;

   nor_ver =
      {
         // Cara X-
         { -1.0,  0.0, 0.0 },
         { -1.0,  0.0, 0.0 },
         { -1.0,  0.0, 0.0 },
         { -1.0,  0.0, 0.0 },

         // Cara X+
         { +1.0,  0.0, 0.0 },
         { +1.0,  0.0, 0.0 },
         { +1.0,  0.0, 0.0 },
         { +1.0,  0.0, 0.0 },

         // Cara Y-
         { 0.0, -1.0, 0.0 },
         { 0.0, -1.0, 0.0 },
         { 0.0, -1.0, 0.0 },
         { 0.0, -1.0, 0.0 },

         // Cara Y+
         { 0.0, +1.0, 0.0 },
         { 0.0, +1.0, 0.0 },
         { 0.0, +1.0, 0.0 },
         { 0.0, +1.0, 0.0 },

         // Cara Z-
         { 0.0, 0.0, -1.0 },
         { 0.0, 0.0, -1.0 },
         { 0.0, 0.0, -1.0 },
         { 0.0, 0.0, -1.0 },

         // Cara Z+
         { 0.0, 0.0, +1.0 },
         { 0.0, 0.0, +1.0 },
         { 0.0, 0.0, +1.0 },
         { 0.0, 0.0, +1.0 }

      } ;

   cc_tt_ver =
      {
         // Cara X- (1)
         {     0.0,         0.0 },
         {     0.0,     1.0/3.0 },
         { 1.0/2.0,         0.0 },
         { 1.0/2.0,     1.0/3.0 },

         // Cara X+ (6)
         { 1.0/2.0, 2.0/3.0 },
         { 1.0/2.0,     1.0 },
         { 1.0,     2.0/3.0 },
         { 1.0,         1.0 },

         // Cara Y- (2)
         { 1.0/2.0,         0.0 },
         { 1.0/2.0,     1.0/3.0 },
         {     1.0,         0.0 },
         {     1.0,     1.0/3.0 },

         // Cara Y+ (5)
         { 0.0,     2.0/3.0 },
         { 0.0,         1.0 },
         { 1.0/2.0, 2.0/3.0 },
         { 1.0/2.0,     1.0 },

         // Cara Z- (3)
         { 0.0,     1.0/3.0 },
         { 0.0,     2.0/3.0 },
         { 1.0/2.0, 1.0/3.0 },
         { 1.0/2.0, 2.0/3.0 },

         // Cara Z+  (4)
         { 1.0/2.0, 1.0/3.0 },
         { 1.0/2.0, 2.0/3.0 },
         { 1.0,     1.0/3.0 },
         { 1.0,     2.0/3.0 }

      } ;


   nor_tri =
      {
         // Cara X-
         { -1.0,  0.0, 0.0 },
         { -1.0,  0.0, 0.0 },

         // Cara X+
         { +1.0,  0.0, 0.0 },
         { +1.0,  0.0, 0.0 },

         // Cara Y-
         { 0.0, -1.0, 0.0 },
         { 0.0, -1.0, 0.0 },

         // Cara Y+
         { 0.0, +1.0, 0.0 },
         { 0.0, +1.0, 0.0 },

         // Cara Z-
         { 0.0, 0.0, -1.0 },
         { 0.0, 0.0, -1.0 },

         // Cara Z+
            
         { 0.0, 0.0, +1.0 },
            
         { 0.0, 0.0, +1.0 }

      } ;

   //  using namespace std ;
   //cout << "construido objeto cubo ef19" << endl ;
}
// *****************************************************************************

CuboNorCol::CuboNorCol() : CuboNor()
{
   ponerNombre("Cubo 24 vértices + colores");

   auto col = [] ( const float p ) { return 0.0 < p ? 1.0 : 0.0 ; } ;

   for( unsigned iv = 0 ; iv < vertices.size() ; iv++ )
   {
      const glm::vec3 & v = vertices[iv] ;
      col_ver.push_back( glm::vec3( col(v.x), col(v.y), col(v.z) ) ) ;
   }
}
// *****************************************************************************

// Cubo de lado 2 con centro en el origen, 6x4=24 vértices
// Cubo con normales y coordenadas de textura

Cubo24::Cubo24()
{
   ponerNombre("Cubo 24 vértices");
   //ponerColor( { 1.0, 1.0, 1.0 } );

   // crear el objeto:
   vertices =
      {
         // Cara X-
         { -1.0, -1.0, -1.0 },  // 0
         { -1.0, -1.0, +1.0 },  // 1
         { -1.0, +1.0, -1.0 },  // 2
         { -1.0, +1.0, +1.0 },  // 3

         // Cara X+
         { +1.0, -1.0, -1.0 },  // 4
         { +1.0, -1.0, +1.0 },  // 5
         { +1.0, +1.0, -1.0 },  // 6
         { +1.0, +1.0, +1.0 },  // 7

         // Cara Y-
         { -1.0, -1.0, -1.0 },  // 8
         { -1.0, -1.0, +1.0 },  // 9
         { +1.0, -1.0, -1.0 },  // 10
         { +1.0, -1.0, +1.0 },  // 11

         // Cara Y+
         { -1.0, +1.0, -1.0 },  // 12
         { -1.0, +1.0, +1.0 },  // 13
         { +1.0, +1.0, -1.0 },  // 14
         { +1.0, +1.0, +1.0 },  // 15


         // Cara Z-
         { -1.0, -1.0, -1.0 },  // 16
         { -1.0, +1.0, -1.0 },  // 17
         { +1.0, -1.0, -1.0 },  // 18
         { +1.0, +1.0, -1.0 },  // 19

         // Cara Z+
         { -1.0, -1.0, +1.0 },  // 20
         { -1.0, +1.0, +1.0 },  // 21
         { +1.0, -1.0, +1.0 },  // 22
         { +1.0, +1.0, +1.0 },  // 23

      } ;

   triangulos =
      {
         // Cara X-
         { 1, 3, 2 },
         { 0, 1, 2 },

         // Cara X+
         { 4, 6, 5 },
         { 5, 6, 7 },

         // Cara Y-
         { 8, 10,  9 },
         { 9, 10, 11 },

         // Cara Y+
         { 13, 15, 14 },
         { 12, 13, 14 },

         // Cara Z-
         { 17, 19, 18 },
         { 16, 17, 18 },

         // Cara Z+
         { 20, 22, 21 },
         { 21, 22, 23 }

      } ;

   nor_ver =
      {
         // Cara X-
         { -1.0,  0.0, 0.0 },
         { -1.0,  0.0, 0.0 },
         { -1.0,  0.0, 0.0 },
         { -1.0,  0.0, 0.0 },

         // Cara X+
         { +1.0,  0.0, 0.0 },
         { +1.0,  0.0, 0.0 },
         { +1.0,  0.0, 0.0 },
         { +1.0,  0.0, 0.0 },

         // Cara Y-
         { 0.0, -1.0, 0.0 },
         { 0.0, -1.0, 0.0 },
         { 0.0, -1.0, 0.0 },
         { 0.0, -1.0, 0.0 },

         // Cara Y+
         { 0.0, +1.0, 0.0 },
         { 0.0, +1.0, 0.0 },
         { 0.0, +1.0, 0.0 },
         { 0.0, +1.0, 0.0 },

         // Cara Z-
         { 0.0, 0.0, -1.0 },
         { 0.0, 0.0, -1.0 },
         { 0.0, 0.0, -1.0 },
         { 0.0, 0.0, -1.0 },

         // Cara Z+
         { 0.0, 0.0, +1.0 },
         { 0.0, 0.0, +1.0 },
         { 0.0, 0.0, +1.0 },
         { 0.0, 0.0, +1.0 }

      } ;

   cc_tt_ver =
      {
         // Cara X- (1)
         { 0.0, 1.0 },
         { 1.0, 1.0 },
         { 0.0, 0.0 },
         { 1.0, 0.0 },

         // Cara X+ (6)
         { 1.0, 1.0 },
         { 0.0, 1.0 },
         { 1.0, 0.0 },
         { 0.0, 0.0 },

         // Cara Y- (2)
         { 0.0, 1.0 },
         { 0.0, 0.0 },
         { 1.0, 1.0 },
         { 1.0, 0.0 },

         // Cara Y+ (5)
         { 0.0, 0.0 },
         { 0.0, 1.0 },
         { 1.0, 0.0 },
         { 1.0, 1.0 },

         // Cara Z- (3)
         { 1.0, 1.0 },
         { 1.0, 0.0 },
         { 0.0, 1.0 },
         { 0.0, 0.0 },

         // Cara Z+  (4)
         { 0.0, 1.0 },
         { 0.0, 0.0 },
         { 1.0, 1.0 },
         { 1.0, 0.0 }

      } ;

   //
   // nor_tri =
   //    {
   //       // Cara X-
   //       { -1.0,  0.0, 0.0 },
   //       { -1.0,  0.0, 0.0 },
   //
   //       // Cara X+
   //       { +1.0,  0.0, 0.0 },
   //       { +1.0,  0.0, 0.0 },
   //
   //       // Cara Y-
   //       { 0.0, -1.0, 0.0 },
   //       { 0.0, -1.0, 0.0 },
   //
   //       // Cara Y+
   //       { 0.0, +1.0, 0.0 },
   //       { 0.0, +1.0, 0.0 },
   //
   //       // Cara Z-
   //       { 0.0, 0.0, -1.0 },
   //       { 0.0, 0.0, -1.0 },
   //
   //       // Cara Z+
   //       { 0.0, 0.0, +1.0 },
   //       { 0.0, 0.0, +1.0 }
   //
   //    } ;
   //    using namespace std ;
      //cout << "construido objeto cubo ef19" << endl ;
}


// *****************************************************************************

Tetraedro::Tetraedro()
:  MallaInd( "Tetraedro")
{

   // tetraedro con centro de la base en el origen
   const float
      l = 2.0,   // longitud de las aristas
      k = 0.5/sqrt(2.0);

   vertices =
      {  { +0.5*l,  0.0*l, -k*l  },
         { -0.5*l,  0.0*l, -k*l  },
         {  0.0*l, +0.5*l, +k*l  },
         {  0.0*l, -0.5*l, +k*l  }
      } ;

   triangulos =
      {
         { 0, 1, 2 },
         { 0, 2, 3 },
         { 0, 3, 1 },
         { 1, 2, 3 }
      } ;

      calcularNormales() ; // en la práctica 4, añadir cálculo de normales

}
// *****************************************************************************


Piramide::Piramide()
:  MallaInd( "Pirámide")
{

   vertices =
      {  { -0.5, 0.0, -0.5f  },  // 0: base 0 (x-,z-)
         { -0.5, 0.0, +0.5f  },  // 1: base 1 (x-,z+)
         { +0.5, 0.0, -0.5f  },  // 2: base 2 (x+,z-)
         { +0.5, 0.0, +0.5f  },  // 3: base 3 (x+,z+)

         { -0.5, 0.0, -0.5f  },  // 4: cara x-, 1o
         { -0.5, 0.0, +0.5  },  // 5: cara x-, 2o
         {  0.0, 1.0,  0.0  },  // 6: cara x-, 3o  (vértice superior)

         { +0.5, 0.0, +0.5  },  // 7: cara x+, 1o
         { +0.5, 0.0, -0.5  },  // 8: cara x+, 2o
         {  0.0, 1.0,  0.0  },  // 9: cara x+, 3o (vértice superior)

         { +0.5, 0.0, -0.5  },  // 10: cara z-, 1o
         { -0.5, 0.0, -0.5  },  // 11: cara z-, 2o
         {  0.0, 1.0,  0.0  },  // 12: cara z-, 3o  (vértice superior)

         { -0.5, 0.0, +0.5  },  // 13: cara z+, 1o
         { +0.5, 0.0, +0.5  },  // 14: cara z+, 2o
         {  0.0, 1.0,  0.0  }   // 15: cara z+, 3o  (vértice superior)
      } ;

   nor_ver =
      {
         { 0.0, -1.0, 0.0 },  // base
         { 0.0, -1.0, 0.0 },  // base
         { 0.0, -1.0, 0.0 },  // base
         { 0.0, -
            1.0, 0.0 },  // base

         { -1.0, 1.0, 0.0 },  // X-
         { -1.0, 1.0, 0.0 },  // X-
         { -1.0, 1.0, 0.0 },  // X-

         { +1.0, 1.0, 0.0 },  // X+
         { +1.0, 1.0, 0.0 },  // X+
         { +1.0, 1.0, 0.0 },  // X+

         { 0.0, 1.0, -1.0 },  // Z-
         { 0.0, 1.0, -1.0 },  // Z-
         { 0.0, 1.0, -1.0 },  // Z-

         { 0.0, 1.0, +1.0 },  // Z+
         { 0.0, 1.0, +1.0 },  // Z+
         { 0.0, 1.0, +1.0 }   // Z+
      } ;

   for( glm::vec3 & nv : nor_ver )
      nv = normalize( nv );

   cc_tt_ver =
      {
         { 1.0, 1.0 },
         { 0.0, 1.0 },
         { 1.0, 0.0 },
         { 0.0, 0.0 },

         { 0.0, 1.0 },
         { 1.0, 1.0 },
         { 0.5, 0.0 },

         { 0.0, 1.0 },
         { 1.0, 1.0 },
         { 0.5, 0.0 },

         { 0.0, 1.0 },
         { 1.0, 1.0 },
         { 0.5, 0.0 },

         { 0.0, 1.0 },
         { 1.0, 1.0 },
         { 0.5, 0.0 }
      } ;

   triangulos =
      {
         { 0, 2, 3 },  // base 1
         { 0, 3, 
            1 },  // base 2

         {  4,  5,  6 }, // cara X-
         {  7,  8,  9 }, // cara X+
         { 10, 11, 12 }, // cara Z-
         { 13, 14, 15 }  // cara Z+
      } ;

   nor_tri =
      {
         { 0.0, -1.0, 0.0 },  // base 1
         { 0.0, -1.0, 0.0 },  // base 2

         { -1.0,  1.0,  0.0 }, // cara X-
         { +1.0,  1.0,  0.0 }, // cara X+
         {  0.0,  1.0, -1.0 }, // cara X-
         {  0.0,  1.0, +1.0 }, // cara X+
      } ;

   for( glm::vec3 & nt : nor_tri )
      nt = normalize( nt );
         
}
// *****************************************************************************

ConoTruncado::ConoTruncado()
{
   using namespace glm ;
   constexpr int n = 32 ;
   constexpr float r0 = 1.0, r1 = 0.15, h = 1.0 ;

   ponerNombre("Cono truncado");

   for( int i = 0 ; i <= n ; i++ )
   {
      const float frac    = float(i)/float(n),
                  ang     = 2.0*M_PI*frac,
                  cos_ang = cos(ang),
                  sin_ang = sin(ang) ;

      vertices.push_back( { r0*cos_ang, 0.0, r0*sin_ang } );
      vertices.push_back( { r1*cos_ang, h,   r1*sin_ang } );

      auto normal = normalize( vec3( h*cos_ang, r0-r1, h*sin_ang ) );
      nor_ver.push_back( normal );
      nor_ver.push_back( normal );

      cc_tt_ver.push_back( { frac, 0.0 } );
      cc_tt_ver.push_back( { frac, 1.0 } );
   }

   for( unsigned i = 0 ; i < n ; i++ )
   {
      triangulos.push_back( { 2*(i+1), 2*i,        2*i+1   } );
      triangulos.push_back( { 2*i+1,   2*(i+1)+1,  2*(i+1) } );
   }

}

// *****************************************************************************
// un cono con múltiples vértice en la punta para que las normales y las cc.tt.
// salgan correctas
   
// (altura 1, radio de la base 1, centro de la base en el origen)

Cono::Cono()
{
   using namespace glm ;
   constexpr int n = 32 ;

   ponerNombre("Cono");

   for( unsigned i = 0 ; i < n ; i++ )
   {
      const float 
         frac      = float(i)/float(n),
         ang       = 2.0f*M_PI*frac,
         cos_ang   = cos(ang),
         sin_ang   = sin(ang),
         frac_m    = (float(i)+0.5)/float(n),
         ang_m     = 2.0*M_PI*frac_m,
         cos_ang_m = cos(ang_m),
         sin_ang_m = sin(ang_m);

      vertices.push_back( { cos_ang, 0.0, sin_ang } );
      nor_ver.push_back( normalize( vec3( cos_ang, 1.0, sin_ang ) ) );
      cc_tt_ver.push_back( { frac, 0.0 } );

      vertices.push_back( { 0.0, 1.0, 0.0 } );
      nor_ver.push_back( normalize( vec3( cos_ang_m, 1.0, sin_ang_m ) ) );
      cc_tt_ver.push_back( { frac_m, 1.0 } );

      triangulos.push_back( { 2*i, 2*i+1, (2*(i+1)) % (2*n) } );
   }
}

// *****************************************************************************

Esfera::Esfera( const unsigned n_mer, const unsigned n_par  )
{
   using namespace std ;
   ponerNombre( string("Esfera (") + to_string(n_mer) + " x " + to_string(n_par) + ")");

   //constexpr unsigned n_lat = 64, n_mer = 64 ;
   constexpr float    radio = 1.0f ;

   for( unsigned i_lat  = 0 ; i_lat  <= n_par  ; i_lat++ )
   for( unsigned i_long = 0 ; i_long <= n_mer ; i_long++ )
   {
      const float
         f_lat    = float(i_lat)/float(n_par),
         f_long   = float(i_long)/float(n_mer),
         ang_lat  = (f_lat-0.5)*M_PI,
         ang_long = M_PI*2.0*f_long,
         d        = cos(ang_lat),
         y        = sin(ang_lat),
         x        = d*cos(ang_long),
         z        = d*sin(ang_long);

      vertices.push_back( { radio*x,radio*y,radio*z } );
      nor_ver.push_back( { x,y,z } );
      cc_tt_ver.push_back( { f_long, f_lat } );
   }

   for( unsigned i_lat  = 0 ; i_lat  < n_par  ; i_lat++ )
   for( unsigned i_long = 0 ; i_long < n_mer ; i_long++ )
   {
      const unsigned n = n_mer+1,
                     i = i_lat*n + i_long ;

      triangulos.push_back( { i,   i+n,   i+1   } );
      triangulos.push_back( { i+n, i+n+1, i+1 } );
   }

}

EsferaBajaRes::EsferaBajaRes()
{
   constexpr int n_lat = 16, n_mer = 32 ;
   constexpr float radio = 1.0f ;

   using namespace std ;
   ponerNombre( string("Esfera baja res (") + to_string(n_mer) + " x " + to_string(n_lat) + ")");

   for( unsigned i_lat  = 0 ; i_lat  <= n_lat  ; i_lat++ )
   for( unsigned i_long = 0 ; i_long <= n_mer ; i_long++ )
   {
      const float
         f_lat    = float(i_lat)/float(n_lat),
         f_long   = float(i_long)/float(n_mer),
         ang_lat  = (f_lat-0.5)*M_PI,
         ang_long = M_PI*2.0*f_long,
         d        = cos(ang_lat),
         y        = sin(ang_lat),
         x        = d*cos(ang_long),
         z        = d*sin(ang_long);

      vertices.push_back( { radio*x,radio*y,radio*z } );
      nor_ver.push_back( { x,y,z } );
      cc_tt_ver.push_back( { f_long, f_lat } );
   }

   for( unsigned i_lat  = 0 ; i_lat  < n_lat  ; i_lat++ )
   for( unsigned i_long = 0 ; i_long < n_mer ; i_long++ )
   {
      const unsigned n = n_mer+1,
                     i = i_lat*n + i_long ;
      triangulos.push_back( { i,   i+n,   i+1   } );
      triangulos.push_back( { i+n, i+n+1, i+1 } );
   }

}
// *****************************************************************************

Semiesfera::Semiesfera()
{
   constexpr int n_lat = 64, n_mer = 64 ;
   constexpr float radio = 1.0f ;

   using namespace std ;
   ponerNombre( string("Semiesfera (") + to_string(n_mer) + " x " + to_string(n_lat) + ")");

   for( int i_lat  = 0 ; i_lat  <= n_lat  ; i_lat++ )
   for( int i_long = 0 ; i_long <= n_mer ; i_long++ )
   {
      const float
         f_lat    = float(i_lat)/float(n_lat),
         f_long   = float(i_long)/float(n_mer),
         ang_lat  = f_lat*M_PI*0.5,
         ang_long = M_PI*2.0*f_long,
         d        = cos(ang_lat),
         y        = sin(ang_lat),
         x        = d*cos(ang_long),
         z        = d*sin(ang_long);

      vertices.push_back( { radio*x,radio*y,radio*z } );
      nor_ver.push_back( { x,y,z } );
      cc_tt_ver.push_back( { f_long, f_lat } );
   }

   for( unsigned i_lat  = 0 ; i_lat  < n_lat  ; i_lat++ )
   for( unsigned  i_long = 0 ; i_long < n_mer ; i_long++ )
   {
      const unsigned n = n_mer+1,
                     i = i_lat*n + i_long ;
      triangulos.push_back( { i,   i+n,   i+1   } );
      triangulos.push_back( { i+n, i+n+1, i+1 } );
   }

}
// *****************************************************************************

Cilindro::Cilindro( const unsigned n_hor, const unsigned n_vert )
{
   assert( 3 < n_hor );
   assert( 2 < n_vert );

   constexpr float radio = 1.0f ;

   using namespace std ;
   ponerNombre( string("Esfera baja res (") + to_string(n_hor) + " x " + to_string(n_vert) + ")");

   for( unsigned i_vert = 0 ; i_vert <= n_vert ; i_vert++ )
   for( unsigned i_hor  = 0 ; i_hor  <= n_hor  ; i_hor++  )
   {
      const float
         f_vert   = float(i_vert)/float(n_vert),
         f_hor    = float(i_hor)/float(n_hor),
         ang_hor  = f_hor*2.0*M_PI,
         x        = cos(ang_hor),
         z        = sin(ang_hor),
         y        = f_vert ;

      vertices.push_back( { radio*x,y,radio*z } );
      nor_ver.push_back( { x,0.0,z } );
      cc_tt_ver.push_back( { f_hor, f_vert } );
   }

   for( unsigned i_vert = 0 ; i_vert < n_vert ; i_vert++ )
   for( unsigned i_hor  = 0 ; i_hor  < n_hor  ; i_hor++  )
   {
      const unsigned n = n_hor+1,
                     i = i_vert*n + i_hor ;
      triangulos.push_back( { i,   i+n,   i+1   } );
      triangulos.push_back( { i+n, i+n+1, i+1 } );
   }

}

