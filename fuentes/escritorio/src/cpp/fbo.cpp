// *********************************************************************
// **
// ** Gestión de un Framebuffer object (clase Framebuffer) (implementación)
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

#include "utilidades.h"
#include "fbo.h"
#include "vaos-vbos.h"



// ---------------------------------------------------------------------------------

Framebuffer::Framebuffer( const int pancho, const int palto )
{
   inicializar( pancho, palto );
}
// ---------------------------------------------------------------------------------

void Framebuffer::inicializar( const int pancho, const int palto )
{
   assert( fboId == 0 );
   assert( rbId  == 0 );
   assert( textId == 0 );
   assert( ancho == 0 );
   assert( alto == 0 );

   // leer y comprobar parámetros, comprobar y resetear errores previos de OpenGL
   ancho = pancho; assert( 2 < ancho );
   alto  = palto;  assert( 2 < alto );
   CError();

   // crear frame buffer (identificador 'fboId')
   glGenFramebuffers( 1, &fboId );
   CError();
   assert( 0 < fboId );

   // crear buffer o textura de color (identificador 'textid')
   //( reserva memoria en la GPU, sin asignar valores a los pixels)
   glGenTextures( 1, &textId );
   assert( 0 < textId );
   glBindTexture( GL_TEXTURE_2D, textId );
   constexpr GLint  cb_internal_format = GL_RGB;
   constexpr GLenum cb_format          = GL_RGB,
                    cb_type            = GL_UNSIGNED_BYTE ;
   glTexImage2D( GL_TEXTURE_2D, 0, cb_internal_format, ancho, alto, 0, cb_format, cb_type, nullptr );
   CError();

   // configurar buffer de color
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   CError();

   // adjuntar textura de color al framebuffer (como 'color attachment')
   glBindFramebuffer( GL_FRAMEBUFFER, fboId );
   glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textId, 0 );
   CError();

   // crear el render buffer (el z-buffer de framebuffer) (identificador en 'rbId')
   glGenRenderbuffers( 1, &rbId );
   assert( 0 < rbId );
	glBindRenderbuffer( GL_RENDERBUFFER, rbId );
   const GLenum rb_internal_format = GL_DEPTH_COMPONENT ;
   glRenderbufferStorage( GL_RENDERBUFFER, rb_internal_format, ancho, alto );
   CError();

   // adjuntar el z-buffer al framebuffer (como 'depth attachment')
   glBindFramebuffer( GL_FRAMEBUFFER, fboId );
   glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbId );
   CError();

   // hecho: comprobar que el estado es el correcto
   comprobarEstado( );

   // volver a activar el framebuffer y texura por defecto (el inicial de OpenGL)
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
   glBindTexture( GL_TEXTURE_2D, 0 );
}
// ------------------------------------------------------------------------------

void Framebuffer::activar( const int pancho, const int palto )
{
   assert( fboId != 0 );
   if ( ancho != pancho || alto != palto )
   {
      destruir();
      inicializar( pancho, palto );
   }
   glBindFramebuffer( GL_FRAMEBUFFER, fboId );
   CError();
}
// ------------------------------------------------------------------------------

void Framebuffer::desactivar()
{
   CError();
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
   CError();
}
// ------------------------------------------------------------------------------

int Framebuffer::leerAncho() const
{
   return ancho ;
}
// ------------------------------------------------------------------------------

int Framebuffer::leerAlto() const
{
   return alto ;
}


// ------------------------------------------------------------------------------

Framebuffer::~Framebuffer()
{
   destruir();
}
// ------------------------------------------------------------------------------

void Framebuffer::destruir()
{
   CError();
   glDeleteTextures( 1, &textId );
   glDeleteRenderbuffers( 1, &rbId );
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
   glDeleteFramebuffers( 1, &fboId );
   CError();

   fboId  = 0 ;
   rbId   = 0 ;
   textId = 0 ;
   ancho  = 0 ;
   alto   = 0 ;
}

// ---------------------------------------------------------------------------------
// texto fuente de los shaders para suma ponderada de dos texturas (dos framebuffers)
// sobre un tercero

// fuente del vertex shader

