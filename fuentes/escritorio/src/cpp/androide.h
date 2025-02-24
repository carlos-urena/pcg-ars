// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Declaración de la clase 'Cuadroide'
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de los métodos de la clase
// ** Puntero a la aplicación actual, inicialmente nulo (la instancia se crea en la func. 'main')
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

#include "grafo-escena.h"

// *****************************************************************************

class Cuadroide : public NodoGrafoEscena //NodoGrafoEscenaParam
{
   public:
   Cuadroide( ) ;

   virtual ~Cuadroide() ;

   virtual unsigned leerNumParametros() const ; // tiene 10 parámetros
   virtual void actualizarEstadoParametro( const unsigned iParam, const float t_sec );

   // --------------------------------------------------------------------------

   private:

   friend class Miembro ;
   friend class MiembroArti ;
   friend class Ojo ;
   friend class Antena ;

   // subobjetos
   ObjetoVisu3D
      * obj_esfera,
      * obj_semiesfera ,
      * obj_cilindro ,
      * obj_disco ;

   // materiales
   Material * matVerdePlano,
            * matNegroBrill ;

   // parámetros reales que definen el tamaño de algunas partes:
   float
      rad_c, alt_c,
      rad_b, alt_b,
      alt_p, rad_p, sep_p ;

   // punteros a las matrices de los parámetros (cua-wip)
   glm::mat4 
      * pmTraslacionRaiz     = nullptr ,  // 0
      * pmRotPieDerecho      = nullptr ,  // 1
      * pmRotPieIzquierdo    = nullptr ,  // 2
      * pmEscaladoTronco     = nullptr ,  // 3
      * pmRotCabeza          = nullptr ,  // 4
      * pmRotCodoDerecho     = nullptr ,  // 5
      * pmRotHombroDerecho   = nullptr ,  // 6
      * pmRotCodoIzquierdo   = nullptr ,  // 7
      * pmRotHombroIzquierdo = nullptr ;  // 8


} ;
// *****************************************************************************

class Miembro : public NodoGrafoEscena
{
   public:
   Miembro( const float i_r, const float i_h, Cuadroide * raiz ) ;
   float r, h ;
} ;

// *****************************************************************************

class MiembroArti : public NodoGrafoEscena
{
   public:
   MiembroArti( const float i_r, const float i_h, Cuadroide * raiz, unsigned lado ) ;
   float r, h ;

   glm::mat4 * pmRotCodo    = nullptr ,
             * pmRotHombro  = nullptr ;
} ;

// *****************************************************************************

class Ojo : public NodoGrafoEscena
{
   public:
   Ojo(  const float p_ang_ry, Cuadroide * raiz ) ;
   float ang_ry ;
} ;
// *****************************************************************************

class Antena : public NodoGrafoEscena
{
   public:
   Antena( const float p_ang_rz, Cuadroide * raiz ) ;
   float ang_rz ;
} ;
// *****************************************************************************

class FormacionDroides : public ObjetoVisu
{
   public: 
   FormacionDroides( const unsigned pnx, const unsigned pnz );
   virtual ~FormacionDroides() ;

   virtual unsigned leerNumParametros() const ; // tiene 10 parámetros
   virtual void actualizarEstadoParametro( const unsigned iParam, const float t_sec );
   virtual void visualizarGL() ;
   virtual void visualizarGeomGL(  ) ;
   virtual void visualizarNormalesGL ()  ;
   virtual void visualizarModoSeleccionGL() ;

   private:

   void visu_master( unsigned modo );
   void visu_gen( unsigned modo );

   float delta_t( const unsigned ip, const unsigned ix, const unsigned iz ) const ;
   unsigned numpar = 0 ;

   std::vector<float> tiempo_par ;
   std::vector<float> delta_par ;

   unsigned nx = 0; 
   unsigned nz = 0;

   Cuadroide * master = nullptr ;

   std::default_random_engine generator{ (std::random_device())() };
   std::uniform_real_distribution<float> uniform_dist{ 0.0f, 1.0f } ;

   
};

