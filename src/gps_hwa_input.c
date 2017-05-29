#if !defined(gps_h)                         /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(WINDOWS)                        /* zie gps.h */
#include <dir.h>                            /* o.a. getcwd() in windows */
#else
#include <unistd.h>                         /* o.a. getcwd() in UNIX */
#endif


/* function prototypes in andere module */
extern int Write_Log(char* bericht);
extern char stations[NUM_STATIONS][NUM_STATIONS_2];  /* NUM_STATIONS arrayplaatsen van elk 4 char */

/* functions in deze module */
int Read_HWA_Input_Files(void);
int Bepaal_HWA_filenaam(void);
int Read_HWA(void);
int Write_HWA_Temp_File(void);
int Initialiseer_HWA_Array(void);

/* externe var's */
extern char JJJJMMDDUU[11];                          /* via argument list */
extern char OS[8];

/* var's globaal binnen deze module */
char inputfilenaam[256];
char hwa_array[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN];



/*****************************************************************************/
/*                                                                           */
/*                          Read_HWA_Input_Files                             */
/*                                                                           */
/*****************************************************************************/
int Read_HWA_Input_Files()
{

   Bepaal_HWA_filenaam();
   Read_HWA();


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                              Bepaal_HWA_filenaam                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_HWA_filenaam()
{
   /* inputfilenaam b.v.: 2004080606.HWA */


   /* initialisatie */
   strcpy(inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(inputfilenaam, "input_cic\\");
   else                                                /* UNIX */
      strcpy(inputfilenaam, "input_cic/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */


   strcat(inputfilenaam, JJJJMMDDUU);
   strcat(inputfilenaam, ".HWA");


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                                 read_HWA                                  */
/*                                                                           */
/*****************************************************************************/
int Read_HWA()
{
   FILE* in;
   char volledig_path[512];
   char info[1024];
   char line[513];
   int pos;
   int s;                                /* teller stations */
   char record_station[4];               /* b.v. NC2 (nummer wordt hiernaar omgezet */
   char record_station_nummer[6];        /* b.v. 63112 */
   char record_datumtijd[11];
   char string_ddd[4];
   char string_ff[3];
   char string_Hwa[5];
   char string_Pw[3];


   if ((in = fopen(inputfilenaam, "r")) == NULL)  /* inputfilenaam gezet in function: Bepaal_HWA_filenaam() */
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
      while ((fgets(line, 512, in) != NULL))                    /* 512 is willekeurig groot getal */
      {
         if (strlen(line) >= 44)                                /* voor de zekerheid (om exception te voorkomen) */
         {
            /*
            // in de hwa files staan de stations met hun nummer dit omzetten naar hun naam
            */


            /* nummer station die staat in ingelezen record bepalen */
            pos = 0;
            strcpy(record_station_nummer, "\0");                /* initialisatie */
            strncpy(record_station_nummer, line + pos, 5);      /* b.v. 63112 etc. */
            record_station_nummer[5] = '\0';

            /* North Cormorant */
            if (strncmp(record_station_nummer, "63112", 5) == 0)
            {
               strcpy(record_station, "\0");                    /* initialisatie */
               strncpy(record_station, "NC2", 3);               /* dus omgezet van nummer naar naam */
               record_station[3] = '\0';
            }
            /* Anasuria */
            else if (strncmp(record_station_nummer, "62164", 5) == 0)
            {
               strcpy(record_station, "\0");                    /* initialisatie */
               strncpy(record_station, "ANA", 3);               /* dus omgezet van nummer naar naam */
               record_station[3] = '\0';
            }
            /* Auk */
            else if (strncmp(record_station_nummer, "62132", 5) == 0)
            {
               strcpy(record_station, "\0");                    /* initialisatie */
               strncpy(record_station, "AUK", 3);               /* dus omgezet van nummer naar naam */
               record_station[3] = '\0';
            }
            else
            {
               /* default */
               strcpy(record_station, "XXX");                   /* tot 02-08-2005: strcpy(record_station, "\0"); */
            } /* else */

            /* station uit HWA file vergelijken met stations lijst (dit zijn de stations die we willen hebben) */
            for (s = 0; s < NUM_STATIONS; s++)
            {
               /* station in record komt overeen met een stations naam uit de file-lijst (reeds ingelezen in stations[] array) */
               if (strncmp(record_station, stations[s], 3) == 0)
               {
                  /* initialisatie */
                  strcpy(string_ddd, "\0");
                  strcpy(string_ff,  "\0");
                  strcpy(string_Hwa, "\0");
                  strcpy(string_Pw,  "\0");
                  strcpy(record_datumtijd, "\0");
                  Initialiseer_HWA_Array();

                  /* ddd */
                  pos = 19;
                  strncpy(string_ddd, line + pos, 3);           /* graden true */
                  string_ddd[3] = '\0';

                  /* ff */
                  pos = 23;
                  strncpy(string_ff, line + pos, 2);            /* m/s */
                  string_ff[2] = '\0';

                  /* Hwa (HM0) */
                  pos = 26;
                  strncpy(string_Hwa, line + pos, 4);           /* meter */
                  string_Hwa[4] = '\0';

                  /* Pw (TM0-1) */
                  pos = 31;                                     /* sec */
                  strncpy(string_Pw, line + pos, 2);
                  string_Pw[2] = '\0';

                  /* datum tijd */
                  pos = 34;                                      /* de eerste pos is een blanco */
                  strncpy(record_datumtijd, line + pos, 10);
                  record_datumtijd[10] = '\0';

                  /*
                  // wegschrijven
                  */
                  strncpy(hwa_array[0], record_station, LENGTE_HWA_ELEMENTEN);     /* 3 ? */
                  strncpy(hwa_array[1], record_datumtijd, LENGTE_HWA_ELEMENTEN);
                  strncpy(hwa_array[2], string_ddd, LENGTE_HWA_ELEMENTEN);         /* 3 ? */
                  strncpy(hwa_array[3], string_ff, LENGTE_HWA_ELEMENTEN);          /* 2 ? */
                  strncpy(hwa_array[4], string_Hwa, LENGTE_HWA_ELEMENTEN);         /* 4 ? */
                  strncpy(hwa_array[5], string_Pw, LENGTE_HWA_ELEMENTEN);          /* 2 ? */

                  Write_HWA_Temp_File();

                  break;  /* springen uit: "for (m = 0; m < NUM_STATIONS; m++)" */

               } /* if (strncmp(local_station, stations[s], 3) == NULL) */
            } /* for (s = 0; s < NUM_STATIONS; s++) */
         } /* (strlen(line1) >= 44)  */
      } /* while ((fgets(line1, 512, in) != NULL)) */

      fclose(in);
   } /* else (inputfile ok) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                           Initialiseer_HWA_Array                          */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_HWA_Array()
{
   int i;


   /* initialisatie */
   for (i = 0; i < AANTAL_HWA_ELEMENTEN; i++)
      strcpy(hwa_array[i], "\0");


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                           Write_HWA_Temp_File                             */
/*                                                                           */
/*****************************************************************************/
int Write_HWA_Temp_File()
{
   FILE* temp;
   int i;
   char volledig_path[512];
   char info[1024];
   char tempfilenaam[256];


   /* bepalen temp filenaam */                         /* b.v. HWA_K13_2004080612.TMP */
   tempfilenaam[0] = '\0';

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(tempfilenaam, "temp_cic\\");
   else                                                /* UNIX */
      strcpy(tempfilenaam, "temp_cic/");

   strcat(tempfilenaam, "CIC_HWA_");
   strcat(tempfilenaam, hwa_array[0]);                 /* b.v. K13 */
   strcat(tempfilenaam, "_");
   strcat(tempfilenaam, hwa_array[1]);                 /* b.v. 2004080612 */
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
      for (i = 0; i < AANTAL_HWA_ELEMENTEN; i++)
      {
         fwrite(hwa_array[i], strlen(hwa_array[i]), 1, temp);
         putc('\n', temp);
      } /* for (i = 0; i < AANTAL_HWA_ELEMENTEN; i++) */

      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}




