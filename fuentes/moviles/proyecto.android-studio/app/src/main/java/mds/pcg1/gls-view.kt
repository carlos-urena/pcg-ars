// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con la vista de la superficie OpenGL ES y la
// ** la captura de eventos.
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases: GLSurfaceViewPCG, RendererPCG, ScaleGListener
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

package mds.pcg1.gls_view

import android.content.Context
import android.opengl.*
import android.os.Build
import android.util.Log
import android.view.GestureDetector
import android.view.GestureDetector.OnDoubleTapListener
import android.view.GestureDetector.OnGestureListener
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import androidx.annotation.RequiresApi
import androidx.core.view.GestureDetectorCompat

import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

import mds.pcg1.aplicacion.AplicacionPCG
import mds.pcg1.utilidades.nfnd
import mds.pcg1.vec_mat.VecMatTest


// ------------------------------------------------------------------------------------------------

class ScaleGListener : ScaleGestureDetector.SimpleOnScaleGestureListener()
{

}
// ------------------------------------------------------------------------------------------------




// see: https://developer.android.com/develop/ui/views/graphics/opengl/about-opengl#version-check



/**
 * @brief Objeto 'surface view' para una superficie sobre la cual se dibuja con OpenGL
 */
class GLSurfaceViewPCG( p_context: Context ) :
    OnGestureListener,
    OnDoubleTapListener,
    GLSurfaceView( p_context )
{
    private val renderer  : RendererPCG
    private var og_escala  : ScaleGListener       // observador de gestos de escala
    private var dg_escala : ScaleGestureDetector // detector de gestos de escala

    // variable de estado usada para evitar eventos de movimiento que se generan
    // después de acabado un evento de escala, por no levantar los dos dedos a la vez.

    private var mover_habilitado : Boolean = true

    // variable de estado que indica si está en progreso un evento de escala, sirve
    // para
    private var escala_en_progreso : Boolean = false


    // ¿ detector de eventos varios ?
    private var mDetector: GestureDetectorCompat

    companion object
    {
        var contador_eventos : Int = 0
    }

    init {

        // Fijar la versión del contexto OpenGLES (y crearlo?)
        setEGLContextClientVersion(3)

        // Crear el renderer y asociarlo a esta vista
        renderer = RendererPCG()
        setRenderer( renderer )

        // Volver a hacer el render únicamente cuando hay algún cambio
        renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY

        // Eliminar la aplicación PCG anterior y crear una nueva.
        AplicacionPCG.instancia_act = null   // destruye instancia anterior
        AplicacionPCG( this )  // se registra como 'AplicacionPCG.instancia'

        // Crear el observador y el detector de gestos de escala
        // ver: https://developer.android.com/reference/kotlin/android/view/ScaleGestureDetector
        og_escala  = ScaleGListener()
        dg_escala  = ScaleGestureDetector( context, og_escala )

        // asociar el detector de gestos variados con esta vista
        mDetector = GestureDetectorCompat(context, this)
        mDetector.setOnDoubleTapListener(this) // poner el detector como observador de gestos de double tap


    }

    @RequiresApi(Build.VERSION_CODES.Q)
    override fun onTouchEvent( me : MotionEvent ) : Boolean
    {
        var  inicio_escala = false

        //val TAGF = "GLS VIEW onTouchEvent"
        //val da = MotionEvent.actionToString( me.action ) // descripción de la action

        contador_eventos ++

        //Log.v(TAGF,"$TAGF (inicio evento $contador_eventos) action string == ${da}")

        mDetector.onTouchEvent( me ) // analiza los eventos touch....¿?

        dg_escala.onTouchEvent( me )  // detecta gesto de escala y actualiza su estado

        // gestionar evento de escala, no permite mover mientras está en progreso el evento de escala
        if ( dg_escala.isInProgress )
        {
            mover_habilitado = false
            if ( ! escala_en_progreso )
            {
                inicio_escala = true
                escala_en_progreso = true
            }
        }
        else
            escala_en_progreso = false

        if ( me.action == MotionEvent.ACTION_UP )
            mover_habilitado = true

        if ( inicio_escala )
            AplicacionPCG.instancia_act?.mgeInicioPinchInOut( dg_escala.scaleFactor )
        if ( escala_en_progreso )
            AplicacionPCG.instancia_act?.mgePinchInOut( dg_escala.scaleFactor )
        if ( mover_habilitado ) {

            if ( me.action == MotionEvent.ACTION_DOWN )
                AplicacionPCG.instancia_act?.mgeInicioMover( me.rawX, me.rawY )
            else if ( me.action == MotionEvent.ACTION_MOVE )
                AplicacionPCG.instancia_act?.mgeMover( me.rawX, me.rawY )
        }

        return true
    }

    override fun onSingleTapConfirmed(e: MotionEvent): Boolean {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        return true
    }

    override fun onDoubleTap(e: MotionEvent): Boolean {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        return true
    }

    override fun onDoubleTapEvent(e: MotionEvent): Boolean {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        return true
    }

    override fun onDown(e: MotionEvent): Boolean {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        return true
    }

    override fun onShowPress(e: MotionEvent)
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")


    }

    override fun onSingleTapUp(e: MotionEvent): Boolean
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        return true
    }

    override fun onScroll(
        e1: MotionEvent?,
        e2: MotionEvent,
        distanceX: Float,
        distanceY: Float
    ): Boolean
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        return true
    }

    override fun onLongPress(e: MotionEvent)
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        AplicacionPCG.instancia_act?.mgeLongPress( e )
    }

    override fun onFling(
        e1: MotionEvent?,
        e2: MotionEvent,
        velocityX: Float,
        velocityY: Float
    ): Boolean {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //Log.v( TAGF, "$TAGF")

        return true
    }


}

// *************************************************************************************************

/**
 * @brief Objeto de tipo 'Renderer'
 */
class RendererPCG( ) : GLSurfaceView.Renderer
{
    override fun onSurfaceCreated( unused: GL10, config: EGLConfig )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        Log.v( TAGF, "$TAGF inicio")

        VecMatTest().let { it.run() } ;

        Log.v( TAGF, "$TAGF fin")
    }

    override fun onDrawFrame( unused: GL10 )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        AplicacionPCG.instancia_act?.mgeVisualizarFrame()
    }

    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        AplicacionPCG.instancia_act?.mgeCambioTamano( width, height )
    }
}
// -------------------------------------------------------------------------------------------------
