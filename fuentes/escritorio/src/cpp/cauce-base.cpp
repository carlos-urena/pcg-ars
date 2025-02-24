// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// **
// ** Gestión del cauce gráfico (clase 'CauceBase') (implementación)
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


#include "utilidades.h" 
#include "cauce-base.h" 

// -----------------------------------------------------------------------------

Shader::Shader( const GLenum tipo_shader, const std::string & cadena, 
                                          const std::string & nombre_archivo ) 
{
   assert( tipo_shader == GL_VERTEX_SHADER || 
           tipo_shader == GL_GEOMETRY_SHADER || 
           tipo_shader == GL_FRAGMENT_SHADER ) ;  

   assert( cadena == "" || nombre_archivo == "" );

   CError(); 


}

// *****************************************************************************
// Cauce programable básico con texturas (OpenGL 3.3)
// -----------------------------------------------------------------------------


CauceBase::CauceBase()
{
   
   using namespace std ;
   cout << "Constructor de 'CauceBase' (no hace nada)." << endl ;
}
// ---------------------------------------------------------------------------

CauceBase::~CauceBase()
{
   using namespace std ;
   cout << "Destructor de 'CauceBase': inicio. " << descripcion() << endl ;
   if ( id_prog != 0 )
      glDeleteProgram( id_prog );
   id_prog = 0 ;
   cout << "Destructor de 'CauceBase': fin. " << descripcion() << endl ;
} 

// ---------------------------------------------------------------------------

void CauceBase::crearObjetoPrograma()
{
   using namespace std ;
   assert( id_prog == 0 );
   CError();

   if ( nombre_src_fs == "" || nombre_src_vs == "" )
   {
      cout << "Error: se deben especificar los nombres de los archivos fuentes del fragment y vertex shaders al menos." << endl ;
      exit(1);
   } 
   
   // inicializar los nombres de los archivos fuente 
   const std::string 
      nombre_completo_frag = PathCarpetaFuentesShaders() + "/" + nombre_src_fs,
      nombre_completo_vert = PathCarpetaFuentesShaders() + "/" + nombre_src_vs ;

   // crear el objeto programa y compilar y adjuntarle los shaders
   id_prog        = glCreateProgram(); assert( id_prog > 0 );
   id_frag_shader = CompilarAdjuntarShader( id_prog, GL_FRAGMENT_SHADER, "", nombre_completo_frag );
   id_vert_shader = CompilarAdjuntarShader( id_prog, GL_VERTEX_SHADER,   "", nombre_completo_vert );

   if ( nombre_src_gs != "" )
   {
      CError();
      const std::string nombre_completo_geom = PathCarpetaFuentesShaders() + "/" + nombre_src_gs ;
      id_geom_shader = CompilarAdjuntarShader( id_prog, GL_GEOMETRY_SHADER, "", nombre_completo_geom );
      CError();
   }

   if ( nombre_src_tcs != "" && nombre_src_tes != "")
   {
      const std::string 
         nombre_completo_tcs = PathCarpetaFuentesShaders() + "/" + nombre_src_tcs ,
         nombre_completo_tes = PathCarpetaFuentesShaders() + "/" + nombre_src_tes ;

      CError();
      id_tc_shader = CompilarAdjuntarShader( id_prog, GL_TESS_CONTROL_SHADER, "", nombre_completo_tcs );
      id_te_shader = CompilarAdjuntarShader( id_prog, GL_TESS_EVALUATION_SHADER, "", nombre_completo_tes );
      CError();
   }
   
   EnlazarObjetoPrograma( id_prog );
}
// // ---------------------------------------------------------------------------

// void CauceBase::enlazarObjetoPrograma() 
// {
//    using namespace std ;

// 	// enlazar programa y ver errores
// 	glLinkProgram( id_prog );
//    GLint resultado ;
//    glGetProgramiv( id_prog, GL_LINK_STATUS, &resultado );

