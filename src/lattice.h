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
#include <fstream>
#include "magnetic_element.h"



class Lattice
{
  /* Un array di puntatori agli elements magnetici */
  Magnetic_element **elements;
  Magnetic_element *generic_drift;

  /* la variabile description contiene la stringa descrittiva del lattice (tipo "OSOFOD...") */
  char * description;
  /* la variabile magnetic_element_number detiene il numero totale di elements magnetici del reticolo
  (e' la lunghezza di "description"); la variabile   phase_space_size  e' la dimensione totale
  dello spazio delle fasi ( volte il numero di particelle); il metodo rkutta_
  contiene l'algoritmo di runge-kutta del quarto ordine e riceve in ingresso
  gli indirizzi del valore attuale della variabile indipendente (che incrementa)
  e del valore dell'incremento (che lascia inalterato) */
  int magnetic_element_number;
  size_t phase_space_size;

  void rkutta_(double *, double *);

  /********************************************************************************************
  * i puntatori www e param detengono rispettivamente l'area di lavoro per          *
  * l'evoluzione delle particelle [www] e la collezione di tutti i parametri fisici     *
  * del lattice [param]; l'ultima variabile [lunghezza] ne rappresenta l'estensione     *
  * spaziale totale lungo l'asse Z                              *
  ********************************************************************************************/
  double *www, *param, *param_default, length, dt_time_step;

  /* la variabile   mie_vive   detiene il numero di particelle che non sono
  assorbite da alcun diaframma e pervengono alla fine del lattice */
  int mie_vive;
  int global_vive, global_vive_min, global_vive_max;

  /* la variabile  min_energy  detiene l'energia minima, espressa eventualmente da linea
  di comando, che una particle deve avere per poter sopravvivere nella simulazione
  (e' un comando ridondante, sopprimibile con un corretto uso del file di input)    */
  double min_energy;

  /* la variabile  max_energy  detiene l'energia massima, espressa eventualmente da linea
  di comando, che una particle deve avere per poter sopravvivere nella simulazione
  (e' un comando ridondante, sopprimibile con un corretto uso del file di input)    */
  double max_energy;


public:

  bool override_ordinal;
  Magnetic_element * get_generic_drift();

  /* il metodo get_alive restituisce il numero di particelle ancora mie_vive nel bunch */
  int get_alive();

  /* la funzione kill_one riduce di una unita' il conteggio delle particelle mie_vive */
  void kill_one();

  /* la funzione set_alive e' palese nel funzionamento */
  void set_alive(int);

  /* il metodo get_pointer restituisce l'area di lavoro www */
  double * get_pointer();

  Particle * particle;

  /* il metodo read_file esegue la lettura delle particelle dal file distribuzione in input */
  void read_file(std::ifstream &, double& , int , int , int*& , int , int );

  /* il metodo  set_values   ripartisce nel puntatore param i parametri fisici del lattice */
  void set_values(double *);

  /* il metodo   set_phase_space_size   determina la dimensione dello spazio delle fasi = #particelle * N_DIMENSIONI_SPAZIO_FASI (ogni particle ha N_DIMENSIONI_SPAZIO_FASI coordinate, tipicamente 6) */
  void set_phase_space_size(size_t);

  size_t get_phase_space_size();

  /* il metodo update_particles fa quello che dice
  dopo ogni passo di integrazione R.K. */
  void update_particles();

  /* il metodo run produce l'evoluzione  */
  void run(double, double*);

  /* il metodo    get_magnetic_element_number()   restituisce a chi lo chiedesse il valore
  della variabile (inaccessibile altrimenti)  "magnetic_element_number"  */
  int get_magnetic_element_number();

  /* il metodo    get_description()   restituisce a chi lo chiedesse il valore
  della variabile (inaccessibile altrimenti)   description  */
  char * get_description();

  /* il metodo  set_description  assegna alla variabile  description  il valore dell'argomento   d  */
  void set_description(char *);

  /* il metodo init_pointers  inizializza   l'area di lavoro  www */
  void init_pointers();

  /* il seguente overload   di   init_pointers inizializza il puntatore   param */
  void init_pointers(int);

  // il costruttore default di Lattice
  Lattice();

  /* il metodo    get_element(n)   restituisce a chi lo chiedesse il valore
  della variabile (inaccessibile altrimenti)   elements[n] (ossia
  del puntatore all'n-esimo elemento magnetico)  */
  Magnetic_element * get_element(int);

  /*  la funzione che calcola il campo vettoriale nello spazio delle fasi TOTALE */
  void cumulative_field(double *);

  /* due metodi che operano in modo lampante sulla lunghezza del lattice */
  double get_length();

  void increase_length(double);

  void set_length(double);

  double get_dt();

  void set_dt(double);

  void set_min_energy(double);

  void set_max_energy(double);

  /* il metodo init   e'  l'inizializzatore dell'oggetto Lattice; potrebbe
  essere trasformato in un costruttore parametrico, ma in tal caso
  occorrerebbe inserirvi anche l'inizializzazione di   description  */
  void init(Magnetic_element **, int);
};




