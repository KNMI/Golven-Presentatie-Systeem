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




/* externe var's */
extern char JJJJMMDDUU[11];                               /* via argument list */
extern char OS[8];

/* function prototypes in andere module */
extern int Write_Log(char* bericht);                      /* module gps_main.c */

/* var's globaal binnen deze module */
char UBA_inputfilenaam[256];
char UBA_Parameter_Array[NUM_UBA_PARAMETERS][NUM_UBA_VERWACHTINGEN][NUM_UBA_CHAR];

/* function prototypes in deze module */
int Bepaal_UBA_input_filenaam(void);
int Read_UBA(void);
int Initialiseer_UBA_Parameter_Array(void);
int Bepaal_UBA_tempfilenaam(const char* char_datum_tijd, const char* char_local_pos, char* UBA_tempfilenaam);
int Write_UBA_Temp_File(const char* UBA_tempfilenaam);
int Convert_hirlam_coordinates_to_station(const char* local_pos, char* local_station);




/*****************************************************************************/
/*                                                                           */
/*                         Read_UBA_Input_Files                              */
/*                                                                           */
/*****************************************************************************/
int Read_UBA_Input_Files()
{
   Bepaal_UBA_input_filenaam();
   Read_UBA();


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                        Bepaal_UBA_input_filenaam                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_UBA_input_filenaam()
{
   /* NB: inputfilenaam altijd: TSF_REALS */


   /* initialisatie */
   strcpy(UBA_inputfilenaam, "\0");


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(UBA_inputfilenaam, "input_hirlam\\");
   else                                                /* UNIX */
      strcpy(UBA_inputfilenaam, "input_hirlam/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(UBA_inputfilenaam, "TSF_REALS");


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                             read_UBA                                      */
/*                                                                           */
/*****************************************************************************/
int Read_UBA()
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
   char char_aantal_verwachtingen[4];
   char char_bufr_number[7];
   int uba_inlees_status;
   int uba_parameter_index;
   int uba_forecast_index;
   int i;
   const unsigned int lengte_uba_meta_regel   = 10;        /* inclusief '\0' */
   const unsigned int lengte_uba_kop_regel    = 45;        /* inclusief '\0' */
/*   const unsigned int lengte_uba_inhoud_regel = 4336; */     /* inclusief '\0' ???*/ /* 48 uur * 6 = 288 + 1 = 289 * 15 char = 4335 + 1 ('\0') = 4336 */
   const unsigned int lengte_uba_inhoud_regel_2  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_3  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_4  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_5  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_6  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_7  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_8  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_9  = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_10 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_11 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_12 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_13 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_14 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_15 = 301;     /* regel met data; inclusief '\0' */
   const unsigned int lengte_uba_inhoud_regel_16 = 136;     /* regel met data; inclusief '\0' */

   char line0[1024];    /* meta regel */
   char line1[1024];    /* kop regel */
   /*char line2[5120];*/
   char line2[1024];
   char line3[1024];
   char line4[1024];
   char line5[1024];
   char line6[1024];
   char line7[1024];
   char line8[1024];
   char line9[1024];
   char line10[1024];
   char line11[1024];
   char line12[1024];
   char line13[1024];
   char line14[1024];
   char line15[1024];
   char line16[1024];


   int k;
   int j;
   char UBA_tempfilenaam[256];
   char buffer[3];
   int uba_foutcode;



   /* initialisatie */
   uba_inlees_status = 0;                    /* OK */
   uba_foutcode = 0;
   line0[0] = '\0';

   if ((in = fopen(UBA_inputfilenaam, "r")) != NULL)
   {
      /*
      // de eerste regel is uniek en wordt niet meer herhaald in een van de komende blokken
      */
      if (fgets(line0, 1023, in) != NULL)
      {
         if (strlen(line0) == lengte_uba_meta_regel) /* strlen returns the number of characters in s, not counting the null-terminating character. */
         {
            /* aantal parameters */
            pos = 4;
            char_aantal_par[0] = '\0';
            strncpy(char_aantal_par, line0 + pos, 2);
            char_aantal_par[2] = '\0';
            int_aantal_par = atoi(char_aantal_par);

            /* aantal posities */
            pos = 1;                            /* var pos: char positie teller in record (heeft niets met geografische positie te maken) */
            char_aantal_pos[0] = '\0';
            strncpy(char_aantal_pos, line0 + pos, 2);
            char_aantal_pos[2] = '\0';
            int_aantal_pos = atoi(char_aantal_pos);
         } /* if (strlen(line0) == lengte_meta_regel) */
         else
         {
            uba_inlees_status = 1;
            uba_foutcode = 1;
         }
      } /* if (fgets(line0, 1023, in) != NULL) */
      else
      {
         uba_inlees_status = 1;
         uba_foutcode = 2;
      }



      /*
      // alle locaties inlezen
      */
      if (uba_inlees_status == 0)
      {
         for (k = 0; k < int_aantal_pos; k++)
         {
            /* per locatie (positie) UBA_Parameter_Array initialiseren */
            Initialiseer_UBA_Parameter_Array();

            /* initialisatie */
            line1[0] = '\0';
            line2[0] = '\0';

            for (j = 0; j < int_aantal_par; j++)
            {
               /* OUD */
               /* NB altijd 2 regels per parameter */
               /* NB line1: de kopregel (o.a. positie en parameter aanduiding) */
               /*    line2: de verwachtingen per parameter */

               /* NIEUW per april 2013 */
               /* NB altijd 16 regels per parameter */
               /* NB line1: de kopregel (o.a. positie en parameter aanduiding) */
               /*    line2 t/m line16: de verwachtingen per parameter */


               /* //if ( (fgets(line1, 1023, in) != NULL) && (fgets(line2, 5119, in) != NULL) ) */
               if ( (fgets(line1, 1023, in) != NULL) && (fgets(line2, 1023, in) != NULL) && (fgets(line3, 1023, in) != NULL) &&
                    (fgets(line4, 1023, in) != NULL) && (fgets(line5, 1023, in) != NULL) && (fgets(line6, 1023, in) != NULL) &&
                    (fgets(line7, 1023, in) != NULL) && (fgets(line8, 1023, in) != NULL) && (fgets(line9, 1023, in) != NULL) &&
                    (fgets(line10, 1023, in) != NULL) && (fgets(line11, 1023, in) != NULL) && (fgets(line12, 1023, in) != NULL) &&
                    (fgets(line13, 1023, in) != NULL) && (fgets(line14, 1023, in) != NULL) && (fgets(line15, 1023, in) != NULL) &&
                    (fgets(line16, 1023, in) != NULL)
                  )
               {
                  /* //if ( (strlen(line1) == lengte_uba_kop_regel) && (strlen(line2) == lengte_uba_inhoud_regel) ) */
                  if ( (strlen(line1) == lengte_uba_kop_regel) &&
                       (strlen(line2) == lengte_uba_inhoud_regel_2) && (strlen(line3) == lengte_uba_inhoud_regel_3) && (strlen(line4) == lengte_uba_inhoud_regel_4) &&
                       (strlen(line5) == lengte_uba_inhoud_regel_5) && (strlen(line6) == lengte_uba_inhoud_regel_6) && (strlen(line7) == lengte_uba_inhoud_regel_7) &&
                       (strlen(line8) == lengte_uba_inhoud_regel_8) && (strlen(line9) == lengte_uba_inhoud_regel_9) && (strlen(line10) == lengte_uba_inhoud_regel_10) &&
                       (strlen(line11) == lengte_uba_inhoud_regel_11) && (strlen(line12) == lengte_uba_inhoud_regel_12) && (strlen(line13) == lengte_uba_inhoud_regel_13) &&
                       (strlen(line14) == lengte_uba_inhoud_regel_14) && (strlen(line15) == lengte_uba_inhoud_regel_15) && (strlen(line16) == lengte_uba_inhoud_regel_16)
                      )
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
                     Bepaal_UBA_tempfilenaam(char_datum_tijd, char_local_pos, UBA_tempfilenaam);

                     /* aantal verwachtingen */
                     pos = 31;
                     char_aantal_verwachtingen[0] = '\0';
                     strncpy(char_aantal_verwachtingen, line1 + pos, 3);
                     char_aantal_verwachtingen[3] = '\0';

                     /* BUFR nummer (geeft de parameter zoals ff aan) */
                     pos = 38;
                     char_bufr_number[0] = '\0';
                     strncpy(char_bufr_number, line1 + pos, 6);
                     char_bufr_number[6] = '\0';

                     /* BUFR nummer omzetten naar parameter (zoals Hm0) */
                     if (strncmp(char_bufr_number, "011012", 6) == 0)
                     {
                        uba_parameter_index = UBA_PARAMETER_INDEX_FF;
                     }
                     /*                                                         */
                     /*else if (strncmp(char_bufr_number, "011011", 6) == NULL) */
                     /*{                                                        */
                     /*   uba_parameter_index = UBA_PARAMETER_INDEX_DD;  */
                     /*}                                                        */
                     else
                     {
                        uba_inlees_status = 1;
                        uba_foutcode = 3;
                     }


                     /* controle JJJJMMDDUU */
                     if (strncmp(char_datum_tijd, JJJJMMDDUU, 10) != 0)
                     {
                        uba_inlees_status = 1;
                        uba_foutcode = 4;
                     }


                     /* controle aantal_verwachtingen (check voor de toekomst) */
                     if (atoi(char_aantal_verwachtingen) != (NUM_UBA_VERWACHTINGEN - 1) * 6 + 1)
                     {
                        uba_inlees_status = 1;
                        uba_foutcode = 5;
                     }


                     /*
                     // inhoud regel, m.a.w. regel met verwachtingen (line2)
                     */
/*
                     if (uba_inlees_status == 0)
                     {
                        pos = 0;
                        for (i = 0; i < atoi(char_aantal_verwachtingen); i++)
                        {
                           if ((i % 6) == 0)         // alleen de uurlijkse waarde opslaan (staan per 10 minuten in de file
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line2 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;

                        }
                     }
*/
                     if (uba_inlees_status == 0)
                     {
                        pos = 0;
                        for (i = 0; i < 20; i++)
                        {
                           if ((i % 6) == 0)         /* alleen de uurlijkse waarde opslaan (staan per 10 minuten in de file */
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line2 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 20; i < 40; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line3 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 40; i < 60; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line4 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 60; i < 80; i++)
                        {
                           if ((i % 6) == 0)         
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line5 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 80; i < 100; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line6 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 100; i < 120; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line7 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 120; i < 140; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line8 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 140; i < 160; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line9 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 160; i < 180; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line10 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 180; i < 200; i++)
                        {
                           if ((i % 6) == 0)         
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line11 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 200; i < 220; i++)
                        {
                           if ((i % 6) == 0)         
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line12 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 220; i < 240; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line13 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 240; i < 260; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line14 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 260; i < 280; i++)
                        {
                           if ((i % 6) == 0)
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line15 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }

                        pos = 0;
                        for (i = 280; i < atoi(char_aantal_verwachtingen); i++)
                        {
                           if ((i % 6) == 0)         
                           {
                              uba_forecast_index = i / 6;
                              strncpy(UBA_Parameter_Array[uba_parameter_index][uba_forecast_index], line16 + pos, NUM_UBA_CHAR -1);
                              UBA_Parameter_Array[uba_parameter_index][uba_forecast_index][NUM_UBA_CHAR -1] = '\0';
                           }
                           pos += 15;
                        }
                     }


                  } /* if ( (strlen(line1) == lengte_uba_kop_regel) && etc. */
                  else
                  {
                     uba_inlees_status = 1;
                     uba_foutcode = 6;
                  }
               }  /* if ( (fgets(line1, 1024, in) != NULL) && etc. */
               else
               {
                  uba_inlees_status = 1;
                  uba_foutcode = 7;
               }

            } /* for (j = 0; j < int_aantal_par; j++) */


            /* uba parameters array wegschrijven naar temp file */
            if (uba_inlees_status == 0)
               Write_UBA_Temp_File(UBA_tempfilenaam);

         } /* for (k = 0; k < int_aantal_pos; k++) */
      } /* if (uba_inlees_status == 0) */


      /*
      // Er is wat fout gegaan tijdens behandelen UBA input file
      */
      if (uba_inlees_status == 1)
      {
         /* er is wat fout gegaan */
         Initialiseer_UBA_Parameter_Array();

         getcwd(volledig_path, 512);

         /* bericht samen stellen */
         strcpy(info, "\0");
         strcat(info, volledig_path);

         if (strcmp(OS, "WINDOWS") == 0)
            strcat(info, "\\");
         else
            strcat(info, "/");

         strcat(info, UBA_inputfilenaam);
         strcat(info, " conceptuele lees fout (fout code: ");

         sprintf(buffer, "%d", uba_foutcode);
         strcat(info, buffer);
         strcat(info, ")\n");

         /* naar log schrijven */
         Write_Log(info);

      } /* if (uba_inlees_status == 1) */

      fclose(in);

   } /* if ((in = fopen(UBA_inputfilenaam, "r")) != NULL) */
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

      strcat(info, UBA_inputfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* else (dus read file openen is mislukt) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                  Initialiseer_UBA_Parameter_Array                         */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_UBA_Parameter_Array()
{
   int i;
   int j;


   /* initialisatie */
   for (i = 0; i < NUM_UBA_PARAMETERS; i++)
      for (j = 0; j < NUM_UBA_VERWACHTINGEN; j++)
         strcpy(UBA_Parameter_Array[i][j], "\0");


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                           Write_UBA_Temp_File                             */
/*                                                                           */
/*****************************************************************************/
int Write_UBA_Temp_File(const char* UBA_tempfilenaam)
{
   FILE* temp;
   int i;
   int j;
   char volledig_path[512];
   char info[512];



   if ((temp = fopen(UBA_tempfilenaam, "w")) == NULL)
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

      strcat(info, UBA_tempfilenaam);
      strcat(info, " niet te schrijven\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((temp = fopen(UBA_tempfilenaam, "w")) == NULL) */
   else /* temp file ok */
   {
      for (i = 0; i < NUM_UBA_PARAMETERS; i++)
         for (j = 0; j < NUM_UBA_VERWACHTINGEN; j++)
         {
            /* NB sizeof geeft de grootte van een pointer (op PC 4 bytes) */

            fwrite(UBA_Parameter_Array[i][j], strlen(UBA_Parameter_Array[i][j]), 1, temp);
            putc('\n', temp);
         }

      fclose(temp);

   } /* else (temp file ok) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                         Bepaal_UBA_tempfilenaam                           */
/*                                                                           */
/*****************************************************************************/
int Bepaal_UBA_tempfilenaam(const char* char_datum_tijd, const char* char_local_pos, char* UBA_tempfilenaam)
{
   char local_station[4];


   /* stations naam (b.v. AUK) bepalen aan de hand van de goegrafische positie uit de file */
   Convert_hirlam_coordinates_to_station(char_local_pos, local_station);

   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(UBA_tempfilenaam, "temp_hirlam\\");
   else                                                /* UNIX */
      strcpy(UBA_tempfilenaam, "temp_hirlam/");

   strcat(UBA_tempfilenaam, "HIRLAM_UBA_");
   strcat(UBA_tempfilenaam, local_station);
   strcat(UBA_tempfilenaam, "_");
   strcat(UBA_tempfilenaam, char_datum_tijd);
   strcat(UBA_tempfilenaam, ".TMP");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                  Convert_hirlm_coordinates_to_station                    */
/*                                                                           */
/*****************************************************************************/
int Convert_hirlam_coordinates_to_station(const char* local_pos, char* local_station)
{
/*
   if (strncmp(local_pos, "5633   249", 10) == 0)
      strcpy(local_station, "AUK");
   else if (strncmp(local_pos, "5333   425", 10) == 0)
      strcpy(local_station, "ELD");
   else if (strncmp(local_pos, "5306   343", 10) == 0)
      strcpy(local_station, "K13");
   else if (strncmp(local_pos, "5238   378", 10) == 0)
      strcpy(local_station, "MUN");
   else if (strncmp(local_pos, "5191   289", 10) == 0)
      strcpy(local_station, "EPL");
   else if (strncmp(local_pos, "5392   669", 10) == 0)
      strcpy(local_station, "SMN");
   else if (strncmp(local_pos, "6113    46", 10) == 0)
      strcpy(local_station, "NC2");
   else
      strcpy(local_station, "unknown");
*/


   if (strncmp(local_pos, "5363   493", 10) == 0)
      strcpy(local_station, "ELD");
   else if (strncmp(local_pos, "5321   315", 10) == 0)
      strcpy(local_station, "K13");
   else if (strncmp(local_pos, "5213   400", 10) == 0)
      strcpy(local_station, "MUN");
   else if (strncmp(local_pos, "5354   647", 10) == 0)
      strcpy(local_station, "SMN");
   else
      strcpy(local_station, "unknown");


   return 0;
}






