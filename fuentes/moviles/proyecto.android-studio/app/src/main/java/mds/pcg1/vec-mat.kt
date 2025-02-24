// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases para vectores de 2, 3 y 4 floats, y matrices.
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases: Vec2, Vec3, Vec4, Mat4 (package vec_mat)
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

package mds.pcg1.vec_mat


import android.util.Log
import mds.pcg1.utilidades.*
import java.nio.FloatBuffer
import kotlin.math.*


// *************************************************************************************************
/**
 * Clase con tests para vectores y matrices
 */
class VecMatTest
{
    public fun run()
    {
        val v2a = Vec2(1.0f, 2.0f)
        val v2b = Vec2(3.0f, 4.0f)
        val v2c : Vec2 = 2.0f*v2a + v2b*7.0f

        val d : Float = v2a.dot( v2b )

        val v3a = Vec3(3.0f, 4.0f, 5.0f )
        val v3b = Vec3(3.0f, 4.0f, 6.0f )
        val v3c : Vec3 = 2.0f*v3a + v3b*7.0f

        val v4a = Vec4(3.0f, 4.0f, 5.0f, 6.0f )
        val v4b = Vec4(3.0f, 4.0f, 6.0f, 7.0f )
        val v4c : Vec4 = 2.0f*v4a - v4b*7.0f

        Log.v( TAG, "d   == ${d}")
        Log.v( TAG, "v2c == ${v2c}")
        Log.v( TAG, "v3c == ${v3c}")
        Log.v( TAG, "v4c == ${v4c}")

        val m1 : Mat4 = Mat4.escalado( Vec3( 2.0f, 1.0f/2.0f, 3.0f ))
        val m2 : Mat4 = Mat4.escalado( Vec3( 1.0f/2.0f, 2.0f, 1.0f/3.0f ))
        val mid = Mat4.ident()
        val mce = Mat4.ceros()
        val m3  = m1*m2

        Log.v( TAG, "cosas del test: $mid $mce $m3")

    }
}
// *************************************************************************************************

/**
 * Clase para vectores de 2 Float
 */
class Vec2 : VecGen<Longitud2>
{
    constructor( v0 : Float, v1 : Float ) : super( floatArrayOf( v0, v1 ))
    //constructor( v : VecGen<Longitud2> ) : super( v.array )

    // Añadir métodos de consulta de las componentes de los vectores

    val x get() = this[0]
    val y get() = this[1]

    val s get() = this[0]
    val t get() = this[1]

    operator fun plus ( v : Vec2 )  : Vec2 { return Vec2( this[0]+v[0], this[1]+v[1] )  }
    operator fun minus( v : Vec2 )  : Vec2 { return Vec2( this[0]-v[0], this[1]-v[1] )  }
    operator fun times( a : Float ) : Vec2 { return Vec2( this[0]*a, this[1]*a  ) }
    operator fun div  ( a : Float ) : Vec2 { return Vec2( this[0]*a, this[1]*a  ) }
    //fun dot( v : Vec2 ) : Vec2 { return this.op_dot( v ) }
}

operator fun Float.times( v : Vec2 ) : Vec2 { return v*this }

// *************************************************************************************************


/**
 * Clase para vectores de 3 Float
 */
class Vec3 : VecGen<Longitud3>
{
    constructor( v0 : Float, v1 : Float, v2 : Float )  : super( floatArrayOf( v0, v1, v2 ))
    //constructor( v : VecGen<Longitud3> ) : super( v.array )
    // -------------------------------------------------------------------------------------------------

    val x get() = this[0]
    val y get() = this[1]
    val z get() = this[2]

    val r get() = this[0]
    val g get() = this[1]
    val b get() = this[2]

    // -------------------------------------------------------------------------------------------------

    operator fun plus ( v : Vec3 )  : Vec3 { return Vec3( this[0]+v[0], this[1]+v[1], this[2]+v[2] )   }
    operator fun minus( v : Vec3 )  : Vec3 { return Vec3( this[0]-v[0], this[1]-v[1], this[2]-v[2] )  }
    operator fun times( a : Float ) : Vec3 { return Vec3( this[0]*a, this[1]*a, this[2]*a )  }
    operator fun div  ( a : Float ) : Vec3 { return Vec3( this[0]/a, this[1]/a, this[2]/a )  }

