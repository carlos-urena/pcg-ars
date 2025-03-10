// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases y funciones relacionadas con la clase AplicacionPCG
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases: AplicacionPCG
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

package mds.pcg1.aplicacion


import kotlin.math.*

import android.opengl.GLES30
import android.util.Log
import android.view.MotionEvent

import mds.pcg1.utilidades.*
import mds.pcg1.vec_mat.*
import mds.pcg1.gls_view.*
import mds.pcg1.cauce.*
import mds.pcg1.vaos_vbos.*
import mds.pcg1.camaras.*
import mds.pcg1.fuentes_luz.ColeccionFuentesLuz
import mds.pcg1.fuentes_luz.FuenteLuz
import mds.pcg1.objeto_comp.*
import mds.pcg1.malla_ind.*
import mds.pcg1.malla_ply.MallaPLY
import mds.pcg1.material.Material
import mds.pcg1.objeto_visu.*
import mds.pcg1.texturas.*

// -------------------------------------------------------------------------------------------------

/**
 * Clase que actua como controlador, contiene referencias a las vista GL, el cauce, etc...
 */
class AplicacionPCG( p_gls_view: GLSurfaceViewPCG )
{
    var gls_view : GLSurfaceViewPCG = p_gls_view

    private var ancho_vp : Int = 0 // ancho del viewport en pixels
    private var alto_vp  : Int = 0 // alto del viewport en pixels


    private var touch_ult_x : Float = 0.0f  // coordenada X (raw) del último evento touch
    private var touch_ult_y : Float = 0.0f  // coordenada Y (raw) del último evento touch

    private var cauce_act : CauceBase? = null  ; // cauce en uso para hacer el render, se crea en la primera visualización

    private var pinch_ult_fe = 1.0f

    private var ejes = ObjetoEjes()

    // color por defecto inicial
    private val color_ini = Vec3( 0.95f, 0.95f, 1.0f )

    // material por defecto inicial
    private val material_ini = Material(0.1f, 0.4f, 1.5f, 64.0f )

    // colección de objetos visualizables y sus correspondientes cámaras interactivas (una por objeto)
    private var objetos  : MutableList<ObjetoVisualizable> = mutableListOf()
    private var camaras  : MutableList<CamaraInteractiva>  = mutableListOf()

    // coleccion de fuentes de luz que se usa para los objetos con normales
    private val fuente1 = FuenteLuz(
        Vec4( -1.0f, 1.0f, 0.5f, 0.0f ), // dirección de la 1a fuente de luz (w==0)
        Vec3( 1.0f, 1.0f, 1.0f )             // color de la 1a fuente de luz
    )
    private val fuente2 = FuenteLuz(
         Vec4( +1.0f, -0.5f, 0.3f, 0.0f ), // dirección de la 2a fuente de luz (w==0)
         Vec3( 0.4f, 0.2f, 0.2f )              // color de la 2a fuente de luz
    )
    private val coleccion_fuentes = ColeccionFuentesLuz( arrayListOf( fuente1, fuente2 ) )

    // índice del objeto actual
    var ind_objeto_act = 0

    // objeto actual, va cambiando con el índice
    private var objeto_act : ObjetoVisualizable

    // cámara actual, va cambiando con el objeto actual
    private var camara_act : CamaraInteractiva

    // valor actual del parámetro S (entre 0 y 1)
    var param_S : Float = 0.5f

    // ---------------------------------------------------------------------------------------------

    init
    {
        // únicamente se puede crear una instancia de la clase 'AplicacionPCG'
        assert( instancia_act == null ) { "intento de crear una aplicación cuando ya hay otra creada" }

        // añadir objetos al vector de objetos (con sus correspondientes cámaras)

        objetos.add( MallaPLY("beethoven.ply") )
        camaras.add( CamaraOrbital3D( 512, 512 ) )

        objetos.add( MallaPLY("big_dodge.ply") )
        camaras.add( CamaraOrbital3D( 512, 512 ) )

        objetos.add( DosCuadrados() )
        camaras.add( CamaraOrbital3D( 512, 512 ) )

        objetos.add( HelloTriangle() )
        camaras.add( CamaraVista2D( 512, 512) )

        // verificar requisitos sobre las listas de objetos y de cámaras
        assert( objetos.size > 0 ) {"no se han creado objetos en el ctor de la aplic."}
        assert( objetos.size == camaras.size ) {"tamaño de 'objetos' y 'camaras' difieren en el ctor de la aplic."}

        // registrar la camara actual como la camara del objeto actual
        camara_act = camaras[ ind_objeto_act ]

        // registrar el objeto actual
        objeto_act = objetos[ ind_objeto_act ]

        // registrar la instancia ya creada, esto debe ser lo ultimo pues a partir de aquí se puede usar.
        instancia_act = this


    }

