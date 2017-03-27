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


#include "particle.h"


void Particle::absorbe() {
  if (!absorbed) {
    absorbed = true;
    check_if_absorbed = 1;
  }
}

void Particle::absorbe_when_reading() {
  absorbed = true;
  check_if_absorbed = 2;
}

/* il metodo booleano is_absorbed restituisce il valore di  "absorbed" */
bool Particle::is_absorbed() {
  return absorbed;
}

int Particle::just_absorbed() {
  if (check_if_absorbed == 1) {
    check_if_absorbed++;
    return 1;
  }
  else return 0;
}

double Particle::get_mass() {
  return mass;
}

void Particle::set_mass(double mp) {
  mass = mp;
}

int Particle::get_particle_code() {
  return particle_code;
}

void Particle::set_particle_code(int pc) {
  particle_code = pc;
}

double Particle::get_charge() {
  return charge;
}

void Particle::set_charge(double ch) {
  charge = ch;
}

double Particle::get_weight() {
  return weight;
}

void Particle::set_weight(double w) {
  weight = w;
}

void Particle::set_weight_fp(float* w) {
  weight = (double)*w;
}

/* il metodo set_ordinal serve per impostare appunto il numero ordinale di una particle               *
 * all'inizio dell'elaborazione. La funzione e' chiamata dentro Lattice::read_file e dovrebbe esser   *
 * fatta in modo tale che la prima particle del file sia la #1 (e non la #0)                          */
void Particle::set_ordinal(int ord) {
  ordinal = ord;
}

/* il metodo get_ordinal restituisce il numero ordinale della particle nel bunch */
int Particle::get_ordinal() {
  return ordinal;
}

/* il metodo move_far_far_away fa quello che dice, come appare evidente;           *
 * una particle  "schiaffata_a_infinito"   non dara' contributi nemmeno se si      *
 * accende l'interazione coulombiana (quindi bisogna pensarci)                     *
 * QUESTA VARIANTE MANDA LA PARTICELLA AD INFINITO ANCHE LUNGO Z, PONENDOLA        *
 * QUINDI SICURAMENTE FUORI DALLA BEAMLINE.                                        */
void Particle::move_far_far_away() {
  xfase[0] = xfase[1] = xfase[2] = 1.e+308;     // x, y, z all'infinito
  xfase[3] = xfase[4] = xfase[5] = 0.0;         // momenti nulli: la particle e' ferma all'infinito
}

/* il metodo move_far_far_away_same_z, diversamente dal precedente, non manda              *
 * ad infinito anche la coordinata z ddella particle; resta inoltre conservato l'impulso   *
 * lungo z ad essa associato                                                               */
void Particle::move_far_far_away_same_z() {
  xfase[0] = xfase[1] = 1.e+308;
  xfase[3] = xfase[4] = 0.0;
  // NOTA BENE: la particle conserva pero' sia z che la velocita' lungo z
}

/* costruttore default */
Particle::Particle() {
  absorbed = false;
  check_if_absorbed = 0;
  ffi = false;
  fff = false;
  dumped_z = false;
}

/* costruttore parametrico (non usato, al momento) */
Particle::Particle(double m, double c, double w, bool status, int ord, bool ffiniziale, bool fffinale) {
  mass = m;
  charge = c;
  weight = w;
  memset((void *)xfase, 0, PHASE_SPACE_SIZE*sizeof(double));
  absorbed = status;
  check_if_absorbed = 0;
  ordinal = ord;
  ffi = ffiniziale;
  fff = fffinale;
  dumped_z = false;
}

/* il metodo set_values esegue la stessa funzione del costruttore parametrico   *
 * (eccetto l'azzeramento del punto di fase)                                    *
 * DOPO  che le particelle siano gia' state create                              */
void Particle::set_values(double m, double c, double w, bool status, int ord, bool ffiniziale, bool fffinale) {
  mass = m;
  charge = c;
  weight = w;
  absorbed = status;
  ordinal = ord;
  ffi = ffiniziale;
  fff = fffinale;
}

/* il metodo set_phase_space assegna il contenuto dell'argomento x   *
 * al punto di fase della particle                                   */
void Particle::set_phase_space(double *x) {
  for (int i = 0; i < PHASE_SPACE_SIZE; i++) xfase[i] = x[i];
}

void Particle::set_phase_space_x(float *x) {
  for (int i = 0; i < 3; i++) xfase[i] = (double)x[i];
}

void Particle::set_phase_space_px(float *x) {
  for (int i = 3; i < 6; i++) xfase[i] = (double)x[i];
}

/* il metodo get_phase_space restituisce la i-esima componente del punto  *
 * di fase della particle                                                 */
double Particle::get_phase_space(int i) {
  return xfase[i];
}

/* ... e questo suo overload restituisce l'intero array */
double * Particle::get_phase_space() {
  return xfase;
}

/* in v[i] vengono memorizzate le velocita' delle particelle in cm / s */
double Particle::get_speed_cm_s(int i) {
  double v, g = gamma_rel(xfase);
  if (i > 2 && i < 6 && g > 0.0) v = xfase[i] * C / g;
  else v = 0.0;
  return v;
}

double Particle::get_energy_MeV() {
  double E, g = gamma_rel(xfase);
  E = (FROM_ERG_TO_MEV)* (g - 1.0) * mass * C * C;        // nb: non usare la MP_MEV ma la MP (grammi)!!!!!
  return E;
}

bool Particle::get_ffi() {
  return ffi;
}

bool Particle::get_fff() {
  return fff;
}

void Particle::set_ffi(bool ffiniziale) {
  ffi = ffiniziale;
}

void Particle::set_fff(bool fffinale) {
  fff = fffinale;
}

bool Particle::get_dumped_z() {
  return dumped_z;
}

void Particle::set_dumped_z(bool status_dumped) {
  dumped_z = status_dumped;
}

