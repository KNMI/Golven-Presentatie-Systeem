#if !defined(gps_h)                        /* Sentry use file only if it's not already included. */
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
int Read_DSP_Input_Files(void);
int Bepaal_CIC_DSP_filenaam(void);
int Read_CIC_DSP(void);
int Write_DSP_Temp_File(const char* record_station, const char* record_datumtijd);
int Initialiseer_DSP_Spectrum_Array(void);
int Record_naar_DSP_spectrum(const int m, const char* line1);

/* externe var's */
extern char JJJJMMDDUU[11];                        /* via argument list */
extern char OS[8];

/* var's globaal binnen deze module */
char inputfilenaam[256];
char DSP_spectrum[NUM_DSP_FREQUENTIES][NUM_DSP_RICHTINGEN][NUM_DSP_REALS];
/*..double dsp_freq[NUM_DSP_FREQUENTIES];*/


/*****************************************************************************/
/*                                                                           */
/*                          Read_DSP_Input_Files                             */
/*                                                                           */
/*****************************************************************************/
int Read_DSP_Input_Files()
{
   Bepaal_CIC_DSP_filenaam();
   Read_CIC_DSP();


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                            Bepaal_CIC_DSP_filenaam                        */
/*                                                                           */
/*****************************************************************************/
int Bepaal_CIC_DSP_filenaam()
{
   /* inputfilenaam b.v.: WAVE_DSP_200108060600_00000_LC */


   /* initialisatie */
   strcpy(inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(inputfilenaam, "input_cic\\");
   else                                                /* UNIX */
      strcpy(inputfilenaam, "input_cic/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */


   strcat(inputfilenaam, "WAVE_DSP_");
   strcat(inputfilenaam, JJJJMMDDUU);
   strcat(inputfilenaam, "00_00000_LC");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                             read_CIC_DSP                                  */
/*                                                                           */
/*****************************************************************************/
int Read_CIC_DSP()
{
   FILE* in;
   char volledig_path[512];
   char info[1024];
   int stoppen;
   char line1[513];
   int m;                                /* teller frequenties */
   int pos;
   int s;                                /* teller stations */
   char record_station[4];
   char record_datumtijd[9];




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
      stoppen = 0;
      while ( (fgets(line1, 512, in) != NULL) && (stoppen == 0) ) /* lengte record 371 char, 512 is willekeurig groot getal */
      {
         /* per stations blok verder behandelen */
         if (strlen(line1) == 14)                                 /* aanduiding nieuw blok b.v. ANA1 02041512 */
         {
            /* station naam in record */
            strncpy(record_station, line1, 3);
            record_station[3] = '\0';


            for (s = 0; s < NUM_STATIONS; s++)
            {
               /* station in record komt overeen met een stations naam uit de file-lijst (reeds ingelezen in stations[] array) */
               if (strncmp(record_station, stations[s], 3) == 0)
               {
                  /* datum tijd */
                  pos = 5;
                  strncpy(record_datumtijd, line1 + pos, 8);
                  record_datumtijd[8] = '\0';

                  /* initialisatie */
                  Initialiseer_DSP_Spectrum_Array();

                  for (m = 0; m < NUM_DSP_FREQUENTIES; m++)                    /* 48 records (frequenties per locatie) */
                  {
                     if (fgets(line1, 512, in) == NULL)       /* iets mis */
                     {
                        Initialiseer_DSP_Spectrum_Array();
                        stoppen = 1;
                        break;                                /* springen uit "for (i = 0; i < 48; i++)" */
                     }
                     else /* ok */
                     {
                        /* in DSP spectrum array zetten */
                        Record_naar_DSP_spectrum(m, line1);   /* line1 bevat 1 frequentie aanduidinging en 36 richtingen */

/**************  test ************/
/*
               fprintf(stderr, "%s", line1);
               getchar();
*/
/**************** test ************/

                     }
                  } /* for (m = 0; m < NUM_DSP_FREQUENTIES; m++) */

                  Write_DSP_Temp_File(record_station, record_datumtijd);

                  break;  /* springen uit: "for (m = 0; m < NUM_STATIONS; m++)" */

               } /* if (strncmp(local_station, stations[s], 3) == NULL) */
            } /* for (s = 0; s < NUM_STATIONS; s++) */
         } /* if (strlen(line1 >= 14) */
      } /* while ( (fgets(line1, 512, in) != NULL) && (stoppen == 0) ) */

      fclose(in);
   } /* else (inputfile ok) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                         Record_naar_DSP_spectrum                          */
/*                                                                           */
/*****************************************************************************/
int Record_naar_DSP_spectrum(const int m, const char* line1)
{
   /* nb: m = teller frequenties */
   char record_energy[11];
   int k;                                           /* teller richtingen */
   int pos;
   /*char buffer[10];*/


   if (strlen(line1) == 371)                        /* altijd vaste lengte */
   {
      for (k = 10; k < 371; k += 10)                /* op pos 0 staat de frequentie aanduiding vanaf pos 10 beginnen de richtingen */
      {
         pos = k;
         strncpy(record_energy, line1 + pos, 10);
         record_energy[10] = '\0';
         strcpy(DSP_spectrum[m][(k / 10) - 1], record_energy);
      } /* for (k = 10; k < 371; k += 10) */
   } /* if (strlen(line1) == 371) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                          Write_DSP_Temp_File                              */
/*                                                                           */
/*****************************************************************************/
int Write_DSP_Temp_File(const char* record_station, const char* record_datumtijd)
{
   FILE* temp;
   int m;                                              /* teller frequenties */
   int k;                                              /* teller richtingen */
   char volledig_path[512];
   char info[1024];
   char tempfilenaam[256];



   /* bepalen temp filenaam */                         /* b.v. CIC_DSP_K13_20010806.TMP */
   tempfilenaam[0] = '\0';

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(tempfilenaam, "temp_cic\\");
   else                                                /* UNIX */
      strcpy(tempfilenaam, "temp_cic/");

   strcat(tempfilenaam, "CIC_DSP_");
   strcat(tempfilenaam, record_station);
   strcat(tempfilenaam, "_");
   strcat(tempfilenaam, "20");
   strcat(tempfilenaam, record_datumtijd);
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
      for (m = 0; m < NUM_DSP_FREQUENTIES; m++)
      {
         for (k = 0; k < NUM_DSP_RICHTINGEN; k++)
         {
            fwrite(DSP_spectrum[m][k], sizeof(DSP_spectrum[m][k]), 1, temp);    /* gaat goed */ /* record lengte overal het zelfde */
            /*fwrite(spectrum[i], strlen(spectrum[i]) +1, 1, temp);*/  /* gaat ook goed; echter geeft records van verschillende lengte */
            putc('\n', temp);
         } /* for (k = 0; k < NUM_DSP_RICHTINGEN; k++) */
      } /*  for (m = 0; m < NUM_DSP_FREQUENTIES; m++) */

      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                    Initialiseer_DSP_spectrum_Array                        */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_DSP_Spectrum_Array()
{
   int m;
   int k;


   for (m = 0; m < NUM_DSP_FREQUENTIES; m++)
      for (k = 0; k < NUM_DSP_RICHTINGEN; k++)
         strcpy(DSP_spectrum[m][k], "\0");


   return 0;
}
