import { Log, ComprErrorGL, Assert, html, Milisegundos,
         ContextoWebGL, EstadoRaton,
         Ejes, 
         RejillaXY, TrianguloTest, TrianguloIndexadoTest, 
         LeerArchivoTexto,
       } from "./utilidades.js"
import { Cauce, CrearCauce } from "./cauce.js"
import { ObjetoVisualizable } from "./objeto-visu.js"
import { CamaraInteractiva, CamaraOrbital3D, CamaraVista2D, Viewport } from "./camaras.js"
import { Vec3, Vec4, Mat4, CMat4, Vec3DesdeColorHex } from "./vec-mat.js"
import { MallaInd, Cubo24, CuadradoXYTextura } from "./malla-ind.js"
import { Textura } from "./texturas.js"
import { MallaPLY } from "./malla-ply.js"
import { CrearInputCheckbox, CrearSelector, CrearInputColor, CrearInputSlider, CrearInputBoton } from "./controles.js"
import { FuenteLuz, ColeccionFuentesLuz } from "./fuente-luz.js"
import { MallaEsfera, MallaCilindro, MallaCono, MallaColumna, MallaCuadradoXY, MallaToroide, MallaCampoAlturas } from "./malla-sup-par.js"
import { Material } from "./material.js"
import { GrafoTest, GrafoTest2, GrafoTest3SombrasTextura } from "./grafo-escena.js"
import { OC_GrafoTest, OC_GrafoTest2 } from "./objeto-comp.js"
import { ObjetoAnimado, EstadoAnim } from "./objeto-anim.js"
import { EsferaRotacion } from "./animaciones.js"
import { CauceSombras } from "./cauce-sombras.js"

// -----------------------------------------------------------------------

// Función (no miembro) que visualiza el frame de la aplicación en el estado actual.

function VisualizarFrameAplicacionWeb()
{
   let app = AplicacionWeb.instancia 
   app.visualizarFrame()
}

// -------------------------------------------------------------------

/**
 * Clase con la funcionalidad de la aplicación Web.
 * Cumple el papel de 'clase Controlador' en el paradigma Modelo-Vista-Controlador.
 * 
 *  - La 'vista' es la página HTML, el canvas y el cuadro de controles
 *  - El 'modelo' es la colección de modelos de objetos (instancias de 'ObjetoVisualizable')
 */
export class AplicacionWeb 
{
   
    // ---------------------------------------
   // Variables 'de clase' (o estáticas)
   // ---------------------------------------

   /**
    * instancia única (singleton) de esta clase ( estática )
    * se lee con el método 'instancia' (tipo get, estático)
    */
   private static instancia_apl_pcg : AplicacionWeb | null = null 


   // ------------------------------
   // Variables de instancia
   // ------------------------------

   /**
    * Objeto con el 'rendering context' de WebGL
    * (puede ser de tipo WebGL 2 o WebGL 1)
    */
   private gl_act : WebGL2RenderingContext | WebGLRenderingContext

   /**
    * Elemento html (de tipo 'canvas') sobre el que se realiza el rendering
    */
   private canvas! : HTMLCanvasElement  
   
   /**
    * Elemento html (div) que contiene el canvas
    */
   private contenedor : HTMLDivElement  
   
   /**
    * Identificador del elemento html (div) que contiene el canvas
    */
   private id_contenedor : string

   /**
    * Elemento html (div) que contiene los controles
    */
   private controles : HTMLDivElement  
   
   /**
    * Identificador del elemento html (div) que contiene el canvas
    */
   private id_controles : string
   
   /**
    * Objeto Cauce
    */
   private cauce_actual! : Cauce 

   /**
    *  Estado del ratón 
    */
   private estado_raton : EstadoRaton = new EstadoRaton

   /**
    * Cámara en uso
    */
   //private camara : CamaraOrbital3D --> se usa vector de cámaras

   
   /**
    * Objeto para visualizar los ejes de coordenadas  
    */
   private ejes : Ejes 

   
   /**
    * Elemento HTML (div) en el pie de la página, donde se visualizan los mensajes de estado 
    * (null al principio o si no se encuentra)
    */
   private pie : HTMLElement | null = null 

   /**
    *  Colección de objetos cargados
    */
   private objetos : ObjetoVisualizable[] = []

   /**
    * Array con las cámaras, (debe haber una por cada objeto)
    */
   private camaras : CamaraInteractiva[] = []

   /**
    * Objeto que actualmente se está visualizando 
    */
   private indice_objeto_actual : number = 0 

   /**
    * True si queremos visualizar aristas, false si no,
    */
   private visualizar_aristas : boolean = false 

   /**
    * True si queremos visualizar ejes, false si no,
    */
   private visualizar_ejes : boolean = true

   /**
    * Elemento HTML de tipo 'input' (checkbox) para el botón de visualizar ejes si/no
    */
   private input_boton_ejes : HTMLInputElement | null = null

   /**
    * Elemento HTML de tipo 'input' (checkbox) para el botón de las aristas
    */
   private input_boton_aristas : HTMLInputElement | null = null

   /**
    * Elemento HTML de tipo 'input' (buton) para el botón de animación
    */
   private input_boton_estado_anim : HTMLInputElement | null = null
   private input_boton_reset_anim  : HTMLInputElement | null = null

   /**
    * True si queremos visualizar las normales, false si no,
    */
   private visualizar_normales : boolean = false 

   /**
    * Elemento HTML de tipo 'input' (checkbox) para el botón de visualizar normales
    */
   private input_boton_normales : HTMLInputElement | null = null

   /**
    * Elemento HTML de tipo 'input' (checkbox) para el botón de activar/desactivar iluminación
    */
   private input_boton_iluminacion : HTMLInputElement | null = null

   /**
    * Indica si la iluminación está activada o no 
    */
   private iluminacion : boolean = true

   /**
    * Elemento HTML de tipo 'input' (checkbox) para el botón de activar/desactivar sombras arrojadas
    */
   private input_boton_sombras_arrojadas : HTMLInputElement | null = null

   /**
    * Indica si las sombras arrojadas  están activadas o no 
    */
   //private sombras_arrojadas : boolean = true
   
   /**
    *  Elemento HTML de tipo 'select' para el selector de objeto actual
    */
   private selector_objeto_actual : HTMLSelectElement | null = null

   /**
    * Elemento HTML de tipo 'input' para el slider del parámetro S
    */
   private input_param_S : HTMLInputElement | null = null

   /**
    * Valor inicial del parámetro S (estático, es una constante)
    */
   public static valor_inicial_param_S : number = 0.1 

   /**
    * Valor actual del parámetro S
    */
   private param_S : number = AplicacionWeb.valor_inicial_param_S

   /**
    * Longitud de la fuente de luz (direccional)
    */
   private long_luz : number = 45.0

   /**
    * Latitud de la fuente de luz (direccional)
    */
   private lat_luz : number = 45.0

   /**
    * Elemento HTML de tipo 'input' (slider) para la longitud de la fuente de luz
    */
   private slider_long_luz : HTMLInputElement | null = null

   /**
    * Elemento HTML de tipo 'input' (slider) para la latitud de la fuente de luz
    */
   private slider_lat_luz : HTMLInputElement | null = null

   /**
    * Color inicial al visualizar un frame (Vec3 con valores entre 0 y 1)
    */
   private color_defecto : Vec3 = new Vec3([ 0.8, 0.8, 0.8 ])

   /**
    * Elemento HTML de tipo 'input' (color) para el color inicial al visualizar un frame
    */
   private input_color_defecto : HTMLInputElement | null = null 