static const std::string src_vs_suma_pond = R"glsl(
#version 330 core

   layout (location=0) in vec2 in_pos ;  // atributo de entrada: posición del vértice
   layout (location=1) in vec2 in_cct ;  // atributo de entrada: coordenadas de textura
   out vec2 v_cct ;  // variable de salida: coordenadas de textura
   
   void main() 
   {  gl_Position = vec4( in_pos, 0.0, 1.0 );  // pasar la posición con Z=0
      v_cct       = in_cct ;                   // pasar las coordenadas de textura
   }
)glsl" ;

// fuente del fragment shader

static const std::string src_fs_suma_pond =  R"glsl(
#version 330 core 

   uniform sampler2D tex0, tex1 ; // samplers de textura uno y dos
   uniform float     w0, w1 ;     // pesos de cada una de las dos texturas
   in vec2           v_cct ;      // coordenadas de textura interpoladas al centro del pixel. 
   layout(location=0) out vec4 out_color ; //  variable de salida: color del pixel.
   
   void main()  
   {  
      ivec2 tam  = textureSize( tex0, 0 ) ;     // leer tamaño de la textura (número de filas y columnas), lod=0 
      ivec2 ipos = ivec2( vec2(tam)*v_cct );  // posición del pixel en coordenadas enteras (numero de fila y columna) 
      vec3  t0   = texelFetch( tex0, ipos, 0 ).rgb ; // consulta texel de la textura 0 (sin filtrar, lod=0) 
      vec3  t1   = texelFetch( tex1, ipos, 0 ).rgb ; // consulta texel de la textura 1 (sin filtrar, lod=0)
      out_color  = vec4( w0*t0 + w1*t1, 1.0f ) ; // calcular color del texel o pixel de salida.  
   }
)glsl" ;



// ------------------------------------------------------------------------------

void Framebuffer::sumaPonderada( const Framebuffer * fbo0, const float w0, 
                                 const Framebuffer * fbo1, const float w1 ) 
{
   assert( fboId > 0 );
   assert( fbo0 != nullptr ); 
   assert( fbo1 != nullptr );
   assert( fbo0->fboId > 0 );
   assert( fbo1->fboId > 0 );
   assert( fbo0->ancho == fbo1->ancho );
   assert( fbo0->alto == fbo1->alto );
   assert( ancho == fbo0->ancho );
   assert( alto == fbo0->alto );
   CError();


   // variables estáticas de esta función (para no crearlas cada vez)
   static GLuint 
      id_prog = 0 ; 
   static GLint 
      loc_w0 = -1, 
      loc_w1 = -1,
      loc_tex0 = -1 ,
      loc_tex1 = -1 ;

   if ( id_prog == 0 )
   { 
      // leer y comprobar los localizadores de los uniforms
      
      id_prog  = CrearObjetoPrograma( src_vs_suma_pond, src_fs_suma_pond );
      glUseProgram( id_prog );

      loc_tex0 = glGetUniformLocation( id_prog, "tex0" );  assert( loc_tex0 >= 0);
      loc_tex1 = glGetUniformLocation( id_prog, "tex1" );  assert( loc_tex1 >= 0);
      
      loc_w0   = glGetUniformLocation( id_prog, "w0" );    assert( loc_w0 >= 0);
      loc_w1   = glGetUniformLocation( id_prog, "w1" );    assert( loc_w1 >= 0);
      CError();
   }
   else 
      glUseProgram( id_prog );

   

   // activar el objeto programa y este framebuffer, redimensionandolo si es necesario
   activar( fbo0->ancho, fbo0->alto ); 
   CError();

   // asociar a cada unidad de textura la textura de color de cada framebuffer
   glActiveTexture( GL_TEXTURE0 ); glBindTexture( GL_TEXTURE_2D, fbo0->textId );
   glActiveTexture( GL_TEXTURE1 ); glBindTexture( GL_TEXTURE_2D, fbo1->textId );
   CError();

    // asignar a cada sampler del shader cada una de las dos unidades de textura (0 y 1)
   glUniform1i( loc_tex0, 0 );
   glUniform1i( loc_tex1, 1 );
   CError();

   // asignar los pesos de las texturas a los parámetros del shader
   glUniform1f( loc_w0, w0 );
   glUniform1f( loc_w1, w1 );
   CError();

   // Dibujar un rectángulo que ocupe todo el viewport, pero sobre este framebuffer
   // Se desactiva el test de profundidad para asegurarnos que se dibuja el rectángulo
   // (será más eficiente esto que limpiar el depth buffer)
   glViewport( 0, 0, ancho, alto );
   glDisable( GL_DEPTH_TEST ); 
   VisualizarCuadrado();

   CError();
}
// ------------------------------------------------------------------------------

