#!/bin/csh



/*********************************************/
/*                                           */
/*        NB aanroepen via gps_main.sc       */
/*                                           */
/*********************************************/





if (-e $PRESENTDATE) then


   set dtg = `$PRESENTDATE 2`


   #
   # 3 uur geleden (dit script wordt b.v. te 9.50 aangeroepen om de 06.00 UTC file op te halen)
   #

   #huidige tijd 3 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw3 = `$BACKUPDATE $dtg -3`
   set dtgw3 = `echo $dtgw3|awk '{printf "%.10s",$0}'`
   



   ##################################################################################################
   #                                                                                                #
   #                                          input sectie                                          #
   #                                                                                                #
   ##################################################################################################


   #
   # HIRLAM files kopieeren naar GPS input_hirlam dir.
   #
   cd $ENV_GPS_LOCAL/input_hirlam
   #cp $ENV_HIRLAM_GLOBAL/hirver$dtgw3 $ENV_GPS_LOCAL/input_hirlam/hirver$dtgw3
   cp $ENV_HIRLAM_GLOBAL/hirver$dtgw3 $ENV_GPS_LOCAL/input_hirlam


   #
   # TSF (TSA golven) files kopieeren naar GPS input_nedwam dir.
   #
   cd $ENV_GPS_LOCAL/input_nedwam
   cp $ENV_TSA_GLOBAL/NEWH_TSA_$dtgw3"00_XX000_TW" $ENV_GPS_LOCAL/input_nedwam  



   #
   # opgehaalde TSF forecast file testen op niet 0 zijn en op wel aanwezig zijn
   #
   cd $ENV_GPS_LOCAL/input_nedwam
   if (-z NEWH_TSA_$dtgw3"00_XX000_TW" || !(-e NEWH_TSA_$dtgw3"00_XX000_TW")) then

      
      # aanmaken van een flag (file) daarop kan dan weer getest worden door het 'retry' script van 1 uur later
      cd $ENV_GPS_LOCAL
      touch flag_file
  
   else

      # file dus WEL aanwezig en niet leeg


      #
      # reeds opgehaalde TSF file omzetten naar de generieke naam TSF_IN 
      # (originele file blijft nu ook behouden)
      # 
      cp NEWH_TSA_$dtgw3"00_XX000_TW" TSF_IN




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
      gps_new.exe $dtgw3 TSA


      #ps omzetten naar png en zetten in intranet dir (xxx_0 = 0, +3, +6; xxx_1 = +9, +15, +18 etc.)
      cd $ENV_GPS_LOCAL/output_nedwam

      #
      # 1-dimensionaal nedwam spectrum
      #
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_ana_0.png TSA_ANA_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_ana_1.png TSA_ANA_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_ana_2.png TSA_ANA_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_ana_3.png TSA_ANA_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_ana_4.png TSA_ANA_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_ana_5.png TSA_ANA_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_k13_0.png TSA_K13_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_k13_1.png TSA_K13_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_k13_2.png TSA_K13_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_k13_3.png TSA_K13_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_k13_4.png TSA_K13_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_k13_5.png TSA_K13_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_auk_0.png TSA_AUK_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_auk_1.png TSA_AUK_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_auk_2.png TSA_AUK_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_auk_3.png TSA_AUK_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_auk_4.png TSA_AUK_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_auk_5.png TSA_AUK_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_eld_0.png TSA_ELD_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_eld_1.png TSA_ELD_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_eld_2.png TSA_ELD_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_eld_3.png TSA_ELD_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_eld_4.png TSA_ELD_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_eld_5.png TSA_ELD_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_smn_0.png TSA_SMN_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_smn_1.png TSA_SMN_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_smn_2.png TSA_SMN_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_smn_3.png TSA_SMN_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_smn_4.png TSA_SMN_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_smn_5.png TSA_SMN_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_mun_0.png TSA_MUN_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_mun_1.png TSA_MUN_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_mun_2.png TSA_MUN_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_mun_3.png TSA_MUN_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_mun_4.png TSA_MUN_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_mun_5.png TSA_MUN_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_epl_0.png TSA_EPL_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_epl_1.png TSA_EPL_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_epl_2.png TSA_EPL_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_epl_3.png TSA_EPL_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_epl_4.png TSA_EPL_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_epl_5.png TSA_EPL_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_nc2_0.png TSA_NC2_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_nc2_1.png TSA_NC2_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_nc2_2.png TSA_NC2_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_nc2_3.png TSA_NC2_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_nc2_4.png TSA_NC2_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_nc2_5.png TSA_NC2_5.PS -c quit



      #
      # polaire plotjes (2 dimensionaal nedwam spectrum)
      # 
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_ana_0.png TSA_POLAIR_ANA_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_ana_1.png TSA_POLAIR_ANA_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_ana_2.png TSA_POLAIR_ANA_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_ana_3.png TSA_POLAIR_ANA_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_ana_4.png TSA_POLAIR_ANA_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_ana_5.png TSA_POLAIR_ANA_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_k13_0.png TSA_POLAIR_K13_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_k13_1.png TSA_POLAIR_K13_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_k13_2.png TSA_POLAIR_K13_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_k13_3.png TSA_POLAIR_K13_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_k13_4.png TSA_POLAIR_K13_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_k13_5.png TSA_POLAIR_K13_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_auk_0.png TSA_POLAIR_AUK_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_auk_1.png TSA_POLAIR_AUK_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_auk_2.png TSA_POLAIR_AUK_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_auk_3.png TSA_POLAIR_AUK_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_auk_4.png TSA_POLAIR_AUK_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_auk_5.png TSA_POLAIR_AUK_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_eld_0.png TSA_POLAIR_ELD_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_eld_1.png TSA_POLAIR_ELD_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_eld_2.png TSA_POLAIR_ELD_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_eld_3.png TSA_POLAIR_ELD_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_eld_4.png TSA_POLAIR_ELD_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_eld_5.png TSA_POLAIR_ELD_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_smn_0.png TSA_POLAIR_SMN_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_smn_1.png TSA_POLAIR_SMN_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_smn_2.png TSA_POLAIR_SMN_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_smn_3.png TSA_POLAIR_SMN_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_smn_4.png TSA_POLAIR_SMN_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_smn_5.png TSA_POLAIR_SMN_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_mun_0.png TSA_POLAIR_MUN_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_mun_1.png TSA_POLAIR_MUN_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_mun_2.png TSA_POLAIR_MUN_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_mun_3.png TSA_POLAIR_MUN_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_mun_4.png TSA_POLAIR_MUN_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_mun_5.png TSA_POLAIR_MUN_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_epl_0.png TSA_POLAIR_EPL_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_epl_1.png TSA_POLAIR_EPL_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_epl_2.png TSA_POLAIR_EPL_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_epl_3.png TSA_POLAIR_EPL_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_epl_4.png TSA_POLAIR_EPL_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_epl_5.png TSA_POLAIR_EPL_5.PS -c quit

      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_nc2_0.png TSA_POLAIR_NC2_0.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_nc2_1.png TSA_POLAIR_NC2_1.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_nc2_2.png TSA_POLAIR_NC2_2.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_nc2_3.png TSA_POLAIR_NC2_3.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_nc2_4.png TSA_POLAIR_NC2_4.PS -c quit
      gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_polair_nc2_5.png TSA_POLAIR_NC2_5.PS -c quit



      # verwijderen tijdelijke files
      cd $ENV_GPS_LOCAL/input_nedwam
      rm -f TSF_IN
      rm -f TSF_REALS

      cd $ENV_GPS_LOCAL/output_nedwam
      rm -f TSA_*.PS

   endif
   
   

   ##################################################################################################
   #                                                                                                #
   #                                         verwijder sectie                                       #
   #                                                                                                #
   ##################################################################################################

   # 
   # verwijderen van 7 dagen oude files
   # (n.b. ze worden 3 uur na analyse opgehaald dus 7 dagen = -168 uur en dan nog - 3uur)
   # 
   
   set dtgw7 = `$BACKUPDATE $dtg -171`
   set dtgw7 = `echo $dtgw7|awk '{printf "%.10s",$0}'`

   #
   # verwijderen uit temp_nedwam directory
   #
   cd $ENV_GPS_LOCAL/temp_nedwam
   rm -f NEDWAM_TSA_ANA_$dtgw7".TMP"
   rm -f NEDWAM_TSA_K13_$dtgw7".TMP"
   rm -f NEDWAM_TSA_AUK_$dtgw7".TMP"
   rm -f NEDWAM_TSA_ELD_$dtgw7".TMP"
   rm -f NEDWAM_TSA_SMN_$dtgw7".TMP"
   rm -f NEDWAM_TSA_MUN_$dtgw7".TMP"
   rm -f NEDWAM_TSA_EPL_$dtgw7".TMP"
   rm -f NEDWAM_TSA_NC2_$dtgw7".TMP"


   #
   # verwijderen uit input_nedwam directory
   #
   cd $ENV_GPS_LOCAL/input_nedwam
   rm -f NEWH_TSA_$dtgw7"00_XX000_TW"

   # NB verwijderen tijdelijke input files TSF_IN en TSF_REALS hoeft niet ze worden telkens overschreven

   #
   # verwijderen uit temp_hirlam directory
   #
   cd $ENV_GPS_LOCAL/temp_hirlam
   rm -f HIRLAM_K13_$dtgw7".TMP"
   rm -f HIRLAM_AUK_$dtgw7".TMP"
   rm -f HIRLAM_ELD_$dtgw7".TMP"
   rm -f HIRLAM_SMN_$dtgw7".TMP"
   rm -f HIRLAM_MUN_$dtgw7".TMP"
   rm -f HIRLAM_EPL_$dtgw7".TMP"
   rm -f HIRLAM_NC2_$dtgw7".TMP"
   rm -f HIRLAM_ANA_$dtgw7".TMP"

   rm -f HIRLAM_UBA_AUK_$dtgw7".TMP"
   rm -f HIRLAM_UBA_ELD_$dtgw7".TMP"
   rm -f HIRLAM_UBA_EPL_$dtgw7".TMP"
   rm -f HIRLAM_UBA_K13_$dtgw7".TMP"
   rm -f HIRLAM_UBA_MUN_$dtgw7".TMP"
   rm -f HIRLAM_UBA_NC2_$dtgw7".TMP"
   rm -f HIRLAM_UBA_SMN_$dtgw7".TMP"

   #
   # verwijderen uit input_hirlam directory
   #
   cd $ENV_GPS_LOCAL/input_hirlam
   rm -f hirver$dtgw7



endif
