// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con fuentes de luz
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases:
// **
// **   + FuenteLuz
// **   + ColeccionFuentesLuz
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

package mds.pcg1.fuentes_luz

import mds.pcg1.vec_mat.Vec3
import mds.pcg1.vec_mat.Vec4

// *************************************************************************************************

/**
 * Clase para una fuente de luz, incluye su color y su posición/dirección
 * (las fuentes de luz no se activan de forma individual, sino como parte de una colección)
 */
class FuenteLuz( p_pos_dir_wcc : Vec4, p_color : Vec3 )
{
    val pos_dir_wcc : Vec4 = p_pos_dir_wcc  // posicion o dirección de la fuente en coordenadas de mundo
    val color       : Vec3 = p_color        // color o intensidad (r,g,b >0 , pero no necesariamente acotado por arriba)

    init
    {
        val TAGF = "FuenteLuz constructor"
        assert( pos_dir_wcc.w == 0.0f || pos_dir_wcc.w == 1.0f ) { "${TAGF} 'pos_dir_wc' debe tener 'w' puesta a 1 o a 0`" }
        assert( pos_dir_wcc.longitud + pos_dir_wcc.w > 0.0 ) { "${TAGF} 'pos_dir_wc' no puede tener todas las componentes a cero" }
    }

}
// *************************************************************************************************

/**
 * Clase para una colección de fuentes de luz (es un array de instancias de FuenteLuz)
 */
class ColeccionFuentesLuz( p_fuentes : ArrayList<FuenteLuz> )
{

    var fuentes: ArrayList<FuenteLuz> = p_fuentes

    init {
        val TAGF = "FuenteLuz constructor"
        assert(fuentes.size > 0) { "$TAGF se ha dado una colección de fuentes vacía" }
    }
}

// *************************************************************************************************




