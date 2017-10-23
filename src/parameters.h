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


#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "definitions.h"

#if defined(_OPENMP) && !defined (NO_OMP)
#include <omp.h>
#endif

#include <mpi.h>

#if defined(__GNUC__)
#ifdef ENABLE_DEBUG
#include <fenv.h>
#endif
#endif

#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cstdint>

#include "jsoncons/json.hpp"
#include "lattice.h"
#include "magnetic_element.h"
#include "particle.h"


void enable_extended_debug();

class Parameters
{
public:

  struct LElement {
    char type;
    double begin;
    double end;
    double par_01, par_02, par_03, par_04;
  };

  jsoncons::json parameters;

  std::vector<LElement> lattice_elements;

  std::ifstream In_dist;
  std::ofstream log_file, track_file, outz_file, minmax_file, emitt_file;

  std::string input_parameters_filename;
  std::string input_dist_filename;
  std::string run_name;
  std::ostringstream track_filename, outz_filename, minmax_filename, emitt_filename;

  bool fallita_lettura_distribuzione;
  bool restart;
  int restart_step;
  int step_to_be_dumped;
  bool enable_reference_particle;
  double eMin_json, eMax_json, z_dump;
  int MPI_Size, MPI_Rank, err_check;
  int tag_max;

  int big_coutta;
  int diag_coutta;

  double ptot;
  double weightTot;
  double xvx_sum, yvy_sum, zvz_sum;
  double xvx_mean, yvy_mean, zvz_mean;
  double x_sum, x2_sum, x_mean, x2_mean, px_sum, px2_sum, px_mean, px2_mean, sigma_x, sigma_px, epsilon_x;
  double y_sum, y2_sum, y_mean, y2_mean, py_sum, py2_sum, py_mean, py2_mean, sigma_y, sigma_py, epsilon_y;
  double z_sum, z2_sum, z_mean, z2_mean, pz_sum, pz2_sum, pz_mean, pz2_mean, sigma_z, sigma_pz, epsilon_z;

  double x_sum_global, y_sum_global, z_sum_global, px_sum_global, py_sum_global, pz_sum_global;
  double x2_sum_global, y2_sum_global, z2_sum_global, px2_sum_global, py2_sum_global, pz2_sum_global, xvx_sum_global, yvy_sum_global, zvz_sum_global;

  double x_min, x_max, y_min, y_max, z_min, z_max;
  double px_min, px_max, py_min, py_max, pz_min, pz_max;
  double global_x_min, global_x_max, global_y_min, global_y_max, global_z_min, global_z_max;
  double global_px_min, global_px_max, global_py_min, global_py_max, global_pz_min, global_pz_max;

  int tot_global, vive_global;
  double weightTot_global, inverse_weightTot_global;

  int * part_per_cpu;
  int mypart;
  int myalive;
  int tot_particles_up_to_this_rank;

  double * param, *param_default, *param_input, *xvec, dt, lungh_lattice;

  int ntrack;
  double iweightTot;            // inverso di weightTot
  int contacolonne, contarighe;

  char Latticino, *description; /* lettura description del lattice magnetico */

  int *N_tipi;

  int step;
  double sim_time;

  Lattice * lattice;
  Magnetic_element ** elements;

  Drift * drift;
  Solenoid * solenoid;
  Focusing * focusing;
  Defocusing * defocusing;
  Iris * iris;
  Iris_New * iris_new;
  Iris_X * iris_x;
  Solenoid_FF * solenoid_FF;
  Solenoid_SMOOTH * solenoid_SMOOTH;
  Chicane_PRE * chicane_PRE;
  Chicane_POST * chicane_POST;
  Chicane_SELECT * chicane_SELECT;
  Chicane_SELECT_New * chicane_SELECT_new;
  RF_Cavity * rf_cavity;
  RF_Cavity_tm_astra * rf_cavity_tm_astra;

  void parse_command_line(int, char **);
  void enable_mpi(int, char **);
  void warm_up();
  void cool_down();
  void open_dist_file();
  void write_run_parameters();
  void parse_json_file();
  void initialize_lattice();
  void read_particles_per_cpu();
  void filter_particles();
  void open_diag_files();
  void close_diag_files();
  void write_output();
  void write_diag();
  void stats();
  void write_tracks();
  void dump_z();
  int split_count(const char *, char );
  int split_count_stringstream(const char *);
  int split_count_strtok(const char *, size_t );
  Parameters();

};


#endif
