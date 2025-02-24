// *********************************************************************
// **
// ** Informática Gráfica, curso 2020-21
// ** Declaraciones de la clase 
// **
// *********************************************************************


#include <vector>
#include <iostream>
#include <utilidades.h>
#include "tests-glm.h"



void TestsGLM_1()
{
   using namespace glm ;
   using namespace std ;
   
   // La librería GLM incorpora clases para tuplas de valores numéricos (reales en simple o doble precisión y enteros con o sin signo). Cada tupla contiene  unos pocos valores del mismo tipo (entre 2 y 4). Por cada tipo de valores y cada número de valores hay un tipo de tupla distinto (|vec2|,|vec3|,|uvec3|, etc...).

   //Aquí vemos ejemplos de uso de esta

   // adecuadas para coordenadas de puntos, vectores o normales en 3D
   // también para colores (R,G,B)
   vec3  v1 ;  // tuplas de tres valores tipo \verb|float|
   dvec3 v2 ;  // tuplas de tres valores tipo \verb|double|

   // adecuadas para la tabla de caras en mallas indexadas
   uvec3 v3 ;  // tuplas de tres valores tipo \verb|unsigned| 
   uvec4 v4 ;  // tuplas de cuatro valores tipo \verb|unsigned| 

   // adecuadas para puntos o vectores en coordenadas homogéneas
   // también para colores (R,G,B,A)
   vec4  v5 ;  // tuplas de cuatro valores tipo \verb|float|
   dvec4 v6 ;  // tuplas de cuatro valores tipo \verb|double|

   // adecuadas para puntos o vectores en 2D, y coordenadas de textura
   vec2  v7 ;  // tuplas de dos valores tipo \verb|float|
   dvec2 v8 ;  // tuplas de dos valores tipo \verb|double|

   // uso de variables para evitar warnings

   v1 = {1.0,2.0,3.0};  cout << "t1 == " << to_string(v1) << endl ;
   v2 = {1.0,2.0,3.0};  cout << "t2 == " << to_string(v2) << endl ;
   v3 = {1,2,3};        cout << "t3 == " << to_string(v3) << endl ;
   v4 = {1,2,3,4};      cout << "t3 == " << to_string(v3) << endl ;
   

   v5 = {1.0,2.0,3.0,4.0};  cout << "t5 == " << to_string(v5) << endl ;
   v6 = {1.0,2.0,3.0,4.0};  cout << "t5 == " << to_string(v6) << endl ;

   v7 = {1.0,2.0};  cout << "t7 == " << to_string(v7) << endl ;
   v8 = {1.0,2.0};  cout << "t8 == " << to_string(v8) << endl ;


   // Este trozo código válido ilustra las distintas opciones, para creación,
   // consulta y modificación de tuplas:
   
   
   // declaraciones e inicializaciones de tuplas (diversas sintaxis posibles en C++)
   vec3 a( 1.0, 2.0, 3.0 ) ; 
   vec3 b = { 1.0, 2.0, 3.0 } ;
   vec3 c = vec3( 1.0, 2.0, 3.0 ) ;   
   
   // inicialización a partir de un puntero a un array nativo de C/C++
   
   float      arr3f[3] = { 1.0, 2.0, 3.0 } ;
   unsigned   arr3u[3] = { 1, 2, 3 } ;

   vec3   e = make_vec3( arr3f );
   uvec3  f = make_vec3( arr3u );
          

   // accesos de solo lectura, usando su posición o índice en la tupla (0,1,2,...),
   // o bien como una estructura con componentes .x, .y, .z, o bien , o bien .s, .t
   float 
      x1 = a[0], y1 = a[1], z1 = a[2],  // acceso con indices (como array)
      x2 = a.x,  y2 = a.y,  z2 = a.z,   // pensado para coordenadas (.x .y .z .w)
      re = c.r,  gr = c.g,  bl = c.b,   // pensado para colores (.r .g, .b .a)
      es = e.s,  et = e.t,  ep = e.p ;  // pensado para coord.text (.s .t .p .q)
   unsigned 
      u0 = f.x, u1 = f.y, u2 = f.z ; 

   // evita warnings
   cout << x1 << y1 << z1 << x2 << y2 << z2 << re << gr << bl << es << et << ep << u0 << u1 << u2 << endl ;

   // conversiones a punteros
   float *       p1 = value_ptr(a) ; // conv. a puntero de lectura/escritura
   const float * p2 = value_ptr(b) ; // conv. a puntero de solo lectura

   cout << p1 << p2 << endl ;

   // accesos de escritura
   a[0] = x1 ;
   a.x = x2 ;  
   c.g = gr ;

   // escritura en un 'ostream' (cout), convirtiendo a cadena (se escribe como: \verb|(1.0,2.0,3.0)|
   cout << "la tupla 'a' vale: " << to_string(a) << endl ;
}
// --------------------------------------------------------------

