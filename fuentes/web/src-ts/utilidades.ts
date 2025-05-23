
import { Vec2, Vec3, Vec4, UVec3 } from "./vec-mat.js"
import { DescrVAO, DescrVBOAtrib, DescrVBOInd } from "./vaos-vbos.js"
import { Cauce } from "./cauce.js"
import { ObjetoVisualizable } from "./objeto-visu.js"
import { AplicacionWeb } from "./aplicacion-web.js"

// -----------------------------------------------------------------------------

const debug : boolean  = true // indica si se comprueban los asserts y se hace log


// Tipo para tablas o arrays que contienen un número de flotantes múltiplo de 3
export type TablaFloatV3  = Float32Array | number[] | Vec3[]  

// Tipo para tablas o arrays que contienen un número de flotantes múltiplo de 2
export type TablaFloatV2  = Float32Array | number[] | Vec2[]  

// Tipo para tablas o arrays que contienen una secuencia de enteros sin signo o de UVec3
export type TablaUnsigned = Uint8Array | Uint16Array | Uint32Array  | number[] | UVec3[] 

// ----------------------------------------------------------------------------

/**
 * Tipo para un contexto de rendering WebGL, que puede ser de la 
 * versión 1 (WebGLRenderingContext) o de la 2 (WebGL2RenderingContext)
 */
export type ContextoWebGL = WebGLRenderingContext | WebGL2RenderingContext

// ----------------------------------------------------------------------------
/**
 * Convierte un booleano a un 'number' (entero).
 * Si 'b' es true, devuelve 1, en otro caso devuelve 0.
 * 
 * @param b booleano a convertir
 * @returns entero convertido
 */
export function b2n( b : Boolean ) : number 
{
    return b ? 1 : 0 
} 

// -------------------------------------------------------------------------

export function glsl( s : TemplateStringsArray ) : string 
{
    return s.raw[0]
}

// -------------------------------------------------------------------

/**
 * Clase para el estado del ratón durante el arrastre
 */
export class EstadoRaton 
{
   public boton_der_abajo : Boolean = false // true si el botón derecho está abajo 
   public inicio_x : number = 0 // si botón derecho abajo, coord X de la posición del click de la 
   public inicio_y : number = 0 // si botón derecho abajo, coord Y de la posición de 
}

// -----------------------------------------------------------------------------

/**
 * Clase para la caja englobante de un conjunto de puntos
 */
export class CajaEnglobante
{
   private min_pos : Vec3 = new Vec3([0,0,0])
   private max_pos : Vec3 = new Vec3([0,0,0])

   /**
    * Construye una caja englobante dados dos puntos opuestos
    * @param min_ini 
    * @param max_ini 
    */
   public constructor( min_ini : Vec3, max_ini : Vec3 )
   {
      const fname = "CajaEnglobante.constructor"

      for( let i = 0; i < 3; i++ ) 
         if ( min_ini[i] > max_ini[i] )
            throw new Error(`${fname} parámetros iniciales de la caja englobante son erróneos`)

      this.min_pos = min_ini.clonar()
      this.max_pos = max_ini.clonar() 
   }
   /**
    * Construye una caja englobante minima que incluye todas las posiciones en un vector de puntos.
    * @param puntos vector de posiciones de puntos, no puede estar vacío (Vec3[])
    * @returns la caja englobante
    */
   public static desdePuntos( puntos : Vec3[] ) : CajaEnglobante
   {
      if ( puntos.length == 0 )
         throw new Error("CajaEnglobante.dedePuntos: no hay puntos para crear la caja")

      let caja = new CajaEnglobante( puntos[0], puntos[0] )
      for( let i = 1; i < puntos.length; i++ )
         caja.mezcla( puntos[i] )
      return caja
   }

