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


#include "definitions.h"
#include <mpi.h>
#include "lattice.h"


Magnetic_element * Lattice::get_generic_drift()
{
  double * zeri = new double[N_PARAMETRI_LATTICINO_LETTI_DA_INPUT];
  for (int i = 0; i < N_PARAMETRI_LATTICINO_LETTI_DA_INPUT; i++) zeri[i] = 0.0;
  generic_drift->set_values(zeri);
  delete[] zeri;
  return generic_drift;
}


/* il metodo get_alive restituisce il numero di particelle ancora vive nel processo locale del bunch */
int Lattice::get_alive()
{
  return mie_vive;
}

/* la funzione kill_one riduce di una unita' il conteggio delle particelle mie_vive */
void Lattice::kill_one()
{
  mie_vive--;
}

/* la funzione set_alive e' palese nel funzionamento */
void Lattice::set_alive(int quante)
{
  mie_vive = quante;
}

/* il metodo   get_pointer   restituisce l'area di lavoro   www   */
double * Lattice::get_pointer()
{
  return www;
}

/* il metodo read_file esegue la lettura delle particelle dal file di distribuzione in input */
void Lattice::read_file(std::ifstream &In, double& wtot, int tot_particles_up_to_this_rank, int contacolonne, int*& part_per_cpu, int MPI_Rank, int MPI_Size)
{
  MPI_Status status;
  int numero_parametri_distr_in = N_DIMENSIONI_SPAZIO_FASI + 3 + 1; //unsafe x,y,z,px,py,pz,type,weight,alive,id

  int alive = 0;
  double *x = new double[contacolonne];
  double *particle_chunk;
  double *my_particle_chunk;
  my_particle_chunk = new double[part_per_cpu[MPI_Rank] * contacolonne];

  if (MPI_Rank == 0) {
    for (int n = 0; n < part_per_cpu[MPI_Rank] * contacolonne; n++) In >> my_particle_chunk[n];

    for (int rank = 1; rank < MPI_Size; rank++) {
      particle_chunk = new double[part_per_cpu[rank] * contacolonne];
      for (int n = 0; n < part_per_cpu[rank] * contacolonne; n++) In >> particle_chunk[n];
      MPI_Send(particle_chunk, part_per_cpu[rank] * contacolonne, MPI_DOUBLE, rank, rank+MPI_Size, MPI_COMM_WORLD);

      delete[] particle_chunk;
      particle_chunk = nullptr;
    }
  }
  else {
    MPI_Recv(my_particle_chunk, part_per_cpu[MPI_Rank] * contacolonne, MPI_DOUBLE, 0, MPI_Rank + MPI_Size, MPI_COMM_WORLD, &status);
  }

  for (int n = 0; n < part_per_cpu[MPI_Rank]; n++) {
    for (int i = 0; i < contacolonne; i++) x[i] = my_particle_chunk[n*contacolonne + i]; // x[0]=x, x[1]=y, x[2]=z, x[3]=px, x[4]=py, x[5]=pz, x[6]=type, x[7]=weight, x[8]=alive, x[9]=id

    if (contacolonne == N_DIMENSIONI_SPAZIO_FASI) {
      x[0] *= 1.0e-4;     // conversione da micrometri a centimetri delle posizioni per i files vecchi a 6 colonne
      x[1] *= 1.0e-4;
      x[2] *= 1.0e-4;
    }

    if (contacolonne == numero_parametri_distr_in) {
      if ((x[6] > 0.9 && x[6] < 1.1) || (x[6] > -0.1 && x[6] < 0.1)) {
        particle[n].set_charge(CHARGE_CGS);
        particle[n].set_mass(MP_G);
        particle[n].set_particle_code(1);
      }
      else if (x[6] > 2.9 && x[6] < 3.1) {
        particle[n].set_charge(-CHARGE_CGS);
        particle[n].set_mass(ME_G);
        particle[n].set_particle_code(3);
      }
      particle[n].set_weight(x[7]);
      wtot += x[7];
      if (x[8] > 0.5 && x[8] < 1.5) particle[n].absorbe_when_reading();
      if (x[8] > -0.5 && x[8] < 0.5) alive++;                      // 0: viva, 1: assorbita
      if (override_ordinal) particle[n].set_ordinal(n + 1 + ((int)tot_particles_up_to_this_rank));
      else particle[n].set_ordinal((int)x[9]);
    }
    else {
      wtot += 1.0;
      particle[n].set_charge(CHARGE_CGS);
      particle[n].set_mass(MP_G);
      particle[n].set_particle_code(1);
      particle[n].set_weight(1);
      particle[n].set_ordinal(n + 1 + ((int)tot_particles_up_to_this_rank));
      alive++;
    }

    particle[n].set_ffi(false);
    particle[n].set_fff(false);
    particle[n].set_dumped_z(false);
    particle[n].set_phase_space(x);
  }

  set_alive(alive);

  for (int i = 0; i < N_DIMENSIONI_SPAZIO_FASI*part_per_cpu[MPI_Rank]; i++)
  {
    www[i] = particle[i / N_DIMENSIONI_SPAZIO_FASI].get_phase_space(i % N_DIMENSIONI_SPAZIO_FASI);
    www[i + 4 * phase_space_size] = particle[i / N_DIMENSIONI_SPAZIO_FASI].get_phase_space(i % N_DIMENSIONI_SPAZIO_FASI);
    www[i + 5 * phase_space_size] = particle[i / N_DIMENSIONI_SPAZIO_FASI].get_phase_space(i % N_DIMENSIONI_SPAZIO_FASI);
    // www[i+4*phase_space_size] e www[i+5*phase_space_size], che contengono le particelle agli step completi di runge-kutta
    // (iniziale e finale), vengono inizializzati come www[i]
  }

  In.close();
}

