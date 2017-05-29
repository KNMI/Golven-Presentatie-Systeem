#if !defined(gps_h)                         /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>                           /* o.a. cos, sin */
#include <values.h>                         /* MAXINT /MAXFLOAT */

#if defined(WINDOWS)                        /* zie gps.h */
#include <dir.h>                            /* o.a. getcwd() in windows */
#else
#include <unistd.h>                         /* o.a. getcwd() in UNIX */
#endif


/* function prototypes in andere module */
extern int Write_Log(char* bericht);
extern int Bereken_BFI(double freq_array[], const int aantal_freq_array_elementen, const double fp, const double num_Tm0_1, double* BFI, double* K40, double* P, const char* filenaam, const double num_Hm0, const char* station, const double num_Hmax, const double Hmax_Hm0);

/* functions in deze module */
int Read_Input_Files(void);
int Bepaal_CIC_LDS_filenaam(void);
int Read_CIC_LDS(void);
int Write_Temp_File(void);
int Initialiseer_Spectrum_Array(void);
int Bepaal_CIC_CIC_filenaam(void);
int Read_CIC_CIC(const char* station);
int Bepaal_CIC_LFR_filenaam(void);
int Read_CIC_LFR(const char* station);

/* externe var's */
extern char JJJJMMDDUU[11];                         /* via argument list */
extern char OS[8];
extern char stations[NUM_STATIONS][NUM_STATIONS_2]; /* gpsmain.c; NUM_STATIONS arrayplaatsen van elk 4 char */

/* var's globaal binnen deze module */
char inputfilenaam[256];                            /* cic lds file */
char spectrum[NUM_SPECTRUM][NUM_SPECTRUM_2];
char cic_inputfilenaam[256];                        /* cic cic file (als de lds file geen data voor dat betref. station */
char lfr_inputfilenaam[256];                        /* lfr cic file (om Hmax er uit te halen, deze zit alleen in de lfr file) */