   /**
    * Hace una 'mezcla' de esta caja englobante con un punto, es decir,
    * agranda la caja englobantes lo mínimo para que contenga al punto.
    * @param pos punto a mezclar
    */
   public mezcla( pos : Vec3 )
   {
      for( let i = 0; i < 3; i++ )  
      {
         if ( pos[i] > this.max_pos[i] )
            this.max_pos[i] = pos[i]
         else if ( pos[i] < this.min_pos[i] )
            this.min_pos[i] = pos[i]
      }
   }
   public get min() 
   {
      return this.min_pos 
   }
   public get max()
   {
      return this.max_pos 
   }
   public get centro() : Vec3
   {
      return this.min_pos.mas( this.max_pos ).mult( 0.5 )
   }
   public get diagonal() : Vec3
   {
      return this.max_pos.menos( this.min_pos )
   }
   public get radio() : number
   {
      return this.diagonal.longitud * 0.5
   }
}

// -----------------------------------------------------------------------------

/**
 * Si la condición (cond) es falsa, lanzar una excepción con el mensaje
 * (cuando debug == false, no hace nada)
 * 
 * @param cond (boolean) condición a evaluar
 * @param msg  (string, opcional) mensaje de la excepción cuando se lanza
 */
export function Assert( condition : boolean, msg? : string ) : asserts condition    
{
   if ( debug )
      if ( condition == false ) 
      {
         let texto : string = ""
         if ( msg === undefined )
            texto = ", pero no hay texto descriptivo del Assert."
         else 
            texto = `: ${msg}`

         throw new Error( `la condición de un Assert es falsa ${texto}` )
      }
}

// -----------------------------------------------------------------------------

/**
 * Si la variable de 'debug' es 'true', escribir el mensaje en la consola
 * (cuando debug == false, no hace nada)
 * @param msg mensaje a imprimir 
 */
export function Log( msg : string ) : void
{
   if ( debug )
      console.log( msg )
}

// -----------------------------------------------------------------------------

/**
 * Comprueba si ha habido un error de OpenGL, si lo ha habido lanza una excepción 
 * (cuando debug == false, no hace nada)
 * @param gl  contexto donde se comprueba el error
 * @param msg texto de la excepción en caso de error
 */
export function ComprErrorGL( gl : WebGLRenderingContext | WebGL2RenderingContext, msg : string ) : void
{
   Assert( gl.getError() == gl.NO_ERROR, msg )
}

// -------------------------------------------------------------------

/**
 * Construye y devuelve una promesa que se resuelve cuando hayan pasado un 
 * número de milisegundos que se pasa como parámetro
 * 
 * @param milliseconds - número de milisegundos
 * @returns - una promesa (vacía, de tipo void)
 */
export function Milisegundos( milliseconds : number ) : Promise<void> 
{
   return new Promise( resolve => setTimeout(resolve, milliseconds) ) 
}

// -----------------------------------------------------------------------------

/**
 * Función para concatenar las sub-cadenas en una cadena literal etiquetada como 'html'
 * @param cadenas (TemplateStringsArray) secuencia de cadenas
 * @returns (string) cadenas concatenadas
 */
export function html( cadenas : TemplateStringsArray ) : string 
{
   let salida : string = ""
   for( const s of cadenas ) salida = salida + " " + s 
   return salida
}
// -----------------------------------------------------------------------------

/**
 * Convierte un flotante 'v' (entre 0 y 1) en una cadena con el valor de 'v' 
 * en hexadecimal y con exactamente dos digitos
 * @param v 
 */
export function Hex2( v : number ) : string 
{
   Assert( 0.0 <= v && v <= 1.0, "Hex2: el valor no está entre 0 y 1" )
   const k : number = Math.round( v*255.0 )
   const res : string = k.toString(16)
   return res.length == 2 ? res : `0${res}`
   
}
// -----------------------------------------------------------------------------

/**
 * Devuelve un número entre 0 y 1 a partir de una cadena con dos dígitos hexadecimales 
 * @param h2 (string) cadena con dos caracteres codificando un dígito hexadecimal, analizable con 'parseInt'
 * @return (number) número entre 0 y 1
 */
