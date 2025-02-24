
#include "malla-revol.h"
#include "grafo-escena.h"
#include "latapeones.h"

// identificadores (enteros) de objetos para selección
// (se pueden usar enteros cualquiera, pero estos permiten depurar mejor)

constexpr unsigned char 
   k = 254 ;
constexpr int 
   ident_peon_blanco    = CodificaBytes( k, 0, 0 ),     
   ident_peon_negro     = CodificaBytes( 0, k, 0 ),
   ident_peon_madera    = CodificaBytes( 0, 0, k ),
   ident_lata_cocacola  = CodificaBytes( k, k, 0 ),
   ident_lata_2         = CodificaBytes( k, 0, k ),
   ident_lata_3         = CodificaBytes( 0, k, k );

const char * nom_peon_blanco   = "peón blanco",
           * nom_peon_negro    = "peón negro",
           * nom_peon_madera   = "peón madera",
           * nom_lata_cocacola = "lata de CocaCola",
           * nom_lata_2        = "lata de Pepsi" ,
           * nom_lata_3        = "lata de Sprite" ;

// const VectorRGB
//       blanco ( 1.00, 1.00, 1.00, 1.0 ),
//       negro  ( 0.00, 0.00, 0.00, 1.0 ),
//       rojo   ( 1.00, 0.30, 0.20, 1.0 );

//**********************************************************************
//**
//** class MatLata
//**
//**********************************************************************

class MatLata : public Material
{
   public:
   MatLata(  const std::string & nomTextura ) ;
} ;

//----------------------------------------------------------------------

MatLata::MatLata( const std::string & nomTextura )
{
   ponerNombre( std::string("Material de lata, textura = '" + nomTextura + "'") );

   textura = new Textura( nomTextura );
   k_amb   = 0.4 ;
   k_dif   = 1.5 ;
   k_pse   = 2.5 ;
   exp_pse = 50.0 ;
}

//**********************************************************************
//**
//** class MatTapas
//**
//**********************************************************************

class MatTapas : public Material
{
   public:
   MatTapas();
} ;

//----------------------------------------------------------------------

MatTapas::MatTapas( )
{
   ponerNombre("material brillante de las tapas de las latas (sin textura)");

   //color_ad = { 1.0, 1.0, 1.0 }; // CUA, revisar
   k_amb    = 0.1 ;
   k_dif    = 1.0 ;
   k_pse    = 2.0 ;
   exp_pse  = 20.0 ;
}


//**********************************************************************
//**
//**  class Lata
//**
//**********************************************************************

class Lata : public NodoGrafoEscena
{
   public:
   Lata( const std::string & nomObjeto, const std::string & nomTextura, const int ident ) ;
   virtual ~Lata();
} ;
//---------------------------------------------------------------------

Lata::Lata( const std::string & nomObjeto, const std::string & nomTextura, const int ident )
{
   using namespace std ;
   using namespace glm ;
   
   assert( 0 < ident  );
   ponerNombre( nomObjeto );
   ponerIdentificador( ident );

   MallaRevol * mr_cue = new MallaRevolPLY( "lata-pcue.ply", 64 ),
              * mr_inf = new MallaRevolPLY( "lata-pinf.ply", 64 ),
              * mr_sup = new MallaRevolPLY( "lata-psup.ply", 64 );

   mr_cue->ponerIdentificador( -1 );
   mr_sup->ponerIdentificador( -1 );
   mr_inf->ponerIdentificador( -1 );
   // cuerpo
   auto * pc = new NodoGrafoEscena();

   pc->ponerColor( vec3( 1.0, 0.0, 0.0) );
   pc->ponerIdentificador( -1 );

   pc->agregar( new MatLata( nomTextura ) );
   pc->agregar( mr_cue ) ;
   agregar( pc );

   // tapas inferior y superior
   auto pt = new NodoGrafoEscena();
   pt->ponerIdentificador( -1 );
   pt->ponerColor( vec3(1.0,1.0,1.0));

   pt->agregar( new MatTapas() );
   pt->agregar( mr_inf ) ;
   pt->agregar( mr_sup ) ;
   agregar( pt );
}

