// *********************************************************************
// **
// ** Asignatura: PCG (Programación del Cauce Gráfico).
// **
// ** Clases relacionadas con mallas indexadas de triángulos
// ** Copyright (C) 2024 Carlos Ureña
// **
// ** Clases:
// **    + MallaInd (derivada de ObjetoVisualizable) y derivadas:
// **         + MallaIndHelloRectanlgeXY - malla de test, 2 triángulos.
// **         + MallaIndHelloRectangleXZ - malla de test, 2 triángulos.
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

package mds.pcg1.malla_ind


import android.util.Log
import android.opengl.GLES30

import mds.pcg1.aplicacion.AplicacionPCG
import mds.pcg1.objeto_visu.*
import mds.pcg1.texturas.Textura
import mds.pcg1.utilidades.*
import mds.pcg1.vaos_vbos.*
import mds.pcg1.vec_mat.*


// *************************************************************************************************

open class MallaInd : ObjetoVisualizable()
{
    // tablas de atributos
    protected var posiciones  : ArrayList<Vec3> = ArrayList()  // tabla de coordenadas de las posiciones de los vértices
    protected var colores     : ArrayList<Vec3> = ArrayList()  // tabla de colores de los vértices
    protected var normales    : ArrayList<Vec3> = ArrayList()  // tabla de normales de los vértices
    protected var coords_text : ArrayList<Vec2> = ArrayList()  // tabla de coordenadas de textura de los vértices

    // tabla de triángulos (tabla de índices)
    protected var triangulos : ArrayList<UVec3> = ArrayList()


    // descriptor del VAO con la malla
    protected var dvao : DescrVAO? = null

    // descriptor del VAO con las aristas (se crea al llamar a 'crearVAOAristas')
    protected var dvao_aristas : DescrVAO? = null

    // descriptor del VAO con los segmentos de normales (se crea al llamar a 'crearVAONormales')
    protected var dvao_normales : DescrVAO? = null

    // ---------------------------------------------------------------------------------------------

    /**
     * Visualiza la malla indexada
     */
    override fun visualizar()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        val cauce = AplicacionPCG.instancia.leer_cauce

        guardarCambiarEstado( cauce )

        if ( dvao == null )
            crearInicializarVAO()

        val dvao_nn = dvao ?: throw Error("$TAGF - el descriptor de VAO es nulo")
        dvao_nn.draw( GLES30.GL_TRIANGLES )

        restaurarEstado( cauce )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Visualiza las aristas del objeto. Este método puede ser redefinido en clases derivadas, si
     * no se hace, el método no hace nada (eso implica que ese objeto no tiene aristas que se pueden visualizar
     * o que no se ha implementado esto)
     */
    override fun visualizarAristas()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        //val cauce = AplicacionPCG.instancia.leer_cauce

        if ( dvao == null )
            crearInicializarVAO()

        val dvao_nn = dvao ?: throw Error("$TAGF - el descriptor de VAO es nulo")
        dvao_nn.draw( GLES30.GL_LINES )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Visualiza las normales del objeto. Este método puede ser redefinido en clases derivadas, si
     * no se hace, el método no hace nada (eso implica que ese objeto no tiene normales que se pueden visualizar
     * o que no se ha implementado esto)
     */
    override fun visualizarNormales()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        if ( normales.size == 0 )
            return

        if ( dvao_normales == null )
            crearVAONormales()

        val dvao_nn = dvao_normales ?: throw Error("$TAGF - el descriptor de VAO de normales es nulo aquí, esto es imposible")

