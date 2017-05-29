#!/bin/csh



/*********************************************/
/*                                           */
/*        NB aanroepen via gps_main.sc       */
/*                                           */
/*********************************************/



if (-e $PRESENTDATE) then


   set dtg = `$PRESENTDATE 2`


   #
   # 51 uur geleden (dit script wordt b.v. te 9.58 aangeroepen om de 06.00 UTC file van 2 dagen
   #                 geleden op te halen) 

   #huidige tijd 48 + 3 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw51 = `$BACKUPDATE $dtg -51`
   set dtgw51 = `echo $dtgw51|awk '{printf "%.10s",$0}'`
   



   ##################################################################################################
   #                                                                                                #
   #                                          input sectie                                          #
   #                                                                                                #
   ##################################################################################################


   #
   # NB HIRLAM files zijn reeds gekopieered naar GPS input_hirlam dir. (gps_nedwam_control.sc)
   #



   #
   # NB TSF (TSA golven) files zijn reeds gekopieerd naar GPS input_nedwam dir. (gps_nedwam_control.sc)
   #



   #
   # opgehaalde TSF forecast file testen op niet 0 zijn en op wel aanwezig zijn
   #
   cd $ENV_GPS_LOCAL/input_nedwam
   if (-z NEWH_TSA_$dtgw51"00_XX000_TW" || !(-e NEWH_TSA_$dtgw51"00_XX000_TW")) then

      
      # niets
        
   else

      # file dus WEL aanwezig en niet leeg


      #
      # reeds opgehaalde TSF file omzetten naar de generieke naam TSF_IN 
      # (originele file blijft nu ook behouden)
      # 
      cp NEWH_TSA_$dtgw51"00_XX000_TW" TSF_IN




      ###############################################################################################
      #                                                                                             #
      #                                        process sectie                                       #
      #                                                                                             #
      ###############################################################################################


      #
      # extraheren van TSF file (input is altijd file: TSF_IN; output is altijd file: TSF_REALS)
      #
      cd $ENV_GPS_LOCAL/input_nedwam
      $path_tsf_2_real tsf_info.txt


      #process de data
      cd $ENV_GPS_LOCAL
      gps_new.exe $dtgw51 TSV


      #ps omzetten naar png en zetten in intranet archive dir (xxx_0 = 0, +3, +6; xxx_1 = +9, +15, +18 etc.)
      cd $ENV_GPS_LOCAL/output_nedwam

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_ana_0_$dtgw51.png TSA_ANA_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_ana_1_$dtgw51.png TSA_ANA_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_ana_2_$dtgw51.png TSA_ANA_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_ana_3_$dtgw51.png TSA_ANA_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_ana_4_$dtgw51.png TSA_ANA_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_ana_5_$dtgw51.png TSA_ANA_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_k13_0_$dtgw51.png TSA_K13_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_k13_1_$dtgw51.png TSA_K13_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_k13_2_$dtgw51.png TSA_K13_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_k13_3_$dtgw51.png TSA_K13_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_k13_4_$dtgw51.png TSA_K13_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_k13_5_$dtgw51.png TSA_K13_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_auk_0_$dtgw51.png TSA_AUK_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_auk_1_$dtgw51.png TSA_AUK_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_auk_2_$dtgw51.png TSA_AUK_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_auk_3_$dtgw51.png TSA_AUK_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_auk_4_$dtgw51.png TSA_AUK_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_auk_5_$dtgw51.png TSA_AUK_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_eld_0_$dtgw51.png TSA_ELD_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_eld_1_$dtgw51.png TSA_ELD_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_eld_2_$dtgw51.png TSA_ELD_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_eld_3_$dtgw51.png TSA_ELD_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_eld_4_$dtgw51.png TSA_ELD_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_eld_5_$dtgw51.png TSA_ELD_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_smn_0_$dtgw51.png TSA_SMN_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_smn_1_$dtgw51.png TSA_SMN_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_smn_2_$dtgw51.png TSA_SMN_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_smn_3_$dtgw51.png TSA_SMN_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_smn_4_$dtgw51.png TSA_SMN_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_smn_5_$dtgw51.png TSA_SMN_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_mun_0_$dtgw51.png TSA_MUN_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_mun_1_$dtgw51.png TSA_MUN_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_mun_2_$dtgw51.png TSA_MUN_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_mun_3_$dtgw51.png TSA_MUN_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_mun_4_$dtgw51.png TSA_MUN_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_mun_5_$dtgw51.png TSA_MUN_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_epl_0_$dtgw51.png TSA_EPL_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_epl_1_$dtgw51.png TSA_EPL_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_epl_2_$dtgw51.png TSA_EPL_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_epl_3_$dtgw51.png TSA_EPL_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_epl_4_$dtgw51.png TSA_EPL_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_epl_5_$dtgw51.png TSA_EPL_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_nc2_0_$dtgw51.png TSA_NC2_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_nc2_1_$dtgw51.png TSA_NC2_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_nc2_2_$dtgw51.png TSA_NC2_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_nc2_3_$dtgw51.png TSA_NC2_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_nc2_4_$dtgw51.png TSA_NC2_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_archive_gps/tsa_nc2_5_$dtgw51.png TSA_NC2_5.PS -c quit




      # verwijderen tijdelijke files (anders worden deze weer gebruikt door nedwam_gps_control.sc)
      cd $ENV_GPS_LOCAL/input_nedwam
      rm -f TSF_IN
      rm -f TSF_REALS

      cd $ENV_GPS_LOCAL/output_nedwam
      rm -f TSA_*.PS




   endif
   
   

   ##################################################################################################
   #                                                                                                #
   #                                       verwijder sectie                                         #
   #                                                                                                #
   ##################################################################################################


   #
   # 225 uur geleden (dit script wordt b.v. te 9.58 aangeroepen om de 00.00 UTC file van 7 dagen
   #                  geleden te verwijderen) 

   #huidige tijd 225 (6 + 48 + (24 * 7) + 3) uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw225 = `$BACKUPDATE $dtg -225`
   set dtgw225 = `echo $dtgw225|awk '{printf "%.10s",$0}'`

   cd ~
   rm -f $path_intranet_archive_gps/tsa_ana_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_ana_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_ana_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_ana_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_ana_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_ana_5_$dtgw225.png

   rm -f $path_intranet_archive_gps/tsa_k13_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_k13_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_k13_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_k13_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_k13_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_k13_5_$dtgw225.png

   rm -f $path_intranet_archive_gps/tsa_auk_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_auk_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_auk_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_auk_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_auk_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_auk_5_$dtgw225.png

   rm -f $path_intranet_archive_gps/tsa_eld_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_eld_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_eld_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_eld_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_eld_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_eld_5_$dtgw225.png

   rm -f $path_intranet_archive_gps/tsa_smn_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_smn_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_smn_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_smn_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_smn_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_smn_5_$dtgw225.png

   rm -f $path_intranet_archive_gps/tsa_mun_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_mun_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_mun_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_mun_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_mun_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_mun_5_$dtgw225.png

   rm -f $path_intranet_archive_gps/tsa_epl_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_epl_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_epl_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_epl_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_epl_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_epl_5_$dtgw225.png

   rm -f $path_intranet_archive_gps/tsa_nc2_0_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_nc2_1_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_nc2_2_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_nc2_3_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_nc2_4_$dtgw225.png
   rm -f $path_intranet_archive_gps/tsa_nc2_5_$dtgw225.png


endif