//---------------------------------------------------------------------
Lata::~Lata()
{

}

//**********************************************************************
//**
//** class TextPeon
//**
//**********************************************************************

class TextPeon : public Textura
{
   public:
   TextPeon() ;
} ;

//----------------------------------------------------------------------

TextPeon::TextPeon()
: Textura("text-madera.jpg")
{
   modo_gen_ct = mgct_coords_objeto ;

   coefs_s[0] = 1.0 ;
   coefs_s[1] = 0.0 ;
   coefs_s[2] = 0.0 ;
   coefs_s[3] = 0.0 ;

   coefs_t[0] = 0.0 ;
   coefs_t[1] = 1.0 ;
   coefs_t[2] = 0.0 ;
   coefs_t[3] = 0.0 ;
}


//**********************************************************************
//**
//** class MatPeon
//**
//**********************************************************************

class MatPeon : public Material
{
   public:
   MatPeon( bool usatext,
            /**float r, float g, float b, **/
            float ka, float kd, float ks, float br
         ) ;

} ;
//----------------------------------------------------------------------

MatPeon::MatPeon( bool usatext, /** float r, float g, float b, **/ float ka, float kd, float ks, float br  )
{
   if ( usatext )
      textura = new TextPeon();
   else
      textura = nullptr ;

   k_amb    = ka ;
   k_dif    = kd ;
   k_pse    = ks ;
   exp_pse  = br ;
}

//**********************************************************************
//**
//**  class Peones
//**
//**********************************************************************

class Peones : public NodoGrafoEscena
{
   public:
   Peones(  ) ;
} ;


Peones::Peones()
{
   using namespace std ;
   using namespace glm ;

   const float dx = 2.4 ;   // distancia entre centros de peones
   const float h  = 2.8 ;   // rango en Y del peon: desde -1.4 hasta 1.4
   const float sn = 1.0/h ;  // factor que pone el peon con altura unidad

   // malla de revolución (creada una sola vez)
   MallaRevol * mallap = new MallaRevolPLY("peon.ply", 64 ) ;
   mallap->ponerIdentificador( -1 );

   // transformciones que afectan a los tres peones:
   agregar( translate( vec3( 0.0 ,0.0 ,  0.8 )) );
   agregar( scale    ( vec3( 0.5, 0.5,   0.5 )) );
   agregar( scale    ( vec3( sn,  sn,    sn  )) );
   agregar( translate( vec3( 0.0, h/2.0, 0.0 )) );

   // primer peon (textura)
   NodoGrafoEscena * p1 = new NodoGrafoEscena();
   p1->ponerNombre( nom_peon_madera );
   p1->ponerIdentificador( ident_peon_madera );
   p1->ponerColor( vec3(1.0,1.0,1.0));

   p1->agregar( new MatPeon( true,  /**1.0,1.0,1.0,**/  0.1, 1.0, 1.7, 60.0 ) );
   p1->agregar( mallap ) ;
   agregar( p1 );
   //cout << "creado nodo peon madera, ident == " << p1->leerIdentificador() << endl ;

   // segundo peon (blanco difuso)
   NodoGrafoEscena * p2 = new NodoGrafoEscena();
   p2->ponerNombre( nom_peon_blanco );
   p2->ponerIdentificador( ident_peon_blanco );
   p2->ponerColor({ 1.0, 0.9, 0.9 });

   p2->agregar( new MatPeon( false, /** 1.0,0.9,0.9,**/  0.2, 0.7, 0.0, 20.0 ) );
   p2->agregar( mallap ) ;
   //cout << "creado nodo peon blanco, ident == " << p2->leerIdentificador() << endl ;

   agregar( translate( vec3{ dx, 0.0, 0.0 }) );
   agregar( p2 );

   // tercer peon (negro brillante o metálico)
   NodoGrafoEscena * p3 = new NodoGrafoEscena();
   p3->ponerNombre( nom_peon_negro );
   p3->ponerIdentificador( ident_peon_negro );
   p3->ponerColor( vec3(0.1,0.1,0.1));

   p3->agregar( new MatPeon( false, /** 0.1,0.1,0.1,**/  0.1, 0.0, 0.9, 5.0 ) );
   p3->agregar( mallap ) ;

   agregar( translate( vec3( dx, 0.0f, 0.0f )));
   agregar( p3 );

   // gre
}

