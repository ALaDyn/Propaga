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

#define major_version 1
#define minor_version 0
#define fix_release   3
#define release_date  "June 16, 2016"
#define latest_commit "clean up emittance and envelope definitions"

//#define ENABLE_DEBUG
//#define USE_SPACECHARGE // incompleto, funziona solo per singolo task, tra diversi task le particelle non si vedono

#ifndef _GNU_SOURCES
#define _GNU_SOURCES
#endif

#define C                      2.99792458e+10         // speed of light [cm/s]
#define ME_G                   9.10938291E-28         // electron mass [g]
#define MP_G                   1.6726231e-24          // proton mass [g]
#define MP_KG                  1.6726231e-27          // proton mass [kg]
#define MP_MEV                 938.272013             // proton mass [MeV/c^2]
//#define CHARGE_CGS           4.80320425e-10         // electron charge [statC]  [Sinigardi]
#define CHARGE_CGS             4.803262e-10           // electron charge [statC]  [Turchetti]
#define CHARGE_SI              1.602176565e-19        // electron charge [C]
#define FROM_TESLA_TO_GAUSS    1.0e+4
//#define DA_ERG_A_MEV         6.2415097523028e+5     // [Servizi]
#define DA_ERG_A_MEV           6.241509744512e+5      // [Sinigardi]
#define FROM_VOLT_TO_STATVOLT  3.335640951982e-3      // 1 statvolt = 299.792458 volts.
#define E0                     8.85418781762e-12      // epsilon0
#define KC                     1.0                    // Coulomb's constant
//#define KC                   (1.0/(4*M_PI*E0))      // Coulomb's constant
//#define KC                   0.07957747154          // Coulomb's constant
//#define RP                   ( (1.0/(4*M_PI*E0)) * ((CHARGE*CHARGE)/(MP*C*C)) )  
#define RP                     0.877551e-13           // classical proton radius (cm)

/*********************************************************
The cgs unit of force is the dyne. From Newtons second law,
1 dyne = 1 g*cm/s2.
The cgs unit of work is the erg, since work is energy. From the work equation
1 erg = 1 dyne*cm = 1 g*cm2/s2.
This gives the erg in terms of fundamental units.
Joule: The joule (J) is the analog of the erg in the SI system, so
1 J = 1 kg*m2/s2
We easily connect joules and ergs.
1 kg*m2/s2 * 10^3 g/kg * 10^4 cm2/m2 = 10^7 g*cm2/s2 = l0^7 erg
Then we can calculate:
1 eV = e * 1 J/C   (since that Volt=Joule/Coulomb)
= 1.602176 * 10^-19 J
Inverting we obtain
1 J = 6.24150974 * 10^18 eV
--> 10^7 erg = 6.24150974 * 10^18 eV
--> 1 erg = 6.24150974 * 10^5 MeV
*********************************************************
In CGS the coulomb law states that
F=q1*q2/r^2
So 1 statC = g^(1/2) * cm^(3/2) * s^(-1)
*********************************************************/

#define N_DIMENSIONI_SPAZIO_FASI   6

// lettere ancora disponibili: A B E G H L N U Y Z

#define N_TIPI_MAGNETICI                     15
#define N_PARAMETRI_LATTICINO                 7
#define N_PARAMETRI_LATTICINO_LETTI_DA_INPUT  6

#define DRIFT               'O'
#define SOLENOID            'S'
#define SOLENOID_FF         'X'
#define SOLENOID_SMOOTH     'M'
#define FOCUSING            'F'
#define DEFOCUSING          'D'
#define IRIS                'I'
#define IRIS_NEW            'J'
#define IRIS_X              'K'
#define CHICANE_PRE         'P'
#define CHICANE_POST        'Q'
#define CHICANE_SELECT      'R'
#define CHICANE_SELECT_NEW  'T'
#define RF_CAVITY_TM_ASTRA  'V'
#define RF_CAVITY           'W'
#define _DRIFT_              0
#define _SOLENOID_           1
#define _SOLENOID_FF_        5
#define _SOLENOID_SMOOTH_    7
#define _FOCUSING_           2
#define _DEFOCUSING_         3
#define _IRIS_               4
#define _IRIS_NEW_           12
#define _IRIS_X_             6
#define _CHICANE_PRE_        8
#define _CHICANE_POST_       9
#define _CHICANE_SELECT_     10
#define _CHICANE_SELECT_NEW_ 13
#define _RF_CAVITY_TM_ASTRA_ 11
#define _RF_CAVITY_          14
#define gamma_rel_inv(x)     (1.0 / sqrt(1.0 + x[3]*x[3]+x[4]*x[4]+x[5]*x[5]))
#define gamma_rel(x)         (sqrt(1.0 + x[3]*x[3]+x[4]*x[4]+x[5]*x[5]))     // gamma relativistico definito in funzione dei gamma*beta usati nei file

#if defined(CINECA)
#define nullptr NULL
//#define __GXX_EXPERIMENTAL_CXX0X__
#endif
