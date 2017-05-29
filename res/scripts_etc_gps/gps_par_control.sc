#!/bin/csh




/*********************************************/
/*                                           */
/*        NB aanroepen via gps_main.sc       */
/*                                           */
/*********************************************/





if (-e $PRESENTDATE) then


   set dtg = `$PRESENTDATE 2`


   # 
   # initialisatie; eventueel eerst flag verwijderen 
   #
   cd $ENV_GPS_LOCAL
   if (-e flag_file_par) then

      rm -f flag_file_par

   endif


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
   # TSF HIRLAM UBA files kopieeren naar GPS input_hirlam dir.
   #
   cd $ENV_GPS_LOCAL/input_hirlam
   cp $ENV_UBA_GLOBAL/LAMH_UBA_$dtgw3"00_00000_TW" $ENV_GPS_LOCAL/input_hirlam  



   #
   # TSF NEDWAM TMA files kopieeren naar GPS input_nedwam dir.
   #
   cd $ENV_GPS_LOCAL/input_nedwam
   cp $ENV_TMA_GLOBAL/NEWH_TMA_$dtgw3"00_XX000_TW" $ENV_GPS_LOCAL/input_nedwam  



   #
   # opgehaalde TSF forecast files testen op niet 0 zijn en op wel aanwezig zijn
   #
   cd $ENV_GPS_LOCAL/input_nedwam
   if (-z NEWH_TMA_$dtgw3"00_XX000_TW" || !(-e NEWH_TMA_$dtgw3"00_XX000_TW")) then
    
      # aanmaken van een flag (file) daarop kan dan weer getest worden door bv 'retry' script van 1 uur later
      cd $ENV_GPS_LOCAL
      touch flag_file_par
   
   else 

      cd $ENV_GPS_LOCAL/input_hirlam
      if (-z LAMH_UBA_$dtgw3"00_00000_TW" || !(-e LAMH_UBA_$dtgw3"00_00000_TW")) then
      
         # aanmaken van een flag (file) daarop kan dan weer getest worden door bv 'retry' script van 1 uur later
         cd $ENV_GPS_LOCAL
         touch flag_file_par
  
      else
   
         # files dus WEL aanwezig en niet leeg


         #
         # reeds opgehaalde TSF files omzetten naar de generieke naam TSF_IN 
         # (originele file blijft nu ook behouden)
         # 
         cd $ENV_GPS_LOCAL/input_nedwam
         cp NEWH_TMA_$dtgw3"00_XX000_TW" TSF_IN

         cd $ENV_GPS_LOCAL/input_hirlam
         cp LAMH_UBA_$dtgw3"00_00000_TW" TSF_IN



         ###############################################################################################
         #                                                                                             #
         #                                        process sectie                                       #
         #                                                                                             #
         ###############################################################################################


         #
         # extraheren van TSF file (input is altijd file: TSF_IN; output is altijd file: TSF_REALS)
         #
         cd $ENV_GPS_LOCAL/input_nedwam
         $path_tsf_2_real tma_info.txt

         cd $ENV_GPS_LOCAL/input_hirlam
         $path_tsf_2_real uba_info.txt


         #process de data (zowel hirlam-UBA als nedwam-TMA data worden afgehandeld)
         cd $ENV_GPS_LOCAL
         gps_new.exe $dtgw3 TMA


         #ps omzetten naar png en zetten in intranet dir 
         cd $ENV_GPS_LOCAL/output_nedwam

         #
         # nedwam parameters (hm0, hirlam-ff, tm0-1)
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
   rm -f NEDWAM_TMA_ANA_$dtgw7".TMP"
   rm -f NEDWAM_TMA_K13_$dtgw7".TMP"
   rm -f NEDWAM_TMA_AUK_$dtgw7".TMP"
   rm -f NEDWAM_TMA_ELD_$dtgw7".TMP"
   rm -f NEDWAM_TMA_SMN_$dtgw7".TMP"
   rm -f NEDWAM_TMA_MUN_$dtgw7".TMP"
   rm -f NEDWAM_TMA_EPL_$dtgw7".TMP"
   rm -f NEDWAM_TMA_NC2_$dtgw7".TMP"

   #
   # verwijderen uit temp_hirlam directory
   #
   cd $ENV_GPS_LOCAL/temp_nedwam
   rm -f HIRLAM_UBA_ANA_$dtgw7".TMP"
   rm -f HIRLAM_UBA_K13_$dtgw7".TMP"
   rm -f HIRLAM_UBA_AUK_$dtgw7".TMP"
   rm -f HIRLAM_UBA_ELD_$dtgw7".TMP"
   rm -f HIRLAM_UBA_SMN_$dtgw7".TMP"
   rm -f HIRLAM_UBA_MUN_$dtgw7".TMP"
   rm -f HIRLAM_UBA_EPL_$dtgw7".TMP"
   rm -f HIRLAM_UBA_NC2_$dtgw7".TMP"


   #
   # verwijderen uit input_nedwam directory
   #
   cd $ENV_GPS_LOCAL/input_nedwam
   rm -f NEWH_TMA_$dtgw7"00_XX000_TW"


   #
   # verwijderen uit input_hirlam directory
   #
   cd $ENV_GPS_LOCAL/input_hirlam
   rm -f LAMH_UBA_$dtgw7"00_00000_TW"


endif
