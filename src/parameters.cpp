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


#include "parameters.h"


void enable_extended_debug() {
#if (defined ENABLE_DEBUG) && (defined __GNUC__)
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif
}


Parameters::Parameters() {
  prefisso = "test";
  input_parameters_filename = "input.json";
  input_dist_filename = "test.initialbunch.ppg";
  fallita_lettura_distribuzione = true;
  restart = false;
  restart_step = 0;
  step_to_be_dumped = -1;
  enable_reference_particle = false;
  ntrack = 0;
  eMin_json = 0.0, eMax_json = 1.0e+100, z_dump = -1.0;
  MPI_Size = 0, MPI_Rank = 0, err_check = 0;

  step = 0;
  sim_time = 0.0;
  contacolonne = 0;
  contarighe = 0;

  weightTot = 0.;
  xvx_sum = 0., yvy_sum = 0., zvz_sum = 0.;
  xvx_mean = 0., yvy_mean = 0., zvz_mean = 0.;
  x_sum = 0., x2_sum = 0., x_mean = 0., x2_mean = 0., px_sum = 0., px2_sum = 0., px_mean = 0., px2_mean = 0., sigma_x = 0., sigma_px = 0., epsilon_x = 0.;
  y_sum = 0., y2_sum = 0., y_mean = 0., y2_mean = 0., py_sum = 0., py2_sum = 0., py_mean = 0., py2_mean = 0., sigma_y = 0., sigma_py = 0., epsilon_y = 0.;
  z_sum = 0., z2_sum = 0., z_mean = 0., z2_mean = 0., pz_sum = 0., pz2_sum = 0., pz_mean = 0., pz2_mean = 0., sigma_z = 0., sigma_pz = 0., epsilon_z = 0.;

  mypart = 0;
  tot_particles_up_to_this_rank = 0;
  tag_max = 1024 * 1024 + 1;  // unsafe
  lungh_lattice = 0.0;

  N_tipi = new int[NUMBER_OF_MAGNETIC_ELEMENTS_TYPES];
  memset((void *)N_tipi, 0, NUMBER_OF_MAGNETIC_ELEMENTS_TYPES * sizeof(int));

  lattice = new Lattice;
  elements = new Magnetic_element *[NUMBER_OF_MAGNETIC_ELEMENTS_TYPES];
}


void Parameters::enable_mpi(int argc, char ** argv) {
  err_check = MPI_Init(&argc, &argv);
  err_check = MPI_Comm_size(MPI_COMM_WORLD, &MPI_Size);
  err_check = MPI_Comm_rank(MPI_COMM_WORLD, &MPI_Rank);
  part_per_cpu = new int[MPI_Size];
}


void Parameters::warm_up() {
  log_file.open("LOG.ppg", std::ios::app);
  log_file << "\nPropaga v" << MAJOR_VERSION << "." << MINOR_VERSION << "." << FIX_RELEASE << "\nRelease date: " << RELEASE_DATE << "\nLatest change: " << RELEASE_NOTES << std::endl;
  log_file << "MPI Size: " << MPI_Size << std::endl;
  //log_file << "MPI Rank: " << MPI_Rank << std::endl;
}


void Parameters::cool_down() {
  log_file << std::flush;
  MPI_Barrier(MPI_COMM_WORLD);
  err_check = MPI_Finalize();
}


void Parameters::open_dist_file() {
  if (!restart) {
    In_dist.open(input_dist_filename);
    fallita_lettura_distribuzione = In_dist.fail();
    log_file << "Distribution file: " << input_dist_filename << std::endl;
  }
  else {
    std::stringstream filename;
    filename << prefisso << "." << std::setw(7) << std::setfill('0') << restart_step << ".ppg";
    In_dist.open(filename.str().c_str());
    fallita_lettura_distribuzione = In_dist.fail();
    log_file << "Distribution file: " << filename.str().c_str() << std::endl;
  }

  if (fallita_lettura_distribuzione) {
    log_file << "Unable to read the input distribution file" << std::endl;
    log_file.close();
  }
}


void Parameters::write_run_parameters() {
  if (!restart) {
    log_file << "Units:" << std::endl;
    log_file << "C = " << C << " (speed of light, cm/s)" << std::endl;
    log_file << "ME = " << ME_G << " (electron mass, g)" << std::endl;
    log_file << "MP_G = " << MP_G << " (proton mass, g)" << std::endl;
    log_file << "MP_MEV = " << MP_MEV << " (proton mass, MeV/c^2)" << std::endl;
    log_file << "CHARGE = " << CHARGE_CGS << " (charge unit, statC)" << std::endl;
    log_file << "FROM_TESLA_TO_GAUSS = " << FROM_TESLA_TO_GAUSS << " (conversion of B fields from SI to CGS)" << std::endl;
    log_file << "FROM_ERG_TO_MEV = " << FROM_ERG_TO_MEV << " (conversion of energy from CGS to SI)" << std::endl;
    log_file << "FROM_VOLT_TO_STATVOLT = " << FROM_VOLT_TO_STATVOLT << " (conversion of electric potential from SI to CGS)" << std::endl;
    log_file << "E0 = " << E0 << " (epsilon_0 factor for Coulomb force)" << std::endl;
    log_file << "KC = " << KC << " (Coulomb constant)" << std::endl;
    log_file << "RP = " << RP << " (classical proton radius, cm)" << std::endl;
  }
}


