#version 300 es

precision highp float ; // debe coincidir con el FS
precision highp int ; // necesario para dispositivo virtual "Medium API 35" en macOs ¿?


// 1. tipo de normales para iluminación
uniform bool  u_usar_normales_tri ;     // true --> normal triángulo, true --> normal interpolada de los vértices

// uniforms: matrices

uniform mat4  u_mat_modelado ;    // matriz de modelado actual
uniform mat4  u_mat_modelado_nor; // matriz de modelado para normales (traspuesta inversa de la anterior)
uniform mat4  u_mat_vista ;       // matriz de vista (mundo --> camara)
uniform mat4  u_mat_proyeccion ;  // matriz de proyeccion

// uniforms: parámetros relativos a texturas

uniform bool  u_eval_text ;       // false --> no evaluar texturas, true -> evaluar textura en FS, sustituye a (v_color)
uniform int   u_tipo_gct ;        // tipo gen.cc.tt. (0=desact, 1=objeto, 2=camara)
uniform vec4  u_coefs_s ;         // coefficientes para G.CC.TT. (coordenada 's')
uniform vec4  u_coefs_t ;         // coefficientes para G.CC.TT. (coordenada 't')

// atributos de vértice

in vec3 in_posicion_occ;   // 0 -> posicion (en coordenadas de objeto)
in vec3 in_color;          // 1 -> color
in vec3 in_normal_occ;     // 2 -> normal (en coordenadas de objeto)
in vec2 in_coords_textura; // 3 -> coordenadas de textura

// variables de salida hacia el fragment shader

out vec4 v_posic_ecc ;   // posicion del punto (en coords de camara)
out vec4 v_color ;       // color del vértice (interpolado a los pixels)
out vec3 v_normal_ecc;   // normal  (en coords. de cámara)
out vec2 v_coord_text;   // coordenadas de textura
out vec3 v_vec_obs_ecc ; // vector hacia el observador (en coords de cámara)

// parámetro S
uniform float u_param_s ;         // parámetro S

// ------------------------------------------------------------------------------
// calculo de los parámetros de salida (io_... y gl_Position)

vec2 CoordsTextura() // calcula las coordenadas de textura
{
    if ( ! u_eval_text )            // si no se están evaluando las coordenadas de textura
        return vec2( 0.0, 0.0 );     //     devuelve las coordenadas (0.0,0.0)
    if ( u_tipo_gct == 0 )          // texturas activadas, generacion desactivada
        return in_coords_textura.st; //     devuelve las coords. de glTexCoords o tabla

    vec4 pos_ver ;
    if ( u_tipo_gct == 1 )                       // generacion en coordenadas de objeto
        pos_ver = vec4( in_posicion_occ, 1.0  ) ; //    usar las coords originales (objeto)
    else                                         // generacion en coords de cámara
        pos_ver = v_posic_ecc ;                   //    usar las coordenadas de cámara

    return vec2( dot(pos_ver,u_coefs_s), dot(pos_ver,u_coefs_t) );
}

// ------------------------------------------------------------------------------
// devuelve el vector al obervador, a partir de pos. ECC, y en función de
// si la matriz de proyec. es ortogonal (a==0), o es perspectiva (a==-1)

vec3 VectorObservadorVS( vec4 pos_ecc )
{
    float pm23 = u_mat_proyeccion[2][3],   // -1 si es perspectiva, 0 si es ortográfica.
    pm33 = u_mat_proyeccion[3][3];   // 0 si es perspectiva,  1 si es ortográfica.

    // código "largo" (para explicar lo que se hace)
    //
    // if ( pm23 == 0 &&  pm33 == 1.0 )    // si proyección es ortográfica
    //    return vec3( 0.0, 0.0, 1.0 );  //    devolver +Z
    // else                              // si es perspectiva
    //    return (-pos_ecc).xyz ;        //    devolver vector hacia el foco de la cámara (origen en ECC)

    // código "corto" (eficiente), equivalente
    return pm23*(pos_ecc.xyz) + vec3(0.0, 0.0, pm33 );
}

void main()
{
    vec4 posic_wcc  = u_mat_modelado * vec4( in_posicion_occ, 1.0 ) ; // posición del vértice en coords. de mundo
    vec3 normal_wcc = (u_mat_modelado_nor * vec4(in_normal_occ,0)).xyz ;

    // calcular las variables de salida

    v_posic_ecc    = u_mat_vista*posic_wcc ;
    v_normal_ecc   = (u_mat_vista*vec4(normal_wcc,0.0)).xyz ;
    v_vec_obs_ecc  = VectorObservadorVS( v_posic_ecc );  // ver la función arriba
    v_color        = vec4( in_color, 1 ) ;  // color fijado con in_color .....
    v_coord_text   = CoordsTextura();

    vec4 pos       = u_mat_proyeccion * v_posic_ecc ;
    gl_Position    = pos ;
}