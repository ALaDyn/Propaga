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


#include "magnetic_element.h"
#include <cmath>

Particle * Magnetic_element::particle;

/* la classe  base "Magnetic_element": tutti gli oggetti magnetici "concreti"   *
 * sono sue elementarissime struct figlie; cio' consente:                       *
 * A) di aggiungerne magnetic_element_number tipi se ne vuole senza cambiare    *
 * l'architettura del programma;                                                *
 * B) di fornire una funzione campo vettoriale diversa per ogni tipo            */

void Magnetic_element::set_values(double * values) {
  for (int i = 0; i < NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT_READ_FROM_INPUT; i++) parameters[i] = values[i];
}

// il metodo pubblico get_values restituisce  i valori protected delle variabili
double Magnetic_element::get_values(int i) {
  if (i < NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT_READ_FROM_INPUT) return parameters[i];
  else
  {
    return -123456.7890;
  }
}


/**************************************************************************************
 * implementazione dei metodi static per i campi vettoriali di ciascuna struct figlia *
 **************************************************************************************/
Azzeratore::Azzeratore() {}
void Azzeratore::field(double *Phi) {
  Phi[0] = 0.0;
  Phi[1] = 0.0;
  Phi[2] = 0.0;
  Phi[3] = 0.0;
  Phi[4] = 0.0;
  Phi[5] = 0.0;
}


SpaceCharge::SpaceCharge() {}
void SpaceCharge::field(double *Xvec, double *Param, double *t, double *Phi, double *XvecFromZero, int Index) {
  double dx, xi, x, fx;
  double dy, yi, y, fy;
  double dz, zi, z, fz;
  double r2, r, theta, phi, force;
  Particle *particellaInteragente = particle + Index;
  Particle *particellaConCuiInteragisce = particle;

  //  x = particellaInteragente->get_phase_space(0); // non vanno bene queste perche' non sono le posizioni integrate step-by-step dal RK4
  //  y = particellaInteragente->get_phase_space(1);
  //  z = particellaInteragente->get_phase_space(2);
  x = Xvec[0];
  y = Xvec[1];
  z = Xvec[2];
  double mp = particellaInteragente->get_mass();
  double chi;
  double ch = particellaInteragente->get_charge();

  double omega = (1.0) / (mp*C*C);

  int nparticelle = (int)Param[0];

  for (int i = 0; i < nparticelle; i++) {
    if (i == Index) {
      particellaConCuiInteragisce++;
      continue;
    }
    chi = particellaConCuiInteragisce->get_charge();
    //    xi = particellaConCuiInteragisce->get_phase_space(0);
    //    yi = particellaConCuiInteragisce->get_phase_space(1);
    //    zi = particellaConCuiInteragisce->get_phase_space(2);
    xi = XvecFromZero[0 + i*PHASE_SPACE_SIZE];
    yi = XvecFromZero[1 + i*PHASE_SPACE_SIZE];
    zi = XvecFromZero[2 + i*PHASE_SPACE_SIZE];
    dx = x - xi;
    dy = y - yi;
    dz = z - zi;
    r2 = dx*dx + dy*dy + dz*dz;
    if (r2 < (RP*RP)) r2 = RP*RP;
    r = sqrt(r2);
    theta = acos(dz / r);
    if (dx > 0) phi = atan(dy / dx);
    else phi = M_PI;
    force = KC * omega * ch*chi / r2;
    fx = -cos(phi) * sin(theta);
    fy = -sin(phi) * sin(theta);
    fz = -cos(theta);

    Phi[3] += force * fx; // da sistemare
    Phi[4] += force * fy; // da sistemare
    Phi[5] += force * fz; // da sistemare
    particellaConCuiInteragisce++;
  }
}


Drift::Drift() {}
void Drift::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  double gamma = gamma_rel(Xvec);

  Phi[0] = Xvec[3] / gamma; // keep it = and not += or many drifts in the lattice will increase Phi!!
  Phi[1] = Xvec[4] / gamma;
  Phi[2] = Xvec[5] / gamma;
  Phi[3] += 0.;
  Phi[4] += 0.;
  Phi[5] += 0.;
}


