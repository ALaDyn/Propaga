#!/usr/bin/env powershell

$filein         = "distribuzione.ppg"
$filein         = "input.json"
$ncpu           = 4
#############################
$CONVERTER="%USERPROFILE%\bin\converter.exe"
$GNUPLOT="C:\Program Files\gnuplot\bin\gnuplot.exe"
$MPIRUN="C:\Program Files\Microsoft MPI\Bin\mpiexec.exe"

Start-Process -FilePath $MPIRUN -ArgumentList "-n $ncpu Propaga.exe -f $filein -par $fileconf"

#copy from input.json
$NSTEPS="0020000"
$EMIN="0.00"
$EMAX="50.0"
$NBIN_ENERGY=100
$THETAMIN=0
$THETAMAX=100
$NBIN_ANGLE=100
$IMAGE_TYPE="png"
$IMAGE_TERMINAL="pngcairo"
$SIZEX=1280
$SIZEY=720
$XMIN="-20.0"
$XMAX="20.0"
$NBIN_X=80
$ZMIN="0.0"
$ZMAX="250.0"
$NBIN_Z=500
#####################
#np_over_nm
$WEIGHT2=1
#pjmp
$SUBSAMPLE_FACTOR=1
#===================
$MODE_CLEAN=9
$STATUS_PARTICLE_COL=9
$MODE_BIN1D=40
$MODE_BIN2D=39
$MODE_ANGLE_AND_ENERGY=41
$MODE_XYZE=45
$ENERGY_COL=7
$ANGLE_COL=3
$WEIGHT_COL_ENAN_FILE=-2
$WEIGHT_COL_DISTR_FILE=8
$X_COL=1
$Y_COL=2
$Z_COL=3
$PX_COL=4
$PY_COL=5
$PZ_COL=6
$ID_PARTICLE_COL=7


