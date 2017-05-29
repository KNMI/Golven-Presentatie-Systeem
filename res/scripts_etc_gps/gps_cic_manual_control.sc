#!/bin/csh
#set PRESENTDATE       = '/bdsocl/usr/knmi/bin/extdate'
set PRESENTDATE = '/usr/knmi/bin/extdate'

set path_intranet_gps = '/users/stam/GPS/output_manual'


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
# bijvoorbeeld voor de waarnemingen van 03-02-2002 12.00 UTC en t/m 12 uur terug
#
# --> dir GPS             : gps_new.exe                     (vast)
#                           gps_cic_manual_control.sc       (vast)
# --> dir GPS/input_nedwam: n.v.t. 
#                           n.v.t.
# --> dir GPS/input_hirlam: n.v.t.
# --> dir GPS/input_cic   : WAVE_LDS_200202031200_00000_LC
#                           WAVE_LDS_200202031100_00000_LC
#                           WAVE_LDS_200202031000_00000_LC
#                           WAVE_LDS_200202030900_00000_LC
#                           WAVE_LDS_200202030800_00000_LC
#                           WAVE_LDS_200202030700_00000_LC
#                           WAVE_LDS_200202030600_00000_LC
#                           WAVE_LDS_200202030500_00000_LC
#                           WAVE_LDS_200202030400_00000_LC
#                           WAVE_LDS_200202030300_00000_LC
#                           WAVE_LDS_200202030200_00000_LC
#                           WAVE_LDS_200202030100_00000_LC
#
#
#                           2002020312.CIC
#                           2002020311.CIC
#                           etc. 
#
############ STAP 2 ###########
#
# run voor boventaand voorbeeld: gps_cic_manual_control.sc 2002020312
#
#
############ STAP 3 ###########
#
# - aangemaakte png files eventueel copieren naar doel (in BINAIRE mode)
# - input files verwijderen (WAVE_LDS_200202031200_00000_LC etc.)
#