    // -------------------------------------------------------------------------------------------------

    /**
     * Calcula el producto vectorial (cross product) entre este vector y otro
     * @param that (Vec3) el otro vector
     * @returns (Vec3) producto vectorial
     */
    public fun cross( that : Vec3 ) : Vec3
    {
        return Vec3(
            this.y*that.z - this.z*that.y,  // x = y1*z2 - z1*y2
            this.z*that.x - this.x*that.z,  // y = z1*x2 - x1*z2
            this.x*that.y - this.y*that.x   // z = x1*y2 - x2*y1
        )
    }

}

operator fun Float.times( v : Vec3 ) : Vec3 { return v*this }

// *************************************************************************************************


/**
 * Clase para vectores de 4 Float
 */
class Vec4 : VecGen<Longitud4>
{
    constructor( v0 : Float, v1 : Float, v2 : Float, v3 : Float  ) : super( floatArrayOf( v0, v1, v2, v3 )) { }
    //constructor( v : VecGen<Longitud4> ) : super( v.array )

    val x get() = this[0]
    val y get() = this[1]
    val z get() = this[2]
    val w get() = this[3]

    val r get() = this[0]
    val g get() = this[1]
    val b get() = this[2]
    val a get() = this[3]

    // -------------------------------------------------------------------------------------------------

    operator fun plus ( v : Vec4 )  : Vec4 { return Vec4( this[0]+v[0], this[1]+v[1], this[2]+v[2], this[3]+v[3] )}
    operator fun minus( v : Vec4 )  : Vec4 { return Vec4( this[0]-v[0], this[1]-v[1], this[2]-v[2], this[3]-v[3] )}
    operator fun times( a : Float ) : Vec4 { return Vec4( this[0]*a, this[1]*a, this[2]*a, this[3]*a )}
    operator fun div  ( a : Float ) : Vec4 { return Vec4( this[0]/a, this[1]/a, this[2]/a, this[3]/a )}
}

operator fun Float.times( v : Vec4 ) : Vec4 { return v*this }

// *************************************************************************************************

/**
 * Vectores de 3 números enteros sin signo de 4 bytes cada uno (UInt)
 */
@OptIn(ExperimentalUnsignedTypes::class)
class UVec3( u0 : UInt, u1 : UInt, u2 : UInt )
{
    private var valores : UIntArray = uintArrayOf( u0, u1, u2 )

    // -------------------------------------------------------------------------------------------------

    operator fun get( i : Int ) : UInt
    {
        compruebaIndice( i )
        return valores[ i ]
    }
    operator fun set( i : Int, nuevo_valor : UInt )
    {
        compruebaIndice( i )
        valores[ i ] = nuevo_valor
    }
    // -------------------------------------------------------------------------------------------------

    private fun compruebaIndice( i : Int )
    {
        assert( i in 0..<3 ) { "Acceso fuera de rango a un UVec3 (i==$i, debe ser 0, 1 o 2)" }
    }
}

// *************************************************************************************************

/**
 * Clase base para clases que contienen un entero con una longitud
 */
open class Longitud( pn : Int )
{
    public val n = pn
}
// *************************************************************************************************

/** Clases concretas con longitudes concretas
 *
 */
class Longitud2() : Longitud( 2 ) {}
class Longitud3() : Longitud( 3 ) {}
class Longitud4() : Longitud( 4 ) {}

// *************************************************************************************************

/**
 * Clase base (plantilla genérica) para vectores de flotantes de longitud L, donde L es una clase
 * que implementa el interfaz Longitud.
 * Para valuir una instancia, usar: VectorFlotantes( Longitud2() ), por ejemplo
 * Se instancia usando una ongitud L concreta (una clase que implementa el interfaz 'Longitud')
 */
open class VecGen<L> where L : Longitud
{
    /**
     * Array de L.n flotantes
     */
    private var valores : FloatArray

    /**
     * Devuelve el array de valores del vector
     */
    public val array get() = valores

    // ---------------------------------------------------------------------------------------------

    /**
     * devuelve un 'FloatBuffer' que es un alias del array de valores
     */
    public val fb : FloatBuffer get() = FloatBuffer.wrap( valores )  // https://developer.android.com/reference/kotlin/java/nio/FloatBuffer#wrap(kotlin.FloatArray,%20kotlin.Int,%20kotlin.Int)

