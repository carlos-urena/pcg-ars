// *********************************************************************
// **
// ** Asignatura: PCG (Programación del cauce gráfico).
// ** 
// ** Implementación de la clase 'Cuadroide'
// ** Copyright (C) 2016-2023 Carlos Ureña
// **
// ** Implementación de los métodos de la clase
// ** Puntero a la aplicación actual, inicialmente nulo (la instancia se crea en la func. 'main')
// **    
// ** This program is free software: you can redistribute it and/or modify
// ** it under the terms of the GNU General Public License as published by
// ** the Free Software Foundation, either version 3 of the License, or
// ** (at your option) any later version.
// **
// ** This program is distributed in the hope that it will be useful,
// ** but WITHOUT ANY WARRANTY; without even the implied warranty of
// ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// ** GNU General Public License for more details.
// **
// ** You should have received a copy of the GNU General Public License
// ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
// **
// *********************************************************************


#include <vector>
#include "malla-ind.h"
#include "aplic-3d.h"
#include "androide.h"


//**********************************************************************
//**
//**  class Cuadroide
//**
//**********************************************************************

Cuadroide::Cuadroide(  )
{
   using namespace std ;
   using namespace glm ;

   // constantes que definen la forma del cuadroide

   rad_c = 0.50 ;  // radio cilindros cuerpo
   alt_c = 0.80 ;  // altura cuerpo (base a centro cabeza)

   rad_b = 0.10 ;  // radio cilindros brazos
   alt_b = 0.50 ;  // altura brazos (entre centros esferas)

   rad_p = 0.13 ;  // radio cilindros pies
   alt_p = 0.35 ;  // altura pies (entre centros esferas)
   sep_p = 0.20 ;  // distancia entre eje del pie y el eje Y

   // crear objetos en cc.mm.

   const int nu = 30, nv = 15 ;

   obj_esfera     = new Esfera() ; //, nu, nv  );
   obj_semiesfera = new Semiesfera() ; //, nu, nv ) ;
   obj_cilindro   = new Cilindro( nu, nv ) ; //, nu, nv ) ;

   obj_esfera->ponerNombre("esfera (cuadroide)");
   obj_semiesfera->ponerNombre("semiesfera (cuadroide)") ;
   obj_cilindro->ponerNombre("cilindro (cuadroide)") ;
   obj_disco  = NULL ;

   // crear materiales
   matVerdePlano = new Material( /**vec3(0.2,1.0,0.3),**/ 0.20, 0.50, 0.60, 5.0 );
   matNegroBrill = new Material( /**vec3(1.0,1.0,1.0),**/ 0.05, 0.05, 0.90, 5.0 );

   matVerdePlano->ponerNombre("material verde plano");
   matNegroBrill->ponerNombre("material negro brill");

   ponerNombre("Androide.");

   // -------------------------------------------------------------------------
   // entrada 0:
   // material por defeco al inicio del nodo raiz
   agregar( matVerdePlano );
   // -------------------------------------------------------------------------
   // entrada 1:
   // poner una traslacion en vertical para llevarnos el cuadroide al origen
   agregar( translate( vec3( 0.0, -0.8, 0.0 ) )) ;
   // -------------------------------------------------------------------------
   // entrada 2:
   // traslacion asociada a un grado de libertad
   const unsigned indm_trasl =
      agregar( translate( vec3( 0.0, 0.0, 0.0 ) ) ) ;

   pmTraslacionRaiz = leerPtrMatriz( indm_trasl );

   // -------------------------------------------------------------------------
   // entrada: pie derecho
   auto nodo_pd = new NodoGrafoEscena() ;
   nodo_pd->ponerNombre("pie derecho");
   nodo_pd->ponerColor( vec3(0.3, 0.6, 0.2) );
   nodo_pd->agregar( translate( vec3( -sep_p, 0.0, 0.0 ) ) ); // despl X negativo

   nodo_pd->agregar( translate( vec3{ 0.0,+alt_p,0.0 } ));
   const int indmpd = nodo_pd->agregar( rotate( radians(0.0f), vec3(1.0, 0.0, 0.0) ));
   nodo_pd->agregar( translate( vec3{ 0.0,-alt_p,0.0 }));

   nodo_pd->agregar( new Miembro( rad_p, alt_p, this ) );
   agregar( nodo_pd );

   pmRotPieDerecho = nodo_pd->leerPtrMatriz( indmpd ) ;

   // -------------------------------------------------------------------------
   // entrada: pie izquierdo
   auto nodo_pi = new NodoGrafoEscena() ;
   nodo_pi->ponerNombre("pie izquierdo");
   nodo_pi->ponerColor( vec3(0.3, 0.6, 0.2) );
   nodo_pi->agregar( translate( vec3{ +sep_p, 0.0, 0.0 } ) ); // despl X positivo

   nodo_pi->agregar( translate( vec3{ 0.0,+alt_p,0.0 }));
   const unsigned indmpi = nodo_pi->agregar( rotate( radians(0.0f), vec3{1.0, 0.0, 0.0} ));
   nodo_pi->agregar( translate( vec3{ 0.0,-alt_p,0.0 }));

   nodo_pi->agregar( new Miembro( rad_p, alt_p, this ) );
   agregar( nodo_pi );

   pmRotPieIzquierdo = nodo_pi->leerPtrMatriz( indmpi ) ;

   // -------------------------------------------------------------------------
   // resto del cuerpo (desplazado 'alt_p' en Y)
   NodoGrafoEscena * rc = new NodoGrafoEscena() ;
   rc->ponerNombre("cuadroide: resto cuerpo");
   rc->agregar( translate( vec3{ 0.0, alt_p, 0.0 } ) ); // desplazamiento en Y

   // -------------------------------------------------------------------------
   // brazo izquierdo (en el lado positivo de X)
   auto nodo_bi = new NodoGrafoEscena() ;
   nodo_bi->ponerNombre("brazo izquierdo");
   nodo_bi->ponerColor({0.0, 0.9, 0.1 });
   nodo_bi->agregar( translate( vec3{ +1.0f*(rad_c+rad_b), alt_c-alt_b-rad_b, 0.0f } ) );
   MiembroArti * brazo_izq = new MiembroArti( rad_b, alt_b, this,0 ) ;

   pmRotHombroIzquierdo = brazo_izq->pmRotHombro ;
   pmRotCodoIzquierdo   = brazo_izq->pmRotCodo ;

   nodo_bi->agregar( brazo_izq );
   rc->agregar( nodo_bi );

   // -------------------------------------------------------------------------
   // brazo derecho (en el lado negativo de X)
   auto nodo_bd = new NodoGrafoEscena() ;
   nodo_bd->ponerNombre("brazo derecho");
   nodo_bd->ponerColor( vec3(0.0, 0.9, 0.1) );
   nodo_bd->agregar( translate( vec3{ -1.00f*(rad_c+rad_b), alt_c-alt_b-rad_b, 0.0f } ) );
   MiembroArti * brazo_der = new MiembroArti( rad_b, alt_b, this, 1 ) ;

   pmRotHombroDerecho = brazo_der->pmRotHombro ;
   pmRotCodoDerecho   = brazo_der->pmRotCodo ;

   nodo_bd->agregar( brazo_der );
   rc->agregar( nodo_bd );

   // -------------------------------------------------------------------------
   // entrada de escalado del tronco (tapa inferior, cilindro y cabeza)
   const unsigned indtc = rc->agregar( scale( vec3(1.0,1.0,1.0)) ) ;

   pmEscaladoTronco = rc->leerPtrMatriz( indtc ) ;

   // -------------------------------------------------------------------------
   // cilindro que forma el tronco
   auto nodo_tr = new NodoGrafoEscena() ;
   nodo_tr->ponerNombre("cilindro tronco");
   nodo_tr->ponerColor( vec3(0.0, 0.5, 0.1) );
   nodo_tr->agregar( scale( vec3(rad_c, alt_c, rad_c )) );
   nodo_tr->agregar( obj_cilindro );
   rc->agregar( nodo_tr );

   // -------------------------------------------------------------------------
   // tapa inferior
   auto nodo_ta = new NodoGrafoEscena() ;
   nodo_ta->ponerNombre("tapa inferior");
   nodo_ta->ponerColor( vec3(0.1, 0.8, 0.3) );
   nodo_ta->agregar( rotate( radians(180.0f), vec3{0.0,0.0,1.0} ) );
   nodo_ta->agregar( scale( vec3( rad_c, rad_c*0.2f, rad_c )) );
   nodo_ta->agregar( obj_semiesfera );
   rc->agregar( nodo_ta );

   // -------------------------------------------------------------------------
   // cabeza
   auto nodoCab = new NodoGrafoEscena() ;
   nodoCab->ponerNombre("cabeza");
   nodoCab->ponerColor( vec3(0.1, 0.8, 0.3) );
   const unsigned indmcab = nodoCab->agregar( rotate( radians(0.0f), vec3{0.0,1.0,0.0} ) );
   nodoCab->agregar( translate( vec3{ radians(0.0f), alt_c*1.02f, 0.0 } ) );
   nodoCab->agregar( scale( vec3{ rad_c, rad_c*0.9f, rad_c } ) );
   nodoCab->agregar( obj_semiesfera );
   nodoCab->agregar( new Ojo( +30.0, this ) );
   nodoCab->agregar( new Ojo( -30.0, this ) );
   nodoCab->agregar( new Antena( +20.0, this ) );
   nodoCab->agregar( new Antena( -20.0, this ) );

   rc->agregar( nodoCab );

   pmRotCabeza = nodoCab->leerPtrMatriz( indmcab );


   // agegar resto de cuerpo ('rc') al cuadroide (this)
   agregar( rc );

}

