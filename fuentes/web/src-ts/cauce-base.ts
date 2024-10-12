

import  { Assert, ComprErrorGL, LeerArchivoTexto, Log,
         CrearFloat32ArrayV2, CrearFloat32ArrayV3, CrearFloat32ArrayV4, 
         ContextoWebGL} 
from    "./utilidades.js"

import  { Vec2, Vec3, Vec4, Mat4, CMat4 } 
from    "./vec-mat.js"

import  { ShaderObject } 
from    "./shader-obj.js"

import  { ProgramObject } 
from    "./program-obj.js"

import { FramebufferObject }
from   "./framebuffer-obj.js"

// -------------------------------------------------------------------------


/**
 * Funcionalidad básica para cualquier cauce (clase base para los cauces)
 * Ofrece funcionalidad para:
 * - Fijar matriz de modelado, y hacer push/pop 
 * - Fijar matrices de vista y proyección
 * 
 * Incorpora como variables de instancia 
 *    - un objeto programa (debe ser creado en clases derivadas) 
 *    - un contexto WebGL (se debe especificar en el constructor)
 *    - un framebuffer de destino (si es nulo es el framebuffer por defecto, si no es nulo debe ser creado en clases derivadas)
 */
export class CauceBase
{
    // ---------------------------------------------------------------------------
    // Variables de instancia:

    // objeto programa 
    protected objeto_programa  : ProgramObject | null = null

    // contexto WebGL, dado en el constructor 
    protected gl : ContextoWebGL

    // framebuffer (no se usa todavía?)
    protected framebuffer_destino : FramebufferObject | null = null

     
    // variables de estado del cauce

    protected mat_modelado     : Mat4 = CMat4.ident() // matriz de modelado
    protected mat_modelado_nor : Mat4 = CMat4.ident() // matriz de modelado para normales
    protected mat_vista        : Mat4 = CMat4.ident() // matriz de vista
    protected mat_proyeccion   : Mat4 = CMat4.ident() // matriz de proyección

    
    // pilas de colores, matrices modelado, materiales y texturas
    
    protected pila_mat_modelado     : Array<Mat4> = new Array<Mat4>
    protected pila_mat_modelado_nor : Array<Mat4> = new Array<Mat4>

    // locations de los uniforms (cada una de ellas puede ser null)
    protected loc_mat_modelado       : WebGLUniformLocation | null = null
    protected loc_mat_modelado_nor   : WebGLUniformLocation | null = null
    protected loc_mat_vista          : WebGLUniformLocation | null = null
    protected loc_mat_proyeccion     : WebGLUniformLocation | null = null
    
    
    // ---------------------------------------------------------------------------
    // Métodos 
    // ---------------------------------------------------------------------------

    /**
     * Inicializa el objeto cauce, es decir:
     * compila los shaders y enlaza el objeto programa, inicializa uniforms.
     * @param gl contexto WebGL en el cual se usará el objeto programa 
     */
    constructor( gl : ContextoWebGL )
    {
        const nombref : string = 'CauceBase.constructor'
        this.gl = gl 
        
    }
    // ---------------------------------------------------------------------------
    /**
     * Devuelve el objeto programa 
     */
    protected get programa() : ProgramObject 
    {
        let nombref = "Cauce.programa:"
        if ( this.objeto_programa == null )
            throw new Error(`${nombref}: no se puede leer el programa, es nulo`)
        return this.objeto_programa
    }
    // ---------------------------------------------------------------------------

    /**
     *  Lee las 'locations' de todos los uniforms y les da un 
     *  valor inicial por defecto. También inicializa algunas variables de estado.
     */
    protected inicializarUniformsBase() : void
    {
        const nombref : string = 'Cauce.inicializarUniformsBase:'
        if ( this.gl == null ) throw Error(`${nombref} leerLocation - this.gl es nulo`)
        let gl = this.gl

        this.programa.usar()
        
        // obtener las 'locations' de los parámetros uniform

        this.loc_mat_modelado      = this.leerLocation( "u_mat_modelado" )
        this.loc_mat_modelado_nor  = this.leerLocation( "u_mat_modelado_nor" )
        this.loc_mat_vista         = this.leerLocation( "u_mat_vista" )
        this.loc_mat_proyeccion    = this.leerLocation( "u_mat_proyeccion" )
        
        // dar valores iniciales por defecto a los parámetros uniform
        gl.uniformMatrix4fv( this.loc_mat_modelado,     true, this.mat_modelado )
        gl.uniformMatrix4fv( this.loc_mat_modelado_nor, true, this.mat_modelado_nor )
        gl.uniformMatrix4fv( this.loc_mat_vista,        true, this.mat_vista )
        gl.uniformMatrix4fv( this.loc_mat_proyeccion,   true, this.mat_proyeccion) 

        // desactivar objeto programa
        gl.useProgram( null ); 
    }
    // ---------------------------------------------------------------------------