void Parameters::parse_json_file() {
  try {
    parameters = jsoncons::json::parse_file(input_parameters_filename);
  }
  catch (std::exception &e) {
    log_file << e.what() << std::endl;
  }
  jsoncons::json empty_json;
  jsoncons::json json_lattice_elements = parameters.has_member("Magnetic_elements") ? parameters["Magnetic_elements"] : empty_json;

  eMin_json = parameters.has_member("emin") ? parameters["emin"].as<double>() : -1.0;
  eMax_json = parameters.has_member("emax") ? parameters["emax"].as<double>() : -1.0;
  dt = parameters.has_member("dt") ? parameters["dt"].as<double>() : 0.1;
  big_coutta = parameters.has_member("steps_between_dumps") ? parameters["steps_between_dumps"].as<int>() : 0;
  diag_coutta = parameters.has_member("steps_between_diags") ? parameters["steps_between_diags"].as<int>() : 50;

  for (auto it = json_lattice_elements.begin_elements(); it != json_lattice_elements.end_elements(); ++it) {
    LElement new_element;
    jsoncons::json& json_lattice_element = *it;

    new_element.type = json_lattice_element.has_member("type") ? json_lattice_element["type"].as<char>() : 'O';
    new_element.begin = json_lattice_element.has_member("begin") ? json_lattice_element["begin"].as<double>() : 0.0;
    new_element.end = json_lattice_element.has_member("end") ? json_lattice_element["end"].as<double>() : 0.0;
    new_element.par_01 = json_lattice_element.has_member("par_01") ? json_lattice_element["par_01"].as<double>() : 0.0;
    new_element.par_02 = json_lattice_element.has_member("par_02") ? json_lattice_element["par_02"].as<double>() : 0.0;
    new_element.par_03 = json_lattice_element.has_member("par_03") ? json_lattice_element["par_03"].as<double>() : 0.0;
    new_element.par_04 = json_lattice_element.has_member("par_04") ? json_lattice_element["par_04"].as<double>() : 0.0;
    lattice_elements.push_back(new_element);
  }

  if (eMin_json < 0.0) eMin_json = 0.0;
  if (eMax_json < 0.0) eMax_json = 1.0e+100;
  if (MPI_Rank == 0 && !restart) {
    log_file << "Minimum energy: " << eMin_json << std::endl;
    log_file << "Maximum energy: " << eMax_json << std::endl;
    if (eMin_json > eMax_json) log_file << "Wrong energy selection!" << std::endl;
    log_file << "dt: " << dt << " - output dumps every " << big_coutta << " steps, diags every " << diag_coutta << " steps" << std::endl;
  }

}


int Parameters::split_count(const char *str, char c = ' ') {
  int result = 0;
  do {
    const char *begin = str;
    while (*str != c && *str) {
      str++;
      result++;
    }
  } while (0 != *str++);
  return result;
}


int Parameters::split_count_stringstream(const char *str) {
  int result = 0;
  std::string tmp;
  std::stringstream strstr(str);
  while (strstr >> tmp) {
    result++;
  }
  return result;
}


int Parameters::split_count_strtok(const char *str, size_t str_size) {
  int result = 0;
  char * str_copy = new char[str_size];
  sprintf(str_copy, str, str_size);
  char * pch;
  pch = strtok(str_copy, " \t");
  if (pch != NULL) result++;
  while (pch != NULL) {
    pch = strtok(NULL, " \t");
    if (pch != NULL) result++;
  }
  return result;
}


