// *********************************************************************
// **
// ** Objetos que se pueden visualizar (implementación)
// ** Copyright (C) 2014 Carlos Ureña
// **
// ** Implementación de la clase 'Objeto3D' (objetos que se pueden 
// ** visualizar). Son en esencia objetos de clases derivadas que 
// ** definen un método llamado 'visualizarGL'
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

#include <iostream>

#include "objeto-visu.h"
#include "aplic-base.h"  
#include "colecciones-objs.h"
#include "seleccion.h"
#include "camara.h"

using namespace std ;


std::set<ObjetoVisu *> ObjetoVisu::pendientes_destr ;

// -----------------------------------------------------------------------------

ObjetoVisu::ObjetoVisu()
{
}

// -----------------------------------------------------------------------------
// Destructor

ObjetoVisu::~ObjetoVisu()
{
   using namespace std ;
   //cout << "INvocado desructor de ObjetoVisu (no hace nada) '" << nombre_obj << "'" << endl << flush ;
}

// -----------------------------------------------------------------------------

void ObjetoVisu::pendienteDestruccion()
{
   pendientes_destr.insert( this );
}

// -----------------------------------------------------------------------------

void ObjetoVisu::destruirPendientes()
{
   using namespace std ;
   const unsigned n = pendientes_destr.size() ;
   unsigned i = 1 ;

   //cout << "INICIO Destruyendo objetos pendientes destrucción,n.obsj == " << n << endl << flush ;
   
   for( auto & p : pendientes_destr )
   {
      //cout << "---" << endl 
      //     << "Destruyendo objeto pendiente destrucción " << i << " / " << n << "  " << p << " (" << p->leerNombre() << ")" << endl << flush ;
      //cout << "      typeid == '" << typeid(*p).name() << "'" << endl << flush ;
      delete p ;
      //cout << "  -->hecho delete "<< i << "/" << n << endl << flush ;
      i++ ;
   }
   cout << "Destruidos " << n << " objetos de tipo 'ObjetoVisu' pendientes de destrucción" << endl << flush ;
}

// -----------------------------------------------------------------------------

std::string ObjetoVisu::leerNombre() const 
{
   return nombre_obj ;
}
// -----------------------------------------------------------------------------

void ObjetoVisu::ponerNombre( const std::string & nuevoNombre )
{
   nombre_obj = nuevoNombre ;
}
// -----------------------------------------------------------------------------

int ObjetoVisu::leerIdentificador()
{
   return identificador ;
}
// -----------------------------------------------------------------------------

void ObjetoVisu::ponerIdentificador( int nuevoIdent )
{
   identificador = nuevoIdent ;
}
// -----------------------------------------------------------------------------

bool ObjetoVisu::tieneColor()
{
   return tiene_color ;
}
// -----------------------------------------------------------------------------

glm::vec3 ObjetoVisu::leerColor()
{
   assert( tieneColor() );
   return color_objeto ;
}
// -----------------------------------------------------------------------------

void ObjetoVisu::ponerColor( const glm::vec3 & nuevo_color )
{
   tiene_color = true ;
   color_objeto = nuevo_color ;
}
// -----------------------------------------------------------------------------

void ObjetoVisu::ponerCentroOC( const glm::vec3 & nuevoCentro )
{
   centro_oc = nuevoCentro ;
}
// -----------------------------------------------------------------------------

bool ObjetoVisu::cuandoClick( const glm::vec3 & centro_wc ) 
{
   using namespace std ;
   
   cout << "Ejecutando método 'cuandoClick' por defecto." << endl ;
   cout << "Click realizado sobre el objeto: '" << leerNombre() << "'" << endl ;

   return true ;
}
// -----------------------------------------------------------------------------

glm::vec3 ObjetoVisu::leerCentroOC()
{
   return centro_oc ;
}
// -----------------------------------------------------------------------------

void ObjetoVisu::calcularCentroOC()
{
   // Por defecto no se hace nada: se asume que el centro está bien calculado
   // en el constructor. Se puede redefinir en clases derivadas, por ejemplo,
   // se redefine en los nodos de grafo de escena. 
   // (y se puede redefinir en las mallas indexadas)
}

// -----------------------------------------------------------------------------
// buscar un identificador (implementación por defecto para todos los Objeto3D)

bool ObjetoVisu::buscarObjeto
(
   const int          ident_busc,
   const glm::mat4 &  mmodelado,
   ObjetoVisu **        objeto,
   glm::vec3 &        centro_wc
)
{
   using namespace glm ;
   assert( 0 < ident_busc );

   if ( identificador == ident_busc )
   {
      centro_wc = mmodelado*vec4( leerCentroOC(), 1.0f ) ;
      if ( objeto != nullptr )
         *objeto = this ;
      return true ;
   }
   else
      return false ;
}

// -----------------------------------------------------------------------------
// Devuelve el número de parámetros de este objeto
// (por defecto no hay ningún parámetro: devuelve 0)
// (virtual: redefinir en las clases derivadas)

