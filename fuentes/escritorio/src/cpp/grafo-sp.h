// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Declaración de la clase 'GrafoSupPar'
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Clase con un nodo de grafo de escena con varias mallas 
// ** (superficies paramétricas) con texturas.
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

/// @brief Una clase que crea un grafo con diversas superficies paramétricas 
///
class GrafoSupPar : public NodoGrafoEscena //NodoGrafoEscenaParam
{
   public:
   GrafoSupPar( ) ;
};
