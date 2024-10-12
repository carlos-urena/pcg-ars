

import { Assert, ComprErrorGL, Log } from "./utilidades.js"
import { Vec2, Vec3, Vec4, Mat4, CMat4 } from "./vec-mat.js"
import { Cauce } from "./cauce.js"

// -------------------------------------------------------------------------------------------------
// 

/**
 * Dimensiones del viewport (no hace falta!)
 */
export class Viewport 
{
    public ancho : number = 0 
    public alto  : number = 0 
    //private ratio_yx : number = 0.0  

    /**
     * Construye un objeto 'viewport' dando sus dimensiones
     * 
     * @param ancho_inicial (number) número de columnas de pixel del viewport (>0)
     * @param alto_inicial  (number) número de filas de pixel del viewport (>0)
     */
    constructor( ancho_inicial : number, alto_inicial : number )
    {
        this.fijarDimensiones( ancho_inicial, alto_inicial )
    }
    // -----------------------------------------------------------------------------
    
    /**
     * Cambia las dimensiones del viewport
     * @param nuevo_ancho (number) nuevo número de columnas de pixel del viewport (>0)
     * @param nuevo_alto  (number) nuevo número de filas de pixel del viewport (>0)
     */
    fijarDimensiones( nuevo_ancho : number, nuevo_alto : number ) : void
    {
        const nombref : string = 'Viewport.setDimensions()'
        
        Assert( 0 < nuevo_ancho && 0 < nuevo_alto, `${nombref} el ancho o alto del viewport no pueden ser negativos (las dimensiones son ${nuevo_ancho} x ${nuevo_alto})`)

        this.ancho    = nuevo_ancho
        this.alto     = nuevo_alto
    }
    // -----------------------------------------------------------------------------

    /**
     * Devuelve el aspect ratio Y/X de este viewport
     * @returns alto dividido por el ancho 
     */
    leerRatioYX() : number 
    {
        return this.alto / this.ancho 
    }
}
// ***************************************************************************************

export abstract class CamaraInteractiva
{
    /**
     * Nombre de la cámara (para depurar)
     */
    protected nombre : String = "(no asignado)"

    /**
     * Viewport de esta cámara (se usa para saber sus proporciones) 
     */
    protected viewport : Viewport = new Viewport( 256, 256 )

    /**
     * Matriz de vista 
     */
    protected mat_vista : Mat4 = CMat4.ident()

    /**
     * Matriz de proyección 
     */
    protected mat_proyeccion : Mat4 = CMat4.ident()

    //
    /**
     * Construye una cámara nueva, por defecto es una cámara perspectiva
     * @param nombre 
     */
    constructor( nombre? : string )
    {
        if ( nombre != undefined )
            this.nombre = nombre
    }

    /**
     * Modifica los ángulos horizonal y vertical de la cámara orbital 
     * (debe ser redefinido en clases derivadas) 
     * @param delta_x (number) -- incremento en horizontal 
     * @param delta_y (number) -- incremento en vertical 
     */
    abstract mover( delta_x : number, delta_y : number ) : void ;

    /**
     * hacer 'zoom', usando un factor de escala relativo [factor_rel]
     * (debe ser redefinido en clases derivadas)
     */
    abstract zoom( factor_rel : number ) : void ;
    

    /**
     * Recalcular las matrices en función del estado actual
     * (debe ser redefinido en clases derivadas)
     */
    abstract recalcularMatrices( ) : void ;

    /**
     * fijar las dimensiones del viewport en el que se usará esta cámara
     * @param {Viewport} nuevo_viewport 
     */
    fijarViewport( nuevo_viewport : Viewport  ) : void
    {
        this.viewport = nuevo_viewport
    }

    /**
     * Activar esta cámara en un cauce.
     *   - Actualiza las matrices de vista y proyeccion, 
     *   - Fija los uniforms de matrices en el cauce.
     *   - Hace 'reset' (reinicializa) de la matriz de modelado en el cauce.
     * @param cauce (Cauce) cauce en el que se activa la matriz
     */
    activar( cauce : Cauce )
    {
        this.recalcularMatrices()
        cauce.fijarMatrizVista( this.mat_vista )
        cauce.fijarMatrizProyeccion( this.mat_proyeccion )
        cauce.resetMM()
    }

}
// ******************************************************************************

/**
 * Clase que encapsula una matriz de proyección y una matriz de vista,
 * junto con las operaciones para rotarla con dos grados de libertad)
 */
export class CamaraOrbital3D extends CamaraInteractiva
{
    /**
     * amplitud de campo vertical (en grados) para cámaras perspectivas
     */
    private fovy_grad : number = 60.0

    /**
     * distancia al plano de recorte delantero 
     */
    private near : number = 0.05 

    /**
     * Distancia al plano de recorte trasero 
     */
    private far : number = 30.05 

     /**
      *  true si la cámara es perspectiva, false si es paralela (por ahora solo puede ser true)
      */
    private es_perspectiva : boolean = true  

    /**
     * Punto de atención actual
     */
    private punto_atencion : Vec3 = new Vec3([ 0.0, 0.0, 0.0 ])

    /**
     * Ángulo vertical de la cámara en grados (rotación entorno a X)
     */
    private angulo_vert_grad : number = 45.0 

    /**
     * Ángulo horizontal de la cámara en grados (rotación entorno a Y)
     */
    private angulo_hor_grad : number = 45.0

    /**
     * Distancia desde el observador hasta el punto de atención
     */
    private distancia : number = 3.0
    

