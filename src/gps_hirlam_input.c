#if !defined(gps_h)                         /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <values.h>                         /* MAXFLOAT */

#if defined(WINDOWS)                        /* zie gps.h */
#include <dir.h>                            /* o.a. getcwd() in windows */
#else
#include <unistd.h>                         /* o.a. getcwd() in UNIX */
#endif


/* function prototypes in andere module */
extern int Write_Log(char* bericht);        /* gpsmain.c */

/* functions prototypes in deze module */
int Bepaal_Hirlam_filenaam(void);
int Verwerk_Hirlam(void);
int Convert_hirlam_names_to_gps_station(const char* local_name, char* local_station);
int Bepaal_Hirlam_tempfilenaam(const char* run_time, const char* local_station, char* hirlam_tempfilenaam, const char* forecast);
int Write_Hirlam_tempfile(const char* hirlam_tempfilenaam, const char* run_time, const char* forecast, const char* valid_time, const char* ddd, const char* ff);
int Allocate_HIRLAM_Arrays(void);
int Initialiseer_HIRLAM_Arrays(void);
int De_Allocate_HIRLAM_Arrays(void);
int Bepaal_Filenaam_Hirlam_Temp_File(const char* station, char* hirlam_tempfilenaam);
int Read_Temp_File_In_Hirlam_Arrays(const char* hirlam_tempfilenaam);

/* var's in deze module */
char hirlam_inputfilenaam[256];
double* DDD_hirlam;                                 /* hirlam wind-richtingen */
double* FF_hirlam;                                  /* hirlam wind-snelheden */

/* externe var's */
extern char JJJJMMDDUU[11];                         /* gpsmain.c; via argument list */
extern char OS[8];                                  /* gpsmain.c */
extern char stations[NUM_STATIONS][NUM_STATIONS_2]; /* gpsmain.c; NUM_STATIONS arrayplaatsen van elk 4 char */
extern int NUM_VERWACHTINGEN;                       /* gps_nedwam_input.c */
extern int INT_VERWACHTINGEN;                       /* gps_nedwam_input.c (interval uren tussen verwachtingen b.v. 3) */


