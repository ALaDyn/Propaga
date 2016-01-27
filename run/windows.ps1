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
Write-Host "#!/gnuplot\n" >> "$GNUPLOT_FILE_EN"
Write-Host "FILE_IN_1='test.initialENERGY.binned.ppg'\n" >> "$GNUPLOT_FILE_EN"
Write-Host "FILE_IN_3='test.finalENERGY.binned.ppg'\n" >> "$GNUPLOT_FILE_EN"
Write-Host "FILE_OUT='spettriEnergia."$IMAGE_TYPE"'\n" >> "$GNUPLOT_FILE_EN"
Write-Host "set terminal "$IMAGE_TERMINAL" truecolor enhanced size "$SIZEX","$SIZEY"\n" >> "$GNUPLOT_FILE_EN"
Write-Host "set output FILE_OUT\n" >> "$GNUPLOT_FILE_EN"
Write-Host "set xlabel 'E (MeV)' \n" >> "$GNUPLOT_FILE_EN"
Write-Host "set ylabel 'dN/dE (MeV^{-1})'\n" >> "$GNUPLOT_FILE_EN"
Write-Host "set format y '10^{%%L}'\n" >> "$GNUPLOT_FILE_EN"
#Write-Host "set xrange[%s:%s]\n" "0" "60" >> "$GNUPLOT_FILE_EN"
Write-Host "set xrange[%s:%s]\n" "$EMIN" "$EMAX" >> "$GNUPLOT_FILE_EN"
Write-Host "set logscale y\n" >> "$GNUPLOT_FILE_EN"
Write-Host "plot FILE_IN_1 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "$WEIGHT2" "$SUBSAMPLE_FACTOR" >> "$GNUPLOT_FILE_EN"
Write-Host "\n" >> "$GNUPLOT_FILE_EN"
Write-Host "FILE_IN_3 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" "$WEIGHT2" "$SUBSAMPLE_FACTOR" >> "$GNUPLOT_FILE_EN"
#####
$GNUPLOT_FILE_AN="spettriAngolo.plt"
Write-Host "#!/gnuplot\n" >> "$GNUPLOT_FILE_AN"
Write-Host "FILE_IN_1='test.initialANGLE.binned.ppg'\n" >> "$GNUPLOT_FILE_AN"
Write-Host "FILE_IN_3='test.finalANGLE.binned.ppg'\n" >> "$GNUPLOT_FILE_AN"
Write-Host "FILE_OUT='spettriAngolo."$IMAGE_TYPE"'\n" >> "$GNUPLOT_FILE_AN"
Write-Host "set terminal "$IMAGE_TERMINAL" truecolor enhanced size "$SIZEX","$SIZEY"\n" >> "$GNUPLOT_FILE_AN"
Write-Host "set output FILE_OUT\n" >> "$GNUPLOT_FILE_AN"
Write-Host "set xlabel '{/Symbol w} (mrad)' \n" >> "$GNUPLOT_FILE_AN"
Write-Host "set ylabel 'dN/d{/Symbol w} (mrad^{-1})'\n" >> "$GNUPLOT_FILE_AN"
Write-Host "set format y '10^{%%L}'\n" >> "$GNUPLOT_FILE_AN"
#Write-Host "set xrange[%s:%s]\n" "-50" "50" >> "$GNUPLOT_FILE_AN"
Write-Host "set xrange[%s:%s]\n" "$THETAMIN" "$THETAMAX" >> "$GNUPLOT_FILE_AN"
Write-Host "set logscale y\n" >> "$GNUPLOT_FILE_AN"
Write-Host "plot FILE_IN_1 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "$WEIGHT2" "$SUBSAMPLE_FACTOR" >> "$GNUPLOT_FILE_AN"
Write-Host "\n" >> "$GNUPLOT_FILE_AN"
Write-Host "FILE_IN_3 u ((\$1+\$2)*0.5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" "$WEIGHT2" "$SUBSAMPLE_FACTOR" >> "$GNUPLOT_FILE_AN"
#####
$GNUPLOT_FILE_POS="spettriPosizione.plt"
Write-Host "#!/gnuplot\n" >> "$GNUPLOT_FILE_POS"
Write-Host "FILE_IN_1='test.initialPOSITION.binned.ppg'\n" >> "$GNUPLOT_FILE_POS"
Write-Host "FILE_IN_3='test.finalPOSITION.binned.ppg'\n" >> "$GNUPLOT_FILE_POS"
Write-Host "FILE_OUT='spettriPosizione."$IMAGE_TYPE"'\n" >> "$GNUPLOT_FILE_POS"
Write-Host "set terminal "$IMAGE_TERMINAL" truecolor enhanced size "$SIZEX","$SIZEY"\n" >> "$GNUPLOT_FILE_POS"
Write-Host "set output FILE_OUT\n" >> "$GNUPLOT_FILE_POS"
Write-Host "set xlabel 'x (mm)' \n" >> "$GNUPLOT_FILE_POS"
Write-Host "set ylabel 'dN/dx (mm^{-1})'\n" >> "$GNUPLOT_FILE_POS"
Write-Host "set format y '10^{%%L}'\n" >> "$GNUPLOT_FILE_POS"
#Write-Host "set xrange[%s:%s]\n" "0" "5" >> "$GNUPLOT_FILE_POS"
Write-Host "set xrange[%s*10:%s*10]\n" "$XMIN" "$XMAX" >> "$GNUPLOT_FILE_POS"
Write-Host "set logscale y\n" >> "$GNUPLOT_FILE_POS"
Write-Host "plot FILE_IN_1 u ((\$1+\$2)*5):(\$3*%s*%s) w histeps lt 1 lc rgb 'blue' lw 3 t 'full spectrum',\\" "$WEIGHT2" "$SUBSAMPLE_FACTOR" >> "$GNUPLOT_FILE_POS"
Write-Host "\n" >> "$GNUPLOT_FILE_POS"
Write-Host "FILE_IN_3 u ((\$1+\$2)*5):(\$3*%s*%s) w histeps lt 1 lc rgb 'red' lw 3 t 'spectrum after second iris'" "$WEIGHT2" "$SUBSAMPLE_FACTOR" >> "$GNUPLOT_FILE_POS"
#####
$GNUPLOT_FILE_EMITT="emittanza.plt"
Write-Host "#!/gnuplot \n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "file1='xyz.emitt.ppg' \n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "FILE_OUT='emittanza."$IMAGE_TYPE"'\n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "set terminal "$IMAGE_TERMINAL" truecolor enhanced size "$SIZEX","$SIZEY"\n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "set output FILE_OUT \n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "set xlabel 'z_{mean} (cm)' \n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "set ylabel '{/Symbol e} (mm mrad)' \n" >> "$GNUPLOT_FILE_EMITT"
#Write-Host "set yrange[0:50]\n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "set xrange[0:200]\n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "plot file1 u 6:(\$8*10000) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol e}_x',\\" >> "$GNUPLOT_FILE_EMITT"
Write-Host "\n" >> "$GNUPLOT_FILE_EMITT"
Write-Host "file1 u 6:(\$10*10000) w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol e}_y'\n" >> "$GNUPLOT_FILE_EMITT"
######
$GNUPLOT_FILE_SIGMA="sigma.plt"
Write-Host "#!/gnuplot \n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "file1='xyz.emitt.ppg' \n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "FILE_OUT='sigma."$IMAGE_TYPE"'\n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "set terminal "$IMAGE_TERMINAL" truecolor enhanced size "$SIZEX","$SIZEY"\n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "set output FILE_OUT \n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "set xlabel 'z_{mean} (cm)'  \n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "set ylabel '{/Symbol s} (mm)' \n" >> "$GNUPLOT_FILE_SIGMA"
#Write-Host "set yrange[0:20] \n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "set xrange[0:200] \n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "plot file1  u 6:(\$7*10) w lines lt 1 lc rgb 'red' lw 3 t '{/Symbol s}_x',\\" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "\n" >> "$GNUPLOT_FILE_SIGMA"
Write-Host "file1 u 6:(\$9*10)  w lines lt 1 lc rgb 'blue' lw 3 t '{/Symbol s}_y' \n" >> "$GNUPLOT_FILE_SIGMA"
######
$GNUPLOT_FILE_HEATMAP="heatmap.plt"
Write-Host "#!/gnuplot \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "FILE_IN_1='test.finalHEATMAP.binned.ppg' \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "FILE_OUT='heatmap."$IMAGE_TYPE"' \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set terminal "$IMAGE_TERMINAL" truecolor enhanced size "$SIZEX","$SIZEY" \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set output FILE_OUT \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set title 'dN/(dEd{/Symbol W}) (MeV^{-1} mrad^{-1})' \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set xlabel 'E (MeV)' \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set ylabel '{/Symbol q} (mrad)' \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set xrange[%s:%s]\n" "$EMIN" "$EMAX" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set yrange[%s:%s]\n" "$THETAMIN" "$THETAMAX" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set palette rgbformulae 22,13,10 \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set logscale cb \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "set format cb '10^{%%L}' \n" >> "$GNUPLOT_FILE_HEATMAP"
Write-Host "plot FILE_IN_1 u ((\$1+\$2)*0.5):((\$3+\$4)*0.5):(\$5*%s*%s) w image notitle \n" "$WEIGHT2" "$SUBSAMPLE_FACTOR" >> "$GNUPLOT_FILE_HEATMAP"
######
$GNUPLOT_FILE_ENAN="enan.plt"
Write-Host "#!/gnuplot \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "FILE_IN_1='test."$NSTEPS".ENAN.ppg' \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "FILE_OUT='enan."$IMAGE_TYPE"' \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "set terminal "$IMAGE_TERMINAL" truecolor enhanced size "$SIZEX","$SIZEY" \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "set output FILE_OUT \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "set title 'Energy-angle correlation plot' \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "set xlabel 'E (MeV)' \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "set ylabel '{/Symbol q} (mrad)' \n" >> "$GNUPLOT_FILE_ENAN"
Write-Host "set xrange[%s:%s]\n" "$EMIN" "$EMAX" >> "$GNUPLOT_FILE_ENAN"
Write-Host "set yrange[%s:%s]\n" "$THETAMIN" "$THETAMAX" >> "$GNUPLOT_FILE_ENAN"
Write-Host "plot FILE_IN_1 u "$ENERGY_COL":"$ANGLE_COL" w points ps 0.5 lc rgb 'blue' \n" >> "$GNUPLOT_FILE_ENAN"
#######
Start-Process -FilePath $GNUPLOT -ArgumentList "$GNUPLOT_FILE_ENAN"
Start-Process -FilePath $GNUPLOT -ArgumentList "$GNUPLOT_FILE_EMITT"
Start-Process -FilePath $GNUPLOT -ArgumentList "$GNUPLOT_FILE_SIGMA"
Start-Process -FilePath $GNUPLOT -ArgumentList "$GNUPLOT_FILE_AN"
Start-Process -FilePath $GNUPLOT -ArgumentList "$GNUPLOT_FILE_EN"
Start-Process -FilePath $GNUPLOT -ArgumentList "$GNUPLOT_FILE_POS"
Start-Process -FilePath $GNUPLOT -ArgumentList "$GNUPLOT_FILE_HEATMAP"


