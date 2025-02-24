#version 330 core



// buen ejemplo aquí:
// http://joshbeam.com/articles/getting_started_with_glsl/
//
// para ver como hacer bind de una (o varias) texturas:
// https://www.opengl.org/wiki/Sampler_(GLSL)#Language_Definition


// Número máximo de fuentes de luz que el programa puede soportar
const int max_num_luces = 8 ;

// --------------------------------------------------------------------
// Parámetros de entrada uniform (constantes en cada primitiva)
// (iguales en fragment y vertex shader)


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
uniform sampler2D u_tex ;         // al ser el primer 'sampler', está ligado a la unidad 0 de u_texturas


// Valores de entrada (atributos de vértices, valores distintos para cada vértice)
// (los enteros que definen las 'locations' deben ser coherentes con el código de la app)

layout( location = 0 ) in vec3 in_posicion_occ ;   // posición del vértice en coordenadas de objeto
layout( location = 1 ) in vec3 in_color ;          // color del vértice
layout( location = 2 ) in vec2 in_coords_textura ; // coordenadas de textura del vértice 

// Valores calculados como salida ('out' aquí, 'in' en el fragment shader, distintos de cada vértice)

out vec4 v_posicion_wcc ; // posición del vértice en coords. de mundo 
out vec4 v_color ;        // color del vértice 
out vec2 v_coord_text ;   // coordenadas de textura

// ------------------------------------------------------------------------------

vec2 CoordsTextura() // calcula las coordenadas de textura
{
   if ( ! u_eval_text )            // si no se están evaluando las coordenadas de textura
      return vec2( 0.0, 0.0 );     //     devuelve las coordenadas (0.0,0.0)
   if ( u_tipo_gct == 0 )          // texturas activadas, generacion desactivada
      return in_coords_textura.st; //     devuelve las coords. de glTexCoords o tabla

   vec4 pos_ver ;
   if ( u_tipo_gct == 1 )            // generacion en coordenadas de objeto
      pos_ver = vec4( in_posicion_occ, 1.0 ) ;        //    usar las coords originales (objeto)
   else                              // generacion en coords del mundo
      pos_ver = v_posicion_wcc ;     //    usar las coordenadas del mundo

   return vec2( dot(pos_ver,u_coefs_s), dot(pos_ver,u_coefs_t) );
}
// ------------------------------------------------------------------------------

void main()
{
   v_posicion_wcc = u_mat_modelado * vec4( in_posicion_occ, 1.0 ) ; // posición del vértice en coords. de mundo
   v_color        = vec4( in_color, 1 ) ;
   v_coord_text   = CoordsTextura() ; //puede leer v_posicion_wcc
   gl_Position    = u_mat_proyeccion * u_mat_vista * v_posicion_wcc ; 
}
