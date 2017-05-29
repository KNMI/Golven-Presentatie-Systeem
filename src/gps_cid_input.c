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
extern char stations[NUM_STATIONS][NUM_STATIONS_2];       /* NUM_STATIONS arrayplaatsen van elk 4 char */

/* functions in deze module */
int Read_CID_Input_Files(void);
int Bepaal_CID_filenaam(void);
int Read_CID(void);
int Write_CID_Temp_File(void);
int Initialiseer_CID_Array(void);

/* externe var's */
extern char JJJJMMDDUU[11];                        /* via argument list */
extern char OS[8];

/* var's globaal binnen deze module */
char inputfilenaam[256];
char cid_array[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN];



/*****************************************************************************/
/*                                                                           */
/*                          Read_CID_Input_Files                             */
/*                                                                           */
/*****************************************************************************/
int Read_CID_Input_Files()
{

   Bepaal_CID_filenaam();
   Read_CID();


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                              Bepaal_CID_filenaam                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_CID_filenaam()
{
   /* inputfilenaam b.v.: WAVE_CID_200108060600_00000_LC */


   /* initialisatie */
   strcpy(inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(inputfilenaam, "input_cic\\");
   else                                                /* UNIX */
      strcpy(inputfilenaam, "input_cic/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */


   strcat(inputfilenaam, "WAVE_CID_");
   strcat(inputfilenaam, JJJJMMDDUU);
   strcat(inputfilenaam, "00_00000_LC");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                                 read_CID                                  */
/*                                                                           */
/*****************************************************************************/
int Read_CID()
{
   FILE* in;
   char volledig_path[512];
   char info[1024];
   char line[513];
   int pos;
   int s;                                /* teller stations */
   char record_station[4];
   char record_datumtijd[9];
   char string_Hs[8];
   char string_HsSW[8];
   char string_thSW[8];
   char string_TmSW[8];
   char flag[3];
   float num_HsSW;
   float num_TmSW;
   float num_thSW;
   float num_Hs;
   int swell_ok;                          /* 0 = goed; 1 = niet goed */
   float swell_quotient;
   const float swell_toelatings_factor = 0.02;


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
      while ((fgets(line, 512, in) != NULL))                    /* 512 is willekeurig groot getal */
      {
         if (strlen(line) > 124)                                /* voor de zekerheid (om exception te voorkomen) */
         {
            /* initialisatie */
            strcpy(record_station, "\0");

            /* station naam in record */
            pos = 12;
            strncpy(record_station, line + pos, 3);             /* b.v. ELD, NC2 etc. */
            record_station[3] = '\0';

            for (s = 0; s < NUM_STATIONS; s++)
            {
               /* station in record komt overeen met een stations naam uit de file-lijst (reeds ingelezen in stations[] array) */
               if (strncmp(record_station, stations[s], 3) == 0)
               {
                  /* initialisatie */
                  swell_ok = 0;                                 /* goed */
                  strcpy(string_Hs, "\0");
                  strcpy(string_HsSW, "\0");
                  strcpy(string_thSW, "\0");
                  strcpy(string_TmSW, "\0");
                  strcpy(record_datumtijd, "\0");
                  Initialiseer_CID_Array();

                  /* datum tijd */
                  pos = 1;                                      /* de eerste pos is een blanco */
                  strncpy(record_datumtijd, line + pos, 8);
                  record_datumtijd[8] = '\0';

                  /* Hs (HM0) */
                  pos = 59;
                  strncpy(string_Hs, line + pos, 7);
                  string_Hs[7] = '\0';

                  /* HsSW (height swell) */
                  pos = 99;
                  strncpy(string_HsSW, line + pos, 7);
                  string_HsSW[7] = '\0';

                  /* thSW (richting swell) */
                  pos = 107;
                  strncpy(string_thSW, line + pos, 7);
                  string_thSW[7] = '\0';

                  /* TmSW (richting swell) */
                  pos = 115;
                  strncpy(string_TmSW, line + pos, 7);
                  string_TmSW[7] = '\0';

                  /* flag */
                  pos = 123;
                  strncpy(flag, line + pos, 2);
                  flag[2] = '\0';


                  if (strncmp(flag, "OK", 2) == 0)
                  {
                     /*
                     // controles op correcte swell waarden
                     */


                     /* n.b. maximum int op 32bit platform: 2.147.483.648 */

                     /* NB Hs als een hulp nodig voor swell quotient */
                     num_Hs = atof(string_Hs) / 10.0;                /* cm -> m */
                     if ( (num_Hs > 50.0) || (strncmp(string_HsSW, "7654321", 7) == 0) )
                        swell_ok = 1;                                /* niet goed */

                     num_HsSW = atof(string_HsSW) / 10.0;            /* cm -> m */
                     if ( (num_HsSW > 50.0) || (strncmp(string_HsSW, "7654321", 7) == 0) )
                        swell_ok = 1;                                /* niet goed */

                     num_thSW = atof(string_HsSW);                   /* graden */
                     if ( (num_thSW > 360) || (strncmp(string_thSW, "7654321", 7) == 0) )
                        swell_ok = 1;                                /* niet goed */

                     num_TmSW = atof(string_TmSW) / 10.0;            /* 0.1 sec -> 1 sec */
                     if ( (num_TmSW > 50.0) || (strncmp(string_TmSW, "7654321", 7) == 0) )
                        swell_ok = 1;                                /* niet goed */

                     if (swell_ok == 0)
                     {
                        /* swell_quotient = (Hswell / HM0)**2 */
                        swell_quotient = (num_HsSW / num_Hs) * (num_HsSW / num_Hs);
                        if (swell_quotient < swell_toelatings_factor)
                           swell_ok = 1;                             /* niet goed */
                     } /* if (swell_ok == 0) */


                     /* als alles OK dan wegschrijven */
                     if (swell_ok == 0)                              /* dus goed */
                     {
                         strncpy(cid_array[0], record_station, 3);
                         strncpy(cid_array[1], record_datumtijd, 8);
                         strncpy(cid_array[2], string_HsSW, 7);
                         strncpy(cid_array[3], string_thSW, 7);
                         strncpy(cid_array[4], string_TmSW, 7);

                         Write_CID_Temp_File();
                     } /* if (swell_ok == 0)  */

                  } /* if (strncmp(flag, "OK", 2) == NULL) */

                  break;  /* springen uit: "for (m = 0; m < NUM_STATIONS; m++)" */

               } /* if (strncmp(local_station, stations[s], 3) == NULL) */
            } /* for (s = 0; s < NUM_STATIONS; s++) */
         } /* (strlen(line1) > 124)  */
      } /* while ((fgets(line1, 512, in) != NULL)) */

      fclose(in);
   } /* else (inputfile ok) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                           Write_CID_Temp_File                             */
/*                                                                           */
/*****************************************************************************/
int Write_CID_Temp_File()
{
   FILE* temp;
   int i;
   char volledig_path[512];
   char info[1024];
   char tempfilenaam[256];



   /* bepalen temp filenaam */                         /* b.v. CIC_CID_K13_20010806.TMP */
   tempfilenaam[0] = '\0';

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(tempfilenaam, "temp_cic\\");
   else                                                /* UNIX */
      strcpy(tempfilenaam, "temp_cic/");

   strcat(tempfilenaam, "CIC_CID_");
   strcat(tempfilenaam, cid_array[0]);
   strcat(tempfilenaam, "_20");
   strcat(tempfilenaam, cid_array[1]);
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
      for (i = 0; i < AANTAL_SWELL_CID_ELEMENTEN; i++)
      {
         fwrite(cid_array[i], strlen(cid_array[i]), 1, temp);
         putc('\n', temp);
      } /* for (i = 0; i < AANTAL_SWELL_CID_ELEMENTEN; i++) */

      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                           Initialiseer_CID_Array                          */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_CID_Array()
{
   int i;


   /* initialisatie */
   for (i = 0; i < AANTAL_SWELL_CID_ELEMENTEN; i++)
      strcpy(cid_array[i], "\0");


   return 0;
}




 /**************  test ************/
/*
               fprintf(stderr, "%s", line1);
               getchar();
*/
/**************** test ************/

