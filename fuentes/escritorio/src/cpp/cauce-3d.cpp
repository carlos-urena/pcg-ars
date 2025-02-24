// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// **
// ** Gestión del cauce gráfico (clase 'Cauce3D') (implementación)
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
#include "cauce-3d.h" 

// *****************************************************************************
// Cauce programable 3D con iluminación (OpenGL 3.3)
// -----------------------------------------------------------------------------

Cauce3D::Cauce3D()
{
   // no hace nada, la inicialización depende de la versión de OpenGL (3.3 o 4.x)
    using namespace std ;
   cout << __FUNCTION__ << " no hace nada." << endl ;
}
// --------------------------------------------------------------------------- 

Cauce3D::~Cauce3D()
{
   using namespace std ;
   cout << __FUNCTION__ << " no hace nada." << endl ;
}

// ---------------------------------------------------------------------------

void Cauce3D::inicializarUniforms3D()
{
   CError();
   glUseProgram( id_prog );
   CError();
   // obtener las 'locations' de los parámetros uniform

   loc_mat_modelado_nor  = leerLocation( "u_mat_modelado_nor" );
   loc_eval_mil          = leerLocation( "u_eval_mil" );
   loc_usar_normales_tri = leerLocation( "u_usar_normales_tri" );
   loc_mil_ka            = leerLocation( "u_mil_ka" );
   loc_mil_kd            = leerLocation( "u_mil_kd" );
   loc_mil_ks            = leerLocation( "u_mil_ks" );
   loc_mil_exp           = leerLocation( "u_mil_exp" );
   loc_num_luces         = leerLocation( "u_num_luces" );
   loc_pos_dir_luz_ec    = leerLocation( "u_pos_dir_luz_ec" );
   loc_color_luz         = leerLocation( "u_color_luz" );

   // dar valores iniciales por defecto a los parámetros uniform
 
   glUniformMatrix4fv( loc_mat_modelado_nor, 1, GL_FALSE, value_ptr(mat_modelado_nor) );
   glUniform1ui( loc_eval_mil,          eval_mil );
   glUniform1ui( loc_usar_normales_tri, usar_normales_tri );
   glUniform1f( loc_mil_ka,  0.2 );
   glUniform1f( loc_mil_kd,  0.8 );
   glUniform1f( loc_mil_ks,  0.0 );
   glUniform1f( loc_mil_exp, 0.0 );
   glUniform1i( loc_num_luces, 0 ); // por defecto: 0 fuentes de luz activas
   CError();
   
   glUseProgram( 0 );

   CError();
}
// ---------------------------------------------------------------------------

void Cauce3D::fijarEvalMIL( const bool nue_eval_mil  )
{
   CError();
   eval_mil = nue_eval_mil ; // registra valor en el objeto Cauce.
   glUseProgram( id_prog );  // activa el programa 
   glUniform1ui( loc_eval_mil, eval_mil   ); // cambia parámetro del shader
   CError();
}
// -----------------------------------------------------------------------------

void Cauce3D::fijarUsarNormalesTri ( const bool nue_usar_normales_tri )
{
   CError();
   usar_normales_tri = nue_usar_normales_tri ;
   glUseProgram( id_prog );
   glUniform1ui( loc_usar_normales_tri, usar_normales_tri );
   CError();
}

// -----------------------------------------------------------------------------

void Cauce3D::fijarParamsMIL( const float k_amb, const float k_dif,
                            const float k_pse, const float exp_pse )  
{
   CError();
   assert( 0 < id_prog );
   glUseProgram( id_prog );

   assert( -1 < loc_mil_ka );  glUniform1f( loc_mil_ka,   k_amb );
   assert( -1 < loc_mil_kd );  glUniform1f( loc_mil_kd,   k_dif );
   assert( -1 < loc_mil_ks );  glUniform1f( loc_mil_ks,   k_pse );
   assert( -1 < loc_mil_exp ); glUniform1f( loc_mil_exp,  exp_pse );

   CError();
}
// -----------------------------------------------------------------------------



void Cauce3D::fijarFuentesLuz( const std::vector<glm::vec3> & color,
                                        const std::vector<glm::vec4> & pos_dir_wc  )
{
   using namespace glm ;
   const unsigned nl = color.size();
   assert( 0 < nl && nl <= maxNumFuentesLuz() );
   assert( nl == pos_dir_wc.size() );

   assert( 0 < id_prog );
   glUseProgram( id_prog );

   std::vector<vec4> pos_dir_ec ;

   for( unsigned i = 0 ; i < nl ; i++ )
   {
      using namespace std ;
      // cout << endl << endl ;
      // cout << "Cauce::fijarFuentesLuz: long sq antes == " << pos_dir_wc[i].lengthSq()  << endl ;
      const vec4 l = mat_vista * pos_dir_wc[i ] ;
      //cout << "Cauce::fijarFuentesLuz: i == " << i << ", pos_dir_wc[" << i << "] == " << pos_dir_wc[i] <<  endl ;
      pos_dir_ec.push_back( l );
   }
   glUniform1i( loc_num_luces, nl );
   glUniform3fv( loc_color_luz, nl, (const float *)color.data() );
   glUniform4fv( loc_pos_dir_luz_ec, nl, (const float *)pos_dir_ec.data() );
}
// -----------------------------------------------------------------------------

