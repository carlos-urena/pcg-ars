// *********************************************************************
// **
// ** Gestión de materiales y texturas (implementación)
// ** Copyright (C) 2014 Carlos Ureña
// **
// ** Implementación de:
// **    + clase 'Textura' (y derivadas 'TexturaXY', 'TexturaXZ')
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

#include "aplic-3d.h"
#include "texturas.h"

using namespace std ;

//const bool trazam = false ;

// **********************************************************************

Textura::Textura( const std::string & p_nombre_archivo )
{
   // Cargar imagen de textura
   // (las variables de instancia están inicializadas en la decl. de la clase)
   // El nombre del archivo debe convertirse a una cadena (char *) acabada en 
   // 0 tradicional en C. Para eso debe usarse el método 'c_str' de la clase 
   // 'std::string'.
   // El nombre del archivo debe ir sin el 'path', la función 'LeerArchivoJPG' lo 
   // busca en 'materiales/imgs' 
   
   // cargar imagen de textura, escribe en 'ancho' y 'alto'
   using namespace std ;
   nombre_archivo = p_nombre_archivo ;
   imagen = LeerArchivoJPEG( nombre_archivo.c_str(), ancho, alto ) ;
   assert( imagen != nullptr ) ;
   cout << "Leído archivo de textura '" << nombre_archivo << "' (" << ancho << " x " << alto << ")" << endl ;
}
//----------------------------------------------------------------------

void Textura::enviar()
{
   // Enviar la imagen de textura a la GPU
   // y configurar parámetros de la textura (glTexParameter)
   assert( imagen != NULL );
   CError();

   //glEnable( GL_TEXTURE_2D ) ; CError(); // no en OPENGL4 ??
   glActiveTexture( GL_TEXTURE0 ) ; CError();// ¿ innecesario si es la única activa ?

   // generar un nuevo nombre o identificador de textura
   glGenTextures( 1, &ident_textura ) ;            CError();
   glBindTexture( GL_TEXTURE_2D, ident_textura ) ; CError();

   using namespace std ;
   
   // enviar texels a la memoria de vídeo o la memoria de la GPU y
   // generar los mip-maps (versiones a múltiples resoluciones)
   
   CError();
   glTexImage2D
   (	
      GL_TEXTURE_2D ,   // GLenum target,
      0,                // GLint level (nivel de mipmap ?)
      GL_RGB,           // GLint internalformat (formato en el que quedará en la memoria de la GPU)
      ancho,            // GLsizei width   (número de columnas de pixels en la imagen)
      alto,             // GLsizei height (numero de filas de pixel en la imagen)
      0,                // GLint border (borde, no se usa, se pone a 0)
      GL_RGB,           // GLenum format (formato en la memoria de la aplicación)
      GL_UNSIGNED_BYTE, // GLenum type,
      imagen            // const void * data
   );
   CError();
   glGenerateMipmap( GL_TEXTURE_2D ); // generar mipmaps automáticamente
   CError();

   // configurar parámetros para interpolación de texels y mipmapping
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR );

   // configurar parámetros de repetición de la textura
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   CError();

   enviada = true ;
}
//----------------------------------------------------------------------

Textura::~Textura( )
{
   using namespace std ;
   cout << "Destruyendo textura leída de archivo '" <<  nombre_archivo << "'" << endl ;
   if ( imagen != nullptr )
      delete [] imagen ;

   imagen = nullptr ;
   cout << "hecho (no hecho!)" << endl << flush ;
}

//----------------------------------------------------------------------
// por ahora, se asume la unidad de texturas #0

// void Textura::activar(  )
// {
//    using namespace std ;
//    Cauce3D * cauce = Aplicacion3D::instancia()->cauce3D() ;

//    // Enviar la textura a la GPU (solo la primera vez) y activarla
//    if ( ! enviada )
//       enviar();

//    cauce->fijarEvalText( true, ident_textura );
//    cauce->fijarTipoGCT( int(modo_gen_ct), coefs_s, coefs_t );
   
// }

void Textura::activar( CauceBase * cauce )
{
   using namespace std ;
   assert( cauce != nullptr );
   

   // Enviar la textura a la GPU (solo la primera vez) y activarla
   if ( ! enviada )
      enviar();

   cauce->fijarEvalText( true, ident_textura );
   cauce->fijarTipoGCT( int(modo_gen_ct), coefs_s, coefs_t );
   
}
