// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// **
// ** Gestión del cauce gráfico (clase 'CauceBase') (declaración)
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


#pragma once


#include <vector>
#include "utilidades.h"

// mapeo de atributos usados con índices de atributos enteros
// índices de los atributos en los shaders de este cauce

constexpr GLuint 
   ind_atrib_posiciones        = 0 , 
   ind_atrib_colores           = 1 ,
   ind_atrib_coord_text        = 2 , 
   numero_atributos_cauce_base = 3 ;

// -------------------------------------------------------------------------------------
class Shader 
{
   public:

   /// @brief crea un shader a partir de su código fuente
   /// @param tipo_shader - valor que identifica el tipo (ver: https://docs.gl/gl3/glCreateShader)
   /// @param cadena - cadena con el código fuente del shader (o cadena vacía si se lee de un archivo) 
   /// @param nombre_archivo - nombre del archivo fuente sin path (o cadena vacía si se pasa el código fuente en 'cadena
   ///
   Shader( const GLenum tipo_shader, const std::string & cadena, 
                                     const std::string & nombre_archivo ) ;

   private:
   

} ;
// -------------------------------------------------------------------------------------

/// @brief Clase para el cauce de funcionalidad programable (OpenGL 4.0 o superior)
///
class CauceBase 
{
   public:

   CauceBase() ;
   virtual ~CauceBase() ;

   // compila y enlaza el objeto progama 
   // (deja nombre en 'id_prog', debe ser 0 antes)
   void crearObjetoPrograma();

   /// @brief devuelve la 'location' de un uniform.
   GLint leerLocation( const char * name );

   /// @brief lee las 'locations' de todos los uniforms
   /// @brief y les da un valor inicial por defecto.
   void inicializarUniformsBase();

   /// @brief imprime los nombres y tipos de los uniform del programa (para debug)
   void imprimeInfoUniforms();

   /// @brief devuelve el color actual
   glm::vec4 leerColorActual() const ;

   /// @brief activa el cauce (se usará hasta que se active otro)
   void activar();

   /// @brief establece el color actual del cauce para todas las visualizaciones
   void fijarColor( const float r, const float g, const float b );

   /// @brief establece la matriz de vista actual en este cauce
   void fijarMatrizVista( const glm::mat4 & nue_mat_vista );

   /// @brief establece la matriz de proyección actual en este cauce
   void fijarMatrizProyeccion( const glm::mat4 & nue_mat_proyeccion );

   /// @brief  Activa o desactiva la evaluación de textura en el cauce.
   /// @param nue_eval_text - 'true' para activar la evaluación de textura, 'false' para desactivarla.
   /// @param nue_text_id - si 'nue_eval_text' es 'true', identificador de la textura a usar.
   ///
   void fijarEvalText( const bool nue_eval_text, const int nue_text_id = -1 )  ;
   
   /// @brief Activa/desactiva y fija los coeficientes de generación de coordenadas de textura.
   /// @param nue_tipo_gct - vale 0, 1, 2 
   /// @param coefs_s 
   /// @param coefs_t 
   ///
   void fijarTipoGCT( const int nue_tipo_gct, const float * coefs_s =nullptr, 
                                              const float * coefs_t =nullptr)  ;

   /// @brief inserta una copia del color actual en el tope de la pila de colores
   void pushColor();

   /// @brief extrae un color de la pila de colores y lo fija como color actual.
   void popColor();

   /// @brief inserta copia de la matriz de modelado actual (en el tope de la pila)
   void pushMM() ;  

   /// @brief extrae tope de la pila y sustituye la matriz de modelado actual
   void popMM()  ;   

   /// @brief compone matriz de modelado actual con la matriz dada.
   void compMM( const glm::mat4 & mat_componer )  ;  

   // /// @brief establece las fuentes de luz de la escena
   // void fijarFuentesLuz( const std::vector<glm::vec3> & color,
   //                       const std::vector<glm::vec4> & pos_dir_wc  ) ;

   /// @brief lee el nombre o descripción del cauce
   virtual std::string descripcion()  ;

   /// @brief métodos de apoyo para compilar/enlazar un shader (leer un archivo)
   static char * leerArchivo( const std::string & nombreArchivo ) ;