void Cauce3D::actualizarUniformsMatricesMN()
{
   //log("entra");
   assert( 0 < id_prog );
   assert( -1 < loc_mat_modelado );
   assert( -1 < loc_mat_modelado_nor );

   mat_modelado_nor = transpose( inverse( mat_modelado ) );

   CError();
   glUseProgram( id_prog );
   glUniformMatrix4fv( loc_mat_modelado,     1, GL_FALSE, value_ptr( mat_modelado ) );
   glUniformMatrix4fv( loc_mat_modelado_nor, 1, GL_FALSE, value_ptr( mat_modelado_nor ));
   CError();
   //log("sale");
}
// -----------------------------------------------------------------------------

std::string Cauce3D::descripcion()
{
   return "Cauce 3D (OpenGL 3.3) con iluminación" ;
}

// *****************************************************************************
// Cauce programable 3D con iluminación (OpenGL 3.3)
// -----------------------------------------------------------------------------

Cauce3D_ogl3::Cauce3D_ogl3()
{
   using namespace std ;

   
   cout << "Inicio constructor de: " << descripcion() << endl ;

   nombre_src_fs = "cauce33-3D-frag.glsl" ;
   nombre_src_vs = "cauce33-3D-vert.glsl" ;

   crearObjetoPrograma();
   inicializarUniformsBase();
   inicializarUniforms3D();
   imprimeInfoUniforms();  

   cout << "Shaders compilados y objeto 'Cauce' creado con éxito." << endl ;
   cout << "Fin constructor de: " << descripcion() << endl ;
}

// ---------------------------------------------------------------------------

std::string Cauce3D_ogl3::descripcion() 
{
   return "Cauce 3D (OpenGL 3.3) con iluminación";
}

// *****************************************************************************
// Cauce programable 3D con iluminación (OpenGL 4)
// -----------------------------------------------------------------------------

Cauce3D_ogl4::Cauce3D_ogl4()
{
   using namespace std ;

   cout << "Inicio constructor de: " << descripcion() << endl ;

   nombre_src_fs  = "cauce4-3D-frag.glsl" ;
   nombre_src_tcs = "cauce4-3D-tess-control.glsl" ;
   nombre_src_tes = "cauce4-3D-tess-eval.glsl" ;
   nombre_src_gs  = "cauce4-3D-geom.glsl" ;
   nombre_src_vs  = "cauce4-3D-vert.glsl" ;

   sustituir_tris_parches = true ; // ya que hay tesselation shaders

   crearObjetoPrograma();
   inicializarUniformsBase();
   inicializarUniforms3D();
   inicializarUniforms3D_ogl4();
   imprimeInfoUniforms();  

   cout << "Shaders compilados y objeto 'Cauce' creado con éxito." << endl ;
   cout << "Fin constructor de: " << descripcion() << endl ;
}
// ---------------------------------------------------------------------------

void Cauce3D_ogl4::inicializarUniforms3D_ogl4()
{
   using namespace std ;
   cout << __FUNCTION__ << "hay una variable..." << endl ;

   loc_activar_ts  = leerLocation( "u_activar_ts" );
   loc_activar_gs  = leerLocation( "u_activar_gs" );

   fijarActivarTS( true );
   fijarActivarGS( true );
}
// ---------------------------------------------------------------------------

void Cauce3D_ogl4::fijarActivarTS( const bool nuevo_activar_ts )
{
   CError();
   assert( 0 < id_prog );
   glUseProgram( id_prog );
   assert( loc_activar_ts > -1 );
   activar_ts = nuevo_activar_ts ;
   glUniform1ui( loc_activar_ts, activar_ts );
   CError();
}

// ---------------------------------------------------------------------------

void Cauce3D_ogl4::fijarActivarGS( const bool nuevo_activar_gs )
{
   CError();
   assert( 0 < id_prog );
   glUseProgram( id_prog );
   assert( loc_activar_gs > -1 );
   activar_gs = nuevo_activar_gs ;
   glUniform1ui( loc_activar_gs, activar_gs );
   CError();
}

// ---------------------------------------------------------------------------

std::string Cauce3D_ogl4::descripcion() 
{
   return "Cauce 3D (OpenGL 4.1) con iluminación + tess. y geom. shaders";
}

