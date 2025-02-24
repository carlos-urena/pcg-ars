// *********************************************************************
// **
// ** Asignatura: Programación del Cauce Gráfico (MDS)
// ** 
// ** Funciones de parametrización para superficies paramétricas (declaraciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Declaración de las clases 
// **        + FuncionParam: clase base para funciones de parametrización de superficies
// **        + Diversas clases derivadas de 'FuncionParam' para superficies paramétricas concretas
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

#include <string>    
#include <utilidades.h>


/// @brief Clase base para funciones de parametrización de superficies
/// @brief las clases derivadas deben de implementar el método 'evaluarPosicion' 
/// @brief que devuelve la posición de la superficie en un punto dado del dominio [0..1]^2
///
class FuncionParam
{
   private:
      std::string nombre = "no asignado" ;
   
   public: 

      /// @brief constructor
      /// @param nombre_inicial nombre inicial para esta función
      ///
      FuncionParam( const std::string & nombre_inicial );

      /// @brief Pone el nombre 
      /// @param nuevo_nombre nombre nuevo para esta función 
      ///
      void fijarNombre( const std::string & nuevo_nombre ) ;
      
      /// @brief devuelve el nombre de la función 
      /// @return nombre actual de la función
      ///
      const std::string & leerNombre( ) const ;

      /// @brief devuelve la posición de la superficie en un punto dado del dominio [0..1]^2
      /// @param st coordenadas del punto en el dominio [0..1]^2
      /// @return posición de la superficie en el punto dado
      ///
      virtual glm::vec3 evaluarPosicion( const glm::vec2 & st ) const = 0 ;
} ;
// -------------------------------------------------------------------------

/// @brief Función de parametrización de una esfera
///
class FPEsfera : public FuncionParam 
{
   public:

   FPEsfera() : FuncionParam( "Esfera" ) {}

   /// @brief Calcula un punto en la superficie de una esfera 
   /// @param st coordenadas parmétricas del punto 
   /// @return punto en la superficie 
   ///
   virtual glm::vec3 evaluarPosicion( const glm::vec2 & st  ) const override ;
} ;
// -------------------------------------------------------------------------

/// @brief Función de parametrización de un cilindro
///
class FPCilindro : public FuncionParam 
{
   public: 

   FPCilindro() : FuncionParam( "Cilindro" ) {}

   /// @brief Calcula un punto en la superficie de un cilindro
   /// @param st coordenadas parmétricas del punto 
   /// @return punto en la superficie 
   ///
   virtual glm::vec3 evaluarPosicion( const glm::vec2 & st  ) const override ;
} ;
// -------------------------------------------------------------------------

/// @brief * Función de parametrización de un cono
///
class FPCono : public FuncionParam 
{
   public:

   FPCono() : FuncionParam( "Cono" ) {}

   /// @brief Calcula un punto en la superficie de un cono
   /// @param st coordenadas parmétricas del punto 
   /// @return punto en la superficie 
   ///
   virtual glm::vec3 evaluarPosicion( const glm::vec2 & st  ) const override ;
} ;
// -------------------------------------------------------------------------

/// @brief * Función de parametrización de una columna barroca
///
class FPColumna : public FuncionParam 
{
   public:

   FPColumna() : FuncionParam( "Columna" ) {}

   /// @brief Calcula un punto en la superficie de una columna barroca 
   /// @param st coordenadas parmétricas del punto 
   /// @return punto en la superficie 
   ///
   virtual glm::vec3 evaluarPosicion( const glm::vec2 & st ) const override ;
} ;