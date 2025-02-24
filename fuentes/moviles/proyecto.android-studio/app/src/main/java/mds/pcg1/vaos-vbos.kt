// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con VAOs y VBOs y tablas de atributos.
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases:
// **     + TablasAtributos
// **     + DescrVBOAtrib
// **     + DescrVBOInds
// **     + DescrVAO
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

package mds.pcg1.vaos_vbos

import java.nio.FloatBuffer
import java.nio.IntBuffer
import java.nio.IntBuffer.allocate
import android.opengl.GLES30
import android.util.Log
import mds.pcg1.aplicacion.AplicacionPCG
import mds.pcg1.cauce.CauceBase
import mds.pcg1.cauce.ind_atributo
import mds.pcg1.objeto_visu.ObjetoVisualizable
import mds.pcg1.texturas.Textura
import mds.pcg1.utilidades.*



/**
 * Estructura con las tablas de datos, se usa para dar los datos de entrada
 * necesarios para construir un VAO. Hay que inicializar al menos las posiciones
 */

class TablasAtributos( p_posiciones : FloatArray )
{
    // tablas de posiciones y resto de atributos
    val posiciones   : FloatArray = p_posiciones
    var colores      : FloatArray ? = null
    var normales     : FloatArray ? = null
    var coords_text  : FloatArray ? = null

    // tabla de índices
    var indices      : IntArray ?  = null
}

// ----------------------------------------------------------------------------
/**
 * Descriptor de un VBO (Vertex Buffer Object) con una tabla de un atributo de vértices
 *   - [p_index] índice de esta tabla de atributo
 *   - [p_size]  número de valores flotantes en cada tupla
 *   - [p_data]  valores flotantes para guardar en el VBO
 */
class DescrVBOAtrib( p_index : Int, p_size : Int, p_data : FloatArray )
{

    // índice de esta tabla de atributos
    private var index: Int = p_index

    // número de tuplas de valores en la tabla
    private var count: Int = p_data.size / p_size


    // tamaño de cada tupla en la tabla de datos
    private var size: Int = p_size

    // identificador del objeto buffer de OpenGL (VBO)
    // (0 antes de crearlo)
    private var buffer: Int = 0

    // tamaño de cada valor en bytes (por ahora usamos floats de 4 bytes)
    private var val_size : Int = 4

    // tamaño en bytes del buffer completo
    private var tot_size = count * size * val_size

    // referencia a los datos (únicamente para lectura).
    private val data: FloatArray = p_data

    // -------------------------------------------------------------------------------------------

    init {
        // Comprobar que está todo correcto al construir el VBO
        comprobar()
    }
    // -------------------------------------------------------------------------------------------

    fun get_count(): Int // ponerlo como getter de count
    {
        return count
    }

    fun get_index(): Int // ponerlo como getter de index
    {
        return index
    }

    // -------------------------------------------------------------------------------------------

    private fun comprobar()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        assert(2 <= size && size <= 4) { "${TAGF} 'size' (== ${size}) must be between 2 and 4" }
        assert(1 <= data.size / size) { "${TAGF} 'size' (== ${size}) cannot be greater than 'data.length' (== ${data.size}) " }
        assert(data.size % size == 0) { "${TAGF}: número de valores en 'data' (${data.size}) no es múltiplo de 'size' (${size}) " }
        assert( tot_size == count*size*4 ) { "{TAGF} tamaño en bytes ($tot_size) es incoherente"}
    }
    // -------------------------------------------------------------------------------------------

    /**
     * Crea el VBO en la GPU y lo registra en el VAO activo, inicializa [buffer]
     * Usa los datos en [data] como fuentes.
     */
    fun crearVBO(  )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"
        ComprErrorGL( "$TAGF : hay un error de OpenGL a la entrada")

        // crear el identificador de buffer en 'buffer'
        var ident_buffer : IntBuffer = allocate( 1 ) // array con un entero con id. de buff.
        GLES30.glGenBuffers( 1, ident_buffer )
        buffer = ident_buffer[0]
        assert( buffer != 0 ) { "${TAGF} no se ha podido crear el buffer" }

        // copiar datos hacia la GPU
        GLES30.glBindBuffer( GLES30.GL_ARRAY_BUFFER, buffer )
        GLES30.glBufferData( GLES30.GL_ARRAY_BUFFER, tot_size, FloatBuffer.wrap( data ),
                             GLES30.GL_STATIC_DRAW )
        assert( GLES30.glIsBuffer( buffer )) { "${TAGF} el buffer creado es inválido" }

        // registrar y activar el VBO en el VAO activo
        GLES30.glVertexAttribPointer( index, size, GLES30.GL_FLOAT, false, 0, 0  )
        GLES30.glBindBuffer( GLES30.GL_ARRAY_BUFFER, 0 )
        GLES30.glEnableVertexAttribArray( index ) // por defecto la tabla queda habilitada en el VAO.

        // ya está
        ComprErrorGL( "${TAGF}: hay un error de OpenGL a la salida")
    }

}  // final de DescrVBO