//    // si ha habido errores, abortar
//    if ( resultado != GL_TRUE )
//    {  
//       glGetProgramInfoLog( id_prog, report_buffer_long_max, &report_buffer_long, report_buffer);
//       cout << "Error al enlazar el objeto programa:" << endl
//            << report_buffer << flush
//            << "Programa abortado." << endl << std::flush ;
//       exit(1);
//    }
//    CError();
// }
// ---------------------------------------------------------------------------


void CauceBase::inicializarUniformsBase()
{
   CError();
   assert( id_prog > 0);
   glUseProgram( id_prog );
   CError();
   // obtener las 'locations' de los parámetros uniform

   loc_mat_modelado      = leerLocation( "u_mat_modelado" );
   loc_mat_vista         = leerLocation( "u_mat_vista" );
   loc_mat_proyeccion    = leerLocation( "u_mat_proyeccion" );
   
   loc_eval_text         = leerLocation( "u_eval_text" );
   loc_tipo_gct          = leerLocation( "u_tipo_gct" );
   loc_coefs_s           = leerLocation( "u_coefs_s" );
   loc_coefs_t           = leerLocation( "u_coefs_t" );
   loc_param_s           = leerLocation( "u_param_s" );
   
   // dar valores iniciales por defecto a los parámetros uniform
 
   glUniformMatrix4fv( loc_mat_modelado,     1, GL_FALSE, value_ptr(mat_modelado) );
   glUniformMatrix4fv( loc_mat_vista,        1, GL_FALSE, value_ptr(mat_vista) );
   glUniformMatrix4fv( loc_mat_proyeccion,   1, GL_FALSE, value_ptr(mat_proyeccion) );

   glUniform1ui( loc_eval_text, eval_text  );
   glUniform1i ( loc_tipo_gct,  tipo_gct );

   glUniform4fv( loc_coefs_s, 1, coefs_s );
   glUniform4fv( loc_coefs_t, 1, coefs_t );
   glUniform1f( loc_param_s, param_s );

   CError();
   
   glUseProgram( 0 );

   
   CError();
}
// ---------------------------------------------------------------------------


// imprime los nombres y tipos de los uniform del programa 
// (para debug)
void CauceBase::imprimeInfoUniforms()
{
   using namespace std ;
   assert( 0 < id_prog );
   CError();

   GLint n_uniforms;
   glGetProgramiv( id_prog, GL_ACTIVE_UNIFORMS, &n_uniforms );

   cout << "Cuenta de parámetros uniform activos: " << n_uniforms << endl ;

   for ( int i = 0; i < n_uniforms; i++ )
   {
      GLenum tipo;       // tipo de la variable (float, vec3 or mat4, etc)  
      GLint  n_entradas; // si es array, número de entradas, en otro caso 1.
      glGetActiveUniform( id_prog, (GLuint)i, report_buffer_long_max, 
                          &report_buffer_long, &n_entradas, &tipo, report_buffer );
      string nombre = report_buffer ;

      // eliminar de 'nombre' la parte "[0]" que añade glActiveUniform, si está
      const std::string selim = "[0]" ;
      const size_t pos = nombre.find( selim );
      if ( pos != std::string::npos ) 
         nombre.erase(pos, selim.length());

      // añadir a 'nombre' la cadena '[n]', si es un array (si n_entradas > 1)
      const string s_n_ent = (n_entradas == 1) ? "" : string("[") + to_string(n_entradas) + "]" ;
      nombre += s_n_ent ;

      cout << "(" << i << ")\t uniform " << nombreTipoGL(tipo) << "\t " << nombre << endl ;
      CError();
   }
   CError();
}

//----------------------------------------------------------------------
// devuelve la 'location' de un uniform.
// (si no se encuentra, aborta)

GLint CauceBase::leerLocation( const char * name )
{
   using namespace std ;
   assert( name != nullptr );
   assert( id_prog > 0 );

   const GLint location = glGetUniformLocation( id_prog, name ); 
   CError();

   if ( location == -1 )
      cout << "Advertencia: la variable uniform '" << name << "' no está declarada o no se usa." << endl ;
      
   return location ;
}
//----------------------------------------------------------------------