void Parameters::read_particles_per_cpu() {
  int iave_part_per_cpu, remainder;
  if (MPI_Rank == 0) {
    std::string str;
    std::getline(In_dist, str);
    contacolonne = split_count_stringstream(str.c_str());
    // riportiamo lo stream di lettura all'inizio, la prima riga la leggiamo solo per sapere la struttura in colonne del file
    In_dist.clear();
    In_dist.seekg(0, std::ios::beg);

    log_file << "Found " << contacolonne << " columns in your file" << std::endl;

    do {
      std::getline(In_dist, str);
      if (In_dist.eof()) break;
      contarighe++;
    } while (!In_dist.eof());
    // riportiamo lo stream di lettura all'inizio, ora che sappiamo quante righe ha (questo risultato e' stato molto dispendioso, sarebbe meglio passare a file binari)
    In_dist.clear();
    In_dist.seekg(0, std::ios::beg);

    log_file << "Found " << contarighe << " rows in your file" << std::endl;

    double dave_part_per_cpu = ((double)contarighe) / ((double)MPI_Size);
    iave_part_per_cpu = (int)std::floor(dave_part_per_cpu);
    remainder = contarighe - (iave_part_per_cpu*MPI_Size);
    for (int i = 0; i < MPI_Size; i++) part_per_cpu[i] = iave_part_per_cpu;
    for (int i = 0; i < remainder; i++) part_per_cpu[i]++;
    for (int i = 0; i < MPI_Size; i++) log_file << "CPU #" << i << " has " << part_per_cpu[i] << " particles assigned" << std::endl;
  }

  MPI_Bcast(&contacolonne, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&contarighe, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(part_per_cpu, MPI_Size, MPI_INT, 0, MPI_COMM_WORLD);
  mypart = part_per_cpu[MPI_Rank];

  for (int i = 0; i < MPI_Rank; i++) tot_particles_up_to_this_rank += part_per_cpu[i];
  MPI_Allreduce(&mypart, &tot_global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  if (contarighe != tot_global) {
    if (MPI_Rank == 0) log_file << "Warning: number of particles in file is different from the total number of particles distributed among processors!" << std::endl;
  }

  if (ntrack > contarighe) {
    ntrack = contarighe;
    if (MPI_Rank == 0) log_file << "ntrack parameter limited by the total number of particles" << std::endl;
  }

  lattice->set_dt(dt);
  lattice->set_min_energy(eMin_json);
  lattice->set_max_energy(eMax_json);
  if (restart) step = restart_step, sim_time = restart_step * dt;

  // allocazione chiave per cui particle e' dentro lattice!
  lattice->particle = new Particle[mypart];
  lattice->set_phase_space_size(mypart);

  // attribuzione di alcuni valori gia' determinati all'oggetto lattice
  lattice->init_pointers();
  xvec = lattice->get_pointer();

  lattice->read_file(In_dist, weightTot, tot_particles_up_to_this_rank, contacolonne, part_per_cpu, MPI_Rank, MPI_Size);
}


void Parameters::parse_command_line(int argc, char ** argv) {

  for (int i = 1; i < argc; i++)
    /* We will iterate over argv[] to get the parameters stored inside. **
    ** Note that we're starting on 1 because we don't need to know the  **
    ** path of the program, which is stored in argv[0]                   */
  {
    if (std::string(argv[i]) == "-f")
    {
      input_dist_filename = std::string(argv[i + 1]);
      i++;              // so that we skip in the for cycle the parsing of the <nome-file-particelle>
    }
    else if (std::string(argv[i]) == "-restart")
    {
      restart_step = atoi(argv[i + 1]);
      restart = true;
      if (MPI_Rank == 0) log_file << "Restarting from step " << restart_step << std::endl;
      i++;
    }
    else if (std::string(argv[i]) == "-override_ordinal")
    {
      lattice->override_ordinal = true;
      if (MPI_Rank == 0) log_file << "Overriding ordinal number (if present) in input distribution" << std::endl;
    }
    else if (std::string(argv[i]) == "-par")
    {
      input_parameters_filename = std::string(argv[i + 1]);
      if (MPI_Rank == 0) log_file << "Input parameters json file: " << std::string(argv[i + 1]) << std::endl;
      i++;
    }
    else if (std::string(argv[i]) == "-o")
    {
      prefisso = std::string(argv[i + 1]);
      if (MPI_Rank == 0) log_file << "Run string: " << prefisso << std::endl;
      i++;
    }
    else if (std::string(argv[i]) == "-LASTSLOWEST")
    {
      enable_reference_particle = 1;
      if (MPI_Rank == 0) log_file << "The last particle in the distribution is being considered as the reference particle" << std::endl;
    }
    else if (std::string(argv[i]) == "-TRACK")
    {
      ntrack = atoi(argv[i + 1]);
      if (MPI_Rank == 0) if (ntrack < 1) log_file << "Wrong ntrack from commandline, reset to 0" << std::endl;
      if (ntrack < 1) ntrack = 0;
      i++;
    }
    else if (std::string(argv[i]) == "-OUT")
    {
      step_to_be_dumped = atoi(argv[i + 1]);
      if (MPI_Rank == 0) log_file << "Will dump distribution at step " << step_to_be_dumped << std::endl;
      i++;
    }
    else if (std::string(argv[i]) == "-OUTZ")
    {
      z_dump = atof(argv[i + 1]);
      if (MPI_Rank == 0) log_file << "Will dump distribution at z = " << z_dump << std::endl;
      i++;
    }
    else
    {
      if (MPI_Rank == 0) log_file << "Invalid argument: " << argv[i] << std::endl;
    }
  }
}


void Parameters::initialize_lattice() {

  /* prima scansione del file lattice per puro conteggio elements */
  for (size_t i = 0; i < lattice_elements.size(); ++i) {
    switch (lattice_elements[i].type) // conteggio in base al carattere a capo riga
    {
    case DRIFT:
      N_tipi[_DRIFT_]++;
      break;

    case SOLENOID:
      N_tipi[_SOLENOID_]++;
      break;

    case FOCUSING:
      N_tipi[_FOCUSING_]++;
      break;

    case DEFOCUSING:
      N_tipi[_DEFOCUSING_]++;
      break;

    case IRIS:
      N_tipi[_IRIS_]++;
      break;

    case IRIS_NEW:
      N_tipi[_IRIS_NEW_]++;
      break;

    case IRIS_X:
      N_tipi[_IRIS_X_]++;
      break;

    case SOLENOID_FF:
      N_tipi[_SOLENOID_FF_]++;
      break;

    case SOLENOID_SMOOTH:
      N_tipi[_SOLENOID_SMOOTH_]++;
      break;

    case CHICANE_PRE:
      N_tipi[_CHICANE_PRE_]++;
      break;

    case CHICANE_POST:
      N_tipi[_CHICANE_POST_]++;
      break;

    case CHICANE_SELECT:
      N_tipi[_CHICANE_SELECT_]++;
      break;

    case CHICANE_SELECT_NEW:
      N_tipi[_CHICANE_SELECT_NEW_]++;
      break;

    case RF_CAVITY:
      N_tipi[_RF_CAVITY_]++;
      break;

    case RF_CAVITY_TM_ASTRA:
      N_tipi[_RF_CAVITY_TM_ASTRA_]++;
      break;

    default:
      break;

    }
  }

  if (MPI_Rank == 0)
    if (!restart)
      if (N_tipi[_SOLENOID_FF_])
        log_file << "\nWARNING! You're using an experimental lattice element that is improper in a RK4 environment!" << std::endl;


  // i parametri default vengono usati nel caso default e sono inizializzati tutti a zero
  // il caso default dello switch(description[ind]) che seguira' infatti e' un drift ed esso
  // pertanto non richiede alcun parametro particolare nell'evoluzione
  param_default = new double[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT];
  for (int i = 0; i < NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT; i++) param_default[i] = 0.0;

  /* Istanziazione degli oggetti in base ai conteggi */
  drift = new Drift[N_tipi[_DRIFT_]];
  solenoid = new Solenoid[N_tipi[_SOLENOID_]];
  focusing = new Focusing[N_tipi[_FOCUSING_]];
  defocusing = new Defocusing[N_tipi[_DEFOCUSING_]];
  iris = new Iris[N_tipi[_IRIS_]];
  iris_new = new Iris_New[N_tipi[_IRIS_NEW_]];
  iris_x = new Iris_X[N_tipi[_IRIS_X_]];
  solenoid_FF = new Solenoid_FF[N_tipi[_SOLENOID_FF_]];
  solenoid_SMOOTH = new Solenoid_SMOOTH[N_tipi[_SOLENOID_SMOOTH_]];
  chicane_PRE = new Chicane_PRE[N_tipi[_CHICANE_PRE_]];
  chicane_POST = new Chicane_POST[N_tipi[_CHICANE_POST_]];
  chicane_SELECT = new Chicane_SELECT[N_tipi[_CHICANE_SELECT_]];
  chicane_SELECT_new = new Chicane_SELECT_New[N_tipi[_CHICANE_SELECT_NEW_]];
  rf_cavity = new RF_Cavity[N_tipi[_RF_CAVITY_]];
  rf_cavity_tm_astra = new RF_Cavity_tm_astra[N_tipi[_RF_CAVITY_TM_ASTRA_]];

  /* uso di un array di puntatori alla classe base per unificare i riferimenti
  ai diversi tipi di elements magnetici */
  elements[_DRIFT_] = drift;
  elements[_SOLENOID_] = solenoid;
  elements[_FOCUSING_] = focusing;
  elements[_DEFOCUSING_] = defocusing;
  elements[_IRIS_] = iris;
  elements[_IRIS_NEW_] = iris_new;
  elements[_IRIS_X_] = iris_x;
  elements[_SOLENOID_FF_] = solenoid_FF;
  elements[_SOLENOID_SMOOTH_] = solenoid_SMOOTH;
  elements[_CHICANE_PRE_] = chicane_PRE;
  elements[_CHICANE_POST_] = chicane_POST;
  elements[_CHICANE_SELECT_] = chicane_SELECT;
  elements[_CHICANE_SELECT_NEW_] = chicane_SELECT_new;
  elements[_RF_CAVITY_] = rf_cavity;
  elements[_RF_CAVITY_TM_ASTRA_] = rf_cavity_tm_astra;

  /* calcolo del numero complessivo di elements e allocazione della
  stringa descrittiva dell'apparato */
  description = new char[lattice_elements.size()];

  /* seconda scansione: lettura dei parametri fisici per ogni elemento,
  non uno di piu' non uno di meno */
  param = new double[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*lattice_elements.size()];

  // i parametri input sono letti nell'ordine (piu' umano) nel quale sono scritti nell'input file
  // vengono poi utilizzati per assegnare i valori corretti ai parametri interni al codice, che sono 
  // un array con un ordine molto meno comprensibile di quello che viene usato per l'input file;
  // sono un numero diverso rispetto al numero di parametri dentro al codice perche' questi ultimi contengono 
  // anche il numero di particelle assegnate alla cpu, ad esempio, che non viene letto dall'input
  param_input = new double[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT_READ_FROM_INPUT];
  for (int i = 0; i < NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT_READ_FROM_INPUT; i++) param_input[i] = 0.0;

  for (size_t i = 0; i < lattice_elements.size(); ++i) {
    description[i] = lattice_elements[i].type;
    param_input[0] = lattice_elements[i].begin;
    param_input[1] = lattice_elements[i].end;
    param_input[2] = lattice_elements[i].par_01;
    param_input[3] = lattice_elements[i].par_02;
    param_input[4] = lattice_elements[i].par_03;
    param_input[5] = lattice_elements[i].par_04;
    param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i] = static_cast<double>(mypart);
    param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 1] = param_input[1];
    param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 2] = param_input[2];
    param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 3] = param_input[3];
    param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 4] = param_input[0];
    param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 5] = param_input[4];
    param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 6] = param_input[5];
    if (param_input[1] > lungh_lattice) lungh_lattice = param_input[1];

    switch (lattice_elements[i].type)
    {
    case DRIFT:                                             // unnecessary element
      drift->set_values(param_input);
      drift++;
      break;

    case SOLENOID:
      solenoid->set_values(param_input);
      solenoid++;
      break;

    case FOCUSING:
      focusing->set_values(param_input);
      focusing++;
      break;

    case DEFOCUSING:
      defocusing->set_values(param_input);
      defocusing++;
      break;

    case IRIS:
      iris->set_values(param_input);
      iris++;
      break;

    case IRIS_NEW:
      iris_new->set_values(param_input);
      iris_new++;
      break;

    case IRIS_X:
      iris_x->set_values(param_input);
      iris_x++;
      break;

    case SOLENOID_FF:
      solenoid_FF->set_values(param_input);
      solenoid_FF++;
      break;

    case SOLENOID_SMOOTH:
      solenoid_SMOOTH->set_values(param_input);
      solenoid_SMOOTH++;
      break;

    case CHICANE_PRE:
      chicane_PRE->set_values(param_input);
      chicane_PRE++;
      break;

    case CHICANE_POST:
      chicane_POST->set_values(param_input);
      chicane_POST++;
      break;

    case CHICANE_SELECT:
      chicane_SELECT->set_values(param_input);
      chicane_SELECT++;
      break;

    case CHICANE_SELECT_NEW:
      chicane_SELECT_new->set_values(param_input);
      chicane_SELECT_new++;
      break;

    case RF_CAVITY:
      rf_cavity->set_values(param_input);
      rf_cavity++;
      break;

    case RF_CAVITY_TM_ASTRA:
      rf_cavity->set_values(param_input);
      rf_cavity++;
      break;

    default:
      break;
    }
  }

  lattice->set_length(lungh_lattice);

  if (MPI_Rank == 0) {
    if (!restart) {
      log_file << "\nLattice description: (element type, starting pos., ending pos., p, q, r, s)" << std::endl;
      for (size_t i = 0; i < lattice_elements.size(); ++i) {
        log_file << description[i]; // element type

        /*se ci sono piu' di 1.000.000 di particelle, in automatico viene scritto in notazione scientifica*/

        // non c'e' un ciclo for per poter scrivere i parametri in un ordine piu' umanamente comprensibile
        log_file << "\t" << param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 4]; // starting position of the element
        log_file << "\t" << param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 1]; // ending position of the element
        log_file << "\t" << param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 2];
        log_file << "\t" << param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 3];
        log_file << "\t" << param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 5];
        log_file << "\t" << param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i + 6];
        // for (int jj=0; jj < NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT; jj++) log_file << "\t" << param[NUMBER_OF_PARAMETERS_PER_LATTICE_ELEMENT*i+jj];

        log_file << std::endl;
      }

      log_file << std::endl;
    }
  }

  // fine seconda scansione: tutti gli elements sono stati inizializzati
  lattice->set_description(description);
  lattice->init(elements, (int)lattice_elements.size());
  lattice->init_pointers(0);
  lattice->set_values(param);
}