void TestsGLM_2()
{
   using namespace std ;
   using namespace glm ;
   
   // En C++ se pueden sobrecargar los operadores binarios y unarios usuales (|+,-,| etc...)
   // para operar sobre las tuplas de valores reales:

   // declaraciones de tuplas y de valores escalares
   vec3  a = {1.0,2.0,3.0}, 
         b = {1.0,2.0,3.0},
         c = {1.0,2.0,3.0} ;

   float s,l ;
   
   // operadores binarios y unarios de asignación/suma/resta/negación
   a = b ;
   a = b+c ;
   a = b-c ;
   a = -b  ;

   // multiplicación y división por un escalar
   a = 3.0f*b ;    // por la izquierda
   a = b*4.56f ;   // por la derecha
   a = b/34.1f ;   // divisón

   // expresiones arbitrarias
   a = 3.0f*b +4.0f*c + 7.0f*c ;

   // otras operaciones con los vectores
   s = dot( a, b );    // producto escalar (usando método dot)
   a = cross( b, c );  // producto vectorial (solo para tuplas de 3 valores)
   l = length( a );    // calcular módulo o longitud de un vector
   a = normalize( b ); // hacer a= copia normalizada de b (a=b/modulo de b)  (b no cambia)

   cout << " s== " << s << ", l == " << l << endl ;
}
// --------------------------------------------------------------

