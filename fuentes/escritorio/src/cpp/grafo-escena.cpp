// *********************************************************************
// **
// ** Asignatura: Programación del Cauce Gráfico (PCG)
// ** 
// ** Gestión de grafos de escena (implementación)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de: 
// **     + Clase 'NodoGrafoEscena' (derivada de 'ObjetoVisu')
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

#include "utilidades.h"  
#include "grafo-escena.h"
#include "aplic-3d.h"    
#include "seleccion.h"   // para 'ColorDesdeIdent' 



// *********************************************************************
// Entrada del nodo del Grafo de Escena

// ---------------------------------------------------------------------
// Constructor para entrada de tipo sub-objeto

EntradaNGE::EntradaNGE( ObjetoVisu3D * pObjeto )
{
   assert( pObjeto != nullptr );
   tipo   = TipoEntNGE::objeto ;
   objeto = pObjeto ;
}
// ---------------------------------------------------------------------
// Constructor para entrada de tipo "matriz de transformación"

EntradaNGE::EntradaNGE( const glm::mat4 & pMatriz )
{
   tipo    = TipoEntNGE::transformacion ;
   matriz  = new glm::mat4() ; // matriz en el heap, puntero propietario
   *matriz = pMatriz ;
}

// ---------------------------------------------------------------------
// Constructor para entrada de tipo "matriz de transformación"

EntradaNGE::EntradaNGE( Material * pMaterial )
{
   assert( pMaterial != nullptr );
   tipo     = TipoEntNGE::material ;
   material = pMaterial ;
}

// -----------------------------------------------------------------------------
// Destructor de una entrada

EntradaNGE::~EntradaNGE()
{
   /**  no fnciona debido a que se hacen copias (duplicados) de punteros
    * PERO SE PUEDE IMPLEMENTAR UN COPY-CONSTRUCTOR y un MOVE-CONSTRUCTOR, PTE
   if ( tipo == TipoEntNGE::transformacion )
   {
      assert( matriz != NULL );
      delete matriz ;
      matriz = NULL ;
   }
   * **/
}

// *****************************************************************************
// Nodo del grafo de escena: contiene una lista de entradas
// *****************************************************************************

NodoGrafoEscena::NodoGrafoEscena()
{
   ponerNombre( "nodo del grafo de escena." );
}

// -----------------------------------------------------------------------------

NodoGrafoEscena::~NodoGrafoEscena()
{
   using namespace std ;
   //cout << "Invocado destructor de nodo del grafo de escena (destr. matrices)'" << leerNombre() << "'" << endl ; 
   for( auto & ent : entradas )
      if ( ent.tipo == TipoEntNGE::transformacion )
         delete ent.matriz ;
}  

// -----------------------------------------------------------------------------

void NodoGrafoEscena::pendienteDestruccion()
{
   using namespace std ;
   //cout << "NodoGrafoEscena::pendienteDestruccion : " << this << " '" << leerNombre() << "'" << endl ;

   pendientes_destr.insert( this );
   
   for( auto & ent : entradas )
      if ( ent.tipo == TipoEntNGE::objeto )
         ent.objeto->pendienteDestruccion();
}

// -----------------------------------------------------------------------------
// Visualiza usando OpenGL

