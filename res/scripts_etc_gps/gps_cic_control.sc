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
   gps_new.exe $dtgw9 CIC

   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_ana_d.png CIC_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_k13_d.png CIC_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_auk_d.png CIC_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_eld_d.png CIC_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_smn_d.png CIC_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mun_d.png CIC_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_epl_d.png CIC_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_nc2_d.png CIC_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mpn_d.png CIC_MPN.PS -c quit




   #
   # 6, 7 en 8 uur geleden (effectief 7,8,9 uur geleden)
   #

   #huidige tijd 6 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw6 = `$BACKUPDATE $dtg -6`
   set dtgw6 = `echo $dtgw6|awk '{printf "%.10s",$0}'`
 
   #process de data
   cd $ENV_GPS_LOCAL
   gps_new.exe $dtgw6 CIC

   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_ana_c.png CIC_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_k13_c.png CIC_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_auk_c.png CIC_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_eld_c.png CIC_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_smn_c.png CIC_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mun_c.png CIC_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_epl_c.png CIC_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_nc2_c.png CIC_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mpn_c.png CIC_MPN.PS -c quit


   #
   # 3, 4 en 5 uur geleden (effectief 4,5,6 uur geleden)
   #

   #huidige tijd 3 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw3 = `$BACKUPDATE $dtg -3`
   set dtgw3 = `echo $dtgw3|awk '{printf "%.10s",$0}'`
 
   #process de data
   cd $ENV_GPS_LOCAL
   gps_new.exe $dtgw3 CIC

   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_ana_b.png CIC_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_k13_b.png CIC_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_auk_b.png CIC_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_eld_b.png CIC_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_smn_b.png CIC_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mun_b.png CIC_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_epl_b.png CIC_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_nc2_b.png CIC_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mpn_b.png CIC_MPN.PS -c quit


   #
   # 0, 1 en 2 uur geleden (effectief 1,2,3 uur geleden)
   #

   #huidige tijd 0 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw = `$BACKUPDATE $dtg -0`
   set dtgw = `echo $dtgw|awk '{printf "%.10s",$0}'`
   

   #LDS, CIC (CIC als er geen LDS data zijn), CID (gemeten swell data) en HWA (spare rtwndb) golven files kopieeren naar GPS input dir.
   cd $ENV_GPS_LOCAL/input_cic
   #cp $ENV_HWA_GLOBAL/$dtgw".HWA" $ENV_GPS_LOCAL/input_cic   
   cp $ENV_LDS_GLOBAL/WAVE_LDS_$dtgw"00_00000_LC" $ENV_GPS_LOCAL/input_cic 
   cp $ENV_CIC_GLOBAL/$dtgw".CIC" $ENV_GPS_LOCAL/input_cic   
   cp $ENV_CID_GLOBAL/WAVE_CID_$dtgw"00_00000_LC" $ENV_GPS_LOCAL/input_cic   



   #process de data
   cd $ENV_GPS_LOCAL
   gps_new.exe $dtgw CIC


   #ps omzetten naar png en zetten in intranet dir
   cd $ENV_GPS_LOCAL/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_ana.png CIC_ANA.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_k13.png CIC_K13.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_auk.png CIC_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_eld.png CIC_ELD.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_smn.png CIC_SMN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mun.png CIC_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_epl.png CIC_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_nc2.png CIC_NC2.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mpn.png CIC_MPN.PS -c quit


   #
   # verwijderen -tussen resultaat- postscript files
   #
   cd $ENV_GPS_LOCAL/output_cic
   rm -f CIC_*.PS


   #
   # verwijderen van 7 dagen oude files
   # 

   set dtgw7 = `$BACKUPDATE dtg -168`
   set dtgw7 = `echo $dtgw7|awk '{printf "%.10s",$0}'`

   #verwijderen uit temp directory (n.b. de data uit de .cic files zijn in de lds temp files geintegreerd) */
   cd $ENV_GPS_LOCAL/temp_cic
   rm -f CIC_LDS_ANA_$dtgw7".TMP"
   rm -f CIC_LDS_K13_$dtgw7".TMP"
   rm -f CIC_LDS_AUK_$dtgw7".TMP"
   rm -f CIC_LDS_ELD_$dtgw7".TMP"
   rm -f CIC_LDS_SMN_$dtgw7".TMP"
   rm -f CIC_LDS_MUN_$dtgw7".TMP"
   rm -f CIC_LDS_EPL_$dtgw7".TMP"
   rm -f CIC_LDS_NC2_$dtgw7".TMP"
   rm -f CIC_LDS_MPN_$dtgw7".TMP"


   rm -f CIC_CID_ANA_$dtgw7".TMP"
   rm -f CIC_CID_K13_$dtgw7".TMP"
   rm -f CIC_CID_AUK_$dtgw7".TMP"
   rm -f CIC_CID_ELD_$dtgw7".TMP"
   rm -f CIC_CID_SMN_$dtgw7".TMP"
   rm -f CIC_CID_MUN_$dtgw7".TMP"
   rm -f CIC_CID_EPL_$dtgw7".TMP"
   rm -f CIC_CID_NC2_$dtgw7".TMP"
   rm -f CIC_CID_MPN_$dtgw7".TMP"

   rm -f CIC_HWA_*_$dtgw7".TMP"




   #verwijderen uit input directory
   cd $ENV_GPS_LOCAL/input_cic
   rm -f WAVE_LDS_$dtgw7"00_00000_LC"
   rm -f WAVE_CID_$dtgw7"00_00000_LC"
   rm -f $dtgw7".CIC"
   rm -f $dtgw7".HWA"


endif
