

import  { Assert, ComprErrorGL, LeerArchivoTexto, Log,
         CrearFloat32ArrayV2, CrearFloat32ArrayV3, CrearFloat32ArrayV4, 
         ContextoWebGL} 
from    "./utilidades.js"

import  { Vec2, Vec3, Vec4, Mat4, CMat4 } 
from    "./vec-mat.js"

import  { Material } 
from    "./material.js"

import  { Textura } 
from    "./texturas.js"

import  { ShaderObject } 
from    "./shader-obj.js"

import  { ProgramObject } 
from    "./program-obj.js"

import  { CauceBase }   
from    "./cauce-base.js"

import { b2n }
from    "./utilidades.js"
import { FramebufferObject } from "./framebuffer-obj.js"

// -------------------------------------------------------------------------



// -------------------------------------------------------------------------

export async function CrearCauce( gl : ContextoWebGL ) : Promise<Cauce>
{
    let cauce : Cauce = new Cauce( gl )
    await cauce.inicializar()
    return cauce
} 
// -------------------------------------------------------------------------

export class Cauce extends CauceBase
{
    // ---------------------------------------------------------------------------
    // Propiedades de la clase ("estáticas"), no específicas de cada instancia
    // (de solo lectura, 'const')
    
    // número total de atributos de vértice que se gestionan en el objeto programa
    public static readonly numero_atributos : number = 4
 
    /** 
     ** nombres descriptivos para los índices de los atributos que gestiona el cauce.
     */
    public static readonly indice_atributo =  
    {   
        posicion    : 0, 
        color       : 1, 
        normal      : 2, 
        coords_text : 3
    }

    // número máximo de fuentes de luz que gestiona el cauce 
    // (debe coincidir con el valor de la constante declarada para esto 
    // en el fragment shader, con el mismo nombre)
    public static readonly max_num_luces = 8

    // ---------------------------------------------------------------------------
    // Variables de instancia:

    // variables de estado del cauce
    private color : Vec3 = new Vec3([0.0, 0.0, 0.0]) // color actual para visualización sin tabla de colores
   
    
    private eval_mil          : Boolean = false // true -> evaluar MIL, false -> usar color plano
    private usar_normales_tri : Boolean = false // true -> usar normal del triángulo, false -> usar interp. de normales de vértices
    private eval_text         : Boolean = false // true -> eval textura, false -> usar glColor o glColorPointer
    private eval_sombras      : Boolean = false // true -> eval sombras, false -> no evaluar sombras
    private mat_vp_sombras    : Mat4 = CMat4.ident() // matriz de vista-proyección para sombras

    private tipo_gct : number = 0 ;     // tipo de generación de coordenadas de textura
    private coefs_s  : Float32Array = new Float32Array([1.0,0.0,0.0,0.0])  // coeficientes para calcular coord. S con gen. aut. de coordenadas de textura
    private coefs_t  : Float32Array = new Float32Array([0.0,1.0,0.0,0.0])  // coeficientes para calcular coord. S con gen. aut. de coordenadas de textura
    private material : Material = new Material( 0.2, 0.8, 0.0, 10.0 ) // material actual 
    private textura  : Textura | null = null // textura en uso actualmente, (nulo si está desactivado)

    // pilas de colores, matrices modelado, materiales y texturas
    private pila_materiales  : Array<Material> = new Array<Material>
    private pila_texturas    : Array<Textura | null >  = new Array<Textura | null>
    protected pila_colores   : Array<Vec3> = new Array<Vec3>

    // locations de los uniforms (cada una de ellas puede ser null)
    private loc_eval_mil           : WebGLUniformLocation | null = null
    private loc_usar_normales_tri  : WebGLUniformLocation | null = null
    private loc_eval_text          : WebGLUniformLocation | null = null
    private loc_tipo_gct           : WebGLUniformLocation | null = null
    private loc_coefs_s            : WebGLUniformLocation | null = null
    private loc_coefs_t            : WebGLUniformLocation | null = null
    private loc_mil_ka             : WebGLUniformLocation | null = null
    private loc_mil_kd             : WebGLUniformLocation | null = null
    private loc_mil_ks             : WebGLUniformLocation | null = null
    private loc_mil_exp            : WebGLUniformLocation | null = null
    private loc_num_luces          : WebGLUniformLocation | null = null
    private loc_pos_dir_luz_ec     : WebGLUniformLocation | null = null
    private loc_color_luz          : WebGLUniformLocation | null = null
    private loc_param_s            : WebGLUniformLocation | null = null
    private loc_eval_sombras       : WebGLUniformLocation | null = null
    private loc_mat_vp_sombras     : WebGLUniformLocation | null = null

    
    // ---------------------------------------------------------------------------
    // Métodos 
    // ---------------------------------------------------------------------------

