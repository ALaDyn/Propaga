COMPILER = mpic++
EXE      = bin/Propaga.exe
OPT      = -O3 -std=c++11

OBJECTS  = obj/particle.o obj/magnetic_element.o obj/lattice.o obj/parameters.o obj/Propaga.o

all: dirtree $(EXE)

debug:	OPT = -std=c++11 -Og -g 
debug:	dirtree $(EXE)

no-openmp: OPT += -DNO_OMP
no-openmp: dirtree $(EXE)

openmp:	OPT += -fopenmp
openmp:	dirtree $(EXE)

CINECA:	OPT += -DCINECA
CINECA:	dirtree $(EXE)

openmp-intel:	OPT += -openmp 
openmp-intel:	OPT += -ipo -march=core-avx-i 
openmp-intel:	COMPILER = mpiicpc
openmp-intel:	dirtree $(EXE)

intel:	OPT += -ipo -march=core-avx-i
intel:	COMPILER = mpiicpc
intel:	dirtree $(EXE)

ibm_gnu: OPT = -O3 -std=c++0x -DCINECA
ibm_gnu: COMPILER = mpic++
ibm_gnu: dirtree $(EXE)

ibm_xl:  OPT = -qlanglvl=extended0x -qsmp=omp -O3 -qarch=qp -qtune=qp -qmaxmem=-1 -DCINECA
ibm_xl:  COMPILER = mpixlcxx_r
ibm_xl:  dirtree $(EXE)

dirtree:
	@mkdir -p obj
	@mkdir -p bin

$(EXE): $(OBJECTS)
	$(COMPILER) $(OPT) -o $(EXE) $(OBJECTS)

obj/particle.o: src/particle.cpp src/particle.h src/definitions.h
	$(COMPILER) $(OPT) -c src/particle.cpp -o obj/particle.o

obj/magnetic_element.o: src/magnetic_element.cpp src/magnetic_element.h src/definitions.h src/particle.h
	$(COMPILER) $(OPT) -c src/magnetic_element.cpp -o obj/magnetic_element.o

obj/lattice.o: src/lattice.cpp src/lattice.h src/definitions.h src/magnetic_element.h
	$(COMPILER) $(OPT) -c src/lattice.cpp -o obj/lattice.o

obj/parameters.o: src/parameters.cpp src/parameters.h src/definitions.h src/magnetic_element.h src/lattice.h src/particle.h
	$(COMPILER) $(OPT) -I./src/jsoncons/src -c src/parameters.cpp -o obj/parameters.o

obj/Propaga.o: src/Propaga.cpp src/magnetic_element.h src/definitions.h
	$(COMPILER) $(OPT) -I./src/jsoncons/src -c src/Propaga.cpp -o obj/Propaga.o

clean:
	rm -f $(OBJECTS)

cleanall:
	rm -rf bin obj 