// ---------------------------------------------------------------------

Cuadroide::~Cuadroide()
{
   using namespace std ;
   //cout << "Invocado destructor de 'Cuadroide' (borra materiales nada más)" << endl ;

   // (no se borran las mallas indexadas pq se añaden a las entradas de los nodos)
   delete matVerdePlano ;
   delete matNegroBrill ;

   //cout << "FIN destructor cuadroide" << endl ;
}

// ----------------------------------------------------------------------
// el cuadroide tiene un total de 9 parámetros

unsigned Cuadroide::leerNumParametros() const
{
   return 9 ;
}

// ----------------------------------------------------------------------
// actualizar la matriz del correspondiente parámetro

void Cuadroide::actualizarEstadoParametro( const unsigned iParam, const float t_sec )
{
   using namespace glm ;

   assert( iParam < leerNumParametros() );

   assert( pmTraslacionRaiz != nullptr );
   assert( pmRotPieDerecho != nullptr );
   assert( pmRotPieIzquierdo != nullptr );
   assert( pmEscaladoTronco != nullptr );
   assert( pmRotCabeza != nullptr );
   assert( pmRotCodoDerecho != nullptr );
   assert( pmRotHombroDerecho != nullptr );
   assert( pmRotCodoIzquierdo != nullptr );
   assert( pmRotHombroIzquierdo != nullptr );

   float v ;
   constexpr float dosPi = 2.0*M_PI ;
   using namespace std ;

   //cout << __PRETTY_FUNCTION__ << " : iParam == " << iParam << ", t_sec == " << t_sec << endl ;

   switch( iParam )
   {
      // traslacion (traslacion en Z)
      case 0 :
         v = sin( 0.5f*t_sec*dosPi );
         *pmTraslacionRaiz = translate( vec3{ 0.0,0.0, v });
         break ;

      // pie derecho (rotacion en X)
      case 1 :
         v = +60.0f*sin( 0.82f*t_sec*dosPi );
         *pmRotPieDerecho = rotate( radians(v), vec3{ 1.0,0.0,0.0 });
         break ;

      // pie izquierdo (rotacion en X)
      case 2 :
         v = -60.0f*sin( 0.8f*t_sec*dosPi );
         *pmRotPieIzquierdo = rotate( radians(v), vec3{ 1.0,0.0,0.0 });
         break ;

      // escalado tronco (escalado no uniforme)
      case 3 :
         v = 0.1f*sin( 1.5f*t_sec*dosPi );
         *pmEscaladoTronco = scale( vec3( 1.0f+v,1.0f-v,1.0f+v ));
         break ;

      // giro cabeza (rotacion eje Y)
      case 4 :
         v = 45.0f*sin( 0.6f*t_sec*dosPi );
         *pmRotCabeza = rotate(  radians(v), vec3{0.0,1.0,0.0} );
         break ;

      // rotacion codo derecho
      case 5 :
         v = 30.0f + 25.0f*sin( 0.8f*t_sec*dosPi );
         * pmRotCodoDerecho = rotate( radians(v), vec3{-1.0,0.0,0.0 });
         break ;

      // rotacion hombro derecho
      case 6 :
         v = 20.0f + 15.0f*sin( 1.2f*t_sec*dosPi );
         * pmRotHombroDerecho = rotate( radians(v), vec3{-1.0,0.0,0.0} );
         break ;

      // traslacion codo izquierdo ( igual a codo izquierdo)
      case 7 :
         v = 30.0f + 25.0f*sin( 0.8f*t_sec*dosPi );
         *pmRotCodoIzquierdo = rotate( radians(v), vec3{-1.0,0.0,0.0} );
         break ;

      // traslacion hombro izquierdo (simétrico a hombro derecho)
      case 8 :
         v = -20.0f + 15.0f*sin( 1.2f*t_sec*dosPi );
         *pmRotHombroIzquierdo = rotate( radians(v), vec3{-1.0,0.0,0.0} );
         break ;

      default :
         cerr << "error (" << __PRETTY_FUNCTION__ << "): número de parámetros no es coherente con los casos del switch" << endl ;
         exit(1);
         break ;
   }
}