/* il metodo  set_values   ripartisce nel puntatore param i parametri fisici del lattice */
void Lattice::set_values(double *p)
{
  for (int i = 0; i < N_PARAMETRI_LATTICINO*magnetic_element_number; i++) param[i] = p[i];
}

/* il metodo   set_phase_space_size   determina la dimensione dello spazio delle fasi = #particelle * N_DIMENSIONI_SPAZIO_FASI (ogni particle ha N_DIMENSIONI_SPAZIO_FASI coordinate, tipicamente 6) */
void Lattice::set_phase_space_size(size_t i)
{
  phase_space_size = N_DIMENSIONI_SPAZIO_FASI*i;
}

size_t Lattice::get_phase_space_size()
{
  return phase_space_size;
}

/* il metodo    get_magnetic_element_number()   restituisce a chi lo chiedesse il valore
della variabile (inaccessibile altrimenti)  "magnetic_element_number"  */
int Lattice::get_magnetic_element_number()
{
  return magnetic_element_number;
}

/* il metodo    get_description()   restituisce a chi lo chiedesse il valore
della variabile (inaccessibile altrimenti)   description  */
char * Lattice::get_description()
{
  return description;
}

/* il metodo  set_description  assegna alla variabile  description  il valore dell'argomento   d  */
void Lattice::set_description(char *d)
{
  description = d;
}

/* il metodo init_pointers  inizializza   l'area di lavoro  www */
void Lattice::init_pointers()
{
  /********************************************************************************************
  *     www e' inizializzato ad una dimensione 6*phase_space_size perche' il metodo runge-kutta 4  *
  *     necessita, per avanzare di 1 step, delle particelle allo step 0         *
  *     e di 3 sottostep intermedi: il totale e' di 4 phase_space_size richiesti, dove ciascun   *
  *     phase_space_size contiene tutte le particelle a ciascuno step                *
  *     nel 5° e nel 6° vengono salvati per comodita' gli spazio-fase iniziale e finale *
  *     di ciascun ciclo                                *
  ********************************************************************************************/
  www = new double[6 * phase_space_size];
}

/* il seguente overload   di   init_pointers inizializza il puntatore   param */
void Lattice::init_pointers(int i)
{
  param = new double[N_PARAMETRI_LATTICINO*magnetic_element_number];
}

Lattice::Lattice()    // il costruttore default di Lattice
{
  length = 0.0;
  override_ordinal = false;
}

/* il metodo    get_element(n)   restituisce a chi lo chiedesse il valore
della variabile (inaccessibile altrimenti)   elements[n] (ossia
del puntatore all'n-esimo elemento magnetico)  */
Magnetic_element * Lattice::get_element(int n)
{
  if (n >= 0 && n < magnetic_element_number) return elements[n];
  else
  {
    double * zeri = new double[N_PARAMETRI_LATTICINO_LETTI_DA_INPUT];
    for (int i = 0; i < N_PARAMETRI_LATTICINO_LETTI_DA_INPUT; i++) zeri[i] = 0.0;
    generic_drift->set_values(zeri);
    return generic_drift;
  }
}

/* due metodi che operano in modo lampante sulla lunghezza del lattice */
double Lattice::get_length()
{
  return length;
}

void Lattice::increase_length(double l)
{
  length += l;
}

void Lattice::set_length(double l)
{
  length = l;
}

double Lattice::get_dt()
{
  return dt_time_step;
}

void Lattice::set_dt(double dt)
{
  dt_time_step = dt;
}

void Lattice::set_min_energy(double emin)
{
  min_energy = emin;
}

void Lattice::set_max_energy(double emax)
{
  max_energy = emax;
}

