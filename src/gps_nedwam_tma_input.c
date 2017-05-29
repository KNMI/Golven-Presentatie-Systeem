#if !defined(gps_h)                                       /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(WINDOWS)                                      /* zie gps.h */
#include <dir.h>                                          /* o.a. getcwd() in windows */
#else
#include <unistd.h>                                       /* o.a. getcwd() in UNIX */
#endif




/* function prototypes in andere module */
extern int Write_Log(char* bericht);                                                          /* module gps_main.c */
extern int Convert_nedwam_coordinates_to_station(const char* local_pos, char* local_station); /* module: gps_nedwam_tsa_input.c */

/* externe var's */
extern char JJJJMMDDUU[11];                               /* via argument list */
extern char OS[8];

/* function prototypes in deze module */
int Bepaal_TMA_input_filenaam(void);
int Read_TMA(void);
int Initialiseer_TMA_Parameter_Array(void);
int Write_TMA_Temp_File(const char* TMA_tempfilenaam);
int Bepaal_TMA_tempfilenaam(const char* char_datum_tijd, const char* char_local_pos, char* TMA_tempfilenaam);
int Read_TMA_Input_Files(void);

/* var's globaal binnen deze module */
char TMA_inputfilenaam[256];
char TMA_Parameter_Array[NUM_TMA_PARAMETERS][NUM_TMA_VERWACHTINGEN][NUM_TMA_CHAR];







