// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// **
// ** Gestión del cauce gráfico (clase 'Cauce2D') (implementación)
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
#include "cauce-2d.h" 


Cauce2D::Cauce2D()
{
   using namespace std ;
   cout << "Constructor de Cauce2D: inicio." << endl ;
   
   nombre_src_fs = "cauce33-2D-frag.glsl" ;
   nombre_src_vs = "cauce33-2D-vert.glsl" ;

   crearObjetoPrograma();
   inicializarUniformsBase(); // uniforms de 'CauceBase'
   imprimeInfoUniforms();  

   cout << "Constructor de Cauce2D: fin." << endl ;
}
// ---------------------------------------------------------------------------

Cauce2D::~Cauce2D()
{
   using namespace std ;
   cout << "Destructor de 'Cauce2D': inicio+fin (no hace nada) " << descripcion() << "." << endl ;
} 
// ---------------------------------------------------------------------------

std::string Cauce2D::descripcion()
{
   return "Cauce 2D." ;
}

// *********************************************************************** 

Cauce2DLineas::Cauce2DLineas() 
{
   using namespace std ;
   cout << "Constructor de Cauce2DSegmentos: inicio." << endl ;
   
   nombre_src_fs = "cauce33-2Dsegm-frag.glsl" ;
   nombre_src_vs = "cauce33-2Dsegm-vert.glsl" ;
   nombre_src_gs = "cauce33-2Dsegm-geom.glsl" ;

   crearObjetoPrograma();
   inicializarUniformsBase();        // uniforms de la clase 'Cauce2D'
   inicializarUniforms2DLineas(); // uniforms específicos de este cauce 
   imprimeInfoUniforms();  

   cout << "Constructor de Cauce2DSegmentos: fin." << endl ;
}
// ---------------------------------------------------------------------------

Cauce2DLineas::~Cauce2DLineas()
{

}
// ---------------------------------------------------------------------------

void Cauce2DLineas::inicializarUniforms2DLineas() 
{
   CError();
   assert( id_prog > 0 );
   glUseProgram( id_prog );
   CError();

   // leer locations de uniforms
   loc_grosor_lineas_wcc = leerLocation( "u_grosor_lineas_wcc" );
   loc_radio_puntos_wcc  = leerLocation( "u_radio_puntos_wcc" );
   loc_visualizar_puntos = leerLocation( "u_visualizar_puntos" );
   loc_visualizar_lineas = leerLocation( "u_visualizar_lineas" );

   // dar valores iniciales a los uniforms
   glUniform1f( loc_grosor_lineas_wcc, grosor_lineas_wcc_inicial );
   glUniform1f( loc_radio_puntos_wcc, radio_puntos_wcc_inicial );
   glUniform1ui( loc_visualizar_puntos, 1 );
   glUniform1ui( loc_visualizar_lineas, 1 );

   glUseProgram(0);
   CError();
}
// --------------------------------------------------------------------------- 

void Cauce2DLineas::fijarGrosorLineasWCC( float nuevo_ancho_wcc )
{
   assert( loc_grosor_lineas_wcc >= 0 );
   glUniform1f( loc_grosor_lineas_wcc, nuevo_ancho_wcc );
}

// --------------------------------------------------------------------------- 

void Cauce2DLineas::fijarRadioPuntosWCC( float nuevo_radio_wcc )
{
   assert( loc_radio_puntos_wcc >= 0 );
   glUniform1f( loc_radio_puntos_wcc, nuevo_radio_wcc );
}
// --------------------------------------------------------------------------- 

void Cauce2DLineas::fijarVisualizarPuntos( bool nuevo_visualizar_puntos )
{
   assert( loc_visualizar_puntos >= 0 );
   glUniform1ui( loc_visualizar_puntos, (unsigned int) nuevo_visualizar_puntos );
}

// --------------------------------------------------------------------------- 

void Cauce2DLineas::fijarVisualizarLineas( bool nuevo_visualizar_lineas )
{
   assert( loc_visualizar_lineas >= 0 );
   glUniform1ui( loc_visualizar_lineas, (unsigned int) nuevo_visualizar_lineas );
}
// --------------------------------------------------------------------------- 

std::string Cauce2DLineas::descripcion()
{
   return "Cauce 2D para visualizar segmentos de línea." ;
}
