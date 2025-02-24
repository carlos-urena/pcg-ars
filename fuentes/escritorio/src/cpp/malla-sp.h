// *********************************************************************
// **
// ** Asignatura: Programación del Cauce Gráfico (MDS)
// ** 
// ** Mallas indexadas que aproximan superficies paramétricas (declaraciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Declaración de las clases 
// **        + MallaSupPar: malla indexada de triángulos, que aproxima una superficie paramétrica cualquiera
// **        + Diversas clases derivadas de MallaSupPar, que aproximan superficies paramétricas concretas
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

#include <vector>       // usar std::vector
#include <utilidades.h>
#include <malla-ind.h>   // declaración de 'ObjetoVisu'
#include <sup-par.h>     // declaración de 'FuncionParam'

// -----------------------------------------------------------------------
///
/// @brief clase para mallas indexadas generadas a partir de una superficie paramétrica
///
class MallaSupPar : public MallaInd 
{
   private: 
      const FuncionParam * fp = nullptr ;  
      unsigned ns = 0 ;
      unsigned nt = 0 ;


   public:

   /// @brief crea una malla indexada a partir de una función de parametrización
   /// @param p_fp - puntero (no nulo) a la función de parametrización) 
   /// @param p_ns - número de muestras en la dirección 's' (primer parámetro de la función de parametrización)
   /// @param p_nt - número de muestras en la dirección 't' (segundo parámetro de la función de parametrización)
   ///
   MallaSupPar( const FuncionParam * p_fp, const unsigned p_ns, const unsigned p_nt  );

   protected:

   /// @brief promedia las normales de la primera y la última columna de vértices
   ///
   void promediarNormalesCol();
   
    
};

/// @brief Malla indexada generada con la parametrización de una esfera 
///
class MallaSPEsfera : public MallaSupPar 
{
   public:
   MallaSPEsfera( const unsigned ns, const unsigned nt );
};

/// @brief Malla indexada generada con la parametrización de un cilindro 
///
class MallaSPCilindro : public MallaSupPar 
{
   public:
   MallaSPCilindro( const unsigned ns, const unsigned nt );
};

/// @brief Malla indexada generada con la parametrización de un cono 
///
class MallaSPCono : public MallaSupPar 
{
   public:
   MallaSPCono( const unsigned ns, const unsigned nt );
};

/// @brief Malla indexada generada con la parametrización de una columna barroca 
///
class MallaSPColumna : public MallaSupPar
{
   public:
   MallaSPColumna( const unsigned ns, const unsigned nt );
};
