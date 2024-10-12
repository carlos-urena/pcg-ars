import { Mat4, CMat4 } from "./vec-mat.js"
import { ObjetoVisualizable } from "./objeto-visu.js"
import { Textura } from "./texturas.js"
import { Material } from "./material.js"
import { AplicacionWeb } from "./aplicacion-web.js"
import { Log } from "./utilidades.js"


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


import { Vec3 } from "./vec-mat.js"
import { TrianguloTest, TrianguloIndexadoTest, RejillaXY } from "./utilidades.js"
import { CuadroXYColores } from "./vaos-vbos.js"
import { CuadradoXYTextura } from "./malla-ind.js"


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

