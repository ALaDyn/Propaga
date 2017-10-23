
/******************************************************************************************************
*             Copyright 2010-2017 Stefano Sinigardi, Graziano Servizi, Giorgio Turchetti              *
******************************************************************************************************/

#include "definitions.h"
#include <mpi.h>
#include "particle.h"
#include "magnetic_element.h"
#include "lattice.h"
#include "parameters.h"


int main(int argc, char *argv[]) {
  enable_extended_debug();

  Parameters par;
  par.enable_mpi(argc, argv);
  if (par.MPI_Rank == 0) par.warm_up();
  par.input_parameters_filename = std::string(argv[1]);
  par.parse_json_file();
  if (par.MPI_Rank == 0) par.open_dist_file();
  if (par.MPI_Rank == 0) par.write_run_parameters();
  par.read_particles_per_cpu();
  par.initialize_lattice();
  par.stats();
  par.filter_particles();
  if (par.MPI_Rank == 0) par.open_diag_files();
  par.write_diag();
  if (par.big_coutta) par.write_output();

  double sim_length = (par.lungh_lattice > par.z_dump ? par.lungh_lattice : par.z_dump);

  while (par.global_z_min < sim_length && par.vive_global) {
    par.lattice->run(par.dt, &(par.sim_time));
    par.stats();
    par.step++;
    if (par.step == par.step_to_be_dumped) par.write_output();
    else if (par.big_coutta && (!(par.step % par.big_coutta))) par.write_output();
    if (!(par.step % par.diag_coutta)) par.write_diag();
    if (par.ntrack && !(par.step % par.diag_coutta)) par.write_tracks();
    if (par.z_dump > 0.0) par.dump_z();
  }

  if (par.step % par.diag_coutta) par.write_diag();
  if (par.big_coutta && (par.step % par.big_coutta)) par.write_output();
  if (par.MPI_Rank == 0) par.close_diag_files();

  par.cool_down();

  return 0;
}
