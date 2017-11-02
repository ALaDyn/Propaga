#! /bin/bash

NUMERO_TOTALE_CORE_DA_USARE=16
USA_NODI_INTERI=false
CONFIG_FILE_BASENAME="parametri"

USE_GCC=true
#USE_GCC=false

NOME_ESEGUIBILE=$(which Propaga)

job=job_propaga.cmd
job_name=propaga
queue=hpc_inf

###########################

DO_SCAN=true
START_SCAN=1
STEP_SCAN=1
END_SCAN=5

scan_range=$(seq ${START_SCAN} ${STEP_SCAN} ${END_SCAN})
for scan_value in ${scan_range[*]} ; do
  rm -f $job
  touch $job
  chmod 755 $job

  if [ "$DO_SCAN" = true ] ; then
    CONFIG_FILE=${CONFIG_FILE_BASENAME}_${scan_value}.json
  else
    CONFIG_FILE=${CONFIG_FILE_BASENAME}.json
  fi

  {
    echo "#BSUB -J ${job_name}"
    echo "#BSUB -o %J.out"
    echo "#BSUB -e %J.err"
    echo "#BSUB -q ${queue}"
    echo "#BSUB -n ${NUMERO_TOTALE_CORE_DA_USARE}"
    if [ "$USA_NODI_INTERI" = true ] ; then
      echo "#BSUB -R \"span[ptile=32]\""
    fi
    if [ "$USE_GCC" = true ] ; then
      echo "#BSUB -a openmpi"
    fi
    echo "module purge"
    echo "module load compilers/gcc-4.9.0"
    if [ "$USE_GCC" = true ] ; then
      echo "module load compilers/openmpi-1.8.1_gcc-4.9.0_with_cuda6.5"
    else
      echo "module load compilers/intel-parallel-studio-2017"
    fi
    if [ "$USE_GCC" = true ] ; then
      if [ "$USA_NODI_INTERI" = true ] ; then
        echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf ${NOME_ESEGUIBILE} ${CONFIG_FILE}"
      else
        echo "/usr/share/lsf/9.1/linux2.6-glibc2.3-x86_64/bin/mpirun.lsf env PSM_SHAREDCONTEXTS_MAX=8 ${NOME_ESEGUIBILE} ${CONFIG_FILE}"
      fi
    else
      if [ "$USA_NODI_INTERI" = true ] ; then
        echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv I_MPI_FABRICS shm:tmi  ${NOME_ESEGUIBILE} ${CONFIG_FILE}"
      else
        echo "/shared/software/compilers/impi/intel64/bin/mpirun -np ${NUMERO_TOTALE_CORE_DA_USARE} -genv PSM_SHAREDCONTEXTS_MAX 8 -genv I_MPI_FABRICS shm:tmi  ${NOME_ESEGUIBILE} ${CONFIG_FILE}"
      fi
    fi
    #echo "./run_postprocess_data.sh"
  } >> $job

  bsub < $job

done