    protected leerLocation( nombre : string ) : WebGLUniformLocation | null  
    {
        const nombref : string = 'CauceBase.leerLocation:'
        if ( this.gl == null ) 
            throw Error(`${nombref} leerLocation - this.gl es nulo`)
        
        const loc = this.programa.leerLocation( nombre )
        if ( loc == null )
            Log(`${nombref} Advertencia: el uniform '${nombre}' no aparece en los shaders o no se usa en la salida`)
        
        return loc 
    }
    // ---------------------------------------------------------------------------

    protected imprimeInfoUniformsBase() : void 
    {
        
    }
    // ---------------------------------------------------------------------------

    /**
     * Activa el objeto programa (hace 'useProgram' )
     */
    public activar() : void
    {
        const nombref : string = "Cauce.activar:"
        this.programa.usar()
    }
    
    // ---------------------------------------------------------------------------
    
    
    // ---------------------------------------------------------------------------
    
    /**
     * Cambia la matriz de vista en el objeto programa
     * @param nueva_mat_vista (Mat4) nueva matriz de vista 
     */
    public fijarMatrizVista( nueva_mat_vista : Mat4 ) : void 
    {
        this.mat_vista = nueva_mat_vista
        this.gl.uniformMatrix4fv( this.loc_mat_vista, true, this.mat_vista )  // SE TRASPONE!!
    }
    // ---------------------------------------------------------------------------

    /**
     * Cambia la matriz de proyeccion en el objeto programa
     * @param nueva_mat_proyeccion (Mat4) nueva matriz de proyección
     */
    public fijarMatrizProyeccion( nueva_mat_proyeccion : Mat4 ) : void 
    {
        this.mat_proyeccion = nueva_mat_proyeccion
        this.gl.uniformMatrix4fv( this.loc_mat_proyeccion, true, this.mat_proyeccion ) // SE TRASPONE !!
    }
    // ---------------------------------------------------------------------------

    /**
     * Cambia la matriz de modelado en el objeto programa
     * @param nueva_mat_modelado (Mat4) nueva matriz de modelado
     */
    public fijarMatrizModelado( nueva_mat_modelado : Mat4 ) : void 
    {
        this.mat_modelado     = nueva_mat_modelado
        this.mat_modelado_nor = nueva_mat_modelado.inversa3x3().traspuesta3x3()
        this.gl.uniformMatrix4fv( this.loc_mat_modelado, true, this.mat_modelado ) // SE TRASPONE !!
        this.gl.uniformMatrix4fv( this.loc_mat_modelado_nor, true, this.mat_modelado_nor ) 
    }
    // ---------------------------------------------------------------------------

    /**
     * Hace la matriz de modelado igual a la identidad
     */
    public resetMM(  ) : void 
    {
        this.fijarMatrizModelado( CMat4.ident() )
    }
    // ---------------------------------------------------------------------------

    /**
     * Compone una matriz 4x4 por la derecha de la matriz de modelado actual.
     * @param mat_modelado_adic 
     */
    public compMM( mat_modelado_adic : Mat4 ) : void 
    {
        const mat_modelado_nueva = this.mat_modelado.componer( mat_modelado_adic )
        this.fijarMatrizModelado( mat_modelado_nueva )
    }
    // ---------------------------------------------------------------------------

    /**
     * Guarda una copia de la matriz de modelado actual en la pila, no la cambia.
     */
    public pushMM() : void 
    {
        this.pila_mat_modelado.push( this.mat_modelado )
    }

    // ---------------------------------------------------------------------------

    /**
     * Restaura la última matriz de modelado que hubiese en la pila.
     * produce un error si la pila está vacía
     */
    public popMM() : void 
    {
        const n = this.pila_mat_modelado.length 
        Assert( n > 0 , `No se puede hacer 'popMM' en una pila vacía` )
        const prev_mat = this.pila_mat_modelado[ n-1 ]
        this.pila_mat_modelado.pop()
        this.fijarMatrizModelado( prev_mat )
    }

} // fin clase 'CauceBase'