void Parameters::filter_particles() {
  Particle *particellacontatore = lattice->particle;
  if (!restart) {
    for (int i = 0; i < mypart; i++) {
      if (!(particellacontatore->is_absorbed())) {
        if (particellacontatore->get_energy_MeV() <= eMin_json || particellacontatore->get_energy_MeV() >= eMax_json) {
          particellacontatore->absorbe();
          if (particellacontatore->just_absorbed()) lattice->kill_one();
        }
      }
      particellacontatore++;
    }
  }
}


void Parameters::open_diag_files() {
  minmax_filename << prefisso << ".minmax.ppg";
  outz_filename << prefisso << ".9999999.ppg";
  track_filename << prefisso << ".tracked.ppg";
  emitt_filename << prefisso << ".emitt.ppg";
  minmax_file.open(minmax_filename.str().c_str(), std::ios::app);
  minmax_file.setf(std::ios::scientific);

  if (z_dump > 0) {
    outz_file.open(outz_filename.str().c_str());
    outz_file.setf(std::ios::scientific);
  }

  if (ntrack) {
    track_file.open(track_filename.str().c_str(), std::ios::app);
    track_file.setf(std::ios::scientific);
  }

  emitt_file.open(emitt_filename.str().c_str(), std::ios::app);
  emitt_file.setf(std::ios::scientific);

  time_t now = time(0);
  char* now_string = ctime(&now);
  log_file << "Date and time: " << now_string;
  if (!restart) log_file << "\nWORKING....." << std::endl;
  else log_file << "\nRESTARTING from j = " << step << " and ct = " << sim_time << " cm ....." << std::endl;
}