unsigned ObjetoVisu::leerNumParametros() const
{
   return 0 ;
}

// -----------------------------------------------------------------------------
// cambia el parámetro activo actualmente (variable 'ind_par_act')
// si d == +1 pasa al siguiente (o del último al primero)
// si d == -1 pasa al anterior (o del primero al último)
// (si hay 0 parámetros, no hace nada)

void ObjetoVisu::modificarIndiceParametroActivo( const int d )
{
   const int n = leerNumParametros() ;

   using namespace std ;

   if ( n == 0 )
   {  cout << "no es posible cambiar el parámetro activo, este objeto no tiene ningún parámetro definido." << endl ;
      return ;
   }
   assert( 0 <= ind_par_act );
   ind_par_act = ( ind_par_act + n + d ) % n ;
   cout << "parámetro actual del objeto cambiado a " << (ind_par_act+1) << " / " << n << endl ;
}

// -----------------------------------------------------------------------------
// Incrementa o decrementa el valor base de uno de los parámetros del objeto
//  iParam : índice del parámetro (entre 0 y numParametros()-1 )
//  delta  : número de unidades de incremento o decremento (típicamente +1 o -1)
// (por defecto produce un error: número de parámetro fuera de rango)

void ObjetoVisu::modificarParametro( const unsigned iParam, const int delta )
{
   assert( iParam < leerNumParametros() );
   initTP();
   tiempo_par_sec[iParam] += float(delta)/10.0 ; // cada unidad es equivalente a una décima de segundo
   actualizarEstadoParametro( iParam, tiempo_par_sec[iParam] );
}

// -----------------------------------------------------------------------------
// igual que el anterior, pero cambia el parámetro activo (iParam --> ind_par_act)
void ObjetoVisu::modificarParametro( const int delta )
{
   assert( 0 <= ind_par_act && ind_par_act < int(leerNumParametros()) );
   initTP();
   tiempo_par_sec[ind_par_act] += float(delta)/10.0 ; // cada unidad es equivalente a una décima de segundo
   actualizarEstadoParametro( ind_par_act, tiempo_par_sec[ind_par_act] );
}

// -----------------------------------------------------------------------------
// Actualiza el valor de un parámetro a un instante de tiempo
//  iParam : índice del parámetro (entre 0 y numParametros()-1 )
//  tSec   : tiempo en segundos desde el estado inicial
// (por defecto produce un error)
// (virtual: redefinir en las clases derivadas)

void ObjetoVisu::actualizarEstadoParametro( const unsigned iParam, const float tSec )
{
   cerr << "Se ha invocado el método: " << __PRETTY_FUNCTION__  << endl
        << "(este método se debe redefinir para clases de objetos que tengan algún parámetro)" << endl ;
   exit(1);
}

// -----------------------------------------------------------------------------
// Actualiza el estado del objeto tras transcurrir un intervalo de tiempo
// (recorre todos los parámetros y los actualiza uno a uno, para ello
// llama a actualizarEstadoParametro)
// dtSec : es el tiempo en segundos transcurrido desde el inicio de las
//         animaciones o la última llamada a este método

void ObjetoVisu::actualizarEstado( const float dtSec )
{
   initTP();
   for( unsigned i = 0 ; i < leerNumParametros() ; i++  )
   {
      tiempo_par_sec[i] += dtSec ;
      actualizarEstadoParametro( i, tiempo_par_sec[i] );
   }
}

// -----------------------------------------------------------------------------
// Pone los valores de los parámetros a cero (estado inicial)

void ObjetoVisu::resetParametros()
{
   initTP();
   for( unsigned i = 0 ; i < leerNumParametros() ; i++  )
   {
      tiempo_par_sec[i] = 0.0 ;
      actualizarEstadoParametro( i, tiempo_par_sec[i] );
   }
}

// -----------------------------------------------------------------------------
// la primera vez que se llama, inicializa los valores base de los
// parámetros (a cero), las siguientes comprueba que tiempo_par_sec tiene tantas
// entradas como parámetros.

void ObjetoVisu::initTP()
{
   if ( tiempo_par_sec.size() == 0  )
      for( unsigned i = 0 ; i < leerNumParametros() ; i++ )
         tiempo_par_sec.push_back( 0.0 );
   else
      assert( tiempo_par_sec.size() == leerNumParametros() );
}


// *****************************************************************************
// Métodos específicos de ObjetoVisu2D


ObjetoVisu2D::ObjetoVisu2D() 
{

}
// -----------------------------------------------------------------------------

ObjetoVisu2D::~ObjetoVisu2D() 
{

}
// -----------------------------------------------------------------------------

void ObjetoVisu2D::visualizarSegmentosGL() 
{

}


// *****************************************************************************
// Métodos específicos de ObjetoVisu3D


ObjetoVisu3D::ObjetoVisu3D() 
{

}
// -----------------------------------------------------------------------------

ObjetoVisu3D::~ObjetoVisu3D() 
{

}