    /**
     * Constructor
     * @param gl contexto WebGL en el cual se usará el objeto programa 
     */
    constructor( gl : ContextoWebGL )
    {
        super( gl)
    }
    
    // ---------------------------------------------------------------------------

    async inicializar() : Promise<void>
    {
        const nombref : string = 'Cauce.inicializar:'

        Log(`${nombref} inicio.`)
        
        await this.crearObjetoPrograma()
        this.inicializarUniforms()
        this.imprimeInfoUniforms() 

        Log(`${nombref} shaders compilados y objeto 'Cauce' creado con éxito.`)
    }
    // ---------------------------------------------------------------------------

    /**
     *  Lee las 'locations' de todos los uniforms y les da un 
     *  valor inicial por defecto. También inicializa algunas variables de estado.
     */
    private inicializarUniforms() : void
    {
        const nombref : string = 'Cauce.inicializarUniforms:'
        if ( this.gl == null ) 
            throw Error(`${nombref} leerLocation - this.gl es nulo`)
        
        let gl = this.gl
        ComprErrorGL( gl, `${nombref} error al inicio`)

        this.inicializarUniformsBase()
        this.programa.usar()
        
        // obtener las 'locations' de los parámetros uniform

        this.loc_mat_modelado      = this.leerLocation( "u_mat_modelado" )
        this.loc_mat_modelado_nor  = this.leerLocation( "u_mat_modelado_nor" )
        this.loc_mat_vista         = this.leerLocation( "u_mat_vista" )
        this.loc_eval_mil          = this.leerLocation( "u_eval_mil" )
        this.loc_mat_proyeccion    = this.leerLocation( "u_mat_proyeccion" )
        this.loc_eval_mil          = this.leerLocation( "u_eval_mil" )
        this.loc_usar_normales_tri = this.leerLocation( "u_usar_normales_tri" )
        this.loc_eval_text         = this.leerLocation( "u_eval_text" )
        this.loc_tipo_gct          = this.leerLocation( "u_tipo_gct" )
        this.loc_coefs_s           = this.leerLocation( "u_coefs_s" )
        this.loc_coefs_t           = this.leerLocation( "u_coefs_t" )
        this.loc_mil_ka            = this.leerLocation( "u_mil_ka" )
        this.loc_mil_kd            = this.leerLocation( "u_mil_kd" )
        this.loc_mil_ks            = this.leerLocation( "u_mil_ks" )
        this.loc_mil_exp           = this.leerLocation( "u_mil_exp" )
        this.loc_num_luces         = this.leerLocation( "u_num_luces" )
        this.loc_pos_dir_luz_ec    = this.leerLocation( "u_pos_dir_luz_ec" )
        this.loc_color_luz         = this.leerLocation( "u_color_luz" )
        this.loc_param_s           = this.leerLocation( "u_param_s" )
        this.loc_eval_sombras      = this.leerLocation( "u_eval_sombras" )
        this.loc_mat_vp_sombras    = this.leerLocation( "u_mat_vp_sombras" )

        gl.uniform1i( this.loc_eval_mil,          b2n( this.eval_mil ) )
        gl.uniform1i( this.loc_usar_normales_tri, b2n( this.usar_normales_tri ) )
        gl.uniform1i( this.loc_eval_text,         b2n( this.eval_text ) )

        gl.uniform1i( this.loc_tipo_gct, this.tipo_gct )

        gl.uniform4fv( this.loc_coefs_s, this.coefs_s )
        gl.uniform4fv( this.loc_coefs_t, this.coefs_t )  
        
        gl.uniform1f( this.loc_mil_ka,  this.material.ka )
        gl.uniform1f( this.loc_mil_kd,  this.material.kd )
        gl.uniform1f( this.loc_mil_ks,  this.material.ks )
        gl.uniform1f( this.loc_mil_exp, this.material.exp )

        gl.uniform1i( this.loc_eval_sombras, b2n( this.eval_sombras ) )
        gl.uniformMatrix4fv( this.loc_mat_vp_sombras, true, this.mat_vp_sombras )

        gl.uniform1i( this.loc_num_luces, 0 ) // por defecto: 0 fuentes de luz activas
        
        // comprobar errores 
        ComprErrorGL( gl, `${nombref} error al final`)
        // desactivar objeto programa
        gl.useProgram( null ); 
    }
    // ---------------------------------------------------------------------------