// *********************************************************************

Miembro::Miembro( const float i_r, const float i_h, Cuadroide * raiz )
{
   using namespace glm ;

   assert( 0.0 < i_r );
   assert( 0.0 < i_h );
   assert( raiz != NULL );
   assert( raiz->obj_semiesfera != NULL );
   assert( raiz->obj_cilindro != NULL );

   r = i_r ;
   h = i_h ;

   // semiesfera inferior
   NodoGrafoEscena * seInf = new NodoGrafoEscena() ;
   seInf->agregar( rotate( radians(180.0f), vec3{0.0, 0.0, 1.0} ) );
   seInf->agregar( scale( vec3( r, r, r )) );
   seInf->agregar( raiz->obj_semiesfera );
   seInf->ponerNombre("miembro: semiesf. inferior");

   // semiesfera superior
   NodoGrafoEscena * seSup = new NodoGrafoEscena() ;
   seSup->agregar( translate( vec3{ 0.0, h, 0.0 }) );
   seSup->agregar( scale( vec3( r, r, r )) );
   seSup->agregar( raiz->obj_semiesfera );
   seInf->ponerNombre("miembro: semiesf. superior");

   // cilindro
   NodoGrafoEscena * cil = new NodoGrafoEscena() ;
   cil->agregar( scale( vec3( r,h,r )));
   cil->agregar( raiz->obj_cilindro );
   seInf->ponerNombre("miembro: cilindro");

   // miembro
   agregar( seInf );
   agregar( seSup );
   agregar( cil );
   ponerNombre("miembro");
}

