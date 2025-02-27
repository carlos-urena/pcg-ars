#version 300 es

precision highp float ;
precision highp int ;

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

// --------------------------------------------------------------------
// Constantes

// Número máximo de fuentes de luz que el programa puede soportar
const int max_num_luces = 8 ;

// --------------------------------------------------------------------
// Parámetros de entrada uniform (constantes en cada primitiva)
// (iguales en fragment y vertex shader)

// 1. tipo de normales para iluminación
uniform bool  u_usar_normales_tri ;     // true --> normal triángulo, true --> normal interpolada de los vértices

// 2. matrices de transformación y proyección
uniform mat4  u_mat_modelado ;    // matriz de modelado actual
uniform mat4  u_mat_modelado_nor; // matriz de modelado para normales (traspuesta inversa de la anterior)
uniform mat4  u_mat_vista ;       // matriz de vista (mundo --> camara)
uniform mat4  u_mat_proyeccion ;  // matriz de proyeccion

// 3. parámetros relativos a texturas
uniform bool  u_eval_text ;       // false --> no evaluar texturas, true -> evaluar textura en FS, sustituye a (v_color)
uniform int   u_tipo_gct ;        // tipo gen.cc.tt. (0=desact, 1=objeto, 2=camara)
uniform vec4  u_coefs_s ;         // coefficientes para G.CC.TT. (coordenada 's')
uniform vec4  u_coefs_t ;         // coefficientes para G.CC.TT. (coordenada 't')

// 4. parámetros relativos evaluación de iluminación
uniform bool  u_eval_mil ;        // evaluar el MIL sí (true) o no (false) --> si es que no, usar color plano actual
uniform float u_mil_ka ;          // color de la componente ambiental del MIL (Ka)
uniform float u_mil_kd ;          // color de la componente difusa del MIL (Kd)
uniform float u_mil_ks ;          // color de la componente pseudo-especular del MIL (Ks)
uniform float u_mil_exp ;         // exponente de la componente pseudo-especular del MIL (e)

// 5. parámetros de las fuentes de luces actuales
uniform int   u_num_luces ;                     // número de luces activas, si u_eval_mil == true
uniform vec4  u_pos_dir_luz_ec[max_num_luces] ; // posición/dirección de cada luz
uniform vec3  u_color_luz[max_num_luces] ;      // color o intensidad de cada fuente de luz

// 6. 'sampler' de textura
uniform sampler2D u_tex ;         // al ser el primer 'sampler', está ligado a la unidad 0 de texturas

// 7. parámetro S 
uniform float u_param_s ;         // parámetro S 

// 8. sombras (pruebas)
uniform sampler2D u_tex_sombras ;  // segundo sampler, sombras ¿ ligado a la unidad 1 ?
uniform bool      u_eval_sombras ; // true --> usar sombras, false --> no usar sombras
uniform mat4      u_mat_vp_sombras ; // matriz de vista+proyección para sombras, pasa de WCC a coordenadas de de la cámara que se usa para sombras


// --------------------------------------------------------------------
// Parámetros varying ( 'in' aquí, 'out' en el vertex shader)

in vec4 v_posic_wcc ;   // posición del punto (en coords de mundo)
in vec4 v_posic_ecc ;   // posicion del punto (en coords de camara)
in vec4 v_color ;       // color del vértice (interpolado a los pixels)
in vec3 v_normal_ecc;   // normal  (en coords. de camara)
in vec2 v_coord_text;   // coordenadas de textura
in vec3 v_vec_obs_ecc ; // vector hacia el observador (en coords de cámara)

// --------------------------------------------------------------------
// Parámetros de salida 

layout(location = 0) out vec4 out_color_fragmento ; // color que se calcula como resultado final de este shader en 'main'
//out vec4 out_color_fragmento ; // color que se calcula como resultado final de este shader en 'main'

// -----------------------------------------------------------------------------------------------
// Calcula el vector normalizado hacia el observador

vec3 VectorHaciaObsEC()
{
   return normalize( v_vec_obs_ecc );
}
// -----------------------------------------------------------------------------------------------
// Calcula la normal al triángulo en coordenadas de cámara (usa la tangente y la bi-tangente, 
// en el plano del triángulo) 
// Las tangentes se calculan como las derivadas en X y en X de la posición en coordenadas de cámara
//
vec3 NormalTrianguloEC()
{
   return normalize( cross( dFdx( v_posic_ecc.xyz ), dFdy( v_posic_ecc.xyz ) ) );   
}
// -----------------------------------------------------------------------------------------------
// Calcula la normal al triángulo en coordenadas de mundo (usa la tangente y la bi-tangente)
// en el plano del triángulo) 
// Las tangentes se calculan como las derivadas en X y en X de la posición en coordenadas de cámara
//
vec3 NormalTrianguloWCC()
{
   return normalize( cross( dFdx( v_posic_wcc.xyz ), dFdy( v_posic_wcc.xyz ) ) );   
}
// -----------------------------------------------------------------------------------------------
// Calcula el vector normal en coordenadas de cámara, normalizado
// (si u_usar_normales_tri está activado, se usa la normal del triángulo, igual en todos los pixels)
// en cualquier caso devuelve la normal que apunta hacia el observador, segun 'vec_obs'
//
vec3 NormalEC( vec3 vec_obs_ecc )
{
   vec3 n = u_usar_normales_tri ? NormalTrianguloEC() 
                                : normalize( v_normal_ecc );

   return dot(n,vec_obs_ecc) >= 0.0  ? n : -n ;
}
// -----------------------------------------------------------------------------------------------
// vector normalizado en la dirección de la i-ésima fuente de luz