// texto fuente de los shaders usados para visualizar un framebuffer

// fuente del vertex shader

static const std::string src_vs_visu_fb = R"glsl(
#version 330 core

   layout (location=0) in vec2 in_pos ;  // atributo de entrada: posición del vértice
   layout (location=1) in vec2 in_cct ;  // atributo de entrada: coordenadas de textura
   out vec2 v_cct ;  // variable de salida: coordenadas de textura
   
   void main() 
   {  gl_Position = vec4( in_pos, 0.0, 1.0 );  // pasar la posición con Z=0
      v_cct       = in_cct ;                   // pasar las coordenadas de textura
   }
)glsl" ;

// fuente del fragment shader

static const std::string src_fs_visu_fb =  R"glsl(
#version 330 core 

   uniform sampler2D tex ;        // sampler de textura
   in vec2           v_cct ;      // coordenadas de textura interpoladas al centro del pixel. 
   layout(location=0) out vec4 out_color ; //  variable de salida: color del pixel.
   
   void main()  
   {  
      ivec2 tam  = textureSize( tex, 0 ) ;      // leer tamaño de la textura (número de filas y columnas), lod=0 
      ivec2 ipos = ivec2( vec2(tam)*v_cct );    // posición del pixel en coordenadas enteras (numero de fila y columna) 
      vec3  t    = texelFetch( tex, ipos, 0 ).rgb; // consulta texel de la textura 0 (sin filtrar, lod=0) 
      out_color  = vec4( t, 1.0f ) ;                        // producir el color del texel directamente en la salida.
   }
)glsl" ;

// ------------------------------------------------------------------------------

void Framebuffer::visualizar( const unsigned x0, const unsigned y0, const unsigned ancho, const unsigned alto)
{
   CError();

   assert( fboId > 0 );
   assert( textId > 0 );

   static GLuint id_prog =  0 ;
   if ( id_prog == 0 )
   { 
      id_prog = CrearObjetoPrograma( src_vs_visu_fb, src_fs_visu_fb );
      glUseProgram( id_prog );
      GLint loc_tex = glGetUniformLocation( id_prog, "tex" );  assert( loc_tex >= 0 ); // ¿necesario?
      glUniform1i( loc_tex, 0 ); // ¿necesario? (no, solo hay un sampler, está ligado a la unidad 0)
      CError();
   }
   else 
      glUseProgram( id_prog );

   
   glActiveTexture( GL_TEXTURE0 ); 
   glBindTexture( GL_TEXTURE_2D, textId );
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
   glViewport( x0, y0, ancho, alto );
   glClear( GL_DEPTH_BUFFER_BIT );
   CError();

   VisualizarCuadrado();
}

// ------------------------------------------------------------------------------

void Framebuffer::comprobarEstado()
{
   using namespace std ;

   GLenum status;
   bool ok = false ;
   glBindFramebuffer( GL_FRAMEBUFFER, fboId ) ;
   CError();
   status = (GLenum) glCheckFramebufferStatus( GL_FRAMEBUFFER );

   switch( status )
   {
      case GL_FRAMEBUFFER_COMPLETE:
         ok = true ;
         break;

      case GL_FRAMEBUFFER_UNSUPPORTED:
         cout << "Unsupported framebuffer format" << endl ;
         break;

      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
         cout << "Framebuffer incomplete, missing attachment" << endl ;
         break;

      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
         cout << "Framebuffer incomplete, missing draw buffer" << endl ;
         break;

      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
         cout << "Framebuffer incomplete, missing read buffer" << endl ;
         break;

      #ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT
      case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT:
         cout << "Framebuffer incomplete, duplicate attachment" << endl ;
         break;
      #endif

      #ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
         cout << "Framebuffer incomplete, attached images must have same dimensions" << endl ;
         break;
      #endif

      #ifdef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
      case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
         cout << "Framebuffer incomplete, attached images must have same format" << endl ;
         break;
      #endif

      default:
         cout << "unknown framebuffer status (?)" << endl ;
         break ;
   }

	if ( ! ok )
	{
      cout << "unable to initialize FBO due to reason above." << endl ;
	   exit(1);
	}
}