void CauceBase::activar()
{
   //log("activo cauce ",descripcion());
   CError();
   assert( 0 < id_prog );
   glUseProgram( id_prog );
   CError();
}

// -----------------------------------------------------------------------------
// devuelve el color actual

glm::vec4 CauceBase::leerColorActual() const
{
   return glm::vec4( color.r, color.g, color.b, 1.0 );
}



//----------------------------------------------------------------------
// lee un archivo completo como texto  y devuelve una cadena terminada
// en cero con el contenido
// (el resultado está en el heap y debería borrarse con 'delete [] p')

char * CauceBase::leerArchivo( const std::string & nombreArchivo )
{
	using namespace std ;

	ifstream file( nombreArchivo.c_str(), ios::in|ios::binary|ios::ate );

	if ( ! file.is_open())
	{  cout << "imposible abrir archivo para lectura (" << nombreArchivo << ") - termino" << endl << flush ;
		exit(0);
	}
	size_t	numBytes	= file.tellg();
	char *	bytes		= new char [numBytes+2];

	file.seekg (0, ios::beg);
	file.read (bytes, numBytes);
	file.close();

	bytes[numBytes]=0 ;
	bytes[numBytes+1]=0 ;

	return bytes ;
}
// -----------------------------------------------------------------------------

void CauceBase::fijarColor( const float r, const float g, const float b )
{
   CError();
   color = { r,g,b } ; // registra color en el objeto cauce
   glVertexAttrib3f( ind_atrib_colores, r, g, b  ); // cambia valor atributo
   CError();
}
// -----------------------------------------------------------------------------

void CauceBase::pushColor()
{
   pila_colores.push_back( color );
}
// -----------------------------------------------------------------------------

void CauceBase::popColor()
{
   using namespace glm ;
   assert( pila_colores.size() > 0 );
   const vec3 c = pila_colores[ pila_colores.size()-1] ;
   pila_colores.pop_back();
   fijarColor( c );
}
// -----------------------------------------------------------------------------

void CauceBase::fijarMatrizVista( const glm::mat4 & nue_mat_vista )
{
   using namespace glm ;
   assert( 0 < id_prog );

   mat_vista = nue_mat_vista ;
   glUseProgram( id_prog );
   glUniformMatrix4fv( loc_mat_vista, 1, GL_FALSE, value_ptr( mat_vista ) );
   
   pila_mat_modelado.clear();
   //pila_mat_modelado_nor.clear();

   mat_modelado     = mat4( 1.0f ); //MAT_Ident();
   //mat_modelado_nor = mat4( 1.0f ); //MAT_Ident();

   actualizarUniformsMatricesMN();

   CError();
}
// -----------------------------------------------------------------------------

void CauceBase::fijarMatrizProyeccion( const glm::mat4 & nue_mat_proyeccion )
{
   assert( 0 < id_prog );
   mat_proyeccion = nue_mat_proyeccion ;

   glUseProgram( id_prog );
   glUniformMatrix4fv( loc_mat_proyeccion, 1, GL_FALSE, value_ptr( mat_proyeccion ) );
   CError();
}
//-----------------------------------------------------------------------------

void CauceBase::fijarEvalText( const bool nue_eval_text, const int nue_text_id  )
{
   CError();
   eval_text = nue_eval_text ;

   if ( eval_text )
   {
      assert( -1 < nue_text_id );
      glActiveTexture( GL_TEXTURE0 ) ; // creo que necesario en el cauce prog., probar
      glBindTexture( GL_TEXTURE_2D, nue_text_id );
      glUniform1ui( loc_eval_text, true );
      CError();
   }
   else
   {
      glUniform1ui( loc_eval_text, false );
      CError();
   }
   CError();
}
//-----------------------------------------------------------------------------

