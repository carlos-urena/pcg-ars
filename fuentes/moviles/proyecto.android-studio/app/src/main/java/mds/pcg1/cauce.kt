// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con el Cauce
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases: indice_atributo, CauceBase
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

package mds.pcg1.cauce


import java.nio.*

import android.content.res.*
import android.opengl.GLES30
import android.util.Log

import mds.pcg1.aplicacion.*
import mds.pcg1.fuentes_luz.ColeccionFuentesLuz
import mds.pcg1.material.Material
import mds.pcg1.texturas.Textura
import mds.pcg1.utilidades.*
import mds.pcg1.vec_mat.*


// -------------------------------------------------------------------------------------------------
// Tipo enumerado para los enteros que son indices de atributos (del 0 al 3)

class ind_atributo
{
    companion object
    {
        val posicion      get() = 0
        val color         get() = 1
        val normal        get() = 2
        val coords_text   get() = 3
        val numero_atribs get() = 4
    }
}

// -------------------------------------------------------------------------------------------------

/** Clase que encapsula un objeto programa de OpenGL y los valores de sus parámetros uniform
 *
 *  Para información básica de referencia, ver lo relativo a compilar shaders en el primer enlace
 *  Se deberían guardar los códigos fuentes de los 'assets' del proyecto y leerlos con el _assets manager_, ver segundo enlace
 *
 *  @see developer.android.com/develop/ui/views/graphics/opengl/draw
 *  @see stackoverflow.com/questions/27574136/where-to-store-shader-code-in-android-app
 *
 */
class CauceBase()
{
    // ---------------------------------------------------------------------------------------------

    private val transponer_mat = true

    private var mat_modelado     : Mat4 = Mat4.ident() // matriz de modelado
    private var mat_modelado_nor : Mat4 = Mat4.ident() // matriz de modelado para normales
    private var mat_vista        : Mat4 = Mat4.ident() // matriz de vista
    private var mat_proyeccion   : Mat4 = Mat4.ident() // matriz de proyección

    private var eval_mil          : Boolean = false // true -> evaluar MIL, false -> usar color plano
    private var usar_normales_tri : Boolean = false // true -> usar normal del triángulo, false -> usar interp. de normales de vértices
    private var eval_text         : Boolean = false // true -> eval textura, false -> usar glColor o glColorPointer

    // color actual para visualización sin tabla de colores ni texturas
    private var color : Vec3 = Vec3( 1.0f, 1.0f, 1.0f )

    // textura actualmente activada en el cauce, null si no hay ninguna (inicialmente ninguna)
    private var textura : Textura? = null

    // material actualmente activo en el cauce
    private var material : Material = Material()

    // máximo número de fuentes de Luz, debe coincidir con el FS
    val max_num_luces = 8

    // colección de fuentes de luz activada actualmente (si es null la iluminación está desactivada)
    private var col_fuentes_luz : ColeccionFuentesLuz? = null

    // pilas de: colores, matrices, materiales, texturas y colecciones de fuentes
    private var pila_colores          : ArrayList<Vec3>     = arrayListOf()
    private var pila_mat_modelado     : ArrayList<Mat4>     = arrayListOf()
    private var pila_mat_modelado_nor : ArrayList<Mat4>     = arrayListOf()
    private var pila_materiales       : ArrayList<Material> = arrayListOf()
    private var pila_texturas         : ArrayList<Textura?> = arrayListOf()
    private var pila_colf             : ArrayList<ColeccionFuentesLuz?> = arrayListOf()

    // locations de los uniforms
    private var loc_mat_modelado      : Int = -1
    private var loc_mat_modelado_nor  : Int = -1
    private var loc_mat_vista         : Int = -1
    private var loc_mat_proyeccion    : Int = -1
    private var loc_eval_mil          : Int = -1
    private var loc_usar_normales_tri : Int = -1
    private var loc_eval_text         : Int = -1
    private var loc_tipo_gct          : Int = -1
    private var loc_coefs_s           : Int = -1
    private var loc_coefs_t           : Int = -1
    private var loc_mil_ka            : Int = -1
    private var loc_mil_kd            : Int = -1
    private var loc_mil_ks            : Int = -1
    private var loc_mil_exp           : Int = -1
    private var loc_num_luces         : Int = -1
    private var loc_pos_dir_luz_ec    : Int = -1
    private var loc_color_luz         : Int = -1
    private var loc_param_s           : Int = -1

