import { Assert, Log } from "./utilidades.js"
import { Vec3, Vec4 } from "./vec-mat.js"
import { AplicacionWeb } from "./aplicacion-web.js"
import { Cauce } from "./cauce.js"


/**
 * Clase para una fuente de luz, incluye su color y su posición/dirección
 * (por ahoras son únicamente fuentes de luz direccionales)
 * (las fuentes de luz no se activan de forma individual, sino como parte de una colección)
 */
export class FuenteLuz 
{
    private pos_dir_wc_act : Vec4   // posicion o dirección de la fuente en coordenadas de mundo
    private color_act      : Vec3   // color o intensidad (r,g,b >0 , pero no necesariamente acotado por arriba)
    private long_act : number = 45.0   // longitud actual en grados
    private lat_act  : number = 45.0   // latitud actual en grados



    public set color( nuevo_color : Vec3 )
    {
        this.color_act = nuevo_color
    }
    public get color() : Vec3
    {
        return this.color_act
    }

    public set long( nueva_longitud : number )
    {
        this.long_act = nueva_longitud
        this.pos_dir_wc_act = this.calcular_pos_dir_wc()
    }

    public get long() : number
    {
        return this.long_act
    }

    public set lat( nueva_latitud : number )
    {
        this.lat_act = nueva_latitud
        this.pos_dir_wc_act = this.calcular_pos_dir_wc()
    }

    public get lat() : number
    {
        return this.lat_act
    }

    private calcular_pos_dir_wc() : Vec4
    {
        let long_rad : number = this.long_act * Math.PI / 180.0
        let lat_rad  : number = this.lat_act * Math.PI / 180.0

        let x : number = Math.cos( lat_rad ) * Math.sin( long_rad )
        let y : number = Math.sin( lat_rad )
        let z : number = Math.cos( lat_rad ) * Math.cos( long_rad )

        return new Vec4( [x, y, z, 0.0] )
    }

    public get dir_wcc() : Vec3
    {
        Assert( this.pos_dir_wc_act.w == 0.0, 'FuenteLuz.dir_wcc: la fuente de luz no es direccional')  
        let v = this.pos_dir_wc_act
        return new Vec3([v.x, v.y, v.z])
    }

    /**
     * Construye una fuente de luz (direccional)
     * @param pos_dir (Vec4) posicion (punto) o dirección (vector) de la fuente ('w' debe ser 0 para direcciones o 1 para posiciones)
     * @param color   color o intensidad de la fuente
     */
    constructor( long_ini : number, lat_ini : number, color_ini : Vec3 )
    {
        const nombref : string = 'FuenteLuz.constructor:'
        
        this.long_act = long_ini
        this.lat_act  = lat_ini
        this.pos_dir_wc_act = this.calcular_pos_dir_wc()

        this.color_act = color_ini
    }

}
// ---------------------------------------------------------------------------------------------------------------

/**
 * Clase para una colección de fuentes de luz (es un array de fuentes de luz)
 */
export class ColeccionFuentesLuz extends Array<FuenteLuz>
{
    constructor( arr_fuentes : FuenteLuz[] )
    {
        super( )
        for( let fuente of  arr_fuentes )
            this.push( fuente )
    }
    /**
     * Activa la colección de fuentes de luz en el cauce actual de la aplicación
     */
    public activar() : void 
    {
        const nombref : string = 'ColeccionFuentes.activar'
        Assert( this.length > 0, `${nombref} no se puede activar una colección de fuentes que está vacía`)

        let pos_dir_wc : Vec4[] = []
        let color      : Vec3[] = []

        for( let fuente of this )
        {
            let v = fuente.dir_wcc
            pos_dir_wc.push( new Vec4([v.x, v.y, v.z, 0.0]) )
            color.push( fuente.color )
        }

        let cauce : Cauce = AplicacionWeb.instancia.cauce
        cauce.fijarEvalMIL( true )
        cauce.fijarFuentesLuz( color, pos_dir_wc )
    }
    public desactivar()
    {
        let cauce : Cauce = AplicacionWeb.instancia.cauce
        cauce.fijarEvalMIL( false )
    }
}