    /**
     * Imprime en la consola información sobre los uniforms del objeto programa
     */
    private imprimeInfoUniforms() : void 
    {
        this.imprimeInfoUniformsBase()     
    }
    // ---------------------------------------------------------------------------
    /**
     *  Compila y enlaza el objeto programa 
     * (deja nombre en 'id_prog', debe ser 0 antes)
     */
    private async crearObjetoPrograma() : Promise<void>
    {
        const nombref : string = 'Cauce.crearObjetoPrograma:'
        //if ( this.gl == null ) throw Error(`${nombref} leerLocation - this.gl es nulo`)
        let gl = this.gl!

        Log(`${nombref} inicio.`)
        
        ComprErrorGL( gl, `${nombref} error OpenGL al inicio`)
        Assert( this.objeto_programa == null, `${nombref}  el objeto programa no es nulo` )

        // Leer los fuentes GLSL
        let fs : ShaderObject = await ShaderObject.crearDesdeURL( gl, gl.FRAGMENT_SHADER, "/glsl/cauce_3_00_fragment_shader.glsl" )
        let vs : ShaderObject   = await ShaderObject.crearDesdeURL( gl, gl.VERTEX_SHADER, "/glsl/cauce_3_00_vertex_shader.glsl" )

        Log(`${nombref} compilados.`)

        this.objeto_programa = new ProgramObject( gl )

        Log(`${nombref} po creado.`)
        this.objeto_programa.agregar( vs )
        this.objeto_programa.agregar( fs )
        this.objeto_programa.compilarEnlazar()

        ComprErrorGL( gl, `${nombref} error OpenGL al final`)
        Log(`${nombref} cauce creado ok.`)
    }
    // ---------------------------------------------------------------------------
    /**
     * Fija el color para las siguientes operaciones de visualización de VAOs sin colores
     * @param nuevo_color (Vec3) nuevo color actual 
     */
    public fijarColor( nuevo_color : Vec3 ) : void 
    {
        this.color = nuevo_color
        this.gl.vertexAttrib3fv( Cauce.indice_atributo.color, this.color )
    }
    // ---------------------------------------------------------------------------

    /**
     * Guarda una copia del color actual en la pila de colores
     */
    public pushColor() : void 
    {
        this.pila_colores.push( this.color )
    }
    // ---------------------------------------------------------------------------
    
    /**
     * Restaura último color guardado en la pila de colores
     */
    public popColor() : void 
    {
        const nombref : string = 'Cauce.popColor'
        if ( this.pila_colores.length == 0 )
            throw new Error(`${nombref} intento de hacer pop en la pila de colores vacía`)
        this.fijarColor( this.pila_colores[ this.pila_colores.length-1])
        this.pila_colores.pop()
    }

    /**
     * Activa o desactiva la evaluación del modelo de iluminación local
     * @param nue_eval_mil (boolean) true para activar iluminacion, false para desactivar 
     */
    public fijarEvalMIL( nue_eval_mil : boolean ) : void
    {
        this.eval_mil = nue_eval_mil  // registra valor en el objeto Cauce.
        this.gl.uniform1i( this.loc_eval_mil, b2n( this.eval_mil ) ) // cambia parámetro del shader
    }
    // ---------------------------------------------------------------------------
    
    /**
     * Fija el material actualmente activo en el cauce
     */
    fijarMaterial( nuevo_material : Material )
    {
        this.material = nuevo_material
        
        this.gl.uniform1f( this.loc_mil_ka,   this.material.ka );
        this.gl.uniform1f( this.loc_mil_kd,   this.material.kd );
        this.gl.uniform1f( this.loc_mil_ks,   this.material.ks );
        this.gl.uniform1f( this.loc_mil_exp,  this.material.exp );
    }
    // ---------------------------------------------------------------------------

