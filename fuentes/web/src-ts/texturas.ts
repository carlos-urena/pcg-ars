
import { Log, Assert, esPotenciaDe2, ComprErrorGL,
         LeerArchivoImagen, CrearTexturaWebGL } from "./utilidades.js" 
import { Cauce } from "./cauce.js"
import { AplicacionWeb } from "./aplicacion-web.js"



// -------------------------------------------------------------------------------------

/**
 * Clase que encapsula una imagen usada como textura y el 
 * correspondiente objeto textura de WebGL
 */
export class Textura 
{
   private nombre_arch  : string = ""
   private elemento_img : HTMLImageElement | null = null 
   private texture      : WebGLTexture | null = null 
   
   /**
    * Devuelve el objeto textura WebGL, si es null lo crea antes.
    * La textura debe estar ya cargada.
    */
   public get texturaWebGL() : WebGLTexture 
   {
      if ( this.elemento_img == null )
         throw new Error("Textura.texturaWebGL: intento de leer objeto textura WebGL sin la imagen cargada")

      if ( this.texture == null ) 
         this.texture = CrearTexturaWebGL( this.elemento_img )

      return this.texture 
   }

   // --------------------------------------------------------------
   // variables de instancia estáticas ('static'), no específicas de una instancia

   // Textura actualmente activada en el cauce (se usa para push/pop)
   // (si es null es que no hay textura activada)
   //private static actual : Textura | null = null 

   // pila de texturas, inicialmente vacía
   //private static pila : Array<Textura|null> = []

   // -----------------------------------------------------------------

   /**
    * Construye una textura a partir del nombre del archivo en el sevidor (no la lee, se debe llamar a 'leer)
    * @param p_nombre_arch (string) nombre del archivo de textura en el servidor (en la carpeta 'imgs', hermana de 'index.html')
    */
   constructor( p_nombre_arch : string  ) 
   {
      const nombref : string = `Textura.constructor("${p_nombre_arch}")`
      Assert( p_nombre_arch != "" , `${nombref} la url está vacía`)
      this.nombre_arch = p_nombre_arch  
   }

   /**
    * Lee la textura desde su URL (la descarga del servidor)
    * Crea el elemento imagen con los pixels
    */
   async leer() : Promise<void>  
   {
      const nombref : string = 'Textura.leer:'
      Assert( this.elemento_img == null , `${nombref} no se puede leer una textura ya leída (this.url)`)
      this.elemento_img = await LeerArchivoImagen( this.nombre_arch ) 

      Log(`${nombref} textura '${this.nombre_arch}' cargada, dimensiones == ${this.elemento_img.width} x ${this.elemento_img.height}`)
   }

   /**
    * Crea un objeto textura y espera a leerlo desde el servidor
    * 
    * @param url (string) URL del archivo de textura en el servidor 
    * @returns (Textura) textura leída
    */
   public static async crear( url : string ) : Promise<Textura>
   {
      const nombref : string = 'LeerTextura:'
      let textura = new Textura( url )
      await textura.leer() 
      return textura
   }

   
   // -----------------------------------------------------------------------------------------

   // -----------------------------------------------------------------------------------------
   // activar(  ) : void
   // {
   //    const nombref : string = 'Textura.activar:'
   //    let gl = AplicacionWeb.instancia.gl
   //    let cauce : Cauce = AplicacionWeb.instancia.cauce 

   //    if ( this.elemento_img == null )
   //       throw new Error("intento de activar una textura sin la imagen cargada")

   //    ComprErrorGL( gl, `${nombref} al inicio` )

   //    if ( this.texture == null ) 
   //       this.texture = CrearTexturaWebGL( this.elemento_img )
      
   //    //Textura.actual = this 
   //    cauce.fijarEvalText( true, this.texture )

   //    ComprErrorGL( gl, `${nombref} al final` )
   // }
   // // -------------------------------------------------------------------------------------
   // // Métodos estáticos ('static') o de clase (no se ejecutan sobre una instancia)

   // public static desactivar(  ) : void  
   // {
   //    let cauce : Cauce = AplicacionWeb.instancia.cauce 
   //    //Textura.actual = null 
   //    cauce.fijarEvalText( false, null )
   // }
   // --------------------------------------------------------------------
   

   
   // --------------------------------------------------------------------
   
   // /**
   //  * Hace push (en la pila de texturas) de la textura actualmente activada
   //  * (si no hay ninguna textura activada, introduce 'null')
   //  */
   // public static push() : void 
   // {
   //    const nombref : string = "Material.push:"
   //    Textura.pila.push( Textura.actual )
   // }
   // // --------------------------------------------------------------------

   // /**
   //  * Hace pop de la pila de texturas (la pila no puede estar vacía)
   //  * - Lee la textura en el top de la pila.
   //  * - Si esa textura es 'null', desactiva las texturas, en otro caso activa esa textura.
   //  * 
   //  */
   // public static pop() : void 
   // {
   //    const nombref : string = "Material.pop:"
   //    let pt = Textura.pila

   //    if ( pt.length == 0 )
   //       throw new Error(`${nombref} la pila está vacía`)

   //    let text = pt[ pt.length-1 ]
   //    pt.pop()
      
   //    if ( text == null )
   //       Textura.desactivar()
   //    else
   //       text.activar()
   // }
}
// -----------------------------------------------------------------------------------------