/*****************************************************************************/
/*                                                                           */
/*                            Bepaal_Hirlam_filenaam                         */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Hirlam_filenaam()
{
   /* hirlam_inputfilenaam b.v.: hirver2001080606 */


   /* initialisatie */
   strcpy(hirlam_inputfilenaam, "\0");

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(hirlam_inputfilenaam, "input_hirlam\\");
   else                                                /* UNIX */
      strcpy(hirlam_inputfilenaam, "input_hirlam/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(hirlam_inputfilenaam, "hirver");
   strcat(hirlam_inputfilenaam, JJJJMMDDUU);


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                              Verwerk_Hirlam                               */
/*                                                                           */
/*****************************************************************************/
int Verwerk_Hirlam()
{
   FILE* in;
   char volledig_path[512];
   char info[1024];
   char line1[256];                        /* 256: een getal groter dan lengte langste regel */
   /*const int line1_lengte = 52; */          /* minimum om ook nog de ff er uit te kunnen halen */
   /*int stoppen;*/                           /* stoppen omdat alle hirlam data is ingelezen */
   int stoppen_blok;                      /* stoppen omdat een hirlam forecast blok is ingelezen */
   int i;
   int pos;
   char local_station[4];                 /* de hirlam naam (b.v. Vlieland) omgezet naar programma naam (b.v. ELD) */
   char char_twintig[21];                 /* eerste 20 char van een ingelezen regel */
   char run_time[11];                     /* run time in meta regel */
   char forecast[3];                      /* forecast uren in meta regel */
   char valid_time[11];                   /* valid at tiem in meta regel */
   char local_name[11];                   /* 'stations' naam zoals in hirlam file genoemd (b.v. Vlieland) */
   char ddd[4];                           /* wind richting voor een station */
   char ff[6];                            /* wind snelheid voor een station */
   char hirlam_tempfilenaam[256];


   if ((in = fopen(hirlam_inputfilenaam, "r")) == NULL)
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

      strcat(info, hirlam_inputfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((in = fopen(hirlam_inputfilenaam, "r")) == NULL) */
   else /* file open ok */
   {
      /*stoppen = 0;*/
      /*do*/ /* while(stoppen == 0) */
      while (fgets(line1, 255, in) != NULL)  /* fgets: stops reading when it reads either n - 1 characters or a newline character */
      {
         /* NB 255 is gewoon een voldoende groot getal (moet groeter zijn dan de actuele regellengte) */

         /* regel is dus goed ingelezen */

         /*line1[line1_lengte + 1] = '\0';*/

         /* eerste 20 char (om te testen op meta line) */
         strncpy(char_twintig, line1, 20);
         char_twintig[20] = '\0';

         if (strncmp(char_twintig, "HIRLAM FORECAST DATA", 20) == 0) /* de meta regel dus */
         {
            /* run_time */
            pos = 26;
            strncpy(run_time, line1 + pos, 10);                 /* b.v. 2001111918 */
            run_time[10] = '\0';

            /* forecast */
            pos = 38;
            strncpy(forecast, line1 + pos, 2);                  /* b.v. 12 */
            forecast[2] = '\0';

            /* valid */
            pos = 50;
            strncpy(valid_time, line1 + pos, 10);               /* b.v. 2001112006 */
            valid_time[10] = '\0';

            stoppen_blok = 0;
            do /* while(stoppen_blok == 0) */
            {
               line1[0] = '\0';
               /*if (fread(line1, line1_lengte + 1, 1, in) != NULL)*/ /* stations regel data */
               /*if (fgets(line1, line1_lengte +1, in) != NULL)*/
               if (fgets(line1, 255, in) != NULL)                  /* geeft NULL bij error or EOF */
               {
                  if (strlen(line1) > 52)                          /* in ieder geval t/m ff */
                  {
                     /* 'hirlam' stations naam */
                     pos = 7;
                     strncpy(local_name, line1 + pos, 10);         /* b.v. Vlieland */
                     local_name[10] = '\0';

                     /* local_station = naam station zoals in dit programma in gebruik (b.v. ELD) */
                     /* local_name = zoals in hirlam lijst voorkomt (b.v. Vlieland) */
                     Convert_hirlam_names_to_gps_station(local_name, local_station);

                     /* local station b.v. AUK */
                     if (strncmp(local_station, "unk", 3) != 0) /* geen unk (van unknown) */
                     {
                        for (i = 0; i < NUM_STATIONS; i++)
                        {
                           /* testen of gelezen record (met local_station) = gevraagd station (= stations[i]) */
                           if (strncmp(local_station, stations[i], 3) == 0)
                           {
                              /* ddd */
                              pos = 43;
                              strncpy(ddd, line1 + pos, 3);         /* b.v. __3 */
                              ddd[3] = '\0';

                              /* ff */
                              pos = 48;
                              strncpy(ff, line1 + pos, 5);          /* b.v. __1.4 */
                              ff[5] = '\0';

                              /* schrijven naar temp file */
                              Bepaal_Hirlam_tempfilenaam(run_time, local_station, hirlam_tempfilenaam, forecast);
                              Write_Hirlam_tempfile(hirlam_tempfilenaam, run_time, forecast, valid_time, ddd, ff);

                           } /* if (strncmp(local_station, stations[i], 3) == NULL) */
                        } /* for (i = 0; i < NUM_STATIONS; i++) */
                     } /* if (strncmp(local_name, "unk", 3) != NULL) */
                  } /* if (strlen(line1) > 52) */
                  else
                     stoppen_blok = 1;
               } /* if (fgets(line1, 1000, in) != NULL) */
               else /* error or EOF */
               {
                  stoppen_blok = 1;
               } /* else (error or EOF) */
            } while(stoppen_blok == 0);
         } /* if (strncmp(char_twintig, "HIRLAM FORECAST DATA", 20) == NULL) */
      } /*while(stoppen == 0);*/

      fclose(in);
   } /* else (file open ok) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                  Convert_hirlam_names_to_gps_station                      */
/*                                                                           */
/*****************************************************************************/
int Convert_hirlam_names_to_gps_station(const char* local_name, char* local_station)
{

   if (strncmp(local_name, "Auk       ", 10) == 0)
      strcpy(local_station, "AUK");
   else if (strncmp(local_name, "K13       ", 10) == 0)
      strcpy(local_station, "K13");
   else if (strncmp(local_name, "IJmuiden  ", 10) == 0)
      strcpy(local_station, "MUN");
   else if (strncmp(local_name, "Euro      ", 10) == 0)
      strcpy(local_station, "EPL");

   /*else if (strncmp(local_name, "Vlieland  ", 10) == 0)*/
   else if (strncmp(local_name, "Eierl. Gat", 10) == 0)
      strcpy(local_station, "ELD");

   /*else if (strncmp(local_name, "Huibertsgt", 10) == 0)*/
   else if (strncmp(local_name, "Schier.oog", 10) == 0)
      strcpy(local_station, "SMN");

   else if (strncmp(local_name, "Anasurice ", 10) == 0)
      strcpy(local_station, "ANA");
   else if (strncmp(local_name, "North Corm", 10) == 0)
      strcpy(local_station, "NC2");


   else
      strcpy(local_station, "unk");


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                       Bepaal_Hirlam_tempfilenaam                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Hirlam_tempfilenaam(const char* run_time, const char* local_station, char* hirlam_tempfilenaam, const char* forecast)
{
   FILE* temp;

   /* temp file b.v.: HIRLAM_SMN_2001111218.TMP */

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(hirlam_tempfilenaam, "temp_hirlam\\");
   else                                                /* UNIX */
      strcpy(hirlam_tempfilenaam, "temp_hirlam/");

   strcat(hirlam_tempfilenaam, "HIRLAM_");
   strcat(hirlam_tempfilenaam, local_station);
   strcat(hirlam_tempfilenaam, "_");
   strcat(hirlam_tempfilenaam, run_time);
   strcat(hirlam_tempfilenaam, ".TMP");


   /* omdat hij verder in de append mode benaderd wordt, kan het zijn dan hij steeds aanvuld */
   /* daarom open en weer sluiten bij een nieuwe serie (forecast = 00) */
   if (strncmp(forecast, "00", 2) == 0)
   {
      if ((temp = fopen(hirlam_tempfilenaam, "w")) != NULL) /* reset */
        fclose(temp);
   } /* if (strncmp(forecast, "00", 2) == NULL) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                         Write_Hirlam_tempfile                             */
/*                                                                           */
/*****************************************************************************/
int Write_Hirlam_tempfile(const char* hirlam_tempfilenaam, const char* run_time, const char* forecast,
                          const char* valid_time, const char* ddd, const char* ff)

{
   FILE* temp;
   char volledig_path[512];
   char info[512];


   if ((temp = fopen(hirlam_tempfilenaam, "a")) == NULL)
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

      strcat(info, hirlam_tempfilenaam);
      strcat(info, " niet te schrijven\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((temp = fopen(hirlam_tempfilenaam, "w")) == NULL) */
   else /* temp file ok */
   {
      /* NB onderstaande sizeof geeft de grootte van een pointer (op PC 4 bytes) */
      /*fwrite(TSA_spectrum[k][m][v], sizeof(TSA_spectrum[k][m][v]), 1, temp);*/

      /* run-time schrijven */
      fwrite(run_time, strlen(run_time), 1, temp);
      putc(' ', temp);

      /* forecast schrijven */
      fwrite(forecast, strlen(forecast), 1, temp);
      putc(' ', temp);

      /* valid at-time schrijven */
      fwrite(valid_time, strlen(valid_time), 1, temp);
      putc(' ', temp);

      /* ddd schrijven */
      fwrite(ddd, strlen(ddd), 1, temp);
      putc(' ', temp);

      /* ff schrijven */
      fwrite(ff, strlen(ff), 1, temp);
      putc('\n', temp);


      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                        Allocate_HIRLAM_Arrays                             */
/*                                                                           */
/*****************************************************************************/
int Allocate_HIRLAM_Arrays()
{

   /*
   //
   // alloceren van geheugenruimte 1-dimensionale hirlam-arrays
   //
   */
   DDD_hirlam = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double));    /* cleared to 0 */
   FF_hirlam  = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double));    /* cleared to 0 */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                       Initialiseer_HIRLAM_Arrays                          */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_HIRLAM_Arrays()
{
   int v;                      /* verwachtingen teller */


   /*
   //
   // initialiseren 1-dimensionale hirlam-arrays
   //
   */
   for (v = 0; v < NUM_VERWACHTINGEN; v++)
   {
      DDD_hirlam[v] = MAXFLOAT;
      FF_hirlam[v]  = MAXFLOAT;
   } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                       De_Allocate_HIRLAM_Arrays                           */
/*                                                                           */
/*****************************************************************************/
int De_Allocate_HIRLAM_Arrays()
{

   /*
   //
   // de-alloceren van geheugenruimte 1 dimensionale hirlam-arrays
   //
   */
   free(DDD_hirlam);
   free(FF_hirlam);


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                    Bepaal_Filenaam_Hirlam_Temp_File                       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenaam_Hirlam_Temp_File(const char* station, char* hirlam_tempfilenaam)
{
   /* temp filenaam bv.: HIRLAM_MUN_JJJJMMDDUU.TMP */

   /* initialisatie */
   strcpy(hirlam_tempfilenaam, "\0");

   /* eerste deel tempfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(hirlam_tempfilenaam, "temp_hirlam\\");
   else                                                /* UNIX */
      strcpy(hirlam_tempfilenaam, "temp_hirlam/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   /* file naam hirlam temp file */
   strcat(hirlam_tempfilenaam, "HIRLAM_");
   strcat(hirlam_tempfilenaam, station);                  /* bv: MUN */
   strcat(hirlam_tempfilenaam, "_");
   strcat(hirlam_tempfilenaam, JJJJMMDDUU);
   strcat(hirlam_tempfilenaam, ".TMP");


   return 0;
}

/*****************************************************************************/
/*                                                                           */
/*                    Read_Temp_File_In_Hirlam_Arrays                        */
/*                                                                           */
/*****************************************************************************/
int Read_Temp_File_In_Hirlam_Arrays(const char* hirlam_tempfilenaam)
{
   FILE* temp;
   char line1[256];
   char forecast[3];
   char ddd[4];
   char ff[6];
   int teller;
   int pos;                         /* pointer binnen de ingelezen regel */


   /* NB ze zijn in temp file geschreven m.b.v. strlen (geen eol) en niet met sizeof (zie function: Write_TSA_Temp_File())*/

   /* lay-out temp file
   //
   //  2001112118 00 2001112118 299  10.8
   //  2001112118 03 2001112121 302  12.1
   //  2001112118 06 2001112200 283  13.8
   //  etc.
   //
   //  respectievelijk: run-time - forecast hours - valid at- ddd (degr) - ff (m/s)
   //
   */



   if ((temp = fopen(hirlam_tempfilenaam, "r")) != NULL)    /* dus gelukt */
   {

      /* inlezen temp file */

      teller = 0;
      while (fgets(line1, 255, temp) != NULL)  /* fgets: stops reading when it reads either n - 1 characters or a newline character */
      {
         if (strlen(line1) >= 34)                           /* vaste lengte */
         {
            /* forecast */
            pos = 11;
            strncpy(forecast, line1 + pos, 2);              /* b.v. 12 */
            forecast[2] = '\0';

            /* INT_VERWACHTINGEN = interval uren tussen verwachtingen in deze is bepaald uit NEDWAM (TSA) file */
            /* dus voor de HIRLAM data ook alleen deze verwachtingen ophalen */

            if (atoi(forecast) % INT_VERWACHTINGEN == 0)    /* b.v 6 / 3 = 0 */
            {
               /* ddd */
               pos = 25;
               strncpy(ddd, line1 + pos, 3);                /* b.v. __3 */
               ddd[3] = '\0';

               if (teller < NUM_VERWACHTINGEN)              /* voor de zekerheid, niet buiten array-grenzen komen */
                  DDD_hirlam[teller] = atof(ddd);

               /* ff */
               pos = 29;
               strncpy(ff, line1 + pos, 5);                 /* b.v. __1.4 */
               ff[5] = '\0';

               if (teller < NUM_VERWACHTINGEN)              /* niet buiten array-grenzen komen */
                  FF_hirlam[teller] = atof(ff) * M_S_TO_KNOTS; /* dus nu in knopen */

               teller++;

            } /* if (atoi(forcast) % INT_VERWACHTINGEN == 0) */
         } /* if (strlen(line1) == 34) */
      } /* while (fgets(line1, 255, in) != NULL) */

      fclose(temp);

   } /* if ((temp = fopen(hirlam_tempfilenaam, "r")) != NULL) */

   
   return 0;
}



