#!/bin/csh
#set PRESENTDATE       = '/bdsocl/usr/knmi/bin/extdate'
set PRESENTDATE = '/usr/knmi/bin/extdate'

#set path_convert      = '/usr/local/bin/convert -page letter'
set path_intranet_gps = '~/GPS/output_manual'

#set path_tsf_2_real = '/bdarbs/raid/DEC/usr/knmi/bin/tsf_2_real'
set path_tsf_2_real = 'tsf_2_real'

#unsetenv GS_LIB


#####################################################################################################
#                                                                                                   #
#                                  HANDLEIDING voor op de bdawo8                                    #
#                                                                                                   #
#####################################################################################################
# 
#
############ STAP 1 ###########
#
# benodigde directory structuur:
# ~GPS
#   |--- input_cic     (vast)
#   |--- input_nedwam  (vast)
#   |--- input_hirlam  (vast)
#   |--- output_manual (eventueel te veranderen met 'set path_intranet_gps =')
#   |--- temp_cic      (vast)
#   |--- temp_hirlam   (vast)
#   |--- temp_nedwam   (vast)
#   |--- output_cic    (vast)
#   |--- output_nedwam (vast)
#
#
############ STAP 2 ###########
#
# benodigde files:
# bijvoorbeeld voor de verwachting van (RUN) 03-02-2002 12.00 UTC
#
# --> dir GPS             : gps_new.exe                     (vast)
#                           gps_nedwam_manual_control.sc    (vast)
# --> dir GPS/input_nedwam: tsf_info.txt                    (vast) 
#                           NEWH_TSA_200202031200_XX000_TW
# --> dir GPS/input_hirlam: hirver2002020312
# --> dir GPS/input_cic   : WAVE_LDS_200202031200_00000_LC
#                           WAVE_LDS_200202031500_00000_LC
#                           WAVE_LDS_200202031800_00000_LC
#                           WAVE_LDS_200202032100_00000_LC
#                           WAVE_LDS_200202040000_00000_LC
#                           WAVE_LDS_200202040300_00000_LC
#                           WAVE_LDS_200202040600_00000_LC
#                           WAVE_LDS_200202040900_00000_LC
#                           WAVE_LDS_200202041200_00000_LC
#                           WAVE_LDS_200202041500_00000_LC
#                           WAVE_LDS_200202041800_00000_LC
#                           WAVE_LDS_200202042100_00000_LC
#                           WAVE_LDS_200202050000_00000_LC
#                           WAVE_LDS_200202050300_00000_LC
#                           WAVE_LDS_200202050600_00000_LC
#                           WAVE_LDS_200202050900_00000_LC
#                           WAVE_LDS_200202051200_00000_LC
#
#
############ STAP 2 ###########
#
# run voor boventaand voorbeeld: gps_nedwam_manual_control.sc 2002020312
#
#
############ STAP 3 ###########
#
# - aangemaakte png files eventueel copieren naar doel (in BINAIRE mode)
# - input files verwijderen (NEWH_TSA_200202031200_XX000_TW etc.)
#




#####################################################################################################
#                                                                                                   #
#                                                START                                              #
#                                                                                                   #
#####################################################################################################
if ($#argv >= 1) then

   #set dtgw = 'date +%y%m%d%H'
   #set dtgw = `echo $dtgw|awk '{printf "%.10s",$0}'`
   set dtgw = ($argv[1])
   set dtgw = `echo $dtgw|awk '{printf "%.10s",$0}'`

   #echo dtgw is $dtgw


   ##################################################################################################
   #                                                                                                #
   #                                      CIC input sectie                                          #
   #                                                                                                #
   ##################################################################################################


   #
   # input tijd (= JJJJMMDDUU) + 0 uur vooruit 
   #
   set dtgw0 = `/usr/knmi/bin/backupdate $dtgw +0`
   set dtgw0 = `echo $dtgw0|awk '{printf "%.10s",$0}'`


   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw0"00_00000_LC") then

#      echo gepasseerd binnen dtgw0 if

      cd ~/GPS
      gps_new.exe $dtgw0 CIC 

   endif


   #
   # input tijd (= JJJJMMDDUU) + 3 uur vooruit 
   #
   set dtgw3 = `/usr/knmi/bin/backupdate $dtgw +3`
   set dtgw3 = `echo $dtgw3|awk '{printf "%.10s",$0}'`
 
   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw3"00_00000_LC") then