    /**
     * guarda el material actual en la pila de materiales
     */
    pushMaterial( )
    {
        this.pila_materiales.push( this.material )
    }
    // ---------------------------------------------------------------------------

    /**
     * Activa el último material guardado en la pila de materiales.
     */
    popMaterial()
    {
        let  nombref = "Cauce.popMaterial"
        let n = this.pila_materiales.length
        Assert( n > 0, `${nombref} no se puede hacer 'pop' de la pila de materiales (está vacía)`)
        this.fijarMaterial( this.pila_materiales[n-1] )
        this.pila_materiales.pop()
    }
    // ---------------------------------------------------------------------------

    /**
     * Fija el valor de 'S'
     * @param nue_param_s (number) 
     */
    public fijarParamS( nue_param_s : number ) : void
    {
        
        this.gl.uniform1f( this.loc_param_s, nue_param_s ) // cambia parámetro del shader
    }
    // ---------------------------------------------------------------------------

    /**
     * Fijar los parámetros del modelo de iluminación local (MIL)
     * @param ka (number)
     * @param kd (number)
     * @param ks (number)
     * @param exp  (number)
     */
    public fijarParamsMIL( ka : number, kd : number, ks : number, exp : number )  
    {
        this.gl.uniform1f( this.loc_mil_ka,   ka );
        this.gl.uniform1f( this.loc_mil_kd,   kd );
        this.gl.uniform1f( this.loc_mil_ks,   ks );
        this.gl.uniform1f( this.loc_mil_exp,  exp );
    }
    // ---------------------------------------------------------------------------

    /**
     * Activa o desactiva el uso de una textura en los shaders (fija uniform)
     * Si se activa, hay que especificar el objeto de textura webgl a usar.
     * 
     * @param nuevo_eval_text 
     * @param texture 
     */
    public fijarEvalText( nuevo_eval_text : boolean, texture: WebGLTexture | null  ) : void 
    {
        const nombref : string = "Cauce.fijarEvalText:"
        let gl = this.gl 

        // registrar nuevo valor
        this.eval_text = nuevo_eval_text
        this.gl.uniform1i( this.loc_eval_text, b2n( this.eval_text ) )

        // si se está activando, asociar el sampler de textura en el shader con el objeto textura de la aplicación
        if ( nuevo_eval_text )
        {
            if ( texture == null )
                throw Error(`${nombref} si se habilita uso de texturas, se debe dar una textura no nula` )
            
            gl.activeTexture( gl.TEXTURE0 )  // ver nota aquí abajo.
            gl.bindTexture( gl.TEXTURE_2D, texture )
            
            // Nota: 'activeTexture' activa la unidad 0 de texturas, que está activada por defecto,  solo sería necesario si hubiese más de una textura en el shader (las demás irían en la unidad 1, la 2, etc...), no es el caso, pero lo pongo por si acaso, ver: https://webglfundamentals.org/webgl/lessons/webgl-2-textures.html (al final)
        }
    }
    // ---------------------------------------------------------------------------

    /**
     * Fija la textura actual en uso en el cauce
     * @param nueva_textura nueva textura, puede ser null (se desactivan texturas)
     */
    public fijarTextura( nueva_textura : Textura | null ) : void
    {
        this.textura = nueva_textura 

        if ( this.textura == null ) // si es nula, desactivar texturas
            this.fijarEvalText( false, null  )
        else 
            this.fijarEvalText( true, this.textura.texturaWebGL )
    }
    // ---------------------------------------------------------------------------
    
    /**
     * Guarda la textura actual en la pila de texturas
     */
    public pushTextura() : void 
    {
        this.pila_texturas.push( this.textura )
    }
    // ---------------------------------------------------------------------------
    /**
     * Restaura la textura actual de la pila de texturas.
     */
    public popTextura() : void 
    {
        let  nombref = "Cauce.popTextura"
        let n = this.pila_texturas.length
        Assert( n > 0, `${nombref} no se puede hacer 'pop' de la pila de texturas (está vacía)`)
        this.fijarTextura( this.pila_texturas[n-1] )
        this.pila_texturas.pop()
    }
    // ---------------------------------------------------------------------------
    