export function NumbDesdeHex2( h2 : string ) : number 
{
   const nombref : string = `NumbDesdeHex2:`
   Assert( h2.length == 2 , `${nombref} la cadena ${h2} no tiene exactamente dos caracteres`)
   const n : number = parseInt( h2, 16 )
   Assert( ! Number.isNaN(n), `${nombref} La cadena '${h2}' no puede interpretarse como un entero en hexadecimal` )
   Assert( n >= 0 , `${nombref} la cadena '${h2}' codifica un número negativo`)
   return n/255.0
}
// -----------------------------------------------------------------------------

/**
 * Leer un archivo en un servidor, esperar a que se cargue entero y devolver el 
 * contenido como una promesa con una cadena de texto dentro
 * 
 * @param url_arch cadena con el URL del archivo
 * @returns (string) cadena codificada en UTF8
 */
export async function LeerArchivoTexto( url_arch : string ) : Promise<string>
{ 
   const nombref : string = `LeerArchivoTexto("${url_arch}")`

   // obtener una promesa ('Response') que se resuelve cuando se lee un archivo de la red o el disco.
   let response : Response = await fetch( url_arch )

   // comprobar respuesta (la promesa devuelta por fetch no se rechaza por errores de http como 404)
   if ( ! response.ok ) 
      throw new Error(`${nombref} imposible leer archivo '${url_arch}'`)
      
   // .text devuelve una promesa que se resuelve con una representación UTF-8 del objeto recuperado con el 'fetch'
   let texto : string = await response.text() 
   
   return texto
} 

// -----------------------------------------------------------------------------

/**
 * Leer un archivo GLSL en el servidor, esperar a que se cargue entero y devolver el 
 * contenido como una promesa con una cadena de texto dentro
 * 
 * @param nombre cadena con el nombre del archivo glsl (en la carpeta 'glsl', hermana de 'index.html')
 * @returns (string) cadena codificada en UTF8
 */
export async function LeerArchivoGLSL(  nombre : string ) : Promise<string>
{ 
   const nombref : string = `LeerArchivoGLSL("${nombre}"):`
   let url_arch : string = `glsl/${nombre}`

   return LeerArchivoTexto( url_arch )
} 
// -------------------------------------------------------------------------------------

/**
 * Leer un archivo GLSL en el servidor, esperar a que se cargue entero y devolver el 
 * contenido como una promesa con una cadena de texto dentro
 * 
 * @param nombre cadena con el nombre del archivo glsl (en la carpeta 'plys', hermana de 'index.html')
 * @returns (string) cadena codificada en UTF8
 */
export async function LeerArchivoPLY(  nombre : string ) : Promise<string>
{ 
   const nombref : string = `LeerArchivoPLY("${nombre}"):`
   let url_arch : string = `plys/${nombre}`

   return LeerArchivoTexto( url_arch )
} 
// -------------------------------------------------------------------------------------

/**
 * Leer una imagen desde una URL y devolver un elemento imagen, ver:
 * https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch
 * (sección: Supplying your own Request Object)
 * 
 * @param nombre_arch (string) nombre del archivo de imagen (en la carpeta 'imgs', hermana de 'index.html')
 * @returns (HTMLImageElement) elemento imagen
 */
export async function LeerArchivoImagen( nombre_arch : string ) : Promise<HTMLImageElement>
{
   const nombref : string = 'LeerArchivoImagen:' 

   let url_string = `imgs/${nombre_arch}`
   
   // obtener una promesa ('Response') que se resuelve cuando se lee un archivo de la red o el disco.
   let response : Response = await fetch( url_string )

   // comprobar respuesta (la promesa devuelta por fetch no se rechaza por errores de http como 404)
   if ( ! response.ok ) 
      throw new Error(`${nombref} imposible leer archivo de imagen '${url_string}'`)

   // obtener un objeto con bytes (un "Blob") que contiene la imagen
   let blob : Blob = await response.blob() 

   // crear un elemento imagen vacío
   let elemento_img : HTMLImageElement = document.createElement( "img" ) 

   try 
   {
      // creando una URL "virtual" que apunta al blob, y asignarla a .src 
      // (pone la imagen a cargar)
      elemento_img.src = URL.createObjectURL( blob ) 

      // esperar a que la imagen se decodifique
      await elemento_img.decode()
   }
   catch( err : any )
   {
      throw new Error(`${nombref} error al decodificar archivo de imagen '${url_string}', mensaje: \n ${err.message}`)
   }

   // ya está 
   return elemento_img
} 

