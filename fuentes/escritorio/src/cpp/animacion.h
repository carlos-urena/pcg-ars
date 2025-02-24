// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Gestión de animaciones (declaraciones)
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Declaraciones de las funciones:
// **    + ActualizarEstado: actualiza el estado de un objeto 3D
// **    + AnimacionesActivadas: indica si las animaciones están activadas o no
// **    + ProcesarTeclaAnimacion: procesa una pulsación de una tecla estando baja la tecla 'A'
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

#include "objeto-visu.h"
#include "utilidades.h"

// #####################################
// Funciones usadas para animacion:
// ####################################



/// @brief Función  que actuliza periodicamente el estado de la aplicación actual
/// @brief   (1) calcula el tiempo real transcurrido desde la última llamada a esta función
///  @brief  (2) actualiza el estado del objeto, usando ese tiempo transcurrido
///
bool ActualizarEstado( ) ;

/// @brief  devuelve true sii las nimaciones están activadas
///
bool AnimacionesActivadas() ;

// gestiona una tecla correspondiente a animaciones (se llama cuando se pulsa una
// tecla la vez que la tecla 'A' está pulsada)
bool ProcesarTeclaAnimacion( ObjetoVisu * objeto, int glfw_key ) ;