Start-Process -FilePath $CONVERTER -ArgumentList "1 1 ${MODE_CLEAN} test.${NSTEPS}.ppg ${STATUS_PARTICLE_COL}"
mv "conv.test.${NSTEPS}.ppg" "test.${NSTEPS}.pulito.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList "1 29979245800.0 ${MODE_ANGLE_AND_ENERGY} test.${NSTEPS}.pulito.ppg ${PX_COL} ${PY_COL} ${PZ_COL} ${ID_PARTICLE_COL} ${WEIGHT_COL_DISTR_FILE}"
mv "conv.test.${NSTEPS}.pulito.ppg" "test.${NSTEPS}.ENAN.ppg"
####
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN1D} test.${NSTEPS}.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${WEIGHT_COL_ENAN_FILE}"
mv "conv.test.${NSTEPS}.ENAN.ppg" "test.finalENERGY.binned.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN1D} test.${NSTEPS}.ENAN.ppg ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}"
mv "conv.test.${NSTEPS}.ENAN.ppg" "test.finalANGLE.binned.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN1D} test.${NSTEPS}.pulito.ppg ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}"
mv "conv.test.${NSTEPS}.pulito.ppg" "test.finalPOSITION.binned.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN2D} test.${NSTEPS}.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}"
mv "conv.test.${NSTEPS}.ENAN.ppg" "test.finalHEATMAP.binned.ppg"
#####
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_CLEAN} test.0000000.ppg ${STATUS_PARTICLE_COL}"
mv "conv.test.0000000.ppg" "test.0000000.pulito.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList  "1 29979245800.0 ${MODE_ANGLE_AND_ENERGY} test.0000000.pulito.ppg ${PX_COL} ${PY_COL} ${PZ_COL} ${ID_PARTICLE_COL} ${WEIGHT_COL_DISTR_FILE}"
mv "conv.test.0000000.pulito.ppg" "test.0000000.ENAN.ppg"
#####
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN1D} test.0000000.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${WEIGHT_COL_ENAN_FILE}"
mv "conv.test.0000000.ENAN.ppg" "test.initialENERGY.binned.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN1D} test.0000000.ENAN.ppg ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}"
mv "conv.test.0000000.ENAN.ppg" "test.initialANGLE.binned.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN1D} test.0000000.pulito.ppg ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}"
mv "conv.test.0000000.pulito.ppg" "test.initialPOSITION.binned.ppg"
Start-Process -FilePath $CONVERTER -ArgumentList  "1 1 ${MODE_BIN2D} test.0000000.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}"
mv "conv.test.0000000.ENAN.ppg" "test.initialHEATMAP.binned.ppg"
#####
$GNUPLOT_FILE_EN="spettriEnergia.plt"
"#!/gnuplot"                                                                                                                                     | Out-File -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"FILE_IN_1='test.initialENERGY.binned.ppg'"                                                                                                      | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"FILE_IN_3='test.finalENERGY.binned.ppg'"                                                                                                        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"FILE_OUT='spettriEnergia.{0}'" -f ${IMAGE_TYPE}                                                                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"set terminal {0} truecolor enhanced size {1},{2}" -f ${IMAGE_TERMINAL},${SIZEX},${SIZEY}                                                        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"set output FILE_OUT"                                                                                                                            | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"set xlabel 'E (MeV)'"                                                                                                                           | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"set ylabel 'dN/dE (MeV^{-1})'"                                                                                                                  | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"set format y '10^{%%L}'"                                                                                                                        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"set xrange[{0}:{1}]" -f ${EMIN},${EMAX}                                                                                                         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"set logscale y"                                                                                                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"plot FILE_IN_1 u ((`$1+`$2)*0.5):(`$3*{0}*{1}) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\" -f ${WEIGHT2},${SUBSAMPLE_FACTOR}         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
"FILE_IN_3 u ((`$1+`$2)*0.5):(`$3*{0}*{1}) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" -f ${WEIGHT2},${SUBSAMPLE_FACTOR}    | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EN}
#####
$GNUPLOT_FILE_AN="spettriAngolo.plt"
"#!/gnuplot"                                                                                                                                     | Out-File -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"FILE_IN_1='test.initialANGLE.binned.ppg'"                                                                                                       | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"FILE_IN_3='test.finalANGLE.binned.ppg'"                                                                                                         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"FILE_OUT='spettriAngolo.{0}'" -f ${IMAGE_TYPE}                                                                                                  | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"set terminal {0} truecolor enhanced size {1},{2}" -f ${IMAGE_TERMINAL},${SIZEX},${SIZEY}                                                        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"set output FILE_OUT"                                                                                                                            | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"set xlabel '{/Symbol w} (mrad)' "                                                                                                               | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"set ylabel 'dN/d{/Symbol w} (mrad^{-1})'"                                                                                                       | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"set format y '10^{%%L}'"                                                                                                                        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"set xrange[{0}:{1}]" -f ${THETAMIN},${THETAMAX}                                                                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"set logscale y"                                                                                                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"plot FILE_IN_1 u ((`$1+`$2)*0.5):(`$3*{0}*{1}) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\" -f ${WEIGHT2},${SUBSAMPLE_FACTOR}         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
"FILE_IN_3 u ((`$1+`$2)*0.5):(`$3*{0}*{1}) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" -f ${WEIGHT2},${SUBSAMPLE_FACTOR}    | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_AN}
#####
$GNUPLOT_FILE_POS="spettriPosizione.plt"
"#!/gnuplot"                                                                                                                                     | Out-File -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"FILE_IN_1='test.initialPOSITION.binned.ppg'"                                                                                                    | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"FILE_IN_3='test.finalPOSITION.binned.ppg'"                                                                                                      | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"FILE_OUT='spettriPosizione.{0}'" -f ${IMAGE_TYPE}                                                                                               | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"set terminal {0} truecolor enhanced size {1},{2}" -f ${IMAGE_TERMINAL},${SIZEX},${SIZEY}                                                        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"set output FILE_OUT"                                                                                                                            | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"set xlabel 'x (mm)' "                                                                                                                           | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"set ylabel 'dN/dx (mm^{-1})'"                                                                                                                   | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"set format y '10^{%%L}'"                                                                                                                        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"set xrange[{0}*10:{1}*10]" -f ${XMIN},${XMAX}                                                                                                   | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"set logscale y"                                                                                                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"plot FILE_IN_1 u ((`$1+`$2)*5):(`$3*{0}*{1}) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\" -f ${WEIGHT2},${SUBSAMPLE_FACTOR}           | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
"FILE_IN_3 u ((`$1+`$2)*5):(`$3*{0}*{1}) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" -f ${WEIGHT2},${SUBSAMPLE_FACTOR}      | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_POS}
#####
$GNUPLOT_FILE_EMITT="emittanza.plt"
"#!/gnuplot "                                                                                    | Out-File -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"file1='xyz.emitt.ppg' "                                                                         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"FILE_OUT='emittanza.{0}'" -f ${IMAGE_TYPE}                                                      | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"set terminal {0} truecolor enhanced size {1},{2}" -f ${IMAGE_TERMINAL},${SIZEX},${SIZEY}        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"set output FILE_OUT " `                                                                         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"set xlabel 'z_{mean} (cm)' "                                                                    | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"set ylabel '{/Symbol e} (mm mrad)' "                                                            | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"set xrange[0:200]"                                                                              | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"plot file1 u 6:(`$8*10000) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol e}_x',\"                  | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
"file1 u 6:(`$10*10000) w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol e}_y'"                       | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_EMITT}
######
$GNUPLOT_FILE_SIGMA="sigma.plt"
"#!/gnuplot "                                                                                    | Out-File -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"file1='xyz.emitt.ppg' "                                                                         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"FILE_OUT='sigma.{0}'" -f ${IMAGE_TYPE}                                                          | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"set terminal {0} truecolor enhanced size {1},{2}" -f ${IMAGE_TERMINAL},${SIZEX},${SIZEY}        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"set output FILE_OUT "                                                                           | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"set xlabel 'z_{mean} (cm)'  "                                                                   | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"set ylabel '{/Symbol s} (mm)' "                                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"set xrange[0:200] "                                                                             | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"plot file1  u 6:(`$7*10) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol s}_x',\"                    | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
"file1 u 6:(`$9*10)  w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol s}_y' "                         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_SIGMA}
#####}
$GNUPLOT_FILE_HEATMAP="heatmap.plt"
"#!/gnuplot "                                                                                                          | Out-File -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"FILE_IN_1='test.finalHEATMAP.binned.ppg' "                                                                            | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"FILE_OUT='heatmap.{0}'" -f ${IMAGE_TYPE}                                                                              | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set terminal {0} truecolor enhanced size {1},{2}" -f ${IMAGE_TERMINAL},${SIZEX},${SIZEY}                              | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set output FILE_OUT "                                                                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set title 'dN/(dEd{/Symbol W}) (MeV^{-1} mrad^{-1})' "                                                                | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set xlabel 'E (MeV)' "                                                                                                | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set ylabel '{/Symbol q} (mrad)' "                                                                                     | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set xrange[{0}:{1}]" -f ${EMIN},${EMAX}                                                                               | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set yrange[{0}:{1}]" -f ${THETAMIN},${THETAMAX}                                                                       | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set palette rgbformulae 22,13,10 "                                                                                    | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set logscale cb "                                                                                                     | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"set format cb '10^{%%L}' "                                                                                            | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
"plot FILE_IN_1 u ((`$1+`$2)*0.5):((`$3+`$4)*0.5):(`$5*{0}*{1}) w image notitle " -f ${WEIGHT2},${SUBSAMPLE_FACTOR}    | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_HEATMAP}
######
$GNUPLOT_FILE_ENAN="enan.plt"
"#!/gnuplot "                                                                                    | Out-File -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"FILE_IN_1='test.{0}.ENAN.ppg'" -f ${NSTEPS}                                                     | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"FILE_OUT='enan.{0}'" -f ${IMAGE_TYPE}                                                           | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"set terminal {0} truecolor enhanced size {1},{2}" -f ${IMAGE_TERMINAL},${SIZEX},${SIZEY}        | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"set output FILE_OUT "                                                                           | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"set title 'Energy-angle correlation plot' "                                                     | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"set xlabel 'E (MeV)' "                                                                          | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"set ylabel '{/Symbol q} (mrad)' "                                                               | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"set xrange[{0}:{1}]" -f ${EMIN},${EMAX}                                                         | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"set yrange[{0}:{1}]" -f ${THETAMIN},${THETAMAX}                                                 | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
"plot FILE_IN_1 u {0}:{1} w points ps 0.5 lc rgb 'blue' " -f ${ENERGY_COL},${ANGLE_COL}          | Out-File -Append -Encoding "ASCII" ${GNUPLOT_FILE_ENAN}
#######
Start-Process -FilePath $GNUPLOT -ArgumentList ${GNUPLOT_FILE_ENAN}
Start-Process -FilePath $GNUPLOT -ArgumentList ${GNUPLOT_FILE_EMITT}
Start-Process -FilePath $GNUPLOT -ArgumentList ${GNUPLOT_FILE_SIGMA}
Start-Process -FilePath $GNUPLOT -ArgumentList ${GNUPLOT_FILE_AN}
Start-Process -FilePath $GNUPLOT -ArgumentList ${GNUPLOT_FILE_EN}
Start-Process -FilePath $GNUPLOT -ArgumentList ${GNUPLOT_FILE_POS}
Start-Process -FilePath $GNUPLOT -ArgumentList ${GNUPLOT_FILE_HEATMAP}