    // Objeto programa y objetos shader

    private var programa        : Int = 0 // identificador o nombre del objeto programa
    private var vertex_shader   : Int = 0 // ident. del objeto shader con el VS
    private var fragment_shader : Int = 0 // ident. del objeto shader con el FS

    // Nombre de los archivos que contienen los shaders (deben fijarse antes de llamar a crearObjetoPrograma)

    var nombre_archivo_vs : String = ""
    var nombre_archivo_fs : String = ""

    // ---------------------------------------------------------------------------------------------
    // Variables de clase (companion object)

    companion object
    {
        // Numero de atributos e identificadores de cada atributo.
        public val numero_atributos = 4

        /**
         * crea un 'texture object' de OpenGL a partir de [imagen] y devuelve su identificador
         * Deja la textura 'binded'
         */
        fun CrearTexturaGLES( imagen : Imagen ) : Int
        {
            val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

            ComprErrorGL( "${TAGF} al inicio")

            val level       : Int = 0
            val internalFmt : Int = GLES30.GL_RGB
            val srcFmt      : Int = GLES30.GL_RGB
            val srcType     : Int = GLES30.GL_UNSIGNED_BYTE
            val border      : Int = 0

            // create, bind and fill the texture
            var ids_texturas = IntBuffer.allocate(1)

            GLES30.glGenTextures( 1, ids_texturas )
            val id_textura : Int = ids_texturas[0]

            GLES30.glBindTexture( GLES30.GL_TEXTURE_2D, id_textura )

            GLES30.glTexImage2D( GLES30.GL_TEXTURE_2D, level, internalFmt, imagen.ancho, imagen.alto, border,
                srcFmt, srcType, imagen.pixels  )

            GLES30.glGenerateMipmap( GLES30.GL_TEXTURE_2D )

            // configurar parámetros para interpolación de texels y mipmapping
            GLES30.glTexParameteri( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER,  GLES30.GL_LINEAR_MIPMAP_LINEAR );
            GLES30.glTexParameteri( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER,  GLES30.GL_LINEAR );

            // configurar parámetros de repetición de la textura
            GLES30.glTexParameteri( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_REPEAT );
            GLES30.glTexParameteri( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_REPEAT );

            // ya está (esto deja la textura 'binded'
            ComprErrorGL( "${TAGF} al final")
            return id_textura
        }
    }

    // ---------------------------------------------------------------------------------------------

    /**
     * Activa este cauce, si el objeto programa no estaba creado, lo crea e inicializa aquí.
     * Es importante no usar funciones OpenGL fuera del MGE de revisualizar frame, por eso
     * el objeto programa debe inicializarse la primera vez que se activa (cuando se está
     * ejecutando mgeVisualizarFrame)
     */
    public fun activar()
    {
        val TAGF : String = "[CauceBase.activar]"

        if ( programa == 0 )
            inicializar()
        assert( programa>0 ) { "$TAGF intento de activar un CauceBase que no se ha inicializado (no debería pasar)"}
        GLES30.glUseProgram( programa )
    }
    // ---------------------------------------------------------------------------------------------

    private fun inicializar()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        nombre_archivo_fs = "gles3_fragment_shader.glsl"
        nombre_archivo_vs = "gles3_vertex_shader.glsl"

        Log.v( TAGF, "$TAGF inicio")

        crearObjetoPrograma()
        inicializarUniforms()
        imprimeInfoUniforms()

