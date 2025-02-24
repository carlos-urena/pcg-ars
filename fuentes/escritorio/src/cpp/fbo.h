// *********************************************************************
// **
// ** Gestión de un Framebuffer object (clase Framebuffer) (declaración)
// **
// ** Copyright (C) 2017-2022 Carlos Ureña
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

#include "utilidades.h"

class DescrVAO ; // declaración adelantada para romper circularidad.

class Framebuffer
{
   public:
      /// crea un 'framebuffer object' en la memoria de la GPU
      ///
      Framebuffer( const int pancho, const int palto );

      /// activa este 'framebuffer object', se hará rendering sobre él
      /// si el alto o el ancho especificado es distinto del actual
      /// se destruye el framebuffer en la GPU y se crea uno nuevo con estas
      /// dimensiones
      ///
      void activar( const int pancho, const int palto );

      /// activa el framebuffer por defecto (identificador 0)
      ///
      void desactivar();

      // leer el ancho y alto en pixels de este framebuffer
      int leerAncho() const ;
      int leerAlto() const ;

      /// @brief leer el color de un pixel del framebuffer
      /// @param ix - columna del pixel, debe estar entre 0 y el ancho (sin incluir)
      /// @param iy - fila del pixel, debe de estar entre 0 y el alto (sin incluir)
      /// @param rgb - vector de 3 bytes donde se escribe el color del pixel
      ///
      void leerPixel( const int ix, const int iy, unsigned char * rgb );

      // devuelve el identificador actual del fbo (!=0)
      GLuint leerIdent() const { return fboId ; }

      /// @brief libera la memoria que ocupa en la GPU
      ///
      ~Framebuffer();

      GLuint leerTextId() { return textId ; }


      /// @brief escribe cada pixel de este framebuffer como una suma ponderada de los pixels de otros dos framebuffers 
      /// @brief (los otros dos framebuffers deben tener el mismo tamaño, este se redimensiona a ese tamaño si es necesario)
      ///   
      /// @param fbo0 - puntero al framebuffer 0, no nulo
      /// @param w0 -   peso del framebuffer 0
      /// @param fbo1 - puntero al framebuffer 1, no nulo
      /// @param w1 -   peso del framebuffer 1
      ///
      void sumaPonderada( const Framebuffer * fbo0, const float w0, 
                          const Framebuffer * fbo1, const float w1 ) ;

       /// @brief visualiza este framebuffer en un rectangulo del viewport actual (framebuffer 0)
       /// 
       void visualizar( const unsigned x0, const unsigned y0, const unsigned ancho, const unsigned alto ) ;

   private:
      void inicializar( const int pancho, const int palto );
      void destruir();
      void comprobarEstado();

      GLuint fboId  = 0,  // identificador del framebuffer
             textId = 0,  // identificador de la textura de color asociada al framebuffer 
             rbId   = 0;  // identificador del z-buffer asociado al framebuffer 
      int    ancho  = 0,  // ancho del framebuffer, en pixels
             alto   = 0;  // alto del framebuffer, en pixels

};



