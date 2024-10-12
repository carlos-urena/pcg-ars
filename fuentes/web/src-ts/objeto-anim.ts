import { Mat4, CMat4 } from "./vec-mat.js"
import { ObjetoVisualizable } from "./objeto-visu.js"
import { AplicacionWeb } from "./aplicacion-web.js"
import { CauceBase } from "./cauce-base.js"

/**
 * Estados posibles de un objeto animado:
 * 
 * - parado: no animado, actulizado a t=0
 * - pausado: no animado, pero en algún instante de la animación
 * - animado: en movimiento, en algún instente de la animación
 * 
 * El estado de un objeto animado se controla llamando a estos métodos:
 * 
 * - comenzar (start)  - comienza la animación, estando parado
 * - pausar (pause)    - pausa después de comenzar
 * - reanudar (resume) - reanuda después de pausado
 * - reiniciar (reset) - vuelve al estado inicial y para la animación
 * 
 * El comportamiento del objeto se define implementando estos métodos:
 * 
 * - estadoInicial - pone el objeto en su estado inicial
 * - actualizarObjeto( dt ) - actualiza el estado después de transcurrido 'dt' segundos respecto del estado actual
 *
 */

export enum EstadoAnim { parado = 0, pausado = 1, animado = 2 } 

/**
 * Clase base para objetos que se puden animar
 */
export abstract class ObjetoAnimado extends ObjetoVisualizable
{
   /**
    * Objeto visualizable que se usa para visualizar este objeto animado
    *  - Usualmente será una simple malla indexada, o bien un objeto compuesto
    *  - Debe ser inicialiado en el constructor de las clases derivadas
    */
   protected obj_vis : ObjetoVisualizable | null = null 

   /**
    * Indica el tiempo total acumulado desde que se inició la animación, sin 
    * contar los intervalos en los que ha estado pausado.
    */
   private t_anim : number = 0.0

   /**
    * Devuelve el tiempo acumulado de la animación (tiempo desde inicio sin 
    * contar con el tiempo pausado)
    */
   protected get t_animado() : number 
   {
      return this.t_anim 
   }

   /**
    * Instante de tiempo 
    * absoluto de la última actualización de estado 
    * (es el instante con el que se corresponde el estado actual del objeto)
    */
   private t_ult : number = 0.0 

   /**
    * Estado actual (inicialmente parado)
    */
   private estado_act : EstadoAnim = EstadoAnim.parado 

   /**
    * Devuelve el estado de la animación
    */

   public get estado() : EstadoAnim 
   {
      return this.estado_act
   }

   // --------------------------------------------------------------------------------------------------------
   // METODOS abstractos (se deben redefinir en clases derivadas)

   /**
    * Pone el objeto en su estado inicial 
    * (debe ser redefinido en clases derivadas, y debe poner las variables específicas 
    * de estado al mismo valor que en el constructor.
    */
   protected abstract estadoInicial() : void ;

   /**
    * Método específico que realiza una actualización de estado de las variables propias 
    * del objeto 
    *   * Debe ser redefinido en las clases derivadas
    *   * Se invoca desde  'actualizarEstado'
    *   * En este método se debe leer 't_animado' para conocer el tiempo correspondiente al estado actual del objeto
    *   * El estado se debe actualizar al instante 't' igual a 't_animado + inc_t_animado'
    * 
    * @param inc_t (number) - incremento del tiempo respecto del estado actual del objeto
    */
   protected abstract actualizarObjeto( inc_t_animado : number ) : void ;

   // --------------------------------------------------------------------------------------------------------
   // METODOS

   /**
    * Iniciar la animación (solo si está parada)
    * - Resetea el contador de tiempo acumulado 't_anim' y de ultima actu. 't_ult'
    * - Pone el objeto en su estado inicial
    * @param t_act instante de tiempo absoluto actual
    */
   public comenzar( t_act : number )
   {
      if ( this.estado_act != EstadoAnim.parado )
         throw new Error(`Se ha llamado a 'iniciar', pero el objeto no está en estado 'parado'`) 

      this.t_ult  = t_act
      this.t_anim  = 0.0 
      this.estado_act = EstadoAnim.animado
      this.estadoInicial()
   }
   
   // -------------------------------------------------------------------------------------------------------- 

   /**
    * Pausa la animación en el instante 't_act'
    * - La animación debe se estar en estado 'animado' 
    * - Se actualiza el estado del objeto al instante 't_act'
    * - Pasa al estado 'pausado' 
    * @param t_act instante absoluto en el que se pausa la animación 
    */
   public pausar( t_act : number )
   {
      if ( this.estado_act != EstadoAnim.animado )
         throw new Error(`Se ha llamado a 'pausar', pero el objeto no está en estado 'animado'`) 

      if ( t_act < this.t_ult )
         throw new Error(`Se ha llamado a 'pausar' con t_act == ${t_act} anterior a t_ult == ${this.t_ult}`)

      this.actualizar( t_act ) // lleva el objeto al estado correspondiente a 't_act'
      this.estado_act = EstadoAnim.pausado
   }
   // --------------------------------------------------------------------------------------------------------
   
