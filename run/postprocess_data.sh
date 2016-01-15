#! /bin/bash

#copy from input.json
NSTEPS=$(ls -1 test.???????.ppg | tail -1) ; NSTEPS=${NSTEPS#*.} ; NSTEPS=${NSTEPS%.*} 

CONVERTER=converter
GNUPLOT=gnuplot

FAI_TRACCE=0
EMIN=0.00
EMAX=60.0
NBIN_ENERGY=60
THETAMIN="0"
THETAMAX="100"
NBIN_ANGLE=100
IMAGE_TYPE=png
IMAGE_TERMINAL=pngcairo
SIZEX=1280
SIZEY=720
XMIN="-20.0"
XMAX="20.0"
NBIN_X=80
ZMIN="0.0"
ZMAX="250.0"
NBIN_Z=500
#####################
#np_over_nm
WEIGHT2=21260
#pjmp
SUBSAMPLE_FACTOR=8
#===================
MODE_CLEAN=9
STATUS_PARTICLE_COL=9
MODE_BIN1D=40
MODE_BIN2D=39
MODE_ANGLE_AND_ENERGY=41
MODE_XYZE=45
ENERGY_COL=7
ANGLE_COL=3
WEIGHT_COL_ENAN_FILE=-2
WEIGHT_COL_DISTR_FILE=8
X_COL=1
Y_COL=2
Z_COL=3
PX_COL=4
PY_COL=5
PZ_COL=6
ID_PARTICLE_COL=7
#####
#rm -f *.plt
rm -f *.${IMAGE_TYPE}
#####
$CONVERTER 1 1 ${MODE_CLEAN} test.${NSTEPS}.ppg ${STATUS_PARTICLE_COL}
mv conv.test.${NSTEPS}.ppg test.${NSTEPS}.pulito.ppg
$CONVERTER 1 29979245800.0 ${MODE_ANGLE_AND_ENERGY} test.${NSTEPS}.pulito.ppg ${PX_COL} ${PY_COL} ${PZ_COL} ${ID_PARTICLE_COL} ${WEIGHT_COL_DISTR_FILE}
mv conv.test.${NSTEPS}.pulito.ppg test.${NSTEPS}.ENAN.ppg
#####
$CONVERTER 1 1 ${MODE_BIN1D} test.${NSTEPS}.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.test.${NSTEPS}.ENAN.ppg test.finalENERGY.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} test.${NSTEPS}.ENAN.ppg ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.test.${NSTEPS}.ENAN.ppg test.finalANGLE.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} test.${NSTEPS}.pulito.ppg ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}
mv conv.test.${NSTEPS}.pulito.ppg test.finalPOSITION.binned.ppg
$CONVERTER 1 1 ${MODE_BIN2D} test.${NSTEPS}.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.test.${NSTEPS}.ENAN.ppg test.finalHEATMAP.binned.ppg
#####
$CONVERTER 1 1 ${MODE_CLEAN} test.0000000.ppg ${STATUS_PARTICLE_COL}
mv conv.test.0000000.ppg test.0000000.pulito.ppg
$CONVERTER 1 29979245800.0 ${MODE_ANGLE_AND_ENERGY} test.0000000.pulito.ppg ${PX_COL} ${PY_COL} ${PZ_COL} ${ID_PARTICLE_COL} ${WEIGHT_COL_DISTR_FILE}
mv conv.test.0000000.pulito.ppg test.0000000.ENAN.ppg
#####
$CONVERTER 1 1 ${MODE_BIN1D} test.0000000.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.test.0000000.ENAN.ppg test.initialENERGY.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} test.0000000.ENAN.ppg ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.test.0000000.ENAN.ppg test.initialANGLE.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} test.0000000.pulito.ppg ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}
mv conv.test.0000000.pulito.ppg test.initialPOSITION.binned.ppg
$CONVERTER 1 1 ${MODE_BIN2D} test.0000000.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.test.0000000.ENAN.ppg test.initialHEATMAP.binned.ppg
#####
if [ ${FAI_TRACCE} -eq 1  ] ; then
$CONVERTER 1 1 ${MODE_BIN2D} test.tracked.ppg ${Z_COL} ${NBIN_Z} ${ZMIN} ${ZMAX} ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}
mv conv.test.tracked.ppg test.trackedHEATMAP.binned.ppg
fi
#####
GNUPLOT_FILE_EN="spettriEnergia.plt"
rm -f ${GNUPLOT_FILE_EN}
touch ${GNUPLOT_FILE_EN}
chmod 775 ${GNUPLOT_FILE_EN}
printf "#!/gnuplot\n" >> ${GNUPLOT_FILE_EN}
printf "FILE_IN_1='test.initialENERGY.binned.ppg'\n" >> ${GNUPLOT_FILE_EN}
printf "FILE_IN_3='test.finalENERGY.binned.ppg'\n" >> ${GNUPLOT_FILE_EN}
printf "FILE_OUT='spettriEnergia.${IMAGE_TYPE}'\n" >> ${GNUPLOT_FILE_EN}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY}\n" >> ${GNUPLOT_FILE_EN}
printf "set output FILE_OUT\n" >> ${GNUPLOT_FILE_EN}
printf "set xlabel 'E (MeV)' \n" >> ${GNUPLOT_FILE_EN}
printf "set ylabel 'dN/dE (MeV^{-1})'\n" >> ${GNUPLOT_FILE_EN}
printf "set format y '10^{%%L}'\n" >> ${GNUPLOT_FILE_EN}
#printf "set xrange[%s:%s]\n" "0" "60" >> ${GNUPLOT_FILE_EN}
printf "set xrange[%s:%s]\n" "${EMIN}" "${EMAX}" >> ${GNUPLOT_FILE_EN}
printf "set logscale y\n" >> ${GNUPLOT_FILE_EN}
printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_EN}
printf "\n" >> ${GNUPLOT_FILE_EN}
printf "FILE_IN_3 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_EN}
#####
GNUPLOT_FILE_AN="spettriAngolo.plt"
rm -f ${GNUPLOT_FILE_AN}
touch ${GNUPLOT_FILE_AN}
chmod 775 ${GNUPLOT_FILE_AN}
printf "#!/gnuplot\n" >> ${GNUPLOT_FILE_AN}
printf "FILE_IN_1='test.initialANGLE.binned.ppg'\n" >> ${GNUPLOT_FILE_AN}
printf "FILE_IN_3='test.finalANGLE.binned.ppg'\n" >> ${GNUPLOT_FILE_AN}
printf "FILE_OUT='spettriAngolo.${IMAGE_TYPE}'\n" >> ${GNUPLOT_FILE_AN}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY}\n" >> ${GNUPLOT_FILE_AN}
printf "set output FILE_OUT\n" >> ${GNUPLOT_FILE_AN}
printf "set xlabel '{/Symbol w} (mrad)' \n" >> ${GNUPLOT_FILE_AN}
printf "set ylabel 'dN/d{/Symbol w} (mrad^{-1})'\n" >> ${GNUPLOT_FILE_AN}
printf "set format y '10^{%%L}'\n" >> ${GNUPLOT_FILE_AN}
#printf "set xrange[%s:%s]\n" "-50" "50" >> ${GNUPLOT_FILE_AN}
printf "set xrange[%s:%s]\n" "${THETAMIN}" "${THETAMAX}" >> ${GNUPLOT_FILE_AN}
printf "set logscale y\n" >> ${GNUPLOT_FILE_AN}
printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_AN}
printf "\n" >> ${GNUPLOT_FILE_AN}
printf "FILE_IN_3 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_AN}
#####
GNUPLOT_FILE_POS="spettriPosizione.plt"
rm -f ${GNUPLOT_FILE_POS}
touch ${GNUPLOT_FILE_POS}
chmod 775 ${GNUPLOT_FILE_POS}
printf "#!/gnuplot\n" >> ${GNUPLOT_FILE_POS}
printf "FILE_IN_1='test.initialPOSITION.binned.ppg'\n" >> ${GNUPLOT_FILE_POS}
printf "FILE_IN_3='test.finalPOSITION.binned.ppg'\n" >> ${GNUPLOT_FILE_POS}
printf "FILE_OUT='spettriPosizione.${IMAGE_TYPE}'\n" >> ${GNUPLOT_FILE_POS}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY}\n" >> ${GNUPLOT_FILE_POS}
printf "set output FILE_OUT\n" >> ${GNUPLOT_FILE_POS}
printf "set xlabel 'x (mm)' \n" >> ${GNUPLOT_FILE_POS}
printf "set ylabel 'dN/dx (mm^{-1})'\n" >> ${GNUPLOT_FILE_POS}
printf "set format y '10^{%%L}'\n" >> ${GNUPLOT_FILE_POS}
#printf "set xrange[%s:%s]\n" "0" "5" >> ${GNUPLOT_FILE_POS}
printf "set xrange[%s*10:%s*10]\n" "${XMIN}" "${XMAX}" >> ${GNUPLOT_FILE_POS}
printf "set logscale y\n" >> ${GNUPLOT_FILE_POS}
printf "plot FILE_IN_1 u ((\$1+\$2)*5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_POS}
printf "\n" >> ${GNUPLOT_FILE_POS}
printf "FILE_IN_3 u ((\$1+\$2)*5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_POS}
#####
GNUPLOT_FILE_EMITT="emittanza.plt"
rm -f ${GNUPLOT_FILE_EMITT}
touch ${GNUPLOT_FILE_EMITT}
chmod 775 ${GNUPLOT_FILE_EMITT}
printf "#!/gnuplot \n" >> ${GNUPLOT_FILE_EMITT}
printf "file1='test.emitt.ppg' \n" >> ${GNUPLOT_FILE_EMITT}
printf "FILE_OUT='emittanza.${IMAGE_TYPE}'\n" >> ${GNUPLOT_FILE_EMITT}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY}\n" >> ${GNUPLOT_FILE_EMITT}
printf "set output FILE_OUT \n" >> ${GNUPLOT_FILE_EMITT}
printf "set xlabel 'z_{mean} (cm)' \n" >> ${GNUPLOT_FILE_EMITT}
printf "set ylabel '{/Symbol e} (mm mrad)' \n" >> ${GNUPLOT_FILE_EMITT}
#printf "set yrange[0:50]\n" >> ${GNUPLOT_FILE_EMITT}
printf "set xrange[0:200]\n" >> ${GNUPLOT_FILE_EMITT}
printf "plot file1 u 6:(\$8*10000) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol e}_x',\\" >> ${GNUPLOT_FILE_EMITT}
printf "\n" >> ${GNUPLOT_FILE_EMITT}
printf "file1 u 6:(\$10*10000) w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol e}_y'\n" >> ${GNUPLOT_FILE_EMITT}
######
GNUPLOT_FILE_SIGMA="sigma.plt"
rm -f ${GNUPLOT_FILE_SIGMA}
touch ${GNUPLOT_FILE_SIGMA}
chmod 775 ${GNUPLOT_FILE_SIGMA}
printf "#!/gnuplot \n" >> ${GNUPLOT_FILE_SIGMA}
printf "file1='test.emitt.ppg' \n" >> ${GNUPLOT_FILE_SIGMA}
printf "FILE_OUT='sigma.${IMAGE_TYPE}'\n" >> ${GNUPLOT_FILE_SIGMA}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY}\n" >> ${GNUPLOT_FILE_SIGMA}
printf "set output FILE_OUT \n" >> ${GNUPLOT_FILE_SIGMA}
printf "set xlabel 'z_{mean} (cm)'  \n" >> ${GNUPLOT_FILE_SIGMA}
printf "set ylabel '{/Symbol s} (mm)' \n" >> ${GNUPLOT_FILE_SIGMA}
#printf "set yrange[0:20] \n" >> ${GNUPLOT_FILE_SIGMA}
printf "set xrange[0:200] \n" >> ${GNUPLOT_FILE_SIGMA}
printf "plot file1  u 6:(\$7*10) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol s}_x',\\" >> ${GNUPLOT_FILE_SIGMA}
printf "\n" >> ${GNUPLOT_FILE_SIGMA}
printf "file1 u 6:(\$9*10)  w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol s}_y' \n" >> ${GNUPLOT_FILE_SIGMA}
######
GNUPLOT_FILE_HEATMAP="heatmap.plt"
rm -f ${GNUPLOT_FILE_HEATMAP}
touch ${GNUPLOT_FILE_HEATMAP}
chmod 775 ${GNUPLOT_FILE_HEATMAP}
printf "#!/gnuplot \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "FILE_IN_1='test.finalHEATMAP.binned.ppg' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "FILE_OUT='finalheatmap.${IMAGE_TYPE}' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY} \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set output FILE_OUT \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set title 'dN/(dEd{/Symbol W}) (MeV^{-1} mrad^{-1})' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set xlabel 'E (MeV)' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set ylabel '{/Symbol q} (mrad)' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set xrange[%s:%s]\n" "${EMIN}" "${EMAX}" >> ${GNUPLOT_FILE_HEATMAP}
printf "set yrange[%s:%s]\n" "${THETAMIN}" "${THETAMAX}" >> ${GNUPLOT_FILE_HEATMAP}
printf "set palette rgbformulae 22,13,10 \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set logscale cb \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set format cb '10^{%%L}' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):((\$3+\$4)*0.5):(\$5*%s*%s) w image notitle \n" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_HEATMAP}
######
GNUPLOT_FILE_HEATMAP="heatmap.plt"
rm -f ${GNUPLOT_FILE_HEATMAP}
touch ${GNUPLOT_FILE_HEATMAP}
chmod 775 ${GNUPLOT_FILE_HEATMAP}
printf "#!/gnuplot \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "FILE_IN_1='test.initialHEATMAP.binned.ppg' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "FILE_OUT='initialheatmap.${IMAGE_TYPE}' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY} \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set output FILE_OUT \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set title 'dN/(dEd{/Symbol W}) (MeV^{-1} mrad^{-1})' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set xlabel 'E (MeV)' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set ylabel '{/Symbol q} (mrad)' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set xrange[%s:%s]\n" "${EMIN}" "${EMAX}" >> ${GNUPLOT_FILE_HEATMAP}
printf "set yrange[%s:%s]\n" "${THETAMIN}" "${THETAMAX}" >> ${GNUPLOT_FILE_HEATMAP}
printf "set palette rgbformulae 22,13,10 \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set logscale cb \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "set format cb '10^{%%L}' \n" >> ${GNUPLOT_FILE_HEATMAP}
printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):((\$3+\$4)*0.5):(\$5*%s*%s) w image notitle \n" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}" >> ${GNUPLOT_FILE_HEATMAP}
######
GNUPLOT_FILE_ENAN="enan.plt"
rm -f ${GNUPLOT_FILE_ENAN}
touch ${GNUPLOT_FILE_ENAN}
chmod 775 ${GNUPLOT_FILE_ENAN}
printf "#!/gnuplot \n" >> ${GNUPLOT_FILE_ENAN}
printf "FILE_IN_1='test.${NSTEPS}.ENAN.ppg' \n" >> ${GNUPLOT_FILE_ENAN}
printf "FILE_OUT='finalenan.${IMAGE_TYPE}' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY} \n" >> ${GNUPLOT_FILE_ENAN}
printf "set output FILE_OUT \n" >> ${GNUPLOT_FILE_ENAN}
printf "set title 'Energy-angle correlation plot' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set xlabel 'E (MeV)' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set ylabel '{/Symbol q} (mrad)' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set xrange[%s:%s]\n" "${EMIN}" "${EMAX}" >> ${GNUPLOT_FILE_ENAN}
printf "set yrange[%s:%s]\n" "${THETAMIN}" "${THETAMAX}" >> ${GNUPLOT_FILE_ENAN}
printf "plot FILE_IN_1 u ${ENERGY_COL}:${ANGLE_COL} w points ps 0.5 lc rgb 'blue' \n" >> ${GNUPLOT_FILE_ENAN}
#######
GNUPLOT_FILE_ENAN="enan.plt"
rm -f ${GNUPLOT_FILE_ENAN}
touch ${GNUPLOT_FILE_ENAN}
chmod 775 ${GNUPLOT_FILE_ENAN}
printf "#!/gnuplot \n" >> ${GNUPLOT_FILE_ENAN}
printf "FILE_IN_1='test.0000000.ENAN.ppg' \n" >> ${GNUPLOT_FILE_ENAN}
printf "FILE_OUT='initialenan.${IMAGE_TYPE}' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY} \n" >> ${GNUPLOT_FILE_ENAN}
printf "set output FILE_OUT \n" >> ${GNUPLOT_FILE_ENAN}
printf "set title 'Energy-angle correlation plot' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set xlabel 'E (MeV)' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set ylabel '{/Symbol q} (mrad)' \n" >> ${GNUPLOT_FILE_ENAN}
printf "set xrange[%s:%s]\n" "${EMIN}" "${EMAX}" >> ${GNUPLOT_FILE_ENAN}
printf "set yrange[%s:%s]\n" "${THETAMIN}" "${THETAMAX}" >> ${GNUPLOT_FILE_ENAN}
printf "plot FILE_IN_1 u ${ENERGY_COL}:${ANGLE_COL} w points ps 0.5 lc rgb 'blue' \n" >> ${GNUPLOT_FILE_ENAN}
#######
if [ ${FAI_TRACCE} -eq 1  ] ; then
GNUPLOT_FILE_TH="tracks_heatmap.plt"
rm -f ${GNUPLOT_FILE_TH}
touch ${GNUPLOT_FILE_TH}
chmod 775 ${GNUPLOT_FILE_TH}
printf "#!/gnuplot\n" >> ${GNUPLOT_FILE_TH}
printf "set terminal ${IMAGE_TERMINAL} truecolor enhanced size ${SIZEX},${SIZEY}\n" >> ${GNUPLOT_FILE_TH}
printf "FILE_IN='test.trackedHEATMAP.binned.ppg' \n" >> ${GNUPLOT_FILE_TH}
printf "FILE_OUT='tracks_heatmap.png' \n" >> ${GNUPLOT_FILE_TH}
printf "set output FILE_OUT\n" >> ${GNUPLOT_FILE_TH}
printf "set title 'dN/(dx dz) (cm^{-2})' \n" >> ${GNUPLOT_FILE_TH}
printf "set xlabel 'z (cm)' \n" >> ${GNUPLOT_FILE_TH}
printf "set ylabel 'x (cm)' \n" >> ${GNUPLOT_FILE_TH}
#printf "set xrange[${ZMIN}:${ZMAX}]\n" >> ${GNUPLOT_FILE_TH}
#printf "set yrange[${XMIN}:${XMAX}]\n" >> ${GNUPLOT_FILE_TH}
printf "set palette rgbformulae 22,13,10 \n" >> ${GNUPLOT_FILE_TH}
printf "set logscale cb \n" >> ${GNUPLOT_FILE_TH}
#printf "set format cb '10^{%L}' \n" >> ${GNUPLOT_FILE_TH}
printf "plot FILE_IN u ((\$1+\$2)*0.5):((\$3+\$4)*0.5):5 w image notitle\n" >> ${GNUPLOT_FILE_TH}
$GNUPLOT ${GNUPLOT_FILE_TH}
fi
#####
$GNUPLOT ${GNUPLOT_FILE_ENAN}
$GNUPLOT ${GNUPLOT_FILE_EMITT}
$GNUPLOT ${GNUPLOT_FILE_SIGMA}
$GNUPLOT ${GNUPLOT_FILE_AN}
$GNUPLOT ${GNUPLOT_FILE_EN}
$GNUPLOT ${GNUPLOT_FILE_POS}
$GNUPLOT ${GNUPLOT_FILE_HEATMAP}