        dvao_nn.draw( GLES30.GL_LINES )
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Comprueba que una malla indexada está correctamente inicializada
     * (se debe llamar al final del valructor de las clases derivadas)
     * se llama antes de visualizar la primera vez.
     */
    fun comprobar( TAGF : String )
    {
        assert( posiciones.size > 0 ) { "${TAGF} malla indexada con tabla de posiciones de vértices vacía (${nombre})" }
        assert( triangulos.size > 0 )  { "${TAGF} malla indexada con tabla de triángulos vacía (${nombre})" }

        if ( colores.size > 0 )
            assert( posiciones.size == colores.size ) { "${TAGF} tabla de colores no vacía pero con tamaño distinto a la de vértices (${nombre})" }
        if ( normales.size > 0 )
            assert( posiciones.size == normales.size ) { "${TAGF} tabla de normales no vacía pero con tamaño distinto a la de vértices (${this.nombre}) " }
        if ( coords_text.size > 0 )
            assert( posiciones.size == coords_text.size ) { "${TAGF} tabla de coordenadas de textura no vacía pero con tamaño distinto a la de vértices (${nombre}) " }

    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Crea e inicializa el VAO (crea [dvao)
     */
    fun crearInicializarVAO()
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        Log.v( TAGF, "${TAGF} inicio, creando VAO de ${nombre}" )

        assert( dvao == null ) { "${TAGF} el vao ya está creado."  }
        comprobar( TAGF )

        var tablas = TablasAtributos( ConvFloatArrayV3( posiciones ))

        tablas.indices = ConvIntArray( triangulos )

        if ( colores.size > 0 )
            tablas.colores = ConvFloatArrayV3( colores )
        if ( normales.size > 0 )
            tablas.normales = ConvFloatArrayV3( normales )
        if ( coords_text.size > 0 )
            tablas.coords_text = ConvFloatArrayV2( coords_text )

        dvao = DescrVAO( tablas )

        //Log(`${TAGF} fin "  }
    }
    // ---------------------------------------------------------------------------------------------
    /**
     * Crea el VAO de normales (this.dvao_normales)
     */
    fun crearVAONormales( )
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"
        Log.v(TAGF, "Creando tabla y VAO de segmentos para malla '$nombre': INICIO")

        assert( dvao_normales == null ) { "${TAGF} el VAO de normales ya está creado" }
        assert( normales.size == posiciones.size ) { "${TAGF} no hay normales, o no las mismas que vértices." }

        val segmentos = ArrayList<Vec3>( 2*this.normales.size )

        for( i in 0..< posiciones.size )
        {
            segmentos.add( posiciones[i] )
            segmentos.add( posiciones[i] + 0.35f*normales[i] )
        }

        this.dvao_normales = DescrVAO( TablasAtributos( ConvFloatArrayV3( segmentos ) ))

        Log.v(TAGF, "Creando tabla y VAO de segmentos para malla '$nombre': FIN")
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * Calcular la tabla de normales de triángulos y vértices.
     *
     */
    fun calcularNormales( ) 
    {
        val TAGF = "[${object {}.javaClass.enclosingMethod?.name?: nfnd}]"

        Log.v( TAGF, "${TAGF} inicio, creando VAO de ${nombre}" )
        val num_t   : Int = this.triangulos.size
        val num_v   : Int = this.posiciones.size

        assert( num_t > 0 )  { "${TAGF} no hay triángulos en la malla "  }
        assert( num_v > 0 )  { "${TAGF} no hay vértices en la malla "  }

        // calcular las normales de las caras 
        val v       = this.posiciones
        val ejeY    = Vec3( 0.0f, 1.0f, 0.0f )
        var num_td  = 0 // cuenta de triángulos degenerados (sin área, y por tanto sin normal)
        var num_vd  = 0 // cuenta de vértices degenerados (= sin normal)
        val vcero   = Vec3( 0.0f, 0.0f, 0.0f )


        var nt : ArrayList<Vec3> = ArrayList( num_t ) // normales de triángulos (tamaño 0, capacidad 'num_t')

        val eps : Float = 1.0E-6F  // longitud mínima de las normales para ser normalizadas.

        for( i in 0..< num_t )
        {
            val t     : UVec3  = triangulos[i]
            val edge1 : Vec3   = v[t[1].toInt()] - v[ t[0].toInt() ]
            val edge2 : Vec3   = v[t[2].toInt()] - v[ t[0].toInt() ]
            val nn    : Vec3   = edge1.cross( edge2 )  // normal, no normalizada 
            val l     : Float  = nn.longitud

            if  ( eps < l )
                nt.add( nn.div( l ) )
            else
            {   nt.add( vcero )
                num_td ++
            }
        }
        Log.v(TAGF, "${TAGF} calculadas normales de triángulos (${num_td}/${num_t} triángulos degenerados) "  )

        // calcular las normales de los vértices 
        var nv = ArrayList<Vec3>( num_v )   // tamaño 0, capacidad num_v

        for( it in 0..< num_v )
            nv.add( vcero )

        for( it in 0..< num_t )
        {
            val t : UVec3 = this.triangulos[it]
            for( ivt in 0..< 3  )
                nv[ t[ivt].toInt() ] = nv[ t[ivt].toInt() ] + nt[it]
        }
        for( iv in 0 ..< num_v )
        {
            val l : Float = nv[iv].longitud

            if  ( eps < l )
                nv[iv] = nv[iv].div( l )
            else
            {   nv[iv] = vcero
                num_vd ++
            }
        }
        Log.v( TAGF, "${TAGF} calculadas normales de vértices (${num_vd}/${num_v} vértices sin normal) ")

        normales = nv
    }

    /**
     * redefinir como true para objetos que tengan normales
     * (se usa iluminación solo si el objeto tiene normales)
     */
    override val tieneNormales : Boolean get()
    {
        if ( normales.size == posiciones.size )
            return true
        else if ( normales.size == 0 )
            return false
        else
            throw Error("estado inconsistente de una malla indexada")
    }

} // fin de MallaInd

// *************************************************************************************************

/**
 * Una clase de pruebas para un rectangulo en 3D
 */
class MallaIndHelloRectangleXY() : MallaInd()
{
    init {
        nombre = "MallaInd Hello Rectangle"

        posiciones = arrayListOf(
            Vec3( 0.0f, 0.0f, 0.0f  ),
            Vec3( 1.0f, 0.0f, 0.0f  ),
            Vec3( 1.0f, 1.0f, 0.0f  ),
            Vec3( 0.0f, 1.0f, 0.0f  )
        )
        coords_text = arrayListOf(
            Vec2( 0.0f, 0.0f ),
            Vec2( 1.0f, 0.0f ),
            Vec2( 1.0f, 1.0f ),
            Vec2( 0.0f, 1.0f )
        )
        triangulos = arrayListOf(
            UVec3( 0u, 1u, 2u ),
            UVec3( 0u, 2u, 3u )
        )

        textura = Textura("imgs/madera2.png")

    }
}

// *************************************************************************************************

/**
 * Una clase de pruebas para un rectangulo en 3D
 */
class MallaIndHelloRectangleXZ() : MallaInd()
{
    init {
        nombre = "MallaInd Hello Rectangle"

        posiciones = arrayListOf(
            Vec3( 0.0f, 0.0f, 0.0f  ),
            Vec3( 1.0f, 0.0f, 0.0f  ),
            Vec3( 1.0f, 0.0f, 1.0f  ),
            Vec3( 0.0f, 0.0f, 1.0f  )
        )

        colores = arrayListOf(
            Vec3( 1.0f, 0.0f, 0.0f  ),
            Vec3( 0.0f, 1.0f, 0.0f  ),
            Vec3( 0.0f, 0.0f, 1.0f  ),
            Vec3( 0.0f, 1.0f, 1.0f  )
        )

        triangulos = arrayListOf(
            UVec3( 0u, 1u, 2u ),
            UVec3( 0u, 2u, 3u )
        )

    }
}

// *************************************************************************************************



