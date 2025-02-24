#version 410 core 

// Geometry shader que recibe triangulos y emite triangulos en las aristas de los originales
// (con un grosor dado a través de un uniform?)
// Ver: https://www.khronos.org/opengl/wiki/Geometry_Shader

// --------------------------------------------------------------------------------------------
// Especificación del tipo de primitivas de entrada y el tipo (y numero) de primitivas de salida.

// las primitivas que se pueden enviar dependen del 'layout' de entrada 
//   + si es 'lines' se pueden enviar GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP
//   + si es 'triangles' se pueden enviar GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN

layout ( triangles ) in ; 
layout ( triangle_strip, max_vertices = 8 ) out ; // se produce una tira con 8 vértices como mucho (6 triángulos)

// --------------------------------------------------------------------------------------------
// Uniforms usados en el geometry shader (volvemos a aplicar la matriz de proyección a las coords de vista)

uniform mat4 u_mat_proyeccion ;  // matriz de proyeccion
uniform bool u_activar_gs ; // si es true se activa el geometry shader, en otro caso no se activa (se hace 'passthrough')

// -------------------------------------------------------------------------------------
// Variables de entrada desde el tessellation evaluation shader

in vec4 v3_posic_ecc[] ;   // posicion del punto (en coords de camara)
in vec4 v3_color[] ;       // color del vértice (interpolado a los pixels)
in vec3 v3_normal_ecc[] ;  // normal  (en coords. de cámara)
in vec2 v3_coord_text[] ;  // coordenadas de textura
in vec3 v3_vec_obs_ecc[] ; // vector hacia el observador (en coords de cámara)

// -------------------------------------------------------------------------------------
// Variables de salida hacia el fragment shader 

out vec4 v4_posic_ecc ;    // posicion del punto (en coords de camara)
out vec4 v4_color ;        // color del vértice (interpolado a los pixels)
out vec3 v4_normal_ecc ;   // normal  (en coords. de cámara)
out vec2 v4_coord_text ;   // coordenadas de textura
out vec3 v4_vec_obs_ecc ;  // vector hacia el observador (en coords de cámara)

// -----------------------------------------------------------------------------------------
// Esta función simplemente copia las entradas en las salidas (equivale a desactivar el geometry shader)

void Passthrough()
{
    // Recorrer los 3 vértices del triángulo de entrada
    // gl_in.length() == 3 en este caso (solo se envían triángulos)

    for( int i=0 ; i < 3 ; i++ )  
    {
        // copiar variables de entrada de este vértice en las variables de salida
        v4_posic_ecc   = v3_posic_ecc[i] ;
        v4_color       = v3_color[i] ;
        v4_normal_ecc  = v3_normal_ecc[i] ;
        v4_coord_text  = v3_coord_text[i] ;
        v4_vec_obs_ecc = v3_vec_obs_ecc[i] ;
        gl_Position    = gl_in[i].gl_Position ;

        EmitVertex() ;
    }
    EndPrimitive() ;
}

// -----------------------------------------------------------------------------------------
// Esta función produce pares de triángulos en cada arista del triángulo de entrada

void TriangulosEnAristas()
{
    // calcular el centro y los tres vectores desde el centro a los vértices
    vec3 centro = (v3_posic_ecc[0].xyz + v3_posic_ecc[1].xyz + v3_posic_ecc[2].xyz) / 3.0 ;
    
    const float g = 0.05 ; // grosor relativo de las ariastas

    // emitir una tira con 6 triángulos (2 triángulos por arista)
    // para eso se emiten 8 vértices (2 por iteración del bucle)
    //   + primero dos vértices iniciales 
    //   + luego cada uno de los 6 restantes cierra un triángulo
    
    for( int i = 0 ; i < 4 ; i++ )  // en cada iteración se emiten dos vértices
    {
        // j == indice del vértice original en la entrada
        int j = ( i < 3 ) ? i : 0 ;

        // calcular el vértice externo (original) y el interno (nuevo)
        vec4 posic_ecc_ext = v3_posic_ecc[j] ;
        vec4 posic_ecc_int = vec4( centro + (1.0-g)*(v3_posic_ecc[j].xyz - centro), 1.0 ); 
        
        // copiar las variables de entrada en las de salida para los dos vértices.
        v4_color       = v3_color[j] ;
        v4_normal_ecc  = v3_normal_ecc[j] ;
        v4_coord_text  = v3_coord_text[j] ;
        v4_vec_obs_ecc = v3_vec_obs_ecc[j] ;

        // emitir vértice interno (nuevo)
        v4_posic_ecc   = posic_ecc_int ;
        gl_Position    = u_mat_proyeccion * posic_ecc_int ; 
        EmitVertex() ;

        // emitir vértice externo (original)
        v4_posic_ecc   = posic_ecc_ext ;
        gl_Position    = u_mat_proyeccion * posic_ecc_ext ;
        EmitVertex() ;
    }
    EndPrimitive() ;
    
}

// --------------------------------------------------------------------------------------------
// 'main' se invoca una vez por cada triángulo de entrada

void main()
{
    if ( u_activar_gs )
        TriangulosEnAristas();
    else 
        Passthrough();
}


