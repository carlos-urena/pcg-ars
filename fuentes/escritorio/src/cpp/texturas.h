// *********************************************************************
// **
// ** Gestión de materiales y texturas (declaraciones)
// ** Copyright (C) 2014 Carlos Ureña
// **
// ** Declaraciones de:
// **    + clase 'Textura' (y derivadas 'TexturaXY', 'TexturaXZ')
// **    
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

#pragma once

#include <vector>
#include "utilidades.h"
#include "lector-jpg.h"

class Textura  ;

//**********************************************************************
// posibles modos de generacion de coords. de textura

typedef enum
{  mgct_desactivada,
   mgct_coords_objeto,
   mgct_coords_ojo
}
   ModoGenCT ;

// *********************************************************************
// Clase Textura:
// ---------------
// clase que encapsula una imagen de textura de OpenGL, así como los
// parámetros relativos a como se aplica  a las primitivas que se dibujen
// mientras está activa

class Textura
{
   public:

   // carga una imagen de textura en la memoria de vídeo, e
   // inicializa los atributos de la textura a valores por defecto.
   // El nombre del archivo debe ir sin el 'path', se busca en 'materiales/imgs' y si 
   // no está se busca en 'archivos-alumno'
   Textura( const std::string & nombreArchivoJPG ) ;

   // libera la memoria dinámica usada por la textura, si hay alguna
   ~Textura() ;

   // activar una textura en un cauce base (el cauce base tiene funcionalidad de texturas)
   void activar( CauceBase * cauce ) ;

   protected: //--------------------------------------------------------

   void enviar() ;    // envia la imagen a la GPU (gluBuild2DMipmaps)

   std::string 
      nombre_archivo = "no asignado"; // nombre del archivo de imagen de textura

   bool
      enviada       = false ; // true si ha sido enviada, false en otro caso
   GLuint
      ident_textura = -1 ;// 'nombre' o identif. de textura para OpenGL
   unsigned
      ancho         = 0,  // número de columnas de la imagen
      alto          = 0 ; // número de filas de la imagen
   unsigned char *
      imagen        = nullptr ; // pixels de la imagen, por filas.
   ModoGenCT
      modo_gen_ct   = mgct_desactivada ;  // modo de generacion de coordenadas de textura
   float
      coefs_s[4]    = {1.0,0.0,0.0,0.0},   // si 'modo_gen_ct != desactivadas', coeficientes para la coord. S
      coefs_t[4]    = {0.0,1.0,0.0,0.0};   // idem para coordenada T
} ;

// *********************************************************************
// clase: TexturaXY
// ---------------------------------------------------------------------
// textura con generación automática de coords de textura (s=x,t=y)

class TexturaXY : public Textura
{
   public: TexturaXY( const std::string & nom );
} ;
class TexturaXZ : public Textura
{
   public: TexturaXZ( const std::string & nom );
} ;