    // ---------------------------------------------------------------------------------------------

    /**
     * Devuelve la longitud de este vector (Int) (la longitud del array
     */
    public val long : Int get() =  valores.size

    // ---------------------------------------------------------------------------------------------
    // constructores: son 'internal' ya que permiten construir un VecGen<Longitud3>, p.ej., usando 2 floats.

    /**
     * construye un vector dando su longitud (los valores se ponen a 0)
     */
    internal constructor( n : Int )
    {
        valores = FloatArray( n, {0.0f} )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * construye un vector dando su longitud y un array de flotantes (que debe ser de dicha longitud)
     */
    internal constructor( arr : FloatArray )
    {
        valores = arr
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Comprueba si un índice está en el rango de índices del array
     * @param i - indice
     */
    private fun compruebaIndice( i : Int )
    {
        assert( 0 <= i && i < long )
        {   "Acceso de lectura fuera de rango a un 'ArrayFlotantes', el índice es $i pero el tamaño es $valores.size"
        }
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Comprueba si un índice (sin signo) está en el rango de índices del array
     * @param i - indice (sin signo)
     */
    private fun compruebaIndice( i : UInt )
    {
        assert( i.toInt() < long )
        {   "Acceso de lectura fuera de rango a un 'ArrayFlotantes', el índice es $i pero el tamaño es $valores.size"
        }
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Devuelve el valor de la entrada, si el índice está fuera de rango, produce un error
     * @param i - índice
     */
    operator fun get( i : Int ) : Float
    {
        compruebaIndice( i )
        return valores[ i ]
    }
    operator fun get( iu : UInt ) : Float
    {
        compruebaIndice( iu )
        return valores[ iu.toInt() ]
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Escribe en una entrda,  si el índice está fuera de rango, produce un error
     * @param i - índice
     */
    operator fun set( i : Int, a : Float )
    {
        compruebaIndice( i )
        valores[i] = a
    }
    // ---------------------------------------------------------------------------------------------

    operator fun set( iu : UInt, a : Float )
    {
        compruebaIndice( iu )
        valores[iu.toInt()] = a
    }
    // ---------------------------------------------------------------------------------------------

    override fun toString() : String {
        var s : String = "("
        for( i in 0..<long )
            s = s + valores[i] + if (i<long-1) "," else ""
        return s + ")"
    }
    // ---------------------------------------------------------------------------------------------

    fun dot( v : VecGen<L> ) : Float
    {
        var d = 0.0f
        for( i in 0..<long )
            d += this[i]*v[i]
        return d
    }
    // ---------------------------------------------------------------------------------------------

    val longitud : Float get()
    {
        var sqs = 0.0f
        for( i in 0..<valores.size )
            sqs += valores[i]*valores[i]
        return sqrt( sqs )
    }

}
// *************************************************************************************************

/**
 * Clase para matrices de 4x4 flotantes
 *
 */

class Mat4
{
    /**
     * Array de 16 flotantes, son los valores reales de la matriz
     * (dispuestos POR FILAS)
     */
    private var valores : FloatArray

    // ---------------------------------------------------------------------------------------------

    /**
     * devuelve un 'FloatBuffer' que es un alias del array de valores
     */
    public val fb : FloatBuffer get() = FloatBuffer.wrap( valores )  // https://developer.android.com/reference/kotlin/java/nio/FloatBuffer#wrap(kotlin.FloatArray,%20kotlin.Int,%20kotlin.Int)

    // ---------------------------------------------------------------------------------------------

    constructor( arr : FloatArray )
    {
        assert( arr.size == 16 ) { "Intento de valruir una matriz (Mat4) con un array de longitud distinta de 16" }
        valores = arr
    }
    // ---------------------------------------------------------------------------------------------
    /**
     * Lee un valor en una fila y una columna (ambas entre 0 y 3)
     * @param f índice de fila
     * @param c índice de columna
     * @returns  valor en la celda
     */
    operator fun get( f : Int, c : Int ) : Float
    {
        return valores[ ind(f,c) ]
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Cambia el valor en una fila y una columna (ambas entre 0 y 3)
     * @param f índice de fila
     * @param c índice de columna
     */
    operator fun set( f : Int, c : Int, a : Float )
    {
        valores[ ind(f,c) ] = a
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Operador de multiplicación (composición) de esta matriz y otra por la derecha
     *
     * @param   m (Mat4)
     * @returns (Mat4) matriz resultado
     */
    operator fun times( m : Mat4 ) : Mat4
    {
        var res = Mat4.ceros()

        for( fila in 0..3  )
            for( colu in 0..3 )
                for( i in 0..3 )
                    res[fila,colu] = res[fila,colu] + this[fila,i] * m[i,colu]

        return res
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Aplicar esta matriz a un vector de 4 entradas y devolver el vector 4 resultado
     *
     * @param   v (Vec4) vector original
     * @returns (Vec4) vector resultado
     */
    operator fun times( v : Vec4 ) : Vec4
    {
        var res = Vec4( 0.0f, 0.0f, 0.0f, 0.0f )

        for( fila in 0..3  )
            for( colu in 0..3 )
                res[fila] = res[fila] + this[fila,colu] * v[colu]

        return res
    }

    // -----------------------------------------------------------------------------
    // Métodos estáticos de la clase (miembros del objeto singleton 'companion')
    // Principalmente son constructores de matrices

    companion object
    {
        /**
         * Para una matriz 4x4 dispuesta POR FILAS, devuelve el índice del elemento en una
         * fila y columna determinadas.
         *
         * @param f índice de fila
         * @param c índice de columna
         * @returns  índice de la celda
         */
        fun ind( f : Int, c : Int ) : Int
        {
            assert( 0 <= f && f < 4 ) { "índice de fila (${f}) fuera de rango (accediendo a Mat4)" }
            assert( 0 <= c && c < 4 ) { "índice de columna (${c}) fuera de rango (accediento a Mat4)" }
            return 4*f + c
        }

        // ---------------------------------------------------------------------------------------------
        /**
         * Produce una matriz con todas las entradas puesta a cero
         * @return (Mat4) matriz con ceros
         */
        fun ceros() : Mat4
        {
            return Mat4( floatArrayOf(
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f
            ))
        }
        // ---------------------------------------------------------------------------------------------

        /**
         * Produce la matriz identidad
         * @return (Mat4) matriz identidad 4x4
         */
        fun ident() : Mat4
        {
            return Mat4( floatArrayOf(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            ))
        }
        // ---------------------------------------------------------------------------------------------

        /**
         * construye una matriz 4x4 de traslación a partir de un vector de traslación
         *
         * @param v (Vec3) vector de traslación
         * @return (Mat4) matriz de traslación
         */
        fun traslacion( v : Vec3 ) : Mat4
        {
            return Mat4( floatArrayOf(
                1.0f, 0.0f, 0.0f, v.x,
                0.0f, 1.0f, 0.0f, v.y,
                0.0f, 0.0f, 1.0f, v.z,
                0.0f, 0.0f, 0.0f, 1.0f
            ))
        }
        // ---------------------------------------------------------------------------------------------

        /**
         * construye una matriz 4x4 de escalado a partir de un vector con los factores de escala
         *
         * @param v (Vec3) vector con los factores de escala en X, Y y Z
         * @return (Mat4) matriz de escalado
         */
        fun escalado( v : Vec3 ) : Mat4
        {
            return Mat4( floatArrayOf(
                v.x,  0.0f,  0.0f,   0.0f,
                0.0f, v.y,   0.0f,   0.0f,
                0.0f, 0.0f,  v.z,    0.0f,
                0.0f, 0.0f,  0.0f,   1.0f
            ))
        }
        // ---------------------------------------------------------------------------------------------

        /**
         * construye una matriz de rotación entorno al eje X, dado un ángulo (en grados)
         * @param angulo_grad (Float) ángulo en grados
         * @return (Mat4) matriz de rotación entorno a X
         */
        fun rotacionXgrad( angulo_grad : Float ) : Mat4
        {
            val ar : Float = (angulo_grad*PI.toFloat())/180.0f
            val c  : Float = cos( ar )
            val s  : Float = sin( ar )

            return Mat4( floatArrayOf(
                1.0f,  0.0f,  0.0f,  0.0f ,
                0.0f,  c,     -s,    0.0f ,
                0.0f,  s,     c,     0.0f ,
                0.0f,  0.0f,  0.0f,  1.0f
            ))
        }
        // ---------------------------------------------------------------------------------------------

        /**
         * construye una matriz de rotación entorno al eje Y, dado un ángulo (en grados)
         * @param angulo_grad (Float) ángulo en grados
         * @return (Mat4) matriz de rotación entorno a Y
         */
        fun rotacionYgrad( angulo_grad : Float ) : Mat4
        {
            val ar : Float = (angulo_grad*PI.toFloat())/180.0f
            val c  : Float = cos( ar )
            val s  : Float = sin( ar )

            return Mat4( floatArrayOf(
                c,    0.0f, s,    0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -s,   0.0f, c,    0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            ))
        }
        // ---------------------------------------------------------------------------------------------

        /**
         * construye una matriz de rotación entorno al eje Z, dado un ángulo (en grados)
         * @param angulo_grad (Float) ángulo en grados
         * @return (Mat4) matriz de rotación entorno a Y
         */
        fun rotacionZgrad( angulo_grad : Float ) : Mat4
        {
            val ar : Float = (angulo_grad*PI.toFloat())/180.0f
            val c  : Float = cos( ar )
            val s  : Float = sin( ar )

            return Mat4( floatArrayOf(
                c,    -s,   0.0f, 0.0f ,
                s,    c,    0.0f, 0.0f ,
                0.0f, 0.0f, 1.0f, 0.0f ,
                0.0f, 0.0f, 0.0f, 1.0f
            ))
        }

        // ------------------------------------------------------------------------------------------------
        /**
         * construye una matriz de proyección perspectiva a partir de los límites del view-frustum
         * (según: http://docs.gl/gl2/glFrustum)
         *
         * @param l (Float) limite izquierdo del frustum en X (en el plano z=near)
         * @param r (Float) limite derecho del frustum en X (en el plano z=near)
         * @param b (Float) limite inferior del frustum en Y (en el plano z=near)
         * @param t (Float) limite superior del frustum en Y (en el plano z=near)
         * @param n (Float) distancia en Z entre observador y plano de recorte delantero (>0)
         * @param f (Float) distancia en Z entre observador y plano de recorte trasero (>n)
         * @returns {Mat4}   (Float) matriz de proyeccion perspectiva
         */
        fun frustum( l : Float, r : Float, b : Float, t : Float, n : Float, f : Float ) : Mat4
        {
            val eps = 1e-6
            assert( eps < n && eps < f )  { "'n' o 'f' son casi cero o negativos" }
            assert( n < f ) { "'n' no es menor que 'f'" }
            assert( Math.abs(r-l) > eps && Math.abs(t-b) > eps  && Math.abs(n-f) > eps ) { "parámetros incorrectos (el tamaño en X, Y o Z es casi cero)" }

            val a00 : Float = (2.0f*n)/(r-l) ; val a02 : Float = (r+l)/(r-l)
            val a11 : Float = (2.0f*n)/(t-b) ; val a12 : Float = (t+b)/(t-b)
            val a22 : Float = -(n+f)/(f-n)   ; val a23 : Float = -(2.0f*f*n)/(f-n)

            return Mat4( floatArrayOf(
                a00,   0.0f,   a02,   0.0f,
                0.0f,   a11,   a12,   0.0f,
                0.0f,   0.0f,  a22,   a23,
                0.0f,   0.0f,  -1.0f, 0.0f
            ))
        }

        // ------------------------------------------------------------------------------------------------
        /**
         * construye una matriz de proyección perspectiva a partir de un conjunto alternativo de parámetros
         * (según: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml)
         *
         * @param fovy_grad (Float) amplitud de campo en vertical (un ángulo en grados, entre 0 y 180.0f, sin incluir)
         * @param aspect_xy (Float) relación de aspecto (ancho del viewport dividido por el alto)
         * @param near      (Float) distancia en Z entre observador y plano de recorte delantero (>0)
         * @param far       (Float) distancia en Z entre observador y plano de recorte trasero (>n)
         * @returns
         */
        fun perspective( fovy_grad : Float, aspect_xy : Float, near : Float, far : Float ) : Mat4
        {
            val fovy_rad : Float = (fovy_grad*PI.toFloat())/180.0f
            val h        : Float = near * tan( fovy_rad/2.0f )
            val w        : Float = h*aspect_xy

            return frustum( -w, +w, -h, +h, near, far )
        }

    } // fin del 'companion object'

} // fin de la clase 'Mat4'

// *************************************************************************************************
