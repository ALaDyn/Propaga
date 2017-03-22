[![Build Status Master](https://travis-ci.org/ALaDyn/Propaga.png?branch=master)](https://travis-ci.org/ALaDyn/Propaga "master")

Copyright 2010-2017 Stefano Sinigardi, Graziano Servizi, Giorgio Turchetti  
*Uses [jsoncons library](https://github.com/danielaparker/jsoncons)*

`Propaga` is a C++ code developed by the Physics of Complex System group, inside the Physics and Astronomy Department at the Bologna University, to study the three dimensional propagation of a particle bunch along a transfer line, composed by many different lattice elements.  
If you want to use it, you're kindly requested to inform the authors and cite this Zenodo DOI:
[![DOI](https://zenodo.org/badge/4711/ALaDyn/Propaga.svg)](https://zenodo.org/badge/latestdoi/4711/ALaDyn/Propaga)

CMake 3.2, MPI libraries and a C++11-compatible compiler are required.  
On Windows, use [vcpkg](https://github.com/Microsoft/vcpkg/) to install libraries and [chocolatey](https://github.com/chocolatey/choco) to install tools for best results.  
On Mac, use [homebrew](https://github.com/Homebrew/brew) to install libraries and tools for best results.  
On Linux, please use your package manager (yum, dnf, apt, ...) to install libraries and tools.  

In order to work, beside the executable, you should have at least two other important files: a json parameter file (default: input.json) and a phase-space distribution particle file (default test.initialbunch.ppg). Of course these default names can be varied, providing the correct path to Propaga through the following flags:

+ use `-par` to define the name of the json file
+ use `-f` to define the name of the phase-space distribution particle file

There are other useful command line parameters that you can give to the executable:

+ use `-restart xxx` to tell the program that we want to restart from time-step=xxx (of course the dump must be present in the folder)
+ use `-LASTSLOWEST` to tell the program that the last particle in the input file is the reference particle, or to say, the slowest particle that you keep as a reference to wait for when deciding how to terminate the simulation. This particle, in this case, will be discarded in the bunch parameters calculations
+ use `-o` to define the name of the simulation. It will be used at the beginning of any file written during the simulation.
+ use `-override_ordinal` to discard the particle id column in the input distribution file (10th column, if present).
+ use `-OUT xxx` to define a time-step=xxx at which you want the full output.
+ use `-OUTZ xxx` to define a *z* position at which you want an output. When a particle cross that invisible boundary, it will be dumped on a special file (`sim_name.9999999.ppg`), with the 7th column being the time-step at which they have been dumped and *not* their type!
+ use `-TRACK nn` to obtain the full phase-space of the first #nn particles during the whole simulation, all together in the same file `sim_name.tracked.ppg`. It is useful to create heatmaps of the particle density along the transfer line


## Json file description ##
The input parameters for the simulation are driven by a single json file.

+ `emin`, `emax`: if you define a negative value, no minimum/maximum energy is defined. Particle outside this energy band will be flagged as lost, but their evolution will proceed anyway (being lost means that they are excluded from bunch properties calculations, but not from full dumps, where in the nineth column their flag will be set to `1`)
+ `dt`: interval (in units of ct [cm]) between the 4th order Runge-Kutta integrator steps
+ `steps_between_dumps`: number of steps between full data dumps (if 0, no dumps will be done)
+ `steps_between_diags`: number of steps between diagnostics records
+ `Magnetic_elements`: an array of json elements defining the lattice characteristics

This is an example of a working json file

```
{
  "emin": -1,
  "emax": -1,
  "dt": 0.1,
  "steps_between_dumps": 5000,
  "steps_between_diags": 50,
  "Magnetic_elements":[
    {
      "type": "J",
      "begin": 1.0,
      "end": 1.0,
      "par_01": 0.05
    },
    {
      "type": "M",
      "begin": 2.0,
      "end": 8.0,
      "par_01": 5.00,
      "par_02": 2.0
    },
    {
      "type": "J",
      "begin": 50.0,
      "end": 50.1,
      "par_01": 0.05
    }
  ]
}
```

Of course `Magnetic_elements` can be customized according to your needs (the previous was just an example of a solenoid between two circular selectors)

### Magnetic_elements "type" codes and params ###
Every `Magnetic_elements` json definition shares the same identical basic structure. You have to define the `type`, the initial z position `begin`, the final z position `end` and few other parameters, if required by the element, as follows:

+ `"type" : "O"`: DRIFT: useful to define the minimum *z* the simulation should reach, through the parameter `end`. If the lattice is dense of objects to the end, no need to define it. Note that particles outside any other define element are drifting even if no drift is defined.
+ `"type" : "M"`: SOLENOID
  - `par_01`: peak magnetic field inside the solenoid, in T
  - `par_02`: exponential decay factor of the magnetic field on the element fringes
+ `"type" : "F"`: FOCUSING QUADRUPOLE
  - `par_01`: magnetic gradient inside the quadrupole, in T/m
+ `"type" : "D"`: DEFOCUSING QUADRUPOLE
  - `par_01`: magnetic gradient inside the quadrupole, in T/m
+ `"type" : "J"`: CIRCULAR SELECTOR
  - `par_01`: selector radius, in cm
+ `"type" : "K"`: SELECTOR
  - `par_01`: aperture starting point, along x, in cm
  - `par_02`: aperture ending point, along x, in cm
+ `"type" : "P"`: BENDING MAGNET (PRE)
  - `par_01`: bending magnetic field along y, uniform, in T (bends in the xz plane)
+ `"type" : "Q"`: BENDING MAGNET (POST)
  - `par_01`: bending magnetic field along -y, uniform, in T (the field is reversed with respect to element `"type" : "P"`, in order to write always the same number)
+ `"type" : "T"`: CHICANE PARTICLE SELECTOR
  - `par_01`: transverse position (along x) of the selector, in cm
  - `par_02`: aperture of the selector, in cm

#### deprecated or untested elements ####

+ `"type" : "S"`: SOLENOID
  - `par_01`: uniform magnetic field inside the solenoid, in T
+ `"type" : "X"`: SOLENOID (ENTRANCE/EXIT SINGLE KICK MODEL)
  - `par_01`: uniform magnetic field inside the solenoid, in T
+ `"type" : "I"`: CIRCULAR SELECTOR (DEPRECATED)
  - `par_01`: selector radius, in cm
+ `"type" : "R"`: CHICANE PARTICLE SELECTOR (DEPRECATED)
  - `par_01`: transverse position (along x) of the selector, in cm
  - `par_02`: aperture of the selector, in cm

#### work-in-progress elements ####

+ `"type" : "V"`: RF CAVITY (ASTRA TM-CAVITY MODEL)
  - `par_01`: standing wave frequency, in MHz
  - `par_02`: maximum cavity field amplitude, in MV/m
  - `par_03`: wavelength (in cm)
  - `par_04`: initial wave phase (in degrees)
+ `"type" : "W"`: RF CAVITY
  - `par_01`: standing wave frequency, in MHz
  - `par_02`: maximum cavity field amplitude, in MV/m
  - `par_03`: wavelength (in cm)
  - `par_04`: initial wave phase (in degrees)


## Particle distribution file description ##
Each line defines a different particle, with fields in this order:
```
x  y  z  x'  y'  z'  type   weight   flag   #id
```
where `x,y,z` are in cm, `x',y',z'` are particle momenta normalized by mass*c, `type` follows FLUKA conventions, `flag` says if the particle is alive (0) or not and `#id` is just a progressive number to identify univocally each one of them from the beginning to the end of the simulation, even if shuffled inside the file

```
-8.375e-04  -9.600e-04  2.703e-03   -1.120e-03  -1.348e-03  4.619e-02   1    0.225   0     1
-8.398e-04  -9.588e-04  2.703e-03   -1.108e-03  -1.366e-03  4.618e-02   1    0.225   0     2
-8.386e-04  -9.588e-04  2.703e-03   -1.094e-03  -1.346e-03  4.617e-02   1    0.225   0     3
-8.375e-04  -9.588e-04  2.703e-03   -1.109e-03  -1.328e-03  4.621e-02   1    0.225   0     4
```

## Papers containing Propaga simulations ##

1) P. Londrillo, G. Servizi, A. Sgattoni, S. Sinigardi, M. Sumini, G. Turchetti, Protons Acceleration by CO2 Laser Pulses and Perspectives for Medical Applications,
CO2 Laser-Optimisation and Application (2012)  
[doi:10.5772/38882](http://www.intechopen.com/books/co2-laser-optimisation-and-application/protons-acceleration-from-co2-laser-pulses-for-biomedical-applications)

2) G. Turchetti, S. Sinigardi, P. Londrillo, F. Rossi, M. Sumini, D. Giove, C. De Martinis, The LILIA experiment: Energy selection and post-acceleration of laser generated protons, AIP Conference Proceedings, 1507, 820-824 (2012)  
[doi:10.1063/1.4773804](http://dx.doi.org/10.1063/1.4773804)

3) S. Sinigardi, G. Turchetti, P. Londrillo, F. Rossi, D. Giove, C. De Martinis, M. Sumini, Transport and energy selection of laser generated protons for postacceleration with a compact linac, Phys. Rev. ST Accel. Beams 16, 3, 031301 (2013)  
[doi:10.1103/PhysRevSTAB.16.031301](http://link.aps.org/doi/10.1103/PhysRevSTAB.16.031301)

4) S. Sinigardi, P. Londrillo, F. Rossi, G. Turchetti, P. R. Bolton, Post-acceleration of laser driven protons with a compact high field linac, Proc. SPIE 8779, 87791J (2013)  
[doi:10.1117/12.2017235](http://dx.doi.org/10.1117/12.2017235)

5) S. Sinigardi, G. Turchetti, F. Rossi, P. Londrillo, D. Giove, C. De Martinis, P. R. Bolton, High quality proton beams from hybrid integrated laser-driven ion acceleration systems, Nucl. Instr. Meth. A 740, 99-104 (2014)  
[doi:10.1016/j.nima.2013.10.080](http://www.sciencedirect.com/science/article/pii/S0168900213014873)