void CauceBase::fijarTipoGCT( const int nue_tipo_gct,
                           const float * coefs_s, const float * coefs_t )
{
   assert( 0 <= nue_tipo_gct && nue_tipo_gct <= 2 );

   tipo_gct = nue_tipo_gct ;

   if ( tipo_gct != 0 )
   {
      assert( coefs_s != nullptr );
      assert( coefs_t != nullptr );
   }

   //glUniform1i( loc_tipo_gct, tipo_gct  ? 1 : 0 );
   glUniform1i( loc_tipo_gct, tipo_gct );

   if ( tipo_gct == 1 || tipo_gct == 2 )
   {
      glUniform4fv( loc_coefs_s, 1, coefs_s );
      glUniform4fv( loc_coefs_t, 1, coefs_t );
   }
   CError();
}
// -----------------------------------------------------------------------------

void CauceBase::pushMM()
{
   pila_mat_modelado.push_back( mat_modelado );
}
// -----------------------------------------------------------------------------

void CauceBase::popMM()
{
   CError();
   const unsigned n = pila_mat_modelado.size(); assert( 0 < n );
   
   mat_modelado = pila_mat_modelado[n-1] ;
   pila_mat_modelado.pop_back();
   
   actualizarUniformsMatricesMN();
}


// -----------------------------------------------------------------------------
// compone matriz de modelado actual con la matriz dada.

void CauceBase::compMM( const glm::mat4 & mat_componer )
{
   using namespace glm ;
   CError();

   mat_modelado = mat_modelado * mat_componer ;
   
   actualizarUniformsMatricesMN();
   CError();
}
// -----------------------------------------------------------------------------

void CauceBase::actualizarUniformsMatricesMN()
{
   assert( 0 < id_prog );
   assert( -1 < loc_mat_modelado );

   CError();
   glUseProgram( id_prog );
   glUniformMatrix4fv( loc_mat_modelado,     1, GL_FALSE, value_ptr( mat_modelado ) );
   
   CError();
}

// -----------------------------------------------------------------------------

void CauceBase::modificarParametroS( const float signo )
{
   using namespace std ;
   if ( loc_param_s == -1 )
      cout << "Advertencia: no se ha encontrado el parámetro uniform 'u_param_s' en ningún shader." << endl ;
   
   // calcular el nuevo valor del parámetro s
   constexpr float delta = 0.1  ;   // va de 0 a 1 en 10 pasos.
   param_s = std::min( 1.0f, std::max( 0.0f, param_s + signo*delta ) );

   // fijar el uniform en el objeto programa
   glUseProgram( id_prog );
   glUniform1f( loc_param_s, param_s );
   
   // ya está.
   cout << "Nuevo valor del parámetro s == " << param_s << endl ;
   
}
// -----------------------------------------------------------------------------
// lee el nombre o descripción del cauce

std::string CauceBase::descripcion()
{
   return "Cauce base" ;
}

// ----------------------------------------------------------------------------
// devuelve el nombre de un tipo de OpenGL a partir de la constante simbólica

