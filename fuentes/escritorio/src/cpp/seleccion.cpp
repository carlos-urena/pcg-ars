// *********************************************************************
// **
// ** Gestión de la selección (implementación)
// ** Copyright (C) 2014 Carlos Ureña
// **
// ** Implementación de las funciones:
// **   + Selección 
// **   + FijarColVertsIdent
// **   + LeerIdentEnPixel
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
#include "camara.h"
#include "fbo.h"
#include "aplic-3d.h"
#include "seleccion.h"
#include "colecciones-objs.h"
#include "grafo-escena.h"

// ----------------------------------------------------------------------------------
// calcula un color usando un identificador entero no negativo

glm::vec4 ColorDesdeIdent( const int ident )  // 0 ≤ ident < 2^24
{
   using namespace glm ;
   assert( 0 <= ident );

   // decodificar: extraer los tres bytes menos significativos del valor entero.
   const unsigned char
      byteR = ( ident            ) % 0x100U,  // rojo  = byte menos significativo
      byteG = ( ident / 0x100U   ) % 0x100U,  // verde = byte intermedio
      byteB = ( ident / 0x10000U ) % 0x100U;  // azul  = byte más significativo

   //log(" RGB == ",(unsigned int)byteR,", ",(unsigned int)byteG,", ", (unsigned int)byteB);

   // Fijar el color normalizando desde [0..255] hacia [0..1]
   // (otra posibilidad es pasarle los bytes directamente a 'glVertexAttrib4Nubv')
   // (ver: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttrib.xhtml)

   return vec4 { float(byteR)/255.0f, float(byteG)/255.0f, float(byteB)/255.0f, 1.0f };
}

// ----------------------------------------------------------------------------------
// leer un identificador entero codificado en el color de un pixel en el
// framebuffer activo actualmente

int LeerIdentEnPixel( int xpix, int ypix )
{
   CError();

   unsigned char bytes[3] ;
   glReadPixels( xpix,ypix, 1,1, GL_RGB, GL_UNSIGNED_BYTE, (void *)bytes);
   CError();

   using namespace std ;
   cout << "xpix ==" << xpix << ", ypix == " << ypix << endl ;
   cout << "bytes == " << int(bytes[0]) << ", " << int(bytes[1]) << ", " << int(bytes[2]) << endl ;

   // reconstruir el identificador y devolverlo:
   return int(bytes[0]) + ( int(0x100U)*int(bytes[1]) ) + ( int(0x10000U)*int(bytes[2]) ) ;
}

// -------------------------------------------------------------------------------
// Función principal de selección, se llama al hacer click con el botón izquierdo
//
// (x,y) = posición donde se ha hecho click en coordenadas del sistema de ventanas (enteras)
//
// devuelve: true si se ha seleccionado algún objeto, false si no


bool Aplicacion3D::seleccion( int x, int y ) 
{
   
   using namespace std ;
   using namespace glm ;

   // comprobar algunas precondiciones
   assert( 0 < ventana_tam_x );
   assert( 0 < ventana_tam_y );
   ColeccionObjs * coleccion = coleccionActual();
   assert( coleccion != nullptr );

   // Ejecutar 'cuandoClick' para el objeto en el pixel (x,y), si hay alguno.
   // Para ello se dan estos pasos:
   
   // (1) Crear y activar el objeto framebuffer (puntero 'fbo' de la clase 'AplicacionIG') 
   //     (si no existe se crea con el tamaño de la ventana actual, si existe se reutiliza, quizás redimensionandolo)
   if ( fbo == nullptr )
      fbo = new Framebuffer( ventana_tam_x, ventana_tam_y );
   assert( fbo != nullptr );
   fbo->activar( ventana_tam_x, ventana_tam_y ) ;
   CError();
   
   // (2) Visualizar la escena actual en modo selección. 
   visualizarGL_OA_Seleccion(  );
   CError();
   
   // (3) Leer el identificador del pixel en las coordenadas (x,y)
   const int ident_en_pixel = LeerIdentEnPixel( x, y );
   cout << "El valor entero en (" << x << "," << y << ") es: " << ident_en_pixel << endl ;
   CError();
   
   // (4) Desactivar el FBO (vuelve a activar el FBO por defecto, con nombre '0'), 
   //     se usa el método 'desactivar' del FBO
   fbo->desactivar() ;
   CError();
   
   // (5) Si el identificador del pixel es 0, imprimir mensaje y terminar (devolver 'false')
   if ( ident_en_pixel == 0 )
   {
      cout << "No hay ningún objeto seleccionable en este pixel." << endl ;
      return false ;
   }
   
   // (6) Buscar el identificdor en el objeto raiz de la escena y si se encuentra, ejecutar 'cuandoClick'
   ObjetoVisu * objeto_raiz       = coleccion->objetoActual() ; assert( objeto_raiz != nullptr );
   ObjetoVisu * objeto_encontrado = nullptr ;
   mat4       mmodelado         = mat4( 1.0f );
   vec3       centro_wc ;
   
   if ( objeto_raiz->buscarObjeto( ident_en_pixel, mmodelado, &objeto_encontrado, centro_wc ) )
   {
      assert( objeto_encontrado != nullptr );
      return objeto_encontrado->cuandoClick( centro_wc );
   }
   
   // si el flujo de control llega aquí, es que no se encuentra ese identificador, devolver false:
   cout << "El identificador del objeto en el pixel no se encuentra en el objeto raíz que se está visualizando." << endl ;
   return false;
}
