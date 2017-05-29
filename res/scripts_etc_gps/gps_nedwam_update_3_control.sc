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

      ###############################################################################################
      #                                                                                             #
      #                                       verwijder sectie                                      #
      #                                                                                             #
      ###############################################################################################

      # dus niets te up-daten en flag nog niet verwijderen (gebeurd door de tweede update control 3 uur hierna)
      


   else

      ###############################################################################################
      #                                                                                             #
      #                                        process sectie                                       #
      #                                                                                             #
      ###############################################################################################

      #
      # 3 uur geleden (dit script wordt b.v. te 9.50 aangeroepen om op de 06.00 UTC run file te werken)
      #

      #huidige tijd 3 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
      set dtgw3 = `$BACKUPDATE $dtg -3`
      set dtgw3 = `echo $dtgw3|awk '{printf "%.10s",$0}'`


      # NOGMAALS voor de zekerheid !
      # opgehaalde TSF forecast file nogmaals testen op niet 0 zijn en op wel aanwezig zijn
      #
      cd $ENV_GPS_LOCAL/input_nedwam
      if (!(-z NEWH_TSA_$dtgw3"00_XX000_TW") && -e NEWH_TSA_$dtgw3"00_XX000_TW") then


         #
         # reeds opgehaalde TSF file omzetten naar de generieke naam TSF_IN 
         # (originele file blijft nu ook behouden)
         # 
         cp NEWH_TSA_$dtgw3"00_XX000_TW" TSF_IN


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
         # 1-dimensionaal nedwam spectrum (+ cic metingen)
         #
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_ana_0.png TSA_ANA_0.PS -c quit
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_k13_0.png TSA_K13_0.PS -c quit
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_auk_0.png TSA_AUK_0.PS -c quit
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_eld_0.png TSA_ELD_0.PS -c quit
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_smn_0.png TSA_SMN_0.PS -c quit
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_mun_0.png TSA_MUN_0.PS -c quit
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_epl_0.png TSA_EPL_0.PS -c quit
         gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/tsa_nc2_0.png TSA_NC2_0.PS -c quit




         # verwijderen tijdelijke files
         cd $ENV_GPS_LOCAL/input_nedwam
         rm -f TSF_IN
         rm -f TSF_REALS

         cd $ENV_GPS_LOCAL/output_nedwam
         rm -f TSA_*.PS


      endif

   endif

endif