void Parameters::write_output() {
  double *particle_chunk;
  MPI_Status status;
  int numero_parametri_distr_out = PHASE_SPACE_SIZE + 3 + 1; //unsafe x,y,z,px,py,pz,type,weight,alive,id

  if (MPI_Rank == 0) {
    std::stringstream filename;
    std::ofstream ris;
    filename << prefisso << "." << std::setw(7) << std::setfill('0') << step << ".ppg";
    ris.open(filename.str().c_str());
    ris.setf(std::ios::scientific); // sara' scritto in notazione scientifica
    log_file << "STARTING DUMP DATA" << std::endl;
    particle_chunk = new double[mypart * numero_parametri_distr_out];
    for (int n = 0; n < mypart; n++) {
      for (int i = 0; i < PHASE_SPACE_SIZE; i++) particle_chunk[numero_parametri_distr_out*n + i] = xvec[PHASE_SPACE_SIZE*n + i];
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 0] = lattice->particle[n].get_particle_code();
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 1] = lattice->particle[n].get_weight();
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 2] = lattice->particle[n].is_absorbed();
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 3] = lattice->particle[n].get_ordinal();
    }
    for (int n = 0; n < mypart; n++) {
      for (int i = 0; i < numero_parametri_distr_out; i++) ris << std::setprecision(7) << particle_chunk[numero_parametri_distr_out*n + i] << "\t";
      ris << std::endl;
    }
    delete[] particle_chunk;
    particle_chunk = nullptr;

    for (int rank = 1; rank < MPI_Size; rank++) {
      particle_chunk = new double[part_per_cpu[rank] * numero_parametri_distr_out];
      MPI_Recv(particle_chunk, part_per_cpu[rank] * numero_parametri_distr_out, MPI_DOUBLE, rank, rank + MPI_Size, MPI_COMM_WORLD, &status);
      for (int n = 0; n < part_per_cpu[rank]; n++) {
        for (int i = 0; i < numero_parametri_distr_out; i++) ris << std::setprecision(7) << particle_chunk[numero_parametri_distr_out*n + i] << "\t";
        ris << std::endl;
      }
      delete[] particle_chunk;
      particle_chunk = nullptr;
    }
    ris.close();
  }
  else {
    particle_chunk = new double[mypart * numero_parametri_distr_out];
    for (int n = 0; n < mypart; n++) {
      for (int i = 0; i < PHASE_SPACE_SIZE; i++) particle_chunk[numero_parametri_distr_out*n + i] = xvec[PHASE_SPACE_SIZE*n + i];
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 0] = lattice->particle[n].get_particle_code();
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 1] = lattice->particle[n].get_weight();
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 2] = lattice->particle[n].is_absorbed();
      particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 3] = lattice->particle[n].get_ordinal();
    }
    MPI_Send(particle_chunk, mypart * numero_parametri_distr_out, MPI_DOUBLE, 0, MPI_Rank + MPI_Size, MPI_COMM_WORLD);
    delete[] particle_chunk;
    particle_chunk = nullptr;
  }
  if (MPI_Rank == 0) {
    time_t now = time(0);
    char* now_string = ctime(&now);
    log_file << "DUMP DATA FINISHED, " << now_string;
  }
}


