/******************************************************************************************************
*             Copyright 2010-2016 Stefano Sinigardi, Graziano Servizi, Giorgio Turchetti              *
******************************************************************************************************/

/******************************************************************************************************
*  This file is part of Propaga.                                                                      *
*                                                                                                     *
*  Propaga is free software: you can redistribute it and/or modify                                    *
*  it under the terms of the GNU General Public License as published by                               *
*  the Free Software Foundation, either version 3 of the License, or                                  *
*  (at your option) any later version.                                                                *
*                                                                                                     *
*  Propaga is distributed in the hope that it will be useful,                                         *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of                                     *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                      *
*  GNU General Public License for more details.                                                       *
*                                                                                                     *
*  You should have received a copy of the GNU General Public License                                  *
*  along with Propaga.  If not, see <http://www.gnu.org/licenses/>.                                   *
******************************************************************************************************/


#pragma once

#include "definitions.h"
#include "particle.h"

 /* la classe  base "Magnetic_element": tutti gli oggetti magnetici "concreti"               *
  * sono sue elementarissime struct figlie; cio' consente:                                   *
  * A) di aggiungerne altri tipi se ne vuole senza cambiare l'architettura del programma;    *
  * B) di fornire una funzione campo vettoriale diversa per ogni tipo                        */

class Magnetic_element
{
protected:
  double parameters[N_PARAMETRI_LATTICINO_LETTI_DA_INPUT];

public:
  // il metodo pubblico set_values assegna valori alle variabili membro
  void set_values(double *);

  // il metodo pubblico get_values restituisce  i valori protected delle variabili
  double get_values(int);

  /* un puntatore alla class Particle, di cui si ammirera' l'utilita' */
  static Particle * particle;

};

struct Drift : public Magnetic_element
{
  Drift();
  static void field(double *, double *, double*, double *, int);
};

struct Solenoid : public Magnetic_element
{
  Solenoid();
  static void field(double *, double *, double*, double *, int);
};

struct Focusing : public Magnetic_element
{
  Focusing();
  static void field(double *, double *, double*, double *, int);
};

struct Defocusing : public Magnetic_element
{
  Defocusing();
  static void field(double *, double *, double*, double *, int);
};

struct Iris : public Magnetic_element
{
  Iris();
  static void field(double *, double *, double*, double *, int);
};

struct Iris_New : public Magnetic_element
{
  Iris_New();
  static void field(double *, double *, double*, double *, int);
};

struct Iris_X : public Magnetic_element
{
  Iris_X();
  static void field(double *, double *, double*, double *, int);
};

struct Solenoid_FF : public Magnetic_element
{
  Solenoid_FF();
  static void field(double *, double *, double*, double *, int);
};

struct Solenoid_SMOOTH : public Magnetic_element
{
  Solenoid_SMOOTH();
  static void field(double *, double *, double*, double *, int);
};

struct Chicane_PRE : public Magnetic_element
{
  Chicane_PRE();
  static void field(double *, double *, double*, double *, int);
};

struct Chicane_SELECT : public Magnetic_element
{
  Chicane_SELECT();
  static void field(double *, double *, double*, double *, int);
};

struct Chicane_SELECT_New : public Magnetic_element
{
  Chicane_SELECT_New();
  static void field(double *, double *, double*, double *, int);
};

struct Chicane_POST : public Magnetic_element
{
  Chicane_POST();
  static void field(double *, double *, double*, double *, int);
};

struct RF_Cavity : public Magnetic_element
{
  RF_Cavity();
  static void field(double *, double *, double*, double *, int);
};

struct RF_Cavity_tm_astra : public Magnetic_element
{
  RF_Cavity_tm_astra();
  static void field(double *, double *, double*, double *, int);
};

struct Azzeratore : public Magnetic_element
{
  Azzeratore();
  static void field(double*);
};

struct SpaceCharge : public Magnetic_element
{
  SpaceCharge();
  static void field(double *, double *, double*, double *, double *, int);
};