void NodoGrafoEscena::visualizarGL(  )
{
   using namespace std ;
   Aplicacion3D * apl = Aplicacion3D::instancia() ;
  
    // comprobar que hay un cauce y una pila de materiales y recuperarlos.
   Cauce3D *        cauce           = apl->cauce3D() ;            
   PilaMateriales * pila_materiales = apl->pilaMateriales(); 

   // Visualización del nodo:
   //
   // Se deben de recorrer las entradas y llamar recursivamente de visualizarGL, pero 
   // teniendo en cuenta que, al igual que el método visualizarGL de las mallas indexadas,
   // si el nodo tiene un color, debemos de cambiar el color del cauce (y hacer push/pop). 
   // Además, hay que hacer push/pop de la pila de modelado. 
   // Así que debemos de dar estos pasos:
   //
   // 1. Si el objeto tiene un color asignado (se comprueba con 'tieneColor')
   //     - hacer push del color actual del cauce (con 'pushColor') y después
   //     - fijar el color en el cauce (con 'fijarColor'), usando el color del objeto (se lee con 'leerColor()')
   // 2. Guardar copia de la matriz de modelado (con 'pushMM'), 
   // 3. Para cada entrada del vector de entradas:
   //     - si la entrada es de tipo objeto: llamar recursivamente a 'visualizarGL'
   //     - si la entrada es de tipo transformación: componer la matriz (con 'compMM')
   // 4. Restaurar la copia guardada de la matriz de modelado (con 'popMM')
   // 5. Si el objeto tiene color asignado:
   //     - restaurar el color original a la entrada (con 'popColor')


   // Gestión de los materiales cuando la iluminación está activada    
   //
   // Si 'apl->iluminacion' es 'true', se deben de gestionar los materiales:
   //
   //   1. al inicio, hacer 'push' de la pila de materiales (guarda material actual en la pila)
   //   2. si una entrada es de tipo material, activarlo usando a pila de materiales
   //   3. al finalizar, hacer 'pop' de la pila de materiales (restaura el material activo al inicio)

   if ( tieneColor() )
   {
      // guardar copia del color previo  en la pila de colores, y fijar color actual
      cauce->pushColor();
      cauce->fijarColor( leerColor() );
   }

   // hacer 'push' de la matriz de modelado
   cauce->pushMM();

   // si la iluminación está activada, guardar en la pila un puntero al material activado 
   if ( apl->iluminacionActiva() )
      pila_materiales->push() ;

   // recorrer todas las entradas del array que hay en el nodo:
   for( unsigned i = 0 ; i < entradas.size() ; i++ )
      switch( entradas[i].tipo )
      {
         case TipoEntNGE::objeto :
            assert( entradas[i].objeto != nullptr );
            entradas[i].objeto->visualizarGL(  ) ; // visualizarlo
            break ;
         case TipoEntNGE::transformacion :
            assert( entradas[i].matriz != nullptr );
            cauce->compMM( *(entradas[i].matriz) );
            break ;
         case TipoEntNGE::material :
            if ( apl->iluminacionActiva() )
               pila_materiales->activar( entradas[i].material );
            break ;
         default:
            cout << "error: tipo de entrada incorrecto en 'NodoGrafoEscena::visualizarGL'" << endl ;
            exit(1);
            break ;
      }

   // si iluminacion activada, restaurar material previo al nodo
   if ( apl->iluminacionActiva() )
      pila_materiales->pop();

   // restaurar el color anterior
   if ( tieneColor())
      cauce->popColor();

   // hacer 'pop' de la matriz de modelado
   cauce->popMM();
}

// *****************************************************************************
// visualizar pura y simplemente la geometría, sin colores, normales, coord. text. etc...

void NodoGrafoEscena::visualizarGeomGL(  )
{
   using namespace std ;
   // comprobar que hay un cauce 
   Aplicacion3D * apl   = Aplicacion3D::instancia() ;
   Cauce3D *      cauce = apl->cauce3D() ;; assert( cauce != nullptr );
  
   // Visualización del nodo (ignorando colores)
   //
   // Este método hace un recorrido de las entradas del nodo, parecido a 'visualizarGL', pero más simple,
   // Se dan estos pasos:
   //
   // 1. Guardar copia de la matriz de modelado (con 'pushMM'), 
   // 2. Para cada entrada del vector de entradas:
   //         - Si la entrada es de tipo objeto: llamar recursivamente a 'visualizarGeomGL'.
   //         - Si la entrada es de tipo transformación: componer la matriz (con 'compMM').
   //   3. Restaurar la copia guardada de la matriz de modelado (con 'popMM')

 
 
   // hacer 'push' de la matriz de modelado
   cauce->pushMM();

   // recorrer todas las entradas del array que hay en el nodo, llamar a visualizarGeomGL para hijos.
   for( unsigned i = 0 ; i < entradas.size() ; i++ )
      switch( entradas[i].tipo )
      {  case TipoEntNGE::objeto :
            assert( entradas[i].objeto != nullptr );
            entradas[i].objeto->visualizarGeomGL( ) ; 
            break ;
         case TipoEntNGE::transformacion :
            assert( entradas[i].matriz != nullptr );
            cauce->compMM( *(entradas[i].matriz) );
            break ;
         case TipoEntNGE::material :
            break ;
         default:
            cout << "error: tipo de entrada incorrecto en 'NodoGrafoEscena::visualizarGL'" << endl ;
            if ( entradas[i].tipo == TipoEntNGE::noInicializado )  // por si hay constructor mal escrito
               cout << "(el tipo de entrada es 'no inicializado')" << endl << flush ;
            exit(1);
            break ;
      }
   // hacer 'pop' de la matriz de modelado
   cauce->popMM();
}

