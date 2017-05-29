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

/* functions in deze module */
int Read_TSA_Input_Files(int* inlees_result);
int Bepaal_TSA_input_filenaam(void);
int Read_TSA(void);
int Write_TSA_Temp_File(const char* TSA_tempfilenaam);
int Initialiseer_TSA_Spectrum_Array(void);
int Convert_nedwam_coordinates_to_station(const char* local_pos, char* local_station);
int Bepaal_TSA_tempfilenaam(const char* line, char* TSA_tempfilenaam);
int Allocate_TSA_Spectrum_Array(void);
int De_Allocate_TSA_Spectrum_Array(void);
int Scan_TSA(int* scan_result);
int Allocate_TSA_Frequenties_Array(void);
int De_Allocate_TSA_Frequenties_Array(void);

/* externe var's */
extern char JJJJMMDDUU[11];                               /* via argument list */
extern char OS[8];

/* var's globaal binnen deze module */
char TSA_inputfilenaam[256];
/*char TSA_spectrum[NUM_RICHTINGEN][NUM_FREQUENTIES][NUM_VERWACHTINGEN][NUM_REALS];*/
char**** TSA_spectrum;
int NUM_RICHTINGEN;
int NUM_FREQUENTIES;
int NUM_VERWACHTINGEN;
int NUM_STATIONS_TSA;
int INT_VERWACHTINGEN;                                    /* interval in uren tussen de verwachtingen (bv. 3 uur) */
/*const int NUM_REALS               = 16;*/
double* FR;                                               /* frequenties_array [NEDWAM benaming] */
double CO;                                                /* verhouding tussen 2 opeenvolgende frequenties [NEDWAM benaming] */


