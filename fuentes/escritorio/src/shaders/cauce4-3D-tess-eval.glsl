#version 410 core

// *********************************************************************
// Tesselation Evaluation Shader: 
// 
// This shader is called after the tessellation control shader has
// executed. It is responsible for evaluating the tessellation and
// generating the vertex data for the tessellated vertices.
//
// https://www.khronos.org/opengl/wiki/Tessellation_Control_Shader
// https://ogldev.org/www/tutorial30/tutorial30.html
//
// --------------------------------------------------------------------

layout(triangles, equal_spacing, ccw) in;

// -------------------------------------------------------------------------------------
// Variables de entrada desde el Tesselation Control Shader:
// se usa el prefijo v2_ para indicar que son varying de entrada desde TCS con destino a este TES
// son arrays de 3 valores (por que ese es el size del input patch), correspondientes a los valores del triángulo donde está este vértice
//

in vec4 v2_posic_ecc[] ;   // posicion del punto (en coords de camara)
in vec4 v2_color[] ;       // color del vértice (interpolado a los pixels)
in vec3 v2_normal_ecc[] ;   // normal  (en coords. de cámara)
in vec2 v2_coord_text[] ;   // coordenadas de textura
in vec3 v2_vec_obs_ecc[] ; // vector hacia el observador (en coords de cámara)

// Variables de entrada predefinidas:
// in vec3 glTessCoord ; // coordenadas baricentricas del punto a evaluar (esas coordenadas son los pesos relativo de los 3 vértices que definen el triángulo donde está este punto)

// -------------------------------------------------------------------------------------
// Variables de salida hacia el Fragment Shader o el Geometry Shader:
// se usa el prefijo v3_ para indicar que son varying de salida del TES hacia el FS o el GS
//

out vec4 v3_posic_ecc ;   // posicion del punto (en coords de camara)
out vec4 v3_color ;       // color del vértice (interpolado a los pixels)
out vec3 v3_normal_ecc ;   // normal  (en coords. de cámara)
out vec2 v3_coord_text ;   // coordenadas de textura
out vec3 v3_vec_obs_ecc ; // vector hacia el observador (en coords de cámara)

// -------------------------------------------------------------------------------------
// funciones de interpolación para vectores de 2 floats, 3 floats, y 4 floats
// usan 'glTessCoord', que contiene las coordenadas baricentricas del punto a evaluar

vec2 interpolar_vec2( vec2 v0, vec2 v1, vec2 v2 )
{
    return gl_TessCoord[0]*v0 + gl_TessCoord[1]*v1 + gl_TessCoord[2]*v2;
}

vec3 interpolar_vec3( vec3 v0, vec3 v1, vec3 v2 )
{
    return gl_TessCoord[0]*v0 + gl_TessCoord[1]*v1 + gl_TessCoord[2]*v2;
}

vec4 interpolar_vec4( vec4 v0, vec4 v1, vec4 v2 )
{
    return gl_TessCoord[0]*v0 + gl_TessCoord[1]*v1 + gl_TessCoord[2]*v2;
}



// -------------------------------------------------------------------------------------

void main()
{

   // Generar las variables o atributos v3_ para este vértice:
   // para ello se interpolando a partir de los v2_ y las coordenadas baricentricas

   v3_posic_ecc   = interpolar_vec4( v2_posic_ecc[0], v2_posic_ecc[1], v2_posic_ecc[2] );
   v3_color       = interpolar_vec4( v2_color[0], v2_color[1], v2_color[2] );
   v3_normal_ecc  = interpolar_vec3( v2_normal_ecc[0], v2_normal_ecc[1], v2_normal_ecc[2] );
   v3_coord_text  = interpolar_vec2( v2_coord_text[0], v2_coord_text[1], v2_coord_text[2] );
   v3_vec_obs_ecc = interpolar_vec3( v2_vec_obs_ecc[0], v2_vec_obs_ecc[1], v2_vec_obs_ecc[2] );

   // Generar la posición de salida del vértice (en coords de recortado)
   // Para ello usamos:
   //    +  los tres vértices del parche de entrada (en gl_in[].gl_Position) 
   //    +  los pesos (coords. baricéntricas en 'gl_TessCoord') para interpolarlos

   gl_Position = gl_in[0].gl_Position*gl_TessCoord[0] +
                 gl_in[1].gl_Position*gl_TessCoord[1] +
                 gl_in[2].gl_Position*gl_TessCoord[2] ;    
}