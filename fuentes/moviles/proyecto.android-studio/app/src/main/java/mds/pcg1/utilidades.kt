// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Funciones y variables auxiliares
// ** Copyright (C) 2024 Carlos Ureña
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

package mds.pcg1.utilidades

import android.R.attr.bitmap
import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES30
import android.util.Log

import java.io.BufferedReader
import java.io.IOException
import java.io.InputStream
import java.io.InputStreamReader
import java.nio.ByteBuffer
import java.util.stream.Collectors

import mds.pcg1.OpenGLES30Activity
import mds.pcg1.objeto_visu.ObjetoVisualizable
import mds.pcg1.vaos_vbos.DescrVAO
import mds.pcg1.vaos_vbos.TablasAtributos
import mds.pcg1.vec_mat.*
import java.nio.IntBuffer


val nfnd : String = "No disponible"  // nombre de función no disponible (para TAGF)

// -------------------------------------------------------------------------------------------------

/**
 * Etiqueta para mensajes impresos con Log.v
 * Ver: [esto](https://stackoverflow.com/questions/45841067/what-is-the-best-way-to-define-log-tag-constant-in-kotlin))
 * @return 'CUA +  nombre de la clase'
 */
val Any.TAG : String get()
    {
        val tag = javaClass.simpleName
        return "CUA $tag"
    }

// -------------------------------------------------------------------------------------------------

/**
 * Comprueba y desactiva la variable interna de error de OpenGL, si estaba activada, lanza una
 * excepción de error con el mensaje en [msg], en otro caso no hace nada.
 *
 * Descripciones de los errores obtenidas de:
 * https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glGetError.xhtml
 */
fun ComprErrorGL( donde : String )
{
    val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
    val codigo_error : Int = GLES30.glGetError()

    if ( codigo_error == GLES30.GL_NO_ERROR )
        return

    val descripcion_error : String = when ( codigo_error )
    {
        GLES30.GL_INVALID_ENUM -> "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag."
        GLES30.GL_INVALID_VALUE -> "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag."
        GLES30.GL_INVALID_OPERATION -> "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag."
        GLES30.GL_INVALID_FRAMEBUFFER_OPERATION -> "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag."
        GLES30.GL_OUT_OF_MEMORY -> "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded."
        else -> "No hay descripción de texto disponible para este código de error."
    }

    Log.v( TAGF, "")
    Log.v( TAGF, "--------------------------------------------------")
    Log.v( TAGF, "Error de OpenGL en: $donde" )
    Log.v( TAGF, "Código de error == $codigo_error")
    Log.v( TAGF, "Descripción     == $descripcion_error")
    Log.v( TAGF, "--------------------------------------------------")
    Log.v( TAGF, "")


    throw Error( "$TAGF Error OpenGL en: $donde (ver descripción)" )

}
// -------------------------------------------------------------------------------------------------

/**
 * Lee un archivo con texto en la carpeta de assets.
 * Devuelve el archivo completo como un String (respetando las lineas).
 * Si hay cualquier problema al leer, se aborta con una excepción
 *
 * @param [nombre_archivo] Nombre del archivo con extensión,
 *                         puede incluir path, relativo a la carpeta de assets.
 * @return cadena con el nombre del archivo
 *
 * @see stackoverflow.com/questions/27574136/where-to-store-shader-code-in-android-app
 * @see stackoverflow.com/questions/309424/how-do-i-read-convert-an-inputstream-into-a-string-in-java
 *
 */
fun LeerArchivoDeTexto( nombre_archivo : String ) : String
{
    val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"

    var assets : AssetManager = OpenGLES30Activity.instancia?.applicationContext?.assets
        ?: throw Error("$TAGF no puedo recuperar el 'Assets manager'")

    var istream: InputStream

    try {
        istream = assets.open( nombre_archivo )
    }
    catch( e : java.io.FileNotFoundException )
    {
        throw Error("$TAGF No encuentro el archivo '${nombre_archivo}' en la carpeta de assets.")
    }

    // leer istream en un String usando "Stream API"

    val texto_archivo : String = BufferedReader( InputStreamReader( istream ) )
        .lines().collect(Collectors.joining("\n"));

    istream.close()

    return texto_archivo
}
// -------------------------------------------------------------------------------------------------

/**
 * Clase para encapsular los bytes y dimensiones de una imagen RGB
 */
class Imagen( p_pixels : ByteBuffer, p_ancho : Int, p_alto : Int )
{
    var pixels : ByteBuffer = p_pixels
    var ancho  : Int        = p_ancho
    var alto   : Int        = p_alto
}
// -------------------------------------------------------------------------------------------------

/**
 * Lee un archivo con una imagen en la carpeta 'assets'
 * Devuelve el archivo completo como un objeto 'Imagen'
 *
 * @param [nombre_archivo] Nombre del archivo con extensión,
 *                         puede incluir path, relativo a la carpeta de assets.
 * @return cadena
 *
 * @see stackoverflow.com/questions/27574136/where-to-store-shader-code-in-android-app
 * @see stackoverflow.com/questions/309424/how-do-i-read-convert-an-inputstream-into-a-string-in-java
 *
 */
