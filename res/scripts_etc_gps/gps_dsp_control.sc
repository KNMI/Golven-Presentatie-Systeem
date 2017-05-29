#!/bin/csh




/*********************************************/
/*                                           */
/*        NB aanroepen via gps_main.sc       */
/*                                           */
/*********************************************/





if (-e $PRESENTDATE) then

   set dtg = `$PRESENTDATE 2`



   #
   # 9, 10 en 11 uur geleden (effectief 10,11,12 uur geleden)
   #

   #huidige tijd 9 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw9 = `$BACKUPDATE $dtg -9`
   set dtgw9 = `echo $dtgw9|awk '{printf "%.10s",$0}'`
 
   #process de data
   cd $ENV_GPS_LOCAL
   gps_new.exe $dtgw9 DSP

   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_ana_d.png DSP_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_k13_d.png DSP_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_auk_d.png DSP_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_eld_d.png DSP_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_smn_d.png DSP_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mun_d.png DSP_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_epl_d.png DSP_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_nc2_d.png DSP_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mpn_d.png DSP_MPN.PS -c quit



   #
   # 6, 7 en 8 uur geleden (effectief 7,8,9 uur geleden)
   #

   #huidige tijd 6 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw6 = `$BACKUPDATE $dtg -6`
   set dtgw6 = `echo $dtgw6|awk '{printf "%.10s",$0}'`
 
   #process de data
   cd $ENV_GPS_LOCAL
   gps_new.exe $dtgw6 DSP

   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_ana_c.png DSP_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_k13_c.png DSP_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_auk_c.png DSP_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_eld_c.png DSP_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_smn_c.png DSP_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mun_c.png DSP_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_epl_c.png DSP_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_nc2_c.png DSP_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mpn_c.png DSP_MPN.PS -c quit



   #
   # 3, 4 en 5 uur geleden (effectief 4,5,6 uur geleden)
   #

   #huidige tijd 3 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw3 = `$BACKUPDATE $dtg -3`
   set dtgw3 = `echo $dtgw3|awk '{printf "%.10s",$0}'`
 
   #process de data
   cd $ENV_GPS_LOCAL
   gps_new.exe $dtgw3 DSP

   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_ana_b.png DSP_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_k13_b.png DSP_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_auk_b.png DSP_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_eld_b.png DSP_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_smn_b.png DSP_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mun_b.png DSP_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_epl_b.png DSP_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_nc2_b.png DSP_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mpn_b.png DSP_MPN.PS -c quit



   #
   # 0, 1 en 2 uur geleden (effectief 1,2,3 uur geleden)
   #

   #huidige tijd 0 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw = `$BACKUPDATE $dtg -0`
   set dtgw = `echo $dtgw|awk '{printf "%.10s",$0}'`
   

   #LDS golven files kopieeren naar GPS input dir.
   cd $ENV_GPS_LOCAL/input_cic
   cp $ENV_DSP_GLOBAL/WAVE_DSP_$dtgw"00_00000_LC" $ENV_GPS_LOCAL/input_cic   


   #process de data
   cd $ENV_GPS_LOCAL
   gps_new.exe $dtgw DSP


   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_ana.png DSP_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_k13.png DSP_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_auk.png DSP_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_eld.png DSP_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_smn.png DSP_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mun.png DSP_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_epl.png DSP_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_nc2.png DSP_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/dsp_mpn.png DSP_MPN.PS -c quit



   #
   # verwijderen -tussen resultaat- postscript files
   #
   cd $ENV_GPS_LOCAL/output_cic
   rm -f DSP_*.PS


   #
   # verwijderen van 7 dagen oude files
   # 

   set dtgw7 = `$BACKUPDATE $dtg -168`
   set dtgw7 = `echo $dtgw7|awk '{printf "%.10s",$0}'`

   #verwijderen uit temp directory
   cd $ENV_GPS_LOCAL/temp_cic
   rm -f CIC_DSP_ANA_$dtgw7".TMP"
   rm -f CIC_DSP_K13_$dtgw7".TMP"
   rm -f CIC_DSP_AUK_$dtgw7".TMP"
   rm -f CIC_DSP_ELD_$dtgw7".TMP"
   rm -f CIC_DSP_SMN_$dtgw7".TMP"
   rm -f CIC_DSP_MUN_$dtgw7".TMP"
   rm -f CIC_DSP_EPL_$dtgw7".TMP"
   rm -f CIC_DSP_NC2_$dtgw7".TMP"
   rm -f CIC_DSP_MPN_$dtgw7".TMP"


   #verwijderen uit input directory
   cd $ENV_GPS_LOCAL/input_cic
   rm -f WAVE_DSP_$dtgw7"00_00000_LC"


endif