// *********************************************************************

MiembroArti::MiembroArti( const float i_r, const float i_h, Cuadroide * raiz, unsigned lado )
{
   using namespace glm ;
   assert( 0.0 < i_r );
   assert( 0.0 < i_h );
   assert( raiz != NULL );
   assert( raiz->obj_semiesfera != NULL );
   assert( raiz->obj_cilindro != NULL );

   r = i_r ;
   h = i_h ;

   // semiesfera inferior
   NodoGrafoEscena * seInf = new NodoGrafoEscena() ;
   seInf->agregar( rotate( radians(180.0f), vec3{0.0, 0.0, 1.0} ) );
   seInf->agregar( scale( vec3( r, r, r )) );
   seInf->agregar( raiz->obj_semiesfera );
   seInf->ponerNombre("miembro: semiesf. inferior");

   // semiesfera superior
   NodoGrafoEscena * seSup = new NodoGrafoEscena() ;
   seSup->agregar( translate( vec3{ 0.0, h, 0.0 }) );
   seSup->agregar( scale( vec3( r, r, r )) );
   seSup->agregar( raiz->obj_semiesfera );
   seSup->ponerNombre("miembro: semiesf. superior");

   // esfera central en la articulación
   NodoGrafoEscena * esfCen = new NodoGrafoEscena();
   esfCen->agregar( translate( vec3{ 0.0, h/2, 0.0 }) );
   esfCen->agregar( scale( vec3( r, r, r )) );
   esfCen->agregar( raiz->obj_esfera );
   esfCen->ponerNombre("esfera central");

   // cilindro (inferior)
   NodoGrafoEscena * cilInf = new NodoGrafoEscena() ;
   cilInf->agregar( scale( vec3( r, h/2, r )));
   cilInf->agregar( raiz->obj_cilindro );
   cilInf->ponerNombre("miembro: cilindro inferior");

   // cilindro (superior)
   NodoGrafoEscena * cilSup = new NodoGrafoEscena() ;
   cilSup->agregar( translate( vec3{ 0.0, h/2, 0.0 }));
   cilSup->agregar( scale( vec3( r, h/2, r )));
   cilSup->agregar( raiz->obj_cilindro );
   cilSup->ponerNombre("miembro: cilindro superior");

   // miembro comleto:

   // paramtro de rotacion en la unión del hombro
   // (centro de la semiesfera superior)

   agregar( translate( vec3{ 0.0, h, 0.0 }));
   const unsigned indrothom = agregar( rotate( radians(10.0f), vec3{1.0,0.0,0.0} ));
   agregar( translate( vec3{ 0.0, -h, 0.0 }));

   // objetos de la parte superior
   agregar( seSup );
   agregar( cilSup );
   agregar( esfCen );

   // rotacion en el codo
   agregar( translate( vec3{ 0.0, h/2.0f, 0.0 }));
   const unsigned indrotcodo = agregar( rotate( radians(30.0f), vec3{-1.0,0.0,0.0} ));
   agregar( translate( vec3{ 0.0, -h/2.0f, 0.0 }));

   // objetos de la parte inferior
   agregar( cilInf );
   agregar( seInf );

   if ( lado == 1 )
      ponerNombre("brazo (lado derecho)");
   else
      ponerNombre("brazo (lado izquierdo)");


   // añadir un grado de libertad en el codo:
   //float signo = lado == 1 ? +1.0 : -1.0 ;

   std::string nom1, nom2 ;

   if ( lado == 1 )
   {
      nom1 = "rotación codo derecho" ;
      nom2 = "rotación hombro derecho " ;
   }
   else
   {
      nom1 = "rotación codo izquierdo" ;
      nom2 = "rotación hombro izquierdo" ;
   }

   pmRotCodo   = leerPtrMatriz( indrotcodo );
   pmRotHombro = leerPtrMatriz( indrothom );

}
// *****************************************************************************
// clase Ojo