// -------------------------------------------------------------------------------------------------

/**
 * Descriptor de VBOs (Vertex Buffer Object) de índices
 */

class DescrVBOInd( p_indices : IntArray )
{

    // -------------------------------------------------------------------------------------------
    // Variables de instancia

    // copia de los indices, propiedad de este objeto

    val indices : IntArray = p_indices

    // Tipo de los valores (GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT)
    // Por ahora, únicamente unsigned de 32 bits
    //
    // Cuidado: el array 'indices' es de Int, no de UInt. OpenGL lo reinterpreta como unsigned,
    // pero Ints negativos se interpretan como positivos.

    val type : Int = GLES30.GL_UNSIGNED_INT

    // Cuenta de índices (>0)
    val count : Int = p_indices.size

    // Tamaño de cada valor en bytes (el correspondiente a 'type')
    private var val_size : Int = 4

    // Tamaño en bytes del buffer completo
    private var tot_size = count * val_size

    // VBO OpenGL, null hasta que se crea (con 'crearVVBO')
    var buffer : Int = 0

    // -------------------------------------------------------------------------------------------

    init {
        // Comprobar que está todo correcto al construir el VBO
        comprobar()
    }
    // -------------------------------------------------------------------------------------------

    private fun comprobar() {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        assert( count > 0 && count == indices.size ) { "$TAGF cuenta de índices es 0 o incoherente" }
        assert( tot_size == count*val_size ) { "$TAGF total de bytes incoherente"}
    }
    // -------------------------------------------------------------------------------------------

    fun get_count(): Int // ponerlo como getter de count
    {
        return count
    }
    // -------------------------------------------------------------------------------------------

    /**
     * Devuelve el tipo OpenGL de los índices
     * @returns variable de instancia 'type'
     */
    fun get_type() : Int
    {
        return this.type
    }
    // -------------------------------------------------------------------------------------------

    /**
     * Crea el VBO en la GPU, inicializa [buffer] con el identificador de buffer
     */
    fun crearVBO( )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        ComprErrorGL( "${TAGF} error de OpenGL al inicio" )

        // crear y activar el buffer
        var ident_buffer  : IntBuffer = allocate( 1 ) // array de enteros con un entero con el identificador del buffer
        GLES30.glGenBuffers( 1, ident_buffer )
        buffer = ident_buffer[0]
        assert( buffer != 0 ) { "${TAGF} no se ha podido crear el buffer" }

        GLES30.glBindBuffer( GLES30.GL_ELEMENT_ARRAY_BUFFER, buffer )
        GLES30.glBufferData( GLES30.GL_ELEMENT_ARRAY_BUFFER, tot_size, IntBuffer.wrap( indices ),
                             GLES30.GL_STATIC_DRAW )
        assert( GLES30.glIsBuffer( buffer )) { "${TAGF} el buffer creado es inválido" }

        Log.v( TAGF, "creado VBO de índices, buffer == $buffer, tot_size == $tot_size bytes, indices.long == ${indices.size}, type == $type, GL_INT == ${GLES30.GL_INT}")

        // ya está
        ComprErrorGL( "${TAGF} error de OpenGL al final")
    }

} // final DescrVBOInd



// -------------------------------------------------------------------------------------------------
/**
 *   Descriptor de VAO (Vertex Array Object)
 */
open class DescrVAO( tablas : TablasAtributos )
{

    // --------------------------------------------------------------------
    // Variables de instancia

    public var nombre : String = "no asignado"

    // identificador o nombre del Vertex Array Object (0 antes de crearse)
    var array : Int = 0

    // número de tablas de atributos que tiene el VAO (incluyendo las posicione)
    // como máximo será el numero total de atributos que gestiona el cauce
    private var num_atribs : Int = 0

    // número de vértices en la tabla de posiciones de vértices
    private var count : Int = 0

    // número de índices en la tabla de índices (si hay índices, en otro caso 0)
    private var idxs_count : Int = 0

    // si hay índices, tiene el tipo de los índices
    private var idxs_type : Int = 0

    // si la secuencia es indexada, VBO de attrs, en otro caso 'null'
    private var dvbo_indices : DescrVBOInd ? = null

    // array con los descriptores de VBOs de atributos
    private var dvbo_atributo : Array< DescrVBOAtrib? > = emptyArray()

    // array que indica si cada tabla de atributos está habilitada o deshabilitada
    private var atrib_habilitado : Array<Boolean> = emptyArray()

