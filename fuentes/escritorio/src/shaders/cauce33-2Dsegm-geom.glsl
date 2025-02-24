#version 330 core 

// Geometry shader que recibe línea o polilíneas y produce triangulos que forman las polilíneas 
// con un grosor dado a través de un uniform

// --------------------------------------------------------------------------------------------
// Especificación del tipo de primitivas de entrada y el tipo (y numero) de primitivas de salida.


layout( lines ) in ; // se pueden enviar GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP
layout( triangle_strip, max_vertices = 80) out ; // se producen tiras de triángulos

const int max_vertices = 76 ; // número máximo de vértices de salida (debe coincidir con el de arriba!)

// entradas (uniform)
uniform mat4  u_mat_modelado ;       // matriz de modelado actual
uniform mat4  u_mat_vista ;          // matriz de vista (mundo --> camara)
uniform mat4  u_mat_proyeccion ;     // matriz de proyeccion
uniform float u_grosor_lineas_wcc ;  // ancho o grosor de las líneas,  en WCC
uniform float u_radio_puntos_wcc ;   // radio de los discos en los puntos, en WCC
uniform bool  u_visualizar_puntos ;  // visualizar puntos sí (true) o no (false) 
uniform bool  u_visualizar_lineas ;  // visualizar líneas sí (true) o no (false)

// entradas (distintas en cada vértice), adicionales a la posición
in vec4 v1_color[] ;        // color del vértice 

// salida (distinta en cada vértice)
out vec4 v2_color ;        // color del vértice 

// ----------------------------------------------------------------------------
// Emite un vértice con la posición 'pos_wcc'
//
void Vertice( vec4 pos_wcc )
{
    gl_Position = u_mat_proyeccion*pos_wcc ;
    EmitVertex() ;
}
// ----------------------------------------------------------------------------
// Emite las primitivas (triángulos) que forman un disco de radio w/2 centrado en centro

void EmitirPrimDisco( vec4 centro, vec4 color )
{
    const int   num_t  = ((max_vertices-4)/2)/3 ;
    const float angulo = 2.0f * 3.14159265f / float(num_t) ;
    float       radio  = u_radio_puntos_wcc ;

    v2_color = color ;

    vec4 vert_ant  = centro + vec4( radio, 0.0f, 0.0f, 0.0f ) ;
    
    for( int i = 1 ; i <= num_t ; i++ )
    {
        float a        = float(i) * angulo ;
        vec4  vert_nue = centro + vec4( radio*cos(a), radio*sin(a), 0.0f, 0.0f ) ;
        
        Vertice( centro );  
        Vertice( vert_ant ); 
        Vertice( vert_nue );
        EndPrimitive();

        vert_ant = vert_nue ;
    }
}

// ----------------------------------------------------------------------------
// Emite la primitiva (una tira de dos triángulos) que forma un segmento de recta de grosor w/2
// desde v0 hasta v1, con colores c0 en v0 y c1 en v1 
// (los colores se interpolan linealmente en el interior de la primitiva) 
//
void EmitirPrimSegmento( vec4 v0, vec4 v1, vec4 c0, vec4 c1 )
{
    vec4 s  = normalize( v1 - v0 ); // vector director del segmento
    vec4 n  = (u_grosor_lineas_wcc/2.0f)*vec4( -s.y, s.x, 0.0f, 0.0f ); // vector normal al segmento de long w/2

    v2_color = c0 ; Vertice( v0 - n ); Vertice( v0 + n );  
    v2_color = c1 ; Vertice( v1 - n ); Vertice( v1 + n );  

    EndPrimitive();
}

// ----------------------------------------------------------------------------

void SegmentoDiscos() 
{
    vec4 
        v0 = gl_in[0].gl_Position,
        c0 = v1_color[0] ,
        v1 = gl_in[1].gl_Position,
        c1 = v1_color[1] ,
        dz = vec4( 0.0f, 0.0f, -0.05f, 0.0f ) ; // desplazamiento en z para evitar z-fighting
    
    if ( u_visualizar_lineas )
        EmitirPrimSegmento( v0+dz, v1+dz, c0, c1 );
    
    if ( u_visualizar_puntos ) 
    {
        EmitirPrimDisco( v0+2.0*dz, c0 ); 
        EmitirPrimDisco( v1+2.0*dz, c1 );
    }
}

// --------------------------------------------------------------------------------------------

void main()
{
    SegmentoDiscos();
}


