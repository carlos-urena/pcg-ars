#pragma once

#include "grafo-escena.h"

// *****************************************************************************

class LataPeones : public NodoGrafoEscena
{
   public:
   LataPeones( ) ;

   virtual ~LataPeones() ;


} ;

// *****************************************************************************

class VariasLatasPeones : public NodoGrafoEscena
{
   public:
   VariasLatasPeones( const int ident_dado ) ;


   virtual ~VariasLatasPeones() ;

   // --------------------------------------------------------------------------
   private:

} ;
// *****************************************************************************


class ColFuentesLataPeones : public ColFuentesLuz
{
   public:
      ColFuentesLataPeones();
} ;


//**********************************************************************
// clase para contenedor del nodo con el 'Cubo24' y su material específico

class NodoCubo24 : public NodoGrafoEscena
{
  public:
    NodoCubo24() ;
} ;