// -----------------------------------------------------------------------------
// Visualizar las normales de los objetos del nodo

void NodoGrafoEscena::visualizarNormalesGL(  )
{
   using namespace std ;

   // comprobar que hay un cauce 
   Aplicacion3D * apl = Aplicacion3D::instancia() ;
   Cauce3D * cauce = apl->cauce3D() ;; assert( cauce != nullptr );
  

   // Visualizar las normales del nodo del grafo de escena
   //
   // Este método hace un recorrido de las entradas del nodo, parecido a 'visualizarGL', teniendo 
   // en cuenta estos puntos:
   //
   // - usar push/pop de la matriz de modelado al inicio/fin (al igual que en visualizatGL)
   // - recorrer las entradas, llamando recursivamente a 'visualizarNormalesGL' en los nodos u objetos hijos
   // - ignorar el color o identificador del nodo (se supone que el color ya está prefijado antes de la llamada)
   // - ignorar las entradas de tipo material, y la gestión de materiales (se usa sin iluminación)

   // hacer 'push' de la matriz de modelado
   cauce->pushMM();

   // recorrer todas las entradas del array que hay en el nodo:
   for( unsigned i = 0 ; i < entradas.size() ; i++ )
      switch( entradas[i].tipo )
      {
         case TipoEntNGE::objeto :
            assert( entradas[i].objeto != nullptr );
            entradas[i].objeto->visualizarNormalesGL( ) ; // visualizar sub-objeto (solo geometría)
            break ;
         case TipoEntNGE::transformacion :
            assert( entradas[i].matriz != nullptr );
            cauce->compMM( *(entradas[i].matriz) );
            break ;
         case TipoEntNGE::material :
            break ;
         default:
            cout << "error: tipo de entrada incorrecto en 'NodoGrafoEscena::visualizarGL'" << endl ;
            exit(1);
            break ;
      }
   
   // hacer 'pop' de la matriz de modelado
   cauce->popMM();

}

// -----------------------------------------------------------------------------
// visualizar el objeto en 'modo seleccion', es decir, sin iluminación y con los colores 
// basados en los identificadores de los objetos
void NodoGrafoEscena::visualizarModoSeleccionGL()
{
   using namespace std ;
   Aplicacion3D * apl = Aplicacion3D::instancia() ;
   Cauce3D * cauce = apl->cauce3D() ; ; assert( cauce != nullptr );

   // Visualizar este nodo en modo selección.
   // Se dan estos pasos:
   // 
   // 2. Leer identificador (con 'leerIdentificador'), si el identificador no es -1 
   //      + Guardar una copia del color actual del cauce (con 'pushColor')
   //      + Fijar el color del cauce de acuerdo al identificador, (usar 'ColorDesdeIdent'). 
   // 3. Guardar una copia de la matriz de modelado (con 'pushMM')
   // 4. Recorrer la lista de nodos y procesar las entradas transformación o subobjeto:
   //      + Para las entradas subobjeto, invocar recursivamente a 'visualizarModoSeleccionGL'
   //      + Para las entradas transformación, componer la matriz (con 'compMM')
   // 5. Restaurar la matriz de modelado original (con 'popMM')   
   // 6. Si el identificador no es -1, restaurar el color previo del cauce (con 'popColor')
   
   const int ident_nodo = leerIdentificador();
   if ( ident_nodo != -1 )
   {
      cauce->pushColor();
      cauce->fijarColor( ColorDesdeIdent( ident_nodo ) );
   }
   // recorrer todas las entradas del array que hay en el nodo:
   cauce->pushMM();

   for( unsigned i = 0 ; i < entradas.size() ; i++ )
      switch( entradas[i].tipo )
      {  case TipoEntNGE::objeto :
            assert( entradas[i].objeto != nullptr ); 
            entradas[i].objeto->visualizarModoSeleccionGL( ) ; 
            break ;
         case TipoEntNGE::transformacion :
            assert( entradas[i].matriz != nullptr );
            cauce->compMM( *(entradas[i].matriz) );
            break ;
         case TipoEntNGE::material :
            break ;
         default:
            cout << "error: tipo de entrada incorrecto en 'NodoGrafoEscena::visualizarModoSeleccionGL'" << endl ;
            exit(1);
            break ;
      }
      
   cauce->popMM();
   if ( ident_nodo != -1 )
      cauce->popColor();
}