/*****************************************************************************/
/*                                                                           */
/*                         Read_TMA_Input_Files                              */
/*                                                                           */
/*****************************************************************************/
int Read_TMA_Input_Files()
{
   Bepaal_TMA_input_filenaam();
   Read_TMA();


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                        Bepaal_TMA_input_filenaam                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_TMA_input_filenaam()
{
   /* NB: inputfilenaam altijd: TSF_REALS */


   /* initialisatie */
   strcpy(TMA_inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(TMA_inputfilenaam, "input_nedwam\\");
   else                                                /* UNIX */
      strcpy(TMA_inputfilenaam, "input_nedwam/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(TMA_inputfilenaam, "TSF_REALS");


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                             read_TMA                                      */
/*                                                                           */
/*****************************************************************************/
int Read_TMA()
{
   FILE* in;
   char volledig_path[512];
   char info[1024];
   int pos;
   char char_aantal_par[3];
   int int_aantal_par;
   char char_aantal_pos[3];
   int int_aantal_pos;
   char char_datum_tijd[11];
   char char_local_pos[11];
   char char_aantal_verwachtingen[3];
   char char_bufr_number[7];
   int tma_inlees_status;
   int tma_parameter_index;
   int i;
   const unsigned int lengte_meta_regel   = 10;        /* inclusief '\0' */
   const unsigned int lengte_kop_regel    = 45;        /* inclusief '\0' */
   /*const unsigned int lengte_inhoud_regel = 736;       inclusief '\0' */
   const unsigned int lengte_inhoud_regel_2 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_inhoud_regel_3 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_inhoud_regel_4 = 136;     /* regel met data; inclusief '\0' */
   char line0[1024];
   char line1[1024];
   char line2[1024];
   char line3[1024];
   char line4[1024];
   int k;
   int j;
   char TMA_tempfilenaam[256];
   char buffer[3];
   int tma_foutcode;


   
   /* initialisatie */
   tma_inlees_status = 0;                    /* OK */
   tma_foutcode = 0;
   line0[0] = '\0';

   if ((in = fopen(TMA_inputfilenaam, "r")) != NULL)
   {
      /*
      // de eerste regel is uniek en wordt niet meer herhaald in een van de komende blokken
      */
      if (fgets(line0, 1023, in) != NULL)
      {
         if (strlen(line0) == lengte_meta_regel) /* strlen returns the number of characters in s, not counting the null-terminating character. */
         {
            /* aantal parameters */
            pos = 4;
            char_aantal_par[0] = '\0';
            strncpy(char_aantal_par, line0 + pos, 2);
            char_aantal_par[2] = '\0';
            int_aantal_par = atoi(char_aantal_par);

            /* aantal posities */
            pos = 1;                            /* positie teller in record (heeft niets met geografische positie te maken) */
            char_aantal_pos[0] = '\0';
            strncpy(char_aantal_pos, line0 + pos, 2);
            char_aantal_pos[2] = '\0';
            int_aantal_pos = atoi(char_aantal_pos);
         } /* if (strlen(line0) == lengte_meta_regel) */
         else
         {
            tma_inlees_status = 1;
            tma_foutcode = 1;
         }
      } /* if (fgets(line0, 1023, in) != NULL) */
      else
      {
         tma_inlees_status = 1;
         tma_foutcode = 2;
      }



      /*
      // alle locaties inlezen
      */
      if (tma_inlees_status == 0)
      {
         for (k = 0; k < int_aantal_pos; k++)
         {
            /* per locatie (positie) TMA_Parameter_Array initialiseren */
            Initialiseer_TMA_Parameter_Array();

            /* initialisatie */
            line1[0] = '\0';
            line2[0] = '\0';
            line3[0] = '\0';
            line4[0] = '\0';


            for (j = 0; j < int_aantal_par; j++)
            {
               /* // NB altijd 2 regels per parameter */
               /* NB altijd 4 regels per parameter */
               /* NB line1: de kopregel (o.a. positie en parameter aanduiding */
               /*  //  line2: de verwachtingen per parameter */
               /*    line2, line3, line4: de verwachtingen per parameter */

               /* //if ( (fgets(line1, 1023, in) != NULL) && (fgets(line2, 1023, in) != NULL) )*/
               if ( (fgets(line1, 1023, in) != NULL) && (fgets(line2, 1023, in) != NULL) && (fgets(line3, 1023, in) != NULL) && (fgets(line4, 1023, in) != NULL) )
               {
                  /* //if ( (strlen(line1) == lengte_kop_regel) && (strlen(line2) == lengte_inhoud_regel) ) */
                  if ( (strlen(line1) == lengte_kop_regel) && (strlen(line2) == lengte_inhoud_regel_2) && (strlen(line3) == lengte_inhoud_regel_3) && (strlen(line4) == lengte_inhoud_regel_4))
                  {
                     /*
                     // kop regel (line1)
                     */

                     /* datum-tijd */
                     pos = 0;
                     char_datum_tijd[0] = '\0';
                     strncpy(char_datum_tijd, line1 + pos, 10);
                     char_datum_tijd[10] = '\0';

                     /* positie */
                     pos = 21;
                     char_local_pos[0] = '\0';
                     strncpy(char_local_pos, line1 + pos, 10);
                     char_local_pos[10] = '\0';

                     /* per locatie (positie) een temp file bepalen (kan meerdere keren per zelfde locatie gebeuren, maakt niet uit) */
                     Bepaal_TMA_tempfilenaam(char_datum_tijd, char_local_pos, TMA_tempfilenaam);

                     /* aantal verwachtingen */
                     pos = 32;
                     char_aantal_verwachtingen[0] = '\0';
                     strncpy(char_aantal_verwachtingen, line1 + pos, 2);
                     char_aantal_verwachtingen[2] = '\0';

                     /* BUFR nummer (geeft de parameter zoals hm0 aan) */
                     pos = 38;
                     char_bufr_number[0] = '\0';
                     strncpy(char_bufr_number, line1 + pos, 6);
                     char_bufr_number[6] = '\0';

                     /* BUFR nummer omzetten naar parameter (zoals Hm0) */
                     if (strncmp(char_bufr_number, "054022", 6) == 0)      /* Hm0 */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_HM0;
                     }
                     else if (strncmp(char_bufr_number, "054028", 6) == 0) /* Tm0-1 */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_TM0_1;
                     }
                     else if (strncmp(char_bufr_number, "054023", 6) == 0) /* E10 (LFE) */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_E10;        /* nb: Hs10 = wortel(E10) * 4 */
                     }
                     else if (strncmp(char_bufr_number, "054026", 6) == 0) /* direction E10 */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_DD_E10;
                     }
                     else if (strncmp(char_bufr_number, "054049", 6) == 0) /* Hs7 */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_HS7;
                     }
                     else if (strncmp(char_bufr_number, "054046", 6) == 0) /* HSW (Height Swell) */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_HSW;
                     }
                     else if (strncmp(char_bufr_number, "054047", 6) == 0) /* DSW (direction Swell) */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_DSW;
                     }
                     else if (strncmp(char_bufr_number, "054048", 6) == 0) /* PSW (Period Swell) */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_PSW;
                     }
                     else if (strncmp(char_bufr_number, "054024", 6) == 0) /* Dtotal (mean wave direction) */
                     {
                        tma_parameter_index = TMA_PARAMETER_INDEX_DTOTAL;
                     }
                     else
                     {
                        tma_inlees_status = 1;
                        tma_foutcode = 3;
                     }


                     /* controle JJJJMMDDUU */
                     if (strncmp(char_datum_tijd, JJJJMMDDUU, 10) != 0)
                     {
                        tma_inlees_status = 1;
                        tma_foutcode = 4;
                     }


                     /* controle aantal_verwachtingen */
                     if (atoi(char_aantal_verwachtingen) != NUM_TMA_VERWACHTINGEN)
                     {
                        tma_inlees_status = 1;
                        tma_foutcode = 5;
                     }

/*
                     //
                     //// inhoud regel, m.a.w. regel met verwachtingen (line2)
                     //
                     //if (tma_inlees_status == 0)
                     //{
                     //   pos = 0;
                     //   for (i = 0; i < NUM_TMA_VERWACHTINGEN; i++)
                     //   {
                     //      strncpy(TMA_Parameter_Array[tma_parameter_index][i], line2 + pos, NUM_TMA_CHAR -1);
                     //      TMA_Parameter_Array[tma_parameter_index][i][NUM_TMA_CHAR -1] = '\0';
                     //      pos += 15;
                     //   } 
                     //}
*/


                     /*
                     // inhoud regels, m.a.w. regels met verwachtingen (line2, line3, line4)
                     */
                     if (tma_inlees_status == 0)
                     {
                        pos = 0;
                        for (i = 0; i < 20; i++)
                        {
                           strncpy(TMA_Parameter_Array[tma_parameter_index][i], line2 + pos, NUM_TMA_CHAR -1);
                           TMA_Parameter_Array[tma_parameter_index][i][NUM_TMA_CHAR -1] = '\0';
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 20; i < 40; i++)
                        {
                           strncpy(TMA_Parameter_Array[tma_parameter_index][i], line3 + pos, NUM_TMA_CHAR -1);
                           TMA_Parameter_Array[tma_parameter_index][i][NUM_TMA_CHAR -1] = '\0';
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 40; i < NUM_TMA_VERWACHTINGEN; i++)
                        {
                           strncpy(TMA_Parameter_Array[tma_parameter_index][i], line4 + pos, NUM_TMA_CHAR -1);
                           TMA_Parameter_Array[tma_parameter_index][i][NUM_TMA_CHAR -1] = '\0';
                           pos += 15;
                        }

                     } /* if (tma_status == 0) */


                  } /* if ( (strlen(line1) == lengte_kop_regel) && etc. */
                  else
                  {
                     tma_inlees_status = 1;
                     tma_foutcode = 6;
                  }
               }  /* if ( (fgets(line1, 1024, in) != NULL) && etc. */
               else
               {
                  tma_inlees_status = 1;
                  tma_foutcode = 7;
               }

            } /* for (j = 0; j < int_aantal_par; j++) */


            /* tma parameters array wegschrijven naar temp file */
            if (tma_inlees_status == 0)
               Write_TMA_Temp_File(TMA_tempfilenaam);

         } /* for (k = 0; k < int_aantal_pos; k++) */
      } /* if (tma_inlees_status == 0) */


      /*
      // Er is wat fout gegaan tijdens behandelen TMA input file
      */
      if (tma_inlees_status == 1)
      {
         /* er is wat fout gegaan */
         Initialiseer_TMA_Parameter_Array();

         getcwd(volledig_path, 512);

         /* bericht samen stellen */
         strcpy(info, "\0");
         strcat(info, volledig_path);

         if (strcmp(OS, "WINDOWS") == 0)
            strcat(info, "\\");
         else
            strcat(info, "/");

         strcat(info, TMA_inputfilenaam);
         strcat(info, " conceptuele lees fout (fout code: ");

         sprintf(buffer, "%d", tma_foutcode);
         strcat(info, buffer);
         strcat(info, ")\n");

         /* naar log schrijven */
         Write_Log(info);

      } /* if (tma_inlees_status == 1) */

      fclose(in);

   } /* if ((in = fopen(TMA_inputfilenaam, "r")) != NULL) */
   else
   {
      /* dus read file openen is mislukt */

      getcwd(volledig_path, 512);

      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, volledig_path);

      if (strcmp(OS, "WINDOWS") == 0)
         strcat(info, "\\");
      else
         strcat(info, "/");

      strcat(info, TMA_inputfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* else (dus read file openen is mislukt) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                  Initialiseer_TMA_Parameter_Array                         */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_TMA_Parameter_Array()
{
   int i;
   int j;


   /* initialisatie */
   for (i = 0; i < NUM_TMA_PARAMETERS; i++)
      for (j = 0; j < NUM_TMA_VERWACHTINGEN; j++)
         strcpy(TMA_Parameter_Array[i][j], "\0");


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                        Write_TMA_Temp_File                                */
/*                                                                           */
/*****************************************************************************/
int Write_TMA_Temp_File(const char* TMA_tempfilenaam)
{
   FILE* temp;
   int i;
   int j;
   char volledig_path[512];
   char info[512];



   if ((temp = fopen(TMA_tempfilenaam, "w")) == NULL)
   {
      /* dus temp file schrijven mislukt */

      getcwd(volledig_path, 512);

      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, volledig_path);

      if (strcmp(OS, "WINDOWS") == 0)
         strcat(info, "\\");
      else
         strcat(info, "/");

      strcat(info, TMA_tempfilenaam);
      strcat(info, " niet te schrijven\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((temp = fopen(TMA_tempfilenaam, "w")) == NULL) */
   else /* temp file ok */
   {
      for (i = 0; i < NUM_TMA_PARAMETERS; i++)
         for (j = 0; j < NUM_TMA_VERWACHTINGEN; j++)
         {
            /* NB onderstaande sizeof geeft de grootte van een pointer (op PC 4 bytes) */

            fwrite(TMA_Parameter_Array[i][j], strlen(TMA_Parameter_Array[i][j]), 1, temp);
            putc('\n', temp);
         }

      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                         Bepaal_TMA_tempfilenaam                           */
/*                                                                           */
/*****************************************************************************/
int Bepaal_TMA_tempfilenaam(const char* char_datum_tijd, const char* char_local_pos, char* TMA_tempfilenaam)
{
   char local_station[4];


   /* stations naam (b.v. AUK) bepalen aan de hand van de geografische positie uit de file */
   Convert_nedwam_coordinates_to_station(char_local_pos, local_station);

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(TMA_tempfilenaam, "temp_nedwam\\");
   else                                                /* UNIX */
      strcpy(TMA_tempfilenaam, "temp_nedwam/");

   strcat(TMA_tempfilenaam, "NEDWAM_TMA_");
   strcat(TMA_tempfilenaam, local_station);
   strcat(TMA_tempfilenaam, "_");
   strcat(TMA_tempfilenaam, char_datum_tijd);
   strcat(TMA_tempfilenaam, ".TMP");


   return 0;
}