// -----------------------------------------------------------------------------

export class VAOEjes extends DescrVAO 
{

   constructor(  )
   {
      
      const numero_atributos_vao : number = 2
      const lon : number = 15.0 
      super
      ({ 
         posiciones :
         [ 
            -0.5*lon, 0.0,      0.0,      lon,  0.0,  0.0,
            0.0,     -0.5*lon,  0.0,      0.0,  lon,  0.0, 
            0.0,      0.0,     -0.5*lon,  0.0,  0.0,  lon 
         ],
         colores : 
         [
            1.0, 0.0, 0.0,  1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,  0.0, 1.0, 0.0, 
            0.0, 0.0, 1.0,  0.0, 0.0, 1.0
         ]
      })

      this.nombre = 'Ejes'
   }

}
// -----------------------------------------------------------------------------

export class Ejes extends ObjetoVisualizable
{
   private vao : VAOEjes | null = null 
   public visualizar(): void 
   {
      let gl = AplicacionWeb.instancia.gl
      let cauce = AplicacionWeb.instancia.cauce
      
      this.guardarCambiarEstado( cauce )
      if ( this.vao == null )
         this.vao = new VAOEjes()
      this.vao.draw( gl.LINES )
      this.restaurarEstado( cauce )
   }
}

// -----------------------------------------------------------------------------

/**
 * Una rejilla de lineas, en el plano Z=0, entre 0 y 1
 */
export class RejillaXY extends ObjetoVisualizable
{
   private dvao : DescrVAO 
   //private gl   : WebGLRenderingContext | WebGL2RenderingContext

   constructor(  )
   {
      super()
      let gl = AplicacionWeb.instancia.gl
      this.nombre = "RejillaXY"

      const nx : number = 8, ny : number = 8
      const sx : number = 1.0/nx ,
            sy : number = 1.0/ny 

      let arr_verts : Array<number> = new Array<number>

      for( let ix = 0 ; ix < nx ; ix++ )
         arr_verts.push( ix*sx, 0.0, 0.0,   ix*sx, 1.0, 0.0 )
      
      for( let iy = 0 ; iy < nx ; iy++ )
         arr_verts.push( 0.0, iy*sy, 0.0,   1.0, iy*sy, 0.0 )
     
     
      this.dvao = new DescrVAO({ posiciones: arr_verts })
   }

   public visualizar( ): void 
   {
      let gl = AplicacionWeb.instancia.gl 
      let cauce = AplicacionWeb.instancia.cauce 

      this.guardarCambiarEstado( cauce )
      this.dvao.draw( gl.LINES )   
      this.restaurarEstado( cauce )
   }

   public visualizarAristas( ): void 
   {
        
   }

}

// -----------------------------------------------------------------------------

export class TrianguloTest extends ObjetoVisualizable
{
   private dvao : DescrVAO 
   //private gl   : WebGLRenderingContext | WebGL2RenderingContext

   constructor(  )
   {
      super()
      this.nombre = "Triángulo test"

      const num_atribs : number = 3
      this.dvao = new DescrVAO
      ({
         posiciones: 
         [ 
            -0.4, -0.4, 0.0,  
            +0.4, -0.4, 0.0,   
            +0.0, +0.4, 0.0,   
         ],

         colores:
         [
            1.0, 0.0, 0.0,  
            0.0, 1.0, 0.0,   
            0.0, 0.0, 1.0,  
         ],
      
         normales: 
         [
            0.0, 0.0, 1.0,  
            0.0, 0.0, 1.0,   
            0.0, 0.0, 1.0,  
         ]
      })

   }