fun LeerArchivoImagen( nombre_archivo : String ) : Imagen {
    val TAGF = "[${object {}.javaClass.enclosingMethod?.name ?: nfnd}]"

    // establecer opciones de decodificación

    var opciones = BitmapFactory.Options()
    opciones.inScaled = false

    // abrir un InputStream para leer del archivo

    var assets: AssetManager = OpenGLES30Activity.instancia?.applicationContext?.assets
        ?: throw Error("$TAGF no puedo recuperar el 'Assets manager'")

    var istream: InputStream

    try {
        istream = assets.open(nombre_archivo)
    } catch (e: java.io.FileNotFoundException) {
        throw Error("$TAGF No encuentro el archivo '${nombre_archivo}' en la carpeta de assets.")
    }

    // leer el bitmap y cerrar el istream

    var bitmap: Bitmap?

    try
    {
        bitmap = BitmapFactory.decodeStream( istream, null, opciones )
                     ?: throw Error("$TAGF Error leyendo o decodificando el archivo '${nombre_archivo}' en la carpeta de assets")
    }
    catch ( e: IOException )
    {
        istream.close()
        throw Error("$TAGF Error leyendo o decodificando el archivo '${nombre_archivo}' en la carpeta de assets")
    }

    istream.close()

    // crear un byte array ('pixels') y liberar el bitmap

    val ancho      : Int = bitmap.getWidth()
    val alto       : Int = bitmap.getHeight()
    val tam_bytes  : Int = alto * ancho * 3

    val pixels = ByteArray(tam_bytes)

    for ( ix in 0..<ancho)
        for ( iy in 0..<alto) {
            val color : Int = bitmap.getPixel( ix, iy )
            val offset = (iy * ancho + ix) * 3

            pixels[offset + 0] = (color shr 16 and 0xff).toByte()
            pixels[offset + 1] = (color shr  8 and 0xff).toByte()
            pixels[offset + 2] = (color shr  0 and 0xff).toByte()
        }

    // ya está, crear objeto Imagen y devolverlo.

    Log.v(TAGF, "Leído bitmap en '${nombre_archivo}' (${ancho} x ${alto} pixels)")

    return Imagen( ByteBuffer.wrap(pixels), ancho, alto )
}
// -------------------------------------------------------------------------------------------------

/**
 * Convierte un array de Vec3 a un FloatArray con un float por entrada
 */

fun ConvFloatArrayV4( av4 : ArrayList<Vec4> ) : FloatArray
{
    val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd} -V3]"
    assert( av4.size > 0 ) {"$TAGF: el array de entrada está vacío"}

    val fa = FloatArray( av4.size*4 )

    for( i in 0..<av4.size )
    {
        fa[ i*4+0 ] = av4[i][0]
        fa[ i*4+1 ] = av4[i][1]
        fa[ i*4+2 ] = av4[i][2]
        fa[ i*4+3 ] = av4[i][2]
    }
    return fa
}
// -------------------------------------------------------------------------------------------------

/**
 * Convierte un array de Vec3 a un FloatArray con un float por entrada
 */

fun ConvFloatArrayV3( av3 : ArrayList<Vec3> ) : FloatArray
{
    val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd} -V3]"
    assert( av3.size > 0 ) {"$TAGF: el array de entrada está vacío"}

    val fa = FloatArray( av3.size*3 )

    for( i in 0..<av3.size )
    {
        fa[ i*3+0 ] = av3[i][0]
        fa[ i*3+1 ] = av3[i][1]
        fa[ i*3+2 ] = av3[i][2]
    }
    return fa
}
// -------------------------------------------------------------------------------------------------

/**
 * Convierte un array dinámico de Vec2 a un FloatArray con un float por entrada
 */
fun ConvFloatArrayV2( av2 : ArrayList<Vec2> ) : FloatArray
{
    val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd} - V2]"
    assert( av2.size > 0 ) {"$TAGF: el array de entrada está vacío"}

    val fa = FloatArray( av2.size*2 )

    for( i in 0..<av2.size )
    {
        fa[ i*2+0 ] = av2[i][0]
        fa[ i*2+1 ] = av2[i][1]
    }
    return fa
}
// -------------------------------------------------------------------------------------------------

/**
 * Convierte un array dinámico de UVec3 a un IntArray con un float por entrada
 */
fun ConvIntArray( auv3 : ArrayList<UVec3> ) : IntArray
{
    val TAGF = "[${object {}.javaClass.enclosingMethod?.name?:nfnd}]"
    assert( auv3.size > 0 ) {"$TAGF: el array de entrada está vacío"}

    val ia = IntArray( auv3.size*3 )

    for( i in 0..<auv3.size )
    {
        ia[ i*3+0 ] = (auv3[i][0]).toInt()
        ia[ i*3+1 ] = (auv3[i][1]).toInt()
        ia[ i*3+2 ] = (auv3[i][2]).toInt()
    }
    return ia
}

// -------------------------------------------------------------------------------------------------

class ObjetoEjes : ObjetoVisualizable()
{
    // crear objeto 'TablasAtributos' dando las posiciones

    var dvao : DescrVAO ? = null

    override fun visualizar() {
        if (dvao == null) {

            val lrp: Float = 1.5f  // long rama positiva
            val lrn: Float = 0.3f  // long rama negativa

            var tablas = TablasAtributos( floatArrayOf
            (
                -lrn, 0.0f, 0.0f,   lrp, 0.0f, 0.0f,
                0.0f, -lrn, 0.0f,   0.0f, lrp, 0.0f,
                0.0f, 0.0f, -lrn,   0.0f, 0.0f, lrp
            ))

            // añadir los colores
            tablas.colores = floatArrayOf(
                1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f
            )

            dvao = DescrVAO(tablas)
        }

        val dvao_nn = dvao ?: throw Error("no hay VAO, creando ejes")
        dvao_nn.draw(GLES30.GL_LINES)
    }
}