        Log.v( TAGF, "$TAGF fin")
    }
    // ---------------------------------------------------------------------------

    private fun b2n( b : Boolean ) : Int  { return if (b) 1 else 0 }
    // ---------------------------------------------------------------------------

    /**
     *  Lee las 'locations' de todos los uniforms y les da un
     *  valor inicial por defecto. También inicializa algunas variables de estado.
     */
    private fun inicializarUniforms()
    {
        val TAGF : String = "Cauce.leerLocation"

        assert( programa > 0 ) { "$TAGF imposible inicializar uniforms: el objeto programa no se ha creado"}

        GLES30.glUseProgram( programa )

        // obtener las 'locations' de los parámetros uniform

        loc_mat_modelado      = leerLocation( "u_mat_modelado" )
        loc_mat_modelado_nor  = leerLocation( "u_mat_modelado_nor" )
        loc_mat_vista         = leerLocation( "u_mat_vista" )
        loc_eval_mil          = leerLocation( "u_eval_mil" )
        loc_mat_proyeccion    = leerLocation( "u_mat_proyeccion" )
        loc_eval_mil          = leerLocation( "u_eval_mil" )
        loc_usar_normales_tri = leerLocation( "u_usar_normales_tri" )
        loc_eval_text         = leerLocation( "u_eval_text" )
        loc_tipo_gct          = leerLocation( "u_tipo_gct" )
        loc_coefs_s           = leerLocation( "u_coefs_s" )
        loc_coefs_t           = leerLocation( "u_coefs_t" )
        loc_mil_ka            = leerLocation( "u_mil_ka" )
        loc_mil_kd            = leerLocation( "u_mil_kd" )
        loc_mil_ks            = leerLocation( "u_mil_ks" )
        loc_mil_exp           = leerLocation( "u_mil_exp" )
        loc_num_luces         = leerLocation( "u_num_luces" )
        loc_pos_dir_luz_ec    = leerLocation( "u_pos_dir_luz_ec" )
        loc_color_luz         = leerLocation( "u_color_luz" )
        loc_param_s           = leerLocation( "u_param_s" )

        // dar valores iniciales por defecto a los parámetros uniform

        GLES30.glUniform1i( loc_usar_normales_tri, if (usar_normales_tri) 1 else 0 )

        fijarMatrizModelado( mat_modelado ) // fija matriz de modelado y de modelado de normales
        fijarMatrizVista( mat_vista ) // fija la matriz de modelado
        fijarMatrizProyeccion( mat_proyeccion ) // fija la matriz de proyecccion
        fijarColor( color ) // fija el color por defecto
        fijarMaterial( material ) // fija el material por defecto.
        fijarTextura( textura )  // está inicializado a null, así que se desactivan texturas
        fijarColeccionFuentes( col_fuentes_luz ) // esta ini. a null, así que se desactiva ilum.

        GLES30.glUniform1i( loc_num_luces, 0 ) // por defecto: 0 fuentes de luz activas
        GLES30.glUniform1i( loc_eval_mil, 0 )  // por defecto: no evaluar el MIL.

        // desactivar objeto programa
        //gl.useProgram( null );
    }

    // ---------------------------------------------------------------------------------------------

    /**
     * obtiene el localizador único de un parámetro uniform dado su [nombre]
     * @return localizador (Int), o -1 si no se encuentra ese nombre como uniform o bien sí está pero no se usa en la salida
     */
    private fun leerLocation( nombre : String ) : Int
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        val loc =  GLES30.glGetUniformLocation( programa, nombre )
        if ( loc == -1 )
            Log.v( TAGF, "$TAGF advertencia: el uniform de nombre '$nombre' no se encuentra en el fuente o no se usa en la salida")

        return loc
    }
    // ---------------------------------------------------------------------------

    private fun imprimeInfoUniforms()
    {

    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Cambia la matriz de vista en el objeto programa
     * @param [nueva_mat_vista] (Mat4) nueva matriz de vista
     */
    fun fijarMatrizVista( nueva_mat_vista : Mat4 )
    {
        mat_vista = nueva_mat_vista
        GLES30.glUniformMatrix4fv( loc_mat_vista, 1, transponer_mat, mat_vista.fb )
    }
    // ---------------------------------------------------------------------------

    /**
     * Cambia la matriz de proyeccion en el objeto programa
     * @param [nueva_mat_proyeccion] (Mat4) nueva matriz de proyección
     */
    fun fijarMatrizProyeccion( nueva_mat_proyeccion : Mat4 )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        assert( programa > 0 ) {"$TAGF, no hay programa"}
        GLES30.glUseProgram( programa )

        mat_proyeccion = nueva_mat_proyeccion
        GLES30.glUniformMatrix4fv( loc_mat_proyeccion, 1, transponer_mat, mat_proyeccion.fb )
    }
    // ---------------------------------------------------------------------------

    /**
     * Cambia la matriz de modelado en el objeto programa
     * @param [nueva_mat_modelado] (Mat4) nueva matriz de modelado
     */
    fun fijarMatrizModelado( nueva_mat_modelado : Mat4 )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        assert( programa > 0 ) {"$TAGF, no hay programa"}
        GLES30.glUseProgram( programa )

        mat_modelado     = nueva_mat_modelado
        mat_modelado_nor = nueva_mat_modelado // FALTA TODO:  nueva_mat_modelado.inversa3x3().traspuesta3x3()

        GLES30.glUniformMatrix4fv( loc_mat_modelado, 1, transponer_mat, mat_modelado.fb )
        GLES30.glUniformMatrix4fv( loc_mat_modelado_nor, 1, transponer_mat, mat_modelado_nor.fb )
    }
    // ---------------------------------------------------------------------------

    /**
     * Registra la nueva textura, si es null, desactiva texturas en el OP, en otro caso
     * activa textura en el OP y configura los uniforms de texturas
     */
    fun fijarTextura( nueva_textura : Textura? )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        // registrar la nueva textura
        textura = nueva_textura

        // si la nueva textura es nula, desactivar uso de texturas en el objeto programa
        if ( textura == null )
            GLES30.glUniform1ui( loc_eval_text, 0 )

        // si la nueva textura no es nula, activar la textura en el objeto programa

        textura?.let {

            GLES30.glUniform1ui( loc_eval_text, 1 )
            GLES30.glActiveTexture( GLES30.GL_TEXTURE0 )

            if ( it.id_textura == 0 )
            {
                it.id_textura = CrearTexturaGLES( it.imagen ) // queda binded
                assert( it.id_textura > 0 ) { "$TAGF no se ha podido crear la textura o el identificador es cero " }
            }
            else
                GLES30.glBindTexture( GLES30.GL_TEXTURE_2D, it.id_textura )

            GLES30.glUniform1i( loc_tipo_gct, it.tipo_gct )

            if ( it.tipo_gct != 0 )
            {
                GLES30.glUniform4fv( loc_coefs_s, 1, it.coefs_s.fb );
                GLES30.glUniform4fv( loc_coefs_t, 1, it.coefs_t.fb );
            }
        }
    }
    // ---------------------------------------------------------------------------

    /**
     * Guarda la textura actual en la pila de texturas
     */
    fun pushTextura()
    {
        pila_texturas.add( textura )
    }

    // ---------------------------------------------------------------------------------------------

    /**
     *  restaura la última textura guardada en la pila de texturas
     */
    fun popTextura()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        assert( pila_texturas.size > 0 ) {"$TAGF no se puede hacer 'pop' de la pila de texturas (está vacía)"}
        fijarTextura( pila_texturas.last() )
        pila_texturas.removeLast()
    }

    // ---------------------------------------------------------------------------

    /**
     * Fija el material actualmente activo en el cauce
     */
    fun fijarMaterial( nuevo_material : Material )
    {
        material = nuevo_material
        GLES30.glUniform1f( loc_mil_ka,   material.ka )
        GLES30.glUniform1f( loc_mil_kd,   material.kd )
        GLES30.glUniform1f( loc_mil_ks,   material.ks )
        GLES30.glUniform1f( loc_mil_exp,  material.exp )
    }
    // ---------------------------------------------------------------------------

    /**
     * guarda el material actual en la pila de materiales
     */
    fun pushMaterial( )
    {
        pila_materiales.add( material )
    }
    // ---------------------------------------------------------------------------

    fun popMaterial()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        assert( pila_materiales.size > 0 ) {"$TAGF no se puede hacer 'pop' de la pila de materiales (está vacía)"}
        fijarMaterial( pila_materiales.last() )
        pila_materiales.removeLast()

    }
    // ---------------------------------------------------------------------------------------------
    /**
     * Fija el viewport y lo limpia al color de fondo.
     */
    fun inicializarViewport( org_x : Int, org_y : Int , ancho_vp : Int , alto_vp : Int )
    {
        GLES30.glViewport( org_x, org_y, ancho_vp, alto_vp )
        GLES30.glClearColor(0.4f, 0.4f, 0.4f, 1.0f)
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT or GLES30.GL_DEPTH_BUFFER_BIT)  // 'or' --> bitwise OR
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * establece un valor inicial por defecto de diversos parámetros de rasterización
     */
    fun fijarParametrosRasterizacion()
    {
        GLES30.glEnable( GLES30.GL_DEPTH_TEST )
        GLES30.glDisable( GLES30.GL_CULL_FACE )
    }
    // ---------------------------------------------------------------------------------------------
    /**
     *
     * fija [nueva_coleccion_f] como la nueva colección de fuentes de este cauce (si es nulo se
     * desactiva la iluminación)
     *
     */
    fun fijarColeccionFuentes( nueva_colf : ColeccionFuentesLuz? )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        ComprErrorGL( "$TAGF: al inicio" )

        // registrar la colección (puede ser nula)
        col_fuentes_luz = nueva_colf

        // activar la colección si es no nula
        col_fuentes_luz?.let{

            val nl : Int = it.fuentes.size
            assert( 0 < nl  )  {" ${TAGF} no hay fuentes de luz" }
            assert( nl < max_num_luces )  {" ${TAGF} demasiadas fuentes de luz (hay $nl y el máximo es $max_num_luces)" }

            // crear los dos arrays que se pasan como uniform al objeto programa

            var pos_dir_ecc : ArrayList<Vec4> = ArrayList( nl )  // posiciones o direcciones de las fuentes de luz
            var colores     : ArrayList<Vec3> = ArrayList( nl )  // colores (intensidades) de las fuentes de luz

            for( i in 0..< nl )
            {
                pos_dir_ecc.add( mat_vista * it.fuentes[i].pos_dir_wcc )
                colores.add( it.fuentes[i].color)
            }

            // activar la iluminación y enviar los arrays al objeto programa

            GLES30.glUniform1i( loc_num_luces, nl )
            GLES30.glUniform3fv( loc_color_luz, nl, FloatBuffer.wrap ( ConvFloatArrayV3( colores ) ))
            GLES30.glUniform4fv( loc_pos_dir_luz_ec, nl, FloatBuffer.wrap( ConvFloatArrayV4( pos_dir_ecc ) ))
            GLES30.glUniform1ui( loc_eval_mil, 1 )
        }

        // desactivar la iluminación si es no nula
        if ( col_fuentes_luz == null )
            GLES30.glUniform1ui( loc_eval_mil, 0 )

        ComprErrorGL( "$TAGF: al final" )
    }
    // ---------------------------------------------------------------------------

    fun pushColeccionFuentes()
    {
        pila_colf.add( col_fuentes_luz )
    }

    // ---------------------------------------------------------------------------

    fun popColeccionFuentes()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        assert( pila_colf.size > 0 ) {"$TAGF no se puede hacer 'pop' de la pila de colecciones de fuentes de luz (está vacía)"}
        fijarColeccionFuentes( pila_colf.last() )
        pila_colf.removeLast()
    }
    // ---------------------------------------------------------------------------

    /**
     * Fija el valor de 'S'
     * @param nue_param_s (number)
     */
    fun fijarParamS( nue_param_s : Float )
    {

        GLES30.glUniform1f( this.loc_param_s, nue_param_s ) // cambia parámetro del shader
    }

    // ---------------------------------------------------------------------------
    /**
     * Hace la matriz de modelado igual a la identidad
     */
    fun resetMM(  )
    {
        fijarMatrizModelado( Mat4.ident() )
    }
    // ---------------------------------------------------------------------------

    fun compMM( mat_componer : Mat4 )
    {
        assert( programa > 0 ) {"$TAG, no hay programa"}
        GLES30.glUseProgram( programa )

        mat_modelado = mat_modelado*mat_componer

        GLES30.glUniformMatrix4fv( loc_mat_modelado, 1, transponer_mat, mat_modelado.fb )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * guarda una copia de la matriz de modelado actual en la pila de matrices de modelado
     */
    fun pushMM()
    {
        pila_mat_modelado.add( mat_modelado )
    }
    // ---------------------------------------------------------------------------------------------
    /**
     * restaura la matriz de modelado actual usando la que haya en el tope pila de matrices de modelado
     */
    fun popMM()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        assert( pila_mat_modelado.size > 0 ) {"$TAGF no se puede hacer 'pop' de la pila de matrices de modelado (está vacía)"}
        val prev_mm = pila_mat_modelado.last()
        pila_mat_modelado.removeLast()
        fijarMatrizModelado( prev_mm )
    }

    // ---------------------------------------------------------------------------------------------
    /**
     * Cambia el color actual por defecto en el cauce
     */
    fun fijarColor( nuevo_color : Vec3 )
    {
        color = nuevo_color
        GLES30.glVertexAttrib3fv( ind_atributo.color, color.fb )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * guarda una copia del color actual en la pila de colores
     */
    fun pushColor()
    {
        pila_colores.add( color )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     *  restaura el color actual usando el que haya en el tope de la pila de colores
     */
    fun popColor()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        assert( pila_colores.size > 0 ) {"$TAGF no se puede hacer 'pop' de la pila de colores (está vacía)"}
        fijarColor( pila_colores.last() )
        pila_colores.removeLast()
    }


    // ---------------------------------------------------------------------------------------------
    /**
     * Compilar un shader y, si va bien, adjuntarlo al objeto programa.
     *   - Se muestra el log de errores o warnings.
     *   - Si hay errores se lanza una excepción.
     *
     * @param [tipo_shader]  uno de: GLES30.GL_FRAGMENT_SHADER, GLES30.GL_VERTEX_SHADER,
     * @param [nombre_archivo] (string) nombre del archivo que contiene el texto fuente (se busca en `assets/shaders)
     * @param [texto_fuente] texto fuente del shader.
     */
    private fun compilarAdjuntarShader( tipo_shader : Int, nombre_archivo : String ) : Int
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
        //Log.v( TAG, "Entra función [$TAGF] ")

        // Comprobar precondiciones

        ComprErrorGL( "$TAGF: error de OpenGL al inicio")

        assert( programa > 0 )
            { "$TAGF: el objeto programa no está creado" }

        assert( tipo_shader == GLES30.GL_VERTEX_SHADER || tipo_shader == GLES30.GL_FRAGMENT_SHADER )
            { "$TAGF: El valor de 'tipo_shader' ($tipo_shader) es incorrecto" }

        assert( nombre_archivo != "" )
            { "$TAGF se ha dado un nombre de archivo vacío"}


        // Leer archivo de la carpeta de assets

        var texto_fuente = LeerArchivoDeTexto("shaders/${nombre_archivo}" )

        // Crear y compilar el shader

        val shader = GLES30.glCreateShader( tipo_shader )
        assert( shader > 0 ) { "$TAGF no se ha podido crear el objeto shader " }

        GLES30.glShaderSource( shader, texto_fuente )
        GLES30.glCompileShader( shader )

        // Mostrar errores o warnings

        val info = GLES30.glGetShaderInfoLog( shader )
        if ( info != "" )
        {
            Log.v(TAGF, "")
            Log.v(TAGF, "---------------------------------------------------- ")
            Log.v(TAGF, "Resultado de compilar shader en: '${nombre_archivo}'")
            Log.v(TAGF, "---------------------------------------------------- ")
            Log.v(TAGF, info)
            Log.v(TAGF, "---------------------------------------------------- ")
            Log.v(TAGF, "")
        }

        // Si ha habido error, lanzar excepción de error

        var sin_errores = IntBuffer.allocate( 1 )
        GLES30.glGetShaderiv( shader, GLES30.GL_COMPILE_STATUS, sin_errores )
        if ( sin_errores[0] != GLES30.GL_TRUE )
        {
            val msg = "$TAGF Errores al compilar shader en '${nombre_archivo}'. Aborto."
            Log.v( TAGF, msg )
            throw Error( msg )
        }

        // Adjuntar objeto shader al objeto programa

        GLES30.glAttachShader( programa, shader )
        ComprErrorGL( "$TAGF error OpenGL al final" )

        // Devolver el identificador del objeto programa creado

        Log.v( TAGF, "$TAGF shader en '${nombre_archivo}' compilado ok.")
        return shader
    }
    // ---------------------------------------------------------------------------------------------

    /**
     *  Compila y enlaza el objeto programa, escribe [programa]
     *  (deja nombre en 'id_prog', debe ser 0 antes)
     */
    private fun crearObjetoPrograma()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

        // Comprobar precondiciones

        ComprErrorGL( "${TAGF} error de OpenGL al inicio")

        assert( programa == 0 )
            { "$TAGF: el objeto programa ya está creado" }

        assert( nombre_archivo_vs != "" ) { "No hay nombre del archivo fuente del VS"}
        assert( nombre_archivo_fs != "" ) { "No hay nombre del archivo fuente del FS"}

        // Crear el objeto programa

        programa = GLES30.glCreateProgram() ;
        assert( programa > 0 ) {"$TAGF no se ha podido crear el objeto programa" }


        // Adjuntarle los shaders al objeto programa
        vertex_shader   = compilarAdjuntarShader( GLES30.GL_VERTEX_SHADER,   nombre_archivo_vs )
        fragment_shader = compilarAdjuntarShader( GLES30.GL_FRAGMENT_SHADER, nombre_archivo_fs )

        //Log.v( TAGF, "shader compilados y adjuntados")

        // Asociar los índices de atributos con las correspondientes variables de entrada ("in")
        // del vertex shader (hay que hacerlo antes de enlazar)
        // (esto es necesario para asegurarnos que conocemos el índice de cada atributo específico

        ComprErrorGL( "$TAGF antes de bind de atributos " )

        assert( numero_atributos >= 4 ) { "El cauce no gestiona al menos 4 atributos" }

        GLES30.glBindAttribLocation( programa, ind_atributo.posicion,    "in_posicion_occ" )
        GLES30.glBindAttribLocation( programa, ind_atributo.color,       "in_color" )
        GLES30.glBindAttribLocation( programa, ind_atributo.normal,      "in_normal_occ" )
        GLES30.glBindAttribLocation( programa, ind_atributo.coords_text, "in_coords_textura" )

        ComprErrorGL( "$TAGF después de bind de atributos" )

        // Enlazar programa y ver errores

        GLES30.glLinkProgram( programa )

        // Mostrar errores o warnings

        val info = GLES30.glGetProgramInfoLog( programa )
        if ( info != "")
        {
            Log.v( TAGF, "$TAGF resultado de enlazar el programa:")
            Log.v( TAGF, "---------------------- ")
            Log.v( TAGF, info )
            Log.v( TAGF, "---------------------- ")
        }

        // Si ha habido errores al enlazar, o el objeto programa es inválido, abortar

        var sin_errores = IntBuffer.allocate( 1 )
        GLES30.glGetProgramiv( programa, GLES30.GL_LINK_STATUS, sin_errores )
        if ( sin_errores[0] != GLES30.GL_TRUE )
        {
            val msg = "$TAGF errores al enlazar un programa (ver log). Aborto."
            Log.v( TAGF, msg )
            throw Error( msg )
        }
        if ( ! GLES30.glIsProgram( programa ) )
        {
            val msg = "$TAGF el objeto programa creado no es válido. Aborto."
            Log.v( TAGF, msg )
            throw Error( msg )
        }

        // ya está

        ComprErrorGL( "$TAGF error OpenGL al final" )
        Log.v( TAGF, "$TAGF objeto programa compilado y enlazado ok." )
    }





    // ---------------------------------------------------------------------------------------------
}