/*****************************************************************************/
/*                                                                           */
/*                           Read_Input_Files                                */
/*                                                                           */
/*****************************************************************************/
int Read_Input_Files()
{
   char tempfilenaam[256];
   int i;


   /* LDS files (hierin de basis cic data) INPUT */
   Bepaal_CIC_LDS_filenaam();
   Read_CIC_LDS();                                           /* via deze function wordt: Write_Temp_File(); aangeroepen */


   /* als er geen 1D en 2D spectrum is worden er geen temp files aangemaakt voor dat station */
   /* echter er kunnen wel Hm0, TM0-1 etc. zijn van en eenvoudige (baak) sensor */
   /* NB als er alleen een 1D spectrum is worden wel LDS files aangemaakt maar TMO-1 en LFE staan dan op 0 ?? */
   for (i = 0; i < NUM_STATIONS; i++)
   {
      if (strcmp(stations[i], "\0") != 0)                    /* niet leeg */
      {
         /* bepalen TEMP filenaam  */                        /* b.v. CIC_LDS_K13_20010806.TMP */
         tempfilenaam[0] = '\0';

         if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
            strcpy(tempfilenaam, "temp_cic\\");
         else                                                /* UNIX */
            strcpy(tempfilenaam, "temp_cic/");

         strcat(tempfilenaam, "CIC_LDS_");                    /* hier staat dan wel CIC_LDS maar de data voor deze temp file kan ook uit andere input files komen (lds, cic, lfr) */
         strncat(tempfilenaam, stations[i], 3);               /* // strcat(tempfilenaam, spectrum[0]); */
         strcat(tempfilenaam, "_");
         strcat(tempfilenaam, JJJJMMDDUU);                    /* //strcat(tempfilenaam, spectrum[1]); */
         strcat(tempfilenaam, ".TMP");

         /*if ((temp = fopen(tempfilenaam, "r")) == NULL)*/
         if ((fopen(tempfilenaam, "r")) == NULL)
         {
            /* dus geen LDS temp file van dat station aanwezig */
            /* nu de cic file proberen (b.v. 2003042121.CIC) */
            Bepaal_CIC_CIC_filenaam();
            Read_CIC_CIC(stations[i]);                       /* via deze function wordt: Write_Temp_File(); aangeroepen */
         } /* if ((temp = fopen(tempfilenaam, "w")) == NULL) */
      } /* if (strcmp(stations[i], "\0") != 0) */
   } /* for (i = 0; i < NUM_STATIONS; i++) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                            Bepaal_CIC_LFR_filenaam                        */
/*                                                                           */
/*****************************************************************************/
int Bepaal_CIC_LFR_filenaam()
{
   /* NB inputfilenaam b.v.: 2007012515.LFR */


   /* initialisatie */
   strcpy(lfr_inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(lfr_inputfilenaam, "input_cic\\");
   else                                                /* UNIX */
      strcpy(lfr_inputfilenaam, "input_cic/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(lfr_inputfilenaam, JJJJMMDDUU);
   strcat(lfr_inputfilenaam, ".LFR");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                            Bepaal_CIC_CIC_filenaam                        */
/*                                                                           */
/*****************************************************************************/
int Bepaal_CIC_CIC_filenaam()
{
   /* NB inputfilenaam b.v.: 2003042121.CIC */


   /* initialisatie */
   strcpy(cic_inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(cic_inputfilenaam, "input_cic\\");
   else                                                /* UNIX */
      strcpy(cic_inputfilenaam, "input_cic/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(cic_inputfilenaam, JJJJMMDDUU);
   strcat(cic_inputfilenaam, ".CIC");


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                            Bepaal_CIC_LDS_filenaam                        */
/*                                                                           */
/*****************************************************************************/
int Bepaal_CIC_LDS_filenaam()
{
   /* inputfilenaam b.v.: WAVE_LDS_200108060600_00000_LC */


   /* initialisatie */
   strcpy(inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(inputfilenaam, "input_cic\\");
   else                                                /* UNIX */
      strcpy(inputfilenaam, "input_cic/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */


   strcat(inputfilenaam, "WAVE_LDS_");
   strcat(inputfilenaam, JJJJMMDDUU);
   strcat(inputfilenaam, "00_00000_LC");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                             read_CIC_LFR                                  */
/*                                                                           */
/*****************************************************************************/
int Read_CIC_LFR(const char* station)
{
   FILE* in;

   char volledig_path[512];
   char info[1024];
   char line[1024];
   char line_station[4];
   char hulp_Hmax[6];
   char hulp_H_1_50[6];
   char hulp_aantal_golven[4];
   char hulp_T_Hmax[6];

   int pos;
   int num_hulp_Hmax;
   int num_hulp_H_1_50;
   int num_hulp_aantal_golven;
   int num_hulp_T_Hmax;


   if ((in = fopen(lfr_inputfilenaam, "r")) == NULL)
   {
      /* dus mislukt */

      getcwd(volledig_path, 512);

      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, volledig_path);

      if (strcmp(OS, "WINDOWS") == 0)
         strcat(info, "\\");
      else
         strcat(info, "/");

      strcat(info, lfr_inputfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((in = fopen(lfr_inputfilenaam, "r")) == NULL) */
   else /* (lfr_inputfile_ok) */
   {
      while (fgets(line, 1024, in) != NULL)                        /* dus gelukt (1024 is willekeurig groot getal) */
      {
         if (strlen(line) >= 845)                                  /* totale lengte lfr record 845 char */
         {
            /* station name in de record */
            pos = 0;
            strncpy(line_station, line + pos, 3);
            line_station[3] = '\0';

            /* NB station = gevraagde station                         */
            /* NB line_station = zoals in de record van deze cic file */
            /* gevraagde station; (NB bij North Cormorant staat NC2 in locations.txt (en doorgegeven vanuit read_CIC_LDS()) en kan NC1 in .lfr file staan */
            if ( (strncmp(line_station, station, 3) == 0) || ((strncmp(station, "NC2", 3) == 0) && (strncmp(line_station, "NC1", 3) == 0))
                                                          || ((strncmp(station, "Q12", 3) == 0) && (strncmp(line_station, "Q11", 3) == 0)) )
            {
               /* aantal golven */
               pos = 630;
               strncpy(hulp_aantal_golven, line + pos, 3);
               hulp_aantal_golven[3] = '\0';
               num_hulp_aantal_golven = atoi(hulp_aantal_golven);

               if ((num_hulp_aantal_golven <= 999) && (num_hulp_aantal_golven != 0))   /* 3 posities dus max = 999 */
               {
                  strncpy(spectrum[AANTAL_GOLVEN_SPECTRUM], line + pos, 3);
                  spectrum[AANTAL_GOLVEN_SPECTRUM][3] = '\0';
               } /* if (num_hulp_H_1_50 < 9900) */

               /* H(1/50) */
               pos = 811;
               strncpy(hulp_H_1_50, line + pos, 5);
               hulp_H_1_50[5] = '\0';
               num_hulp_H_1_50 = atoi(hulp_H_1_50);

               if (num_hulp_H_1_50 < 9900)                            /* 9900 mm = 99 m */
               {
                  strncpy(spectrum[H_1_50_SPECTRUM], line + pos, 5);
                  spectrum[H_1_50_SPECTRUM][5] = '\0';
               } /* if (num_hulp_H_1_50 < 9900) */

               /* Hmax */
               pos = 821;
               strncpy(hulp_Hmax, line + pos, 5);
               hulp_Hmax[5] = '\0';
               num_hulp_Hmax = atoi(hulp_Hmax);

               if (num_hulp_Hmax < 9900)                            /* 9900 mm = 99 m */
               {
                  strncpy(spectrum[HMAX_SPECTRUM], line + pos, 5);
                  spectrum[HMAX_SPECTRUM][5] = '\0';
               } /* if (num_hulp_Hmax < 9900) */

               /* T(Hmax) */
               pos = 838;
               strncpy(hulp_T_Hmax, line + pos, 3);
               hulp_T_Hmax[3] = '\0';
               num_hulp_T_Hmax = atoi(hulp_T_Hmax);

               if (num_hulp_T_Hmax < 999)                            /* 999 .s = 99,9 sec */
               {
                  strncpy(spectrum[T_HMAX_SPECTRUM], line + pos, 3);
                  spectrum[T_HMAX_SPECTRUM][3] = '\0';
               } /* if (num_hulp_T_Hmax < 9900) */

            } /* if ( (strncmp(line_station, station, 3) == 0) || */
         }  /* if (strlen(line) >= 845) */
      } /* while (fgets(line, 1024, in) != NULL) */

      fclose(in);

   } /* else (lfr_inputfile_ok) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                             read_CIC_CIC                                  */
/*                                                                           */
/*****************************************************************************/
int Read_CIC_CIC(const char* station)
{
   FILE* in;
   char volledig_path[512];
   char info[1024];
   char line[1024];
   char line_station[4];
   char flag[3];
   char hulp_hm0_cm[8];
   char hulp_hm0_mm[8];
   char hulp_tm0_1[8];
   char hulp_tm0_1_mm[8];
   char hulp_dd[8];
   char hulp_dd_10graden[8];
   char hulp_ff[8];
   char hulp_ff_knots[8];
   char hulp_lfe_cm_kwadraat[8];
   char he3[8];
   int pos;
   int num_hulp_hm0_mm;
   int num_hulp_tm0_1;
   int num_hulp_dd;
   int num_hulp_ff;
   int num_he3;
   double num_hulp_lfe_mm_kwadraad;


   if ((in = fopen(cic_inputfilenaam, "r")) == NULL)
   {
      /* dus mislukt */

      getcwd(volledig_path, 512);

      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, volledig_path);

      if (strcmp(OS, "WINDOWS") == 0)
         strcat(info, "\\");
      else
         strcat(info, "/");

      strcat(info, cic_inputfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((in = fopen(cic_inputfilenaam, "r")) == NULL) */
   else /* (cic_inputfile_ok) */
   {
      while (fgets(line, 255, in) != NULL)                        /* dus gelukt */
      {
         if (strlen(line) >= 100)
         {
            /* station name in de record */
            pos = 12;
            strncpy(line_station, line + pos, 3);
            line_station[3] = '\0';

            /* NB station = gevraagde station                         */
            /* NB line_station = zoals in de record van deze cic file */
            /* gevraagde station; (NB bij North Cormorant wordt staat NC2 in locations.txt en kan NC1 in .cic file staan */
            if ( (strncmp(line_station, station, 3) == 0) || ((strncmp(station, "NC2", 3) == 0) && (strncmp(line_station, "NC1", 3) == 0))
                                                          || ((strncmp(station, "Q12", 3) == 0) && (strncmp(line_station, "Q11", 3) == 0)) )
            {
               /* Initialiseer_Spectrum_Array() */
               Initialiseer_Spectrum_Array();

               /* flag bepalen bij OK: hm0, tm0-1, dd, ff, e10 nemen; bij flag CO alleen HM0,dd en ff nemen */
               pos = 99;
               strncpy(flag, line + pos, 2);
               flag[2] = '\0';

               /* stations naam (nb dus North Cormorant altijd als NC2) */
               strncpy(spectrum[STATION_SPECTRUM], station, 3);
               spectrum[STATION_SPECTRUM][3] = '\0';

               /* datum-tijd */
               pos = 1;
               strcpy(spectrum[DATUMTIJD_SPECTRUM], "20");            /* jaartal is in de file zelf maar 2 posities */
               strncat(spectrum[DATUMTIJD_SPECTRUM], line + pos, 8);   /* JJ + JJMMDDUU = JJJJMMDDUU */
               spectrum[DATUMTIJD_SPECTRUM][10] = '\0';


               if ( (strncmp(flag, "CO", 2) == 0) || (strncmp(flag, "OK", 2) == 0) )
               {
                  /* HM0 */
                  pos = 59;
                  strncpy(hulp_hm0_cm, line + pos, 7);                      /* in cm */
                  hulp_hm0_cm[7] = '\0';
                  num_hulp_hm0_mm = atoi(hulp_hm0_cm) * 10;                 /* nu in mm */

                  if (num_hulp_hm0_mm <= 99999)                             /* i.v.m. 5 posities in spectrum array */
                  {
                     sprintf(hulp_hm0_mm, "%5d", num_hulp_hm0_mm);
                     strncpy(spectrum[HM0_SPECTRUM], hulp_hm0_mm, 5);       /* 5 posities is zelfde als uit LDS file */
                     spectrum[HM0_SPECTRUM][5] = '\0';
                  }  /* if (num_hulp_hm0_mm <= 99999) */


                  /*
                  // dd (wind)
                  */
                  pos = 35;
                  strncpy(hulp_dd, line + pos, 7);                             /* in 10-tallen graden */
                  hulp_dd[7] = '\0';
                  num_hulp_dd = atoi(hulp_dd);                                 /* blijft in 10-tallen graden */

                  if (num_hulp_dd <= 999)
                  {
                     sprintf(hulp_dd_10graden, "%3d",   num_hulp_dd);
                     strncpy(spectrum[DD_WIND], hulp_dd_10graden, 3);          /* 3 posities is zelfde als uit LDS file */
                     spectrum[DD_WIND][3] = '\0';
                  } /* if (num_hulp_dd <= 999) */


                  /*
                  // ff (wind)
                  */
                  pos = 43;
                  strncpy(hulp_ff, line + pos, 7);                             /* in knopen */
                  hulp_ff[7] = '\0';
                  num_hulp_ff = atoi(hulp_ff);

                  if (num_hulp_ff <= 99)
                  {
                     sprintf(hulp_ff_knots, "%2d", num_hulp_ff);
                     strncpy(spectrum[FF_WIND], hulp_ff_knots, 2);             /* 2 posities is zelfde als uit LDS file */
                     spectrum[FF_WIND][2] = '\0';
                  } /* if (num_hulp_ff <= 99) */

                  /**************  test ************/
                  /*
                  fprintf(stderr, "%s", spectrum[HM0_SPECTRUM]);
                  getchar();
                  */
                  /**************** test ************/

               } /* if ( (strncmp(flag, "CO", 2) == NULL) || (strncmp(flag, "OK", 2) == NULL) ) */

               if (strncmp(flag, "OK", 2) == 0)
               {
                  /*
                  // Tm0-1
                  */
                  pos = 51;
                  strncpy(hulp_tm0_1, line + pos, 7);                          /* in 0.1 sec */
                  hulp_tm0_1[7] = '\0';
                  num_hulp_tm0_1 = atoi(hulp_tm0_1);

                  if (num_hulp_tm0_1 <= 9999)
                  {
                     sprintf(hulp_tm0_1_mm, "%4d", num_hulp_tm0_1);
                     strncpy(spectrum[TM0_1_SPECTRUM], hulp_tm0_1_mm, 4);      /* 4 posities is zelfde als uit LDS file */
                     spectrum[TM0_1_SPECTRUM][4] = '\0';
                  } /* if (num_hulp_tm0_1 <= 9999) */



                  /*
                  // HE3 (HE3 = wortel(LFE) * 4)                                [LFE = E10]
                  */
                  /*                  LET op: in de LDS file staat: HE3 in mm   [HE3 = Hs10]            */
                  /*                          in de .cic file staat LFE in cm** [HE3 = wortel(LFE) * 4] */
                  /*                           1 cm** = 100 mm**                                        */
                  pos = 67;
                  strncpy(hulp_lfe_cm_kwadraat, line + pos, 7);                  /* in cm kwadraad */
                  hulp_lfe_cm_kwadraat[7] = '\0';
                  num_hulp_lfe_mm_kwadraad = atof(hulp_lfe_cm_kwadraat) * 100;   /* nu in mm kwadraad */

                  num_he3 = (int)(sqrt(num_hulp_lfe_mm_kwadraad) * 4 + 0.5);

                  if (num_he3 <= 99999)                                          /* om binnen de 5 characters te blijven */
                  {
                     sprintf(he3, "%5d", num_he3);
                     strncpy(spectrum[HE3_SPECTRUM], he3, 5);
                     spectrum[HE3_SPECTRUM][5] = '\0';
                  } /* if (num_he3 <= 99999) */

               } /* if (strncmp(flag, "OK", 2) == NULL) */

               Write_Temp_File();

            } /* if ( (strncmp(line, station, 3) == NULL) || etc. */
         } /* if (strlen(line) >= 100) */
      } /* while (fgets(line, 255, in) != NULL) */

      fclose(in);
   } /* else (cic_inputfile_ok) */


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                             read_CIC_LDS                                  */
/*                                                                           */
/*****************************************************************************/
int Read_CIC_LDS()
{
   FILE* in;

   char volledig_path[512];
   char info[1024];
   char line1[775];                        /* line1_lengte +1 (dus t/m eol) */
   char line2[429];                        /* line2_lengte +1 (dus t/m eol) */
   char char_status[2];                    /* status van de cic data (3 = foutieve data) */

   const int line1_lengte = 774;
   const int line2_lengte = 428;
   int stoppen;
   int i;
   int pos;
   int Epiek_hulp;                         /* piek energy (hulp om bij ruis geen richting te plotten) */
   int k;


   double num_Czz10_array[AANTAL_CZZ10_ELEMENTEN];  /* BFI etc. */
   double BFI;
   double num_fp;
   double C4;
   double P;
   double num_Hmax;
   double num_Hm0;
   double num_Tm0_1;
   int aantal_freq_array_elementen;                 /* BFI etc. */



   /* nb kijk voor de zekerheid in gps.h */
   /*
   **
   ** array spectrum NUM_SPECTRUM elementen
   ** spectrum[0]                : sensorcode / naam station
   ** spectrum[1]                : datum-tijd
   ** spectrum[2] - [42]         : variantie dichtheid elementen
   ** spectrum[43]               : Hm0
   ** spectrum[44]               : Tm0-1
   ** spectrum[45]               : LFE
   ** spectrum[46]               : ff
   ** spectrum[47]               : dd
   ** spectrum[48] - [88]        : richting elementen
   ** spectrum[89]               : gem. richting totale spectrum
   ** spectrum[]                 : gem. richting LFE
   **
   */



   /* intialisatie */
   for (i = 0; i < AANTAL_CZZ10_ELEMENTEN; i++)
      num_Czz10_array[i] = 0.0;



   if ((in = fopen(inputfilenaam, "r")) == NULL)
   {
      /* dus mislukt */

      getcwd(volledig_path, 512);

      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, volledig_path);

      if (strcmp(OS, "WINDOWS") == 0)
         strcat(info, "\\");
      else
         strcat(info, "/");

      strcat(info, inputfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((in = fopen(inputfilenaam, "r")) == NULL) */
   else  /* inputfile ok */
   {
      /* 7-03-2002 binnen do-while lus gezet */
      /* Initialiseer_Spectrum_Array() */
      /*Initialiseer_Spectrum_Array();*/

      stoppen = 0;
      do
      {
         /* Initialiseer_Spectrum_Array() */
         Initialiseer_Spectrum_Array();

         line1[0] = '\0';
         line2[0] = '\0';

         /* in de LDS files zijn twee regels per station gegeven */
         /*if (fgets(line1, line1_lengte +1, in) != NULL)*/
         if (fread(line1, line1_lengte +1, 1, in) != 0)
         /*if (fread(line1, sizeof(line1_lengte), 1, in) != NULL)*/ /* gaat niet goed onder UNIX */
         /*if (fread(line1, strlen(line1) +1, 1, in) != NULL)*/     /* gaat niet goed omdat lengte hier nog maar 1 is */
         {
               /**************  test ************/
               /*
               fprintf(stderr, "%s", line1);
               getchar();
               */
               /**************** test ************/


            /*if (fgets(line2, line2_lengte +1, in) != NULL)*/
            if (fread(line2, line2_lengte +1, 1, in) != 0)
            /*if (fread(line2, sizeof(line2_lengte), 1, in) != NULL)*/ /* gaat niet goed onder UNIX */
            {
               /* beide regels zijn dus in deze fase goed ingelezen */

               /* ophalen status cijfer */
               pos = 19;
               strncpy(char_status, line1 + pos, 1);
               char_status[1] = '\0';

               /* alleen data opslaan als status op OK (dus geen '3') staat */
               if (atoi(char_status) != 3)                               /* status = 3 = foutieve/verdachte data */
               {
                  /* stations naam (nb. NC1 standaard omzetten naar NC2 naam) dit voor uniformiteit, heeft te maken met nc heeft maar 2 char en wordt overal van 3 char voor station uitgegaan) */
                  if (strncmp(line1, "NC1", 3) == 0)
                  {
                     strncpy(spectrum[STATION_SPECTRUM], "NC2", 3);
                     spectrum[STATION_SPECTRUM][3] = '\0';
                  }
                  /* stations naam (nb. Q11 standaard omzetten naar Q12 naam) dit voor uniformiteit, heeft te maken met nc heeft maar 2 char en wordt overal van 3 char voor station uitgegaan) */
                  else if (strncmp(line1, "Q11", 3) == 0)
                  {
                     strncpy(spectrum[STATION_SPECTRUM], "Q12", 3);
                     spectrum[STATION_SPECTRUM][3] = '\0';
                  }
                  else
                  {
                     strncpy(spectrum[STATION_SPECTRUM], line1, 3);
                     spectrum[STATION_SPECTRUM][3] = '\0';
                  }

                  /* datum-tijd */
                  strcpy(spectrum[DATUMTIJD_SPECTRUM], "20");            /* jaartal is in de file zelf maar 2 posities */
                  strncat(spectrum[DATUMTIJD_SPECTRUM], line1 + 5, 8);   /* JJ + JJMMDDUU = JJJJMMDDUU */
                  /* n.b. spectrum[1][10] = '\0'; doet strncat al */


                  /* LDS files: spectrum elementen 88-102 (10mHz: 0.0 - 0.14) */
                  /* eerste begint op positie 640 (C positie 639) */
                  pos = 639;
                  for (i = BEGIN_SPECTRUM_ENERGY; i < TUSSEN_SPECTRUM_ENERGY; i++)                  /* 15 waarden */
                  {
                     strncpy(spectrum[i], line1 + pos, 9);
                     spectrum[i][9] = '\0';
                     pos += 9;                              /* volgende 9 posities verder in record */
                  }

                  /* LDS files: spectrum elementen 39-74 (10mHz: 0.15 - 0.4) */ /* n.b. dus stukje van 0.4 - 0.5 wordt NIET meegenomen */
                  /* eerste begint op positie 333 (C positie 332) */
                  pos = 332;
                  for (i = TUSSEN_SPECTRUM_ENERGY; i <= END_SPECTRUM_ENERGY; i++)                   /* 26 waarden */
                  {
                     strncpy(spectrum[i], line1 + pos, 7);
                     spectrum[i][7] = '\0';
                     pos += 7;                              /* volgende 7 posities verder in record */
                  }

                  /* Hm0 */
                  pos = 584;
                  strncpy(spectrum[HM0_SPECTRUM], line1 + pos, 5);
                  spectrum[HM0_SPECTRUM][5] = '\0';

                  /* HE3 ( LFE = (HE3 / 4)**2 ) */                        /* hier HE3 in mm */
                  pos = 604;
                  strncpy(spectrum[HE3_SPECTRUM], line1 + pos, 5);
                  spectrum[HE3_SPECTRUM][5] = '\0';

                  /* Tm0-1 */
                  pos = 617;
                  strncpy(spectrum[TM0_1_SPECTRUM], line1 + pos, 4);
                  spectrum[TM0_1_SPECTRUM][4] = '\0';

                  /* fp */
                  pos = 621;
                  strncpy(spectrum[FP_SPECTRUM], line1 + pos, 4);
                  spectrum[FP_SPECTRUM][4] = '\0';

                  /* Hs7 */
                  pos = 625;
                  strncpy(spectrum[HS7_SPECTRUM], line1 + pos, 5);
                  spectrum[HS7_SPECTRUM][5] = '\0';

                  /* ff (wind) */
                  pos = 16;
                  strncpy(spectrum[FF_WIND], line1 + pos, 2);
                  spectrum[FF_WIND][2] = '\0';

                  /* dd (wind) */
                  pos = 13;
                  strncpy(spectrum[DD_WIND], line1 + pos, 3);
                  spectrum[DD_WIND][3] = '\0';


                  /* LDS files: richting spectrum elementen 108-148 (10mHz: 0.0 - 0.4) */ /* n.b. dus stukje van 0.4 - 0.5 wordt NIET meegenomen */
                  /* eerste begint op positie 21 lINE2 (C positie 20) */
                  pos = 20;
                  for (i = BEGIN_SPECTRUM_RICHTING; i <= END_SPECTRUM_RICHTING; i++)                   /* 41 waarden */
                  {
                     strncpy(spectrum[i], line2 + pos, 4);
                     spectrum[i][4] = '\0';
                     pos += 4;                              /* volgende 4 posities verder in record */
                  }

                  /* dd-gem totale spectrum */
                  pos = 0;
                  strncpy(spectrum[DD_GEM_SPECTRUM], line2 + pos, 4);
                  spectrum[DD_GEM_SPECTRUM][4] = '\0';

                  /* dd-gem LFE spectrum */
                  pos = 8;
                  strncpy(spectrum[DD_GEM_LFE_SPECTRUM], line2 + pos, 4);
                  spectrum[DD_GEM_LFE_SPECTRUM][4] = '\0';


                  /* bij ruis geen richting (-1) opslaan */
                  Epiek_hulp = 0;
                  for (i = BEGIN_SPECTRUM_ENERGY; i <= END_SPECTRUM_ENERGY; i++)
                  {
                     if (atoi(spectrum[i]) > Epiek_hulp)
                        Epiek_hulp = atoi(spectrum[i]);
                  } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= END_SPECTRUM_ENERGY; i++) */

                  for (i = BEGIN_SPECTRUM_ENERGY; i <= END_SPECTRUM_ENERGY; i++)
                  {
                     if (atoi(spectrum[i]) == 0)                  /* 0 apart behandelen om exception te voorkomen */
                     {
                        /*strcpy(spectrum[BEGIN_SPECTRUM_RICHTING + (i - BEGIN_SPECTRUM_ENERGY)], "\0");*/  /* reset */

                        for (k = 0; k < NUM_SPECTRUM_2; k++)
                           spectrum[BEGIN_SPECTRUM_RICHTING + (i - BEGIN_SPECTRUM_ENERGY)][k] = '\0';  /* reset */

                        strcpy(spectrum[BEGIN_SPECTRUM_RICHTING + (i - BEGIN_SPECTRUM_ENERGY)], "-1"); /* richting op 'onbekend' */
                     }
                     else if (Epiek_hulp / atoi(spectrum[i]) > 100)       /* dan aanmerken als ruis */
                     {
                        /*strcpy(spectrum[BEGIN_SPECTRUM_RICHTING + (i - BEGIN_SPECTRUM_ENERGY)], "\0");*/  /* reset */

                        for (k = 0; k < NUM_SPECTRUM_2; k++)
                           spectrum[BEGIN_SPECTRUM_RICHTING + (i - BEGIN_SPECTRUM_ENERGY)][k] = '\0';  /* reset */

                        strcpy(spectrum[BEGIN_SPECTRUM_RICHTING + (i - BEGIN_SPECTRUM_ENERGY)], "-1"); /* richting op 'onbekend' */
                     }
                  } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= END_SPECTRUM_ENERGY; i++) */

                  /* LFR files -> Hmax, T(Hmax), H(1/50), aantal_golven */
                  Bepaal_CIC_LFR_filenaam();
                  Read_CIC_LFR(spectrum[STATION_SPECTRUM]);

                  
                  aantal_freq_array_elementen = AANTAL_CZZ10_ELEMENTEN;   /* voor een generieke aanroep (zo kan zowel cic als nedwam energy array deze function gebruiken) */

                  for (i = BEGIN_SPECTRUM_ENERGY; i <= END_SPECTRUM_ENERGY; i++) /* voor summary regel (kurtosis etc.) */
                  {
                     /* num_Czz10_array beginnen bij 0.0 mHz */
                     num_Czz10_array[i - BEGIN_SPECTRUM_ENERGY] = atof(spectrum[i]) / 100000;    /* van .cm2s -> m2/Hz */
                  }

                  num_fp    = atof(spectrum[FP_SPECTRUM]) / 1000;
                  num_Tm0_1 = atof(spectrum[TM0_1_SPECTRUM]) / 10;                               /* van 0.1 s -> s */
                  num_Hm0   = atof(spectrum[HM0_SPECTRUM]) / 1000;                               /* van mm -> m */
                  num_Hmax  = atof(spectrum[HMAX_SPECTRUM]) / 1000;                              /* van mm -> m */

                  /* berekenen BFI, C4 en logging */
                  Bereken_BFI(num_Czz10_array, aantal_freq_array_elementen, num_fp, num_Tm0_1, &BFI, &C4, &P, inputfilenaam, num_Hm0, spectrum[STATION_SPECTRUM], num_Hmax, num_Hmax / num_Hm0);

                  if (BFI != MAXFLOAT)
                     sprintf(spectrum[BFI_SPECTRUM], "%.2f", BFI);

                  if (C4 != MAXFLOAT)
                     sprintf(spectrum[C4_SPECTRUM], "%.2f", C4);

                  if (P != MAXFLOAT)
                     sprintf(spectrum[P_SPECTRUM], "%.2f", P);


                  /* per station wegschrijven naar temp directory */
                  Write_Temp_File();

               } /* if (atoi(char_status) != 3) */

#if 0
   // 7-03-2002 ; anders als bijv Anasuria een 3 geet worden de anderen ook niet meer ingelezen en naar temp geschreven
               else  /* status bit = 3 -> foutieve data) */
                  stoppen = 1;
#endif
            } /* if (fgets(line2, line2_lengte, in) != NULL) */
            else
               stoppen = 1;
         } /* if (fgets(line1, line1_lengte, in) != NULL) */
         else
            stoppen = 1;

      } while(stoppen == 0);


      fclose(in);
   } /* else (inputfile ok) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                              Write_Temp_File                              */
/*                                                                           */
/*****************************************************************************/
int Write_Temp_File()
{
   FILE* temp;
   int i;
   char volledig_path[512];
   char info[1024];
   char tempfilenaam[256];



   /* bepalen temp filenaam */                         /* b.v. CIC_LDS_K13_20010806.TMP */
   tempfilenaam[0] = '\0';

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(tempfilenaam, "temp_cic\\");
   else                                                /* UNIX */
      strcpy(tempfilenaam, "temp_cic/");

   strcat(tempfilenaam, "CIC_LDS_");
   strcat(tempfilenaam, spectrum[STATION_SPECTRUM]);   /* oud: strcat(tempfilenaam, spectrum[0]); */
   strcat(tempfilenaam, "_");
   strcat(tempfilenaam, spectrum[DATUMTIJD_SPECTRUM]); /* oud: strcat(tempfilenaam, spectrum[1]); */
   strcat(tempfilenaam, ".TMP");


   if ((temp = fopen(tempfilenaam, "w")) == NULL)
   {
      /* dus mislukt */

      getcwd(volledig_path, 512);

      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, volledig_path);

      if (strcmp(OS, "WINDOWS") == 0)
         strcat(info, "\\");
      else
         strcat(info, "/");

      strcat(info, tempfilenaam);
      strcat(info, " niet te schrijven\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((temp = fopen(tempfilenaam, "w")) == NULL) */
   else /* temp file ok */
   {
      for (i = 0; i < NUM_SPECTRUM; i++)
      {
         fwrite(spectrum[i], sizeof(spectrum[i]), 1, temp);         /* gaat goed */ /* record lengte overal het zelfde */
         /*fwrite(spectrum[i], strlen(spectrum[i]) +1, 1, temp);*/  /* gaat ook goed; echter geeft records van verschillende lengte */
         putc('\n', temp);
      } /* for (i = 0; i < NUM_SPECTRUM; i++) */

      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                      Initialiseer_Spectrum_Array                          */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_Spectrum_Array()
{
   int i;

   for (i = 0; i < NUM_SPECTRUM; i++)
      strcpy(spectrum[i], "\0");                      /* NUM_SPECTRUM arrayplaatsen van elk 10 char */


   return 0;
}