void TestsGLM_3() // matrices
{
   using namespace std ;
   using namespace glm ;

   {
      // matrices cuadradas de 2x2, 3x3 o 4x4, de floats o de doubles (6 tipos)
      
      mat2 m2x2f = mat2(1.0f); // 2x2 floats (nombre alternativo: mat2x2)
      mat3 m3x3f = mat3(1.0f); // 3x3 floats (mat3x3)
      mat4 m4x4f = mat4(1.0f); // 4x4 floats (mat4x4)

      dmat2 m2x2d = dmat2(1.0f); // 2x2 doubles (dmat2x2)
      dmat3 m3x3d = dmat3(1.0f); // 3x3 doubles (dmat3x3)
      dmat4 m4x4d = dmat4(1.0f); // 4x4 doubles (dmat4x4)

      // matrices no cuadradas: [d]matmxn, donde:
      //    - 'm' se sustituye por el número de columnas (2, 3 o 4)
      //    - 'n' se sustituye por el número de filas (2, 3 o 4)
      
      mat2x3 m2x3f = mat2x3(1.0f); mat2x4 m2x4f = mat2x4(1.0f);
      mat3x2 m3x2f = mat3x2(1.0f); mat3x4 m3x4f = mat3x4(1.0f);
      mat4x2 m4x2f = mat4x2(1.0f); mat4x3 m4x3f = mat4x3(1.0f);

       cout << to_string(m2x2f) << endl 
            << to_string(m3x3f) << endl
            << to_string(m4x4f) << endl
            << to_string(m2x2d) << endl 
            << to_string(m3x3d) << endl
            << to_string(m4x4d) << endl
            << to_string(m2x3f) << endl << to_string(m2x4f) << endl 
            << to_string(m3x2f) << endl << to_string(m3x4f) << endl
            << to_string(m4x2f) << endl << to_string(m4x3f) << endl ;
   }
   {
      // inicialización de las matrices 

      mat4 m1 ; // valores inderteminados (no usar)
      mat4 m4a  = mat4( 1.0f );   // diagonal a 1, resto a cero (matriz identidasd)
      mat4 m4b  = mat4( 34.0f );  // diagonal a 34, resto a cero
      mat2 m2a = { 1.0f, 2.0f, 3.0f, 4.0f };
      mat2 m2b = { {1.0f, 2.0f}, {3.0f, 4.0f} };  // damos los valores usando dos columnas

      cout << "m2b[0][1] == " << m2b[0][1] << endl ; // debe ser 2: columna 0, fila 1 --> 2.0
   
      m4a[2][3] = 5.0f ;     // escribir en celda en columna 2 fila 3 (empezando en 0)
      float a = m4b[1][2] ;  // leer el valor en columna 1 fila 2

      // obtención de un puntero al primer valor (el resto están contiguos, \ttbf{por columnas})
      float * p = value_ptr( m4a );

      cout  << a << (*p) << endl 
            << to_string(m1) << endl 
            << to_string(m4a) << endl
            << to_string(m4b) << endl
            << to_string(m2a) << endl 
            << to_string(m2b) << endl ;
   }

   {
      // multiplicación de matrices entre ellas y con vectores
      vec4 u, v ; 
      mat4 m1, m2, m3 ;

      // multiplicación de matrices
      m1 = m2 * m3 ;
      m1 = m2 * m3 * m2 ;
   
      // mutiplicación de una matriz por un vector, por la derecha 
      u = m1 * v ; 
      u = m1 * m2 * v ;

      
   }

   // funciones que crean matrices: vista, perspectiva, transformaciones, etc...
 
   {
      // matrices de transformación 
      float ar, ag ;
      vec3 v ;

      ar = ag = 1.0f ;
      v = vec3( 1.0, 2.0, 3.0 );
      
      mat4 mt  = translate( v ),   // matriz de traslación por \ttbf{v}
           me  = scale( v ),       // matriz de escalado (\ttbf{v} en la diagonal prin.)
           mr  = rotate( ar, v ),  // matriz de rotación entorno al vector \ttbf{v}, ángulo \ttbf{ar} (radianes)
           mr1 = rotate( radians(ag), v ); // rotación en \ttbf{v} (ángulo \ttbf{ag} en grados) 

      cout << to_string(mt) << endl 
           << to_string(me) << endl
           << to_string(mr) << endl
           << to_string(mr1) << endl ;
   }
   {  
      // matrices de vista y proyección
      float fovy, aspect, left, right, top, bottom, near, far ;
      vec3  eye, center, up ;

      fovy = aspect = left = right = top = bottom = near = far = 1.0f ;
      eye = center = up = vec3( 1.0, 2.0, 3.0 );

      mat4 mv  = lookAt( eye, center, up ),                      // matriz de vista
           mpo = ortho( left, right, bottom, top, near, far ),   // matriz de proyección ortográfica
           mpf = frustum( left, right, bottom, top, near, far ), // matriz de proyección perspectiva 
           mpp = perspective( fovy, aspect, near, far );         // idem (con parámetros alternativos)

       cout << to_string(mv) << endl 
           << to_string(mpo) << endl
           << to_string(mpf) << endl
           << to_string(mpp) << endl ;
        
   }
   
}

// --------------------------------------------------------------

void TestsGLM()
{
   TestsGLM_1();
   TestsGLM_2();
   TestsGLM_3(); // matrices
}

