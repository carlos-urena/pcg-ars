import { Assert, glsl }
from "./utilidades.js"

import { CauceBase }
from "./cauce-base.js"

import { ProgramObject } 
from "./program-obj.js"

import { FramebufferObject } 
from "./framebuffer-obj.js"

import { ShaderObject } 
from "./shader-obj.js"

import { ComprErrorGL, Log, ContextoWebGL }
from   "./utilidades.js"
import { ObjetoVisualizable } from "./objeto-visu.js"
import { CMat4, Mat4, Vec3 } from "./vec-mat.js"

// -------------------------------------------------------------------------

const src_vertex :string = 
glsl`#version 300 es
    precision highp float;
    precision highp int;

    uniform mat4 u_mat_modelado;
    uniform mat4 u_mat_modelado_nor;
    uniform mat4 u_mat_vista;
    uniform mat4 u_mat_proyeccion;

    layout( location = 0 ) in vec3 pos_occ;
    layout( location = 2 ) in vec3 nor_occ;

    out vec3 pos_wcc;
    out vec3 nor_wcc;
    out vec4 pos_ndc;

    void main()
    {
        pos_wcc = (u_mat_modelado * vec4( pos_occ, 1.0 )).xyz;
        nor_wcc = (u_mat_modelado_nor * vec4( nor_occ, 0.0 )).xyz;

        pos_ndc = u_mat_proyeccion * u_mat_vista *vec4( pos_wcc, 1.0 );
        gl_Position = pos_ndc;
    }
`
// -------------------------------------------------------------------------

const src_fragment : string = 
glsl`#version 300 es
    precision highp float;
    precision highp int;

    uniform mat4 u_mat_modelado;
    uniform mat4 u_mat_modelado_nor;
    uniform mat4 u_mat_vista;
    uniform mat4 u_mat_proyeccion;

    in vec3 pos_wcc;
    in vec3 nor_wcc;
    in vec4 pos_ndc;

    out vec4 color;
    //out float color ;  // NO FUNCIONA

    // NO uso esto (NO FUNCIONA)
    // https://stackoverflow.com/questions/32558579/single-component-texture-with-float

    // Uso mi propia versión:
    // (aquí abajo)

    // Codifica un float 'v' en un vec3 (suponiendo que 'v' 
    // está entre -1 y +1)

    vec3 codificarEnRGB( float v )
    {
        float r   = 0.5*(1.0+v);   // r en [0..1]  (codifica el byte más significativo)
        float re  = r*256.0;
        float fre = floor(re);    
        float r8  = fre/256.0;

        float g   = re-fre;  // g en [0..1]  (byte intermedio)
        float ge  = g*256.0;
        float fge = floor(ge);
        float g8  = fge/256.0;

        float b   = ge-fge;  // b en [0..1]  (byte menos significativo)
        float b8  = floor(b*256.0)/256.0;

        return vec3( r8, g8, b8 ); 
    }

    // función inversa a la anterior (no hace falta aquí pero la escribo para testear el error
    // y para tenerla de referencia)
    // Devuelve un número entre -1 y +1

    float decodificarDeRGB( vec3 rgb )
    {
        float x = rgb.r + rgb.g/256.0 + rgb.b/(256.0*256.0) ;
        return 2.0*x-1.0;
    }
    
    void main()
    {
        
        // FUNCIONA: codifica en RGB
        vec3  c = codificarEnRGB( pos_ndc.z );
        color = vec4( c, 1.0 );
        
        // NO FUNCIONA 
        //float v = 0.5*(1.0+pos_ndc.z) ;
        //color = v ; // vec4( v, v, v, 1.0);  // hay que pasar de [-1,1] a [0,1], igual que al inicio de 'codificarEnRGB'
        
        // des-comentar para comprobar el error (deben verse verdes todos los pixels)
        //float d = decodificarDeRGB( c ) - pos_ndc.z;
        //if ( abs(d) > 1.0/16777216.0 )          // abs(d) > 1/256^3
        //     color = vec4( 1.0, 0.0, 0.0, 1.0 ); 
        // else 
        //     color = vec4( 0.0, 1.0, 0.0, 1.0 );
    }
`

// -------------------------------------------------------------------------

export class CauceSombras extends CauceBase
{
    private fbo_opc : FramebufferObject | null = null
    private dir_vista : Vec3 = new Vec3([0,0,1]) // dirección de vista

    public get fbo() : FramebufferObject
    {
        if ( this.fbo_opc == null )
            throw new Error("CauceSombras.fbo: todavía no se ha creado el objeto FBO")
        return this.fbo_opc
    }

    private mat_vp_act : Mat4 | null  = null 
    
    public get mat_vista_proy() : Mat4
    {
        if ( this.mat_vp_act == null )
            throw new Error("CauceSombras.mat_vp: todavía no se ha fijado la dirección de vista (no hay matriz de vista-proyección)")
        return this.mat_vp_act
    }
    
    // ------------------------------------------------------------------------- 