Solenoid::Solenoid() {}
void Solenoid::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*
  Param[] = { Param[0] = numero totale particelle, Param[1] = fine solenoide (z), Param[2] = B0 solenoide (in Tesla),
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio solenoide (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  omega0 e' la frequenza di Larmor diviso c, ovvero "eB / mc^2".  */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();

    double omega0 = ch*(Param[2] * FROM_TESLA_TO_GAUSS) / (mp*C*C); // in cm^-1

    Phi[3] += omega0*Phi[1];
    Phi[4] += -omega0*Phi[0];
    Phi[5] += 0;
  }
}


Focusing::Focusing() {}
void Focusing::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*
  Param[] = { Param[0] = numero totale particelle, Param[1] = fine quadrupolo (z), Param[2] = gradiente quadrupolo (in Tesla/m),
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio quadrupolo (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  omega1 e' eB_1 / mc^2 ovvero frequenza spaziale su lunghezza per il quadrupolo F.  */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();

    double omega1 = ch*(Param[2] * FROM_TESLA_TO_GAUSS*1e-2) / (mp*C*C); // in cm^-2 (il 10^-2 converte da metri^-1 a cm^-1, perche' il campo e' in T/m!!)

    Phi[3] += omega1*(-Phi[2] * Xvec[0]);
    Phi[4] += omega1*(Phi[2] * Xvec[1]);
    Phi[5] += omega1*(Phi[0] * Xvec[0] - Phi[1] * Xvec[1]);
  }
}


Defocusing::Defocusing() {}
void Defocusing::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*
  Param[] = { Param[0] = numero totale particelle, Param[1] = fine quadrupolo (z), Param[2] = gradiente quadrupolo (in Tesla/m),
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio quadrupolo (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  omega1 e' eB_1 / mc^2 ovvero frequenza spaziale su lunghezza per il quadrupolo F.  */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();

    double omega1 = -ch*(Param[2] * FROM_TESLA_TO_GAUSS*1e-2) / (mp*C*C); // in cm^-2 (il 10^-2 converte da metri^-1 a cm^-1, perche' il campo e' in T/m!!)

    Phi[3] += omega1*(-Phi[2] * Xvec[0]);
    Phi[4] += omega1*(Phi[2] * Xvec[1]);
    Phi[5] += omega1*(Phi[0] * Xvec[0] - Phi[1] * Xvec[1]);
  }
}


Iris::Iris() {}
void Iris::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*
  Xvec[]  = {x,y,z,ux,uy,uz}        dove ux = px / mc
  Param[] = { Param[0] = n_totale_particelle, Param[1] = fine iride (z), Param[2] = raggio_iride,
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio iride (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  Phi[] = { x',y', z',ux', uy',uz' }  dove ' identifica la derivata temporale ( es: (x)' = d(x) / dt )

  NB: questo iride non e' corretto perche' se gli step temporali sono troppo grandi, si rischia che l'iride non
  venga nemmeno visto durante l'evoluzione. Inoltre, le particelle sentono questo campo ad ogni step, anche a quelli
  intermedi del Runge-Kutta (non fisici), quindi e' possibile che vengano filtrate erroneamente alcune particelle */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double r_particella_2 = Xvec[0] * Xvec[0] + Xvec[1] * Xvec[1];
    double r_iride_2 = Param[2] * Param[2];

    if (r_particella_2 > r_iride_2) particle[Index].absorbe();    // non serve controllare che non sia gia' assorbita perche' lo controlla la funz absorbe()
  }
}