    /**
     * Construye una cámara nueva, por defecto es una cámara perspectiva
     * @param nombre 
     */
    
    constructor( nombre? : string )
    {
       super( nombre ) 
    }

    /**
     * constructor por defecto, pone el nombre como "no asignado"
     */
   
    
    
    /**
     * Actualiza la matriz de proyección a partir del aspect ratio del viewport,
     * de la amplitud de campo y de los valores 'near' y 'far' actuales.
     */
    private recalcularMatrizProyeccion() : void 
    {
        const nombref : string = "Camara.actualizarMatrizProyeccion"
        Assert( this.es_perspectiva, `${nombref} todavía no gestiono cámaras paralelas, sorry...` )

        this.mat_proyeccion = CMat4.perspective( this.fovy_grad, 1.0/this.viewport.leerRatioYX(),
                                                    this.near, this.far )
    }

    /**
     * Actualiza la matriz de vista, a partir del punto de atención, los ángulos 
     * vertical y horizontal, y la distancia observador-punto de atención
     */
    private recalcularMatrizVista() : void 
    {
        const nombref : string = "Camara.actualizarMatrizProyeccion"
        const 
            a      : Vec3 = this.punto_atencion,
            mtras1 : Mat4 = CMat4.traslacion( new Vec3([ -a.x, -a.y, -a.z ])),
            mrot2  : Mat4 = CMat4.rotacionXgrad( this.angulo_vert_grad ),
            mrot1  : Mat4 = CMat4.rotacionYgrad( -this.angulo_hor_grad ),
            mtras2 : Mat4 = CMat4.traslacion( new Vec3([ 0, 0, -this.distancia ]))

        this.mat_vista = mtras2.componer( mrot2.componer( mrot1.componer( mtras1 ) ) )
        //Log(`${nombref} matriz de vista  == ${this.matriz_vista}`)        
    }

    /**
     * Actualiza las matrices de vista y proyección
     * (implementa el método abstracto de la clase padre)
     */
    recalcularMatrices(): void 
    {
        this.recalcularMatrizVista() 
        this.recalcularMatrizProyeccion()
    }

    /**
     * Modifica los ángulos horizonal y vertical de la cámara orbital 
     * 
     * @param delta_x (number) -- incremento en horizontal 
     * @param delta_y (number) -- incremento en vertical 
     */
    mover( delta_x : number, delta_y : number ) : void 
    {
        const nombref : string = 'CamaraOrbital.moverHV'

        this.angulo_hor_grad  += delta_x
        this.angulo_vert_grad += delta_y
    }

    /**
     * Modifica la distancia entre el punto de atención y el punto del observador
     * @param factor_rel (number) +1 o -1 según la dirección de movimiento del ratón
     */
    zoom( factor_rel : number ) : void
    {
        const nombref = 'CamaraOrbital.zoom'
        //Log(`${nombref} factor_rel == ${factor_rel}, dist = ${this.distancia}`)
        const incr_en_porcentaje : number = 3.0*factor_rel
        this.distancia = this.distancia * (1.0 + incr_en_porcentaje/100.0)
    }


}  // fin class CamaraOrbital3D

// ****************************************************************************************************

/**
 * Cámaras para vistas 2D, el plano de visión es el plano XY
 * En el viewport se visualiza un cuadrado con centro en [cv_centro_wcc] y
 * lado [cv_lado_wcc]  (todos los valores en coordenadas de mundo)
 */
export class CamaraVista2D extends CamaraInteractiva
{
    private cv_centro_wcc : Vec3   = new Vec3( [0.0, 0.0, 0.0] )
    private cv_lado_wcc   : number = 2.0

    /**
     * Construye una cámara nueva, por defecto es una cámara perspectiva
     * @param nombre 
     */
    constructor( nombre : string )
    {
        super( nombre ) 
    }

    // ---------------------------------------------------------------------------------------------
    /**
     * Mueve la cámara tras un evento de movimiento con un desplazamiento
     * proporcional a [delta_x] y [delta_y]
     * Desplaza [cv_centro_wcc] usando [delta_x] y [delta_y]
     */
    mover( delta_x : number, delta_y : number ) : void
    {
        this.cv_centro_wcc 
            = this.cv_centro_wcc.menos( new Vec3([ delta_x, delta_y, 0.0 ]) )
    }
    // ---------------------------------------------------------------------------------------------
    /**
     * hacer 'zoom', usando un factor de escala relativo [factor_rel]
     * cambia el lado del cuadrado visible ([cv_lado_wcc]
     */
    zoom( factor_rel : number ) : void 
    {
        this.cv_lado_wcc *= 1.0/factor_rel
    }
    // ---------------------------------------------------------------------------------------------

    recalcularMatrices()
    {
        let tam_pixel_wcc = this.cv_lado_wcc/ Math.min( this.viewport.ancho, this.viewport.alto ) // lado de un pixel en WCC
        let fx            = (2.0/this.cv_lado_wcc)* Math.min( 1.0, this.viewport.alto/this.viewport.ancho )
        let fy            = (2.0/this.cv_lado_wcc)* Math.min( 1.0, this.viewport.ancho/this.viewport.alto )

        this.mat_vista     = CMat4.traslacion( this.cv_centro_wcc.mult( - tam_pixel_wcc) )
        this.mat_proyeccion = CMat4.escalado( new Vec3([ fx, fy, 1.0 ]))

    }
}  // fin de CamaraVista2D


