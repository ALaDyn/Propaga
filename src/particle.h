/******************************************************************************************************
*             Copyright 2010-2017 Stefano Sinigardi, Graziano Servizi, Giorgio Turchetti              *
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


#ifndef PARTICLE_H
#define PARTICLE_H

#include "definitions.h"
#include <cstring>
#include <cmath>

class Particle
{
  /* punto di fase occupato dalla particle */
  double xfase[PHASE_SPACE_SIZE];

  /* massa, carica e peso delle particelle */
  double mass, charge, weight;

  /* codice particle - cfr. Fluka particle numbers http://www.fluka.org/fluka.php?id=man_onl&sub=7 */
  int particle_code;

  /* la variabile booleana assorbita e' inizializzata a false dal costruttore default  *
   * la check_if_absorbed serve per verificare quando sia stata appena assorbita       */
  bool absorbed;
  int check_if_absorbed;

  /* la variabile ordinale contiene il numero ordinale appunto della particle in questione */
  int ordinal;

  /* le variabili booleane ffi e fff servono per i fringe fields "kick" tipo francoforte */
  bool ffi, fff;

  /* la variabile bool dumped_z serve per capire se la particle e' gia' stata scritta nel file "outz" */
  bool dumped_z;

public:

  /* il metodo absorbe pone true la variabile "absorbed" */
  void absorbe();

  void absorbe_when_reading();

  /* il metodo booleano is_absorbed restituisce il valore di  "absorbed" */
  bool is_absorbed();

  int just_absorbed();

  double get_mass();

  void set_mass(double);

  int get_particle_code();

  void set_particle_code(int);

  double get_charge();

  void set_charge(double);

  double get_weight();

  void set_weight(double);

  void set_weight_fp(float *);

  /* il metodo set_ordinal serve per impostare appunto il numero ordinale di una particle               *
   * all'inizio dell'elaborazione. La funzione e' chiamata dentro Lattice::read_file e dovrebbe esser   *
   * fatta in modo tale che la prima particle del file sia la #1 (e non la #0)                          */
  void set_ordinal(int);

  /* il metodo get_ordinal restituisce il numero ordinale della particle nel bunch */
  int get_ordinal();

  /* il metodo move_far_far_away fa quello che dice, come appare evidente;           *
   * una particle  "schiaffata_a_infinito"   non dara' contributi nemmeno se si      *
   * accende l'interazione coulombiana (quindi bisogna pensarci)                     *
   * QUESTA VARIANTE MANDA LA PARTICELLA AD INFINITO ANCHE LUNGO Z, PONENDOLA        *
   * QUINDI SICURAMENTE FUORI DALLA BEAMLINE.                                        */
  void move_far_far_away();

  /* il metodo move_far_far_away_same_z, diversamente dal precedente, non manda
   ad infinito anche la coordinata z ddella particle; resta inoltre conservato l'impulso
   lungo z ad essa associato */
  void move_far_far_away_same_z();

  /* costruttore default */
  Particle();

  /* costruttore parametrico (non usato, al momento) */
  Particle(double, double, double, bool, int, bool, bool);

  /* il metodo set_values esegue la stessa funzione del costruttore parametrico
   (eccetto l'azzeramento del punto di fase)
   DOPO che le particelle siano gia' state create */
  void set_values(double, double, double, bool, int, bool, bool);

  /* il metodo set_phase_space assegna il contenuto dell'argomento x
   al punto di fase della particle */
  void set_phase_space(double *);

  void set_phase_space_x(float *);

  void set_phase_space_px(float *);

  /* il metodo get_phase_space restituisce la i-esima componente del punto
   di fase della particle */
  double get_phase_space(int);

  /* ... e questo suo overload restituisce l'intero array */
  double * get_phase_space();

  /* in v[i] vengono memorizzate le velocita' delle particelle in cm / s */
  double get_speed_cm_s(int);

  double get_energy_MeV();

  bool get_ffi();

  bool get_fff();

  void set_ffi(bool);

  void set_fff(bool);

  bool get_dumped_z();

  void set_dumped_z(bool);

};


#endif