   public visualizar( ): void 
   {
      let gl = AplicacionWeb.instancia.gl 
      let cauce = AplicacionWeb.instancia.cauce 

      this.guardarCambiarEstado( cauce )
      this.dvao.draw( gl.TRIANGLES )   
      this.restaurarEstado( cauce )
   }

   public visualizarAristas( ): void 
   {
        
   }
}
// -----------------------------------------------------------------------------

export class TrianguloIndexadoTest extends ObjetoVisualizable
{
   private dvao : DescrVAO 
   //private gl   : WebGLRenderingContext | WebGL2RenderingContext

   constructor(  )
   {
      super()
      let gl = AplicacionWeb.instancia.gl
      this.nombre = "Triángulo ind."

      const num_atribs : number = 3
      this.dvao = new DescrVAO
      ({
         posiciones: 
         [ 
            -0.4, -0.4, 0.0,  
            +0.4, -0.4, 0.0,   
            +0.0, +0.4, 0.0,   
         ],
         indices: 
         [
            0, 1, 2
         ],
         colores: 
         [
            1.0, 0.0, 0.0,  
            0.0, 1.0, 0.0,   
            0.0, 0.0, 1.0,  
         ],
         normales: 
         [
            0.0, 0.0, 1.0,  
            0.0, 0.0, 1.0,   
            0.0, 0.0, 1.0,  
         ]
      })
   }

   public visualizar( ): void 
   {
      let gl = AplicacionWeb.instancia.gl
      let cauce = AplicacionWeb.instancia.cauce 

      this.guardarCambiarEstado( cauce )
      this.dvao.draw( gl.TRIANGLES )   
      this.restaurarEstado( cauce )
   }

   public visualizarAristas( ): void 
   {
        
   }
}
// -----------------------------------------------------------------------------

/**
 * Convierte un array de Vec3 en un Float32Array, 
 * @param v 
 */
export function CrearFloat32ArrayV4( v : Vec4[] ) : Float32Array 
{      
   let array = new Float32Array( v.length*4 )
   
   for( let i = 0 ; i < v.length ; i++ )
   {
      array[4*i+0] = v[i].x 
      array[4*i+1] = v[i].y 
      array[4*i+2] = v[i].z 
      array[4*i+3] = v[i].w 
   }
   return array
}
// -----------------------------------------------------------------------------


/**
 * Convierte un array genérico (de números, o tuplas de 3 números) (de tipo TablaFloatV3) 
 * en un 'Float32Array' de valores 'float' de 4 bytes cada uno.
 * 
 * - Si 'tabla' es de tipo 'Float32Array', devuelve 'tabla', en otro caso hace copia de los datos
 * - La tabla resultado tiene un número de floats múltiplo de 2 siempre
 * 
 * @param tabla datos de entrada, puede ser 'number[]' 'Vec3[]' o 'Float32Array'
 */
