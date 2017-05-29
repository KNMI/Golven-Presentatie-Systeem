#!/bin/csh



/*********************************************/
/*                                           */
/*        NB aanroepen via gps_main.sc       */
/*                                           */
/*********************************************/



if (-e $PRESENTDATE) then


   set dtg = `$PRESENTDATE 2`


   #
   # flag gezet ? (geeft aan dat de eerder opgehaalde TSA leeg was)
   #
   cd $ENV_GPS_LOCAL
   if (-e flag_file) then


      #
      # 5 uur geleden (dit script wordt b.v. te 11.50 aangeroepen om de 06.00 UTC file op te halen)
      #

      #huidige tijd 5 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
      set dtgw5 = `$BACKUPDATE $dtg -5`
      set dtgw5 = `echo $dtgw5|awk '{printf "%.10s",$0}'`
   



      ###############################################################################################
      #                                                                                             #
      #                                      input sectie                                           #
      #                                                                                             #
      ###############################################################################################



      #
      # TSF (TSA golven) files kopieeren naar GPS input_nedwam dir.
      #
      cd $ENV_GPS_LOCAL/input_nedwam
      cp $ENV_TSA_GLOBAL/NEWH_TSA_$dtgw5"00_XX000_TW" $ENV_GPS_LOCAL/input_nedwam  




      #
      # opgehaalde TSF forecast file testen op niet 0 zijn en op wel aanwezig zijn
      #
      cd $ENV_GPS_LOCAL/input_nedwam
      if (-z NEWH_TSA_$dtgw5"00_XX000_TW" || !(-e NEWH_TSA_$dtgw5"00_XX000_TW")) then

         #
         # schrijven naar log file
         #
         cd $ENV_GPS_LOG
         echo `date` >>& log.txt
         echo NEWH_TSA_$dtgw5"00_XX000_TW empty or not present" >>& log.txt
         echo "************************************************************************************" >>& log.txt
 
      else

         # file is dus nu WEL aanwezig

         #
         # reeds opgehaalde TSF file omzetten naar de generieke naam TSF_IN (orig. file blijft behouden)
         # 
         cp NEWH_TSA_$dtgw5"00_XX000_TW" TSF_IN




         ############################################################################################
         #                                                                                          #
         #                                        process sectie                                    #
         #                                                                                          #
         ############################################################################################


         #
         # extraheren van TSF file (input is altijd file: TSF_IN; output is altijd file: TSF_REALS)
         #
         cd $ENV_GPS_LOCAL/input_nedwam
         $path_tsf_2_real tsf_info.txt



         #process de data
         cd $ENV_GPS_LOCAL
         gps_new.exe $dtgw5 TSA


         #ps omzetten naar png en zetten in intranet dir (xxx_0 = 0, +3, +6; xxx_1 = +9, +15, +18 etc.)
         cd $ENV_GPS_LOCAL/output_nedwam

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
         # polaire plotjes
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


         ############################################################################################
         #                                                                                          #
         #                                       verwijder sectie                                   #
         #                                                                                          #
         ############################################################################################


         cd $ENV_GPS_LOCAL
         rm -f flag_file


      endif

        
      # NB de nu nog aanwezige flag-file wordt later verwijderd in gps_nedwam_update_6_control.sc


   endif

endif