#####################################################################################################
#                                                                                                   #
#                                                START                                              #
#                                                                                                   #
#####################################################################################################
if ($#argv >= 1) then

   set dtg = ($argv[1])
   set dtg = `echo $dtg|awk '{printf "%.10s",$0}'`


   #
   # 0, 1 en 2 etc. uur geleden (effectief 1,2,3 etc. uur geleden)
   #

   #huidige tijd 0 uur terug (format JJJJMMDDUU, dus minuten verwaarloosd)
   set dtgw0 = `/usr/knmi/bin/backupdate $dtg -0`
   set dtgw0 = `echo $dtgw0|awk '{printf "%.10s",$0}'`
   
   set dtgw1 = `/usr/knmi/bin/backupdate $dtg -1`
   set dtgw1 = `echo $dtgw1|awk '{printf "%.10s",$0}'`

   set dtgw2 = `/usr/knmi/bin/backupdate $dtg -2`
   set dtgw2 = `echo $dtgw2|awk '{printf "%.10s",$0}'`

   set dtgw3 = `/usr/knmi/bin/backupdate $dtg -3`
   set dtgw3 = `echo $dtgw3|awk '{printf "%.10s",$0}'`

   set dtgw4 = `/usr/knmi/bin/backupdate $dtg -4`
   set dtgw4 = `echo $dtgw4|awk '{printf "%.10s",$0}'`

   set dtgw5 = `/usr/knmi/bin/backupdate $dtg -5`
   set dtgw5 = `echo $dtgw5|awk '{printf "%.10s",$0}'`
   
   set dtgw6 = `/usr/knmi/bin/backupdate $dtg -6`
   set dtgw6 = `echo $dtgw6|awk '{printf "%.10s",$0}'`

   set dtgw7 = `/usr/knmi/bin/backupdate $dtg -7`
   set dtgw7 = `echo $dtgw7|awk '{printf "%.10s",$0}'`

   set dtgw8 = `/usr/knmi/bin/backupdate $dtg -8`
   set dtgw8 = `echo $dtgw8|awk '{printf "%.10s",$0}'`

   set dtgw9 = `/usr/knmi/bin/backupdate $dtg -9`
   set dtgw9 = `echo $dtgw9|awk '{printf "%.10s",$0}'`

   set dtgw10 = `/usr/knmi/bin/backupdate $dtg -10`
   set dtgw10 = `echo $dtgw10|awk '{printf "%.10s",$0}'`

   set dtgw11 = `/usr/knmi/bin/backupdate $dtg -11`
   set dtgw11 = `echo $dtgw11|awk '{printf "%.10s",$0}'`

   set dtgw12 = `/usr/knmi/bin/backupdate $dtg -12`
   set dtgw12 = `echo $dtgw12|awk '{printf "%.10s",$0}'`

   set dtgw13 = `/usr/knmi/bin/backupdate $dtg -13`
   set dtgw13 = `echo $dtgw13|awk '{printf "%.10s",$0}'`
   
   set dtgw14 = `/usr/knmi/bin/backupdate $dtg -14`
   set dtgw14 = `echo $dtgw14|awk '{printf "%.10s",$0}'`

   set dtgw15 = `/usr/knmi/bin/backupdate $dtg -15`
   set dtgw15 = `echo $dtgw15|awk '{printf "%.10s",$0}'`

   set dtgw16 = `/usr/knmi/bin/backupdate $dtg -16`
   set dtgw16 = `echo $dtgw16|awk '{printf "%.10s",$0}'`

   set dtgw17 = `/usr/knmi/bin/backupdate $dtg -17`
   set dtgw17 = `echo $dtgw17|awk '{printf "%.10s",$0}'`

   set dtgw18 = `/usr/knmi/bin/backupdate $dtg -18`
   set dtgw18 = `echo $dtgw18|awk '{printf "%.10s",$0}'`
   
   set dtgw19 = `/usr/knmi/bin/backupdate $dtg -19`
   set dtgw19 = `echo $dtgw19|awk '{printf "%.10s",$0}'`

   set dtgw20 = `/usr/knmi/bin/backupdate $dtg -20`
   set dtgw20 = `echo $dtgw20|awk '{printf "%.10s",$0}'`

   set dtgw21 = `/usr/knmi/bin/backupdate $dtg -21`
   set dtgw21 = `echo $dtgw21|awk '{printf "%.10s",$0}'`

   set dtgw22 = `/usr/knmi/bin/backupdate $dtg -22`
   set dtgw22 = `echo $dtgw22|awk '{printf "%.10s",$0}'`

   set dtgw23 = `/usr/knmi/bin/backupdate $dtg -23`
   set dtgw23 = `echo $dtgw23|awk '{printf "%.10s",$0}'`

   set dtgw24 = `/usr/knmi/bin/backupdate $dtg -24`
   set dtgw24 = `echo $dtgw24|awk '{printf "%.10s",$0}'`

   set dtgw25 = `/usr/knmi/bin/backupdate $dtg -25`
   set dtgw25 = `echo $dtgw25|awk '{printf "%.10s",$0}'`
   
   set dtgw26 = `/usr/knmi/bin/backupdate $dtg -26`
   set dtgw26 = `echo $dtgw26|awk '{printf "%.10s",$0}'`

   set dtgw27 = `/usr/knmi/bin/backupdate $dtg -27`
   set dtgw27 = `echo $dtgw27|awk '{printf "%.10s",$0}'`

   set dtgw28 = `/usr/knmi/bin/backupdate $dtg -28`
   set dtgw28 = `echo $dtgw28|awk '{printf "%.10s",$0}'`

   set dtgw29 = `/usr/knmi/bin/backupdate $dtg -29`
   set dtgw29 = `echo $dtgw29|awk '{printf "%.10s",$0}'`

   set dtgw30 = `/usr/knmi/bin/backupdate $dtg -30`
   set dtgw30 = `echo $dtgw30|awk '{printf "%.10s",$0}'`
   
   set dtgw31 = `/usr/knmi/bin/backupdate $dtg -31`
   set dtgw31 = `echo $dtgw31|awk '{printf "%.10s",$0}'`

   set dtgw32 = `/usr/knmi/bin/backupdate $dtg -32`
   set dtgw32 = `echo $dtgw32|awk '{printf "%.10s",$0}'`

   set dtgw33 = `/usr/knmi/bin/backupdate $dtg -33`
   set dtgw33 = `echo $dtgw33|awk '{printf "%.10s",$0}'`

   set dtgw34 = `/usr/knmi/bin/backupdate $dtg -34`
   set dtgw34 = `echo $dtgw34|awk '{printf "%.10s",$0}'`

   set dtgw35 = `/usr/knmi/bin/backupdate $dtg -35`
   set dtgw35 = `echo $dtgw35|awk '{printf "%.10s",$0}'`

   set dtgw36 = `/usr/knmi/bin/backupdate $dtg -36`
   set dtgw36 = `echo $dtgw36|awk '{printf "%.10s",$0}'`

   set dtgw37 = `/usr/knmi/bin/backupdate $dtg -37`
   set dtgw37 = `echo $dtgw37|awk '{printf "%.10s",$0}'`

   set dtgw38 = `/usr/knmi/bin/backupdate $dtg -38`
   set dtgw38 = `echo $dtgw38|awk '{printf "%.10s",$0}'`
   
   set dtgw39 = `/usr/knmi/bin/backupdate $dtg -39`
   set dtgw39 = `echo $dtgw39|awk '{printf "%.10s",$0}'`

   set dtgw40 = `/usr/knmi/bin/backupdate $dtg -40`
   set dtgw40 = `echo $dtgw40|awk '{printf "%.10s",$0}'`

   set dtgw41 = `/usr/knmi/bin/backupdate $dtg -41`
   set dtgw41 = `echo $dtgw41|awk '{printf "%.10s",$0}'`

   set dtgw42 = `/usr/knmi/bin/backupdate $dtg -42`
   set dtgw42 = `echo $dtgw42|awk '{printf "%.10s",$0}'`

   set dtgw43 = `/usr/knmi/bin/backupdate $dtg -43`
   set dtgw43 = `echo $dtgw43|awk '{printf "%.10s",$0}'`
   
   set dtgw44 = `/usr/knmi/bin/backupdate $dtg -44`
   set dtgw44 = `echo $dtgw44|awk '{printf "%.10s",$0}'`

   set dtgw45 = `/usr/knmi/bin/backupdate $dtg -45`
   set dtgw45 = `echo $dtgw45|awk '{printf "%.10s",$0}'`

   set dtgw46 = `/usr/knmi/bin/backupdate $dtg -46`
   set dtgw46 = `echo $dtgw46|awk '{printf "%.10s",$0}'`

   set dtgw47 = `/usr/knmi/bin/backupdate $dtg -47`
   set dtgw47 = `echo $dtgw47|awk '{printf "%.10s",$0}'`

   set dtgw48 = `/usr/knmi/bin/backupdate $dtg -48`
   set dtgw48 = `echo $dtgw48|awk '{printf "%.10s",$0}'`





   #process de data
   gps_new.exe $dtgw0 CIC
#   gps_new.exe $dtgw2 CIC
#   gps_new.exe $dtgw3 CIC
#   gps_new.exe $dtgw4 CIC
#   gps_new.exe $dtgw5 CIC
#   gps_new.exe $dtgw6 CIC
#   gps_new.exe $dtgw7 CIC
#   gps_new.exe $dtgw8 CIC
#   gps_new.exe $dtgw9 CIC
#   gps_new.exe $dtgw10 CIC
#   gps_new.exe $dtgw11 CIC
#   gps_new.exe $dtgw12 CIC
#   gps_new.exe $dtgw13 CIC
#   gps_new.exe $dtgw14 CIC
#   gps_new.exe $dtgw15 CIC
#   gps_new.exe $dtgw16 CIC
#   gps_new.exe $dtgw17 CIC
#   gps_new.exe $dtgw18 CIC
#   gps_new.exe $dtgw19 CIC
#   gps_new.exe $dtgw20 CIC
#   gps_new.exe $dtgw21 CIC
#   gps_new.exe $dtgw22 CIC
#   gps_new.exe $dtgw23 CIC
#   gps_new.exe $dtgw24 CIC
#   gps_new.exe $dtgw25 CIC
#   gps_new.exe $dtgw26 CIC
#   gps_new.exe $dtgw27 CIC
#   gps_new.exe $dtgw28 CIC
#   gps_new.exe $dtgw29 CIC
#   gps_new.exe $dtgw30 CIC
#   gps_new.exe $dtgw31 CIC
#   gps_new.exe $dtgw32 CIC
#   gps_new.exe $dtgw33 CIC
#   gps_new.exe $dtgw34 CIC
#   gps_new.exe $dtgw35 CIC
#   gps_new.exe $dtgw36 CIC
#   gps_new.exe $dtgw37 CIC
#   gps_new.exe $dtgw38 CIC
#   gps_new.exe $dtgw39 CIC
#   gps_new.exe $dtgw40 CIC
#   gps_new.exe $dtgw41 CIC
#   gps_new.exe $dtgw42 CIC
#   gps_new.exe $dtgw43 CIC
#   gps_new.exe $dtgw44 CIC
#   gps_new.exe $dtgw45 CIC
#   gps_new.exe $dtgw46 CIC
#   gps_new.exe $dtgw47 CIC
#   gps_new.exe $dtgw48 CIC






   #ps omzetten naar png en zetten in intranet dir
   cd ~/GPS/output_cic
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_ana_man.png CIC_ANA.PS -c quit
#   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_k13_man.png CIC_K13.PS -c quit
#   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_auk_man.png CIC_AUK.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_eld_man.png CIC_ELD.PS -c quit
#   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_smn_man.png CIC_SMN.PS -c quit
#   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_mun_man.png CIC_MUN.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_epl_man.png CIC_EPL.PS -c quit
   gs -dQUIET -sPAPERSIZE=letter -dNOPAUSE -sDEVICE=png16m -sOutputFile=$path_intranet_gps/cic_nc2_man.png CIC_NC2.PS -c quit



   #
   # verwijderen -tussen resultaat- postscript files
   #
   #cd ~/GPS/output_cic
   #rm -f CIC_*.PS




   #verwijderen uit input directory
   #
   # HANDMATIG
   


endif
