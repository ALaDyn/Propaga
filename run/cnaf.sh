#! /bin/bash

NUMERO_TOTALE_CORE_DA_USARE=64
USA_NODI_INTERI=true
RESTART=false
RESTART_STEP=0
TRACK_PARTICLES=false
NUMBER_OF_TRACKED_PARTICLES=10
DISTRIB_FILE="distribuzione.ppg"
CONFIG_FILE="parametri.json"

USE_GCC=true
#USE_GCC=false

NOME_ESEGUIBILE="./Propaga.exe"

job=job_propaga.cmd
job_name=propaga
queue=hpc_inf

###########################
rm -f $job 
touch $job
chmod 755 $job

argv1=${NUMERO_TOTALE_CORE_DA_USARE}
argv2=${RESTART_STEP}

echo "#BSUB -J ${job_name}" > $job # Job name
echo "#BSUB -o %J.out" >> $job # Job standard output
echo "#BSUB -e %J.err" >> $job # Job standard error
echo "#BSUB -q ${queue}" >> $job
echo "#BSUB -n ${NUMERO_TOTALE_CORE_DA_USARE}" >> $job
if [ "$USA_NODI_INTERI" = true ]
then
echo "#BSUB -R \"span[ptile=32]\"" >> $job
fi
if [ "$USE_GCC" = true ]
then
 echo "#BSUB -a openmpi" >> $job
 echo "module load compilers/openmpi-1.8.1_gcc-4.8.2" >> $job
else
 echo "module load compilers/ips-xe-2013-sp1" >> $job
 echo "module load compilers/intel-mpi" >> $job
fi
echo "module load compilers/gcc-4.8.2" >> $job
echo "ln -s ${DISTRIB_FILE} test.initialbunch.ppg" >> $job
echo "ln -s ${CONFIG_FILE} input.json" >> $job

if [ "$RESTART" = true ]
then
 if [ "${TRACK_PARTICLES}" = true ]
 then
  if [ "$USE_GCC" = true ]
  then
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf ${NOME_ESEGUIBILE} -restart $argv2 -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   else
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf env PSM_SHAREDCONTEXTS_MAX=8 ${NOME_ESEGUIBILE} -restart $argv2 -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   fi
  else
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv I_MPI_FABRICS shm:tmi ${NOME_ESEGUIBILE} -restart $argv2 -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   else
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv PSM_SHAREDCONTEXTS_MAX 8 -genv I_MPI_FABRICS shm:tmi ${NOME_ESEGUIBILE} -restart $argv2 -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   fi
  fi
 else
  if [ "$USE_GCC" = true ]
  then
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf ${NOME_ESEGUIBILE} -restart $argv2" >> $job
   else
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf env PSM_SHAREDCONTEXTS_MAX=8 ${NOME_ESEGUIBILE} -restart $argv2" >> $job
   fi
  else
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv I_MPI_FABRICS shm:tmi ${NOME_ESEGUIBILE} -restart $argv2" >> $job
   else
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv PSM_SHAREDCONTEXTS_MAX 8 -genv I_MPI_FABRICS shm:tmi ${NOME_ESEGUIBILE} -restart $argv2" >> $job
   fi
  fi
 fi
else
 if [ "${TRACK_PARTICLES}" = true ]
 then
  if [ "$USE_GCC" = true ]
  then
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf ${NOME_ESEGUIBILE} -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   else
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf env PSM_SHAREDCONTEXTS_MAX=8 ${NOME_ESEGUIBILE} -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   fi
  else
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv I_MPI_FABRICS shm:tmi  ${NOME_ESEGUIBILE} -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   else
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv PSM_SHAREDCONTEXTS_MAX 8 -genv I_MPI_FABRICS shm:tmi  ${NOME_ESEGUIBILE} -TRACK ${NUMBER_OF_TRACKED_PARTICLES}" >> $job
   fi
  fi
 else
  if [ "$USE_GCC" = true ]
  then
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf ${NOME_ESEGUIBILE}" >> $job
   else
    echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf env PSM_SHAREDCONTEXTS_MAX=8 ${NOME_ESEGUIBILE}" >> $job
   fi
  else
   if [ "$USA_NODI_INTERI" = true ]
   then
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv I_MPI_FABRICS shm:tmi  ${NOME_ESEGUIBILE}" >> $job
   else
    echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv PSM_SHAREDCONTEXTS_MAX 8 -genv I_MPI_FABRICS shm:tmi  ${NOME_ESEGUIBILE}" >> $job
   fi
  fi
 fi
fi
echo "./run_postprocess_data.sh" >> $job

echo "Lanciare il job con il seguente comando: "
echo "bsub < $job"