/* il metodo init   e'  l'inizializzatore dell'oggetto Lattice; potrebbe
essere trasformato in un costruttore parametrico, ma in tal caso
occorrerebbe inserirvi anche l'inizializzazione di   description  */
void Lattice::init(Magnetic_element * * el, int q)
{
  int i = 0, *j = new int[N_TIPI_MAGNETICI];
  /* allocazione di  magnetic_element_number = q   puntatori ..... */
  elements = new Magnetic_element *[magnetic_element_number = q];
  generic_drift = new Magnetic_element;
  memset((void *)j, 0, N_TIPI_MAGNETICI * sizeof(int));
  while (i < magnetic_element_number)
  {
    /* .... ognuno dei quali vien fatto puntare agli elements magnetici del reticolo
    NELL'ORDINE stabilito in   description: si apprezzi l'uso PARLANTE
    delle macro-costanti  */
    switch (description[i])
    {
    case DRIFT:
      elements[i] = el[_DRIFT_] + j[_DRIFT_]++;
      break;
    case SOLENOID:
      elements[i] = el[_SOLENOID_] + j[_SOLENOID_]++;
      break;
    case FOCUSING:
      elements[i] = el[_FOCUSING_] + j[_FOCUSING_]++;
      break;
    case DEFOCUSING:
      elements[i] = el[_DEFOCUSING_] + j[_DEFOCUSING_]++;
      break;
    case IRIS:
      elements[i] = el[_IRIS_] + j[_IRIS_]++;
      break;
    case IRIS_NEW:
      elements[i] = el[_IRIS_NEW_] + j[_IRIS_NEW_]++;
      break;
    case SOLENOID_FF:
      elements[i] = el[_SOLENOID_FF_] + j[_SOLENOID_FF_]++;
      break;
    case SOLENOID_SMOOTH:
      elements[i] = el[_SOLENOID_SMOOTH_] + j[_SOLENOID_SMOOTH_]++;
      break;
    case CHICANE_PRE:
      elements[i] = el[_CHICANE_PRE_] + j[_CHICANE_PRE_]++;
      break;
    case CHICANE_POST:
      elements[i] = el[_CHICANE_POST_] + j[_CHICANE_POST_]++;
      break;
    case CHICANE_SELECT:
      elements[i] = el[_CHICANE_SELECT_] + j[_CHICANE_SELECT_]++;
      break;
    case CHICANE_SELECT_NEW:
      elements[i] = el[_CHICANE_SELECT_NEW_] + j[_CHICANE_SELECT_NEW_]++;
      break;
    case RF_CAVITY:
      elements[i] = el[_RF_CAVITY_] + j[_RF_CAVITY_]++;
      break;
    case RF_CAVITY_TM_ASTRA:
      elements[i] = el[_RF_CAVITY_TM_ASTRA_] + j[_RF_CAVITY_TM_ASTRA_]++;
      break;
    }
    elements[i]->particle = particle;
    i++;
  }
}

/* implementazione (esterna alla class Lattice) del metodo update_particles                                                           **
** (da riscrivere senza argomenti r[] che consumano molte risorse senza essere piu' necessari, siccome l'evoluzione va col # di step) **
** (che poi un metodo siffatto ha dei limiti in mpi, bisognerebbe al limite usare solamente lo z medio scambiato nel main)             */
void Lattice::update_particles() {
  int i = 0;
  int64_t n = phase_space_size / N_DIMENSIONI_SPAZIO_FASI;
  Particle *p = particle; //  particle e' il puntatore a Particle membro di Lattice

  while (i < n) (p++)->set_phase_space(www + N_DIMENSIONI_SPAZIO_FASI*i++);        // <--- ecco il vero punto chiave dell'update_particles: lo spazio fase degli oggetti e il www sono sincronizzati!
}

