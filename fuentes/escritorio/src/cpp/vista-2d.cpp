// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Gestión de vistas 2D (implementación)
// ** Copyright (C) 2016-2024 Carlos Ureña
// **
// ** Implementación de la clase 'Vista2D'
// **   (parámetros que determinan qué región de las WCC se ven en el viewport)
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


#include <cmath>     // std::cos, std::sin
#include <algorithm> // std::min, std::max
#include "utilidades.h" 
#include "vista-2d.h"

// ----------------------------------------------------------------------

Vista2D::Vista2D( const unsigned vp_nx_ini, const unsigned vp_ny_ini )
{ 
   assert( vp_nx_ini > 1 );
   assert( vp_ny_ini > 1 );
   
   vp_nx = vp_nx_ini ;
   vp_ny = vp_ny_ini ;

   actualizarMatrices();
}
// ----------------------------------------------------------------------

void Vista2D::activar( Cauce2D & cauce )  
{
   using namespace std ;

   // fijar matrices de vista y proyección
   cauce.fijarMatrizVista( matriz_vista );
   cauce.fijarMatrizProyeccion( matriz_proyeccion );
}
// ----------------------------------------------------------------------

std::string Vista2D::descripcion() 
{
   using namespace std ;
   return "Vista2D: " + to_string( vp_nx ) + "x" + to_string( vp_ny ) ;
}
// ----------------------------------------------------------------------

void Vista2D::fijarViewport( const unsigned nuevo_vp_nx, const unsigned nuevo_vp_ny ) 
{
   assert( nuevo_vp_nx > 0 );
   assert( nuevo_vp_ny > 0 );

   vp_nx = nuevo_vp_nx ;
   vp_ny = nuevo_vp_ny ;

   actualizarMatrices();
}
// ----------------------------------------------------------------------

void Vista2D::actualizarMatrices() 
{
   using namespace glm ;

   const float 
      fx  = (2.0f/cv_lado_wcc)*std::min( 1.0f, float(vp_ny)/float(vp_nx) ) ,  
      fy  = (2.0f/cv_lado_wcc)*std::min( 1.0f, float(vp_nx)/float(vp_ny) ) ; 
   
   matriz_proyeccion = scale( vec3( fx, fy, 1.0f ) );
   matriz_vista      = rotate( radians(cv_angulo_grad), vec3( 0.0f,0.0f,1.0f ))*
                       translate( -cv_centro_wcc );

   // tamaño de un pixel en world coordinates en X y en Y (deben coincidir)
   lado_pixel_wcc = cv_lado_wcc / float( std::min( vp_nx, vp_ny ) ) ;
}
// ----------------------------------------------------------------------

void Vista2D::zoom( const float exp ) 
{
   constexpr float v = 0.05f ;
   cv_lado_wcc *= powf( 1.0f+v, -exp );  
   actualizarMatrices();
}
// ----------------------------------------------------------------------

void Vista2D::desplazar( const float npx, const float npy ) 
{ 
   cv_centro_wcc += -lado_pixel_wcc * glm::vec3( npx, -npy, 0.0f ) ;  
   actualizarMatrices();
}

   