Ojo::Ojo(  const float p_ang_ry, Cuadroide * raiz )
{
   using namespace glm ;

   const float rad_o = 0.07 , // radio de los ojos
              ang_rx = 60.0 ; // angulo entre eje de ojo y eje Y

   ang_ry = p_ang_ry ;
   assert( raiz != NULL );
   assert( raiz->matNegroBrill != NULL );

   ponerColor( { 0.0,0.0,0.0 } );
   agregar( raiz->matNegroBrill );
   agregar( rotate( radians(ang_ry), vec3{0.0,1.0,0.0} ) );
   agregar( rotate( radians(ang_rx), vec3{1.0,0.0,0.0} ) );
   agregar( translate( vec3{ 0.0, 1.0, 0.0 }) );
   agregar( scale( vec3{ rad_o, rad_o*0.3, rad_o } ));
   agregar( raiz->obj_semiesfera );

  ponerNombre("ojo");
}
// *****************************************************************************
// clase Antena

Antena::Antena( const float p_ang_rz, Cuadroide * raiz )
{
   using namespace glm ;
   const float rad_a = 0.05 , // radio de la antena
              lon_a = 0.4 ;  // longitud de la antena (entre centros de esferas en los extremos)

   ang_rz = p_ang_rz ;
   ponerColor({ 0.0, 0.0, 0.0 });
   agregar( rotate( radians(ang_rz), vec3{ 0.0, 0.0, 1.0 }) );
   agregar( translate( vec3{ 0.0, 1.0, 0.0 }) );
   agregar( new Miembro( rad_a, lon_a, raiz ) );

   ponerNombre("antena");
} ;
// *****************************************************************************