    companion object
    {
        // instancia de la clase AplicacionPCG que está en ejecución (puede ser nula)
        var instancia_act : AplicacionPCG ? = null

        // lee la instancia y la devuelve, si es nula se produce una excepción
        val instancia : AplicacionPCG get()
        {
            return instancia_act ?: throw Error("Error en AplicacionPCG.instancia: la instancia actual es nula.")
        }
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Método gestor de evento de visualización de un frame,
     * se debe llamar cuando cambia algo del modelo o los parámetros de visualización,
     * su ejecución se puede forzar con el método `requestRender` de [gls_view]
     */
    fun mgeVisualizarFrame()
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"


        // comprobar precondiciones
        assert( objetos.size > 0 )
        assert( camaras.size == objetos.size )
        assert( 0 <= ind_objeto_act && ind_objeto_act < objetos.size  )

        // poner el cauce en un estado adecuado
        inicializarCauceInicioFrame()

        // obtener el cauce de esta instancia
        val cauce = leer_cauce

        // visualizar los ejes
        ejes.visualizar()

        // visualizar el objeto actual
        objeto_act.visualizar()

        // visualizar las normales (activar cambiando 'false' por 'true', por ahora)
        if ( false && objeto_act.tieneNormales )
        {
            cauce.fijarColeccionFuentes(null )
            cauce.fijarTextura( null )
            cauce.fijarColor(Vec3(1.0f, 0.9f, 0.8f))
            objeto_act.visualizarNormales()
        }

        // visualizar las aristas en negro con texturas e ilumijnación desactivadas
        // (no es posible hasta que no se genere la tabla de aristas de la malla, y el VAO corresp.)
        //cauce.fijarColor( Vec3( 0.0f, 0.0f, 0.0f ))
        //cauce.fijarEvalText( false, 0 )
        //objeto_act.visualizarAristas()

        // ya está.

        //Log.v(TAGF, "$TAGF fin")
    }
    // ---------------------------------------------------------------------------------------------
    /**
     * Inicialización del cauce que se hace al inicio de cada frame
     */
    private fun inicializarCauceInicioFrame()
    {
        // obtener el cauce de esta instancia
        val cauce = leer_cauce

        // activar el cauce (pone en uso el objeto programa)
        cauce.activar()

        // inicializa diversos parámetros de rasterización en el cauce
        cauce.fijarParametrosRasterizacion()

        // resetear la matriz de modelado
        cauce.resetMM()

        // por defecto no se usan texturas (hasta que algún objeto lo cambie)
        cauce.fijarTextura( null )

        // fijar el color por defecto para los objetos que no tengan ninguno
        cauce.fijarColor( color_ini )

        // material por defecto.
        cauce.fijarMaterial( material_ini )

        // si el objeto tiene normales, activar iluminación, en otro caso desactivar
        if ( objeto_act.tieneNormales )
            cauce.fijarColeccionFuentes( coleccion_fuentes )
        else
            cauce.fijarColeccionFuentes( null )

        // establecer el valor del parámetro S para las actividades
        cauce.fijarParamS( param_S )

        // configurar y activar la cámara
        camara_act.fijarViewport( ancho_vp, alto_vp )
        camara_act.activar( cauce )

        // inicializar el viewport (fijar dimensiones y limpiarlo)
        cauce.inicializarViewport( 0, 0, ancho_vp, alto_vp )

    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Método que procesa un evento tipo _touch_
     * @param [me] objeto tipo `MotionEvent` con los atributos del evento
     */
    //@RequiresApi(Build.VERSION_CODES.Q)
    fun mgeInicioMover( rawX : Float, rawY : Float  )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        //Log.v( TAGF, "$TAGF rawX == $rawX - rawY == $rawY")

        touch_ult_x = rawX
        touch_ult_y = rawY
    }
    // ---------------------------------------------------------------------------------------------

