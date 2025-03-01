import { Vec3, Mat4, CMat4 } from "./vec-mat.js"
import { ObjetoVisualizable } from "./objeto-visu.js"
import { Textura } from "./texturas.js"
import { Material } from "./material.js"
import { AplicacionWeb } from "./aplicacion-web.js"
import { Log } from "./utilidades.js"
import { ObjetoCompuesto } from "./objeto-comp.js"
import { CuadradoXZ, CuadradoXYTextura } from "./malla-ind.js"
import { TrianguloTest, TrianguloIndexadoTest, RejillaXY } from "./utilidades.js"
import { CuadroXYColores } from "./vaos-vbos.js"


/**
 * Tipo unión para los objetos en las entradas de un nodo del grafo de Escena
 */
type TipoObjetoNGE = ObjetoVisualizable | Mat4 | Textura | Material


class NodoGrafoEscena extends ObjetoVisualizable
{
   private entradas : TipoObjetoNGE[] = []

   public agregar( objeto : TipoObjetoNGE ) : number 
   {
      this.entradas.push( objeto )
      return this.entradas.length-1
   }
   
   /**
    * Visualiza este nodo del grafo de escena, 
    * (si la aplicación tiene activada la iluminación, tiene en cuento normales )
    */
   public visualizar() : void 
   {
      const nombref : string = `NodoGrafoEscena.visualizar (${this.nombre}):`
      let apl   = AplicacionWeb.instancia
      let cauce = apl.cauce

      // guardar atributos que pueden cambiar durante el recorrido del nodo
      
      cauce.pushMaterial()
      cauce.pushTextura()
      cauce.pushMM()
       
      // guardar estado: color, material, textura, matriz de modelado
      this.guardarCambiarEstado( cauce )

      // recorrer las entradas y procesar cada una de ellas en función del 
      // tipo de objeto que hay en la misma

      for( let objeto of this.entradas )
      {
         if ( objeto instanceof ObjetoVisualizable )
            objeto.visualizar()
            
         else if ( objeto instanceof Mat4 )
            cauce.compMM( objeto )
            
         else if ( objeto instanceof Textura )
           cauce.fijarTextura( objeto )

         else if ( objeto instanceof Material )
         {
            if ( apl.iluminacion_activada )
               cauce.fijarMaterial( objeto )
         }
      }
      
      // recuperar estado anterior: color, material, textura, matriz de modelado
      this.restaurarEstado( cauce )

      // restaurar atributos
      cauce.popMM() 
      cauce.popTextura()
      cauce.popMaterial() 
   }

   /**
     * Visualiza el objeto sobre un cauce básico, únicamente la geometría, nada más
     * (se supone que el cauce está activo al llamar a este método)
     */
   public visualizarGeometria( cauceb : CauceBase ) : void 
   {
      const nombref : string = `MallaInd.visualizarGeometria (${this.nombre}):`
      let gl = AplicacionWeb.instancia.gl

      cauceb.pushMM()

      if ( this.tieneMatrizModelado )
         cauceb.compMM( this.matrizModelado)

      for( let objeto of this.entradas )
      {
         if ( objeto instanceof ObjetoVisualizable )
            objeto.visualizarGeometria( cauceb )
         else if ( objeto instanceof Mat4 )
            cauceb.compMM( objeto ) 
      }
      cauceb.popMM()
   }
   
   public visualizarAristas() : void 
   {
      const nombref : string = `NodoGrafoEscena.visualizarAristas  (${this.nombre}):`
      let cauce = AplicacionWeb.instancia.cauce 

      
      this.pushCompMM( cauce )

      for( let objeto of this.entradas )
      {
         if ( objeto instanceof ObjetoVisualizable )
            objeto.visualizarAristas()
         else if ( objeto instanceof Mat4 )
            cauce.compMM( objeto )
        
      }
      this.popMM( cauce )
   }

   public visualizarNormales() : void 
   {
      const nombref : string = `NodoGrafoEscena.visualizarNormales  (${this.nombre}):`
      let cauce = AplicacionWeb.instancia.cauce 

      this.pushCompMM( cauce )

      for( let objeto of this.entradas )
      {
         if ( objeto instanceof ObjetoVisualizable )
            objeto.visualizarNormales()
         else if ( objeto instanceof Mat4 )
            cauce.compMM( objeto )
        
      }
      this.popMM( cauce )
   }
}
// -------------------------------------------------------------------------------------------