/*****************************************************************************/
/*                                                                           */
/*                     Read_TSA_Input_Files                                  */
/*                                                                           */
/*****************************************************************************/
int Read_TSA_Input_Files(int* inlees_result)
{
   int scan_result;


   Bepaal_TSA_input_filenaam();

   Scan_TSA(&scan_result);                              /* array grenzen en aantal stations bepalen */

   if (scan_result == 1)
   {                                /* ok */
      Read_TSA();
      *inlees_result = 1;           /* ok (dit eventueel via function Read_TSA() teruggeven) */
   }
   else
      *inlees_result = 0;           /* not ok */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                        Bepaal_TSA_filenaam                                */
/*                                                                           */
/*****************************************************************************/
int Bepaal_TSA_input_filenaam()
{
   /* NB: inputfilenaam altijd: TSF_REALS */


   /* initialisatie */
   strcpy(TSA_inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(TSA_inputfilenaam, "input_nedwam\\");
   else                                                /* UNIX */
      strcpy(TSA_inputfilenaam, "input_nedwam/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(TSA_inputfilenaam, "TSF_REALS");

   
   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                             read_TSA                                      */
/*                                                                           */
/*****************************************************************************/
int Read_TSA()
{
   FILE* in;
   const int line_1_meta_lengte    = 9;
   char line_1_meta[10];                         /* line_1_meta_lengte +1 (dus t/m eol) */
   const int line_2_meta_lengte    = 44;
   char line_2_meta[45];                         /* line_meta_lengte +1 (dus t/m eol) */
   const int line_inhoud_lengte    = 255;
   char line_inhoud[256];                        /* line_inhoud_lengte +1 (dus t/m eol) */
   int k;                                        /* teller richtingen */
   int m;                                        /* teller frequenties */
   int v;                                        /* teller verwachtingen */
   char TSA_tempfilenaam[256];
   int pos;
   int a;                                        /* teller stations */



   /* nb kijk voor de zekerheid in gps.h */
   /*
   **
   ** template array TSA_spectrum [NUM_RICHTINGEN][NUM_FREQUENTIES[[NUM_VERWACHTINGEN][NUM_REALS] elementen
   **
   */


   /* nb in function Scan_TSA() was al gekeken of deze file te openen was */
   if ((in = fopen(TSA_inputfilenaam, "r")) != NULL)
   {
      /* de eerste regel is uniek en wordt niet meer herhaald in een van de komende blokken */
      fread(line_1_meta, line_1_meta_lengte +1, 1, in);         /* was ook al gelezen in function Scan_TSA() */

      /*do*/ /* while (!eof(in)) --> GAAT NIET GOED */
      for (a = 0; a < NUM_STATIONS_TSA; a++)
      {
         /* 4-dimensionaal array aanmaken (geheugenruimte reserveren) */
         Allocate_TSA_Spectrum_Array();

         /* Initialiseer_TSA_Spectrum_Array() */
         Initialiseer_TSA_Spectrum_Array();

         /* 13 blokken (= NUM_RICHTINEGN + 1) voor elke stations positie (1 meta blok + 12 richtings blokken) */
         for (k = -1; k < NUM_RICHTINGEN; k++)
         {
            /* het eerste blok is een apart (meta) blok (hier staan de frequenties die overal gelden) */
            if (k == -1)
            {
               for (m = -1; m < NUM_FREQUENTIES; m++)
               {
                  if (m == -1)
                  {
                     line_2_meta[0] = '\0';
                     if (fread(line_2_meta, line_2_meta_lengte +1, 1, in) != 0)
                     {
                        TSA_tempfilenaam[0] = '\0';
                        Bepaal_TSA_tempfilenaam(line_2_meta, TSA_tempfilenaam);

                        /**************  test ************/
                        /*
                        fprintf(stderr, "%s", line_meta);
                        getchar();
                        */
                        /**************** test ************/

                     } /* if (fread(line_meta, line_meta_lengte +1, 1, in) != NULL) */
                  } /* if (m == -1) */
                  else /* /* gewone frequentie regel */
                  {
                     line_inhoud[0] = '\0';
                     fread(line_inhoud, line_inhoud_lengte +1, 1, in);

                     /* test op willekeurige (14de) frequentie of die wel is zoals aangenomen */
                     /* if (m == 14)                            */
                     /* {                                       */
                     /*    Check_Vaste_Frequentie(line_inhoud); */
                     /* }                                       */

                     /**************  test ************/
                     /*
                     fprintf(stderr, "%s", line_inhoud);
                     getchar();
                     */
                     /**************** test ************/
                  }  /* else (gewone frequentie regel) */
               } /* for (m = -1; m < 25; k++) */
            } /* if (k == -1) */
            else /* "gewoon blok" */
            {
               /* eerste regel is een meta-data regel in elk blok */
               /* hierna 25 frequentie regels in elk blok */
               for (m = -1; m < NUM_FREQUENTIES; m++)
               {
                  if (m == -1)
                  {
                     /* meta line */
                     fread(line_2_meta, line_2_meta_lengte +1, 1, in);


                     /**************  test ************/
                     /*
                     fprintf(stderr, "%s", line_meta);
                     getchar();
                     */
                     /**************** test ************/

                  } /* if (m == -1) */
                  else /* gewone frequentie regel */
                  {
                     line_inhoud[0] = '\0';
                     fread(line_inhoud, line_inhoud_lengte +1, 1, in);

                     /* 17 verwachtingen (0, +3, +6, ..., +48hr) */
                     pos = 0;
                     for (v = 0; v < NUM_VERWACHTINGEN; v++)
                     {
                        /* template: TSA_SPECTRUM[NUM_RICHTINGEN][NUM_FREQUENTIES[[NUM_VERWACHTINGEN][NUM_REALS] */

                        strncpy(TSA_spectrum[k][m][v], line_inhoud + pos, LENGTE_NEDWAM_ENERGY_WAARDE);
                        TSA_spectrum[k][m][v][15] = '\0';

                        pos += 15;
                     } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
                  } /* else (gewone frequentie regel) */
               } /* for (m = -1; m < NUM_FREQUENTIES; m++) */
            } /* else (gewoon blok) */
         } /* for (k == -1; k < NUM_RICHTINGEN; k++) */

         /* per station wegschrijven naar temp directory */
         Write_TSA_Temp_File(TSA_tempfilenaam);

         /* meerdimensinaal array geheugenruimte weer vrijgeven */
         De_Allocate_TSA_Spectrum_Array();

      } /* for (a = 0; a < NUM_STATIONS_TSA; a++) */

      fclose(in);
   } /* if ((in = fopen(TSA_inputfilenaam, "r")) != NULL) */

   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                           Write_TSA_Temp_File                             */
/*                                                                           */
/*****************************************************************************/
int Write_TSA_Temp_File(const char* TSA_tempfilenaam)
{
   FILE* temp;
   int k;
   int m;
   int v;
   char volledig_path[512];
   char info[512];



   if ((temp = fopen(TSA_tempfilenaam, "w")) == NULL)
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

      strcat(info, TSA_tempfilenaam);
      strcat(info, " niet te schrijven\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((temp = fopen(TSA_tempfilenaam, "w")) == NULL) */
   else /* temp file ok */
   {
      for (k = 0; k < NUM_RICHTINGEN; k++)
         for (m = 0; m < NUM_FREQUENTIES; m++)
            for (v = 0; v < NUM_VERWACHTINGEN; v++)
            {
               /* NB onderstaande sizeof geeft de grootte van een pointer (op PC 4 bytes) */
               /*fwrite(TSA_spectrum[k][m][v], sizeof(TSA_spectrum[k][m][v]), 1, temp);*/

               fwrite(TSA_spectrum[k][m][v], strlen(TSA_spectrum[k][m][v]), 1, temp);
               putc('\n', temp);
            } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */

      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                  Initialiseer_TSA_Spectrum_Array                          */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_TSA_Spectrum_Array()
{
   int k;
   int m;
   int v;

   /* nb met calloc wellicht overbodig */

   /* initialisatie */
   for (k= 0; k < NUM_RICHTINGEN; k++)
      for (m = 0; m < NUM_FREQUENTIES; m++)
         for (v = 0; v < NUM_VERWACHTINGEN; v++)
            strcpy(TSA_spectrum[k][m][v], "\0");

   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                      Allocate_TSA_Spectrum_Array                          */
/*                                                                           */
/*****************************************************************************/
int Allocate_TSA_Spectrum_Array()
{
   int k;                    /* teller richtingen */
   int m;                    /* teller frequenties */
   int v;                    /* teller verwachtingen */
   char info[1024];


   /* NB calloc: cleared to 0 */



#if 0
//   /* alloceren van geheugenruimte 4-dimensionaal array */
//   TSA_spectrum = (char****)calloc(NUM_RICHTINGEN, sizeof(char***));
//
//   for (k = 0; k < NUM_RICHTINGEN; k++)
//   {
//      TSA_spectrum[k] = (char***)calloc(NUM_FREQUENTIES, sizeof(char**));
//      for (m = 0; m < NUM_FREQUENTIES; m++)
//      {
//         TSA_spectrum[k][m] = (char**)calloc(NUM_VERWACHTINGEN, sizeof(char*));
//         for (v = 0; v < NUM_VERWACHTINGEN; v++)
//         {
//            TSA_spectrum[k][m][v] = (char*)calloc(NUM_REALS, sizeof(char));
//         }
//      } /* for (m = 0; m < NUM_FREQUENTIES; m++) */
//   } /* for (k = 0; k < NUM_RICHTINGEN; k++) */
#endif


   /* NB de grnezen van dit dynamisch array worden bepaald door NUM_RICHTINGEN etc.       */
   /* deze komen eigenliijk uit TSF_IN / TSF_REALS dus wanneer er geen genoeg geheugen is */
   /* kan het zijn dat NUM_RICHTINGEN etc. veel te grote waarde hebben gekregen           */

   /* alloceren van geheugenruimte 4-dimensionaal array */
   TSA_spectrum = (char****)calloc(NUM_RICHTINGEN, sizeof(char***));
   if (TSA_spectrum == NULL)
   {
      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, "TSA_spectrum calloc <<TSA_spectrum = (char****)calloc(NUM_RICHTINGEN, sizeof(char***))>> not enough space\n");

      /* naar log schrijven */
      Write_Log(info);

      exit(1);
   } /* if (TSA_spectrum == NULL) */

   for (k = 0; k < NUM_RICHTINGEN; k++)
   {
      TSA_spectrum[k] = (char***)calloc(NUM_FREQUENTIES, sizeof(char**));
      if (TSA_spectrum[k] == NULL)
      {
          /* bericht samen stellen */
         strcpy(info, "\0");
         strcat(info, "TSA_spectrum calloc <<TSA_spectrum[k] = (char***)calloc(NUM_FREQUENTIES, sizeof(char**))>> not enough space\n");

         /* naar log schrijven */
         Write_Log(info);

         exit(1);
      } /* if (TSA_spectrum[k] == NULL) */

      for (m = 0; m < NUM_FREQUENTIES; m++)
      {
         TSA_spectrum[k][m] = (char**)calloc(NUM_VERWACHTINGEN, sizeof(char*));
         if (TSA_spectrum[k][m] == NULL)
         {
             /* bericht samen stellen */
            strcpy(info, "\0");
            strcat(info, "TSA_spectrum calloc <<TSA_spectrum[k][m] = (char**)calloc(NUM_VERWACHTINGEN, sizeof(char*))>> not enough space\n");

            /* naar log schrijven */
            Write_Log(info);

            exit(1);
         } /* if (TSA_spectrum[k][m] == NULL) */

         for (v = 0; v < NUM_VERWACHTINGEN; v++)
         {
            TSA_spectrum[k][m][v] = (char*)calloc(NUM_REALS, sizeof(char));
            if (TSA_spectrum[k][m][v] == NULL)
            {
                /* bericht samen stellen */
               strcpy(info, "\0");
               strcat(info, "TSA_spectrum calloc <<TSA_spectrum[k][m][v] = (char*)calloc(NUM_REALS, sizeof(char))>> not enough space\n");

               /* naar log schrijven */
               Write_Log(info);

               exit(1);
            } /* if (TSA_spectrum[k][m][v] == NULL) */
         } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
      } /* for (m = 0; m < NUM_FREQUENTIES; m++) */
   } /* for (k = 0; k < NUM_RICHTINGEN; k++) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                     De_Allocate_TSA_Spectrum_Array                        */
/*                                                                           */
/*****************************************************************************/
int De_Allocate_TSA_Spectrum_Array()
{
   int k;
   int m;
   int v;


   /* de-alloceren van geheugenruimte */
   for (k = 0; k < NUM_RICHTINGEN; k++)
      for (m = 0; m < NUM_FREQUENTIES; m++)
         for (v = 0; v < NUM_VERWACHTINGEN; v++)
            free(TSA_spectrum[k][m][v]);

   free(TSA_spectrum);


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                  Convert_nedwam_coordinates_to_station                    */
/*                                                                           */
/*****************************************************************************/
int Convert_nedwam_coordinates_to_station(const char* local_pos, char* local_station)
{

   if (strncmp(local_pos, "5767     0", 10) == 0)
      strcpy(local_station, "ANA");
   else if (strncmp(local_pos, "5633   200", 10) == 0)
      strcpy(local_station, "AUK");
   else if (strncmp(local_pos, "5333   450", 10) == 0)
      strcpy(local_station, "ELD");
   else if (strncmp(local_pos, "5333   300", 10) == 0)
      strcpy(local_station, "K13");
   else if (strncmp(local_pos, "5267   400", 10) == 0)
      strcpy(local_station, "MUN");
   else if (strncmp(local_pos, "5200   300", 10) == 0)
      strcpy(local_station, "EPL");
   /*else if (strncmp(local_pos, "5400   650", 10) == 0)                /* t/m 30-03-2010 */
   else if (strncmp(local_pos, "5367   600", 10) == 0)                  /* vanaf 31-03-2010 */
      strcpy(local_station, "SMN");
   else if (strncmp(local_pos, "6133   100", 10) == 0)
      strcpy(local_station, "NC2");
   else if (strncmp(local_pos, "5533   400", 10) == 0)
      strcpy(local_station, "A12");

   else
      strcpy(local_station, "unknown");


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                         Bepaal_TSA_tempfilenaam                           */
/*                                                                           */
/*****************************************************************************/
int Bepaal_TSA_tempfilenaam(const char* line, char* TSA_tempfilenaam)
{
   char local_pos[11];
   char local_date_time[11];
   char local_station[4];


   strncpy(local_date_time, line, 10);      /* bv 2001091300 */
   local_date_time[10] = '\0';

   strncpy(local_pos, line + 21, 10);       /* bv 5767_____0 */
   local_pos[10] = '\0';

   local_station[0] = '\0';
   Convert_nedwam_coordinates_to_station(local_pos, local_station);


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(TSA_tempfilenaam, "temp_nedwam\\");
   else                                                /* UNIX */
      strcpy(TSA_tempfilenaam, "temp_nedwam/");

   strcat(TSA_tempfilenaam, "NEDWAM_TSA_");
   strcat(TSA_tempfilenaam, local_station);
   strcat(TSA_tempfilenaam, "_");
   strcat(TSA_tempfilenaam, local_date_time);
   strcat(TSA_tempfilenaam, ".TMP");


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                             Scan_TSA                                      */
/*                                                                           */
/*****************************************************************************/
int Scan_TSA(int* scan_result)
{
   FILE* in;
   char volledig_path[512];
   char info[1024];
   const int line_1_meta_lengte    = 9;
   char line_1_meta[10];                         /* line_1_meta_lengte +1 (dus t/m eol) */
   const int line_2_meta_lengte    = 44;
   char line_2_meta[45];                         /* line_2_meta_lengte +1 (dus t/m eol) */
   char char_num_richtingen[3];                  /* hulp var. */
   char char_num_frequenties[3];                 /* hulp var. */
   char char_num_verwachtingen[3];               /* hulp var. */
   char char_num_stations[3];                    /* hulp var. */
   char frequentie[NUM_REALS];                   /* hulp var. */
   char char_interval_verwachtingen[2];          /* hulp_var. */
   int m;                                        /* frequentie teller */
   const int line_inhoud_lengte    = 255;
   char line_inhoud[256];                        /* line_inhoud_lengte +1 (dus t/m eol) */
   char line_2_JJJJMMDDUU[11];
   /*char buffer[30];*/




   /* bepalen van:                                          */
   /*   - NUM_RICHTINGEN (uit eerste meta/kopregel)         */
   /*   - NUM_FREQUENTIES (uit tweede meta/kop regel)       */
   /*   - NUM_VERWACHTINGEN (uit tweede meta/kopregel)      */
   /*   nb - NUM_REALS (= vast)                             */




   /* initialisatie */
   NUM_FREQUENTIES                = 0;
   NUM_VERWACHTINGEN              = 0;
   NUM_RICHTINGEN                 = 0;
   NUM_STATIONS_TSA               = 0;
   /*NUM_REALS                    = 0;*/
   char_num_frequenties[0]        = '\0';
   char_num_verwachtingen[0]      = '\0';
   char_num_richtingen[0]         = '\0';
   char_num_stations[0]           = '\0';
   char_interval_verwachtingen[0] = '\0';



   if ((in = fopen(TSA_inputfilenaam, "r")) == NULL)
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

      strcat(info, TSA_inputfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

      *scan_result = 0;                    /* not ok */

   } /* if ((in = fopen(TSA_inputfilenaam, "r")) == NULL) */
   else /* file te openen */
   {
      /* initialisatie */
      line_1_meta[0] = '\0';
      line_2_meta[0] = '\0';

      /* eerste regel in de file */
      if (fread(line_1_meta, line_1_meta_lengte +1, 1, in) != 0)
      {
         if (strlen(line_1_meta) >= 9)
         {
            strncpy(char_num_stations, line_1_meta + 1, 2);
            char_num_stations[2] = '\0';

            strncpy(char_num_richtingen, line_1_meta + 7, 2);
            char_num_richtingen[2] = '\0';
         }
      } /* if (fread(line_1_meta, line_1_meta_lengte +1, 1, in) != NULL) */

      /* tweede regel in de file */
      if (fread(line_2_meta, line_2_meta_lengte +1, 1, in) != 0)
      {
         if (strlen(line_2_meta) >= 31)
         {
            strncpy(line_2_JJJJMMDDUU, line_2_meta, 10);
            line_2_JJJJMMDDUU[10] = '\0';

            strncpy(char_interval_verwachtingen, line_2_meta + 18, 1);
            char_interval_verwachtingen[1] = '\0';

            strncpy(char_num_frequenties, line_2_meta + 35, 2);
            char_num_frequenties[2] = '\0';

            strncpy(char_num_verwachtingen, line_2_meta + 32, 2);
            char_num_verwachtingen[2] = '\0';
         }
      } /* if (fread(line_meta, line_meta_lengte +1, 1, in) != NULL) */



      NUM_RICHTINGEN    = atoi(char_num_richtingen) - 1;        /* meta blok waarin alleen de gebruikte frequenties staan niet meetellen */
      /* NUM_REALS         = 16; */                             /* vast */
      NUM_FREQUENTIES   = atoi(char_num_frequenties);
      NUM_VERWACHTINGEN = atoi(char_num_verwachtingen);
      NUM_STATIONS_TSA  = atoi(char_num_stations);
      INT_VERWACHTINGEN = atoi(char_interval_verwachtingen);


      /* inlezen frequentie waarden */
      Allocate_TSA_Frequenties_Array();                     /* dynamisch array met gebruikte (vaste) frequenties) */

      for (m = 0; m < NUM_FREQUENTIES; m++)
      {
         line_inhoud[0] = '\0';                   /* initialisatie */
         if (fread(line_inhoud, line_inhoud_lengte +1, 1, in) != 0)
         {
            frequentie[0] = '\0';                 /* initialisatie */

            strncpy(frequentie, line_inhoud, LENGTE_NEDWAM_ENERGY_WAARDE);
            frequentie[LENGTE_NEDWAM_ENERGY_WAARDE] = '\0';

/***********************************  test *************************************/
/*
                     fprintf(stderr, "%s", frequentie);
                     getchar();
*/
/************************************ test ************************************/

            FR[m] = atof(frequentie);            /* array met frequentie waarden */

         } /* if (fread(line_inhoud, line_inhoud_lengte +1, 1, in) != NULL) */
      } /* for (m = 0; m < NUM_FREQUENTIES; m++) */


      /* CO (verhouding tussen twee opeenvolgende frequenties) berekenen */
      CO = FR[1] / FR[0];                /* had ook FR[2] / FR[1] kunnen zijn etc. */


      /* datum tijd in file moet wel het zelfde zijn als wat aangevraagd via argument lijst */
      if (strncmp(JJJJMMDDUU, line_2_JJJJMMDDUU, 10) != 0)
      {
         /* bericht samen stellen */
         strcpy(info, "\0");

         strcat(info, "JJJJMMDDUU via argument list (");
         strcat(info, JJJJMMDDUU);
         strcat(info, ") and ");
         strcat(info, "JJJJMMDDUU from 2nd line from input file TSF_REALS (");
         strcat(info, line_2_JJJJMMDDUU);
         strcat(info, ") not the same");

         /* naar log schrijven */
         Write_Log(info);

         *scan_result = 0;                              /* not ok */

      } /* if (strncmp(JJJJMMDDUU, line_2_JJJJMMDDUU, 10) != 0) */
      else
         *scan_result = 1;                              /* ok */

      fclose(in);

   } /* else (file te openen) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                      Allocate_Frequenties_Array                           */
/*                                                                           */
/*****************************************************************************/
int Allocate_TSA_Frequenties_Array()
{

   /* alloceren van geheugenruimte 1-dimensionaal array */
   FR = (double*)calloc(NUM_FREQUENTIES, sizeof(double));     /* cleared to 0 */

   
   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                      De_Allocate_Frequenties_Array                        */
/*                                                                           */
/*****************************************************************************/
int De_Allocate_TSA_Frequenties_Array()
{

   /* de-alloceren van geheugenruimte */
   free(FR);


   return 0;
}