    fun actualizarS( rawX : Float )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        //param_S = max( 0.0f, min( 1.0f, rawX/(ancho_vp.toFloat()) ))

        val m  = 0.15f   // margen a ambos lados
        val fx = (rawX/(ancho_vp.toFloat())) // fracción de X

        param_S = max( 0.0f, min( 1.0f, (fx-m)/(1.0f-2.0f*m) ))

        Log.v( TAGF, "Valor de S actualizado a: $param_S")

        gls_view.requestRender()
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Método que procesa un evento tipo _touch_
     * @param [me] objeto tipo `MotionEvent` con los atributos del evento
     */
    fun mgeMover( rawX : Float, rawY : Float  )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        val incr_x_raw =  rawX - touch_ult_x
        val incr_y_raw =  touch_ult_y - rawY
        val pry        = rawY / alto_vp.toFloat()

        //Log.v( TAGF, "$TAGF touch move, delta == ${incr_x_raw} , ${incr_y_raw}")
        //Log.v( TAGF, "$TAGF pry = $pry")

        if ( pry > 0.9f )
            actualizarS( rawX )
        else
        {
            camara_act.mover(incr_x_raw, incr_y_raw)
            touch_ult_x = rawX
            touch_ult_y = rawY
            gls_view.requestRender()
        }
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Método gestor de inicio de evento de un evento 'de escala' (_pinch in/out_)
     * @param [fe] (Float) factor de escala inicial (es siempre 1)
     */
    fun mgeInicioPinchInOut( fe : Float )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        //Log.v( TAGF, "$TAGF INICIO escala: fe == $fe")

        pinch_ult_fe = fe
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Método gestor de evento de un evento 'de escala' (_pinch in/out_)
     * @param [fe] (Float) factor de escala, >1 _pinch **out**_, <1 _pinch in_
     */
    fun mgePinchInOut( fe : Float  )
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        val factor_relativo = fe/pinch_ult_fe // factor relativo, respecto del inicio o del anterior
        pinch_ult_fe = fe

        //Log.v( TAGF, "$TAGF escala: fe == $fe - factor.rel == $factor_relativo ")

        camara_act.zoom( factor_relativo )
        gls_view.requestRender()
    }
    // ---------------------------------------------------------------------------------------------


    /**
     * Método gestor de evento de cambio de tamaño,
     * Recibe el nuevo tamaño [nuevo_ancho]  [nuevo_alto]
     * Simplemente registra el tamaño (lo usa al visualizar).
     */
    fun mgeCambioTamano( nuevo_ancho : Int, nuevo_alto : Int )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        Log.v( TAGF, "$TAGF dimensiones de la superficie = $nuevo_alto x $nuevo_ancho")

        alto_vp  = nuevo_alto
        ancho_vp = nuevo_ancho
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Método gestor del evento de pulsacion larga (pulsar y dejar pulsado un poco antes de levantar)
     * Actualiza el objeto actual de la escena, pasa al siguiente objeto.
     */
    fun mgeLongPress( me : MotionEvent)
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        val pry = me.rawY / alto_vp.toFloat()

        if ( pry > 0.8f )
            return

        ind_objeto_act = ( ind_objeto_act + 1 ) % objetos.size

        objeto_act = objetos[ ind_objeto_act ]
        camara_act = camaras[ ind_objeto_act ]

        Log.v( TAGF,"Objeto actual ${ind_objeto_act+1} / ${objetos.size} '${objeto_act.nombre}'")

        gls_view.requestRender()
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Devuelve el cauce en uso en la aplicación (nunca nulo)
     * Si el cauce no existe, se crea
     */
    val leer_cauce : CauceBase get()
        {
            val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

            if ( cauce_act == null )
                cauce_act = CauceBase()

            return cauce_act ?: throw Error( "$TAGF el cauce es nulo, esto no debería pasar")
        }

    // ---------------------------------------------------------------------------------------------


}