   /**
    * Reanuda (en el instante 't_act') una animación que está pausada
    * (debe de estar en estado 'pausado')
    * @param t_act 
    */
   public reanudar( t_act : number )
   {
      if ( this.estado_act != EstadoAnim.pausado )
         throw new Error(`Se ha llamado a 'reanudar', pero el objeto no está en estado 'pausado'`)
   
      if ( t_act < this.t_ult )
         throw new Error(`Se ha llamado a 'reanudar' con t_act == ${t_act} anterior a t_ult == ${this.t_ult}`)

      this.t_ult = t_act
      this.estado_act = EstadoAnim.animado
   }
   // --------------------------------------------------------------------------------------------------------
   
   /**
    * Para la animación y vuelve al estado inicial
    * @param t_act 
    */
   public reiniciar( t_act : number )
   {
      if ( t_act < this.t_ult )
         throw new Error(`Se ha llamado a 'parar' con t_act == ${t_act} anterior a t_ult == ${this.t_ult}`)

      if ( this.estado_act == EstadoAnim.parado )
         return  

      this.t_ult = t_act 
      this.estado_act = EstadoAnim.parado
      this.estadoInicial()
   }
   // --------------------------------------------------------------------------------------------------------
   

   /**
    * Actualiza la animación al instante de tiempo absoluto actual 
    *  - El estado de la animación debe ser 'animado'
    *  - Calcula el tiempo transcurrido desde la ultima actualización
    *  - Invoca a actualizarObjeto
    *  - Añade el tiempo transcurrido al tiempo acumulado 
    *  - Actualiza el intante de ultima actualización.
    * @param t_act instante de tiempo absoluto actual.
    */
   public actualizar( t_act : number ) 
   {
      // si está pausado o parado, no hace nada.
      if ( this.estado_act != EstadoAnim.animado )
         throw new Error(`Se ha llamado a 'actualizar', pero el objeto no está en estado 'animado'`) 

      if ( t_act < this.t_ult )
         throw new Error(`Se ha llamado a 'actualizar' con t_act == ${t_act} anterior a t_ult == ${this.t_ult}`)

      let inc_t = t_act - this.t_ult // nunca negativo

      if ( inc_t > 0.0 )
         this.actualizarObjeto( inc_t )

      this.t_anim += inc_t 
      this.t_ult = t_act 
   }

   // ----------------------------------------------------------------------------------
   // METODOS de Visualización (se delega en el objeto visualizable 'obj_vis')

   /**
    * Visualiza el objeto 
    */
   public visualizar() : void 
   {
      let nombref= `ObjetoAnimado.visualizar (${this.nombre}):`
      if ( this.obj_vis == null )
         throw new Error(`${nombref} error al visualizar objeto animado: no se ha creado objeto visualizable en el ctor.`)

      let cauce = AplicacionWeb.instancia.cauce 
      
      this.guardarCambiarEstado( cauce )
      this.obj_vis.visualizar()
      this.restaurarEstado( cauce )
   }

   /**
     * Visualiza el objeto sobre un cauce básico, únicamente la geometría, nada más
     * (se supone que el cauce está activo al llamar a este método)
     */
   public visualizarGeometria( cauceb: CauceBase ): void 
   {
      let nombref = `ObjetoAnimado.visualizarGeometria (${this.nombre}):`
      if ( this.obj_vis == null )
         throw new Error(`${nombref} error en visualizar objeto animado: no se ha creado objeto visualizable en el ctor.`)

      if ( this.tieneMatrizModelado )
      {
         cauceb.pushMM()
         cauceb.compMM( this.matrizModelado )
      }
      this.obj_vis.visualizarGeometria( cauceb )

      if ( this.tieneMatrizModelado )
         cauceb.popMM()
   }

   /**
    * Visualizar normales del objeto 
    */
   public visualizarNormales() : void 
   {
      if ( this.obj_vis == null )
         throw new Error(`Error al visualizar aristas de objeto animado: no se ha creado objeto visualizable en el ctor.`)
      
      let cauce = AplicacionWeb.instancia.cauce 

      this.pushCompMM( cauce )
      this.obj_vis.visualizarNormales()
      this.popMM( cauce )
   }

   /**
    * Visualizar aristas del objeto
    */
   public visualizarAristas() : void 
   {
      if ( this.obj_vis == null )
         throw new Error(`Error al visualizar normales de objeto animado: no se ha creado objeto visualizable en el ctor.`)
      
      let cauce = AplicacionWeb.instancia.cauce 

      this.pushCompMM( cauce )
      this.obj_vis.visualizarAristas()
      this.popMM( cauce )
   }
   
}

// **********************************************************************************************