#      echo gepasseerd binnen dtgw3 if

      cd ~/GPS
      gps_new.exe $dtgw3 CIC 

   endif


   #
   # input tijd (= JJJJMMDDUU) + 6 uur vooruit 
   #
   set dtgw6 = `/usr/knmi/bin/backupdate $dtgw +6`
   set dtgw6 = `echo $dtgw6|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw6"00_00000_LC") then

#      echo gepasseerd binnen dtgw6 if

      cd ~/GPS
      gps_new.exe $dtgw6 CIC 

   endif



   #
   # input tijd (= JJJJMMDDUU) + 9 uur vooruit 
   #
   set dtgw9 = `/usr/knmi/bin/backupdate $dtgw +9`
   set dtgw9 = `echo $dtgw9|awk '{printf "%.10s",$0}'`
   
   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw9"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw9 CIC 

   endif


   #
   # input tijd (= JJJJMMDDUU) + 12 uur vooruit 
   #
   set dtgw12 = `/usr/knmi/bin/backupdate $dtgw +12`
   set dtgw12 = `echo $dtgw12|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw12"00_00000_LC") then
      
      cd ~/GPS
      gps_new.exe $dtgw12 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 15 uur vooruit 
   #
   set dtgw15 = `/usr/knmi/bin/backupdate $dtgw +15`
   set dtgw15 = `echo $dtgw15|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw15"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw15 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 18 uur vooruit 
   #
   set dtgw18 = `/usr/knmi/bin/backupdate $dtgw +18`
   set dtgw18 = `echo $dtgw18|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw18"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw18 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 21 uur vooruit 
   #
   set dtgw21 = `/usr/knmi/bin/backupdate $dtgw +21`
   set dtgw21 = `echo $dtgw21|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw21"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw21 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 24 uur vooruit 
   #
   set dtgw24 = `/usr/knmi/bin/backupdate $dtgw +24`
   set dtgw24 = `echo $dtgw24|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw24"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw24 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 27 uur vooruit 
   #
   set dtgw27 = `/usr/knmi/bin/backupdate $dtgw +27`
   set dtgw27 = `echo $dtgw27|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw27"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw27 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 30 uur vooruit 
   #
   set dtgw30 = `/usr/knmi/bin/backupdate $dtgw +30`
   set dtgw30 = `echo $dtgw30|awk '{printf "%.10s",$0}'`
   
   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw30"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw30 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 33 uur vooruit 
   #
   set dtgw33 = `/usr/knmi/bin/backupdate $dtgw +33`
   set dtgw33 = `echo $dtgw33|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw33"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw33 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 36 uur vooruit 
   #
   set dtgw36 = `/usr/knmi/bin/backupdate $dtgw +36`
   set dtgw36 = `echo $dtgw36|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw36"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw36 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 39 uur vooruit 
   #
   set dtgw39 = `/usr/knmi/bin/backupdate $dtgw +39`
   set dtgw39 = `echo $dtgw39|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw39"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw39 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 42 uur vooruit 
   #
   set dtgw42 = `/usr/knmi/bin/backupdate $dtgw +42`
   set dtgw42 = `echo $dtgw42|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw42"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw42 CIC 

   endif


   # 
   # input tijd (= JJJJMMDDUU) + 45 uur vooruit 
   #
   set dtgw45 = `/usr/knmi/bin/backupdate $dtgw +45`
   set dtgw45 = `echo $dtgw45|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw45"00_00000_LC") then
 
      cd ~/GPS
      gps_new.exe $dtgw45 CIC 

   endif

 
   # 
   # input tijd (= JJJJMMDDUU) + 48 uur vooruit 
   #
   set dtgw48 = `/usr/knmi/bin/backupdate $dtgw +48`
   set dtgw48 = `echo $dtgw48|awk '{printf "%.10s",$0}'`

   cd ~/GPS/input_cic
   if (-e WAVE_LDS_$dtgw48"00_00000_LC") then

      cd ~/GPS
      gps_new.exe $dtgw48 CIC 

   endif



   ##################################################################################################
   #                                                                                                #
   #                                    HIRLAM input sectie                                         #
   #                                                                                                #
   ##################################################################################################

   #
   # HIRLAM files aanwezig in manual dir ?
   #

   if (!(-z hirver$dtgw) || -e hirver$dtgw) then

      # indien aanwezig worden ze automatisch meegenomen

   endif


   ##################################################################################################
   #                                                                                                #
   #                                   NEDWAM input sectie                                          #
   #                                                                                                #
   ##################################################################################################

   #
   # TSF (TSA) forecast file testen op niet 0 zijn en op wel aanwezig zijn
   # 
   cd ~/GPS/input_nedwam
   if (-z NEWH_TSA_$dtgw"00_XX000_TW" || !(-e NEWH_TSA_$dtgw"00_XX000_TW")) then

      # dan dus niet aanwezig of leeg
  
   else

      # TSF (TSA) forecast file dus WEL aanwezig en niet leeg


      #
      # reeds aanwezige TSF file omzetten naar de generieke naam TSF_IN 
      # (originele file blijft nu ook behouden)
      # 
      cd ~/GPS/input_nedwam
      cp NEWH_TSA_$dtgw"00_XX000_TW" TSF_IN



      #
      # extraheren van TSF file (input is altijd file: TSF_IN; output is altijd file: TSF_REALS)
      #
      cd ~/GPS/input_nedwam
      $path_tsf_2_real tsf_info.txt



      #process de data
      cd ~/GPS
      gps_new.exe $dtgw TSA


      #ps omzetten naar png en zetten in 'output'(intranet) dir (xxx_0 = 0, +3, +6; xxx_1 = +9, +15, +18 etc.)
      cd ~/GPS/output_nedwam

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




      # verwijderen tijdelijke files
      #cd ~/GPS/input_nedwam
      #rm -f TSF_IN
      #rm -f TSF_REALS

      #cd ~/GPS/output_nedwam
      #rm -f *.PS

   endif
   
   

   ##################################################################################################
   #                                                                                                #
   #                                      verwijder sectie                                          #
   #                                                                                                #
   ##################################################################################################

   # 
   # HANDMATIG



endif
