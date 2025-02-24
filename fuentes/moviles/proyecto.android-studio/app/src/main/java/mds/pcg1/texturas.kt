// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con texturas
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clase:
// **     + Textura
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

package mds.pcg1.texturas

import android.graphics.Bitmap
import android.opengl.GLES30
import android.util.Log

import java.nio.Buffer
import java.nio.ByteBuffer
import java.nio.IntBuffer.*

import mds.pcg1.utilidades.*
import mds.pcg1.cauce.*
import mds.pcg1.vec_mat.Vec4


// -------------------------------------------------------------------------------------

/**
 * Clase que encapsula un objeto 'Imagen' con los pixels de una textura, así como
 * los valores de los parámetros uniform de una textura.
 */
class Textura( p_nombre_archivo : String )
{
    var nombre_archivo: String = p_nombre_archivo // nombre del archivo desde donde se ha leído la textura
    var imagen     : Imagen = LeerArchivoImagen(nombre_archivo) // pixels y dimensiones de la imagen de textura
    var id_textura : Int    = 0  // identificador de textura (0 hasta que se envía a la GPU)
    var coefs_s    : Vec4   = Vec4(1.0f, 0.0f, 0.0f, 0.0f)  // coefs. GACT (s)
    var coefs_t    : Vec4   = Vec4(0.0f, 1.0f, 0.0f, 0.0f)  // coefs. GACT (t)
    var tipo_gct   : Int    =  0 ;     // tipo de generación de coordenadas de textura (0->desact, 1->c.obj, 2->c.mundo)
}

