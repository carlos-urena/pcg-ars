#version 330 core


// *********************************************************************
//
// fragment shader sencillo
//
// *********************************************************************

// ver:
//
// buen ejemplo aquí:
// http://joshbeam.com/articles/getting_started_with_glsl/
//
// para ver como hacer bind de una (o varias) u_texturas:
// https://www.opengl.org/wiki/Sampler_(GLSL)#Language_Definition


// 1. matrices de transformación y proyección
uniform mat4  u_mat_modelado ;    // matriz de modelado actual
uniform mat4  u_mat_vista ;       // matriz de vista (mundo --> camara)
uniform mat4  u_mat_proyeccion ;  // matriz de proyeccion

// 2. parámetros relativos a texturas
uniform bool  u_eval_text ;       // false --> no evaluar texturas, true -> evaluar textura en FS, sustituye a (v_color)
uniform int   u_tipo_gct ;        // tipo gen.cc.tt. (0=desact, 1=objeto, 2=camara)
uniform vec4  u_coefs_s ;         // coefficientes para G.CC.TT. (coordenada 's')
uniform vec4  u_coefs_t ;         // coefficientes para G.CC.TT. (coordenada 't')

// 3. 'sampler' de textura
uniform sampler2D u_tex ;         // al ser el primer 'sampler', está ligado a la unidad 0 de texturas

// --------------------------------------------------------------------
// Parámetros varying ( 'in' aquí, 'out' en el vertex shader)

in vec4 v_posicion_wcc ; // posición del vértice en coords. de mundo 
in vec4 v_color ;        // color del vértice 
in vec2 v_coord_text ;   // coordenadas de textura

// --------------------------------------------------------------------
// Parámetros de salida 

layout(location = 0) out vec4 out_color_fragmento ; // color que se calcula como resultado final de este shader en 'main'
//out vec4 out_color_fragmento ; // color que se calcula como resultado final de este shader en 'main'

// -----------------------------------------------------------------------------------------------
// Función principal (escribe en 'out_color_fragment', que es la variable de salida.

void main()
{
   // consultar color del objeto en el centro del pixel ('color_obj')
   vec4 color_obj ;
   if ( u_eval_text  ) // si hay textura:
      color_obj = texture( u_tex, v_coord_text );  // es el color de la textura en las coordenadas de textura actuales
   else  // si no hay textura:
      color_obj = v_color ; // no hacer nada, simplemente usar color de entrada
 
   out_color_fragmento = color_obj ; // el color del pixel es el resultado de evaluar iluminación
}