export function CrearFloat32ArrayV3( tabla : TablaFloatV3 ) : Float32Array 
{      
   const nombref : string = "CrearFloat32ArrayV2:"
   const lonvec  : number = 3

   if ( tabla instanceof Float32Array )
   {
      Assert( tabla.length % lonvec == 0, `${nombref} la tabla no tiene longitud múltiplo de ${lonvec}` )
      return tabla 
   }
   else if ( tabla instanceof Array )
   {
      if ( tabla.length == 0 )
         throw new Error(`${nombref} la tabla está vacía`)

      if ( typeof tabla[0] == "number" )
      {
         Assert( tabla.length % lonvec == 0, `${nombref} la tabla no tiene longitud múltiplo de ${lonvec}` )
         let array = new Float32Array( tabla.length )
   
         for( let i = 0 ; i < tabla.length ; i++ )
         {
            let v_i = tabla[i]
            if ( typeof v_i != "number" )
               throw new Error(`${nombref} la tabla de valores 'number' tiene una entrada de otro tipo`)
            array[i] = v_i
         }
         return array
      }
      else if ( tabla[0] instanceof Vec3 )
      {
         let array = new Float32Array( tabla.length*lonvec )
   
         for( let i = 0 ; i < tabla.length ; i++ )
         {
            let v_i = tabla[i]
            if ( ! ( v_i instanceof Vec3 ) )
               throw new Error(`${nombref} la tabla de vectores 'Vec2' tiene una entrada de otro tipo`)
            for( let j = 0 ; j < lonvec ; j++ )
               array[lonvec*i+j] = v_i[j]
         }
         return array
      }
      else 
         throw new Error( `${nombref} la tabla es un 'Array' pero no de 'number' ni 'Vec3'`)
   }
   else 
      throw new Error( `${nombref} la tabla no es un 'Array' ni un 'Float32Array'`)
}

// -----------------------------------------------------------------------------

/**
 * Convierte un array genérico (de números, o tuplas de 2 números) (de tipo TablaFloatV3) 
 * en un 'Float32Array' de valores 'float' de 4 bytes cada uno.
 * 
 * - Si 'tabla' es de tipo 'Float32Array', devuelve 'tabla', en otro caso hace copia de los datos
 * - La tabla resultado tiene un número de floats múltiplo de 2 siempre
 * 
 * @param tabla datos de entrada, puede ser 'number[]' 'Vec3[]' o 'Float32Array'
 */
export function CrearFloat32ArrayV2( tabla : TablaFloatV2 ) : Float32Array 
{      
   const nombref : string = "CrearFloat32ArrayV2:"
   const lonvec  : number = 2

   if ( tabla instanceof Float32Array )
   {
      Assert( tabla.length % lonvec == 0, `${nombref} la tabla no tiene longitud múltiplo de ${lonvec}` )
      return tabla 
   }
   else if ( tabla instanceof Array )
   {
      if ( tabla.length == 0 )
         throw new Error(`${nombref} la tabla está vacía`)

      if ( typeof tabla[0] == "number" )
      {
         Assert( tabla.length % lonvec == 0, `${nombref} la tabla no tiene longitud múltiplo de ${lonvec}` )
         let array = new Float32Array( tabla.length )
   
         for( let i = 0 ; i < tabla.length ; i++ )
         {
            let v_i = tabla[i]
            if ( typeof v_i != "number" )
               throw new Error(`${nombref} la tabla de valores 'number' tiene una entrada de otro tipo`)
            array[i] = v_i
         }
         return array
      }
      else if ( tabla[0] instanceof Vec2 )
      {
         let array = new Float32Array( tabla.length*lonvec )
   
         for( let i = 0 ; i < tabla.length ; i++ )
         {
            let v_i = tabla[i]
            if ( ! ( v_i instanceof Vec2 ) )
               throw new Error(`${nombref} la tabla de vectores 'Vec2' tiene una entrada de otro tipo`)
            for( let j = 0 ; j < lonvec ; j++ )
               array[lonvec*i+j] = v_i[j]
         }
         return array
      }
      else 
         throw new Error( `${nombref} la tabla es un 'Array' pero no de 'number' ni 'Vec3'`)
   }
   else 
      throw new Error( `${nombref} la tabla no es un 'Array' ni un 'Float32Array'`)
}
// -----------------------------------------------------------------------------


/**
 * Convierte un array de datos unsigned (tipo TablaUnsigned) en un Uint32Array, 
 * @param v 
 */