    constructor( gl : ContextoWebGL, sizex : number, sizey : number )
    {
        super( gl )
        const fname : string = "CauceSombras.constructor:"
    
        ComprErrorGL( gl, `${fname} al inicio`)

        this.objeto_programa = new ProgramObject( gl )

        this.objeto_programa.agregar( ShaderObject.crearDesdeTexto( gl, gl.VERTEX_SHADER, src_vertex ) )
        this.objeto_programa.agregar( ShaderObject.crearDesdeTexto( gl, gl.FRAGMENT_SHADER, src_fragment ))
        
        this.objeto_programa.compilarEnlazar()
        this.objeto_programa.usar()
        this.inicializarUniformsBase()

        this.fbo_opc = new FramebufferObject( this.gl, sizex, sizey )
        ComprErrorGL( gl, `${fname} al final`)
    }
    // --------------------------------------------------------------------------

    public fijarDimensionesFBO( nuevo_tamX : number, nuevo_tamY : number ) : void
    {
        const fname : string = "CauceSombras.fijarDimensionesFBO:"
        Assert( nuevo_tamX > 0 && nuevo_tamY > 0, `${fname} tamaño del framebuffer inválido (${nuevo_tamX} x ${nuevo_tamY}`)
        //Log(`${fname} nuevo tamaño del FBO: ${nuevo_tamX} x ${nuevo_tamY} --- anteriores: ${this.fbo.tamX} x ${this.fbo.tamY}`)
        if ( nuevo_tamX != this.fbo.tamX || nuevo_tamY != this.fbo.tamY )
            this.fbo_opc = new FramebufferObject( this.gl, nuevo_tamX, nuevo_tamY )
        //else 
        //    Log(`${fname} no se cambia el tamaño del FBO`)
    }
    // --------------------------------------------------------------------------
    /**
     * fija la nueva dirección de vista (apunta hacia la fuente de luz)
     * @param nueva_dir Vec3
     */
    public fijarDireccionVista( nueva_dir : Vec3 ) : void
    {
        const fname : string = "CauceSombras.fijarDireccionVista:"
        const ld : number = nueva_dir.length

        Assert( Math.abs(ld) > 1e-4, `${fname} dirección de vista inválida` )
        this.dir_vista = nueva_dir

        // actualizar matrices de vista y proyección:
        // el view-frustum es un cubo de lado 2*w en XY y 3*w en Z
        // (centro en el origen, eje Z hacia la fuente de luz, eje X horizontal).)
        const w = 1.4

        const ejey_wcc = new Vec3([ 0.0, 1.0, 0.0 ])
        const ejez_ecc = nueva_dir.normalizado
        const ejex_ecc = ejey_wcc.cross( ejez_ecc ).normalizado 
        const ejey_ecc = ejez_ecc.cross( ejex_ecc ).normalizado

        // Log(`${fname} ejex_ecc = ${ejex_ecc}`)
        // Log(`${fname} ejey_ecc = ${ejey_ecc}`)
        // Log(`${fname} ejez_ecc = ${ejez_ecc}`)

        this.mat_vista = CMat4.filas( ejex_ecc, ejey_ecc, ejez_ecc )
        this.mat_proyeccion = CMat4.ortho( -w, +w, -w, +w, +w, -w )
        this.mat_vp_act = this.mat_proyeccion.componer( this.mat_vista )

        // Nota: aquí arriba se elije el signo de 'near' y 'far' en la llamada a 'ortho' 
        // de forma que el eje Z del marco de referencia de coordenadas de la fuente apunte 
        // en la dirección contraria de la fuente de luz, de forma que en ese marco de referencia 
        // la coordenada Z crece respecto a la distancia a la fuente de luz (codifica esa distancia 
        // y no su inversa)
    }
    
    // --------------------------------------------------------------------------
    /**
     * Visualizar la geometría de un objeto en este cauce, usando la 
     * matriz de vista actual
     * @param obj objeto a visualizar
     */

    visualizarGeometriaObjeto( obj : ObjetoVisualizable )
    {
        const fname = "CauceSombras.visualizarObjeto:"
        let gl = this.gl

        ComprErrorGL( gl, `${fname} inicio`)
       
        this.fbo.activar()
        this.programa.usar()

        gl.viewport( 0, 0, this.fbo.tamX, this.fbo.tamY )
        gl.clearColor( 0.99, 0.99, 0.99, 1.0 )   // 0.99 es la distancia máxima en Z
        gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT )
        gl.enable( gl.DEPTH_TEST )
        gl.disable( gl.CULL_FACE )
        
        this.resetMM()
        this.fijarMatrizVista( this.mat_vista )
        this.fijarMatrizProyeccion( this.mat_proyeccion )
        
        obj.visualizarGeometria( this )

        //gl.flush()
        //gl.finish()
        
        gl.useProgram( null )
        gl.bindFramebuffer( this.gl.FRAMEBUFFER, null )

        ComprErrorGL( this.gl, `${fname} fin`)
    }
    // --------------------------------------------------------------------------


}