    // ---------------------------------------------------------------------------------------------
    /**
     * Crea un descriptor de VAO, dando un descriptor del VBO de posiciones de vértices
     */
    init
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        //ComprErrorGL( "$TAGF error al inicio")

        num_atribs = CauceBase.numero_atributos

        // inicializar tablas de VBOs y de estado habilitado/deshabilitado
        for( i in 0..<num_atribs )
        {
            dvbo_atributo += null    // inicialmente no hay tabla de atributos (son null)
            atrib_habilitado += true // habilitado por defecto (si no es null)
        }

        // crear el VBO de posiciones (siempre tiene que estar)
        val dvbo_posiciones = DescrVBOAtrib( ind_atributo.posicion, 3, tablas.posiciones )

        this.dvbo_atributo[0] = dvbo_posiciones
        this.count = dvbo_posiciones.get_count()

        // si hay tablas de atributos o de índices, crear y añadir cada correspondiente VBO

        tablas.indices?.let     { agregar_tabla_ind( it ) }
        tablas.colores?.let     { agregar_tabla_atrib_v3( ind_atributo.color, it ) }
        tablas.normales?.let    { agregar_tabla_atrib_v3( ind_atributo.normal, it ) }
        tablas.coords_text?.let { agregar_tabla_atrib_v2( ind_atributo.coords_text, it ) }

        //ComprErrorGL( "$TAGF error al final")
    }
    // -------------------------------------------------------------------------------------------------
    /**
     * Añade al VAO un descriptor de VBO de atributos, se llama desde 'agregar'
     * @param dvbo_atributo
     */
    fun agregar_atrib( p_dvbo_atributo : DescrVBOAtrib )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        val index = p_dvbo_atributo.get_index()

        assert(index in 1..<num_atribs) { "intro de agregar VBO con índice fuera de rango" }
        assert( count == p_dvbo_atributo.get_count() ) { "$TAGF intento de añadir un descriptor de atributo con un número de tuplas distinto al de vértices" }
        // registrar el descriptor de VBO en la tabla de descriptores de VBOs de atributos
        dvbo_atributo[index] = p_dvbo_atributo
    }
    // -------------------------------------------------------------------------------------------------

    /**
     * Crea un descriptor de VBO de atributos con tuplas de 2 elementos, y lo añade al VAO
     * @param dvbo_atributo
     */
    fun agregar_tabla_atrib_v2( index : Int, tabla : FloatArray )
    {
        agregar_atrib( DescrVBOAtrib( index, 2, tabla ) )
    }
    // -------------------------------------------------------------------------------------------------

    /**
     * Crea un descriptor de VBO de atributos con tuplas de 3 elementos, y lo añade al VAO
     * @param dvbo_atributo
     */
    fun agregar_tabla_atrib_v3( index : Int, tabla : FloatArray )
    {
        agregar_atrib( DescrVBOAtrib( index, 3,  tabla ) )
    }
    // -------------------------------------------------------------------------------------------------

    /**
     * Añade al VAO un descriptor de VBO de índices, se llama desde 'agregar'
     * @param dvbo_atributo
     */
    fun agregar_ind( p_dvbo_indices : DescrVBOInd )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"
        idxs_count = p_dvbo_indices.get_count()
        idxs_type  = p_dvbo_indices.get_type()  /// ????
        dvbo_indices = p_dvbo_indices

        Log.v( TAGF, "idxs_count == $idxs_count, idxs_type == $idxs_type, type GL_INT == ${GLES30.GL_INT}")
    }
    // -------------------------------------------------------------------------------------------------

    /**
     * Crea un descriptor de VBO de índices y lo añade al VAO
     * @param dvbo_atributo
     */
    fun agregar_tabla_ind( tabla : IntArray )
    {
        agregar_ind( DescrVBOInd( tabla ) )
    }

    // ---------------------------------------------------------------------------------------------

    /**
     * Crea todos los VBOs de este VAO, deja el array 'binded'
     */
    fun crearActivarVAO()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        assert( array == 0 ) { "${TAGF} ya se había creado este VAO"}
        assert( dvbo_atributo[0] != null ) { "${TAGF} no se puede crear los VBOs: no hay posiciones"}
        ComprErrorGL( "${TAGF} error de OpenGL a la entrada" )

        // crear y activar el VAO
        val ident_array : IntBuffer = allocate( 1 ) // array de enteros con un entero con el identificador del buffer
        GLES30.glGenVertexArrays( 1, ident_array )
        array = ident_array[0]
        assert( array != 0 ) { "${TAGF} no se ha podido crear el array" }
        GLES30.glBindVertexArray( array )

        // crear (y habilitar) los VBOs de posiciones, resto de atributos e índices de este VAO

        this.dvbo_atributo[0]?.crearVBO() ?:
            throw Error( "$TAGF VBO de posiciones es nulo al intentar crearlo...")

        for( i in 0 ..<num_atribs )
            dvbo_atributo[i]?.crearVBO()

        this.dvbo_indices?.crearVBO()

        // deshabilitar tablas presentes pero que no estén habilitadas
        for( i in 0..<num_atribs ) {
            if ( dvbo_atributo[i] != null )
                if ( ! this.atrib_habilitado[i] )
                    GLES30.glDisableVertexAttribArray( i )
        }

        // ya está: comprobar post-condiciones
        assert( GLES30.glIsVertexArray( array ) ) {"$TAGF no se ha podido crear un VAO válido"}
        ComprErrorGL( "${TAGF} error de OpenGL a la salida" )
    }
    // -------------------------------------------------------------------------------------------------

    /**
     * habilita o deshabilita el atributo con índice [index], si [habilitar] es true, habilita, si
     * es false, deshabilita.
     */
    fun habilitarAtrib( index : Int, habilitar : Boolean )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        // comprobar precondiciones
        assert( 0 < index) { "${TAGF} índice es cero o negativo" }
        assert( index < this.num_atribs ) { "${TAGF} índice fuera de rango" }
        assert( this.dvbo_atributo[index] != null ) { "${TAGF} no se puede habilitar/deshab. un atributo sin tabla" }

        atrib_habilitado[index] = habilitar

        // si el VAO ya se ha enviado a la GPU, actualizar estado del VAO en OpenGL
        if ( array != 0 )
        {
            ComprErrorGL( "${TAGF} antes de hacer bind del VAO")
            GLES30.glBindVertexArray( array )

            if ( habilitar )
                GLES30.glEnableVertexAttribArray( index );
            else
                GLES30.glDisableVertexAttribArray( index );
        }
    }

    // -------------------------------------------------------------------------------------------------

    /**
     * Visualiza el VAO, si no está creado en la GPU, lo crea.
     * (para los tipos de primitivas, ver: )
     *
     * @param [mode] (Int) tipo de primitiva (GL_LINES, GL_TRIANGLES, etc...)
     */
    fun draw( mode : Int )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

        ComprErrorGL( "${TAGF} al inicio (vao==${nombre})" )

        if ( array == 0 )
            this.crearActivarVAO()
        else
            GLES30.glBindVertexArray( array )

        ComprErrorGL( "${TAGF} vao creado, antes draw" )

        //Log.v(TAGF, " va draw, idxs_type == $idxs_type, GL_INT == ${GLES30.GL_INT}, GL_UNSIGNED_INT == ${GLES30.GL_UNSIGNED_INT}")

        if ( this.dvbo_indices == null )
            GLES30.glDrawArrays( mode, 0, count )
        else
            GLES30.glDrawElements( mode, idxs_count, idxs_type, 0 )

        ComprErrorGL( "${TAGF} al final, antes desact. (vao==${array}) ")
        GLES30.glBindVertexArray( 0 )
        ComprErrorGL( "${TAGF} al final, después desact  (vao==${array}) ")
    }

} // fin de la clase DescrVAO