//**********************************************************************
// clase para contenedor del nodo con el 'Cubo24' y su material específico

NodoCubo24::NodoCubo24()
{

   ponerNombre("Nodo con cubo 24 y textura");

   Textura * textura = new Textura( "window-icon.jpg");
   agregar( new Material( textura, 0.1, 1.0, 0.5, 10.0 ) );
   agregar( new Cubo24() );
}



//**********************************************************************
//**
//**  class LataPeones
//**
//**********************************************************************

LataPeones::LataPeones(  )
{
   using namespace std ;
   using namespace glm ;

   ponerNombre("Lata y 3 peones");

   agregar( new Lata( nom_lata_cocacola, "lata-coke.jpg", ident_lata_cocacola ) );
   agregar( new Peones() );
   //agregar( new Material( 0.1, 0.2, 0.3, 10 ) );
   //agregar( new NodoCubo24() );

   // agregar las tres monedas para el examen (quitar si no conviene)

   if ( false )
   {
      auto n = new NodoGrafoEscena();
      n->agregar( scale( vec3( 0.2, 0.2, 0.2 )) );
      //n->agregar( new Moneda() );

      agregar( translate( vec3( 0.0, 0.0, 1.5 )));
      agregar( n );

      agregar( translate( vec3( 0.4, 0.0, 0.0 )));
      agregar( rotate( radians(20.0f), vec3( 1.0, 0.0, 0.0 )));
      agregar( n );

      agregar( translate( vec3( 0.4, 0.0, 0.0 )));
      agregar( rotate( radians(20.0f), vec3( 1.0, 0.0, 0.0 )));
      agregar( n );
   }
}

// ---------------------------------------------------------------------

LataPeones::~LataPeones()
{

}

// ---------------------------------------------------------------------


VariasLatasPeones::VariasLatasPeones(  const int ident_dado )
{
   using namespace std ;
   using namespace glm ;

   ponerNombre("Varias latas y 3 peones");

   // añadir peones
   agregar( new Peones() );

   // añadir latas
   NodoGrafoEscena * latas = new NodoGrafoEscena();
   latas->agregar( new Lata( nom_lata_cocacola, "lata-coke.jpg", ident_lata_cocacola ) );
   latas->agregar( translate( vec3( 1.0, 0.0, 0.0 )));
   latas->agregar( new Lata( nom_lata_2, "lata-pepsi.jpg", ident_lata_2 ) );

   // la última lata la podemos quitar o poner
   if ( true )
   {
     latas->agregar( translate( vec3( 1.0,0.0,0.0 )));
     //latas->agregar( new Lata( nom_lata_3, "../recursos/imgs/lata-sprite.jpg", ident_lata_3 ) );
     latas->agregar( new Lata( nom_lata_3, "window-icon.jpg", ident_lata_3 ) );
     
   }
   agregar( latas );
}

// ---------------------------------------------------------------------

VariasLatasPeones::~VariasLatasPeones()
{

}

//**********************************************************************
//**
//**  class ColFuentesLataPeones
//**
//**********************************************************************

ColFuentesLataPeones::ColFuentesLataPeones()
{
   using namespace glm ;

   const float f0 = 0.9, f1 = 0.5 ;
   insertar( new FuenteLuz( +45.0, 60.0,  vec3 { f0, f0,     f0,    } ) );
   insertar( new FuenteLuz( -70.0, -30.0, vec3 { f1, f1*0.5, f1*0.5 } ) );

}
