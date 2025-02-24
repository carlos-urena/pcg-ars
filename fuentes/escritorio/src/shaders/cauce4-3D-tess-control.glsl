#version 410 core


// *********************************************************************
// Tesselation Control Shader: 
//
// Este shader se invoca una vez por cada punto de control de un patch
// y se encarga de establecer los niveles de teselación para el patch
// y de copiar los valores (atributos de vértices) de entrada a los valores de salida
//
// https://www.khronos.org/opengl/wiki/Tessellation_Control_Shader
// https://ogldev.org/www/tutorial30/tutorial30.html
//
// ---------------------------------------------------------------------

layout( vertices = 3 ) out; // la salida son triangulos

// --------------------------------------------------------------------------------------------
// Uniforms usados en los tesselation shaders

uniform bool u_activar_ts ; // si es true se activa la teselación, en otro caso no se tesela (se hace 'passthrough')

// --------------------------------------------------------------------------------------------
// Valores de entrada desde el Vertex Shader

in vec4 v1_posic_ecc[] ;   // posicion del punto (en coords de camara)
in vec4 v1_color[] ;       // color del vértice (interpolado a los pixels)
in vec3 v1_normal_ecc[] ;   // normal  (en coords. de cámara)
in vec2 v1_coord_text[] ;   // coordenadas de textura
in vec3 v1_vec_obs_ecc[] ; // vector hacia el observador (en coords de cámara)

// --------------------------------------------------------------------------------------------
// Variables de salida hacia el Tesselation Evaluation Shader 

out vec4 v2_posic_ecc[] ;   // posicion del punto (en coords de camara)
out vec4 v2_color[] ;       // color del vértice (interpolado a los pixels)
out vec3 v2_normal_ecc[] ;   // normal  (en coords. de cámara)
out vec2 v2_coord_text[] ;   // coordenadas de textura
out vec3 v2_vec_obs_ecc[] ; // vector hacia el observador (en coords de cámara)

// --------------------------------------------------------------------------------------------

void CopiarAtributos()
{
   v2_posic_ecc[gl_InvocationID]    = v1_posic_ecc[gl_InvocationID] ;
   v2_color[gl_InvocationID]        = v1_color[gl_InvocationID] ;
   v2_normal_ecc[gl_InvocationID]   = v1_normal_ecc[gl_InvocationID] ;
   v2_coord_text[gl_InvocationID]   = v1_coord_text[gl_InvocationID] ;
   v2_vec_obs_ecc[gl_InvocationID]  = v1_vec_obs_ecc[gl_InvocationID] ;
}

// --------------------------------------------------------------------------------------------
// Función para establecer los niveles de teselación 
// para el caso de triángulos se usan tres valores del array 'outer' y un valor del array 'inner'
// los valores del array outer se interpretan como el numero de segmentos en los que se divide cada lado del triángulo externo
// los valores del array inner se interpretan como el numero de triangulos internos que se generan en la teselación

void CalcularNivelesParaTeselacion()
{
   // obtenemos las posiciones de los tres vértices en coordenadas de cámara
   vec3 v0 = v2_posic_ecc[0].xyz ;
   vec3 v1 = v2_posic_ecc[1].xyz ;
   vec3 v2 = v2_posic_ecc[2].xyz ;

   // calculamos las longitudes de los tres lados, divididas por la distancia de su punto medio al observador
   float lon0 = length( v1 - v0 )/ (length(v0+v1)/2.0);
   float lon1 = length( v2 - v1 )/ (length(v1+v2)/2.0);
   float lon2 = length( v0 - v2 )/ (length(v2+v0)/2.0);

   // longitud esperada de los segmentos producidos por la teselación en CC 
   const float lon_esp = 0.2f ; // fijado heuristicamente 

   // el nivel de tes. de aristas se basa en la long. de cada una.
   float lev0 = max( 1.0, lon0/lon_esp );
   float lev1 = max( 1.0, lon1/lon_esp );
   float lev2 = max( 1.0, lon2/lon_esp );

   gl_TessLevelOuter[0] = lev0;
   gl_TessLevelOuter[1] = lev1;
   gl_TessLevelOuter[2] = lev2;
   
   // el nivel de teselación interior se calcula usando los niveles externos
   gl_TessLevelInner[0] = max( 1.0, max( lev0, max( lev1, lev2 ))) ; 
   
}

// --------------------------------------------------------------------------------------------

void CalcularNivelesParaNoTeselacion()
{
   // si está desactivado se usa el nivel de teselación 1 (implica no teselar)
   
   gl_TessLevelOuter[0] = 1.0;
   gl_TessLevelOuter[1] = 1.0;
   gl_TessLevelOuter[2] = 1.0;

   gl_TessLevelInner[0] = 1.0;
}

// --------------------------------------------------------------------------------------------
// 'main' se invoca una vez por cada punto de control del patch

void main(void)
{
   // En 'gl_in' están cada uno de los tres vértices del triángulo de entrada
   // copiamos en 'gl_out' el vértice correspondiente a esta instancia del TCS
   // (hay que hacerlo pues el TCS lee estos puntos de control)
   // (todo esto asume parches de 3 vértices como entrada y como salida)
   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

   // Copiamos las variables de entrada a las de salida, sin transformar
   CopiarAtributos();

   // Calcular los niveles de teselación 'gl_TessLevelOuter' y 'gl_TessLevelInner'
   if ( u_activar_ts )
      CalcularNivelesParaTeselacion();
   else 
      CalcularNivelesParaNoTeselacion();
}