vec3 VectorHaciaFuenteEC( int i )
{
   if ( u_pos_dir_luz_ec[i].w == 1.0  )
      return normalize( u_pos_dir_luz_ec[i].xyz - v_posic_ecc.xyz );
   else
      return normalize( u_pos_dir_luz_ec[i].xyz );
}
// -----------------------------------------------------------------------------------------------
// Devuelve un número entre -1 y +1 codificado en un color RGB con componentes entre 0 y 1

float DecodificarDeRGB( vec3 rgb )
{
   float x = rgb.r + rgb.g/256.0 + rgb.b/(256.0*256.0) ;
   return 2.0*x-1.0;
}

// -----------------------------------------------------------------------------------------------
// consulta si el punto central al pixel está en una sombra arrojada 

bool EnSombraArrojada()
{
   // calcular coordenadas del punto en el marco de coordenadas de la fuente.
   vec4 posic_fcc = u_mat_vp_sombras * v_posic_wcc ;

   // calcular las coordenadas de textura enteras, entre 0 y el tamaño del FBO de sombras  
   ivec2 csi = ivec2( int( posic_fcc.x), int( posic_fcc.y) ) ;

   // recupar el RGB que codifica la coordenada Z
   vec4 rgb = texelFetch( u_tex_sombras, csi, 0 ) ;

   // decodificar el valor de Z más cercano a la fuente
   float z_mas_cercana = DecodificarDeRGB( rgb.rgb ) ;

   // si está más allá de la Z más cercana, está en sombras
   if ( posic_fcc.z > z_mas_cercana+0.01 )
      return true ;
   else 
      return false ;

}

float FactorSombraArrojada()
{
   if ( ! u_eval_sombras )
      return 1.0 ;

   // calcular coordenadas del punto en el marco de coordenadas de la fuente.
   vec4 posic_fcc = u_mat_vp_sombras * v_posic_wcc ;

   // calcular las coordenadas de textura enteras, entre 0 y el tamaño del FBO de sombras  
   ivec2 csi = ivec2( int( posic_fcc.x), int( posic_fcc.y) ) ;

   
   float suma = 0.0 ;
   const int ns_root = 9 ;  
   const int c = ns_root/2 ;
   
   for( int i = 0 ; i < ns_root ; i++ )
   for( int j = 0 ; j < ns_root ; j++ )
   {
      vec4 rgb = texelFetch( u_tex_sombras, csi + ivec2( i-c, j-c ), 0 ) ;
      float z = DecodificarDeRGB( rgb.rgb ) ;
      if ( posic_fcc.z > z+0.01 )
         suma += 1.0 ;
   }
   return 1.0-suma/float(ns_root*ns_root) ;

}
// -----------------------------------------------------------------------------------------------
// Función que evalúa un MIL sencillo
// color_obj  == color del objeto en el punto central al pixel

vec3 EvalMIL(  vec3 color_obj )
{
   vec3  v = VectorHaciaObsEC() ;  // vector hacia el observador (en ECC), normalizado
   vec3  n = NormalEC( v );        // vector normal (en ECC), normalizado, apuntando hacia el lado de 'v'
   vec3  col_suma = vec3( 0.0, 0.0, 0.0 ); // suma de los colores debidos a cada fuente de luz
   //bool sa = EnSombraArrojada() ; // evaluar si está en sombra arrojada

   // para cada fuente de luz, sumar en 's' el color debido a esa fuente:
   for( int i = 0 ; i < u_num_luces ; i++ )
   {
      
      // sumar la componente ambiental debida a esta fuente de luz
      col_suma = col_suma + u_color_luz[i]*color_obj*u_mil_ka ;

      // si la normal apunta al hemisferio de la fuente, añadir
      // componentes difusa y especular

      vec3  l   = VectorHaciaFuenteEC( i ) ;
      float nl  = dot( n, l ) ;

      if ( 0.03 < nl )
      {  
         // si es la fuente 0 y está en sombra arrojada, no hacer nada más
         // if ( i == 0 )
         //    if ( EnSombraArrojada() )
         //       continue ;

         float fs = FactorSombraArrojada() ;

         float hn  = max( 0.0, dot( n, normalize( l+v ) ));
         vec3  col = color_obj*(u_mil_kd*nl) + pow(hn,u_mil_exp)*u_mil_ks ;
         col_suma = col_suma + fs * (u_color_luz[i] * col);
      }
      // nota: esto es lo más lógico y parecido al cauce fijo,
      // (causa terminadores de sombra "duros" en la componente pseudo-especular)
      // otra opción es simplemente añadir la componente pseudo-especular aún cuando
      // nl < 0.0, (siempre con hn no-negativo, por supuesto).
   }
   return col_suma ;
}



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
 
   // color calculado según en el shader original.
   vec4 color_calculado  ; 

   // calcular el color del pixel (out_color_fragmento)
   if ( ! u_eval_mil  ) // si está desactivada iluminación:
      color_calculado = color_obj ; // el color del pixel es el color del objeto
   else // si está activada iluminación
      color_calculado = vec4( EvalMIL( color_obj.rgb ), 1.0 ); // el color del pixel es el resultado de evaluar iluminación

   // escribe en la variable de salida
   out_color_fragmento = color_calculado;
}