/**
 * Clase de pruebas para grafos de escena (contiene varios objetos de prueba)
 */
export class GrafoTest extends NodoGrafoEscena
{
   constructor( textura : Textura )
   {
      super()
      this.nombre = 'GrafoTest'

      let n = new NodoGrafoEscena()
      n.agregar( CMat4.rotacionYgrad( 70.0 ))
      n.agregar( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.3 ])))
      n.agregar( new CuadradoXYTextura(textura) )

      this.agregar( n )
      this.agregar( new CuadroXYColores() )
      this.agregar( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.2 ])))
      this.agregar( new TrianguloTest() )
      this.agregar( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.2 ])))
      this.agregar( new TrianguloIndexadoTest() )
      this.agregar( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.2 ])))
      this.agregar( new RejillaXY() )
      this.agregar( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.5 ])))
      this.agregar( new RejillaXY() )
   }
}

import { MallaEsfera, MallaCono, MallaCilindro } from "./malla-sup-par.js"
import { CauceBase } from "./cauce-base.js"
/**
 * Clase de pruebas para grafos de escena (contiene varios objetos de prueba con distintos materiales 
 * y distintas texturas)
 */
export class GrafoTest2 extends NodoGrafoEscena
{
   constructor( tex1 : Textura, tex2 : Textura, tex3 : Textura )
   {
      super()
      this.nombre = 'GrafoTest2'

      this.agregar( CMat4.escalado( new Vec3([ 0.4, 0.4, 0.4 ])))
      this.agregar( tex1 )
      this.agregar( new MallaEsfera(32,32) )
      this.agregar( CMat4.traslacion( new Vec3([ 2.0, 0.0, 0.0 ])))

      this.agregar( tex2 )
      this.agregar( new MallaCono(32,32) )
      this.agregar( CMat4.traslacion( new Vec3([ 2.0, 0.0, 0.0 ])))

      this.agregar( tex3 )
      this.agregar( new MallaCilindro(32,32) )
      this.agregar( CMat4.traslacion( new Vec3([ 2.0, 0.0, 0.0 ])))

      
   }
}

// -------------------------------------------------------------------------------------------

/**
 * Animación sencilla de una esfera que hace circunferencias entorno al origen
 */
export class GrafoTest3SombrasTextura extends NodoGrafoEscena 
{    
      private radio_circ : number = 0.8 // radio de la circunferencia que hace la esfera
      private radio_esf  : number = 0.1 // radio de la esfera  
      //private textura   : Textura
     /**
      * Objeto compuesto que tiene la esfera dentro (instanciada con escalado y traslacion en Y)
      */
     private nodo_esfera : ObjetoCompuesto

     constructor( p_textura : Textura )
     {
          super()
          this.nombre = "GrafoTest3SombrasTextura"
          //this.textura = p_textura

          const re = this.radio_esf

          // construir las matrices que le afectarán a la esfera (aparte de la de animación)
          const mat_traX = CMat4.traslacion( new Vec3([this.radio_circ, 0.0, 0.0])) // 3. trasladar en X una distancia igual al radio de la circunferencia
          const mat_escR = CMat4.escalado( new Vec3([ re,re,re ]))       // 2. escalar para que tenga el radio 're' en lugar de 1 (=this.radio_esf)
          const mat_traY = CMat4.traslacion( new Vec3([0.0, 1.0, 0.0]))  // 1. elevar centro en Y una unidad para que la parte inferior sea tangente al rectángulo de la base.
          const mat_esf  = mat_traX.componer( mat_escR ).componer( mat_traY ) // componer trasl.Y, después escalado, después trasl. X
          
          // crear la malla con la esfera y con su transformación (mat_esf)
          let malla_esfera = new MallaEsfera( 32, 32 )
          malla_esfera.matrizModelado = mat_esf

          // el nodo de la esfera contiene unicamente la esfera, pero se usa 
          // para después cambiarle su matriz de modelado con el tiempo
          this.nodo_esfera = new ObjetoCompuesto         
          this.nodo_esfera.agregar( malla_esfera )

          // el objeto raiz contiene los dos sub-objetos de la escena: el nodo esfera y el cuadrado de la base.
          
          this.agregar( this.nodo_esfera )

          let cuadrado = new CuadradoXZ()
          cuadrado.textura = p_textura 
          this.agregar( cuadrado )
     }
}

