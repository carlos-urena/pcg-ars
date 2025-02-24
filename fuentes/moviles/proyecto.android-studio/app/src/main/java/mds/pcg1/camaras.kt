// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con cámaras interactivas
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clase: CamaraInteractiva, CamaraVista2D, Camara3D
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

package mds.pcg1.camaras

import android.util.Log
import mds.pcg1.cauce.*
import mds.pcg1.utilidades.*
import mds.pcg1.vec_mat.*
import kotlin.math.min

// *************************************************************************************************

open class CamaraInteractiva( ancho_vp : Int, alto_vp : Int )
{
    // dimensiones del viewport sobre el cual se usa esta cámara
    protected var tam_vp_x_dcc  : Float = ancho_vp.toFloat()   // ancho del viewport en pixels
    protected var tam_vp_y_dcc  : Float = alto_vp.toFloat()    // alto del viewport en pixels

    // matrices de vista y proyección
    protected var mat_vista      = Mat4.ident()
    protected var mat_proyeccion = Mat4.ident()

    /**
     * Mueve la cámara tras un evento de movimiento con un desplazamiento
     * proporcional a [delta_x] y [delta_y]
     * (debe ser redefinido en clases derivadas)
     */
    open fun mover( delta_x : Float, delta_y : Float )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        Log.v( TAG, "este objeto cámara no tiene redefinido el método '$TAGF'")
    }

    /**
     * hacer 'zoom', usando un factor de escala relativo [factor_rel]
     * (debe ser redefinido en clases derivadas)
     */
    open fun zoom( factor_rel : Float )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        Log.v( TAG, "este objeto cámara no tiene redefinido el método '$TAGF'")
    }

    /**
     * Recalcular las matrices en función del estado actual
     * (debe ser redefinido en clases derivadas)
     */
    protected open fun recalcularMatrices( )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        Log.v( TAG, "este objeto cámara no tiene redefinido el método '$TAGF'")
    }

    /**
     * cambia las dimensiones del viewport (tiene efecto cuando se vuelve a activar)
     */
    fun fijarViewport( ancho_vp : Int, alto_vp : Int )
    {
        tam_vp_x_dcc  = ancho_vp.toFloat()   // ancho del viewport en pixels
        tam_vp_y_dcc  = alto_vp.toFloat()    // alto del viewport en pixels
    }

    /**
     * Activa esta cámara en este cauce: recalcula las matrices y fija uniforms
     */
    fun activar( cauce : CauceBase )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        recalcularMatrices()
        cauce.fijarMatrizVista( mat_vista )
        cauce.fijarMatrizProyeccion( mat_proyeccion )
    }

}
// *************************************************************************************************

/**
 * Cámaras para vistas 2D, el plano de visión es el plano XY
 * En el viewport se visualiza un cuadrado con centro en [cv_centro_wcc] y
 * lado [cv_lado_wcc]  (todos los valores en coordenadas de mundo)
 */
class CamaraVista2D( ancho_vp : Int, alto_vp : Int ) : CamaraInteractiva( ancho_vp, alto_vp )
{
    private var cv_centro_wcc : Vec3  = Vec3( 0.0f, 0.0f, 0.0f )
    private var cv_lado_wcc   : Float = 2.0f

    // ---------------------------------------------------------------------------------------------
    /**
     * Mueve la cámara tras un evento de movimiento con un desplazamiento
     * proporcional a [delta_x] y [delta_y]
     * Desplaza [cv_centro_wcc] usando [delta_x] y [delta_y]
     */
    override fun mover( delta_x : Float, delta_y : Float )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        cv_centro_wcc = cv_centro_wcc - Vec3( delta_x, delta_y, 0.0f )
    }
    // ---------------------------------------------------------------------------------------------
    /**
     * hacer 'zoom', usando un factor de escala relativo [factor_rel]
     * cambia el lado del cuadrado visible ([cv_lado_wcc]
     */
    override fun zoom( factor_rel : Float )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        cv_lado_wcc *= 1.0f/factor_rel
    }
    // ---------------------------------------------------------------------------------------------

    override fun recalcularMatrices()
    {
        val tam_pixel_wcc = cv_lado_wcc/ min( tam_vp_x_dcc, tam_vp_y_dcc ) // lado de un pixel en WCC
        val fx            = (2.0f/cv_lado_wcc)* min( 1.0f, tam_vp_y_dcc/tam_vp_x_dcc )
        val fy            = (2.0f/cv_lado_wcc)* min( 1.0f, tam_vp_x_dcc/tam_vp_y_dcc )

        mat_vista = Mat4.traslacion( (-tam_pixel_wcc)*cv_centro_wcc )
        mat_proyeccion = Mat4.escalado( Vec3( fx, fy, 1.0f  ))

    }
}  // fin de CamaraVista2D

// *************************************************************************************************

class CamaraOrbital3D( ancho_vp : Int, alto_vp : Int ) : CamaraInteractiva( ancho_vp, alto_vp )
{

    // amplitud de campo vertical (en grados) para cámaras perspectivas
    private var fovy_grad : Float = 70.0f

    // distancia al plano de recorte delantero
    private var near : Float = 0.05f

    // distancia al plano de recorte trasero
    private var far : Float = 100.0f

    // true si la cámara es perspectiva, false si es paralela (por ahora solo puede ser true)
    //private es_perspectiva : boolean = true

    // punto de atención actual
    private var punto_atencion : Vec3 = Vec3( 0.0f, 0.0f, 0.0f )

    // ángulo vertical de la cámara en grados (rotación entorno a X)
    private var angulo_vert_grad : Float = 20.0f //45.0f

    // ángulo horizontal de la cámara en grados (rotación entorno a Y)
    private var angulo_hor_grad : Float = -20.0f //45.0f

    // distancia desde el observador hasta el punto de atención
    private var distancia : Float = 12.0f

    // ---------------------------------------------------------------------------------------------

    override fun recalcularMatrices()
    {
        val ratio_xy : Float = tam_vp_x_dcc/tam_vp_y_dcc
        val ratio_yx : Float = tam_vp_y_dcc/tam_vp_x_dcc
        mat_proyeccion = Mat4.perspective( fovy_grad, ratio_xy, near, far )

        val a      : Vec3 = punto_atencion
        val mtras1 : Mat4 = Mat4.traslacion( Vec3( -a.x, -a.y, -a.z ))
        val mrot2  : Mat4 = Mat4.rotacionXgrad( angulo_vert_grad )
        val mrot1  : Mat4 = Mat4.rotacionYgrad( -angulo_hor_grad )
        val mtras2 : Mat4 = Mat4.traslacion( Vec3( 0.0f, 0.0f, -distancia ))

        mat_vista = mtras2 * mrot2 * mrot1 * mtras1
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Modifica los ángulos horizonal y vertical de la cámara orbital
     *
     * @param incr_hor (number) -- incremento en horizontal
     * @param incr_ver (number) -- incremento en vertical
     */
    override fun mover( delta_x : Float, delta_y : Float )
    {
        val fac : Float = 0.3f
        this.angulo_hor_grad  += -fac*delta_x
        this.angulo_vert_grad += -fac*delta_y
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Modifica la distancia entre el punto de atención y el punto del observador
     * @param incr_z (number) +1 o -1 según la dirección de movimiento del ratón
     */
    override fun zoom( factor_rel : Float )
    {
        distancia = distancia/factor_rel
    }
}

// *************************************************************************************************