FormacionDroides::FormacionDroides( const unsigned pnx, const unsigned pnz )
{
   ponerNombre("Formación de androides");

   nx = pnx ;  assert( nx > 0 );
   nz = pnz ;  assert( nz > 0 );

   master = new Cuadroide(); assert( master != nullptr );
   numpar = master->leerNumParametros();
   tiempo_par.resize( numpar );
   delta_par.resize( nx*nz );
   for( unsigned  i = 0 ; i < nx*nz ; i++ )
      delta_par[i] = uniform_dist( generator );  // entre 0 y 1
   
}
// ----------------------------------------------------------------------------------

FormacionDroides::~FormacionDroides()
{
   using namespace std ;
   //cout << "Invocado destructor de FormacionDroides" << endl ; 
   delete master ;
   master = nullptr ;
   tiempo_par.clear() ;
   delta_par.clear() ;
   //cout << "FIN destructor de FormacionDroides"  << endl ;
}
// ----------------------------------------------------------------------------------

unsigned FormacionDroides::leerNumParametros() const 
{
   return numpar ;
}
// ----------------------------------------------------------------------------------

void FormacionDroides::actualizarEstadoParametro( const unsigned iParam, const float t_sec )
{
   assert( iParam  < numpar );
   assert( master != nullptr );
   tiempo_par[ iParam ] = t_sec ;
   //master->actualizarEstadoParametro( iParam, t_sec );
}
// ----------------------------------------------------------------------------------

float FormacionDroides::delta_t( const unsigned ip, const unsigned ix, const unsigned iz ) const 
{
   const float fac  = (ip == 0) ? 0.2f : 2.0f ;
   return fac*delta_par[ix+iz*nx] ;
}
// ----------------------------------------------------------------------------------

void FormacionDroides::visu_master( unsigned modo )
{
   switch( modo )
   {
      case 0: master->visualizarGL() ; break ;
      case 1: master->visualizarGeomGL() ; break ;
      case 2: master->visualizarNormalesGL() ; break ;
      case 3: master->visualizarModoSeleccionGL() ; break ;
      default : assert( false );
   }   
}

// ----------------------------------------------------------------------------------

void FormacionDroides::visu_gen( unsigned modo )
{
   using namespace glm ;
   assert( master != nullptr );
   assert( modo < 4 );

   Aplicacion3D * apl   = Aplicacion3D::instancia();
   Cauce3D *      cauce = apl->cauce3D() ;  
  
   cauce->pushMM();
   for( unsigned iz = 0 ; iz < nz ; iz++ )
   {
      cauce->pushMM();
      for( unsigned ix = 0 ; ix < nx ; ix++ )
      {
         for( unsigned ip = 0 ; ip < numpar ; ip++ )
         {
            master->actualizarEstadoParametro( ip, tiempo_par[ip] + delta_t(ip,ix,iz) );
         }
         visu_master( modo );   
         cauce->compMM( glm::translate( vec3( 2.0f, 0.0f, 0.0f  )));
      }
      cauce->popMM(); 
      cauce->compMM( glm::translate( vec3( 0.0f, 0.0f, 2.0f  )));
   }
   cauce->popMM() ;
}

// ----------------------------------------------------------------------------------

void FormacionDroides::visualizarGL()
{
   visu_gen( 0 );
}

// ----------------------------------------------------------------------------------

void FormacionDroides::visualizarGeomGL(  ) 
{
   visu_gen( 1 );
}

// ----------------------------------------------------------------------------------

void FormacionDroides::visualizarNormalesGL ()  
{
   visu_gen( 2 );
}

// ----------------------------------------------------------------------------------

void FormacionDroides::visualizarModoSeleccionGL() 
{
   visu_gen( 3 );
}