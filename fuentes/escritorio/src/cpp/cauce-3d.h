// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// **
// ** Gestión del cauce gráfico (clase 'Cauce3D') (declaración)
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
#include "cauce-base.h"

// atributos adicionales usados en el cauce 3D (adicionales a los usados en el cauce base)
constexpr GLuint 
   ind_atrib_normales        = 3 , 
   numero_atributos_cauce_3d = 4 ;

// -------------------------------------------------------------------------------------
/// @brief Clase para el cauce de funcionalidad programable (OpenGL 3.3 o superior)
///
class Cauce3D : public CauceBase
{
   public:

   /// @brief Crea el objeto programa e inicializa los parámetros uniform
   ///
   Cauce3D() ;

   /// @brief destructor: destruye el objeto programa
   ///
   virtual ~Cauce3D() ;

   /// @brief lee las 'locations' de todos los uniforms
   /// @brief y les da un valor inicial por defecto.
   ///
   void inicializarUniforms3D();

   /// @brief Activa o desactiva el uso de iluminación para añadir realismo 
   ///
   /// @param nue_eval_mil (bool) true -> activa evaluación del MIL, false -> desactiva
   ///
   void fijarEvalMIL( const bool nue_eval_mil );

   /// @brief Activa o desactiva el uso de normales del triángulo para iluminación 
   ///
   /// @param nue_usar_normales_tri (bool) true -> activa uso de normales del triángulo, false -> desactiva
   ///
   void fijarUsarNormalesTri ( const bool nue_usar_normales_tri );

   /// @brief Fija los valores de los parámetros del modelo de iluminación local
   ///
   /// @param k_amb (float) coeficiente de la componente ambiente
   /// @param k_dif  (float) coeficiente de reflexión de la componente difusa 
   /// @param k_pse  (float) coeficiente de reflexión de la componentes pseudo-especular
   /// @param exp_pse (float) exponente de brillo de la componente pseudo-especular
   ///
   void fijarParamsMIL( const float k_amb, const float k_dif,
                        const float  k_pse, const float exp_pse )  ;
  
   /// @brief establece las fuentes de luz de la escena
   
   
   /// @brief da valores a los uniforms relacionados con las fuentes de luz en el cauce 
   ///
   /// @param color      vector de colores de las fuentes de luz
   /// @param pos_dir_wc vector de posiciones o direcciones a la fuentes de luz
   ///
   void fijarFuentesLuz( const std::vector<glm::vec3> & color,
                         const std::vector<glm::vec4> & pos_dir_wc  ) ;

   /// @brief devuelve una descripción de este cauce
   virtual std::string descripcion()  override ;

   
   /// @brief devuelve el máximo número de fuentes de luz permitidas:
   /// @brief (según el estándar OpenGL, ese número debe ser como minimo 8) (aunque en OpenGL 4.1 quizás no tiene sentido!)
   ///
   unsigned maxNumFuentesLuz() { return 8 ; } ;

   // -------------------------------------------------------------
   protected:

  
   GLint
      loc_mat_modelado_nor  = -1,
      loc_eval_mil          = -1,
      loc_usar_normales_tri = -1,
      loc_mil_ka            = -1,
      loc_mil_kd            = -1,
      loc_mil_ks            = -1,
      loc_mil_exp           = -1,
      loc_num_luces         = -1,
      loc_color_luz         = -1,
      loc_pos_dir_luz_ec    = -1 ;

   bool
      eval_mil          = false, // true -> evaluar MIL, false -> usar color plano
      usar_normales_tri = false;
   glm::mat4
      mat_modelado_nor = glm::mat4(1.0f);   // matriz de modelado para normales
   std::vector<glm::mat4>   
      pila_mat_modelado_nor ;

   // fijar (con glUniform) las matrices de modelado y de normales en el shader prog.
   virtual void actualizarUniformsMatricesMN() override;

} ;
// -------------------------------------------------------------------------------------

/// @brief Clase para el cauce de funcionalidad programable (OpenGL  3.3)
///
class Cauce3D_ogl3 : public Cauce3D 
{

   public: 
   
   /// @brief Crea el objeto programa e inicializa los parámetros uniform
   ///
   Cauce3D_ogl3() ;

   /// @brief devuelve una descripción de este cauce (cauce 3D con OpenGL 3.3)
   virtual std::string descripcion()  override ;

   
} ;
// -------------------------------------------------------------------------------------

/// @brief Clase para el cauce de funcionalidad programable (OpenGL 4.0 o superior)
///
class Cauce3D_ogl4 : public Cauce3D 
{
   public: 
   
   /// @brief Crea el objeto programa e inicializa los parámetros uniform
   ///
   Cauce3D_ogl4() ;

   /// @brief devuelve una descripción de este cauce (cauce 3D con OpenGL 4)
   virtual std::string descripcion()  override ;

   
   /// @brief Activa o desactiva la teselación en los tesselation shaders
   /// @param nuevo_activar_ts (bool) true -> activar, false -> desactivar
   ///
   void fijarActivarTS( const bool nuevo_activar_ts );

   /// @brief Activa o desactiva la visualización de aristas en el geometry shader
   /// @param nuevo_activar_ts (bool) true -> activar, false -> desactivar
   ///
   void fijarActivarGS( const bool nuevo_activar_gs );

   /// @brief devuelve el estado de activación de la teselación en los tesselation shaders
   /// @return (bool) true -> activado, false -> desactivado
   ///
   bool leerActivarTS() { return activar_ts ; } ;

   /// @brief devuelve el estado de activación de la visualización de aristas en el geometry shader
   /// @return (bool) true -> activado, false -> desactivado
   ///
   bool leerActivarGS() { return activar_gs ; } ;

   protected:
   void inicializarUniforms3D_ogl4(); // inicializa los uniforms específicos de OpenGL 4
   
   // uniforms y variables de estado.
   GLint loc_activar_ts = -1 ; // location del uniform 'u_activar_ts' (tessellation shader)
   GLint loc_activar_gs = -1 ; // location del uniform 'u_activar_gs' (geometry shader)
   
   bool activar_ts = false ; // true -> activar tessellation shader, false -> no activar
   bool activar_gs = false ; // true -> activar geometry shader, false -> no activar
} ;


