// *********************************************************************
// **
// ** Asignatura: INFORMÁTICA GRÁFICA
// ** 
// ** Mallas indexadas (declaraciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Declaración de las clases 
// **        + MallaInd: malla indexada de triángulos (derivada de Objeto3D)
// **        + MallaPLY: malla indexada de triángulos, leída de un PLY (derivada de MallaInd)
// **        + algunas clases derivadas de MallaInd.
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
#include <vaos-vbos.h>
#include <objeto-visu.h>   // declaración de 'ObjetoVisu'


// ---------------------------------------------------------------------
///
/// @brief  Mallas indexadas de triángulos en 3D 
///
class MallaInd : public ObjetoVisu3D
{
   
   protected:
      // COMPLETAR: incluir aquí las variables y métodos privados que sean
      // necesarios para una malla indexada (y que no aparezcan ya declarados en esta plantilla)
      // ......

      std::vector<glm::vec3>  vertices ;
      std::vector<glm::uvec3> triangulos ;

      std::vector<glm::vec3> col_ver ;   // colores de los vértices
      std::vector<glm::vec3> nor_ver ;   // normales de vértices
      std::vector<glm::vec3> nor_tri ;   // normales de triángulos
      std::vector<glm::vec2> cc_tt_ver ; // coordenadas de textura de los vértices
      
      // descriptor del VAO con los vértices, triángulos y atributos de esta malla indexada
      // (se crea bajo demanda en 'visualizarGL')
      DescrVAO * dvao = nullptr  ;

      // VAO con los segmentos de las normales (vis. con GL_LINES)
      // ( se crea bajo demanda en 'visualizarNormales')
      DescrVAO * dvao_normales = nullptr ;

      std::vector<glm::vec3> segmentos_normales ; // guarda los segmentos de normales
      

      // normales de triángulos y vértices
      void calcularNormales();

      // calculo de las normales de triángulos (solo si no están creadas ya)
      void calcularNormalesTriangulos() ;

      

   public:
      // crea una malla vacía (nombre: "malla indexada nueva vacía")
      MallaInd() ;
      virtual ~MallaInd() ;

      // crea una malla vacía con un nombre concreto:
      MallaInd( const std::string & nombreIni );

      // visualizar el objeto con OpenGL, métodos virtuales 

      virtual void visualizarGL(  ) override ;
      virtual void visualizarGeomGL(  ) override ;
      virtual void visualizarNormalesGL() override ;
      virtual void visualizarModoSeleccionGL() override ; 
} ;
// ---------------------------------------------------------------------
// Clase para mallas obtenidas de un archivo 'ply'
// es un tipo de malla indexada que define un nuevo constructor
// que recibe el nombre del archivo ply como parámetro

class MallaPLY : public MallaInd
{
   public:
      MallaPLY( const std::string & nombre_arch ) ;
} ;


// ---------------------------------------------------------------------

class Cubo : public MallaInd
{
   public:
      Cubo();
};

// ---------------------------------------------------------------------
//#inicio N

// ---------------------------------------------------------------------

class CuboTejado : public MallaInd
{
   public:
      CuboTejado();
};

// --------------------------------------------------------------------

class Cubo24 : public MallaInd
{
  public:
      Cubo24();
} ;



class CuboNor : public MallaInd
{
   public:
      CuboNor();
};
// ---------------------------------------------------------------------

class CuboNorCol : public CuboNor
{
   public:
      CuboNorCol();
} ;

// ---------------------------------------------------------------------

class Tetraedro : public MallaInd
{
   public:
      Tetraedro();
};

// ---------------------------------------------------------------------

class Piramide : public MallaInd
{
   public:
      Piramide();
};
// ---------------------------------------------------------------------

class Cono : public MallaInd
{
   public:
      Cono() ;
} ;
// ---------------------------------------------------------------------

class ConoTruncado : public MallaInd
{
   public:
      ConoTruncado();
};


// ---------------------------------------------------------------------

class Esfera : public MallaInd
{
   public:
      //Esfera();
      Esfera( const unsigned n_mer = 64, const unsigned n_par = 64 );
};

// ---------------------------------------------------------------------

class EsferaBajaRes : public MallaInd
{
   public:
      EsferaBajaRes();
};

// ---------------------------------------------------------------------

class Semiesfera : public MallaInd
{
   public:
      Semiesfera();
};
// ---------------------------------------------------------------------

class Cilindro : public MallaInd
{
   public:
      Cilindro(  const unsigned n_hor, const unsigned n_vert );
};