void Lattice::cumulative_field(double *t)
{
  int j;

#pragma omp parallel for
  for (size_t i = 0; i < phase_space_size; i += N_DIMENSIONI_SPAZIO_FASI)   // scansione delle particelle: si trovano in particle
  {
    Azzeratore::field(www + phase_space_size + i);
    Drift::field(www + i, param + N_PARAMETRI_LATTICINO * 0, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
#ifdef USE_SPACECHARGE
    SpaceCharge::field(www + i, param + N_PARAMETRI_LATTICINO * 0, t, www + phase_space_size + i, www, i / N_DIMENSIONI_SPAZIO_FASI);
#endif
    j = 0;
    while (j < magnetic_element_number)
    {
      switch (description[j])
      {
      case DRIFT:
        break;
      case SOLENOID_SMOOTH:
        Solenoid_SMOOTH::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      case IRIS:
        Iris::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        if (particle[i / N_DIMENSIONI_SPAZIO_FASI].just_absorbed()) kill_one(); // non posso farlo dentro campo perche' la' kill_one() non e' accessibile
        break;
      case IRIS_NEW:
        Iris_New::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        if (particle[i / N_DIMENSIONI_SPAZIO_FASI].just_absorbed()) kill_one(); // non posso farlo dentro campo perche' la' kill_one() non e' accessibile
        break;
      case IRIS_X:
        Iris_X::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        if (particle[i / N_DIMENSIONI_SPAZIO_FASI].just_absorbed()) kill_one(); // non posso farlo dentro campo perche' la' kill_one() non e' accessibile
        break;
      case SOLENOID:
        Solenoid::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      case FOCUSING:
        Focusing::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      case DEFOCUSING:
        Defocusing::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      case SOLENOID_FF:
        Solenoid_FF::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      case CHICANE_PRE:
        Chicane_PRE::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      case CHICANE_POST:
        Chicane_POST::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      case CHICANE_SELECT:
        Chicane_SELECT::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        if (particle[i / N_DIMENSIONI_SPAZIO_FASI].just_absorbed()) kill_one(); // non posso farlo dentro campo perche' la' kill_one() non e' accessibile
        break;
      case CHICANE_SELECT_NEW:
        Chicane_SELECT_New::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        if (particle[i / N_DIMENSIONI_SPAZIO_FASI].just_absorbed()) kill_one(); // non posso farlo dentro campo perche' la' kill_one() non e' accessibile
        break;
      case RF_CAVITY:
        RF_Cavity::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        //        forse sarebbe meglio non farla morire se nel lattice ci sono cavita' acceleranti?!?
        break;
      case RF_CAVITY_TM_ASTRA:
        RF_Cavity_tm_astra::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        //        forse sarebbe meglio non farla morire se nel lattice ci sono cavita' acceleranti?!?
        break;
      default:
        Drift::field(www + i, param + N_PARAMETRI_LATTICINO*j, t, www + phase_space_size + i, i / N_DIMENSIONI_SPAZIO_FASI);
        break;
      }

      if (particle[i / N_DIMENSIONI_SPAZIO_FASI].get_phase_space(5) <= 0.0)  // se per qualche motivo la particle ha una v_z negativa, viene assorbita e non piu' conteggiata
      {
        particle[i / N_DIMENSIONI_SPAZIO_FASI].absorbe();
        if (particle[i / N_DIMENSIONI_SPAZIO_FASI].just_absorbed()) kill_one(); // non posso farlo dentro campo perche' la' kill_one() non e' accessibile
      }

      j++;
    }
  }
}

// implementazione del metodo run: e' praticamente un wrap-around di rkutta_ con update_particles
void Lattice::run(double Dt, double *t0)
{
  rkutta_(t0, &Dt);
  update_particles();    // sincronizza lo spaziofase delle particelle con il www[]
}

void Lattice::rkutta_(double *T, double *H)
{
  size_t I;
  double t, H2, H3, H6;
  t = *T;
  H2 = *H / 2.0;
  H3 = *H / 3.0;
  H6 = *H / 6.0;

  // STEP #1
  cumulative_field(&t);

#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I + 2 * phase_space_size] = www[I] + (www[I + phase_space_size] * H6);
  t += H2;

#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I + 3 * phase_space_size] = www[I],
    www[I] += (www[I + phase_space_size] * H2);  // aggiorno le posizioni in www[i] dopo lo step #1

  // STEP #2
  cumulative_field(&t);

#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I + 2 * phase_space_size] += (www[I + phase_space_size] * H3);

#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I] = www[I + 3 * phase_space_size] + www[I + phase_space_size] * H2;  // aggiorno le posizioni in www[i] dopo lo step #2

  // STEP #3
  cumulative_field(&t);

#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I + 2 * phase_space_size] += (www[I + phase_space_size] * H3);
  t += H2;

#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I] = www[I + 3 * phase_space_size] + www[I + phase_space_size] * (*H);  // aggiorno le posizioni in www[i] dopo lo step #3

  // STEP FINALE (#4) METODO RK4
  cumulative_field(&t);
  *T = t;

#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I] = www[I + 2 * phase_space_size] + www[I + phase_space_size] * H6;  // aggiorno le posizioni in www[i] dopo lo step #4

  // salvo lo spaziofasi iniziale (che e' il finale dello step precedente) in www[I+4*phase_space_size]
#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I + 4 * phase_space_size] = www[I + 5 * phase_space_size];

  // salvo lo spaziofasi finale in www[I+5*phase_space_size]
#pragma omp parallel for
  for (I = 0; I < phase_space_size; ++I) www[I + 5 * phase_space_size] = www[I];

}