// -------------------------------------------------------------------------------------------------
/**
 * A simple indexed vertex sequence (with colors)
 */

fun DescrVAOHelloTriangle(  ) : DescrVAO
{
    // crear objeto 'TablasAtributos' dando las posiciones

    var tablas = TablasAtributos( floatArrayOf(
        -0.6f, -0.6f, 0.0f,
        +0.6f, -0.6f, 0.0f,
        0.0f,  0.6f, 0.0f
    ))

    // añadir los colores
    tablas.colores = floatArrayOf(
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            )

    // añadir las coordenadas de textura
    tablas.coords_text = floatArrayOf(
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.5f, 0.0f
    )

    // opcionalmente, añadir índices (poner o quitar para testear indexado/no indexado)
    tablas.indices = intArrayOf( 0, 1, 2 )

    // crear el descriptor de VAO y devolverlo.
    var dvao = DescrVAO( tablas )

    dvao.nombre = "VAO Hello-Triangle"
    return dvao
}

// *************************************************************************************************

class HelloTriangle : ObjetoVisualizable()
{
    private val dvao_tri : DescrVAO

    init
    {
        nombre   = "Hello Triangle"
        textura  = Textura("imgs/madera2.png")
        dvao_tri = DescrVAOHelloTriangle()
    }
    override fun visualizar()
    {
        //val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        val cauce = AplicacionPCG.instancia.leer_cauce

        guardarCambiarEstado( cauce )
        dvao_tri.draw( GLES30.GL_TRIANGLES )
        restaurarEstado( cauce )
    }
}
