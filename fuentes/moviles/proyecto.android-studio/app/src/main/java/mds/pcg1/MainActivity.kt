// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Implementación de la actividad principal
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clase: MainActivity
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

package mds.pcg1

import android.os.Bundle
import android.app.Activity
import android.util.Log
import android.view.Gravity
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import mds.pcg1.gls_view.*



/**
 * Notas sobre aspectos de esta aplicación:
 *
 * Ver el OpenGL Dahsboard: el 93% de los móviles soportan OpenGL ES 3.0 (Marzo 24)
 * https://developer.android.com/about/dashboards#OpenGL
 *   ---> decido usar GLES30 para esta aplicación
 *
 * Código basado en:
 * https://developer.android.com/develop/ui/views/graphics/opengl/environment?authuser=1&hl=es-419
 *
 * Se ha usado (en 'GLSurfaceViewPCG') lo que se indica aquí para gestionar diversos tipos de eventos
 * (en el apartado "Detect All Suported Gestures")
 * https://developer.android.com/develop/ui/views/touch-and-input/gestures/detector
 *
 * Para crear custom Views, sería conveniente ver este artículo:
 * https://dev.to/rubicon_dev/creating-custom-views-in-android-4n8b
 */

// *************************************************************************************************
/**
 * Actividad principal de la aplicación
 * (simplemente crea una 'GLSurfaceViewPCG' al inicio o cada vez que se 'regenera' la app,
 * aquí 'regenerar' incluye rotarla o volver a traerla cuando la has llevado a segundo plano)
 */
class OpenGLES30Activity : Activity()
{

    private lateinit var gl_view : GLSurfaceViewPCG

    /**
     * Función que se ejecuta cada vez que se crea la actividad, eso
     * ocurre cuando se lanza la aplicación, pero también cuando se gira el dispositivo.
     * (se crea una surface, un contexto y una aplicación nuevas)
     */
    public override fun onCreate( savedInstanceState: Bundle? )
    {
        instancia = this
        super.onCreate( savedInstanceState )

        // Create a GLSurfaceView instance and set it
        // as the ContentView for this Activity.

        gl_view = GLSurfaceViewPCG( this )
        setContentView( gl_view )

        // añadir widgets (WIP)
        anadirWidgets()
    }
    // ---------------------------------------------------------------------------------------

    /**
     * Función que añade widgets de pruebas
     */
    private fun anadirWidgets() {

        // crear un 'view' tipo 'botón' y configurarla
        val button = Button(this) ;
        button.text = "Menú"
        button.setOnClickListener{ showPopupMenu(it) }

        // anadir boton como "contenido" adicional en esta actividad.
        val lp =  LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        )
        lp.gravity = Gravity.TOP or Gravity.END
        addContentView( button, lp )
    }
    // ---------------------------------------------------------------------------------------

    /**
     * Función que se ejecuta cuando se pulsa el botón de pruebas
     */
    private fun showPopupMenu(anchor: View?) {
        val TAGF : String = "[OpenGLES30Activity.showPopupMenu]"
        Log.v( TAGF, "** menú pulsado **")
        // val menuView = layoutInflater.inflate(R.layout.menu_layout, null)  // --> da error y AS propone crear el XML del men
        //popupWindow = PopupWindow(menuView, LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT, true)
        //popupWindow.showAsDropDown(anchor)
    }
    // ---------------------------------------------------------------------------------------


    /**
     * 'companion object': incluye las variables y métodos de clase
     */
    companion object {
        public var instancia : OpenGLES30Activity? = null
    }
}

