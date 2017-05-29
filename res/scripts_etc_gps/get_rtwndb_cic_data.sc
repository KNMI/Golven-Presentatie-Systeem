#!/bin/csh





/*********************************************/
/*                                           */
/*        NB aanroepen via gps_main.sc       */
/*                                           */
/*********************************************/






if (-e $PRESENTDATE) then

   set dtg = `$PRESENTDATE 2`

   #
   # ophalen files van 0 uur terug
   # 
   set dtgw = `$BACKUPDATE $dtg -0`
   set dtgw = `echo $dtgw|awk '{printf "%.10s",$0}'`

   cd $ENV_GPS_LOCAL
   get_rtwndb_cic_data.exe $dtgw


   #
   # verwijderen van 7 dagen oude files
   # 
   set dtgw7 = `$BACKUPDATE $dtg -168`
   set dtgw7 = `echo $dtgw7|awk '{printf "%.10s",$0}'`

   #verwijderen uit input_cic dir
   cd $ENV_GPS_LOCAL/input_cic
   rm -f $dtgw7".HWA"


endif
