// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con objetos visualizables compuestos de sub-objetos.
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases:
// **    + ObjetoVisCompuesto (derivada de ObjetoVisualizable)
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

package mds.pcg1.objeto_comp

import android.opengl.GLES30
import mds.pcg1.aplicacion.AplicacionPCG
import mds.pcg1.malla_ind.MallaIndHelloRectangleXY
import mds.pcg1.malla_ind.MallaIndHelloRectangleXZ
import mds.pcg1.objeto_visu.*
import mds.pcg1.utilidades.nfnd


// *************************************************************************************************
/**
 * Una clase para objetos visualizables que están compuestos de listas de otros objetos visualizables
 */
open class ObjetoVisCompuesto : ObjetoVisualizable()
{
    /** lista de sub-objetos de este objeto
     */
    protected var sub_objetos : MutableList<ObjetoVisualizable> = mutableListOf()

    /**
     * Visualiza el objeto compuesto
     */
    override fun visualizar()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        val cauce = AplicacionPCG.instancia.leer_cauce

        assert( sub_objetos.size > 0 ) {"$TAGF el array de subobjetos está vacío"}

        guardarCambiarEstado( cauce )

        for( obj in sub_objetos )
            obj.visualizar()

        restaurarEstado( cauce )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Visualiza las aristas de todos los sub-objetos.
     */
    override fun visualizarAristas()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //val cauce = AplicacionPCG.instancia.leer_cauce

        for( obj in sub_objetos )
            obj.visualizarAristas()
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Visualiza las normales de todos los sub-objetos
     */
    override fun visualizarNormales()
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //val cauce = AplicacionPCG.instancia.leer_cauce

        for( obj in sub_objetos )
            obj.visualizarNormales()
    }
}
// *************************************************************************************************

/**
 * Una clase de test para objetos compuestos
 */
class DosCuadrados : ObjetoVisCompuesto()
{
    init
    {
        nombre = "Dos cuadrados"
        sub_objetos.add( MallaIndHelloRectangleXY() )
        sub_objetos.add( MallaIndHelloRectangleXZ() )
    }
}