// -----------------------------------------------------------------------------
// Añadir una entrada (al final).
// genérica (de cualqiuer tipo de entrada)

unsigned NodoGrafoEscena::agregar( const EntradaNGE & entrada )
{
   // Agregar la entrada al nodo, devolver índice de la entrada agregada
   entradas.push_back( entrada );
   return entradas.size()-1 ;
}
// -----------------------------------------------------------------------------
// construir una entrada y añadirla (al final)
// objeto (copia solo puntero)

unsigned NodoGrafoEscena::agregar( ObjetoVisu3D * pObjeto )
{
   return agregar( EntradaNGE( pObjeto ) );
}
// ---------------------------------------------------------------------
// construir una entrada y añadirla (al final)
// matriz (copia objeto)

unsigned NodoGrafoEscena::agregar( const glm::mat4 & pMatriz )
{
   return agregar( EntradaNGE( pMatriz ) );
}
// ---------------------------------------------------------------------
// material (copia solo puntero)
unsigned NodoGrafoEscena::agregar( Material * pMaterial )
{
   return agregar( EntradaNGE( pMaterial ) );
}

// devuelve el puntero a la matriz en la i-ésima entrada
glm::mat4 * NodoGrafoEscena::leerPtrMatriz( unsigned indice )
{
   
   // Devolver el puntero a la matriz en la entrada indicada por 'indice'. 
   // Debe de dar error y abortar si: 
   //   - el índice está fuera de rango 
   //   - la entrada no es de tipo transformación
   //   - el puntero a la matriz es nulo 
   
   assert( indice < entradas.size() );
   assert( entradas[indice].tipo == TipoEntNGE::transformacion );
   assert( entradas[indice].matriz != nullptr );

   return entradas[indice].matriz ;
}
// -----------------------------------------------------------------------------
// si 'centro_calculado' es 'false', recalcula el centro usando los centros
// de los hijos (el punto medio de la caja englobante de los centros de hijos)

