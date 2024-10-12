import { Assert, Log } from "./utilidades.js"
//import { AplicacionWeb } from "./aplicacion-web.js"

export class Material
{
   private nombre_mat : string = "material anónimo";  // nombre del material

   public set nombre( nuevo_nombre : string ) { this.nombre_mat = nuevo_nombre }
   public get nombre( ) : string              { return this.nombre_mat }
   
   public ka  : number = 0.2  // coeficiente de reflexión ambiente (en 0..1)
   public kd  : number = 0.8  // coeficiente de reflexión difusa (en 0..1)
   public ks  : number = 0.0  // coeficiente de reflexión pseudo-especular (en 0..1)
   public exp : number = 5.0  // exponente de brillo para reflexión pseudo-especular

   // ---------------------------------------------------------------------------

   /**
    * Crea un nuevo material
    * @param ka (number) coeficiente de reflexión ambiente (en 0..1)
    * @param kd (number) coeficiente de reflexión difusa (en 0..1)
    * @param ks (number) coeficiente de reflexión pseudo-especular (en 0..1)
    * @param exp (number) exponente de brillo para reflexión pseudo-especular (2.0 como mínimo)
    */
   constructor( ka : number, kd : number, ks : number, exp : number )
   {
      const nombref : string = 'Material.constructor:'

      Assert( 0.0 <= ka && ka <= 1.0, `${nombref} 'ka' (${ka}) está fuera del rango 0..1`)
      Assert( 0.0 <= kd && kd <= 1.0, `${nombref} 'kd' (${kd}) está fuera del rango 0..1`)
      Assert( 0.0 <= ks && ks <= 1.0, `${nombref} 'ks' (${ks}) está fuera del rango 0..1`)
      Assert( 2.0 <= exp, `${nombref} 'exp_pse' (${exp}) no puede ser menor que 2.0`)

      this.ka  = ka
      this.kd  = kd 
      this.ks  = ks
      this.exp = exp
   }
   
   
}