Iris_New::Iris_New() {}
void Iris_New::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*
  Xvec[]  = {x,y,z,ux,uy,uz}        dove ux = px / mc
  Param[] = { Param[0] = n_totale_particelle, Param[1] = fine iride (z), Param[2] = raggio_iride,
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio iride (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  Phi[] = { x',y', z',ux', uy',uz' }  dove ' identifica la derivata temporale ( es: (x)' = d(x) / dt )

  NB: questo iride dovrebbe funzionare meglio, siccome funziona solo sugli step interi del Runge-Kutta
  ed inoltre interpola la posizione precedente con l'attuale nel verificare che non sia stato "saltato"
  un iride nel frattempo    */

  double inizio = Param[4];
  double fine = Param[1];

  size_t phase_space_size = (size_t)Param[0] * PHASE_SPACE_SIZE;
  double posizione_x_attuale = Xvec[0 + 5 * phase_space_size];
  double posizione_y_attuale = Xvec[1 + 5 * phase_space_size];
  double posizione_z_attuale = Xvec[2 + 5 * phase_space_size];
  double posizione_x_precedente = Xvec[0 + 4 * phase_space_size];
  double posizione_y_precedente = Xvec[1 + 4 * phase_space_size];
  double posizione_z_precedente = Xvec[2 + 4 * phase_space_size];
  double posizione_x_interpolata, posizione_y_interpolata, r_particella_2, r_iride_2;

  // se e' passata per l'ingresso
  if (posizione_z_precedente <= inizio && posizione_z_attuale >= inizio) {
    posizione_x_interpolata = ((inizio - posizione_z_precedente) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_attuale - ((inizio - posizione_z_attuale) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_precedente;
    posizione_y_interpolata = ((inizio - posizione_z_precedente) / (posizione_z_attuale - posizione_z_precedente)) * posizione_y_attuale - ((inizio - posizione_z_attuale) / (posizione_z_attuale - posizione_z_precedente)) * posizione_y_precedente;
    r_particella_2 = posizione_x_interpolata * posizione_x_interpolata + posizione_y_interpolata * posizione_y_interpolata;
    r_iride_2 = Param[2] * Param[2];

    if (r_particella_2 > r_iride_2) particle[Index].absorbe();    // non serve controllare che non sia gia' assorbita perche' lo controlla la funz absorbe()
  }

  // se e' dentro
  if (posizione_z_precedente >= inizio && posizione_z_attuale <= fine) {
    // posizione_x_interpolata = posizione_x_precedente + (posizione_x_attuale - posizione_x_precedente) / (posizione_z_attuale - posizione_z_precedente) * (inizio - posizione_z_precedente);
    // posizione_y_interpolata = posizione_y_precedente + (posizione_y_attuale - posizione_y_precedente) / (posizione_z_attuale - posizione_z_precedente) * (inizio - posizione_z_precedente);
    // r_particella_2 = posizione_x_interpolata * posizione_x_interpolata + posizione_y_interpolata * posizione_y_interpolata;
    // r_particella_2 = Xvec[0]*Xvec[0]+Xvec[1]*Xvec[1];    // non va bene nemmeno questa, meglio la successiva che controlla solo su posizioni fisiche e non anche quelle numeriche del RK4

    r_particella_2 = posizione_x_attuale * posizione_x_attuale + posizione_y_attuale * posizione_y_attuale;
    r_iride_2 = Param[2] * Param[2];

    if (r_particella_2 > r_iride_2) particle[Index].absorbe();    // non serve controllare che non sia gia' assorbita perche' lo controlla la funz absorbe()
  }

  // se e' uscita
  if (posizione_z_precedente <= fine   && posizione_z_attuale >= fine) {
    posizione_x_interpolata = ((fine - posizione_z_precedente) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_attuale - ((fine - posizione_z_attuale) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_precedente;
    posizione_y_interpolata = ((fine - posizione_z_precedente) / (posizione_z_attuale - posizione_z_precedente)) * posizione_y_attuale - ((fine - posizione_z_attuale) / (posizione_z_attuale - posizione_z_precedente)) * posizione_y_precedente;
    r_particella_2 = posizione_x_interpolata * posizione_x_interpolata + posizione_y_interpolata * posizione_y_interpolata;
    r_iride_2 = Param[2] * Param[2];

    if (r_particella_2 > r_iride_2) particle[Index].absorbe();    // non serve controllare che non sia gia' assorbita perche' lo controlla la funz absorbe()
  }
}


Iris_X::Iris_X() {}
void Iris_X::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*
  Xvec[]  = {x,y,z,ux,uy,uz}        dove ux = px / mc
  Param[] = { Param[0] = n_totale_particelle, Param[1] = fine iride (z), Param[2] = inizio iride (x),
  Param[3] = fine iride (x), Param[4] = inizio iride (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  Phi[]   = {x',y',z',ux',uy',uz'}  dove ' identifica la derivata temporale ( es: (x)' = d(x) / dt )
  */

  double inizio_x = Param[2];
  double fine_x = Param[3];

  double inizio_z = Param[4];
  double fine_z = Param[1];

  size_t phase_space_size = (size_t)Param[0] * PHASE_SPACE_SIZE;
  double posizione_x_attuale = Xvec[0 + 5 * phase_space_size];
  double posizione_z_attuale = Xvec[2 + 5 * phase_space_size];
  double posizione_x_precedente = Xvec[0 + 4 * phase_space_size];
  double posizione_z_precedente = Xvec[2 + 4 * phase_space_size];
  double posizione_z_interpolata;

  if (posizione_x_precedente >= inizio_x && posizione_x_attuale <= inizio_x) { // nb: gli estremi sono cosi' stranamente invertiti perche' un campo B positivo per l'elemento P devia le particelle in x negativi!
    posizione_z_interpolata = ((inizio_x - posizione_x_precedente) / (posizione_x_attuale - posizione_x_precedente)) * posizione_z_attuale - ((inizio_x - posizione_x_attuale) / (posizione_x_attuale - posizione_x_precedente)) * posizione_z_precedente;

    if (posizione_z_interpolata < inizio_z || posizione_z_interpolata > fine_z) // || posizione_y_interpolata < inizio_y || posizione_y_interpolata > fine_y
      particle[Index].absorbe();    // non serve controllare che non sia gia' assorbita perche' lo controlla la funz absorbe()
  }

  if (posizione_x_precedente <= inizio_x && posizione_x_attuale >= fine_x)  // nb: gli estremi sono cosi' stranamente invertiti perche' un campo B positivo per l'elemento P devia le particelle in x negativi!
    if (posizione_z_attuale < inizio_z || posizione_z_attuale > fine_z) // || posizione_y_interpolata < inizio_y || posizione_y_interpolata > fine_y
      particle[Index].absorbe();    // non serve controllare che non sia gia' assorbita perche' lo controlla la funz absorbe()

  if (posizione_x_precedente >= fine_x && posizione_x_attuale <= fine_x) { // nb: gli estremi sono cosi' stranamente invertiti perche' un campo B positivo per l'elemento P devia le particelle in x negativi!
    posizione_z_interpolata = ((fine_x - posizione_x_precedente) / (posizione_x_attuale - posizione_x_precedente)) * posizione_z_attuale - ((fine_x - posizione_x_attuale) / (posizione_x_attuale - posizione_x_precedente)) * posizione_z_precedente;

    if (posizione_z_interpolata < inizio_z || posizione_z_interpolata > fine_z) // || posizione_y_interpolata < inizio_y || posizione_y_interpolata > fine_y
      particle[Index].absorbe();    // non serve controllare che non sia gia' assorbita perche' lo controlla la funz absorbe()
  }
}


Solenoid_FF::Solenoid_FF() {}
void Solenoid_FF::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine kick solenoide (z), Param[2] = B0 solenoide (in Tesla),
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio kick solenoide (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento } */

  double inizio = Param[4];
  double fine = Param[1];
  size_t phase_space_size = (size_t)Param[0] * PHASE_SPACE_SIZE;

  double mp = particle->get_mass();
  double ch = particle->get_charge();

  double omega = ch*(Param[2] * FROM_TESLA_TO_GAUSS) / (mp*C*C*2.0);
  double omega0 = ch*(Param[2] * FROM_TESLA_TO_GAUSS) / (mp*C*C); // in cm^-1

  double posizione_z_attuale = Xvec[2 + 5 * phase_space_size];
  double posizione_z_precedente = Xvec[2 + 4 * phase_space_size];

  double vx, vy, vz;
  vx = Xvec[3];
  vy = Xvec[4];
  vz = Xvec[5];

  if (posizione_z_precedente <= inizio && posizione_z_attuale >= inizio && !particle[Index].get_ffi()) {
    Xvec[3] += omega*Xvec[1];
    Xvec[4] -= omega*Xvec[0];
    Xvec[5] = sqrt(vx*vx + vy*vy + vz*vz - Xvec[3] * Xvec[3] - Xvec[4] * Xvec[4]);
    if (vz < 0) Xvec[5] = -Xvec[5];
    particle[Index].set_ffi(true);
  }

  vz = Xvec[5];

  if (posizione_z_precedente <= fine && posizione_z_attuale >= fine && !particle[Index].get_fff()) {
    Xvec[3] -= omega*Xvec[1];
    Xvec[4] += omega*Xvec[0];
    Xvec[5] = sqrt(vx*vx + vy*vy + vz*vz - Xvec[3] * Xvec[3] - Xvec[4] * Xvec[4]);
    if (vz < 0) Xvec[5] = -Xvec[5];
    particle[Index].set_fff(true);
  }

  vz = Xvec[5];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    Phi[3] += omega0*Phi[1];
    Phi[4] += -omega0*Phi[0];
    Phi[5] += 0;
  }

  if (posizione_z_precedente >= fine) {      // azzeriamo in previsione eventualmente di un altro solenoide...
    particle[Index].set_ffi(false);
    particle[Index].set_fff(false);
  }
}


Solenoid_SMOOTH::Solenoid_SMOOTH() {}
void Solenoid_SMOOTH::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine solenoide (z), Param[2] = B0 solenoide (in Tesla),
  Param[3] = lambda (costante "decadimento" campo B), Param[4] = inizio solenoide (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento } */

  double inizio = Param[4];
  double fine = Param[1];
  double lambda = Param[3];
  double SMOOTHING_MAGNIFYING_FACTOR = 10.0;

  // non vengono piu' calcolati ad ogni z perche' se le particelle sono molto lontane dal solenoide in z negativi
  // (per chissa' quali ragioni rimbalzate e tornate indietro) le esponenziali possono esplodere molto facilmente.
  // non sarebbero in linea di principio un problema, siccome assumono valori corretti che poi si cancellano automaticamente
  // ma purtroppo esplodono oltre il valore massimo consentito da un double!

  if (Xvec[2] >= (inizio - SMOOTHING_MAGNIFYING_FACTOR*lambda) && Xvec[2] <= (fine + SMOOTHING_MAGNIFYING_FACTOR*lambda)) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();

    double f[2];
    double f1[2]; // f1[2] contiene le derivate di f[2]
    double exp_inizio = exp(-(Xvec[2] - inizio) / lambda);
    double exp_fine = exp(-(Xvec[2] - fine) / lambda);

    f[0] = 1. / (1. + exp_inizio);
    f1[0] = exp_inizio / ((1. + exp_inizio)*(1. + exp_inizio) * lambda);
    f[1] = 1. / (1. + exp_fine);
    f1[1] = exp_fine / ((1. + exp_fine)*(1. + exp_fine) * lambda);

    double Df = f[0] - f[1];
    double Df1 = f1[0] - f1[1];

    double Sol = ch*(Param[2] * FROM_TESLA_TO_GAUSS) / (mp*C*C); // in cm^-1

    Phi[3] += Sol*(Phi[1] * Df + 0.5*Xvec[1] * Phi[2] * Df1);
    Phi[4] += -Sol*(Phi[0] * Df + 0.5*Xvec[0] * Phi[2] * Df1);
    Phi[5] += 0.5*Sol*Df1 * (Xvec[0] * Phi[1] - Xvec[1] * Phi[0]);
  }
}