export function CrearUint32Array( tabla : TablaUnsigned ) : Uint32Array 
{      
   const nombref : string = 'CrearUint32Array:'

   if ( tabla instanceof Uint32Array )
      return tabla   
   else if ( tabla instanceof Uint8Array || tabla instanceof Uint16Array )   
      return new Uint32Array( tabla )
   else if ( tabla instanceof Array )
   {
      if ( tabla.length == 0 )
         throw new Error(`${nombref} la tabla está vacía`)

      if ( typeof tabla[0] == "number" )
      {
         let array = new Uint32Array( tabla.length )
   
         for( let i = 0 ; i < tabla.length ; i++ )
         {
            let v_i = tabla[i]
            if ( typeof v_i != "number" )
               throw new Error(`${nombref} la tabla de valores 'number' tiene una entrada de otro tipo`)
            array[i] = v_i
         }
         return array
      }
      else if ( tabla[0] instanceof UVec3 )
      {
         let array = new Uint32Array( tabla.length*3 )
   
         for( let i = 0 ; i < tabla.length ; i++ )
         {
            let v_i = tabla[i]
            if ( ! ( v_i instanceof UVec3 ) )
               throw new Error(`${nombref} la tabla de vectores 'UVec3' tiene una entrada de otro tipo`)
            for( let j = 0 ; j < 3 ; j++ )
               array[ 3*i+j ] = v_i[j]
         }
         return array
      }
      else 
         throw new Error( `${nombref} la tabla es un 'Array' pero no de 'number' ni 'UVec3'`)
   }
   else 
      throw new Error( `${nombref} la tabla no es un 'Array' ni un 'Float32Array'`)
   
  
}
// -----------------------------------------------------------------------------

/**
 *  devuelve true si el argumento es una potencia de 2
 *  obetnido de: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Tutorial/Using_textures_in_WebGL
 *    @param   (number) value -- value to test
 *    @returns (Boolean)         -- true iif value is 2^n for some integer 'n', false otherwise
 */
export function esPotenciaDe2(value : number) : Boolean
{
    return (value & (value - 1)) == 0
}
// -----------------------------------------------------------------------------

export function CrearTexturaWebGL( img : HTMLImageElement ) : WebGLTexture 
   {
      const nombref : string = 'Textura.crearTexturaWebGL'
      
      let gl = AplicacionWeb.instancia.gl

      ComprErrorGL( gl, `${nombref} al inicio`)

      const
        level       : number = 0,
        internalFmt : number = gl.RGB,   
        srcFmt      : number = gl.RGB,   //// ---> como saberlo ??
        srcType     : number = gl.UNSIGNED_BYTE

      // create, bind and fill the texture
      let textura_wgl = gl.createTexture()
      
      if ( textura_wgl == null )
         throw new Error("imposible crear textura, gl.createTexture devuelve null")

      gl.bindTexture( gl.TEXTURE_2D, textura_wgl )
      gl.texImage2D( gl.TEXTURE_2D, level, internalFmt, srcFmt, srcType, img )

      // Generate MIPMAPS ....
      // WebGL1 has different requirements for power of 2 images
      // vs non power of 2 images so check if the image is a
      // power of 2 in both dimensions.
      if ( esPotenciaDe2( img.width ) && esPotenciaDe2( img.height ))
      {
         // Yes, it's a power of 2. Generate mips.
         gl.generateMipmap( gl.TEXTURE_2D )
         Log(`${nombref} generado mip-map.`)
      }
      else
      {
         // No, it's not a power of 2. Turn off mips and set
         // wrapping to clamp to edge
         gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
         gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
         gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
         Log(`${nombref} mip-map no generado.`)
      }

      gl.bindTexture( gl.TEXTURE_2D, null )
      ComprErrorGL( gl, `${nombref} al final`)

      return textura_wgl

   }

// adaptado a typescript desde:
// https://stackoverflow.com/questions/1013239/can-i-get-the-name-of-the-currently-running-function-in-javascript

/**
 * 
 * @returns 
 */
export function getFunctionCallerName ()
{
   // gets the text between whitespace for second part of stacktrace
   let err : Error = new Error()
   let match = err.stack?.match(/at (\S+)/g)
   
   return match ? match[1].slice(3) : "nombre de función no se sabe"
   
}