const std::string  CauceBase::nombreTipoGL( const GLenum tipo )
{
   std::string p ;

   switch( tipo )
   {
      case GL_FLOAT       : p = "float" ; break ;
      case GL_FLOAT_VEC2  : p = "vec2" ; break ;
      case GL_FLOAT_VEC3  : p = "vec3" ; break ;
      case GL_FLOAT_VEC4  : p = "vec4" ; break ;
      case GL_INT	        : p = "int" ; break ;  
      case GL_INT_VEC2	  : p = "ivec2" ; break ;
      case GL_INT_VEC3	  : p = "ivec3" ; break ;
      case GL_INT_VEC4	  : p = "ivec4" ; break ;
      case GL_UNSIGNED_INT	      : p = "unsigned int" ; break ;
      case GL_UNSIGNED_INT_VEC2	: p = "uvec2" ; break ;
      case GL_UNSIGNED_INT_VEC3	: p = "uvec3" ; break ;
      case GL_UNSIGNED_INT_VEC4	: p = "uvec4" ; break ;
      case GL_BOOL	   : p = "bool" ; break ;
      case GL_BOOL_VEC2	: p = "bvec2" ; break ;
      case GL_BOOL_VEC3	: p = "bvec3" ; break ;
      case GL_BOOL_VEC4	: p = "bvec4" ; break ;
      case GL_FLOAT_MAT2	: p = "mat2" ; break ;
      case GL_FLOAT_MAT3	: p = "mat3" ; break ;
      case GL_FLOAT_MAT4	: p = "mat4" ; break ;
      case GL_FLOAT_MAT2x3	: p = "mat2x3" ; break ;
      case GL_FLOAT_MAT2x4	: p = "mat2x4" ; break ;
      case GL_FLOAT_MAT3x2	: p = "mat3x2" ; break ;
      case GL_FLOAT_MAT3x4	: p = "mat3x4" ; break ;
      case GL_FLOAT_MAT4x2	: p = "mat4x2" ; break ;
      case GL_FLOAT_MAT4x3	: p = "mat4x3" ; break ;
      case GL_SAMPLER_1D	: p = "sampler1D" ; break ;
      case GL_SAMPLER_2D	: p = "sampler2D" ; break ;
      case GL_SAMPLER_3D	: p = "sampler3D" ; break ;
      case GL_SAMPLER_CUBE	: p = "samplerCube" ; break ;
      case GL_SAMPLER_1D_SHADOW	: p = "sampler1DShadow" ; break ;
      case GL_SAMPLER_2D_SHADOW	: p = "sampler2DShadow" ; break ;
      case GL_SAMPLER_1D_ARRAY	: p = "sampler1DArray" ; break ;
      case GL_SAMPLER_2D_ARRAY	: p = "sampler2DArray" ; break ;
      case GL_SAMPLER_1D_ARRAY_SHADOW	: p = "sampler1DArrayShadow" ; break ;
      case GL_SAMPLER_2D_ARRAY_SHADOW	: p = "sampler2DArrayShadow" ; break ;
      case GL_SAMPLER_2D_MULTISAMPLE	: p = "sampler2DMS" ; break ;
      case GL_SAMPLER_2D_MULTISAMPLE_ARRAY : p = "sampler2DMSArray" ; break ;
      case GL_SAMPLER_CUBE_SHADOW	  : p = "samplerCubeShadow" ; break ;
      case GL_SAMPLER_BUFFER	        : p = "samplerBuffer" ; break ;
      case GL_SAMPLER_2D_RECT	        : p = "sampler2DRect" ; break ;
      case GL_SAMPLER_2D_RECT_SHADOW  : p = "sampler2DRectShadow" ; break ;
      case GL_INT_SAMPLER_1D	        : p = "isampler1D" ; break ;
      case GL_INT_SAMPLER_2D	        : p = "isampler2D" ; break ;
      case GL_INT_SAMPLER_3D	        : p = "isampler3D" ; break ;
      case GL_INT_SAMPLER_CUBE	     : p = "isamplerCube" ; break ;
      case GL_INT_SAMPLER_1D_ARRAY	  : p = "isampler1DArray" ; break ;
      case GL_INT_SAMPLER_2D_ARRAY	  : p = "isampler2DArray" ; break ;
      case GL_INT_SAMPLER_2D_MULTISAMPLE	     : p = "isampler2DMS" ; break ;
      case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY : p = "isampler2DMSArray" ; break ;
      case GL_INT_SAMPLER_BUFFER	         : p = "isamplerBuffer" ; break ;
      case GL_INT_SAMPLER_2D_RECT	      : p = "isampler2DRect" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_1D	   : p = "usampler1D" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_2D	   : p = "usampler2D" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_3D	   : p = "usampler3D" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_CUBE	: p = "usamplerCube" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY	: p = "usampler2DArray" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY	: p = "usampler2DArray" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE	      : p = "usampler2DMS" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY	: p = "usampler2DMSArray" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_BUFFER	   : p = "usamplerBuffer" ; break ;
      case GL_UNSIGNED_INT_SAMPLER_2D_RECT	: p = "usampler2DRect" ; break ;
      default : assert( false ); break ;
   
   }
   return p ;
}