Chicane_PRE::Chicane_PRE() {}
void Chicane_PRE::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine magnete bending (z), Param[2] = By uniforme interno (in Tesla) - devia nel piano xz,
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio magnete bending (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  dai calcoli analitici risulta che la deflessione s per un fascio non divergente sia pari a
  s = (B(Tesla) / beta) * l (lunghezza dipolo) * L (lunghezza drift) = (omega1) / (Xvec[5]/gamma) * Param[1] + DRIFT
  Questo significa che una chicane e' composta da

  Chicane_PRE (1cm, 2T) + Drift (10cm) + Chicane_POST (1cm, 2T) + Drift (1cm) +
  + Chicane_SELECT (di solito a circa 1 cm di spostamento x, aperta di un paio di mm) +
  + Drift (1cm) + Chicane_POST (1cm, 2T) + Drift (10cm) + Chicane_PRE (1cm, 2T)
  */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();

    double omega1 = ch*(Param[2] * FROM_TESLA_TO_GAUSS) / (mp*C*C);

    Phi[3] += -omega1*Phi[2];
    Phi[4] += 0.;
    Phi[5] += omega1*Phi[0];
  }
}


Chicane_POST::Chicane_POST() {}
void Chicane_POST::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine magnete bending (z), Param[2] = By uniforme interno (in Tesla) - devia nel piano xz,
  Param[3] = non utilizzato in questo elemento, Param[4] = inizio magnete bending (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  nb: omega1 ha un meno all'inizio perche' il campo qui e' inverso rispetto a quello scritto, cosi' da poter utilizzare
  sempre 3 T (ad esempio), sia per il pre- che per il post-, e non 3 T / -3 T   */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();
    double omega1 = -ch*(Param[2] * FROM_TESLA_TO_GAUSS) / (mp*C*C);

    Phi[3] += -omega1*Phi[2];  // non cambio segno perche' e' gia' cambiato in omega1!
    Phi[4] += 0.;
    Phi[5] += omega1*Phi[0];
  }
}


