
import { Log }                  
from "./utilidades.js"
import { Vec3, Mat4 }
from "./vec-mat.js"

import { Cauce }
from "./cauce.js"

import { Textura }
from "./texturas.js" 

import { Material }
from "./material.js"

import { AplicacionWeb }
from "./aplicacion-web.js"
import { CauceBase } from "./cauce-base.js"

export abstract class ObjetoVisualizable
{
    
    /**
     * Color del objeto, null si no tiene
     */
    private color : Vec3 | null = null 

    public get tieneColor() : Boolean
    {
        return this.color != null
    }
    public get leerColor() : Vec3
    {
        if ( this.color == null )
            throw new Error(`intento de leer el color de un ObjetoVisualizable que no lo tiene (${this.nombre})`)
        return this.color
    }
    public set fijarColor( nuevo_color : Vec3 | null ) 
    {
        this.color = nuevo_color
    }

    /**
     * Textura del objeto:
     *   * undefined    --> hereda la textura que hubiera en el cauce cada vez que se visualiza
     *   * null         --> se visualiza sin textura    
     *   * en otro caso --> apunta a una textura que se usa para visualizar
     */
    private textura_act : Textura | null | undefined = undefined

    /**
     * Devuelve true si la textura no se hereda.
     */
    public get tieneTexturaDefinida() : Boolean 
    {
        if ( this.textura_act === undefined )
            return false 
        return true 
    }
    /**
     * Devuelve la textura del objeto (solo si la tiene definida)
     *   * Puede devolver 'null' (el objeto se dibuja sin textura)
     *   * O bien devolver un puntero a una textura (se dibuja con esa textura)
     */
    public get textura() : Textura | null 
    {
        if ( this.textura_act === undefined ) 
            throw new Error(`intento de leer la textura de un objeto que no tiene` )

        return this.textura_act
    }
    /**
     * Fija la textura del objeto.
     */
    public set textura( nueva_textura : Textura | null | undefined )
    {
        this.textura_act = nueva_textura
    } 


    /**
     * Matriz de modelado de este objeto (si no es nula),
     *   * Si está presente, es adicional a la que haya establecida en el cauce
     * (se hace pushMM antes de visualizar y popMM después)
     */
    private matrizm : Mat4 | null = null

    /**
     * Devuelve 'true' si la matriz de modelado no es nula
     */
    public get tieneMatrizModelado() : boolean 
    {
        return this.matrizm != null 
    }
    /**
     * Devuelve la matriz de modelado (no nula).
     * Si es nula produce un error
     */
    public get matrizModelado() : Mat4 
    { 
        if ( this.matrizm == null )  
            throw new Error("Intento de leer una matriz de modelado nula")
        return this.matrizm
    }
    /**
     * Fija la matriz de modelado (clona la que se le pasa como parámetro)
     */
    public set matrizModelado( nueva_matrizm : Mat4 | null )
    {
        if ( nueva_matrizm == null )
            this.matrizm = null 
        else 
            this.matrizm = nueva_matrizm.clonar()
    }

    /**
     * Si el objeto tiene definida su propia matriz de modelado
     * Guarda (push) la matriz de modelado en el cauce, y la compone con la actual
     * @param cauce 
     */
    public pushCompMM( cauce : Cauce )
    {
        if ( this.tieneMatrizModelado )
        {
            cauce.pushMM()
            cauce.compMM( this.matrizModelado )
        }
    }
    public popMM( cauce : Cauce )
    {
        if ( this.tieneMatrizModelado )
            cauce.popMM()
    }

    /**
     * Material del objeto (null si no tiene ninguno definido)
     */
    private material_act : Material | null = null 

    /**
     * Devuelve true si el objeto tiene un material propio, false en otro caso
     */
    public get tieneMaterial() : boolean 
    {
        return this.material_act != null 
    }
    /**
     * Si tiene material, lo devuelve, en otro caso se produce un error
     */
    public get material() : Material 
    {
        const nombref = "ObjetoVisualizable.material (getter)"
        if ( this.material_act == null )
            throw new Error(`${nombref}: intento de leer un material nulo.`)
        return this.material_act
    }

