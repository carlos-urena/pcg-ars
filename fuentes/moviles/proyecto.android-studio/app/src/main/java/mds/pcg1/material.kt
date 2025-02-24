package mds.pcg1.material


// *************************************************************************************************
/**
 * Clase que encapsula los parámetros de un material
 */

class Material
{

    var ka  : Float // coeficiente de reflexión ambiental
    var kd  : Float // coeficiente de reflexión difusa
    var ks  : Float // coeficiente de reflexión pseudo-especular
    var exp : Float // exponente de la reflexión pseudo-especular

    // ---------------------------------------------------------------------------------------------
    /**
     * construye un material con los valores por defecto de los atributos.
     */
    constructor()
    {
        ka  = 0.1f
        kd  = 0.4f
        ks  = 1.5f
        exp = 64.0f
    }
    // ---------------------------------------------------------------------------------------------

    /**
     * construye un material, dando explícitamente los atributos
     */
    constructor( pka : Float, pkd : Float, pks : Float, pexp : Float )
    {
        ka  = pka
        kd  = pkd
        ks  = pks
        exp = pexp
    }
}

// *************************************************************************************************