Chicane_SELECT::Chicane_SELECT() {}
void Chicane_SELECT::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine selettore chicane (z), Param[2] = posizione trasversa lungo x della fenditura,
  Param[3] = apertura (dx) della fenditura, Param[4] = inizio selettore chicane (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  NB: questa fenditura non e' corretta perche' se gli step temporali sono troppo grandi, si rischia che non
  venga nemmeno vista durante l'evoluzione. Inoltre, le particelle sentono questo campo ad ogni step, anche a quelli
  intermedi del Runge-Kutta (non fisici), quindi e' possibile che vengano filtrate erroneamente alcune particelle */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine)
    if (Xvec[0] > (Param[2] + Param[3]) || Xvec[0] < (Param[2] - Param[3]))
      particle[Index].absorbe();
}


Chicane_SELECT_New::Chicane_SELECT_New() {}
void Chicane_SELECT_New::field(double *Xvec, double *Param, double *t, double *Phi, int Index)
{
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine selettore chicane (z), Param[2] = posizione trasversa lungo x della fenditura,
  Param[3] = apertura (dx) della fenditura, Param[4] = inizio selettore chicane (z),
  Param[5] = non utilizzato in questo elemento, Param[6] = non utilizzato in questo elemento }

  NB: questa fenditura dovrebbe funzionare meglio, siccome funziona solo sugli step interi del Runge-Kutta
  ed inoltre interpola la posizione precedente con l'attuale nel verificare che non sia stata "saltata" nel frattempo   */

  double inizio = Param[4];
  double fine = Param[1];

  size_t phase_space_size = (size_t)Param[0] * PHASE_SPACE_SIZE;
  double posizione_x_attuale = Xvec[0 + 5 * phase_space_size];
  double posizione_z_attuale = Xvec[2 + 5 * phase_space_size];
  double posizione_x_precedente = Xvec[0 + 4 * phase_space_size];
  double posizione_z_precedente = Xvec[2 + 4 * phase_space_size];
  double posizione_x_interpolata;

  if (posizione_z_precedente <= inizio && posizione_z_attuale >= inizio) {
    posizione_x_interpolata = ((inizio - posizione_z_precedente) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_attuale - ((inizio - posizione_z_attuale) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_precedente;
    if (posizione_x_interpolata > (Param[2] + Param[3]) || posizione_x_interpolata < (Param[2] - Param[3])) particle[Index].absorbe();
  }

  if (posizione_z_precedente >= inizio && posizione_z_attuale <= fine) {
    // posizione_x_interpolata = posizione_x_precedente + (posizione_x_attuale - posizione_x_precedente) / (posizione_z_attuale - posizione_z_precedente) * (inizio - posizione_z_precedente);
    // posizione_x_interpolata = ((inizio - posizione_z_precedente) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_attuale - ((inizio - posizione_z_attuale) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_precedente; 

    if (posizione_x_attuale > (Param[2] + Param[3]) || posizione_x_attuale < (Param[2] - Param[3])) particle[Index].absorbe();
  }

  if (posizione_z_precedente <= fine   && posizione_z_attuale >= fine) {
    posizione_x_interpolata = ((fine - posizione_z_precedente) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_attuale - ((fine - posizione_z_attuale) / (posizione_z_attuale - posizione_z_precedente)) * posizione_x_precedente;
    if (posizione_x_interpolata > (Param[2] + Param[3]) || posizione_x_interpolata < (Param[2] - Param[3])) particle[Index].absorbe();
  }
}


RF_Cavity::RF_Cavity() {}
void RF_Cavity::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine cavita' RF (z), Param[2] = freq. standing wave (MHz),
  Param[3] = campo max cavita' (in MV/m), Param[4] = inizio cavita' RF (z),
  Param[5] = lungh. onda standing wave (in cm),
  Param[6] = fase iniziale cavita' (in gradi)}  */

  double inizio = Param[4];
  double fine = Param[1];

  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();

    double campomax = Param[3] * 1.0E4 * FROM_VOLT_TO_STATVOLT;   // 1E6*1E-2=1E4 (1E6 per andare da MV a V, 1E-2 per andare da m-1 a cm-1), il fattore FROM_VOLT_TO_STATVOLT serve per convertire il campo da SI a CGS
    double k = 2.0*M_PI / Param[5];
    double phi0 = Param[6] * 2.0*M_PI / 360.0;
    double time = *t;
    double w = 2.0*M_PI*Param[2] * 1.0E6;   // * 1.E6 serve per convertire i MHz dell'input file in Hz
    double alpha = ch / (mp*C*C);

    Phi[3] += 0.;
    Phi[4] += 0.;
    Phi[5] += alpha * 2.0 * campomax * cos(w*time - phi0) * sin(k*Xvec[2]);
  }
}