    /**
     * Cambia el material actual, se puede borrar (con null) o
     * bien poner uno nuevo (!= null)
     */
    public set material( nuevo_material : Material | null ) 
    {
        this.material_act = nuevo_material
    }


    /**
     * Nombre actual del objeto
     */
    private nombre_act  : string = "no asignado" 

    /**
     * Devuelve el nombre del objeto 
     */
    public get nombre() : string
    {
        return this.nombre_act 
    }
    /**
     * Cambia el nombre del objeto.
     */
    public set nombre( nuevo_nombre : string ) 
    {
        this.nombre_act = nuevo_nombre
    }

    /**
     * Parámetro 'S', usado para gestionar diversos aspectos del objeto
     */
    private param_S_act : number = AplicacionWeb.valor_inicial_param_S

    /**
     * Fija el nuevo valor del parámetro S
     */
    public set param_S( nuevo_param_s : number )  
    {
        this.param_S_act = nuevo_param_s
    }

    public get param_S() : number 
    {
        return this.param_S_act
    }

    /**
     * Visualiza el objeto. este método debe ser redefinido en clases derivadas
     * El objeto se visualiza usando el cauce definido en la aplicación (clase 'Cauce')
     */
    public abstract visualizar() : void ;


    /**
     * Visualiza el objeto sobre un cauce básico, únicamente la geometría, nada más
     * (se supone que el cauce está activo al llamar a este método)
     */
    public visualizarGeometria( cauceb : CauceBase ) : void 
    {
        const nombref : string = `ObjetoVisualizable.visualizarGeometria (${this.nombre}):`
        Log(`${nombref}: advertencia: no se hace nada: este objeto es de una clase que no redefine el método`)
    }

    /**
     * Visualiza las aristas del objeto. Este método puede ser redefinido en clases derivadas, si 
     * no se hace, el método no hace nada (eso implica que ese objeto no tiene aristas que se pueden visualizar 
     * o que no se ha implementado esto)
     */
    public visualizarAristas() : void  
    {
        Log(`ObjetoVisible.visualizarAristas: no se hace nada: este objeto es de una clase que no redefine el método`)
    }
    

    /**
     * Visualiza las normales del objeto. Este método puede ser redefinido en clases derivadas, si 
     * no se hace, el método no hace nada (eso implica que ese objeto no tiene normales que se pueden visualizar 
     * o que no se ha implementado esto)
     */
    public visualizarNormales() : void 
    {
        Log(`ObjetoVisible.visualizarNormales: no se hace nada: este objeto es de una clase que no redefine el método`)
    }
    
    /**
     * Guarda el estado actual de los (algunos/todos?) los uniforms y 
     * lo actualiza según este objeto
     * @param cauce cauce sobre el que se fija y que guarda el estado.
     */
    public guardarCambiarEstado( cauce : Cauce )
    {
        if ( this.tieneMaterial )
        {
            cauce.pushMaterial()
            cauce.fijarMaterial( this.material )
        }

        if ( this.tieneTexturaDefinida )
        {
            cauce.pushTextura()
            cauce.fijarTextura( this.textura )
        }

        if ( this.tieneColor )
        {
            cauce.pushColor()
            cauce.fijarColor( this.leerColor )
        }

        this.pushCompMM( cauce )
    }

    /**
     * Restaura el estado previo a la última llamada a 'guardarFijarEstado'
     * @param cauce cauce desde donde se restaura el estado.
     */
    public restaurarEstado( cauce: Cauce )
    {
        this.popMM( cauce )

        if ( this.tieneColor )
            cauce.popColor()   

        if ( this.tieneTexturaDefinida )
            cauce.popTextura()

        if ( this.tieneMaterial )
            cauce.popMaterial()
    }
}