// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con objetos visualizables
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clase: ObjetoVisualizable
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

package mds.pcg1.objeto_visu

import android.util.Log
import mds.pcg1.aplicacion.AplicacionPCG
import mds.pcg1.cauce.CauceBase
import mds.pcg1.material.Material
import mds.pcg1.texturas.*
import mds.pcg1.vec_mat.*
import mds.pcg1.utilidades.*

// *************************************************************************************************

/**
 * Clase abstracta Objetos Visualizables:
 * Un objeto visualizable es un objeto que se puede visualizar invocando el método 'visualizar'
 *
 */
abstract class ObjetoVisualizable
{
    // ---------------------------------------------------------------------------------------------

    /**
     * nombre del objeto (es una String que lo identifica, por defecto no tiene)
     */
    var nombre : String = "no asignado" // nombre del objeto

        get() = field
        set( nuevo_nombre ) { field = nuevo_nombre }

    // ---------------------------------------------------------------------------------------------

    /**
     * Color específico del objeto (nulo si no tiene un color específico, en ese caso al
     * visualizarse en un cauce, se visualiza con el color que tenga el cauce en ese momento).
     * Por defecto es null.
     */
    var color : Vec3? = null // color del objeto, null si no tiene (inicialmente no tiene)

        get() = field
        set( nuevo_color ) { field = nuevo_color }

    // ---------------------------------------------------------------------------------------------

    /**
     * Textura del objeto (nulo si no tiene una textura, en ese caso se visualiza con la que haya
     * activa en el cauce, si hay alguna).
     */
    var textura : Textura? = null

        get() = field
        set( nueva_textura ) { field = nueva_textura }

    // ---------------------------------------------------------------------------------------------

    /**
     * Matriz de modelado de este objeto (si no es nula),
     * Si está presente, es adicional a la que haya establecida en el cauce
     * (se hace pushMM antes de visualizar y popMM después)
     */
    var matrizm : Mat4? = null

        get() = field
        set( nueva_matriz ) { field = nueva_matriz }

    // ---------------------------------------------------------------------------------------------

    /**
     * Material de este objeto (si no es nula),
     * Si está presente, el objeto se debe visualizar con este material
     * Si es nulo, el objeto se visualiza con el material que hubiera activo en el cauce
     */
    private var material : Material? = null

        get() = field
        set( nuevo_material ) { field = nuevo_material }

    // ---------------------------------------------------------------------------------------------
    /**
     * Visualiza el objeto. este método debe ser redefinido en clases derivadas
     */
    abstract fun visualizar()

    // ---------------------------------------------------------------------------------------------
    /**
     * Visualiza las aristas del objeto. Este método puede ser redefinido en clases derivadas, si
     * no se hace, el método no hace nada (eso implica que ese objeto no tiene aristas que se pueden visualizar
     * o que no se ha implementado esto)
     */
    open fun visualizarAristas()
    {
        Log.v( TAG, "El objeto '${nombre}' no tiene método para visualizar aristas ('visualizarAristas')" )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Visualiza las normales del objeto. Este método puede ser redefinido en clases derivadas, si
     * no se hace, el método no hace nada (eso implica que ese objeto no tiene normales que se pueden visualizar
     * o que no se ha implementado esto)
     */
    open fun visualizarNormales()
    {
        Log.v( TAG, "El objeto '${nombre}' no tiene método para visualizar normales ('visualizarNormales')." )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * redefinir como true para objetos que tengan normales
     * (se usa iluminación solo si el objeto tiene normales)
     */
    open val tieneNormales : Boolean get()
    {
        return false
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * guardar el estado de ciertos uniforms del cauce, y los cambia según este objeto
     *   - color
     *   - textura (que puede ser nula)
     *
     *   TODO: las texturas no se deberían guardar en el companion object, sino en una pila propia de cada cauce.
     */
    fun guardarCambiarEstado( cauce : CauceBase )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        // si tiene matriz de modelado, guardar la actual y componerla en el cauce
        matrizm?.let {
            cauce.pushMM()
            cauce.compMM( it )
        }

        // si tiene color, guardar estado de color y cambiarlo
        color?.let {
            cauce.pushColor()
            cauce.fijarColor( it )
        }

        // guardar el estado de las texturas y cambiarlo
        // (la textura no se 'hereda' como el color, o bien el objeto tiene (textura!= null) o no tiene (textura == null)
        cauce.pushTextura()
        cauce.fijarTextura( textura )  // (desactiva texturas si textura == null)

        // si tiene material específico, lo activa, en otro caso se usa el que tenga el cauce
        material?.let {
            cauce.pushMaterial()
            cauce.fijarMaterial( it )
        }
    }
    // ---------------------------------------------------------------------------------------------
    /**
     * restaura el estado de ciertos uniforms del cauce (debe estar pareado con 'guardarEstado')
     */
    fun restaurarEstado( cauce : CauceBase )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        // si tiene material, restaurar el color anterior
        if ( material != null )
            cauce.popMaterial()

        // restaurar el estado de las texturas
        cauce.popTextura( )

        // si tiene color, restaurar el estado de color
        if ( color != null )
            cauce.popColor()

        // si tiene matriz de modelado, restaurar la que había antes
        if ( matrizm  != null )
            cauce.popMM()
    }
}

// *************************************************************************************************