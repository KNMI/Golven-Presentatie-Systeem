#!/bin/csh




/*********************************************/
/*                                           */
/*        NB aanroepen via gps_main.sc       */
/*                                           */
/*********************************************/




if (-e $PRESENTDATE) then

   set dtg = `$PRESENTDATE 2`


   #
   # flag gezet ? (geeft aan dat de eerder opgehaalde nedwam-TMA of hirlam-UBA leeg was)
   #
   cd $ENV_GPS_LOCAL
   if (-e flag_file_par) then

      ###############################################################################################
      #                                                                                             #
      #                                       verwijder sectie                                      #
      #                                                                                             #
      ###############################################################################################

      # dus niets te up-daten en flag nog niet verwijderen (gebeurd door ....... uur hierna)
      


   else

      ###############################################################################################
      #                                                                                             #
      #                                        process sectie                                       #
      #                                                                                             #
      ###############################################################################################

      #
      # 6 uur geleden (dit script wordt b.v. te 12.55 aangeroepen om op de 06.00 UTC run file te werken)
      #

      #huidige tijd 6 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
      set dtgw6 = `$BACKUPDATE $dtg -6`
      set dtgw6 = `echo $dtgw6|awk '{printf "%.10s",$0}'`


      # NOGMAALS voor de zekerheid !
      # opgehaalde TSF forecast files nogmaals testen op niet 0 zijn en op wel aanwezig zijn
      #
      cd $ENV_GPS_LOCAL/input_nedwam
      if (!(-z NEWH_TMA_$dtgw6"00_XX000_TW") && -e NEWH_TMA_$dtgw6"00_XX000_TW") then 

         cd $ENV_GPS_LOCAL/input_hirlam
         if (!(-z LAMH_UBA_$dtgw6"00_00000_TW") && -e LAMH_UBA_$dtgw6"00_00000_TW" ) then


            #
            # reeds opgehaalde TSF files omzetten naar de generieke naam TSF_IN 
            # (originele file blijft nu ook behouden)
            # 
            cd $ENV_GPS_LOCAL/input_nedwam
            cp NEWH_TMA_$dtgw6"00_XX000_TW" TSF_IN

            cd $ENV_GPS_LOCAL/input_hirlam
            cp LAMH_UBA_$dtgw6"00_00000_TW" TSF_IN



            #
            # extraheren van TSF files (input is altijd file: TSF_IN; output is altijd file: TSF_REALS)
            #
            cd $ENV_GPS_LOCAL/input_nedwam
            $path_tsf_2_real tma_info.txt
  
            cd $ENV_GPS_LOCAL/input_hirlam
            $path_tsf_2_real uba_info.txt


            #process de data
            cd $ENV_GPS_LOCAL
            gps_new.exe $dtgw6 TMA


            #ps omzetten naar png en zetten in intranet dir 
            cd $ENV_GPS_LOCAL/output_nedwam
         


            #
            # nedwam parameters (hm0, hirlam-ff, tm0-1) (+cic data)
            # 
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_k13.png TMA_K13.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_ana.png TMA_ANA.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_nc2.png TMA_NC2.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_epl.png TMA_EPL.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_mun.png TMA_MUN.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_smn.png TMA_SMN.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_eld.png TMA_ELD.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_auk.png TMA_AUK.PS -c quit


            #
            # nedwam parameters -tweede pagina- (hs10, dir hs10, hs7)
            # 
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_k13.png TMA_b_K13.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_ana.png TMA_b_ANA.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_nc2.png TMA_b_NC2.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_epl.png TMA_b_EPL.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_mun.png TMA_b_MUN.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_smn.png TMA_b_SMN.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_eld.png TMA_b_ELD.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_b_auk.png TMA_b_AUK.PS -c quit

            #
            # nedwam parameters -derde pagina- (height swell, dir swell, period swell)
            # 
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_k13.png TMA_c_K13.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_ana.png TMA_c_ANA.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_nc2.png TMA_c_NC2.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_epl.png TMA_c_EPL.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_mun.png TMA_c_MUN.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_smn.png TMA_c_SMN.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_eld.png TMA_c_ELD.PS -c quit
            gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/par_c_auk.png TMA_c_AUK.PS -c quit


            #
            # verwijderen tijdelijke files
            #
            cd $ENV_GPS_LOCAL/input_nedwam
            rm -f TSF_IN
            rm -f TSF_REALS

            cd $ENV_GPS_LOCAL/input_hirlam
            rm -f TSF_IN
            rm -f TSF_REALS


            cd $ENV_GPS_LOCAL/output_nedwam
            rm -f TMA_*.PS

         endif

      endif

   endif

endif
