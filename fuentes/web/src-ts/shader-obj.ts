import { Assert, ComprErrorGL, Log, ContextoWebGL, LeerArchivoGLSL } from "./utilidades.js"


/**
 * Clase que encapsula un "shader-object"
 */
export class ShaderObject 
{
    private tipo_shader         : GLenum        // tipo de shader (gl.VERTEX_SHADER o gl.FRAGMENT_SHADER)
    private url_fuente          : string        // url del archivo con el fuente del shader
    private texto_fuente        : string        // texto fuente del shader
    private gl                  : ContextoWebGL // contexto usado para crear este shader
    private descripcion_fuente  : string        // descripción de la fuente (URL del archivo o "cadena de texto")

    private shader_object       : WebGLShader | null = null // objeto shader encapsulado

    // --------------------------------------------------------------------------------------------

    /**
     * Devuelve el objeto shader encapsulado (WebGLShader), si ya se ha creado, en otro caso lanza una
     * excepción de error. 
     * @returns objeto shader encapsulado
     */
    public get shader_wgl() : WebGLShader 
    {
        let nombref = "ShaderObject.shader:"
        if ( this.shader_object == null )
            throw new Error(`${nombref} no se puede leer el shader object, es nulo (no se ha construido todavía)`)
        return this.shader_object 
    }
    // --------------------------------------------------------------------------------------------
    /**
     * Devuelve el tipo de shader (gl.VERTEX_SHADER o gl.FRAGMENT_SHADER)
     * @returns tipo de shader
     */
    public get tipo() : GLenum 
    { 
        return this.tipo_shader 
    }
    // --------------------------------------------------------------------------------------------

    /**
     * Compilar un shader y, si va bien, adjuntarlo al objeto programa. Si hay errores se lanza 
     * una excepción cuyo texto tiene el log de errores.
     * 
     * @param gl   objeto de tipo WebGLRenderingContext o WebGL2RenderingContext)
     * @param tipo_shader  uno de: gl.FRAGMENT_SHADER, gl.VERTEX_SHADER, 
     * @param url_fuente (string) nombre del archivo que contenía el texto fuente
     * @param texto_fuente texto fuente del shader.
     */

    constructor( gl : ContextoWebGL, tipo_shader : GLenum, url_fuente : string | null, texto_fuente : string | null )
    {
        const nombref : string = "ShaderObject.constructor:"

        // comprobar precondiciones
        Assert( tipo_shader == gl.VERTEX_SHADER || tipo_shader == gl.FRAGMENT_SHADER, 
                 `${nombref} valor de 'tipo_shader' es incorrecto` ) 

        // guardar variables de instancia 
        this.url_fuente   = url_fuente   != null ? url_fuente : ""
        this.texto_fuente = texto_fuente != null ? texto_fuente : ""
        this.gl           = gl 
        this.tipo_shader  = tipo_shader 

        if ( this.url_fuente != ""  && this.texto_fuente != "" )
            throw Error( `${nombref} 'url_fuente' y 'texto_fuente' no pueden ser ambos no vacíos`)
        if ( this.url_fuente == ""  && this.texto_fuente == "" )
            throw Error( `${nombref} 'url_fuente' y 'texto_fuente' no pueden ser ambos vacíos`)

        // guardar descripción de la fuente
        this.descripcion_fuente = this.url_fuente != "" ? `'${this.url_fuente}'` : "cadena de texto"
    }
    // --------------------------------------------------------------------------------------------

    /**
     * Crea un shader a partir de un nombre d eun archivo GLSL en el sevidor
     * @param gl  contexto WebGL
     * @param tipo_shader  gl.VERTEX_SHADER o gl.FRAGMENT_SHADER
     * @param nombre string con el nombre del archivo que contiene el fuente del shader (en la sub-carpeta 'glsl', hermana de 'index.html')
     * @returns 
     */
    public static async crearDesdeArchivoGLSL( gl : ContextoWebGL, tipo_shader : GLenum, nombre : string ) : Promise<ShaderObject>
    {
        const nombref : string = "ShaderObject.crearDesdeURL:"

        let shader = new ShaderObject( gl, tipo_shader, nombre, null )
        await shader.leerFuenteDesdeArchivoGLSL()
        return shader
    }
    // --------------------------------------------------------------------------------------------

    /**
     * 
     * Crea un shader a partir de una cadena de texto con el fuente
     * @param gl            contexto WebGL
     * @param tipo_shader   gl.VERTEX_SHADER o gl.FRAGMENT_SHADER
     * @param texto_fuente  cadena de texto con el fuente del shader
     * @returns 
     */
    public static crearDesdeTexto( gl : ContextoWebGL, tipo_shader : GLenum, texto_fuente : string ) : ShaderObject
    {
        const nombref : string = "ShaderObject.crearDesdeTexto:"

        let shader = new ShaderObject( gl, tipo_shader, null, texto_fuente )
        return shader
    }

    // --------------------------------------------------------------------------------------------

    /**
     * Lee el texto fuente de un shader a partir de un nombre un archivo GLSL (en la carpeta 'glsl', 
     * hermana de 'index.html') y lo guarda en la variable de instancia
     */
    private async leerFuenteDesdeArchivoGLSL() : Promise<void>
    {
        const nombref : string = "ShaderObject.leerFuenteDesdeURL:"

        Assert( this.url_fuente != "", `${nombref} 'url_fuente' no puede ser vacío`)
        Assert( this.texto_fuente == "", `${nombref} 'texto_fuente' debe ser vacío`)

        this.texto_fuente = await LeerArchivoGLSL( this.url_fuente )
    }

    // --------------------------------------------------------------------------------------------

    /**
     * Compilar un shader y, si va bien, adjuntarlo al objeto programa. Si hay errores se lanza 
     * una excepción cuyo texto tiene el log de errores.
     */
    public compilar(   ) 
    {
        const nombref : string = "ShaderObject.compilar:"
        Assert( this.texto_fuente != "", `${nombref} 'texto_fuente' no puede ser vacío`)
        
        // si ya estaba compilado, no hace nada.
        if ( this.shader_object != null )
        {
            Log(`${nombref} shader en ${this.descripcion_fuente} ya estaba compilado, no se hace nada.`)
            return 
        }

        Log(`${nombref} inicio: compilando shader en ${this.descripcion_fuente}.`)

        let gl = this.gl 

        // comprobar precondiciones
        ComprErrorGL( gl, `${nombref} error OpenGL al inicio`)

        // crear y compilar el shader
        this.shader_object = gl.createShader( this.tipo_shader )
        if ( this.shader_object == null )
            throw Error(`${nombref} no se ha podido crear el objeto shader`)

        gl.shaderSource( this.shader_object, this.texto_fuente )
        gl.compileShader( this.shader_object )

        // si ha habido error, lanzar error
        if ( ! gl.getShaderParameter( this.shader_object, gl.COMPILE_STATUS) ) 
        {
            const info = gl.getShaderInfoLog( this.shader_object )
            console.log(`${nombref} mensajes de error al compilar ${this.descripcion_fuente} : \n\n${info}`)
            throw new Error(`${nombref} se han producido errores al COMPILAR (ver arriba)`);
        }

        if ( ! gl.isShader( this.shader_object ) )
        {
            console.log(`${nombref} el objeto creado no es un shader`)
            throw new Error(`${nombref} el objeto creado no es un shader`);
        }

        ComprErrorGL( gl, `${nombref} error OpenGL al final`)
        Log(`${nombref} shader en ${this.descripcion_fuente} compilado ok. -- this.shader_object == ${this.shader_object}`) 
    }

}