void Parameters::write_diag() {
  double x, y, z, vx, vy, vz, w;

  x_sum = 0.;
  y_sum = 0.;
  z_sum = 0.;
  px_sum = 0.;
  py_sum = 0.;
  pz_sum = 0.;
  weightTot = 0.;
  x2_sum = 0.;
  y2_sum = 0.;
  z2_sum = 0.;
  px2_sum = 0.;
  py2_sum = 0.;
  pz2_sum = 0.;
  xvx_sum = 0.;
  yvy_sum = 0.;
  zvz_sum = 0.;

  for (int n = 0; n < mypart; n++) {
    if (enable_reference_particle && n == mypart - 1) continue;     // cosi' che non entrino in gioco particelle perse o inutili
    if (lattice->particle[n].is_absorbed()) continue;


    w = lattice->particle[n].get_weight();
    x = xvec[PHASE_SPACE_SIZE*n + 0] * w;
    y = xvec[PHASE_SPACE_SIZE*n + 1] * w;
    z = xvec[PHASE_SPACE_SIZE*n + 2] * w;
    vx = xvec[PHASE_SPACE_SIZE*n + 3];
    vy = xvec[PHASE_SPACE_SIZE*n + 4];
    vz = xvec[PHASE_SPACE_SIZE*n + 5];
    ptot = sqrt(vx*vx + vy*vy + vz*vz);
    vx /= ptot; // abbiamo bisogno di x' = v_x/v
    vy /= ptot; // abbiamo bisogno di y' = v_y/v
    vx *= w;
    vy *= w;
    vz *= w;

    weightTot += w;
    x_sum += x;
    y_sum += y;
    z_sum += z;
    px_sum += vx;
    py_sum += vy;
    pz_sum += vz;
    x2_sum += x * x;
    y2_sum += y * y;
    z2_sum += z * z;
    px2_sum += vx * vx;
    py2_sum += vy * vy;
    pz2_sum += vz * vz;
    xvx_sum += x * vx;
    yvy_sum += y * vy;
    zvz_sum += z * vz;
  }


  MPI_Allreduce(&x_sum, &x_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&y_sum, &y_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&z_sum, &z_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&px_sum, &px_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&py_sum, &py_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&pz_sum, &pz_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&weightTot, &weightTot_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&x2_sum, &x2_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&y2_sum, &y2_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&z2_sum, &z2_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&px2_sum, &px2_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&py2_sum, &py2_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&pz2_sum, &pz2_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&xvx_sum, &xvx_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&yvy_sum, &yvy_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&zvz_sum, &zvz_sum_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  if (weightTot_global > 0.) inverse_weightTot_global = 1. / weightTot_global;
  else inverse_weightTot_global = 0.;

  if (weightTot > 0.) iweightTot = 1. / weightTot;
  else iweightTot = 0.0;

  x_mean = x_sum_global * inverse_weightTot_global;
  px_mean = px_sum_global * inverse_weightTot_global;
  y_mean = y_sum_global * inverse_weightTot_global;
  py_mean = py_sum_global * inverse_weightTot_global;
  z_mean = z_sum_global * inverse_weightTot_global;
  pz_mean = pz_sum_global * inverse_weightTot_global;
  x2_mean = x2_sum_global * inverse_weightTot_global;
  px2_mean = px2_sum_global * inverse_weightTot_global;
  y2_mean = y2_sum_global * inverse_weightTot_global;
  py2_mean = py2_sum_global * inverse_weightTot_global;
  z2_mean = z2_sum_global * inverse_weightTot_global;
  pz2_mean = pz2_sum_global * inverse_weightTot_global;
  xvx_mean = xvx_sum_global * inverse_weightTot_global;
  yvy_mean = yvy_sum_global * inverse_weightTot_global;
  zvz_mean = zvz_sum_global * inverse_weightTot_global;

  sigma_x = x2_mean - x_mean*x_mean;          // sigma_x = sqrt(x2_mean);
  sigma_y = y2_mean - y_mean*y_mean;          // sigma_y = sqrt(y2_mean);
  sigma_z = z2_mean - z_mean*z_mean;          // sigma_z = sqrt(z2_mean);
  sigma_px = px2_mean - px_mean * px_mean;    // sigma_px = sqrt(px2_mean);
  sigma_py = py2_mean - py_mean * py_mean;    // sigma_py = sqrt(py2_mean);
  sigma_pz = pz2_mean - pz_mean * pz_mean;    // sigma_pz = sqrt(pz2_mean);
  double m_x = xvx_mean - x_mean * px_mean;
  double m_y = yvy_mean - y_mean * py_mean;
  double m_z = zvz_mean - z_mean * pz_mean;
  epsilon_x = sigma_x * sigma_px - m_x*m_x;   // epsilon_x = x2_mean * px2_mean - xvx_mean*xvx_mean;
  epsilon_y = sigma_y * sigma_py - m_y*m_y;   // epsilon_y = y2_mean * py2_mean - yvy_mean*yvy_mean;
  epsilon_z = sigma_z * sigma_pz - m_z*m_z;   // epsilon_z = z2_mean * pz2_mean - zvz_mean*zvz_mean;

  sigma_x > 0. ? sigma_x = sqrt(sigma_x) : sigma_x = 0.;
  sigma_y > 0. ? sigma_y = sqrt(sigma_y) : sigma_y = 0.;
  sigma_z > 0. ? sigma_z = sqrt(sigma_z) : sigma_z = 0.;
  epsilon_x > 0. ? epsilon_x = sqrt(epsilon_x) : epsilon_x = 0.;
  epsilon_y > 0. ? epsilon_y = sqrt(epsilon_y) : epsilon_y = 0.;
  epsilon_z > 0. ? epsilon_z = sqrt(epsilon_z) : epsilon_z = 0.;

  if (MPI_Rank == 0) {
    minmax_file << step << " : " << tot_global << "\t" << vive_global << "\t";
    minmax_file << global_x_min << "\t" << global_x_max << "\t" << global_y_min << "\t" << global_y_max << "\t" << global_z_min << "\t" << global_z_max << "\t";
    minmax_file << global_px_min << "\t" << global_px_max << "\t" << global_py_min << "\t" << global_py_max << "\t" << global_pz_min << "\t" << global_pz_max;
    minmax_file << std::endl;

    emitt_file << step << " : " << tot_global << "\t" << vive_global << "\t"
      << std::setprecision(6) << std::setiosflags(std::ios::scientific) << weightTot << "\t"
      << z_mean << "\t" << sigma_x << "\t" << epsilon_x << "\t" << sigma_y << "\t" << epsilon_y << "\t" << sigma_z << "\t" << epsilon_z << std::endl;

    log_file << "ct = " << sim_time << " cm, z_min = " << global_z_min << " cm, z_max = " << global_z_max << " cm, tot_global = " << tot_global << ", alive_global " << vive_global << "." << std::endl;
  }
}



void Parameters::dump_z() {
  MPI_Status status;
  int particle_number_to_dump = 0;
  int numero_parametri_distr_out = PHASE_SPACE_SIZE + 3 + 1; //unsafe x,y,z,px,py,pz,type,weight,alive,id

  if (MPI_Rank == 0) {
    for (int n = 0; n < mypart; n++) {
      if (!(lattice->particle[n].get_dumped_z()) && lattice->particle[n].get_phase_space(2) >= z_dump) {
        for (int i = 0; i < PHASE_SPACE_SIZE; i++) outz_file << std::setprecision(7) << xvec[PHASE_SPACE_SIZE*n + i] << "\t";
        outz_file << std::setprecision(7) << sim_time << "\t"         // pay attention: in this output there's not the particle id but the time at which it has been dumped!!
          << lattice->particle[n].get_weight() << "\t"
          << (double)lattice->particle[n].is_absorbed() << "\t"       // the cast, while terrible in the quality of the output, is just to uniform the file, since the buffer dumped
          << (double)lattice->particle[n].get_ordinal() << std::endl; // from other processor will dump just a blob of doubles
        lattice->particle[n].set_dumped_z(true);
      }
    }

    double *particle_chunk = NULL;
    for (int rank = 1; rank < MPI_Size; rank++) {
      MPI_Recv(&particle_number_to_dump, 1, MPI_INT, rank, rank + 3 * MPI_Size, MPI_COMM_WORLD, &status);
      particle_chunk = new double[particle_number_to_dump];

      MPI_Recv(particle_chunk, particle_number_to_dump * numero_parametri_distr_out, MPI_DOUBLE, rank, rank + 4 * MPI_Size, MPI_COMM_WORLD, &status);
      for (int n = 0; n < particle_number_to_dump; n++) {
        for (int i = 0; i < PHASE_SPACE_SIZE + 4; i++) outz_file << std::setprecision(7) << particle_chunk[PHASE_SPACE_SIZE*n + i] << "\t";
        outz_file << std::endl;
      }

      delete[] particle_chunk;
      particle_chunk = NULL;
    }
  }
  else {
    std::vector< std::vector<double> > particle_chunk;
    std::vector<double> one_particle(10, 0.0);
    for (int n = 0; n < mypart; n++) {
      if (!(lattice->particle[n].get_dumped_z()) && lattice->particle[n].get_phase_space(2) >= z_dump) {
        for (int i = 0; i < PHASE_SPACE_SIZE; i++) one_particle[i] = xvec[PHASE_SPACE_SIZE*n + i];
        one_particle[6] = sim_time;                                   // pay attention: in this output there's not the particle id but the time at which it has been dumped!!
        one_particle[7] = lattice->particle[n].get_weight();
        one_particle[8] = (double)lattice->particle[n].is_absorbed();
        one_particle[9] = (double)lattice->particle[n].get_ordinal();
        particle_chunk.push_back(one_particle);
        lattice->particle[n].set_dumped_z(true);
      }
    }

    particle_number_to_dump = (int)particle_chunk.size();
    MPI_Send(&particle_number_to_dump, 1, MPI_INT, 0, MPI_Rank + 3 * MPI_Size, MPI_COMM_WORLD);
    MPI_Send(&particle_chunk[0], particle_number_to_dump * numero_parametri_distr_out, MPI_DOUBLE, 0, MPI_Rank + 4 * MPI_Size, MPI_COMM_WORLD);  //ISO 14882, 2nd Edition : Section 23.2.4[lib.vector] : "The elements of a vector are stored contiguously, meaning that if v is a vector<T, Allocator> where T is some type other than bool, then it obeys the identity &v[n] == &v[0] + n for all 0 <= n < v.size().
  }
}


void Parameters::write_tracks() {
  MPI_Status status;
  double *my_particle_chunk = NULL;
  int numero_parametri_distr_out = PHASE_SPACE_SIZE + 3 + 1; //unsafe x,y,z,px,py,pz,type,weight,alive,id
  bool do_i_have_any_track = (ntrack > tot_particles_up_to_this_rank ? true : false);
  int my_ntrack = (do_i_have_any_track ? ntrack - tot_particles_up_to_this_rank : 0);
  my_ntrack = (my_ntrack > mypart ? mypart : my_ntrack);
  int * tracks_per_cpu = new int[MPI_Size];
  MPI_Gather(&my_ntrack, 1, MPI_INT, tracks_per_cpu, 1, MPI_INT, 0, MPI_COMM_WORLD);

  my_particle_chunk = new double[my_ntrack * numero_parametri_distr_out];

  for (int n = 0; n < my_ntrack; n++) {
    for (int i = 0; i < PHASE_SPACE_SIZE; i++) my_particle_chunk[numero_parametri_distr_out*n + i] = xvec[PHASE_SPACE_SIZE*n + i];
    my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 0] = lattice->particle[n].get_particle_code();
    my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 1] = lattice->particle[n].get_weight();
    my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 2] = lattice->particle[n].is_absorbed();
    my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 3] = lattice->particle[n].get_ordinal();
  }

  if (MPI_Rank == 0) {
    for (int n = 0; n < my_ntrack; n++) {
      for (int i = 0; i < numero_parametri_distr_out; i++) track_file << std::setprecision(7) << my_particle_chunk[numero_parametri_distr_out*n + i] << "\t";
      track_file << std::endl;
    }
    for (int rank = 1; rank < MPI_Size; rank++) {
      delete[] my_particle_chunk;
      my_particle_chunk = NULL;
      my_particle_chunk = new double[tracks_per_cpu[rank] * numero_parametri_distr_out];
      MPI_Recv(my_particle_chunk, tracks_per_cpu[rank] * numero_parametri_distr_out, MPI_DOUBLE, rank, rank + 2 * MPI_Size, MPI_COMM_WORLD, &status);
      for (int n = 0; n < tracks_per_cpu[rank]; n++) {
        for (int i = 0; i < numero_parametri_distr_out; i++) track_file << std::setprecision(7) << my_particle_chunk[numero_parametri_distr_out*n + i] << "\t";
        track_file << std::endl;
      }
    }
  }
  else {
    for (int n = 0; n < my_ntrack; n++) {
      for (int i = 0; i < PHASE_SPACE_SIZE; i++) my_particle_chunk[numero_parametri_distr_out*n + i] = xvec[PHASE_SPACE_SIZE*n + i];
      my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 0] = lattice->particle[n].get_particle_code();
      my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 1] = lattice->particle[n].get_weight();
      my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 2] = lattice->particle[n].is_absorbed();
      my_particle_chunk[numero_parametri_distr_out*n + PHASE_SPACE_SIZE + 3] = lattice->particle[n].get_ordinal();
    }
    MPI_Send(my_particle_chunk, my_ntrack * numero_parametri_distr_out, MPI_DOUBLE, 0, MPI_Rank + 2 * MPI_Size, MPI_COMM_WORLD);
  }

  delete[] tracks_per_cpu;
  tracks_per_cpu = NULL;
  delete[] my_particle_chunk;
  my_particle_chunk = NULL;
}