    /**
     * da valores a los uniforms relacionados con las fuentes de luz en el cauce 
     * 
     * @param color      vector de colores de las fuentes de luz
     * @param pos_dir_wc vector de posiciones o direcciones a la fuentes de luz (en coordenadas de mundo)
     */
    public fijarFuentesLuz( color : Vec3[], pos_dir_wc : Vec4[] ) : void
    {
        const nombref : string = `Cauce.fijarFuentesLuz:`
        const nl : number = color.length
       
        Assert( 0 < nl && nl < Cauce.max_num_luces, `${nombref} demasiadas fuentes de luz` )
        Assert( nl == pos_dir_wc.length, `${nombref} el vector de colores y el de posiciones/direcciones tienen distinto tamaño` );

        let gl = this.gl

        let pos_dir_ec : Vec4[] = [] 

        for( let i = 0 ; i < nl ; i++ )
            pos_dir_ec.push( this.mat_vista.aplica_v4( pos_dir_wc[i] ) )

        gl.uniform1i( this.loc_num_luces, nl )
        gl.uniform3fv( this.loc_color_luz, CrearFloat32ArrayV3( color ) )
        gl.uniform4fv( this.loc_pos_dir_luz_ec, CrearFloat32ArrayV4( pos_dir_ec ) )
    }
    //

    
    public fijarMatrizVPSombras( mat_vp_sombras : Mat4 ) : void
    {
        this.gl.uniformMatrix4fv( this.loc_mat_vp_sombras, true, mat_vp_sombras )
    }

    /**
     * Fija los parámetros de sombras: 
     * 
     * @param nuevo_eval_sombras - true para evaluar sombras, false para no evaluarlas
     * @param mat_vp_sombras - matriz de vista-proyección para sombras (alineada con fuente 0)
     */

    public fijarSombras( nuevo_eval_sombras : boolean, fbo_sombras : FramebufferObject | null, nuevo_mat_vp_sombras : Mat4 | null ) : void
    {
        const fname : string = "Cauce.fijarSombras:"
        let gl = this.gl 

        ComprErrorGL( gl, `${fname} error al inicio`)

        this.eval_sombras = nuevo_eval_sombras
        gl.uniform1i( this.loc_eval_sombras, b2n( this.eval_sombras ) )
        //Log(`## CAUCE ## ${fname} sombras activadas: ${this.eval_sombras}`)
        //Log(`## CAUCE ## ${fname} location eval_sombras: ${this.loc_eval_sombras}`)
        
        if ( this.eval_sombras ) 
        { 
            if( nuevo_mat_vp_sombras == null ) 
                throw new Error(`${fname}: matriz de vista-proyección para sombras es nula, pero se está activando las sombras`)
            if ( fbo_sombras == null )
                throw new Error(`${fname}: FBO de sombras es nulo, pero se está activando las sombras`)
            
            // construir la matriz vp de sombras añadiendole la translación y escalado por el tamaño del fbo 
            let sx = fbo_sombras.tamX
            let sy = fbo_sombras.tamY
            let mt = CMat4.traslacion( new Vec3([ 1.0, 1.0, 0.0 ]) ) // (1) dejar coords X e Y en [0..2] (estaban en -1..1)
            let ms = CMat4.escalado( new Vec3([sx/2.0,sy/2.0,1.0]) ) // (2) dejar coords X en (0..tamX), Y en (0..tamY)
            this.mat_vp_sombras = ms.componer( mt.componer( nuevo_mat_vp_sombras ) )
            
            gl.uniformMatrix4fv( this.loc_mat_vp_sombras, true, this.mat_vp_sombras )
            gl.activeTexture( gl.TEXTURE1 ) // la textura de sombras se asocia a la unidad 1
            gl.bindTexture( gl.TEXTURE_2D, fbo_sombras.cbuffer )
            gl.activeTexture( gl.TEXTURE0 ) // la textura de color se asocia a la unidad 0, lo dejo así por si acaso
        }
        ComprErrorGL( gl, `${fname} error al final`)
    }


} // fin clase 'Cauce'