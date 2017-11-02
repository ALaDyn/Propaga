#! /bin/bash

NSTEPS=$(find . -type f -name "${RUN_NAME}.???????.ppg" -printf "%f\\n" | sort | tail -1) ; NSTEPS=${NSTEPS#*.} ; NSTEPS=${NSTEPS%.*}

CONVERTER=$(which converter)
GNUPLOT=$(which gnuplot)
RUN_NAME="test"

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
#MODE_XYZE=45
ENERGY_COL=7
ANGLE_COL=3
WEIGHT_COL_ENAN_FILE=-2
WEIGHT_COL_DISTR_FILE=8
X_COL=1
#Y_COL=2
Z_COL=3
PX_COL=4
PY_COL=5
PZ_COL=6
ID_PARTICLE_COL=7
#####
#rm -f *.plt
rm -f ./*.${IMAGE_TYPE}
#####
$CONVERTER 1 1 ${MODE_CLEAN} ${RUN_NAME}."${NSTEPS}".ppg ${STATUS_PARTICLE_COL}
mv conv.${RUN_NAME}."${NSTEPS}".ppg ${RUN_NAME}."${NSTEPS}".pulito.ppg
$CONVERTER 1 29979245800.0 ${MODE_ANGLE_AND_ENERGY} ${RUN_NAME}."${NSTEPS}".pulito.ppg ${PX_COL} ${PY_COL} ${PZ_COL} ${ID_PARTICLE_COL} ${WEIGHT_COL_DISTR_FILE}
mv conv.${RUN_NAME}."${NSTEPS}".pulito.ppg ${RUN_NAME}."${NSTEPS}".ENAN.ppg
#####
$CONVERTER 1 1 ${MODE_BIN1D} ${RUN_NAME}."${NSTEPS}".ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.${RUN_NAME}."${NSTEPS}".ENAN.ppg ${RUN_NAME}.finalENERGY.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} ${RUN_NAME}."${NSTEPS}".ENAN.ppg ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.${RUN_NAME}."${NSTEPS}".ENAN.ppg ${RUN_NAME}.finalANGLE.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} ${RUN_NAME}."${NSTEPS}".pulito.ppg ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}
mv conv.${RUN_NAME}."${NSTEPS}".pulito.ppg ${RUN_NAME}.finalPOSITION.binned.ppg
$CONVERTER 1 1 ${MODE_BIN2D} ${RUN_NAME}."${NSTEPS}".ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.${RUN_NAME}."${NSTEPS}".ENAN.ppg ${RUN_NAME}.finalHEATMAP.binned.ppg
#####
$CONVERTER 1 1 ${MODE_CLEAN} ${RUN_NAME}.0000000.ppg ${STATUS_PARTICLE_COL}
mv conv.${RUN_NAME}.0000000.ppg ${RUN_NAME}.0000000.pulito.ppg
$CONVERTER 1 29979245800.0 ${MODE_ANGLE_AND_ENERGY} ${RUN_NAME}.0000000.pulito.ppg ${PX_COL} ${PY_COL} ${PZ_COL} ${ID_PARTICLE_COL} ${WEIGHT_COL_DISTR_FILE}
mv conv.${RUN_NAME}.0000000.pulito.ppg ${RUN_NAME}.0000000.ENAN.ppg
#####
$CONVERTER 1 1 ${MODE_BIN1D} ${RUN_NAME}.0000000.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.${RUN_NAME}.0000000.ENAN.ppg ${RUN_NAME}.initialENERGY.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} ${RUN_NAME}.0000000.ENAN.ppg ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.${RUN_NAME}.0000000.ENAN.ppg ${RUN_NAME}.initialANGLE.binned.ppg
$CONVERTER 1 1 ${MODE_BIN1D} ${RUN_NAME}.0000000.pulito.ppg ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}
mv conv.${RUN_NAME}.0000000.pulito.ppg ${RUN_NAME}.initialPOSITION.binned.ppg
$CONVERTER 1 1 ${MODE_BIN2D} ${RUN_NAME}.0000000.ENAN.ppg ${ENERGY_COL} ${NBIN_ENERGY} ${EMIN} ${EMAX} ${ANGLE_COL} ${NBIN_ANGLE} ${THETAMIN} ${THETAMAX} ${WEIGHT_COL_ENAN_FILE}
mv conv.${RUN_NAME}.0000000.ENAN.ppg ${RUN_NAME}.initialHEATMAP.binned.ppg
#####
if [ ${FAI_TRACCE} -eq 1  ] ; then
$CONVERTER 1 1 ${MODE_BIN2D} ${RUN_NAME}.tracked.ppg ${Z_COL} ${NBIN_Z} ${ZMIN} ${ZMAX} ${X_COL} ${NBIN_X} ${XMIN} ${XMAX} ${WEIGHT_COL_DISTR_FILE}
mv conv.${RUN_NAME}.tracked.ppg ${RUN_NAME}.trackedHEATMAP.binned.ppg
fi
#####
GNUPLOT_FILE_EN="spettriEnergia.plt"
rm -f ${GNUPLOT_FILE_EN}
touch ${GNUPLOT_FILE_EN}
chmod 775 ${GNUPLOT_FILE_EN}
{
  printf "#!/gnuplot\\n"
  printf "FILE_IN_1='%s.initialENERGY.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_IN_3='%s.finalENERGY.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='spettriEnergia.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set xlabel 'E (MeV)'\\n"
  printf "set ylabel 'dN/dE (MeV^{-1})'\\n"
  printf "set format y '10^{%%L}'\\n"
  #printf "set xrange[%s:%s]\\n" "0" "60"
  printf "set xrange[%s:%s]\\n" "${EMIN}" "${EMAX}"
  printf "set logscale y\\n"
  printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'initial spectrum',\\" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
  printf "\\n"
  printf "FILE_IN_3 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'final spectrum'" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
} >> ${GNUPLOT_FILE_EN}
#####
GNUPLOT_FILE_AN="spettriAngolo.plt"
rm -f ${GNUPLOT_FILE_AN}
touch ${GNUPLOT_FILE_AN}
chmod 775 ${GNUPLOT_FILE_AN}
{
  printf "#!/gnuplot\\n"
  printf "FILE_IN_1='%s.initialANGLE.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_IN_3='%s.finalANGLE.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='spettriAngolo.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set xlabel '{/Symbol w} (mrad)'\\n"
  printf "set ylabel 'dN/d{/Symbol w} (mrad^{-1})'\\n"
  printf "set format y '10^{%%L}'\\n"
  printf "set xrange[%s:%s]\\n" "${THETAMIN}" "${THETAMAX}"
  printf "set logscale y\\n"
  printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
  printf "\\n"
  printf "FILE_IN_3 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'final spectrum'" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
} >> ${GNUPLOT_FILE_AN}
#####
GNUPLOT_FILE_POS="spettriPosizione.plt"
rm -f ${GNUPLOT_FILE_POS}
touch ${GNUPLOT_FILE_POS}
chmod 775 ${GNUPLOT_FILE_POS}
{
  printf "#!/gnuplot\\n"
  printf "FILE_IN_1='%s.initialPOSITION.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_IN_3='%s.finalPOSITION.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='spettriPosizione.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set xlabel 'x (mm)'\\n"
  printf "set ylabel 'dN/dx (mm^{-1})'\\n"
  printf "set format y '10^{%%L}'\\n"
  printf "set xrange[%s*10:%s*10]\\n" "${XMIN}" "${XMAX}"
  printf "set logscale y\\n"
  printf "plot FILE_IN_1 u ((\$1+\$2)*5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
  printf "\\n"
  printf "FILE_IN_3 u ((\$1+\$2)*5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'final spectrum'" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
} >> ${GNUPLOT_FILE_POS}
#####
GNUPLOT_FILE_EMITT="emittanza.plt"
rm -f ${GNUPLOT_FILE_EMITT}
touch ${GNUPLOT_FILE_EMITT}
chmod 775 ${GNUPLOT_FILE_EMITT}
{
  printf "#!/gnuplot\\n"
  printf "file1='%s.emitt.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='emittanza.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set xlabel 'z_{mean} (cm)'\\n"
  printf "set ylabel '{/Symbol e} (mm mrad)'\\n"
  printf "set xrange[0:200]\\n"
  printf "plot file1 u 6:(\$8*10000) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol e}_x',\\"
  printf "\\n"
  printf "file1 u 6:(\$10*10000) w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol e}_y'\\n"
} >> ${GNUPLOT_FILE_EMITT}
######
GNUPLOT_FILE_SIGMA="sigma.plt"
rm -f ${GNUPLOT_FILE_SIGMA}
touch ${GNUPLOT_FILE_SIGMA}
chmod 775 ${GNUPLOT_FILE_SIGMA}
{
  printf "#!/gnuplot\\n"
  printf "file1='%s.emitt.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='sigma.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set xlabel 'z_{mean} (cm)'\\n"
  printf "set ylabel '{/Symbol s} (mm)'\\n"
  printf "set xrange[0:200]\\n"
  printf "plot file1  u 6:(\$7*10) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol s}_x',\\"
  printf "\\n"
  printf "file1 u 6:(\$9*10)  w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol s}_y'\\n"
} >> ${GNUPLOT_FILE_SIGMA}
######
GNUPLOT_FILE_HEATMAP_END="heatmap_end.plt"
rm -f ${GNUPLOT_FILE_HEATMAP_END}
touch ${GNUPLOT_FILE_HEATMAP_END}
chmod 775 ${GNUPLOT_FILE_HEATMAP_END}
{
  printf "#!/gnuplot\\n"
  printf "FILE_IN_1='%s.finalHEATMAP.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='finalheatmap.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set title 'dN/(dEd{/Symbol W}) (MeV^{-1} mrad^{-1}) (final)'\\n"
  printf "set xlabel 'E (MeV)'\\n"
  printf "set ylabel '{/Symbol q} (mrad)'\\n"
  printf "set xrange[%s:%s]\\n" "${EMIN}" "${EMAX}"
  printf "set yrange[%s:%s]\\n" "${THETAMIN}" "${THETAMAX}"
  printf "set palette rgbformulae 22,13,10\\n"
  printf "set logscale cb\\n"
  printf "set format cb '10^{%%L}'\\n"
  printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):((\$3+\$4)*0.5):(\$5*%s*%s) w image notitle\\n" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
} >> ${GNUPLOT_FILE_HEATMAP_END}
######
GNUPLOT_FILE_HEATMAP_START="heatmap_start.plt"
rm -f ${GNUPLOT_FILE_HEATMAP_START}
touch ${GNUPLOT_FILE_HEATMAP_START}
chmod 775 ${GNUPLOT_FILE_HEATMAP_START}
{
  printf "#!/gnuplot\\n"
  printf "FILE_IN_1='%s.initialHEATMAP.binned.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='initialheatmap.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set title 'dN/(dEd{/Symbol W}) (MeV^{-1} mrad^{-1}) (initial)'\\n"
  printf "set xlabel 'E (MeV)'\\n"
  printf "set ylabel '{/Symbol q} (mrad)'\\n"
  printf "set xrange[%s:%s]\\n" "${EMIN}" "${EMAX}"
  printf "set yrange[%s:%s]\\n" "${THETAMIN}" "${THETAMAX}"
  printf "set palette rgbformulae 22,13,10\\n"
  printf "set logscale cb\\n"
  printf "set format cb '10^{%%L}'\\n"
  printf "plot FILE_IN_1 u ((\$1+\$2)*0.5):((\$3+\$4)*0.5):(\$5*%s*%s) w image notitle\\n" "${WEIGHT2}" "${SUBSAMPLE_FACTOR}"
} >> ${GNUPLOT_FILE_HEATMAP_START}
######
GNUPLOT_FILE_ENAN_END="enan_end.plt"
rm -f ${GNUPLOT_FILE_ENAN_END}
touch ${GNUPLOT_FILE_ENAN_END}
chmod 775 ${GNUPLOT_FILE_ENAN_END}
{
  printf "#!/gnuplot\\n"
  printf "FILE_IN_1='%s.%s.ENAN.ppg'\\n" "${RUN_NAME}" "${NSTEPS}"
  printf "FILE_OUT='finalenan.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set title 'Final energy-angle correlation plot'\\n"
  printf "set xlabel 'E (MeV)'\\n"
  printf "set ylabel '{/Symbol q} (mrad)'\\n"
  printf "set xrange[%s:%s]\\n" "${EMIN}" "${EMAX}"
  printf "set yrange[%s:%s]\\n" "${THETAMIN}" "${THETAMAX}"
  printf "plot FILE_IN_1 u %s:%s w points ps 0.5 lc rgb 'blue'\\n" "${ENERGY_COL}" "${ANGLE_COL}"
} >> ${GNUPLOT_FILE_ENAN_END}
#######
GNUPLOT_FILE_ENAN_START="enan_start.plt"
rm -f ${GNUPLOT_FILE_ENAN_START}
touch ${GNUPLOT_FILE_ENAN_START}
chmod 775 ${GNUPLOT_FILE_ENAN_START}
{
  printf "#!/gnuplot\\n"
  printf "FILE_IN_1='%s.0000000.ENAN.ppg'\\n" "${RUN_NAME}"
  printf "FILE_OUT='initialenan.%s'\\n" "${IMAGE_TYPE}"
  printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
  printf "set output FILE_OUT\\n"
  printf "set title 'Initial energy-angle correlation plot'\\n"
  printf "set xlabel 'E (MeV)'\\n"
  printf "set ylabel '{/Symbol q} (mrad)'\\n"
  printf "set xrange[%s:%s]\\n" "${EMIN}" "${EMAX}"
  printf "set yrange[%s:%s]\\n" "${THETAMIN}" "${THETAMAX}"
  printf "plot FILE_IN_1 u %s:%s w points ps 0.5 lc rgb 'blue'\\n" "${ENERGY_COL}" "${ANGLE_COL}"
} >> ${GNUPLOT_FILE_ENAN_START}
#######
if [ ${FAI_TRACCE} -eq 1  ] ; then
  GNUPLOT_FILE_TH="tracks_heatmap.plt"
  rm -f ${GNUPLOT_FILE_TH}
  touch ${GNUPLOT_FILE_TH}
  chmod 775 ${GNUPLOT_FILE_TH}
  {
    printf "#!/gnuplot\\n"
    printf "set terminal %s truecolor enhanced size %s,%s\\n" "${IMAGE_TERMINAL}" "${SIZEX}" "${SIZEY}"
    printf "FILE_IN='%s.trackedHEATMAP.binned.ppg'\\n" "${RUN_NAME}"
    printf "FILE_OUT='tracks_heatmap.%s'\\n" "${IMAGE_TYPE}"
    printf "set output FILE_OUT\\n"
    printf "set title 'dN/(dx dz) (cm^{-2})'\\n"
    printf "set xlabel 'z (cm)'\\n"
    printf "set ylabel 'x (cm)'\\n"
    printf "set palette rgbformulae 22,13,10\\n"
    printf "set logscale cb\\n"
    printf "plot FILE_IN u ((\$1+\$2)*0.5):((\$3+\$4)*0.5):5 w image notitle\\n"
  } >> ${GNUPLOT_FILE_TH}
  $GNUPLOT ${GNUPLOT_FILE_TH}
fi
#####
$GNUPLOT ${GNUPLOT_FILE_ENAN_START}
$GNUPLOT ${GNUPLOT_FILE_ENAN_END}
$GNUPLOT ${GNUPLOT_FILE_EMITT}
$GNUPLOT ${GNUPLOT_FILE_SIGMA}
$GNUPLOT ${GNUPLOT_FILE_AN}
$GNUPLOT ${GNUPLOT_FILE_EN}
$GNUPLOT ${GNUPLOT_FILE_POS}
$GNUPLOT ${GNUPLOT_FILE_HEATMAP_START}
$GNUPLOT ${GNUPLOT_FILE_HEATMAP_END}
