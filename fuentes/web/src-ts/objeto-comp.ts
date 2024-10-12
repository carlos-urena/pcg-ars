import { Log } from "./utilidades.js"
import { Mat4, CMat4 } from "./vec-mat.js"
import { ObjetoVisualizable } from "./objeto-visu.js"
import { Textura } from "./texturas.js"
import { Material } from "./material.js"
import { AplicacionWeb } from "./aplicacion-web.js"



export class ObjetoCompuesto extends ObjetoVisualizable
{
   private objetos : ObjetoVisualizable[] = []

   /**
    * Añade un objeto al array de objetos de este objeto compuesto.
    * Le pone al objeto una matriz (si se pasa como parámetro, es opcional).
    * 
    * @param objeto - objeto a añadir
    * @param mat  - matriz para ponersela al objeto (opcional)
    * @returns - número de entrada del array donde queda el objeto añadido
    */
   public agregar( objeto : ObjetoVisualizable, mat? : Mat4 ) : number 
   {
      if ( mat !== undefined )
      {
         objeto.matrizModelado = mat.clonar()  
         this.objetos.push( objeto )
      }
      else 
         this.objetos.push( objeto )
      return this.objetos.length-1
   }
   // ----------------------------------------------------------------------------

   
   /**
    * Visualiza este nodo del grafo de escena, 
    * (si la aplicación tiene activada la iluminación, tiene en cuento normales )
    */
   public visualizar() : void 
   {
      const nombref : string = `ObjetoCompuesto.visualizar (${this.nombre}):`
      let apl   = AplicacionWeb.instancia
      let cauce = apl.cauce
       
      // guardar estado: color, material, textura, matriz de modelado
      this.guardarCambiarEstado( cauce )

      // recorrer las entradas y visualiar cada una de ellas
      for( let objeto of this.objetos )
         objeto.visualizar()
   
      // restaurar estado
      this.restaurarEstado( cauce )
   }
   // ----------------------------------------------------------------------------

   /**
     * Visualiza el objeto sobre un cauce básico, únicamente la geometría, nada más
     * (se supone que el cauce está activo al llamar a este método)
     */
   public visualizarGeometria( cauceb : CauceBase ) : void 
   {
       const nombref : string = `MallaInd.visualizarGeometria (${this.nombre}):`
       let gl = AplicacionWeb.instancia.gl

       if ( this.tieneMatrizModelado )
       {
           cauceb.pushMM()
           cauceb.compMM( this.matrizModelado)
       }

       for( let objeto of this.objetos )
         objeto.visualizarGeometria( cauceb )

       if ( this.tieneMatrizModelado )
           cauceb.popMM()
   }
   // ----------------------------------------------------------------------------

   public visualizarAristas() : void 
   {
      const nombref : string = `ObjetoCompuesto.visualizarAristas  (${this.nombre}):`
      let cauce = AplicacionWeb.instancia.cauce 

      this.pushCompMM( cauce )
      for( let objeto of this.objetos )
         objeto.visualizarAristas()
      this.popMM( cauce )
   }
   // -----------------------------------------------------------------------------

   public visualizarNormales() : void 
   {
      const nombref : string = `ObjetoCompuesto.visualizarNormales  (${this.nombre}):`
      let cauce = AplicacionWeb.instancia.cauce 
      
      this.pushCompMM( cauce )
      for( let objeto of this.objetos )
         objeto.visualizarNormales()
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
export class OC_GrafoTest extends ObjetoCompuesto
{
   constructor( textura : Textura )
   {
      super()
      this.nombre = 'OC_GrafoTest'

      let mr = CMat4.rotacionYgrad( 70.0 )
      let mt = CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.3 ]))

      this.agregar( new CuadradoXYTextura( textura ), mr.componer( mt ) )
      //this.agregar( new CuadradoXYTextura( textura ), mt.componer( mr ) )
      this.agregar( new CuadroXYColores() )

      let m = CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.2 ]))
      this.agregar( new TrianguloTest(), m )
      
      m = m.componer( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.2 ])) )
      this.agregar( new TrianguloIndexadoTest(), m )

      m = m.componer( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.2 ])) )
      this.agregar( new RejillaXY(), m )
      
      m = m.componer( CMat4.traslacion( new Vec3([ 0.0, 0.0, 0.5 ])) )
      this.agregar( new RejillaXY(), m )
   }
}

import { MallaEsfera, MallaCono, MallaCilindro } from "./malla-sup-par.js"
import { CauceBase } from "./cauce-base.js"
/**
 * Clase de pruebas para grafos de escena (contiene varios objetos de prueba con distintos materiales 
 * y distintas texturas)
 */
export class OC_GrafoTest2 extends ObjetoCompuesto
{
   constructor( tex1 : Textura, tex2 : Textura, tex3 : Textura )
   {
      super()
      this.nombre = 'OC_GrafoTest2'

      let esfe = new MallaEsfera(32,32) ;   esfe.textura = tex1 
      let cono = new MallaCono(32,32) ;     cono.textura = tex2 
      let cili = new MallaCilindro(32,32) ; cili.textura = tex3

      let m = CMat4.escalado( new Vec3([ 0.4, 0.4, 0.4 ]))
      this.agregar( esfe, m )

      m = m.componer( CMat4.traslacion( new Vec3([ 2.0, 0.0, 0.0 ])) ) 
      this.agregar( cono, m )

      m = m.componer( CMat4.traslacion( new Vec3([ 2.0, 0.0, 0.0 ])) )
      this.agregar( cili, m )
   
   }
}