   /// @brief versiones alternativas de 'fijarColor', que aceptan tuplas (el 4o componente se ignora)
   inline void fijarColor( const glm::vec3 & c ) { fijarColor( c[0], c[1], c[2] ); }
   inline void fijarColor( const glm::vec4 & c ) { fijarColor( c[0], c[1], c[2] ); }

   /// @brief devuelve true si se deben enviar parches (GL_PATCHES) en el lugar de triángulos (GL_TRIANGLES), 
   /// @brief ocurre cuando está activo un tesselation shader que admita únicamente parches triangulares.
   /// @return (bool) true si se deben enviar parches en lugar de triángulos.
   ///
   bool sustituirTriangulosPorParches() { return sustituir_tris_parches;}; 

   /// @brief Modifica el valor del parámetro uniform 's' de los shaders 
   /// @brief (incrementándolo o decrementándolo, pero siempre en el rango 0..1)
   /// @param signo (bool) - +1.0 para incrementar, -1.0 para decrementar (aunque puede tener cualquier valor flotante)
   ///
   void modificarParametroS( const float signo );

   // -------------------------------------------------------------
   protected:

   std::string 
      nombre_src_fs = "" , // nombre del archivo de código fuente del fragment shader
      nombre_src_vs = "",  // nombre del archivo de código fuente del vertex shader
      nombre_src_gs = "",  // nombre del archivo de código fuente del geometry shader
      nombre_src_tcs = "" , // nombre del archivo de código fuente del tessellation control shader
      nombre_src_tes = "" ; // nombre del archivo de código fuente del tessellation evaluation shader

   GLuint
      id_frag_shader   = 0,       // identificador del fragment shader
      id_vert_shader   = 0,       // identificador del vertex shader 
      id_geom_shader   = 0,       // identificador del geometry shader
      id_tc_shader     = 0,       // identificador del tessellation control shader
      id_te_shader     = 0;       // identificador del tessellation evaluation shader
   
   GLint
      id_prog               = 0 ,  // identificador del 'shader program'
      
      loc_mat_modelado   = -1,  // 'localizaciones' de los parámetros uniform
      loc_mat_vista      = -1,
      loc_mat_proyeccion = -1,
      
      loc_tipo_gct       = -1,
      loc_eval_text      = -1,
      loc_coefs_s        = -1,
      loc_coefs_t        = -1 ,

      loc_param_s        = -1 ; // localización del parámetro uniform 's' de los shaders
      
   bool
      eval_text = false; // true -> eval textura, false -> usar glColor o glColorPointer
   int
      tipo_gct  = 0 ;     // tipo de generación de coordenadas de textura

   glm::vec3 
      color     = { 0.0, 0.0, 0.0}; // color actual para visualización sin tabla de colores
   
   std::vector<glm::vec3> 
      pila_colores ;  // pila de colores 
      
   glm::mat4
      mat_modelado     = glm::mat4(1.0f),  // matriz de modelado
      mat_vista        = glm::mat4(1.0f),  // matriz de vista
      mat_proyeccion   = glm::mat4(1.0f);  // matriz de proyección

   float
      coefs_s[4]       = {1.0,0.0,0.0,0.0},
      coefs_t[4]       = {0.0,1.0,0.0,0.0};

   float 
      param_s = 0.0f ; // copia del valor del parámetro uniform 'u_param_s' de los shaders

   std::vector<glm::mat4>   // pilas de la matriz de modelado y de la matriz de modelado de normales.
      pila_mat_modelado ; //,
      //pila_mat_modelado_nor ;

   /// @brief fijar (con glUniform) las matrices de modelado y de normales en el shader prog.
   /// @brief (es 'virtual' porque en la clase derivada 'Cauce3D' se tiene en cuanta el uniform con la matriz de normales)
   virtual void actualizarUniformsMatricesMN();

   /// @brief una vez compilados y adjuntados los shaders, enlaza el objeto programa
   //void enlazarObjetoPrograma();

   // devuelve el nombre de un tipo de OpenGL a partir de la constante simbólica
   static const std::string nombreTipoGL( const GLenum tipo );

   // true si se deben enviar parches en lugar de triángulos
   // (por defecto a false, excepto si se activan los shaders de teselación)
   bool sustituir_tris_parches = false ; 
} ;