void NodoGrafoEscena::calcularCentroOC()
{
   using namespace std ;
   using namespace glm ;

   // Calcular y guardar el centro del nodo
   //    en coordenadas de objeto (hay que hacerlo recursivamente)
   //   (si el centro ya ha sido calculado, no volver a hacerlo)
   if ( centro_calculado )
      return ;
   //cout << "entra NodoGrafoEscena::calcularCentroOC(), objeto == " << leerNombre() << endl ;

   vec4 centro_min, centro_max ;
   mat4 matmod = glm::mat4( 1.0f ) ; 
   bool primero = true ;

   // calcular recursivamente el centro en los nodos hijos:

   for( unsigned i = 0 ; i < entradas.size() ; i++ )
   {
      switch( entradas[i].tipo )
      {
         case TipoEntNGE::objeto : // tipo puntero a sub-objeto
            assert( entradas[i].objeto != nullptr );
            // calcular centro del hijo, si no estaba calculado ya
            entradas[i].objeto->calcularCentroOC();
            // leer el centro del hijo y aplicarle matriz de modelado actual
            {
               const vec3 ci3 = entradas[i].objeto->leerCentroOC() ;
               const vec4 ci4 = vec4( ci3.x, ci3.y, ci3.z, 1.0f );
               const vec4 centro_hijo = matmod*ci4;

               // actualizar 'centro_min' y 'centro_max'
               if ( primero )
               {  centro_min = centro_hijo ;
                  centro_max = centro_hijo ;
                  primero = false ;
               }
               else
               {  for( unsigned j = 0 ; j < 4 ; j++ )
                  {  centro_min[j] = std::min( centro_min[j], centro_hijo[j] );
                     centro_max[j] = std::max( centro_max[j], centro_hijo[j] ); 
                  }
               }
            }
            break ;
         case TipoEntNGE::transformacion : // tipo matriz de transformación
            // componer la matriz con la matriz de modelado (por la derecha)
            matmod = matmod*(*(entradas[i].matriz));
            break ;
         case TipoEntNGE::material : // tipo material
            // ignorarlo
            break ;
         default: // cualquier otro tipo
            cout << "error: tipo de entrada incorrecto en NodoGrafoEscena::visualizarGL" << endl ;
            if ( entradas[i].tipo == TipoEntNGE::noInicializado )  // por si hay constructor mal escrito
               cout << "(el tipo de entrada es 'no inicializado')" << endl << flush ;
            exit(1);
            break ;
      }
   }
   vec4 cen = 0.5f*(centro_min+centro_max) ;
   ponerCentroOC( vec3( cen.x, cen.y, cen.z ) );
   centro_calculado = true ;
   //cout << "calculado centro de nodo '" << leerNombre() << "', centro = " << leerCentroOC() << endl ;
   
}
// -----------------------------------------------------------------------------
// método para buscar un objeto con un identificador y devolver un puntero al mismo

bool NodoGrafoEscena::buscarObjeto
(
   const int          ident_busc, // identificador a buscar
   const glm::mat4 &  mmodelado,  // matriz de modelado
   ObjetoVisu       **  objeto,     // (salida) puntero al puntero al objeto
   glm::vec3 &        centro_wc   // (salida) centro del objeto en coordenadas del mundo
)
{
   using namespace std ;
   using namespace glm ;
   
   assert( 0 < ident_busc );

   // Buscar un sub-objeto con un identificador
   // Se deben de dar estos pasos:

   // 1. calcula el centro del objeto, (solo la primera vez)
   calcularCentroOC() ;
   

   // 2. si el identificador del nodo es el que se busca, ya está (terminar)
   if ( leerIdentificador() == ident_busc ) // encontrado: terminar búsqueda
   {
      if ( objeto != nullptr )
         *objeto = this ;
      centro_wc = mmodelado*vec4( leerCentroOC(), 1.0f );
      return true ;
   }
   

   // 3. El nodo no es el buscado: buscar recursivamente en los hijos
   //    (si alguna llamada para un sub-árbol lo encuentra, terminar y devolver 'true')
   mat4 matmod = mmodelado ;  // matriz de modelado acumulada local

   for( unsigned i = 0 ; i < entradas.size() ; i++ )
   {
      switch( entradas[i].tipo )
      {
         case TipoEntNGE::objeto : // tipo puntero a sub-objeto
            assert( entradas[i].objeto != nullptr );
            // buscar recursivamente en el hijo
            {  vec3 centro_hijo ;
               if ( entradas[i].objeto->buscarObjeto( ident_busc, matmod, objeto, centro_hijo ) )
               {  centro_wc = centro_hijo ;
                  return true ;
               }
            }
            break ;
         case TipoEntNGE::transformacion : // tipo matriz de transformación
            // componer la matriz con la matriz de modelado (por la derecha)
            matmod = matmod*(*(entradas[i].matriz));
            break ;
         case TipoEntNGE::material : // tipo material
            // ignorarlo
            break ;
         default: // cualquier otro tipo
            cout << "error: tipo de entrada incorrecto en NodoGrafoEscena::visualizarGL" << endl ;
            if ( entradas[i].tipo == TipoEntNGE::noInicializado )  // por si hay constructor mal escrito
               cout << "(el tipo de entrada es 'no inicializado')" << endl << flush ;
            exit(1);
            break ;
      }
   }
   

   // ni este nodo ni ningún hijo es el buscado: terminar
   return false ;
}