void Parameters::close_diag_files() {
  log_file << "At the end of the sim, (ct = " << sim_time << " cm, j = " << step << "), the " << vive_global
    << " particles still alive have these coordinates: z_min: " << global_z_min << " cm, z_max: " << global_z_max << " cm." << std::endl << "END OF RUN!" << std::flush << std::endl;
  time_t now = time(0);
  char* now_string = ctime(&now);
  log_file << "Finished on: " << now_string << std::endl;

  minmax_file.close();
  emitt_file.close();
  if (ntrack) track_file.close();
  if (z_dump > 0) outz_file.close();
  log_file.close();
}


void Parameters::stats()
{
  double a, b, c, d, e, f, t, w;
  x_min = y_min = z_min = px_min = py_min = pz_min = 1.0e+308;
  x_max = y_max = z_max = px_max = py_max = pz_max = -1.0e+308;
  myalive = 0;

  for (int n = 0; n < mypart; n++) {
    if (lattice->particle[n].is_absorbed()) continue;

    a = lattice->particle[n].get_phase_space(0);
    b = lattice->particle[n].get_phase_space(1);
    c = lattice->particle[n].get_phase_space(2);
    d = lattice->particle[n].get_phase_space(3);
    e = lattice->particle[n].get_phase_space(4);
    f = lattice->particle[n].get_phase_space(5);
    w = lattice->particle[n].get_weight();

    t = x_min;
    a < t ? x_min = a : x_min = t;
    t = x_max;
    a > t ? x_max = a : x_max = t;

    t = y_min;
    b < t ? y_min = b : y_min = t;
    t = y_max;
    b > t ? y_max = b : y_max = t;

    t = z_min;
    c < t ? z_min = c : z_min = t;
    t = z_max;
    c > t ? z_max = c : z_max = t;

    t = px_min;
    d < t ? px_min = d : px_min = t;
    t = px_max;
    d > t ? px_max = d : px_max = t;

    t = py_min;
    e < t ? py_min = e : py_min = t;
    t = py_max;
    e > t ? py_max = e : py_max = t;

    t = pz_min;
    f < t ? pz_min = f : pz_min = t;
    t = pz_max;
    f > t ? pz_max = f : pz_max = t;

    myalive++;
  }

  MPI_Allreduce(&x_min, &global_x_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&x_max, &global_x_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&y_min, &global_y_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&y_max, &global_y_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&z_min, &global_z_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&z_max, &global_z_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&px_min, &global_px_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&px_max, &global_px_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&py_min, &global_py_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&py_max, &global_py_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&pz_min, &global_pz_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&pz_max, &global_pz_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&myalive, &vive_global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
}