   /**
    * Colección de fuentes de luz 
    */
   private col_fuentes : ColeccionFuentesLuz = new ColeccionFuentesLuz
   ([ 
      new FuenteLuz(  45.0, +45.0, new Vec3([ 1.0, 1.0, 1.0 ]) ),  
      new FuenteLuz( 170.0, -20.0, new Vec3([ 0.4, 0.2, 0.2 ]) )
   ])
    
   /**
    * Indice de la fuente de luz actual
    */
   private ind_fuente : number = 0

   /**
    * Material por defecto:
    */
   private material_defecto : Material = new Material( 0.1, 0.5, 0.7, 20.0 )

   /**
    * Cauce de sombras (test)
    */
   private cauce_sombras : CauceSombras | null = null

   /**
    * Activa/desactiva evaluación de sombras arrojadas por la fuente 0
    */
   private evaluar_sombras : boolean = true

   /**
    * Resolucion del fbo de sombras (número de píxeles en X e Y, cuadrado)
    */
   private res_fbo_somb : number = 2048

   /**
    * cuenta de frames visualizados
    */
   private cuenta_frames : number = 0

   // -------------------------------------------------------------------------
   
   /**
    * método 'get' (estático) para obtener la instancia (única) de AplicacionWeb 
    * (si es nula se produce un error)
    */
   public static get instancia() : AplicacionWeb 
   {
      if ( AplicacionWeb.instancia_apl_pcg == null )
         throw Error("AplicacionWeb.instancia: no se puede obtener la instancia, todavía no se ha creado")

      return AplicacionWeb.instancia_apl_pcg
   }
   // -------------------------------------------------------------------
 

   /**
    * método 'get' (estático) para devolver la instancia (posiblemente 'null' si no se ha creado todavía)
    */
   public static get instancia_o_null() : AplicacionWeb | null  
   {
      return AplicacionWeb.instancia_apl_pcg
   } 
   // -------------------------------------------------------------------

   /**
    * método (estático) para destruir la instancia única de la aplicación
    */
   public static anularInstancia() : void 
   {
      let inst = AplicacionWeb.instancia_apl_pcg 

      if ( inst != null )
      {
         console.log("Destruyendo la instancia.")
         inst.desactivarFGEs()
         AplicacionWeb.instancia_apl_pcg = null 
      }
      else 
         console.log("La instancia ya estaba destruida.")
   }

   // -------------------------------------------------------------------

   /**
    * Constructor de AplicacionWeb 
    * 
    *   * Registra la instancia como la instancia única (una sola vez)
    *   * Inicializa las variables de instancia que no dependan del Cauce y que 
    *     no requieren descargar archivos del servidor
    */
   constructor(  )
   {
      const nombref : string = "AplicacionWeb.constructor:" 

      // Registrar esta instancia como la instancia única (singleton) de la clase AplicacionWeb
      // (comprobando antes que no estaba ya creada)
      
      if ( AplicacionWeb.instancia_apl_pcg != null )
         throw Error(`${nombref} intento de crear más de una instancia de la clase 'AplicacionWeb'`)
      
      AplicacionWeb.instancia_apl_pcg = this 

      // Recuperar los elementos HTML que debe haber en la página y que esta aplicación usa.

      this.id_contenedor = "contenedor_canvas_pcg"  
      this.id_controles  = "contenedor_controles_pcg"

      this.contenedor  = this.obtenerElementoContenedor( this.id_contenedor )
      this.controles   = this.obtenerElementoControles( this.id_controles )
      this.canvas      = this.obtenerCrearElementoCanvas( this.contenedor )
      
      // Obtener el contexto WebGL a partir del elemento canvas
      
      this.gl_act = this.obtenerContextoWebGL( this.canvas ) 

      // Crea el objeto para visualizar los ejes
      this.ejes = new Ejes( )
   }
   // -------------------------------------------------------------------------

   /**
    * Muestra una línea en la caja de estado al pie de la página
    * (si no existe el elemento 'pie', no hace nada)
    * 
    * @param linea (string) linea a visualizar
    */
   public set estado( linea : string ) 
   {
      if (this.pie == null )
         this.pie = document.getElementById("pie")
      if (this.pie == null )
         return
      
      this.pie.textContent = linea
   }
   // -------------------------------------------------------------------------
   /**
    * Inicialización de la aplicación (posterior al constructor)
    * 
    *    * Crea el cauce gráfico de la aplicación
    *    * Instala los gestores de eventos (así no se invocan nunca antes de que haya un cauce)
    *    * Añade objetos con texturas o modelos que deben descargarse del servidor.
    */
   public async inicializar() : Promise<void>
   {
      const nombref : string = "AplicacionWeb.inicializar:" 

      Assert( AplicacionWeb.instancia == this, "Esto no puede saltar...")
      Assert( this.gl_act != null, `${nombref} gl act es null, debería haberlo creado el ctor?` )
      

      // crear el cauce
      this.cauce_actual = await CrearCauce( this.gl_act )

      // Añadir los objetos (y sus cámaras) 
      await this.crearObjetosCamaras()
      
      // crear los elementos de controles de la aplicación (elementos HTML)
      this.crearElementosControles()

      // definir funciones gestoras de eventos 
      this.activarFGEs()

      Log(`${nombref} this.gl_act == ${this.gl_act} ctor == ${this.gl_act.constructor.name}, va visualizar..`)
      
      
      // fijar longitud y latitud de la fuente de luz 0
      Assert( this.col_fuentes.length > 0, `${nombref} no hay fuentes de luz en la colección`)
      this.col_fuentes[0].long = this.long_luz
      this.col_fuentes[0].lat  = this.lat_luz
      Log(`${nombref} this.col_fuentes[0].long == ${this.col_fuentes[0].long}, this.col_fuentes[0].lat == ${this.col_fuentes[0].lat}`)

      // crear el cauce de sombras (si se va a usar)
      if ( this.evaluar_sombras )
      {
         this.cauce_sombras = new CauceSombras( this.gl_act, this.res_fbo_somb, this.res_fbo_somb )
         this.cauce_sombras.fijarDireccionVista( this.col_fuentes[0].dir_wcc )
      }


      // redimensionar el canvas y visualizar la 1a vez
      this.redimensionarVisualizar()

      this.estado = "Inicialización completa."
   }
   // ----------------------------------------------------------------------------------------
   
