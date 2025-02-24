// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Implementación de la función 'main'.
// ** bucle de eventos y visualización de un cuadro.
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de la función 'main' del programa:
// ** crea el objeto global 'AplicaciónIG' ('apl'), y ejecuta el 
// ** principal de gestión de eventos de GLFW.
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

#include "objeto-visu.h"
#include "aplic-2d.h"
#include "aplic-3d.h"

// evita la necesidad de escribir std::
using namespace std ;

// ---------------------------------------------------------------------

/// @brief Crea la aplicación correspondiente al tipo de aplicación especificado en la linea de órdenes 
/// @param argc (int) número de argumentos en la línea de órdenes
/// @param argv (char *) argumentos de la línea de órdenes
/// @return (AplicacionBase *) puntero a la aplicación creada
///
AplicacionBase * CrearAplicacion( int argc, char * argv[] )
{
   if ( argc < 2 )
   {
      cout << "Error: no se ha especificado el tipo de aplicación ('2d', '3da', o '3db'). Termino. " << endl ;
      exit(1);
   }
   const std::string tipo_aplic = argv[1] ;

   AplicacionBase * apl = nullptr ;

   if ( tipo_aplic == "2d")
      apl = new Aplicacion2D(); // usa Cauce2D y Cauce2DLineas (deriv. de CauceBase)
   else if ( tipo_aplic == "3da" )
      apl = new Aplicacion3D(3,3); // usa Cauce3D_ogl3 (deriv. de Cauce3D)
   else if ( tipo_aplic == "3db" )
      apl = new Aplicacion3D(4,2); // usa Cauce3D_ogl4 (deriv. de Cauce3D)
   else
   {
      cout << "Error: tipo de aplicación no reconocido ('" << tipo_aplic << "'). Termino. " << endl ;
      cout << "    + Usa '2d' para una aplicación 2D con OpenGL 3.3 (shaders: VS+GS+FS)" << endl ;
      cout << "    + Usa '3da' para una aplicación 3D con OpenGL 3.3 (shaders: VS+FS)" << endl ;
      cout << "    + Usa '3db' para una aplicación 3D con OpenGL 4.5 (shaders: VS+TS+GS+FS)" << endl ;
      exit(1) ;
   }
   return apl ;
}

// ---------------------------------------------------------------------
//
/// @brief Función principal del programa
/// @brief Crea la aplicación, ejecuta su bucle principal de eventos, y finalmente la destruye y limpia la memoria.
/// @param argc (int) número de argumentos en la línea de órdenes
/// @param argv (char *) argumentos de la línea de órdenes
///
int main( int argc, char *argv[] )
{   
   using namespace std ;
   cout << "PCG (MDS) - curso 2023-24 (" << NOMBRE_OS << ")" << endl ;

   // crear la aplicación en función de la línea de órdenes: 2D, 3D con OpenGL 3.3, o 3D con OpenGL 4.5.
   AplicacionBase * apl = CrearAplicacion( argc, argv ) ;
      
   // ejecuta el bucle principal de gestión de eventos de GLFW
   apl->buclePrincipalEventos() ;   
   
   // destruye la aplicación
   delete apl ;

   cout << "Programa terminado normalmente." << endl ;
   return 0;
}