RF_Cavity_tm_astra::RF_Cavity_tm_astra() {}
void RF_Cavity_tm_astra::field(double *Xvec, double *Param, double *t, double *Phi, int Index) {
  /*  Param[] = { Param[0] = numero totale particelle, Param[1] = fine cavita' RF (z), Param[2] = freq. standing wave (MHz),
  Param[3] = campo max cavita' (in MV/m), Param[4] = inizio cavita' RF (z),
  Param[5] = lungh. onda standing wave (in cm),
  Param[6] = fase iniziale cavita' (in gradi)}  */

  double inizio = Param[4];
  double fine = Param[1];


  if (Xvec[2] >= inizio && Xvec[2] <= fine) {
    double mp = particle->get_mass();
    double ch = particle->get_charge();
    double campomax = Param[3] * 1.0E4 * FROM_VOLT_TO_STATVOLT;   // 1E6*1E-2=1E4 (1E6 per andare da MV a V, 1E-2 per andare da m-1 a cm-1), il fattore FROM_VOLT_TO_STATVOLT serve per convertire il campo da SI a CGS
    double k = 2.0*M_PI / Param[5];
    double phi0 = Param[6] * 2.0*M_PI / 360.0;
    double time = *t;
    double w = 2.0*M_PI*Param[2] * 1.0E6;   // * 1.E6 serve per convertire i MHz dell'input file in Hz
    double alpha = ch / (mp*C*C);

    double Ez0 = campomax *           cos(k*Xvec[2]);
    double d1Ez0 = campomax * (-k)    * sin(k*Xvec[2]);
    double d2Ez0 = campomax * (-k*k)  * cos(k*Xvec[2]);
    double d3Ez0 = campomax * (k*k*k) * sin(k*Xvec[2]);

    double r2 = Xvec[0] * Xvec[0] + Xvec[1] * Xvec[1];
    double r = sqrt(r2);
    double r3 = pow(r2, 1.5);
    double theta = atan2(Xvec[1], Xvec[0]);

    Phi[3] += alpha * (0.5*r*Ez0 - r3 / 16.0*(d2Ez0 + (w*w) / (C*C)*Ez0)) * w / (C*C) * cos(w*time - phi0) * cos(theta);
    Phi[4] += alpha * (0.5*r*Ez0 - r3 / 16.0*(d2Ez0 + (w*w) / (C*C)*Ez0)) * w / (C*C) * cos(w*time - phi0) * sin(theta);
    Phi[5] += alpha * (Ez0 - r2 / 4.0 * (d2Ez0 + (w*w) / (C*C) * Ez0)) * sin(w*time - phi0);
  }
}