   /**
    * crea todos los objetos y sus camaras 
    * (inicializa vectores 'objetos' y 'camaras')
    */
   private async crearObjetosCamaras() 
   {
      const fname = "AplicacionWeb.crearObjetosCamaras"
      
      // esfera con una rotación
      this.objetos.push( new EsferaRotacion(  ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new Cubo24(  ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new MallaCuadradoXY( 32, 32 ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new MallaToroide( 32, 32 ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new MallaEsfera( 32, 32 ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new MallaCilindro( 32, 32 ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new MallaCono( 32, 32 ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new MallaColumna( 256, 256 ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new MallaCampoAlturas( 512, 512 ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new GrafoTest( await Textura.crear("/imgs/bazinga.jpg" ) ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new GrafoTest2( await Textura.crear("/imgs/uv-checker-1.png" ),
                                         await Textura.crear("/imgs/uv-checker-2.png" ),
                                         await Textura.crear("/imgs/uv-checker-1.png" )))
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new GrafoTest3SombrasTextura( await Textura.crear("/imgs/bazinga.jpg" ) ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new OC_GrafoTest( await Textura.crear("/imgs/bazinga.jpg" ) ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new OC_GrafoTest2( await Textura.crear("/imgs/uv-checker-1.png" ),
                                         await Textura.crear("/imgs/uv-checker-2.png" ),
                                         await Textura.crear("/imgs/uv-checker-1.png" )))
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( new CuadradoXYTextura( await Textura.crear("/imgs/bazinga.jpg" )))
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( await MallaPLY.crear( "/plys/beethoven.ply" ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( await MallaPLY.crear( "/plys/big_dodge.ply" ) )
      this.camaras.push( new CamaraOrbital3D() )

      this.objetos.push( await MallaPLY.crear( "/plys/ant.ply" ) )
      this.camaras.push( new CamaraOrbital3D() )

    


      // centrar y normalizar todos los objetos que sean un nodo terminal de tipo MallaPLY
      // (antes de que se visualizen por primera vez)
      for( let obj of this.objetos )
      {
         if ( obj instanceof MallaInd)
         {
            Log(`${fname} normalizo objeto: ${obj.nombre}`)
            obj.normalizarCentrar()
         }
      }

   }

   // -------------------------------------------------------------------------

   /**
    * Define los métodos gestores de eventos (MGE) de la aplicación
    */
   public activarFGEs()
   {
      this.canvas.onmousedown   = (me) => this.fgeRatonBotonPulsar(me)
      this.canvas.onmouseup     = (me) => this.fgeRatonBotonLevantar(me)
      this.canvas.oncontextmenu = (me) => this.fgeMenuContexto(me) //this.fgeRatonBotonPulsar(me) 
      this.canvas.onwheel       = (we) => this.fgeRatonRueda(we)  
      document.onkeyup          = (ke) => this.fgeTecladoLevantarBoton(ke)
      window.onresize           = (ev) => this.redimensionarVisualizar()
     
   }
   // -------------------------------------------------------------------------

   /**
    * Desconecta las funciones gestoras de eventos del canvas o la ventana
    * (se restauran las FGEs por defecto que estaban antes de ejecutar el constructor)
    * Se debe llamar si ocurre un error y 'this' queda inutilizable.
    */
   public desactivarFGEs()
   {
      this.canvas.onmousedown   = null 
      this.canvas.onmouseup     = null 
      this.canvas.oncontextmenu = null 
      this.canvas.onwheel       = null 
      document.onkeyup          = null 
      window.onresize           = null 
   }
   // ------------------------------------------------------------------------

   /**
    * Leer el cauce actual de la aplicación
    */
   public get cauce() : Cauce 
   {
      if ( this.cauce_actual == null )
         throw Error(`AplicacionWeb.cauce: se ha intentado leer el cauce actual, pero es nulo`)
      return this.cauce_actual 
   }
   // ------------------------------------------------------------------------

   /**
    * Leer el contexto actual de la aplicación
    */
   public get gl() : ContextoWebGL 
   {
      if ( this.gl_act == null )
         throw Error(`AplicacionWeb.gl: se ha intentado leer el contexto actual, pero es nulo`)
      return this.gl_act 
   }
   // ------------------------------------------------------------------------- 
   
   /**
    * Lee si la ilumniación está activada o no lo está
    */
   public get iluminacion_activada() : boolean 
   { 
      return this.iluminacion 
   }
   // ------------------------------------------------------------------------- 
   /**
    * Crea el check box para visualizar aristas si/no 'this.input_boton_aristas'
    */
   private crearCheckboxAristas()
   {
      const nombref : string = 'AplicacionWeb.crearCheckboxAristas'

      this.input_boton_aristas = CrearInputCheckbox( this.controles, this.visualizar_aristas,
                                       'id_boton_aristas', 'Visu.&nbsp;aristas' )
      this.input_boton_aristas.onclick = () => this.fijarVisualizarAristas( ! this.visualizar_aristas )
   }
   // ------------------------------------------------------------------------- 
   /**
    * Crea el check box para visualizar ejes si/no 'this.input_boton_ejes'
    */
   private crearCheckboxEjes()
   {
      const nombref : string = 'AplicacionWeb.crearCheckboxEjes'

      this.input_boton_ejes = CrearInputCheckbox( this.controles, this.visualizar_ejes,
                                       'id_boton_ejes', 'Visu.&nbsp;ejes' )
      this.input_boton_ejes.onclick = () => this.fijarVisualizarEjes( ! this.visualizar_ejes )
   }
   // ------------------------------------------------------------------------- 

   /**
    * Crea el check box para visualizar aristas si/no 'this.input_boton_normales'
    */
   private crearCheckboxNormales()
   {
      const nombref : string = 'AplicacionWeb.crearCheckboxNormales'

      this.input_boton_normales = CrearInputCheckbox( this.controles, this.visualizar_normales,
                                       'id_boton_normales', 'Visu.&nbsp;normales' )
      this.input_boton_normales.onclick = () => this.fijarVisualizarNormales( ! this.visualizar_normales )
   }
   // ------------------------------------------------------------------------- 

   /**
    * Crea el check box para visualizar aristas si/no 'this.input_boton_normales'
    */
   private crearCheckboxIluminacion()
   {
      const nombref : string = 'AplicacionWeb.crearCheckboxIluminacion'

      this.input_boton_iluminacion = CrearInputCheckbox( this.controles, this.iluminacion,
                                       'id_boton_iluminacion', 'Iluminación' )
      this.input_boton_iluminacion.onclick = () => this.fijarIluminacion( ! this.iluminacion )
   }
   // ------------------------------------------------------------------------- 

   /**
    * Crea el check box para visualizar aristas si/no 'this.input_boton_normales'
    */
   private crearCheckboxSombrasArrojadas()
   {
      const nombref : string = 'AplicacionWeb.crearCheckboxSombrasArrojadas'

      this.input_boton_sombras_arrojadas = CrearInputCheckbox( this.controles, this.evaluar_sombras,
                                       'id_boton_sombras_arrojadas', 'Sombras arrojadas' )
      this.input_boton_sombras_arrojadas.onclick = () => this.fijarSombrasArrojadas( ! this.evaluar_sombras )
   }
   // ------------------------------------------------------------------------- 

   /**
    * Crear un selector para el objeto actual (asigna a 'this.selector_objeto_actual')
    */
   private crearSelectorObjetoActual() : void
   {
      const nombref : string = 'AplicacionWeb.crearSelectorObjetoActual'
      Assert( 0 < this.objetos.length, `${nombref} : no hay objetos en la lista de objetos`)

      let nombres : string[] = []
      for( const objeto of this.objetos )
      {
         Log(`${nombref} añadiendo objeto: ${objeto.nombre}`)
         nombres.push( objeto.nombre )
      }

      this.selector_objeto_actual = CrearSelector( this.controles, this.indice_objeto_actual,
                                                        'id_selector_objeto_actual', 'Objeto&nbsp;act.', nombres )

      this.selector_objeto_actual.onchange = (e) => this.fijarObjetoActual( parseInt((e.target as HTMLSelectElement).value) )
   }
   // -------------------------------------------------------------------------


   /**
    * Crea un input de color para el color inicial por defecto para objetos que no tengan color propio
    * (asigna a 'this.input_color_inicial)
    */
   private crearInputColorDefecto() : void  
   {
      this.input_color_defecto = CrearInputColor( this.controles, new Vec3([ 1.0, 0.0, 0.0 ]), "id_test_color", "Color&nbsp;defecto" )
      this.input_color_defecto.value = this.color_defecto.hexColorStr() 
      this.input_color_defecto.oninput = (e) => this.fijarColorDefecto( Vec3DesdeColorHex( this.input_color_defecto!.value ))
   }
   
   // -------------------------------------------------------------------------
   /**
    * Fija el valor del parámetro S en el cauce actual
    * @param nuevo_param_s (string) nuevo valor del parámetro S
    */
   private fijarParamS( nuevo_param_s : String ) : void
   {
      const nombref : string = 'AplicacionWeb.fijarParamS:'
      this.param_S = parseFloat( this.input_param_S!.value )
      let msg = `Nuevo valor del parámetro S == ${this.param_S}`
      this.estado = msg

      // cambia el valor del parámetro S en el objeto actual:
      this.objetos[this.indice_objeto_actual].param_S = this.param_S

      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   // -------------------------------------------------------------------------
   /**
    * Crea el botón de animación
    */
   private crearBotonesAnimacion()
   {
      this.input_boton_estado_anim = CrearInputBoton( this.controles, "Comenzar", "En inicio", "id_boton_estado_anim", "Estado animación")

      this.input_boton_estado_anim.addEventListener( 'click', () => { 
         AplicacionWeb.instancia.fgeClickBotonEstadoAnim()
      })

      this.input_boton_reset_anim = CrearInputBoton( this.controles, "Reiniciar", "", "id_boton_reset_anim", "Reiniciar animación")

      this.input_boton_reset_anim.addEventListener( 'click', () => { 
         AplicacionWeb.instancia.fgeClickBotonResetAnim()
      })
   }
   // -------------------------------------------------------------------------

   /**
    * Actualiza el estado del botón de animación en función del estado del objeto actual.
    * @param obj 
    */
   private actualizarBotonEstadoAnim( obj : ObjetoVisualizable )
   {
      let be  = this.input_boton_estado_anim 
      let te  = document.getElementById( "id_boton_estado_anim_span_estado" ) as HTMLInputElement

      if ( be == null )
         throw new Error("el boton de estado de la animación es nulo")

      if ( te == null )
         throw new Error("el span de texto de estado del boton de estado de la animación es nulo")

      if ( obj instanceof ObjetoAnimado) 
      {
         let obj_anim  = obj as ObjetoAnimado 
         if ( obj_anim.estado == EstadoAnim.animado )
         {
            be.value         = "Pausar"
            te.innerHTML     = "Animada"
         }
         else if ( obj_anim.estado == EstadoAnim.pausado )
         {
            be.value         = "Reanudar"
            te.innerHTML     = "Pausada"
         }
         else if ( obj_anim.estado == EstadoAnim.parado )
         {
            be.value         = "Comenzar"
            te.innerHTML     = "En inicio"
         }
      }
      else 
      {
         te.innerHTML = "No animable"
         be.value     = "No animable"
      }
      
   }

   /**
    * Método que se ejecuta al hacer click en el botón del estado de la animación
    */
   private fgeClickBotonEstadoAnim()
   {
      const ahora_ms = performance.now() 
      const nombref = 'AplicacionWeb.clickBotonEstadoAnim'
      Log(`${nombref} pulsado`)

      let obj = this.objetos[this.indice_objeto_actual]

      if ( obj instanceof ObjetoAnimado )
      {
         let obj_anim  = obj as ObjetoAnimado 
         const ahora_s = ahora_ms/1000   

         if ( obj_anim.estado == EstadoAnim.parado )
         {
            this.estado = "Animación iniciada"
            obj_anim.comenzar( ahora_s )
         }
         else if ( obj_anim.estado == EstadoAnim.animado )
         {
            this.estado = "Animación pausada"
            obj_anim.pausar( ahora_s )
         }
         else if ( obj_anim.estado == EstadoAnim.pausado )
         {
            this.estado = "Animación reanudada"
            obj_anim.reanudar( ahora_s )
         }

         this.actualizarBotonEstadoAnim( obj_anim )
         window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
      }
      else
      {
         this.estado = "No se puede hacer la acción: el objeto no es animado."
      }
   }
   /**
    * Método que se ejecuta al hacer click en el botón de reset de la animación
    */
   private fgeClickBotonResetAnim()
   {
      const ahora_ms = performance.now() 
      const nombref = 'AplicacionWeb.clickBotonResetAnim'
      Log(`${nombref} pulsado`)

      let obj = this.objetos[this.indice_objeto_actual]

      if ( obj instanceof ObjetoAnimado )
      {
         let be  = this.input_boton_estado_anim 
         let te  = document.getElementById( "id_boton_estado_anim_span_estado" ) as HTMLInputElement
         let obj_anim  = obj as ObjetoAnimado 
         const ahora_s = ahora_ms/1000   

         if ( be == null )
            throw new Error("el boton de estado de la animación es nulo")

         if ( te == null )
            throw new Error("el span de texto de estado del boton de estado de la animación es nulo")

         obj_anim.reiniciar( ahora_s )
         
         be.value     = "Comenzar"
         te.innerHTML = "En inicio"
         this.estado  = "Animación parada y puesta en estado inicial"

         window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
      }
      else
      {
         this.estado = "No se puede hacer reset. El objeto no es animado."
      }
   }
   // -------------------------------------------------------------------------

   /**
    * Crea un input tipo 'range slider' para el parámetro S de los shaders
    * (asigna a ??)
    */
   private crearSliderParamS() : void  
   {
      this.input_param_S = CrearInputSlider( this.controles, this.param_S, 0.0, 1.0, 0.01, "id_slider_param_s", "Parámetro&nbsp;S" )
     
      this.input_param_S.oninput = (e) => this.fijarParamS( this.input_param_S!.value ) 
   }

   /**
    * Crea dos inputs de tipo slider para la longitud/latitud de la 1a fuente de luz (direccional)
    */
   private crearSlidersDirLuz() : void  
   {
      this.slider_long_luz = CrearInputSlider( this.controles, this.long_luz, 0.0, 360.0, 1.0, "id_slider_long_luz", "Longitud luz" )
      this.slider_lat_luz  = CrearInputSlider( this.controles, this.lat_luz, 0.0, 90.0, 1.0, "id_slider_lat_luz", "Latitud luz" )

      this.slider_long_luz.oninput = (e) => this.fijarLongitudLuz( parseFloat( this.slider_long_luz!.value ) )
      this.slider_lat_luz.oninput  = (e) => this.fijarLatitudLuz( parseFloat( this.slider_lat_luz!.value ) )
     
      //this.slider_long_luz.oninput = (e) => this.fijarParamS( this.input_param_S!.value ) 
   }
   // -------------------------------------------------------------------------

   /**
    * Fija la longitud de la fuente de luz (direccional)
    * @param long_luz (number) nueva longitud de la fuente de luz
    */
   private fijarLongitudLuz( long_luz : number ) : void
   {
      const nombref : string = 'AplicacionWeb.fijarLongitudLuz:'
      Assert( this.col_fuentes.length > 0, `${nombref} no hay fuentes de luz en la colección`)
      
      this.long_luz = long_luz
      
      let msg = `Nueva longitud de la fuente de luz == ${this.long_luz}`
      this.estado = msg

      // cambia el valor de la longitud de la fuente de luz en la colección de fuentes de luz:

      this.col_fuentes[0].long = this.long_luz

      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }

   // -------------------------------------------------------------------------
   /**
    * Fija la latitud de la fuente de luz (direccional)
    * @param lat_luz (number) nueva latitud de la fuente de luz
    */
   private fijarLatitudLuz( lat_luz : number ) : void
   {
      const nombref : string = 'AplicacionWeb.fijarLatitudLuz:'
      Assert( this.col_fuentes.length > 0, `${nombref} no hay fuentes de luz en la colección`)

      this.lat_luz = lat_luz
      let msg = `Nueva latitud de la fuente de luz == ${this.lat_luz}`
      this.estado = msg

      // cambia el valor de la latitud de la fuente de luz en la colección de fuentes de luz:
      this.col_fuentes[0].lat = this.lat_luz

      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   //--------------------------------------------------------------------------- 
   /**
    * Crea diversos controles
    */
   private crearElementosControles()
   {
      const nombref : string = 'AplicacionWeb.crearElementosControles:'

      this.crearCheckboxEjes()
      this.crearCheckboxAristas()
      this.crearCheckboxNormales()
      this.crearCheckboxIluminacion()
      this.crearCheckboxSombrasArrojadas()
      this.crearSelectorObjetoActual()
      this.crearInputColorDefecto()
      this.crearSlidersDirLuz()
      this.crearSliderParamS()
      this.crearBotonesAnimacion()

      Log(`${nombref} controles creados ok.`)
   }
   // ------------------------------------------------------------------------- 

   /**
    * Ignora un evento de tipo "menu-contexto" y no lo procesa
    * (permite gestionar la camara con el botón derecho, sin que aparezca un menú al 
    * hacer click con el botón derecho)
    * @param me (MouseEvent) evento a ignorar
    * @returns (Boolean) false
    */
   ignorarMenuContexto( me : MouseEvent )
   {
      me.preventDefault()
      return false 
   }
   // -------------------------------------------------------------------------
    
   /**
    * Pone el ancho y alto del "buffer" (pixels del framebuffer) al mismo 
    * tamaño que el "client rect" (tamaño en la ventana del navegador)
    * Después redibuja el frame.
    * 
    */ 
   public redimensionarVisualizar() : void 
   {
      const nombref : string = "AplicacionWeb.redimensionarVisualizar:"

      //Log(`${nombref} this.gl_act == ${this.gl_act}`)

      if ( this.canvas == null )
      {
         Log(`${nombref} no hay canvas, salgo`)
         return
      }

      // set the size of the drawingBuffer based on the size it's displayed.
      this.canvas.width  = this.canvas.clientWidth
      this.canvas.height = this.canvas.clientHeight

      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   // -------------------------------------------------------------------------

   /**
    * Recupera el elemento contenedor, asigna 'this.contenedor'
    * @param id_contenedor (string) identificador del elemento contenedor en la página
    */
   obtenerElementoContenedor( id_contenedor : string ) : HTMLDivElement
   {
      const nombref : string = "AplicacionWeb.obtenerElementoContenedor:" 

      let contenedor : HTMLElement | null = document.getElementById( id_contenedor )
      if ( contenedor == null )
         throw Error(`${nombref} no puedo inicializar la aplicación PCG, no encuentro el contenedor con id "${id_contenedor}" en la página`)
      if ( !( contenedor instanceof HTMLDivElement))
         throw Error(`${nombref} no puedo inicializar la aplicación PCG, el contenedor no es un elemento 'div'`)
      Log(`${nombref} contenedor '${id_contenedor}' encontrado`)
      return contenedor 
   }
   // -------------------------------------------------------------------------

   /**
    * Recupera el elemento (tipicamente 'div') donde se añaden los controles de la aplicación
    * 
    * @param id_controles (string) identificador del elemento para los controles
    * @returns (HTMLDivElement) elemento 'div' donde poner los controles.
    */
   obtenerElementoControles( id_controles : string  ) : HTMLDivElement
   {
      const nombref : string = "AplicacionWeb.obtenerElementoContenedor:" 

      let controles : HTMLElement | null = document.getElementById( id_controles )
      if ( controles == null )
         throw Error(`${nombref} no puedo inicializar la aplicación PCG, no encuentro el 'div' para controles con id ${id_controles}`)
      if ( !( controles instanceof HTMLDivElement))
         throw Error(`${nombref} no puedo inicializar la aplicación PCG, el elemento para controles no es un elemento 'div'`)
      Log(`${nombref} div de controles '${id_controles}' encontrado`)
      return controles 
   }
   // -------------------------------------------------------------------------
   
   /**
    *  Recupera o crea el canvas como hijo del contenedor
    *  Si el contenedor tiene un elemento canvas, lo recupera 
    *  Si el contenedor no tiene un elemento canvas, lo crea (como hijo directo)
    *  Si el contenedor tiene más de un elemento canvas, produce un error
    *  @param contenedor (HTMLDivElement) elemento contenedor donde se busca o crea el canvas 
    */
   obtenerCrearElementoCanvas( contenedor : HTMLDivElement ) : HTMLCanvasElement 
   {
      const nombref : string = "AplicacionWeb.obtenerCrearElementoCanvas:" 

      let canvas : HTMLCanvasElement | null = null 
      let lista  : NodeListOf<HTMLCanvasElement> = contenedor.querySelectorAll("canvas")
      
      if ( lista.length == 0 )
      {  
         canvas = document.createElement( "canvas")
         if ( canvas == null )
            throw Error(`${nombref} error al crear el canvas, es 'null'`)
         contenedor.appendChild( canvas )
         Log(`${nombref} elemento canvas creado y añadido al contenedor ok.`)
      }
      else if ( lista.length == 1 )
      {  
         canvas = lista[0]
         Log(`${nombref} elemento canvas encontrado ok.`)
      }
      else
         throw Error(`${nombref} se ha encontrado más de un elemento canvas en el contenedor`)
      
      return canvas
   }
   // -------------------------------------------------------------------------
   
   /**
    * Recupera el contexto WebGL 
    * ('this.canvas' debe estar creado ya)
    */
   obtenerContextoWebGL( canvas : HTMLCanvasElement ) : WebGL2RenderingContext | WebGLRenderingContext
   {
      const nombref : string = "AplicacionWeb.obtenerContextoWebGL:" 

      let gl : RenderingContext | null = this.canvas.getContext( "webgl2" )
      if ( gl == null )
         gl = this.canvas.getContext( "webgl" )
      if ( gl == null )
         throw Error(`${nombref} no se puede obtener un contexto del canvas, se obtiene 'null'` )
      
      if ( gl instanceof WebGL2RenderingContext )
         Log(`${nombref} contexto de rendering de WebGL 2 recuperado ok`)
      else if ( gl instanceof WebGLRenderingContext )
         Log(`${nombref} contexto de rendering de WebGL 1 recuperado ok`)
      else 
         throw Error(`${nombref} no se puede obtener un contexto WebGL 1 ni WebGL 2 del canvas` )

      return gl
   }
    
   // -------------------------------------------------------------------

   /**
    * Si el objeto actual está animado, actualizar el estado
    * @param ahora_ms tiempo de actualización
    */
   actualizarEstadoObjeto( ahora_ms : number )
   {
      let obj = this.objetos[this.indice_objeto_actual]
      if ( obj instanceof ObjetoAnimado)
      {
         let obj_anim = obj as ObjetoAnimado
         if ( obj_anim.estado == EstadoAnim.animado )
         {
            const ahora_s = ahora_ms / 1000
            obj_anim.actualizar( ahora_s )
         }
      }
   }
   
   // --------------------------------------------------------------------------
   
   /**
    * Visualizar un frame, por ahora es un simple test
    */
   visualizarFrame() : void 
   {
      

      const t_inicio_ms = performance.now()
      const nombref : string = 'AplicacionWeb.visualizarFrame:' 

      // recuperar contexto y cauce
      let gl    = this.gl_act 
      let cauce = this.cauce_actual 

      //Log(`${nombref} inicio, this.ctor == ${this.constructor.name}`)
      ComprErrorGL( gl, `${nombref} al inicio`)
      
      // si el objeto actual es animable, actualiza el estado del objeto
      this.actualizarEstadoObjeto( t_inicio_ms )

      // comprobar algunas precondiciones
      Assert( this.camaras.length == this.objetos.length, `${nombref} el array de cámaras debe tener el mismo tamaño que el de objetos` )

      // recuperar el objeto actual y su cámara
      let objeto = this.objetos[this.indice_objeto_actual]
      let camara = this.camaras[this.indice_objeto_actual]

      // indica si se debe evaluar la iluminación o no 
      const eval_iluminacion : Boolean = this.iluminacion && !( camara instanceof CamaraVista2D )

      // indica si se debe evaluar las sombras o no
      const eval_sombras : Boolean = eval_iluminacion && this.evaluar_sombras

      //Log(`#### --> eval_sombras == ${eval_sombras}`)

      // si están activadas las sombras, visualizar el objeto sobre el FBO de sombras
      if ( eval_sombras )
      {
         if ( this.cauce_sombras == null ) 
            throw new Error(`{fname} debería haber un cauce de sombras`)
         let v = this.col_fuentes[0].dir_wcc.clonar()
         this.cauce_sombras.activar()
         this.cauce_sombras.fijarDimensionesFBO( this.res_fbo_somb, this.res_fbo_somb )
         this.cauce_sombras.fijarDireccionVista( v )
         this.cauce_sombras.visualizarGeometriaObjeto( objeto )
         //Log(`${nombref} frame: ${this.cuenta_frames} visualizado objeto sobre el FBO de sombras, dir == ${v}`)
      }

      // reactivar el framebuffer por defecto y el cauce
      gl.bindFramebuffer( gl.FRAMEBUFFER, null )
      cauce.activar()

      // leer tamaño actual del viewport
      let ancho = gl.drawingBufferWidth 
      let alto  = gl.drawingBufferHeight

      // asignar el 'color buffer' de sombras al sampler 0 del cauce (como si fuese una textura 2D)
      // esto provoca que se espere hasta la terminación de las ordenes visualización del objeto 
      // sobre el FBO de sombras, de forma que el FBO se puede usar a continuación para visualizar el frame.
      if ( eval_sombras )
      {
         Assert( this.cauce_sombras != null, `${nombref} debería haber un cauce de sombras`)
         cauce.fijarEvalText( true, this.cauce_sombras.fbo.cbuffer )
         cauce.fijarEvalText( false, null ) // después se desactiva.
      }

      // fijar el valor del parámetro S  (antes que cualquier otra cosa)
      cauce.fijarParamS( this.param_S )

      gl.enable( gl.DEPTH_TEST )
      gl.disable( gl.CULL_FACE )
      gl.viewport( 0, 0, ancho, alto )

      gl.clearColor( 0.0, 0.0, 0.0, 1.0 )
      gl.clear( this.gl_act.DEPTH_BUFFER_BIT | this.gl_act.COLOR_BUFFER_BIT )
      
      // activar la cámara, configurando antes su viewport
      camara.fijarViewport( new Viewport( ancho, alto ))
      camara.activar( this.cauce_actual )  // incluye cauce.resetMM

      // fijar el color para todo lo que se dibuje después que no tenga color 
      cauce.fijarColor( this.color_defecto ) 

      // inicialmente, desactivar texturas y poner la textura actual a null
      cauce.fijarTextura( null )

      // si 'iluminacion' == 'true', (y la cámara no es 2D) activar la colección de fuentes y el material por defecto
      // en otro caso, desactivar iluminación.
      if ( eval_iluminacion )
      {
         cauce.fijarEvalMIL( true )
         cauce.fijarMaterial( this.material_defecto )
         this.col_fuentes.activar()
      }
      else 
         cauce.fijarEvalMIL( false )

      // Si se activan sombras, fijar la textura del FBO de sombras en la unidad 1
      // (en otro caso, desactivar las sombras en el cauce
      if ( eval_sombras )
      {
         if ( this.cauce_sombras == null ) 
            throw new Error(`{fname} debería haber un cauce de sombras`)
         cauce.fijarSombras( true, this.cauce_sombras.fbo,  this.cauce_sombras.mat_vista_proy )
      }
      else 
         cauce.fijarSombras( false, null, null )


      // dibujar el objeto actual 
      Assert( 0 <= this.indice_objeto_actual && this.indice_objeto_actual < this.objetos.length , `${nombref} indice de objeto actual fuera de rango `)
      this.objetos[ this.indice_objeto_actual ].visualizar(  )

      // desactivar las fuentes de luz (a partir de aquí se dibuja sin iluminación)
      cauce.fijarEvalMIL( false )

      // dibujar los ejes
      if ( this.visualizar_ejes )
         this.ejes.visualizar()
  
      
      // dibujar las aristas del objeto actual con color negro, si procede
      if ( this.visualizar_aristas )
      {
         cauce.pushColor()
            cauce.fijarColor( new Vec3([ 1.0, 1.0, 1.0 ]))
            this.objetos[ this.indice_objeto_actual ].visualizarAristas(  )
         cauce.popColor()
      }

      // dibujar las normales en color naranja, si procede
      if ( this.visualizar_normales ) 
      {
         cauce.pushColor()
            cauce.fijarColor( new Vec3([ 1.0, 0.6, 0.1 ]))
            this.objetos[ this.indice_objeto_actual ].visualizarNormales(  )
         cauce.popColor()
      }

      // TEST: visualizar el objeto sobre el fbo de sombras y luego visualizar elfr

      const mostrar_fbo_sombras : Boolean = true  

      if ( eval_sombras && mostrar_fbo_sombras )
      {
         Assert( this.cauce_sombras != null, `${nombref} debería haber un cauce de sombras`)
         //Log(`######### ${nombref} ${this.cuenta_frames} inicio visualización FBO de sombras`)
         this.cauce_sombras.fbo.visualizarEn( cauce, ancho, alto )
         //Log(`######### ${nombref}  ${this.cuenta_frames} fin visualización FBO de sombras`)
         Log(``)
      }

      ComprErrorGL( gl, `${nombref} al final`)

      // incrementar la cuenta de frames
      this.cuenta_frames += 1

      // medir y logear tiempo de render de este frame
      const t_fin_ms      = performance.now()
      const t_visu_ms     = t_fin_ms - t_inicio_ms   
      
      // Si el objeto actual está animado, solicitar que en el futuro se vuelva a visualizar un frame
      if ( objeto instanceof ObjetoAnimado )
      {
         let objeto_anim = objeto as ObjetoAnimado 
         if ( objeto_anim.estado == EstadoAnim.animado )
         {
            const T_objetivo_ms = 16 ;
            const t_restante_ms = Math.max( 0.0, T_objetivo_ms - t_visu_ms  )
            setTimeout( VisualizarFrameAplicacionWeb, t_restante_ms )
         }
      }
      
   }
   // ---------------------------------
   /**
    * Método que se ejecuta cuando se produce un evento de tipo 'contextmenu', no hace nada,
    * pero evita que aparezca el 'context menu' que por defecto aparece pulsando
    * con el botón derecho del ratón  ('preventdefault()')
    * 
    * @param me (MouseEvent)
    */
   fgeMenuContexto( me : MouseEvent ) : Boolean
   {
      const nombref = 'AplicacionWeb.fgeMenuContexto'
      me.stopImmediatePropagation()
      me.preventDefault()

      return false ;
   }
   // ---------------------------------

   /**
    * Método que se ejecuta cuando se pulsa (se baja) un botón del ratón 
    * (también se ejecuta cuando se produce un evento de tipo 'contextmenu' de forma 
    * que se puede evitar que aparezca el 'context menu' que por defecto aparece pulsando
    * con el botón derecho del ratón)
    * 
    * @param e (MouseEvent)
    */
   fgeRatonBotonPulsar( e : MouseEvent ) : Boolean
   {
      const nombref = 'AplicacionWeb.mgeRatonBotonPulsar'
      e.stopImmediatePropagation()
      e.preventDefault()

      if ( e.button == 2 ) // 2 --> botón derecho
      {
         this.contenedor.style.cursor = 'move'
         this.estado_raton.boton_der_abajo = true
         this.estado_raton.inicio_x = e.offsetX
         this.estado_raton.inicio_y = e.offsetY 
         this.canvas.onmousemove = me => this.fgeRatonArrastre(me)
         //Log(`${nombref} bajada. pos x,y == ${this.estado_raton.inicio_x}, ${this.estado_raton.inicio_y}`)
      }
      return false 
   }
   // ------------------------------------------------------------------------------------

   /**
    * Activa el siguiente objeto de la lista de objetos de 'this', visualiza el frame
    */
   siguienteObjeto() : void 
   {
      const nombref = 'AplicacionWeb.siguienteObjeto'
      this.fijarObjetoActual( ( this.indice_objeto_actual +1 ) % this.objetos.length )
   }
   // -----------------------------------------------------------------------------------

   /**
    * Activa un nuevo objeto, dando su índice. Visualiza el frame.
    * 
    * @param indice_obj indice del nuevo objeto activo (entre 0 y número de objetos -1)
    */
   fijarObjetoActual( indice_obj : number ) : void
   {
      const nombref = 'AplicacionWeb.fijarObjetoActivo:'
      Assert( 0 <= indice_obj && indice_obj < this.objetos.length, `${nombref} índice (${indice_obj}) fuera de rango (0 - ${this.objetos.length-1})` )

      this.indice_objeto_actual = indice_obj
      let objeto = this.objetos[this.indice_objeto_actual]

      Assert( objeto != null, `${nombref} el objeto actual es nulo`)

      
      this.estado = `Visualizando objeto: ${objeto.nombre}`
      Log( `${nombref} visualizando objeto ${this.indice_objeto_actual}: ${objeto.nombre}` )
      if ( this.selector_objeto_actual != null )
         this.selector_objeto_actual.value = `${this.indice_objeto_actual}`

      objeto.param_S = this.param_S 
      this.actualizarBotonEstadoAnim( objeto )

      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   // ------------------------------------------------------------------------------------


   /**
    * Fija el color inicial por defecto para los objetos
    * 
    * @param nuevo_color_inicial (Vec3) nuevo color inicial (componentes entre 0 y 1)
    */
   fijarColorDefecto( nuevo_color_inicial : Vec3 ) : void
   {
      const nombref = 'AplicacionWeb.fijarColorDefecto:'
      
      this.color_defecto = nuevo_color_inicial
      const msg : string = `Color por defecto fijado a ${this.color_defecto.toStringPercent()}`
      this.estado = msg
      //Log( `${nombref} ${msg}` )
      if ( this.input_color_defecto != null )
         this.input_color_defecto.value = this.color_defecto.hexColorStr() 
      
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   // ------------------------------------------------------------------------------------
   
   /**
    * Activa o desactiva la visualización de los ejes
    * 
    * @param nuevo_visualizar_ejes (Boolean) true para activar, false para desactivar 
    */
   fijarVisualizarEjes( nuevo_visualizar_ejes : boolean  ) : void 
   {
      const nombref = 'AplicacionWeb.fijarVisualizarEjes'
      this.visualizar_ejes = nuevo_visualizar_ejes 

      if ( this.input_boton_ejes != null )
         this.input_boton_ejes.checked = this.visualizar_ejes
      const msg : string = `Visualizar ejes: ${this.visualizar_ejes ? "activado" : "desactivado"}`
      this.estado = msg 
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   // ------------------------------------------------------------------------------------
   
   /**
    * Activa o desactiva la visualización de aristas
    * 
    * @param nuevo_visualizar_aristas (Boolean) true para activar, false para desactivar 
    */
   fijarVisualizarAristas( nuevo_visualizar_aristas : boolean  ) : void 
   {
      const nombref = 'AplicacionWeb.fijarVisualizarAristas'
      this.visualizar_aristas = nuevo_visualizar_aristas 

      if ( this.input_boton_aristas != null )
         this.input_boton_aristas.checked = this.visualizar_aristas 
      const msg : string = `Visualizar aristas: ${this.visualizar_aristas ? "activado" : "desactivado"}`
      this.estado = msg 
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
    // ------------------------------------------------------------------------------------
   
   /**
    * Activa o desactiva la visualización de normales
    * 
    * @param nuevo_visualizar_normales (Boolean) true para activar, false para desactivar 
    */
   fijarVisualizarNormales( nuevo_visualizar_normales : boolean  ) : void 
   {
      const nombref = 'AplicacionWeb.fijarVisualizarNormales'
      this.visualizar_normales = nuevo_visualizar_normales 

      if ( this.input_boton_normales != null )
         this.input_boton_normales.checked = this.visualizar_normales 
      const msg : string = `Visualizar normales: ${this.visualizar_normales ? "activado" : "desactivado"}`
      this.estado = msg 
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }

    // ------------------------------------------------------------------------------------
   
   /**
    * Activa o desactiva la iluminacion
    * 
    * @param nuevo_iluminacion (boolean) true para activar, false para desactivar 
    */
   fijarIluminacion( nuevo_iluminacion : boolean  ) : void 
   {
      const nombref = 'AplicacionWeb.fijarIluminacion'
      this.iluminacion = nuevo_iluminacion

      if ( this.input_boton_iluminacion != null )
         this.input_boton_iluminacion.checked = this.iluminacion 
      const msg : string = `Iluminación: ${this.iluminacion ? "activada" : "desactivada"}`
      this.estado = msg 
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   // ------------------------------------------------------------------------------------
   
   /**
    * Activa o desactiva las sombras arrojadas
    * 
    * @param nuevo_sombras_arrojadas (boolean) true para activar, false para desactivar 
    */
   fijarSombrasArrojadas( nuevo_sombras_arrojadas : boolean  ) : void 
   {
      const nombref = 'AplicacionWeb.fijarSombrasArrojadas'
      this.evaluar_sombras = nuevo_sombras_arrojadas

      if ( this.input_boton_sombras_arrojadas != null )
         this.input_boton_sombras_arrojadas.checked = this.evaluar_sombras
      const msg : string = `Sombras arrojadas: ${this.evaluar_sombras ? "activada" : "desactivada"}`
      this.estado = msg 
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
   }
   // ------------------------------------------------------------------------------------

   /**
    * Función que se ejecuta al levantar un botón del teclado 
    * 
    * @param e (KeyboardEvent)
    * @returns (Boolean) siempre 'false'
    */
   fgeTecladoLevantarBoton(  e : KeyboardEvent ) : boolean
   {
      const nombref = 'AplicacionWeb.fgeLevantarBoton'
      e.stopImmediatePropagation()
      e.preventDefault()
      
      switch ( e.code )
      {
         case 'KeyO' : 
            this.siguienteObjeto()  
            break
         case 'KeyW' : 
            this.fijarVisualizarAristas( ! this.visualizar_aristas ) 
            break
         case 'KeyN' : 
            this.fijarVisualizarNormales( ! this.visualizar_normales ) 
            break
         case 'KeyE' :  // producir un error al pulsar E (debug) 
            //Log(`SE HA PULSADO 'E'`)
            //Assert( 0.0 < 1.0 && 1.0 < 0.0 )
            //Assert( 0.0 < 1.0 && 1.0 < 0.0, `esto es un assert imposible de cumplir, 1.0 < 0.0` ) // salta siempre
            //let p = await LeerArchivoTexto("pepe.txt")  //--> para probar esto hay que hacer 'async' esta FGE
            break 
      }
      return false 
   }
   // ------------------------------------------------------------------------------------

   /**
    * Función que se ejecuta cuando se mueve el ratón con el botón derecho pulsado
    * 
    * @param e (MouseEvent) evento 
    * @returns (Boolean) siempre 'false'
    */
   fgeRatonArrastre( e : MouseEvent ) : Boolean 
   {
      const nombref = 'AplicacionWeb.fgeRatonArrastre'
      e.stopImmediatePropagation()
      e.preventDefault()

      //Log(`${nombref} movement x,y == ${e.movementX} ${e.movementY}`)

      const dh : number = -0.3*e.movementX
      const dv : number =  0.3*e.movementY 

      let camara = this.camaras[this.indice_objeto_actual]

      camara.mover( dh, dv )
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )

      return false
   }
   // ------------------------------------------------------------------------------------

   /**
    * Función que se ejecuta cuando se levantar un botón del raton 
    * @param e 
    * @returns (Boolean) siempre 'false'
    */
   fgeRatonBotonLevantar( e : MouseEvent ) : Boolean 
   {
      const nombref = 'AplicacionWeb.botonRatonLevantar'
      e.stopImmediatePropagation()
      e.preventDefault()
      
      if ( e.button == 2 ) // 2 --> botón derecho 
      {
         this.contenedor.style.cursor = 'auto'
         this.estado_raton.boton_der_abajo = false
         this.canvas.onmousemove = null
         //Log(`${nombref} subida. pos x,y == ${e.clientX}, ${e.clientY}`)
      }
      return false
   }
   // ------------------------------------------------------------------------------------

   /**
    * Función que se ejecuta cuando se mueve la rueda del ratón 
    * @param e 
    * @returns (Boolean) siempre 'false'
    */
   fgeRatonRueda( e : WheelEvent ) : Boolean 
   {
      const nombref = 'AplicacionWeb.botonRatonArriba'
      e.stopImmediatePropagation()
      e.preventDefault()
      
      //Log(`${nombref} rueda movida, deltaY == ${e.deltaY}, delta mode == ${e.deltaMode}`)
      const signo : number = e.deltaY >= 0.0 ? +1.0 : -1.0

      let camara = this.camaras[this.indice_objeto_actual]

      camara.zoom( signo )
      window.requestAnimationFrame( VisualizarFrameAplicacionWeb )
      return false
   }
   // --------------------------------------------------------------------------------

   /**
    * Inicializa la instancia 'apl' de la aplicación
    * @param id_contenedor : nombre del elemento html contenedor del canvas 
    */
   public static async crear(  ) : Promise<void> 
   {
      const nombref = "AplicacionWeb.crear:" // getFuncName()

      // fijar el gestor de errores, debe 
      window.onerror = (err) => AplicacionWeb.gestionarError( err, "window.onerror" )
      window.onunhandledrejection = (err) => AplicacionWeb.gestionarError( err, "window.onunhandledrejection" ) 

      console.log( `${nombref} inicio.`)
      
      document.body.style.cursor = "wait"
      
      var pie : HTMLElement | null = document.getElementById("pie")
      if ( pie != null )
         pie.textContent = "Inicializando ..." 
      
      
      let instancia_apl : AplicacionWeb | null = null 

      try 
      {  // crear e inicializar la instancia única 
         instancia_apl = new AplicacionWeb(  )
      }
      catch( err : any )
      {  AplicacionWeb.gestionarError( err, "AplicacionWeb.constructor" )
      }

      if ( instancia_apl == null )
         AplicacionWeb.gestionarError( null, "AplicacionWeb.constructor - devuelve null" )
      else 
      {
         try 
         {  // inicializar la instancia única 
            await instancia_apl.inicializar()
         }
         catch( err : any )
         {  AplicacionWeb.gestionarError( err, "AplicacionWeb.inicializar" )
         }
      }

      if ( AplicacionWeb.instancia_o_null == null )
         console.log(`${nombref} no se ha creado la aplicación`)
      else 
         console.log(`${nombref} la aplicación se ha creado sin errores`)

      // ya está 
      console.log( `${nombref} fin.`)

      // restaurar el estilo del cursor
      document.body.style.cursor = "default"  

      if ( pie != null )
         pie.textContent = "Inicialización completa sin errores." 
   }

   // -------------------------------------------------------------------

   /**
    * Gestiona un error: "cierra" la aplicación y muestra el mensaje de error. Da estos pasos:
    * 
    * - Los elementos de la página dejan de ser visibles, se desactivan las FGEs.
    * - Se anula la instancia de la aplicación (si no era nula ya).
    * - La página muestra únicamente el mensaje de error y no se puede interactuar con ella.
    * 
    * @param err  objeto de error, puede ser de cualquier tipo   
    */
   public static gestionarError( err : any, punto_llamada : string ) : void 
   {
      const nombref = "AplicacionWeb.gestionarError:"

      let descripcion : string = "(no hay más información del error)"

      console.log(`${nombref} err constructor name == '${err.constructor.name}'`)

      let tipo : string = "no disponible"
      let clase : string = err.constructor.name

      if ( err instanceof PromiseRejectionEvent )
      {
         tipo = "es un 'PromiseRejectionEvent'"
         descripcion = err.reason
      }
      //else if ( err.hasOwnProperty('message') )
      else if ( err.message !== undefined )
      {
         tipo = "con 'message'"
         descripcion = err.message
      }
      //else if ( err.hasOwnProperty('toString') )
      else if ( err.toString !== undefined )
      {
         tipo = "convertible a 'string'"
         descripcion = err.toString()
      }
      else 
         descripcion = `sin información del motivo (el objeto de error es de clase '${err.constructor.name}').`

      
      console.log(`${nombref} Ha ocurrido un ERROR. La aplicación se desactivará.`)
      console.log(`${nombref} Punto de llamda   : ${punto_llamada}`)
      console.log(`${nombref} Tipo de error     : ${tipo}`)
      console.log(`${nombref} Nombre clase error: ${clase}`)
      console.log(`${nombref} Descripción       : ${descripcion}`)

      let instancia = AplicacionWeb.instancia_o_null 
      
      // eliminar la instancia si todavía existe
      if ( instancia != null )
      { 
         instancia.desactivarFGEs() // desactiva las FGEs, restaura valores anteriores (ver la función)
         AplicacionWeb.anularInstancia() // desconecta la aplicación para que no se vuelva a usar
      }

      // desactivar gestores de eventos de error (se activaron al inicializar)
      window.onerror = null 
      window.onunhandledrejection = null  

      // mostrar un cuadro con el error (en una capa sobre la ventana)
      const texto_html : string = `
            <div style='
               z-index    : 1 ;
               position   : absolute;
               top        : 50%;
               left       : 50%;
               font-size  : 14pt ;
               transform  : translate(-50%, -50%);
               text-align : center ;
            '>
            Ha ocurrido un error. Incluyo aquí la descripción disponible.
            <div style='
               color : rgb(100%,40%,40%);
            '>
               ${descripcion}
            </div>
         </div>
      `
      let cuadro :HTMLDivElement = document.createElement('div') 
      cuadro.setAttribute("style", ` 
         width:100%; height:100% ; background-color: rgb(20%,20%,20%);
         position: absolute ; top:0px ; left:0px;
      `)

      cuadro.innerHTML = texto_html 

      let lista_body = document.getElementsByTagName('body')
      let body = lista_body[0]
      if ( body == null )
         return 

      body.appendChild( cuadro )

      
   }

}
// -------------------------------------------------------------------

