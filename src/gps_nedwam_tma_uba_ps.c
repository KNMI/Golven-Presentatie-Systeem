#if !defined(gps_h)            /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <values.h>            /* MAXINT */
#include <time.h>    
#include <math.h>              /* sqrt */

#if defined(WINDOWS)           /* zie gps.h */
#include <dir.h>               /* o.a. getcwd() in windows */
#else
#include <unistd.h>            /* o.a. getcwd() in UNIX */
#endif



/* function prototypes in andere module */
extern int Write_Log(char* bericht);                               /* zie module: gps_main.c */
extern int Write_Postscript_General(void);                         /* zie module: gps_ps.c */
extern int Write_Postscript_End(void);                             /* zie module: gps_ps.c */
extern int Initialiseer_TMA_Parameter_Array(void);                 /* zie module: gps_nedwam_tma_input */
extern int Initialiseer_UBA_Parameter_Array(void);                 /* zie module: gps_hirlam_uba_input */

/* function prototypes in deze module */
int Initialiseer_CIC_TMA_Spectrum_Arrays(void);
int Initialiseer_CID_TMA_Arrays(void);
int Initialiseer_HWA_TMA_Arrays(void);
int Bepaal_Filenamen_CIC_Temp_Files_voor_TMA_Per_Locatie(const char* station);
int Bepaal_Filenamen_CID_Temp_Files_voor_TMA_Per_Locatie(const char* station);
int Bepaal_Filenamen_HWA_Temp_Files_voor_TMA_Per_Locatie(const char* station);
int Read_CIC_TMA_Temps_in_Spectrums(void);
int Read_CID_TMA_Temps_in_Arrays(void);
int Read_HWA_TMA_Temps_in_Arrays(void);
int Bepaal_Scale_X(double* scale_x);
int Bepaal_TMA_Outputfilenaam_Per_Locatie(const char* station, const int tma_pagina_no);
int Bepaal_TMA_Tempfilenaam(const char* station);
int Bepaal_UBA_Tempfilenaam(const char* station);
int Bepaal_Max_Parameter_Model(double* max_parameter_nedwam, const char* parameter);
int Bepaal_Max_Parameter_CIC(double* max_parameter_cic, const char* parameter);
int Bepaal_Scale_Y_Max_Parameter(const double* max_parameter_nedwam_cic, const char* parameter, int* scale_y);
int Bepaal_Title_Par_Nedwam_Per_Grafiek(char* nedwam_par_title, const char* station, const char* parameter);
int Write_Postscript_Par_Raster_Box(const char* grafiek_orientatie, const int scale_y, const double scale_x, const char* parameter, const char* model, const char* nedwam_par_title, int* y_interval);
int Nedwam_Par_To_Postscript(const int scale_x, const int scale_y, const int y_interval, const char* parameter);
int Write_Nedwam_Par_Legenda(const char* model);
int Read_TMA_Temps_in_TMA_Parameter_Arrays(void);
int Read_UBA_Temps_in_UBA_Parameter_Arrays(void);
int Write_Nedwam_TMA_UBA_Postscript_Files(void);

/* variabelen voor deze module */
const int lengte_x_as_par = 528;  /* 512 */             /* = 612 - (2 * 42); n.b. totale pagina breedte = 612 */
const int lengte_y_as_par = 160;
const int x1_par = 42;  /* 50 */                        /* main parameter box */
const int _y1_par = 50;                                 /* main parameter box */
int x2_par;                                             /* main parameter box */
int y2_par;                                             /* main parameter box */
char cic_tma_temp_0[256];                               /* cic temp file corresponderende met tma 0 hr */
char cic_tma_temp_1[256];                               /* cic temp file corresponderende met tma +1 hr */
char cic_tma_temp_2[256];                               /* cic temp file corresponderende met tma +2 hr */
char cic_tma_temp_3[256];                               /* cic temp file corresponderende met tma +3 hr */
char cic_tma_temp_4[256];                               /* cic temp file corresponderende met tma +4 hr */
char cic_tma_temp_5[256];                               /* cic temp file corresponderende met tma +5 hr */
char cic_tma_temp_6[256];                               /* cic temp file corresponderende met tma +6 hr */
char cic_tma_temp_7[256];                               /* cic temp file corresponderende met tma +7 hr */
char cic_tma_temp_8[256];                               /* cic temp file corresponderende met tma +8 hr */
char cic_tma_spectrum_0[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU 0 hr */
char cic_tma_spectrum_1[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +1 hr */
char cic_tma_spectrum_2[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +2 hr */
char cic_tma_spectrum_3[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +3 hr */
char cic_tma_spectrum_4[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +4 hr */
char cic_tma_spectrum_5[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +5 hr */
char cic_tma_spectrum_6[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +6 hr */
char cic_tma_spectrum_7[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +7 hr */
char cic_tma_spectrum_8[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU +8 hr */
const int aantal_cic_tma_temp_files = 9;                /* zoveel cic temp files openen om bij tma verwachtingen te zetten */
const int aantal_cid_tma_temp_files = 9;                /* zoveel cid temp files openen om bij tma verwachtingen te zetten */
const int aantal_hwa_tma_temp_files = 9;                /* zoveel hwa temp files openen om bij tma verwachtingen te zetten */
char tma_outputfilenaam[256];                           /* postscript file */
char tma_tempfilenaam[256];
char uba_tempfilenaam[256];
char cid_tma_temp_0[256];                               /* cid temp file corresponderende met tma 0 hr */
char cid_tma_temp_1[256];                               /* cid temp file corresponderende met tma +1 hr */
char cid_tma_temp_2[256];                               /* cid temp file corresponderende met tma +2 hr */
char cid_tma_temp_3[256];                               /* cid temp file corresponderende met tma +3 hr */
char cid_tma_temp_4[256];                               /* cid temp file corresponderende met tma +4 hr */
char cid_tma_temp_5[256];                               /* cid temp file corresponderende met tma +5 hr */
char cid_tma_temp_6[256];                               /* cid temp file corresponderende met tma +6 hr */
char cid_tma_temp_7[256];                               /* cid temp file corresponderende met tma +7 hr */
char cid_tma_temp_8[256];                               /* cid temp file corresponderende met tma +8 hr */
char cid_tma_array_0[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU 0 hr */
char cid_tma_array_1[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +1 hr */
char cid_tma_array_2[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +2 hr */
char cid_tma_array_3[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +3 hr */
char cid_tma_array_4[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +4 hr */
char cid_tma_array_5[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +5 hr */
char cid_tma_array_6[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +6 hr */
char cid_tma_array_7[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +7 hr */
char cid_tma_array_8[AANTAL_SWELL_CID_ELEMENTEN][LENGTE_CID_SWELL_ELEMENTEN]; /* CID parameters van JJJJMMDDUU +8 hr */
char hwa_tma_temp_0[256];                               /* hwa temp file corresponderende met tma 0 hr */
char hwa_tma_temp_1[256];                               /* hwa temp file corresponderende met tma +1 hr */
char hwa_tma_temp_2[256];                               /* hwa temp file corresponderende met tma +2 hr */
char hwa_tma_temp_3[256];                               /* hwa temp file corresponderende met tma +3 hr */
char hwa_tma_temp_4[256];                               /* hwa temp file corresponderende met tma +4 hr */
char hwa_tma_temp_5[256];                               /* hwa temp file corresponderende met tma +5 hr */
char hwa_tma_temp_6[256];                               /* hwa temp file corresponderende met tma +6 hr */
char hwa_tma_temp_7[256];                               /* hwa temp file corresponderende met tma +7 hr */
char hwa_tma_temp_8[256];                               /* hwa temp file corresponderende met tma +8 hr */
char hwa_tma_array_0[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU 0 hr */
char hwa_tma_array_1[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +1 hr */
char hwa_tma_array_2[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +2 hr */
char hwa_tma_array_3[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +3 hr */
char hwa_tma_array_4[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +4 hr */
char hwa_tma_array_5[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +5 hr */
char hwa_tma_array_6[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +6 hr */
char hwa_tma_array_7[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +7 hr */
char hwa_tma_array_8[AANTAL_HWA_ELEMENTEN][LENGTE_HWA_ELEMENTEN]; /* HWA parameters van JJJJMMDDUU +8 hr */




/* globale var's in andere module */
extern FILE* os_ps;                                     /* gps_main.c; schrijven postscript files */
extern char TMA_Parameter_Array[NUM_TMA_PARAMETERS][NUM_TMA_VERWACHTINGEN][NUM_TMA_CHAR];  /* gps_nedwam_tma_input */
extern char UBA_Parameter_Array[NUM_UBA_PARAMETERS][NUM_UBA_VERWACHTINGEN][NUM_UBA_CHAR];  /* gps_hirlam_uba_input */
extern char OS[8];                                      /* gps_main.c */
extern char JJJJMMDDUU[11];                             /* gps_main.c; via argument list */
extern char font_keuze[50];                             /* gps_main.c */
extern char stations[NUM_STATIONS][NUM_STATIONS_2];     /* gps_main.c; NUM_STATIONS arrayplaatsen van elk 4 char */





/*****************************************************************************/
/*                                                                           */
/*                        Write_Nedwam_TMA_Postscript_Files                  */
/*                                                                           */
/*****************************************************************************/
int Write_Nedwam_TMA_UBA_Postscript_Files()
{
   double max_parameter_nedwam;                /* gebruikt voor HM0, Tm0-1, ff (hoogste van nedwam gedeelte) */
   double max_parameter_cic;                   /* gebruikt voor HM0, Tm0-1, ff (hoogste van cic gedeelte) */
   double max_parameter_nedwam_cic;            /* gebruikt voor HM0, Tm0-1, ff (hoogste van nedwam EN cic gedeelte) */
   int scale_y;
   double scale_x;                             /* afstand tussen 2 marks (forecast tijdstippen) op de x-as */
   char info[512];
   char volledig_path[512];
   char grafiek_orientatie[256];
   char parameter[4];                          /* bv. HM0, TM0, _FF */
   char model[7];                              /* NEDWAM of HIRLAM */
   char nedwam_par_title[256];                 /* title (b.v.: RUN 21-05-2002 00.00 UTC K13 HM0 */
   int y_interval;                             /* aantal eenheden (ff, m, sec) tussen 2 marks op y-as (n.b. dit is geen scale_y; bij scale_y gaat het over aantal PIXELS tussen 2 marks op y-as) */
   int i;
   /*char buffer[100];*/
   int aantal_tma_outputfiles = 3;             /* meerdere pagina's met (integrale, zoals hm0, hs10) parameters */
   int p;


   /* per locatie postscript files aanmaken */
   for (i = 0; i < NUM_STATIONS; i++)
   {
      if (strcmp(stations[i], "\0") != 0)                                          /* niet leeg */
      {
         /* initialisatie */
         max_parameter_nedwam_cic = 0.0;
         max_parameter_nedwam = 0.0;
         max_parameter_cic = 0.0;

         /* bepalen TMA en UBA temp files */
         Bepaal_TMA_Tempfilenaam(stations[i]);                                      /* zie module: deze */
         Bepaal_UBA_Tempfilenaam(stations[i]);                                      /* zie module: deze */

         /* initialiseren TMA en UBA parameter arrays */
         Initialiseer_TMA_Parameter_Array();                                        /* zie module: gps_nedwam_tma_input */
         Initialiseer_UBA_Parameter_Array();                                        /* zie module: gps_hirlam_uba_input */

         /* vullen TMA en UBA parameter arrays */
         Read_TMA_Temps_in_TMA_Parameter_Arrays();                                  /* zie module: deze */
         Read_UBA_Temps_in_UBA_Parameter_Arrays();                                  /* zie module: deze */

         /* bepalen CIC (hmo etc.) en CID (voor swell) temp files die bij de verwachtingen (+00, +01, +02hr etc.) horen */
         Bepaal_Filenamen_CIC_Temp_Files_voor_TMA_Per_Locatie(stations[i]);         /* zie module: deze */
         Bepaal_Filenamen_CID_Temp_Files_voor_TMA_Per_Locatie(stations[i]);         /* zie module: deze */
/***/    Bepaal_Filenamen_HWA_Temp_Files_voor_TMA_Per_Locatie(stations[i]);         /* zie module: deze */

         /* initialisatie CIC/CID arrays */
         Initialiseer_CIC_TMA_Spectrum_Arrays();                                    /* zie module: deze */
         Initialiseer_CID_TMA_Arrays();                                             /* zie module: deze */
/***/    Initialiseer_HWA_TMA_Arrays();                                             /* zie module: deze */

         /* vullen CIC/CID arrays */
         Read_CIC_TMA_Temps_in_Spectrums();                                         /* zie module: deze */
         Read_CID_TMA_Temps_in_Arrays();                                            /* zie module: deze */
/***/    Read_HWA_TMA_Temps_in_Arrays();                                            /* zie module: deze */

         /* afstand tussen 2 marks (forecast tijdstippen) in pixels op x-as bepalen */
         Bepaal_Scale_X(&scale_x);                                                  /* zie module: deze */


         /* worden meer dan 1 outputfile voor tma (integrale parameters) aangemaakt */
         for (p = 0; p < aantal_tma_outputfiles; p++)
         {
            /* naar welke file de parameter postscript regels wegschrijven */
            Bepaal_TMA_Outputfilenaam_Per_Locatie(stations[i], p);                   /* zie module: deze */

            if ((os_ps = fopen(tma_outputfilenaam, "w")) == NULL)      /* dus mislukt */
            {
               getcwd(volledig_path, 512);

               /* bericht samen stellen */
               strcpy(info, "\0");
               strcat(info, volledig_path);

               if (strcmp(OS, "WINDOWS") == 0)
                  strcat(info, "\\");
               else
                  strcat(info, "/");

               strcat(info, tma_outputfilenaam);
               strcat(info, " niet te schrijven\n");

               /* naar log schrijven */
               Write_Log(info);

            } /* if ((os_ps = fopen(tma_outputfilenaam, "w")) == NULL) */
            else /* outputfile is te schrijven */
            {
               /* ps pagina correct beginnen */
               Write_Postscript_General();


               /*
               // onderste grafiek (I.V.M. 'TRANSLATE' ONDERSTE GRAFIEK ALS EERSTE DOEN)
               //
               // bv Tm0-1 van NEDWAM op tma pagina 0  of PSW op pagina 2
               //
               */
               strcpy(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK);

               if (p == 0)                                             /* eerste pagina met integrale parameters */
               {
                  strncpy(parameter, "TM0", 3);
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }
               else if (p == 1)                                        /* tweede pagina met integrale parameters */
               {
                  strncpy(parameter, "HS7", 3);
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }
               else if (p == 2)                                        /* derde pagina met integrale parameters */
               {
                  strncpy(parameter, "PSW", 3);
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }


               Bepaal_Max_Parameter_Model(&max_parameter_nedwam, parameter);                /* zie module: deze */
               Bepaal_Max_Parameter_CIC(&max_parameter_cic, parameter);                     /* zie module: deze */
               max_parameter_nedwam_cic = max_parameter_nedwam;
               if (max_parameter_cic > max_parameter_nedwam_cic)
                  max_parameter_nedwam_cic = max_parameter_cic;

               Bepaal_Scale_Y_Max_Parameter(&max_parameter_nedwam_cic, parameter, &scale_y); /* zie module: deze */
               Bepaal_Title_Par_Nedwam_Per_Grafiek(nedwam_par_title, stations[i], parameter);/* zie module: deze */
               Write_Postscript_Par_Raster_Box(grafiek_orientatie, scale_y, scale_x, parameter, model, nedwam_par_title, &y_interval); /* zie module: deze */
               Nedwam_Par_To_Postscript(scale_x, scale_y, y_interval, parameter);           /* zie module: deze */


               /*
               // middelste grafiek (I.V.M. 'TRANSLATE' MIDDELSTE GRAFIEK ALS TWEEDE DOEN)
               //
               // bv ff van HIRLAM op tma pagina 0
               */
               strcpy(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK);

               if (p == 0)                                 /* eerste pagina met integrale parameters */
               {
                  strncpy(parameter, "_FF", 3);
                  parameter[3] = '\0';

                  strncpy(model, "HIRLAM", 6);
                  model[6] = '\0';
               }
               else if (p == 1)                            /* tweede pagina met integrale parameters */
               {
                  strncpy(parameter, "DHE", 3);            /* direction HE3 */
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }
               else if (p == 2)                            /* derde pagina met integrale parameters */
               {
                  strncpy(parameter, "DSW", 3);
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }


               Bepaal_Max_Parameter_Model(&max_parameter_nedwam, parameter);
               Bepaal_Max_Parameter_CIC(&max_parameter_cic, parameter);
               max_parameter_nedwam_cic = max_parameter_nedwam;
               if (max_parameter_cic > max_parameter_nedwam_cic)
                  max_parameter_nedwam_cic = max_parameter_cic;

               Bepaal_Scale_Y_Max_Parameter(&max_parameter_nedwam_cic, parameter, &scale_y);
               Bepaal_Title_Par_Nedwam_Per_Grafiek(nedwam_par_title, stations[i], parameter);
               Write_Postscript_Par_Raster_Box(grafiek_orientatie, scale_y, scale_x, parameter, model, nedwam_par_title, &y_interval);
               Nedwam_Par_To_Postscript(scale_x, scale_y, y_interval, parameter);


               /*
               // bovenste grafiek (I.V.M. 'TRANSLATE' MIDDELSTE GRAFIEK ALS LAATSTE DOEN)
               //
               // b.v HM0 van NEDWAM op tma pagina 0 of Hs10 op tma pagina 1 of HSW op pagina 2
               */
               strcpy(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK);

               if (p == 0)
               {
                  strncpy(parameter, "HM0", 3);
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }
               else if (p == 1)
               {
                  strncpy(parameter, "HE3", 3);                      /* nb: HE3 = Hs10 */
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }
               else if (p == 2)                                      /* derde pagina met integrale parameters */
               {
                  strncpy(parameter, "HSW", 3);
                  parameter[3] = '\0';

                  strncpy(model, "NEDWAM", 6);
                  model[6] = '\0';
               }


               Bepaal_Max_Parameter_Model(&max_parameter_nedwam, parameter);
               Bepaal_Max_Parameter_CIC(&max_parameter_cic, parameter);
               max_parameter_nedwam_cic = max_parameter_nedwam;
               if (max_parameter_cic > max_parameter_nedwam_cic)
                  max_parameter_nedwam_cic = max_parameter_cic;

               Bepaal_Scale_Y_Max_Parameter(&max_parameter_nedwam_cic, parameter, &scale_y);
               Bepaal_Title_Par_Nedwam_Per_Grafiek(nedwam_par_title, stations[i], parameter);
               Write_Postscript_Par_Raster_Box(grafiek_orientatie, scale_y, scale_x, parameter, model, nedwam_par_title, &y_interval);
               Nedwam_Par_To_Postscript(scale_x, scale_y, y_interval, parameter);

               /* ps pagina correct afsluiten */
               Write_Postscript_End();

               fclose(os_ps);

            } /* else (outputfile is te schrijven) */

         } /* for (p = 0; p < aantal_tma_outputfiles; p++) */

      } /* if (strcmp(stations[i], "\0") != 0) */
   } /* for (i = 0; i < NUM_STATIONS; i++) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                       Bepaal_Max_Parameter_CIC                            */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Parameter_CIC(double* max_parameter_cic, const char* parameter)
{
   int SI_factor;                         /* met dit getal array inhoud delen om SI eenheid te krijgen */
   double SI_factor_hwa;                  /* met dit getal array inhoud delen om SI eenheid te krijgen bij hwa (rtwndb)  */
   int par_indice;                        /* indice nummer van te aangevraagde parameter (bv. hm0) */
   int par_indice_hwa;                    /* hwa data (rtwndb data noordelijke stations) */
   double hulp_0_max_parameter_cic;
   double hulp_1_max_parameter_cic;
   double hulp_2_max_parameter_cic;
   double hulp_3_max_parameter_cic;
   double hulp_4_max_parameter_cic;
   double hulp_5_max_parameter_cic;
   double hulp_6_max_parameter_cic;
   double hulp_7_max_parameter_cic;
   double hulp_8_max_parameter_cic;
   /*char buffer[100]; */

   /* de cic (cid) parameters komen uit 3 temp files (dus ook uit 3 verschillende arrays) */
   /*     - swell parameter uit cid_array */
   /*     - en andere parameters uit spectrum_0 etc. verder ook als eerste leeg is rtwndb data uit hwa file */

   /* nb .._spectrum_0/array_0 is cic spectrum/cid_array dat correspondeerd met nedwam +00 hr */
   /* nb .._spectrum_1/array_1 is cic spectrum/cid_array dat correspondeerd met nedwam +01 hr */
   /* nb .._spectrum_2/array_2 is cic spectrum/cid_array dat correspondeerd met nedwam +02 hr */
   /* nb .._spectrum_3/array_3 is cic spectrum/cid_array dat correspondeerd met nedwam +03 hr */
   /* nb .._spectrum_4/array_4 is cic spectrum/cid_array dat correspondeerd met nedwam +04 hr */
   /* nb .._spectrum_5/array_5 is cic spectrum/cid_array dat correspondeerd met nedwam +05 hr */
   /* nb .._spectrum_6/array_6 is cic spectrum/cid_array dat correspondeerd met nedwam +06 hr */
   /* nb .._spectrum_7/array_7 is cic spectrum/cid_array dat correspondeerd met nedwam +07 hr */
   /* nb .._spectrum_8/array_8 is cic spectrum/cid_array dat correspondeerd met nedwam +08 hr */


   /* initialisatie */
   *max_parameter_cic = 0.0;
   hulp_0_max_parameter_cic = 0.0;
   hulp_1_max_parameter_cic = 0.0;
   hulp_2_max_parameter_cic = 0.0;
   hulp_3_max_parameter_cic = 0.0;
   hulp_4_max_parameter_cic = 0.0;
   hulp_5_max_parameter_cic = 0.0;
   hulp_6_max_parameter_cic = 0.0;
   hulp_7_max_parameter_cic = 0.0;
   hulp_8_max_parameter_cic = 0.0;


   /* afh. van de parameter instellingen maken */
   if (strncmp(parameter, "HM0", 3) == 0)
   {
      par_indice = HM0_SPECTRUM;
      SI_factor = 1000;                              /* bv. inhoud array delen door 1000 om meter te krijgen */

      /* cic files leeg dan data uit hwa(rtwndb) files gebruiken */
      par_indice_hwa = HM0_HWA_INDEX;                /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1;
   }
   else if (strncmp(parameter, "TM0", 3) == 0)
   {
      par_indice = TM0_1_SPECTRUM;
      SI_factor = 10;

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = TM0_HWA_INDEX;                /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1;
   }
   else if (strncmp(parameter, "_FF", 3) == 0)
   {
      par_indice = FF_WIND;
      SI_factor = 1;

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = FF_HWA_INDEX;                 /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1 / M_S_TO_KNOTS;
   }
   else if (strncmp(parameter, "HE3", 3) == 0)       /* HE3 = Hs10 */
   {
      par_indice = HE3_SPECTRUM;                  /* staat in mm */
      SI_factor = 1000;                           /* nu in m */
   }
   else if (strncmp(parameter, "DHE", 3) == 0)    /* direction HE3 */
   {
      par_indice = DD_GEM_LFE_SPECTRUM;           /* staat in graden */
      SI_factor = 1;
   }
   else if (strncmp(parameter, "HS7", 3) == 0)    /*  */
   {
      par_indice = HS7_SPECTRUM;                  /* staat in mm */
      SI_factor = 1000;                           /* nu in m */
   }
   else if (strncmp(parameter, "HSW", 3) == 0)    /* height swell */
   {
      par_indice = HSW_CID_INDEX;                 /* staat in cm */
      SI_factor = 100;                            /* nu in m */
   }
   else if (strncmp(parameter, "DSW", 3) == 0)    /* direction swell  */
   {
      par_indice = DSW_CID_INDEX;                 /* staat in graden */
      SI_factor = 1;
   }
   else if (strncmp(parameter, "PSW", 3) == 0)    /* period swell */
   {
      par_indice = PSW_CID_INDEX;                 /* staat in 0.1 sec */
      SI_factor = 10;                             /* nu in 1 sec */
   }


   if ( (strncmp(parameter, "HSW", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0) || (strncmp(parameter, "PSW", 3) == 0) )
   {
      if ( (strcmp(cid_tma_array_0[par_indice], "\0") != 0) && (atoi(cid_tma_array_0[par_indice]) > 0) )
         hulp_0_max_parameter_cic = atof(cid_tma_array_0[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_1[par_indice], "\0") != 0) && (atoi(cid_tma_array_1[par_indice]) > 0) )
         hulp_1_max_parameter_cic = atof(cid_tma_array_1[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_2[par_indice], "\0") != 0) && (atoi(cid_tma_array_2[par_indice]) > 0) )
         hulp_2_max_parameter_cic = atof(cid_tma_array_2[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_3[par_indice], "\0") != 0) && (atoi(cid_tma_array_3[par_indice]) > 0) )
         hulp_3_max_parameter_cic = atof(cid_tma_array_3[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_4[par_indice], "\0") != 0) && (atoi(cid_tma_array_4[par_indice]) > 0) )
         hulp_4_max_parameter_cic = atof(cid_tma_array_4[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_5[par_indice], "\0") != 0) && (atoi(cid_tma_array_5[par_indice]) > 0) )
         hulp_5_max_parameter_cic = atof(cid_tma_array_5[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_6[par_indice], "\0") != 0) && (atoi(cid_tma_array_6[par_indice]) > 0) )
         hulp_6_max_parameter_cic = atof(cid_tma_array_6[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_7[par_indice], "\0") != 0) && (atoi(cid_tma_array_7[par_indice]) > 0) )
         hulp_7_max_parameter_cic = atof(cid_tma_array_7[par_indice]) / SI_factor;

      if ( (strcmp(cid_tma_array_8[par_indice], "\0") != 0) && (atoi(cid_tma_array_8[par_indice]) > 0) )
         hulp_8_max_parameter_cic = atof(cid_tma_array_8[par_indice]) / SI_factor;

   } /* if ( (strncmp(parameter, "HSW", 3) == 0) || etc. */
   else /* alle andere cic parameters  */
   {
      /*
      // CIC spectrum behorende bij nedwam forecasts per parameter van +00hr +01hr +02hr etc.
      // nb extra bij cic wind: 99 betekent hier niet waargenomen; bij 0 kan het zijn niet waargenomen en ook windstil
      */

      /* 0 hr */
      if ( (strcmp(cic_tma_spectrum_0[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_0[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_0[par_indice]) != 99)) )
            hulp_0_max_parameter_cic = atof(cic_tma_spectrum_0[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_0[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_0[par_indice_hwa], "//", 2) != 0) )
         hulp_0_max_parameter_cic = atof(hwa_tma_array_0[par_indice_hwa]) / SI_factor_hwa;

      /* + 1hr */
      if ( (strcmp(cic_tma_spectrum_1[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_1[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_1[par_indice]) != 99)) )
            hulp_1_max_parameter_cic = atof(cic_tma_spectrum_1[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_1[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_1[par_indice_hwa], "//", 2) != 0) )
         hulp_1_max_parameter_cic = atof(hwa_tma_array_1[par_indice_hwa]) / SI_factor_hwa;

      /* + 2hr */
      if ( (strcmp(cic_tma_spectrum_2[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_2[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_2[par_indice]) != 99)) )
            hulp_2_max_parameter_cic = atof(cic_tma_spectrum_2[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_2[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_2[par_indice_hwa], "//", 2) != 0) )
         hulp_2_max_parameter_cic = atof(hwa_tma_array_2[par_indice_hwa]) / SI_factor_hwa;

      /* + 3hr */
      if ( (strcmp(cic_tma_spectrum_3[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_3[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_3[par_indice]) != 99)) )
            hulp_3_max_parameter_cic = atof(cic_tma_spectrum_3[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_3[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_3[par_indice_hwa], "//", 2) != 0) )
         hulp_3_max_parameter_cic = atof(hwa_tma_array_3[par_indice_hwa]) / SI_factor_hwa;

      /* +4 hr */
      if ( (strcmp(cic_tma_spectrum_4[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_4[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_4[par_indice]) != 99)) )
            hulp_4_max_parameter_cic = atof(cic_tma_spectrum_4[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_4[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_4[par_indice_hwa], "//", 2) != 0) )
         hulp_4_max_parameter_cic = atof(hwa_tma_array_4[par_indice_hwa]) / SI_factor_hwa;

      /* +5 hr */
      if ( (strcmp(cic_tma_spectrum_5[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_5[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_5[par_indice]) != 99)) )
            hulp_5_max_parameter_cic = atof(cic_tma_spectrum_5[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_5[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_5[par_indice_hwa], "//", 2) != 0) )
         hulp_5_max_parameter_cic = atof(hwa_tma_array_5[par_indice_hwa]) / SI_factor_hwa;

      /* + 6 hr */
      if ( (strcmp(cic_tma_spectrum_6[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_6[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_6[par_indice]) != 99)) )
           hulp_6_max_parameter_cic = atof(cic_tma_spectrum_6[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_6[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_6[par_indice_hwa], "//", 2) != 0) )
         hulp_6_max_parameter_cic = atof(hwa_tma_array_6[par_indice_hwa]) / SI_factor_hwa;

      /* + 7 hr */
      if ( (strcmp(cic_tma_spectrum_7[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_7[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_7[par_indice]) != 99)) )
           hulp_7_max_parameter_cic = atof(cic_tma_spectrum_7[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_7[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_7[par_indice_hwa], "//", 2) != 0) )
         hulp_7_max_parameter_cic = atof(hwa_tma_array_7[par_indice_hwa]) / SI_factor_hwa;

      /* + 8 hr */
      if ( (strcmp(cic_tma_spectrum_8[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_8[par_indice]) > 0) )
      {
         if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_8[par_indice]) != 99)) )
           hulp_8_max_parameter_cic = atof(cic_tma_spectrum_8[par_indice]) / SI_factor;
      }
      else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                (strcmp(hwa_tma_array_8[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_8[par_indice_hwa], "//", 2) != 0) )
         hulp_8_max_parameter_cic = atof(hwa_tma_array_8[par_indice_hwa]) / SI_factor_hwa;

   } /* else */

   /*
   // van alle cic data (behorende bij +00, +01 en +02 hr etc. de hoogste bepalen)
   */
   *max_parameter_cic = hulp_0_max_parameter_cic;

   if (hulp_1_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_1_max_parameter_cic;
   if (hulp_2_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_2_max_parameter_cic;
   if (hulp_3_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_3_max_parameter_cic;
   if (hulp_4_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_4_max_parameter_cic;
   if (hulp_5_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_5_max_parameter_cic;
   if (hulp_6_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_6_max_parameter_cic;
   if (hulp_7_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_7_max_parameter_cic;
   if (hulp_8_max_parameter_cic > *max_parameter_cic)
      *max_parameter_cic = hulp_8_max_parameter_cic;


/**************  test ************/
/*
               sprintf(buffer, "%lf", *max_parameter_cic);
               fprintf(stderr, "%s", buffer);
               getchar();
*/
/**************** test ************/


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                       Bepaal_Max_Parameter_Model                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Parameter_Model(double* max_parameter_nedwam, const char* parameter)
{
   /* NB max_parameter_nedwam krijgt bij FF de waarde van hirlam (uba) */
   double parameter_nedwam;                           /* wordt gebruikt voor max nedwam en hirlam (uba) */
   int v;                                             /* teller verwachtingen */
   int start_indice;                                  /* begin indice juiste verwachting */
   int eind_indice;                                   /* eind indice juiste verwachting */
   /*char buffer[100];*/
   int par_indice;



   /* initialisatie */
   *max_parameter_nedwam = 0.0;
   start_indice = 0;
   eind_indice = NUM_TMA_VERWACHTINGEN;

   /* max_parameter_nedwam bepalen */
   if (strncmp(parameter, "HM0", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_HM0;
   else if (strncmp(parameter, "TM0", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_TM0_1;
   else if (strncmp(parameter, "HE3", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_E10;            /* HE3 = Hs10 en Hs10 = wortel(E10) * 4 */
   else if (strncmp(parameter, "DHE", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_DD_E10;         /* direction van E10 (= direction van Hs10) */
   else if (strncmp(parameter, "HS7", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_HS7;
   else if (strncmp(parameter, "_FF", 3) == 0)
      par_indice = UBA_PARAMETER_INDEX_FF;
   else if (strncmp(parameter, "HSW", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_HSW;
   else if (strncmp(parameter, "DSW", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_DSW;
   else if (strncmp(parameter, "PSW", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_PSW;



   /* max_parameter_nedwam bepalen */
   for (v = start_indice; v < eind_indice; v++)
   {
      if ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) ||
          (strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "HS7", 3) == 0) ||
          (strncmp(parameter, "HSW", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0) ||
          (strncmp(parameter, "PSW", 3) == 0) )
         parameter_nedwam = atof(TMA_Parameter_Array[par_indice][v]);

      /* HE3 omzetten naar Hs10 */
      else if (strncmp(parameter, "HE3", 3) == 0)      /* HE3 = Hs10 = wortel(E10) * 4 */
         parameter_nedwam = sqrt(atof(TMA_Parameter_Array[par_indice][v])) * 4.0;

      /* ff van m/s naar knots */
      else if (strncmp(parameter, "_FF", 3) == 0)
         parameter_nedwam = atof(UBA_Parameter_Array[par_indice][v]) * M_S_TO_KNOTS;    /* nu in knots */

      if (parameter_nedwam > *max_parameter_nedwam)
         *max_parameter_nedwam = parameter_nedwam;

   } /* for (v = start_indice; v < eind_indice; v++) */


/**************  test ************/
/*
                 buffer[0] = '\0';
                 sprintf(buffer, "%lf", *max_parameter_nedwam);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************** test ************/


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                        Bepaal_Scale_Y_Max_Parameter                       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Scale_Y_Max_Parameter(const double* max_parameter_nedwam_cic, const char* parameter, int* scale_y)
{
   /* NB scale_y: aantal PIXELS tussen 2 marks op y-as */
   const int limit_scale_y_Hm0_1 = 4;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_Hm0_2 = 8;              /* boven deze waarde andere schaalverdeling */

   const int limit_scale_y_TM0_1 = 8;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_TM0_2 = 16;             /* boven deze waarde andere schaalverdeling */

   const int limit_scale_y_ff_32 = 32;             /* boven deze waarde andere schaal verdeling */

   const int limit_scale_y_HE3_a = 1;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_HE3_0 = 2;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_HE3_1 = 4;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_HE3_2 = 8;              /* boven deze waarde andere schaalverdeling */

   const int limit_scale_y_HS7_a = 1;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_HS7_0 = 2;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_HS7_1 = 4;              /* boven deze waarde andere schaalverdeling */
   const int limit_scale_y_HS7_2 = 8;              /* boven deze waarde andere schaalverdeling */

   int of9 = 20;                                   /* vert. afstand marks op y-as (parameter units marks) */
   int of21 = 2 * of9;                             /* vert. afstand marks op y-as (parameter units marks) */
   int of22 = of9 / 2;                             /* vert. afstand marks op y-as (parameter units marks) */
   int of23 = 4 * of9;                             /* vert. afstand marks op y-as (parameter units marks) */
   int of24 = 8 * of9;                             /* vert. afstand marks op y-as (parametr units marks) */
   int of25 = of9 / 4;                             /* vert. afstand marks op y-as (parametr units marks) */


   if ( (strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "HSW", 3) == 0) )
   {
      if (*max_parameter_nedwam_cic < (double)limit_scale_y_Hm0_1)  /* b.v. 0 - 4 meter */
         *scale_y = of21;                                       /* b.v. 3 meter -> 3 * scale_y [ps_eenheden] t.o.v. y1 */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_Hm0_2)  /* b.v. 0 - 8 meter */
         *scale_y = of9;
      else                                                      /* b.v 0 - 16 meter */
         *scale_y = of22;
   } /* if (strncmp(parameter, "HM0", 3) == 0) */
   else if ( (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "PSW", 3) == 0) )
   {
      if (*max_parameter_nedwam_cic < (double)limit_scale_y_TM0_1)
         *scale_y = of9;                                        /* 0 - 8 sec */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_TM0_2)
         *scale_y = of22;                                       /* 0 - 16 sec */
      else
         *scale_y = of25;                                       /* 0 - 32 sec */
   } /* else if (strncmp(parameter, "TM0", 3) == 0) */
   else if  (strncmp(parameter, "HE3", 3) == 0)                 /* HE3 = Hs10 */
   {
      if (*max_parameter_nedwam_cic < (double)limit_scale_y_HE3_a)  /* b.v. 0 - 1 meter */
         *scale_y = of24;                                       /* b.v. 3 meter -> 3 * scale_y [ps_eenheden] t.o.v. y1 */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_HE3_0)  /* b.v. 0 - 2 meter */
         *scale_y = of23;                                       /* b.v. 3 meter -> 3 * scale_y [ps_eenheden] t.o.v. y1 */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_HE3_1)  /* b.v. 0 - 4 meter */
         *scale_y = of21;                                       /* b.v. 3 meter -> 3 * scale_y [ps_eenheden] t.o.v. y1 */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_HE3_2)  /* b.v. 0 - 8 meter */
         *scale_y = of9;
      else                                                      /* b.v 0 - 16 meter */
         *scale_y = of22;
   } /* else if  (strncmp(parameter, "HE3", 3) == 0) */
   else if  (strncmp(parameter, "HS7", 3) == 0)
   {
      if (*max_parameter_nedwam_cic < (double)limit_scale_y_HS7_a)  /* b.v. 0 - 1 meter */
         *scale_y = of24;                                       /* b.v. 3 meter -> 3 * scale_y [ps_eenheden] t.o.v. y1 */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_HS7_0)  /* b.v. 0 - 2 meter */
         *scale_y = of23;                                       /* b.v. 3 meter -> 3 * scale_y [ps_eenheden] t.o.v. y1 */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_HS7_1)  /* b.v. 0 - 4 meter */
         *scale_y = of21;                                       /* b.v. 3 meter -> 3 * scale_y [ps_eenheden] t.o.v. y1 */
      else if (*max_parameter_nedwam_cic < (double)limit_scale_y_HS7_2)  /* b.v. 0 - 8 meter */
         *scale_y = of9;
      else                                                      /* b.v 0 - 16 meter */
         *scale_y = of22;
   } /* else if  (strncmp(parameter, "HE3", 3) == 0) */
   else if (strncmp(parameter, "_FF", 3) == 0)
   {
      if (*max_parameter_nedwam_cic < (double)limit_scale_y_ff_32)
         *scale_y = of9;                                        /* 0 - 32 knots */
      else
         *scale_y = of22;                                       /* 0- 64 knots */
   } /* else if (strncmp(parameter, "_FF", 3) == 0) */
   else if ( (strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0) )/* NB direction of E10 (is ook direction of Hs10) */
   {
      *scale_y = of9;                                           /* b.v. 45 graden = 1 * scale_y [ps_eenheden} */
   } /* else if (strncmp(parameter, "DHE", 3) == 0) */
   else
      *scale_y = 1;                                             /* NB: i.v.m. run-time error *scale_y niet 0 maken */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                    Bepaal_TMA_Outputfilenaam_Per_Locatie                  */
/*                                                                           */
/*****************************************************************************/
int Bepaal_TMA_Outputfilenaam_Per_Locatie(const char* station, const int tma_pagina_no)
{
   /* NB nedwam_par_outputfilenaam (inclusief sub. dir.) bv.: output_nedwam/PARAMETERS_K13.PS */
   /* NB hier zit dus de ff van hirlam-uba bij inbegrepen */

   /* initialisatie */
   strcpy(tma_outputfilenaam, "\0");

   if (strcmp(OS, "WINDOWS") == 0)                 /* WINDOWS */
      strcpy(tma_outputfilenaam, "output_nedwam\\");
   else                                            /* UNIX */
      strcpy(tma_outputfilenaam, "output_nedwam/");

   if (tma_pagina_no == 0)
      strcat(tma_outputfilenaam, "TMA_");
   else if (tma_pagina_no == 1)
      strcat(tma_outputfilenaam, "TMA_b_");
   else if (tma_pagina_no == 2)
      strcat(tma_outputfilenaam, "TMA_c_");


   strcat(tma_outputfilenaam, station);
   strcat(tma_outputfilenaam, ".PS");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                       Write_Postscript_Par_Raster_Box                     */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_Par_Raster_Box(const char* grafiek_orientatie, const int scale_y, const double scale_x, const char* parameter, const char* model, const char* nedwam_par_title, int* y_interval)
{

   /*
   //          (x1,y2)                            (x2,y2)
   //              *--------------------------------*
   //              |                                |
   //              |                                |
   //              |              BOX               |
   //              |                                |
   //              *--------------------------------*
   //          (x1,y1)                            (x2,y1)
   //
   */

   const int mark_lengte = 6;
   const int of5_par = 16;                                      /* uur offset tot x-as om uur te zetten */
   const int of6_par = 8;                                       /* uur offset rel. hor afstand tot marks */
   int hor_afstand;                                             /* afstand linkse as tot indicatie (bv. 4 meter) */
   const int hor_afstand_rechts = 12;                           /* afstand rechtse as tot indicatie (bv. 4 meter) */
   const int of11_par = 3;                                      /* vert. ofset unit (bv. metre) aanduiding y-as  */
   const int of40_par = lengte_y_as_par / 2 - 15;               /* vert. offset t.o.v. _y1 voor y-as aanduiding (b.v. metre) */
   const int of41_par = 30;                                     /* hort. offset t.o.v. x1 voor y-as aanduiding (b.v. metre) */
   const int of_x_unit = 5;                                     /* rel. afstand voor x-as unit aanduiding (b.v. + 48 hr) */
   const int of_hor_title = 125;                                /* rel. afstand vanaf x1_par om title te zetten */
   const int of_ver_title = 25;                                 /* rel. afstand vanaf y2_par om title te zetten */
   int ofi_par;                                                 /* "lopende" afstand tot y-as */
   int i;                                                       /* teller */
   char line[256];
   char buffer1[10];
   char buffer2[10];
   /*const int aantal_forecast_marks = 17;*/                        /* aantal marks langs x-as (bv. om de 3 uur) */
   /*int aantal_forecast_marks;*/
   const int tijdverschil_forecast_marks = 3;                   /* aantal uren wat tussen 2 marks op x-as zit */
#if 0
   int hulp_graden;
#endif

   /* initialisatie */
   /*aantal_forecast_marks = (NUM_TMA_VCERWACHTINGEN - 1) / tijdverschil_forecast_marks + 1;  /* (49 - 1) / 3  + 1= 17 */

   /* initialisatie [LET OP: wordt later ook gebruikt in function: Write_Nedwam_Par_Legenda()] */
   x2_par = x1_par + lengte_x_as_par;                           /* main parameter box */
   y2_par = _y1_par + lengte_y_as_par;                          /* main parameter box */



   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
   {
      /* basis waarden aanhouden */
   }
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
   {
      /* translate */
      strcpy(line, "\0");
      strcpy(line, "0 264 translate    %middelste grafiek\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);
   }
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
   {
      /* translate */
      strcpy(line, "\0");
      strcpy(line, "0 264 translate    %bovenste grafiek\n"); /* nb t.o.v. laatste translate */
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);
   }



   /***********************************  test ************************/
   /*
                     fprintf(stderr, "%s", grafiek_orientatie);
                     getchar();
   */
   /************************************ test ************************/


   /*
   ///////////// color x-y box  //////////////
   */

   /* newpath (color x-y box) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* color (color x-y box) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_ZEE);
   strcat(line, " setrgbcolor %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* moveto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par);
   sprintf(buffer2, "%d", _y1_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2_par);
   sprintf(buffer2, "%d", _y1_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2_par);
   sprintf(buffer2, "%d", y2_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par);
   sprintf(buffer2, "%d", y2_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* closepath (color x-y box) */
   strcpy(line, "\0");
   strcpy(line, "closepath      %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* fill (color x-y box) */
   strcpy(line, "\0");
   strcpy(line, "fill           %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   /////////// font zetten box ////////////
   */

   /* newpath (kader font) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %bottom kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* font (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, font_keuze);
   fprintf(os_ps, "%s", line);

   /* font (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, "10 scalefont       %kader font x-y box\n");
   fprintf(os_ps, "%s", line);

   /* font (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, "setfont            %kader font x-y box\n");
   fprintf(os_ps, "%s", line);

   /* setlinewidth (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, "0.2 setlinewidth   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   ////////////// kader (contouren) tekenen box //////////////
   */

   /* newpath (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* color (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* moveto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par);
   sprintf(buffer2, "%d", _y1_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2_par);
   sprintf(buffer2, "%d", _y1_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2_par);
   sprintf(buffer2, "%d", y2_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par);
   sprintf(buffer2, "%d", y2_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* closepath (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, "closepath      %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* stroke (kader x-y box) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   /////// x-as indeling box /////////
   */

   /* marks color (x-y box) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %color marks\n");
   fprintf(os_ps, "%s", line);

   /*
   //
   //          10101010
   //  (x1,y1) *----------------------------
   //          |   |   |   |   |     } 5
   //          |   |   |   |   |     } 5
   //                                } 5
   //        '+03 +06'               } 5
   //
   //
   */


   /*for (i = 0; i < aantal_forecast_marks; i++)*/                   /* loop over aantal verwachtingen b.v. van 0 - +48hr */
   for (i = 0; i < NUM_TMA_VERWACHTINGEN; i++)
   {
      /*if ((i % tijdverschil_forecast_marks) == 0)*/                     /* niet bij alle forecast uren een mark met aanduiding zetten */
      /*{  */
      ofi_par = (int)((double)i * scale_x + 0.5);                 /* "lopende" afstand vanaf y-as (i.v.m. afronden double naar int) */

      /* moveto (mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1_par + ofi_par);
      sprintf(buffer2, "%d", _y1_par);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %+hr-mark\n");
      fprintf(os_ps, "%s", line);

      /* lineto (mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1_par + ofi_par);
      sprintf(buffer2, "%d", _y1_par - mark_lengte);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %+hr-mark\n");
      fprintf(os_ps, "%s", line);

      /* stroke (mark) */
      strcpy(line, "\0");
      strcpy(line, "stroke         %+hr-mark\n");
      fprintf(os_ps, "%s", line);

      if ((i % tijdverschil_forecast_marks) == 0)                     /* niet bij alle forecast uren een mark met aanduiding zetten */
      {
         /* moveto (+hr aanduiding) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", x1_par + ofi_par - of6_par);
         sprintf(buffer2, "%d", _y1_par - of5_par);
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " moveto   %+hr\n");
         fprintf(os_ps, "%s", line);

         /* show (+hr aanduiding) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         sprintf(buffer1, "%02d", i);
         strcpy(line, "(");
         strcat(line, "+");
         strcat(line, buffer1);
         strcat(line, ")");
         strcat(line, " show      %+hr\n");
         fprintf(os_ps, "%s", line);
      } /* if ((i % tijdverschil_forecast_mark) == 0) */
   } /* for (i = 0; i <= aantal_forecast_marks; i++) */

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   /////// x-as unit aanduiding (bv. hr) /////////
   */

   /* moveto (x-as unit aanduiding) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2_par + of6_par + of_x_unit);
   sprintf(buffer2, "%d", _y1_par - of5_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto     %x-as unit aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* show (x-as unit aanduiding) */
   strcpy(line, "\0");
   strcat(line, "(hr) show    %x-as unit aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   /////// linkse y-as indeling box /////////
   */

   if (strncmp(parameter, "_FF", 3) == 0)
      *y_interval = 4;                                  /* aantal knots tussen 2 marks op y-as */
   else if ((strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0))   /* richtingen */
      *y_interval = 45;                                 /* aantal graden tussen 2 marks op y -as */
   else
      *y_interval = 1;                                  /* aantal metres of seconds tussen 2 marks op y-as */


   /* bij graden op linkse y-as iets verder vanaf de as beginnen */
   if ((strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0))   /* directions */
      hor_afstand = 25;
   else
      hor_afstand = 20;


   for (i = 0; i <= lengte_y_as_par / scale_y; i++)     /* b.v. scale_y = 10, 20, 40 en lengte_y_as = 160 */
   {
      /* linkse-as moveto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1_par);
      sprintf(buffer2, "%d", _y1_par + (i * scale_y));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark links\n");
      fprintf(os_ps, "%s", line);

      /* linkse-as lineto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1_par - mark_lengte);
      sprintf(buffer2, "%d", _y1_par + (i * scale_y));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %par-mark links\n");
      fprintf(os_ps, "%s", line);

      /* linkse-as stroke (par. mark) */
      strcpy(line, "\0");
      strcpy(line, "stroke         %par-mark links\n");
      fprintf(os_ps, "%s", line);

      /* linkse-as moveto */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1_par - hor_afstand);
      sprintf(buffer2, "%d", _y1_par + (i * scale_y) - of11_par);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark links\n");
      fprintf(os_ps, "%s", line);

      /* linkse-as show */
      strcpy(line, "\0");
      buffer1[0] = '\0';

#if 0
      if ((strncmp(parameter, "DHE", 3) == 0)  || (strncmp(parameter, "DSW", 3) == 0))
      {
         /* omdat meestal uit het noorden, alles met 180 graden verschuiven (N dan in midden grafiek) */
         hulp_graden = 180 + (i * (*y_interval));
         if (hulp_graden > 360)
            hulp_graden -= 360;

         sprintf(buffer1, "%d", hulp_graden);
      }
      else /* geen richting */
      {
#endif
         /* bij TM0-1 en period swell bij schaal 0 - 32 sec (scale_y = 5) niet bij elk streepje een aantal sec zetten (te druk) */
         if ( ((strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "PSW", 3) == 0)) && (scale_y == 5) )
         {
            if ((i % 2) == 0)                              /* dus alleen bij even aantal een sec indicatie zetten */
               sprintf(buffer1, "%d", i * (*y_interval));
            else
               buffer1[0] = '\0';
         }
         else
            sprintf(buffer1, "%d", i * (*y_interval));
#if 0
      } /* else (geen richting) */
#endif
      strcpy(line, "(");
      strcat(line, buffer1);
      strcat(line, ")");
      strcat(line, " show      %par-mark links\n");
      fprintf(os_ps, "%s", line);
   } /* for (i = 0; i <= lengte_y_as / scale_y; i++)  */

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   /////// rechtse y-as indeling box /////////
   */

   for (i = 0; i <= lengte_y_as_par / scale_y; i++)         /* b.v. scale_y = 10, 20, 40 en lengte_y_as = 160 */
   {
      /* rechtse-as moveto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x2_par);
      sprintf(buffer2, "%d", _y1_par + (i * scale_y));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark rehts\n");
      fprintf(os_ps, "%s", line);

      /* rechtse-as lineto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x2_par + mark_lengte);
      sprintf(buffer2, "%d", _y1_par + (i * scale_y));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %par-mark rechts\n");
      fprintf(os_ps, "%s", line);

      /* rechts-as stroke (par. mark) */
      strcpy(line, "\0");
      strcpy(line, "stroke         %par-mark rechts\n");
      fprintf(os_ps, "%s", line);

      /* rechtse-as moveto */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x2_par + hor_afstand_rechts);
      sprintf(buffer2, "%d", _y1_par + (i * scale_y) - of11_par);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark rechts\n");
      fprintf(os_ps, "%s", line);

      /* rechtse-as show */
      strcpy(line, "\0");
      buffer1[0] = '\0';
#if 0
      if ((strncmp(parameter, "DHE", 3) == 0)  || (strncmp(parameter, "DSW", 3) == 0))
      {
         /* omdat meestal uit het noorden, alles met 180 graden verschuiven (N dan in midden grafiek) */
         hulp_graden = 180 + (i * (*y_interval));
         if (hulp_graden > 360)
            hulp_graden -= 360;

         sprintf(buffer1, "%d", hulp_graden);
      }
      else
#endif      
         sprintf(buffer1, "%d", i * (*y_interval));

      strcpy(line, "(");
      strcat(line, buffer1);
      strcat(line, ")");
      strcat(line, " show      %par-mark rechts\n");
      fprintf(os_ps, "%s", line);
   } /* for (i = 0; i <= lengte_y_as / scale_y; i++) */

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   /////// linkse y-as unit aanduiding (bv. metre) /////////
   */

   /* moveto (y-as unit aanduiding) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par - of41_par);
   sprintf(buffer2, "%d", _y1_par + of40_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto     %y-as unit aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* rotate (y-as aanduiding) */
   strcpy(line, "\0");
   strcat(line, "90 rotate        %y-as unit aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* show (y-as aanduiding) */
   strcpy(line, "\0");
   if (strncmp(parameter, "HM0", 3) == 0)
      strcat(line, "(metres) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "TM0", 3) == 0)
      strcat(line, "(seconds) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "HE3", 3) == 0)
      strcat(line, "(metres) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "_FF", 3) == 0)
      strcat(line, "(knots) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "DHE", 3) == 0)
      strcat(line, "(degrees) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "HS7", 3) == 0)
      strcat(line, "(metres) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "HSW", 3) == 0)
      strcat(line, "(metres) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "PSW", 3) == 0)
      strcat(line, "(seconds) show    %y-as unit aanduiding\n");
   else if (strncmp(parameter, "DSW", 3) == 0)
      strcat(line, "(degrees) show    %y-as unit aanduiding\n");
   else
       strcat(line, "(unknown) show    %y-as unit aanduiding\n");

   fprintf(os_ps, "%s", line);


   /* rotate (y-as aanduiding) [weer terugzetten] */
   strcpy(line, "\0");
   strcat(line, "-90 rotate       %y-as unit aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   ////////////// legenda ////////////////
   */
   Write_Nedwam_Par_Legenda((char*)model);


   /*
   /////////////// title ////////////////
   */

   /* newpath (title) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %title\n");
   fprintf(os_ps, "%s", line);

   /* font (title) */
   strcpy(line, "\0");
   strcpy(line, font_keuze);
   fprintf(os_ps, "%s", line);

   /* font (title) */
   strcpy(line, "\0");
   strcpy(line, "14 scalefont       %font title\n");
   fprintf(os_ps, "%s", line);

   /* font (title) */
   strcpy(line, "\0");
   strcpy(line, "setfont            %font title\n");
   fprintf(os_ps, "%s", line);

   /* color (title) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_title_ZEE);
   strcat(line, " setrgbcolor %color title\n");
   fprintf(os_ps, "%s", line);

   /* moveto (title) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of_hor_title);
   sprintf(buffer2, "%d", y2_par + of_ver_title);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %title\n");
   fprintf(os_ps, "%s", line);

   /* show (title) */
   strcpy(line, "\0");
   strcpy(line, "(");
   strcat(line, nedwam_par_title);
   strcat(line, ")");
   strcat(line, " show      %title\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                         Write_Nedwam_Par_Legenda                          */
/*                                                                           */
/*****************************************************************************/
int Write_Nedwam_Par_Legenda(const char* model)
{
   /* n.b stream (os_ps) is geopend en wordt gesloten door aanroepende function */

   /* n.b x1_par en _y1_par zijn globaal in deze module + globaal geinitialiseerd */
   /* n.b x2_par en y2_par zijn globaal in deze module + geinitialiseerd in function: Write_Postscript_Par_Raster_Box()*/


   const int of14 = 11;                        /* ofset legenda kader t.o.v. y2 */
   const int of15 = 165;                       /* ofset legenda kader t.o.v. x1 */
   const int of16 = 6;                         /* ofset legenda tekst model t.o.v. y2 */
   const int of17 = of16;                      /* ofset legenda tekst obs t.o.v. y2 */
   const int of18 = 10;                        /* ofset legenda tekst t.o.v. x1 (bij meerdaagse alleen model) */
   const int of19 = 30;                        /* (sub) ofset legenda x richting (tussen - en tekst) */
   const int of20 = 3;                         /* (sub) ofset legenda y richting (tekst zakken t.o.v. -) */
   const int of21 = 100;                       /* bij model: ofset legenda tekst obs t.o.v. x1 */
   const int of_hook = 4;                      /* bij model: (geeft mooie geschuinde hoek) */
   char line[256];
   char buffer1[256];
   char buffer2[256];



   /*
   //
   //                                                   _____ of_hook
   //                                                  |
   //                  ______________________________| V_|_ _ _ _
   //                /         ____ of19_____        \       } of_hook
   //              /          |              |         \  _ _} _ _ _
   //             |           V              V           |   }
   //             |        --- model      --- obs        |   } of14
   //      of16 { |______________________________________|___}____________ y2
   //             |<-------------- of15 ---------------->|
   //             |<-of18->|
   //             |<------- of21 -------->|
   //             |
   //             |
   //             x1
   //
   */



   /* newpath (legenda) */
   strcpy(line, "\0");
   strcpy(line, "newpath            %legenda\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda kader) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of15);
   sprintf(buffer2, "%d", y2_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of15);
   sprintf(buffer2, "%d", y2_par + of14 - of_hook);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of15 - of_hook);
   sprintf(buffer2, "%d", y2_par + of14);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of_hook);
   sprintf(buffer2, "%d", y2_par + of14);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par);
   sprintf(buffer2, "%d", y2_par + of14 - of_hook);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par);
   sprintf(buffer2, "%d", y2_par);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* stroke (legenda kader) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* color (- legenda text model) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_model_parameter);
   strcat(line, " setrgbcolor %legenda text model\n");
   fprintf(os_ps, "%s", line);

   /* moveto (- legenda text model) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of18);
   sprintf(buffer2, "%d", y2_par + of16);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %legenda text model\n");
   fprintf(os_ps, "%s", line);

   /* show (- legenda text model) */
   strcpy(line, "\0");
   strcat(line, "( __) show    %legenda text model\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text model) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda text model\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda text model) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of18 + of19);
   sprintf(buffer2, "%d", y2_par + of16 - of20);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %legenda text model\n");
   fprintf(os_ps, "%s", line);

   /* show (legenda text model) */
   strcpy(line, "\0");
   if (strncmp(model, "NEDWAM", 6) == 0)
      strcat(line, "(nedwam) show    %legenda text model\n");
   else if (strncmp(model, "HIRLAM", 6) == 0)
      strcat(line, "(hirlam22) show    %legenda text model\n");
   else
      strcat(line, "(unknown) show    %legenda text model\n");
   fprintf(os_ps, "%s", line);

   /* color (- legenda text obs) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
   strcat(line, " setrgbcolor %legenda text obs\n");
   fprintf(os_ps, "%s", line);

   /* moveto (- legenda text obs) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of21);
   sprintf(buffer2, "%d", y2_par + of17);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %legenda text obs\n");
   fprintf(os_ps, "%s", line);

   /* show (- legenda text obs) */
   strcpy(line, "\0");
   strcat(line, "( __) show    %legenda text obs\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text obs) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda text obs\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda text obs) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1_par + of19 + of21);
   sprintf(buffer2, "%d", y2_par + of17 - of20);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %legenda text obs\n");
   fprintf(os_ps, "%s", line);

   /* show (legenda text obs) */
   strcpy(line, "\0");
   strcat(line, "(obs) show    %legenda text obs\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                      Bepaal_Title_Par_Nedwam_Per_Grafiek                  */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Title_Par_Nedwam_Per_Grafiek(char* nedwam_par_title, const char* station, const char* parameter)
{
   struct tm time_valid;
   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   int tijd_ok;
   char run_time[100];                    /* NB 100 = willekeurige groot-genoeg waarde */


   /* initialisatie */
   nedwam_par_title[0]              = '\0';


   /* jaar van argument list */
   strncpy(JJJJ, JJJJMMDDUU, 4);              /* nb bv: JJJJJMMDDUU: 2001093018            */
   JJJJ[4] = '\0';
   num_JJJJ = atoi(JJJJ);

   /* maand van argument list */
   strncpy(MM, JJJJMMDDUU + 4, 2);
   MM[2] = '\0';
   num_MM = atoi(MM);

   /* dag van argument list */
   strncpy(DD, JJJJMMDDUU + 6, 2);
   DD[2] = '\0';
   num_DD = atoi(DD);

   /* uur van argument list */
   strncpy(UU, JJJJMMDDUU + 8, 2);
   UU[2] = '\0';
   num_UU = atoi(UU);

   /*
   // runtime b.v. 2001093018
   */
   time_valid.tm_year = num_JJJJ - 1900;
   time_valid.tm_mon  = num_MM - 1;
   time_valid.tm_mday = num_DD;
   time_valid.tm_hour = num_UU;
   time_valid.tm_min = 0;
   time_valid.tm_sec = 0;
   time_valid.tm_isdst = -1;                     /* daylight saving time: -1 = no info available */

   /* 'RUN' datum aanmaken */
   tijd_ok = 1;
   if (mktime(&time_valid) == -1)
      tijd_ok = 0;


   if (tijd_ok == 1)
   {
      (void)strftime(run_time, 100 /*sizeof(date_time)*/, "%d-%m-%Y %H.00 UTC", &time_valid);

      strcpy(nedwam_par_title, "");
      strcpy(nedwam_par_title, "RUN ");
      strcat(nedwam_par_title, run_time);                  /* b.v. 30-09-2001 18.00 UTC */
      strcat(nedwam_par_title, " ");
      strcat(nedwam_par_title, station);                   /* b.v. ANA */
      strcat(nedwam_par_title, " ");

      if (strncmp(parameter, "HM0", 3) == 0)
         strcat(nedwam_par_title, "HM0");
      else if (strncmp(parameter, "TM0", 3) == 0)
         strcat(nedwam_par_title, "TM0-1");
      else if (strncmp(parameter, "HE3", 3) == 0)          /* HE3 = Hs10 */
         strcat(nedwam_par_title, "Hs10");
      else if (strncmp(parameter, "_FF", 3) == 0)
         strcat(nedwam_par_title, "FF");
      else if (strncmp(parameter, "DHE", 3) == 0)
         strcat(nedwam_par_title, "dir Hs10");
      else if (strncmp(parameter, "HS7", 3) == 0)
         strcat(nedwam_par_title, "Hs7");
      else if (strncmp(parameter, "HSW", 3) == 0)
         strcat(nedwam_par_title, "height swell");
      else if (strncmp(parameter, "PSW", 3) == 0)
         strcat(nedwam_par_title, "period swell");
      else if (strncmp(parameter, "DSW", 3) == 0)
         strcat(nedwam_par_title, "dir swell");
      else
         strcat(nedwam_par_title, "unknown");

   } /* if (tijd_ok == 1) */
   else
      strcpy(nedwam_par_title, "unknown");


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                            Nedwam_Par_To_Postscript                       */
/*                                                                           */
/*****************************************************************************/
int Nedwam_Par_To_Postscript(const int scale_x, const int scale_y, const int y_interval, const char* parameter)
{
   double model_value;
   double model_value_new;
   double model_value_old;
   double obs_value;
   double obs_value_new;
   double obs_value_old;
   double hulp_model_value_new;                     /* hulp var bij richtingen */
   double hulp_model_value_old;                     /* hulp var bij richtingen */
   double hulp_obs_value_new;                       /* hulp var bij richtingen */
   double hulp_obs_value_old;                       /* hulp var bij richtingen */
   int y_pos_model;
   int y_pos_obs;
   char line[256];
   int v;                                           /* teller forecasts */
   char buffer1[10];
   char buffer2[10];
   char buffer3[10];
   int start_indice;
   int eind_indice;
   int par_indice;
   int par_indice_hwa;
   int SI_factor;
   double SI_factor_hwa;
   const int straal_dot = 2;
   const float max_richtings_shift = 120.0;        /* bij richtingen tussen twee model of obs waarden */


   /*
   // model (nedwam HM0, TM0-1, Hs10 etc. of hirlam ff)
   */

   /* newpath (model line) */
   strcpy(line, "\0");
   strcpy(line, "newpath            %model line\n");
   fprintf(os_ps, "%s", line);

   /* color (model line) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_model_parameter);
   strcat(line, " setrgbcolor %model line\n");
   fprintf(os_ps, "%s", line);


   /* initialisatie */
   model_value_old = MAXFLOAT;
   start_indice = 0;
   eind_indice = NUM_TMA_VERWACHTINGEN;


   /* parameter indice instellen */
   if (strncmp(parameter, "HM0", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_HM0;
   else if (strncmp(parameter, "TM0", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_TM0_1;
   else if (strncmp(parameter, "HE3", 3) == 0)                    /* HE3 = Hs10 = wortel(E10) * 4 */
      par_indice = TMA_PARAMETER_INDEX_E10;
   else if (strncmp(parameter, "DHE", 3) == 0)                    /* direction E10 (is gelijk ook richting Hs10) */
      par_indice = TMA_PARAMETER_INDEX_DD_E10;
   else if (strncmp(parameter, "HS7", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_HS7;
   else if (strncmp(parameter, "_FF", 3) == 0)
      par_indice = UBA_PARAMETER_INDEX_FF;
   else if (strncmp(parameter, "HSW", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_HSW;
   else if (strncmp(parameter, "PSW", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_PSW;
   else if (strncmp(parameter, "DSW", 3) == 0)
      par_indice = TMA_PARAMETER_INDEX_DSW;


   for (v = start_indice; v < eind_indice; v++)
   {
      if ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) ||
          (strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "HS7", 3) == 0) ||
          (strncmp(parameter, "HSW", 3) == 0) || (strncmp(parameter, "PSW", 3) == 0) ||
          (strncmp(parameter, "DSW", 3) == 0) )
      {
         if (strcmp(TMA_Parameter_Array[par_indice][v], "\0") == 0)          /* lege array plaats */
            model_value = MAXFLOAT;
         else
            model_value = atof(TMA_Parameter_Array[par_indice][v]);
      } /* if ( (strncmp(parameter, "HM0", 3) == 0) || etc. */

      /* HE3 omzetten naar Hs10 */
      else if (strncmp(parameter, "HE3", 3) == 0)                            /* HE3 = Hs10 = wortel(E10) * 4 */
      {
         if (strcmp(TMA_Parameter_Array[par_indice][v], "\0") == 0)          /* lege array plaats */
            model_value = MAXFLOAT;
         else
            model_value = sqrt(atof(TMA_Parameter_Array[par_indice][v])) * 4.0;   /* Hs10 = wortel(E10) * 4 */
      } /* else if (strncmp(parameter, "HE3", 3) == 0) */

      /* ff omzetten van m/s naar knopen */
      else if (strncmp(parameter, "_FF", 3) == 0)
      {
         if (strcmp(UBA_Parameter_Array[par_indice][v], "\0") == 0)          /* lege array plaats */
            model_value = MAXFLOAT;
         else
            model_value = atof(UBA_Parameter_Array[par_indice][v]) * M_S_TO_KNOTS;  /* nu in knots */
      } /* else if (strncmp(parameter, "_FF", 3) == 0) */



      /* > 200 meter, sec, kn, graden is altijd fout (en alleen ff kan echt 0 zijn) */
      if (model_value > 360.0)
         model_value = MAXFLOAT;


      if (model_value != MAXFLOAT)
      {
#if 0
         if ( (strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0) )
         {
            /* i.v.m schaalverdeling van 180 - 360 - 180 */
            if (model_value >= 180.0)
               y_pos_model = (int)(_y1_par + ((model_value - 180) * (double)scale_y / (double)y_interval) + 0.5);
            else
               y_pos_model = (int)(_y1_par + ((model_value + 180) * (double)scale_y / (double)y_interval) + 0.5);
         }
         else
#endif
            y_pos_model = (int)(_y1_par + (model_value * (double)scale_y / (double)y_interval) + 0.5);

         model_value_new = model_value;

         if ( (strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0) )
         {
#if 0
            if (model_value_new >= 0 && model_value_new < 180)
               hulp_model_value_new = model_value_new + 360;
            else
#endif
               hulp_model_value_new = model_value_new;
#if 0
            if (model_value_old >= 0 && model_value_old < 180)
               hulp_model_value_old = model_value_old + 360;
            else
#endif            
               hulp_model_value_old = model_value_old;
         } /* if (strncmp(parameter, "DHE", 3) == 0) */


/*
         if ( ((model_value_old != MAXFLOAT) && ((strncmp(parameter, "DHE", 3) != 0) || (strncmp(parameter, "DSW", 3) != 0))) ||
              ((model_value_old != MAXFLOAT) && ((strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0)) && (fabs(hulp_model_value_new - hulp_model_value_old) < max_richtings_shift)) )
*/
         if ( ((model_value_old != MAXFLOAT) && (strncmp(parameter, "DHE", 3) != 0) && (strncmp(parameter, "DSW", 3) != 0)) ||
              ((model_value_old != MAXFLOAT) && (strncmp(parameter, "DHE", 3) == 0) && (fabs(hulp_model_value_new - hulp_model_value_old) < max_richtings_shift)) ||
              ((model_value_old != MAXFLOAT) && (strncmp(parameter, "DSW", 3) == 0) && (fabs(hulp_model_value_new - hulp_model_value_old) < max_richtings_shift)) )

         {
            /* lineto (model line) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1_par + (v * scale_x));
            sprintf(buffer2, "%d", y_pos_model);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " lineto   %model line\n");
            fprintf(os_ps, "%s", line);

            /* stroke (model line) */
            strcpy(line, "\0");
            strcpy(line, "stroke         %model line\n");
            fprintf(os_ps, "%s", line);

            /* moveto (model line) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1_par + (v * scale_x));
            sprintf(buffer2, "%d", y_pos_model);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %model line\n");
            fprintf(os_ps, "%s", line);

         } /* if ( (model_value_old != MAXFLOAT) || etc. */
         else /* model_value_old == MAXFLOAT */
         {
             /* moveto (model line) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1_par + (v * scale_x));
            sprintf(buffer2, "%d", y_pos_model);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %model line\n");
            fprintf(os_ps, "%s", line);

         } /* else (model_value_old == MAXINT) */

         model_value_old = model_value_new;

      } /* if (model_value != MAXFLOAT */
      else
      {
         model_value_old = MAXFLOAT;
      }

   } /* for (v = start_indice; v <= eind_indice; v++) */



   /*
   // obs - line (CIC HM0, TM0-1 etc. en hirlamFF)
   */

   /* newpath (obs line) */
   strcpy(line, "\0");
   strcpy(line, "newpath            %obs line\n");
   fprintf(os_ps, "%s", line);

   /* color (obs line) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
   strcat(line, " setrgbcolor %obs line\n");
   fprintf(os_ps, "%s", line);

   /* initialisatie */
   obs_value_old = MAXFLOAT;
   start_indice = 0;
   eind_indice = aantal_cic_tma_temp_files;                /* zoveel cic temp files openen */


   /* afh. van de parameter instellingen maken */
   if (strncmp(parameter, "HM0", 3) == 0)
   {
      par_indice = HM0_SPECTRUM;
      SI_factor = 1000;                                    /* bv. inhoud array delen door 1000 om meter te krijgen */

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = HM0_HWA_INDEX;                      /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1;
   }
   else if (strncmp(parameter, "TM0", 3) == 0)
   {
      par_indice = TM0_1_SPECTRUM;
      SI_factor = 10;

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = TM0_HWA_INDEX;                      /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1;
   }
   else if (strncmp(parameter, "HE3", 3) == 0)             /* HE3 = Hs10 */
   {
      par_indice = HE3_SPECTRUM;
      SI_factor = 1000;
   }
   else if (strncmp(parameter, "DHE", 3) == 0)             /* direction E10 (is ook gelijk richting Hs10) */
   {
      par_indice = DD_GEM_LFE_SPECTRUM;
      SI_factor = 1;
   }
   else if (strncmp(parameter, "HS7", 3) == 0)
   {
      par_indice = HS7_SPECTRUM;
      SI_factor = 1000;
   }
   else if (strncmp(parameter, "_FF", 3) == 0)
   {
      par_indice = FF_WIND;
      SI_factor = 1;

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = FF_HWA_INDEX;                      /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1 / M_S_TO_KNOTS;
   }
   else if (strncmp(parameter, "HSW", 3) == 0)
   {
      par_indice = HSW_CID_INDEX;
      SI_factor = 100;                                    /* inhoud array delen door 100 om meter te krijgen */
   }
   else if (strncmp(parameter, "PSW", 3) == 0)
   {
      par_indice = PSW_CID_INDEX;
      SI_factor = 10;                                     /* inhoud array delen door 10 om sec te krijgen */
   }
   else if (strncmp(parameter, "DSW", 3) == 0)
   {
      par_indice = DSW_CID_INDEX;
      SI_factor = 1;
   }



   /* obs value bepalen */
   for (v = start_indice; v < eind_indice; v++)
   {
      /* initialisatie */
      obs_value = MAXFLOAT;

      /* nb bij cid (swell data) wordt gewerkt met cid_array[] en de andere cic data met cic_tma_spectrum[] */
      if ( (strncmp(parameter, "HSW", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0) || (strncmp(parameter, "PSW", 3) == 0) )
      {
         if (v == 0)
         {
            if ( (strcmp(cid_tma_array_0[par_indice], "\0") != 0) && (atoi(cid_tma_array_0[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_0[par_indice]) / SI_factor;
         }
         else if (v == 1)
         {
            if ( (strcmp(cid_tma_array_1[par_indice], "\0") != 0) && (atoi(cid_tma_array_1[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_1[par_indice]) / SI_factor;
         }
         else if (v == 2)
         {
            if ( (strcmp(cid_tma_array_2[par_indice], "\0") != 0) && (atoi(cid_tma_array_2[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_2[par_indice]) / SI_factor;
         }
         else if (v == 3)
         {
            if ( (strcmp(cid_tma_array_3[par_indice], "\0") != 0) && (atoi(cid_tma_array_3[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_3[par_indice]) / SI_factor;
         }
         else if (v == 4)
         {
            if ( (strcmp(cid_tma_array_4[par_indice], "\0") != 0) && (atoi(cid_tma_array_4[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_4[par_indice]) / SI_factor;
         }
         else if (v == 5)
         {
            if ( (strcmp(cid_tma_array_5[par_indice], "\0") != 0) && (atoi(cid_tma_array_5[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_5[par_indice]) / SI_factor;
         }
         else if (v == 6)
         {
            if ( (strcmp(cid_tma_array_6[par_indice], "\0") != 0) && (atoi(cid_tma_array_6[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_6[par_indice]) / SI_factor;
         }
         else if (v == 7)
         {
            if ( (strcmp(cid_tma_array_7[par_indice], "\0") != 0) && (atoi(cid_tma_array_7[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_7[par_indice]) / SI_factor;
         }
         else if (v == 8)
         {
            if ( (strcmp(cid_tma_array_8[par_indice], "\0") != 0) && (atoi(cid_tma_array_8[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_8[par_indice]) / SI_factor;
         }
      } /* if ( (strncmp(parameter, "HSW", 3) == 0) || etc. */
      else /* alle andere cic parameters  */
      {
         /* 0 hr */
         if (v == 0)
         {
            if ( (strcmp(cic_tma_spectrum_0[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_0[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_0[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_0[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_0[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_0[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_0[par_indice_hwa]) / SI_factor_hwa;
         } /* if (v == 0) */

         /* +1 hr */
         else if (v == 1)
         {
            if ( (strcmp(cic_tma_spectrum_1[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_1[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_1[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_1[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_1[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_1[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_1[par_indice_hwa]) / SI_factor_hwa;
         } /* else if ( v == 1) */

         /* +2 hr */
         else if (v == 2)
         {
            if ( (strcmp(cic_tma_spectrum_2[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_2[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_2[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_2[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_2[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_2[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_2[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 2) */

         /* +3 hr */
         else if (v == 3)
         {
            if ( (strcmp(cic_tma_spectrum_3[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_3[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_3[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_3[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_3[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_3[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_3[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 3) */

         /* +4 hr */
         else if (v == 4)
         {
            if ( (strcmp(cic_tma_spectrum_4[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_4[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_4[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_4[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_4[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_4[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_4[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 4) */

         /* + 5 hr */
         else if (v == 5)
         {
            if ( (strcmp(cic_tma_spectrum_5[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_5[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_5[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_5[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_5[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_5[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_5[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 5) */

         /* + 6hr */
         else if (v == 6)
         {
            if ( (strcmp(cic_tma_spectrum_6[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_6[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_6[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_6[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_6[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_6[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_6[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 6) */

         /* +7 hr */
         else if (v == 7)
         {
            if ( (strcmp(cic_tma_spectrum_7[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_7[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_7[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_7[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_7[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_7[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_7[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 7) */

         /* + 8 hr */
         else if (v == 8)
         {
            if ( (strcmp(cic_tma_spectrum_8[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_8[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_8[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_8[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_8[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_8[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_8[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 8) */

      } /* else (alle andere cic parameters) */


      if (obs_value != MAXFLOAT)
      {
#if 0
         if ((strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0))
         {
            /* i.v.m schaalverdeling van 180 - 360 - 180 (i.p.v. 0 - 360) */
            if (obs_value >= 180.0)
               y_pos_obs = (int)(_y1_par + ((obs_value - 180) * (double)scale_y / (double)y_interval) + 0.5);
            else
               y_pos_obs = (int)(_y1_par + ((obs_value + 180) * (double)scale_y / (double)y_interval) + 0.5);
         }
         else
#endif
            y_pos_obs = (int)(_y1_par + (obs_value * (double)scale_y / (double)y_interval) + 0.5);

         obs_value_new = obs_value;

         if ((strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0))
         {
#if 0
            if (obs_value_new >= 0 && obs_value_new < 180)
               hulp_obs_value_new = obs_value_new + 360;
            else
#endif
               hulp_obs_value_new = obs_value_new;
#if 0
            if (obs_value_old >= 0 && obs_value_old < 180)
               hulp_obs_value_old = obs_value_old + 360;
            else
#endif            
               hulp_obs_value_old = obs_value_old;
         } /* if (strncmp(parameter, "DHE", 3) == 0) || etc.*/


         if ( ((obs_value_old != MAXFLOAT) && (strncmp(parameter, "DHE", 3) != 0) && (strncmp(parameter, "DSW", 3) != 0)) ||
              ((obs_value_old != MAXFLOAT) && (strncmp(parameter, "DHE", 3) == 0) && (fabs(hulp_obs_value_new - hulp_obs_value_old) < max_richtings_shift)) ||
              ((obs_value_old != MAXFLOAT) && (strncmp(parameter, "DSW", 3) == 0) && (fabs(hulp_obs_value_new - hulp_obs_value_old) < max_richtings_shift)) )
         {
            /* lineto (obs line) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1_par + (v * scale_x));
            sprintf(buffer2, "%d", y_pos_obs);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " lineto   %obs line\n");
            fprintf(os_ps, "%s", line);

            /* stroke (obs line) */
            strcpy(line, "\0");
            strcpy(line, "stroke         %obs line\n");
            fprintf(os_ps, "%s", line);

            /* moveto (obs line) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1_par + (v * scale_x));
            sprintf(buffer2, "%d", y_pos_obs);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %obs line\n");
            fprintf(os_ps, "%s", line);

         } /* if (obs_value_old != MAXFLOAT) */
         else /* obs_value_old == MAXFLOAT || etc. */
         {
            /* moveto (obs line) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1_par + (v * scale_x));
            sprintf(buffer2, "%d", y_pos_obs);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %obs line\n");
            fprintf(os_ps, "%s", line);

         } /* else (obs_value_old == MAXINT) */

         obs_value_old = obs_value_new;

      } /* if (model_value != MAXFLOAT */
      else
      {
         obs_value_old = MAXFLOAT;
      }

   } /* for (v = start_indice; v <= eind_indice; v++) */

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   // obs - dot (CIC HM0, TM0-1, Hs10, dir Hs10, Hs7 en hirlam-FF)
   */

   /* initialisatie */
   start_indice = 0;
   eind_indice = aantal_cic_tma_temp_files;                /* zoveel cic temp files openen */

   /* afh. van de parameter instellingen maken */
   if (strncmp(parameter, "HM0", 3) == 0)
   {
      par_indice = HM0_SPECTRUM;
      SI_factor = 1000;                                    /* bv. inhoud array delen door 1000 om meter te krijgen */

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = HM0_HWA_INDEX;                      /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1;
   }
   else if (strncmp(parameter, "TM0", 3) == 0)
   {
      par_indice = TM0_1_SPECTRUM;
      SI_factor = 10;

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = TM0_HWA_INDEX;                      /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1;
   }
   else if (strncmp(parameter, "HE3", 3) == 0)             /* HE3 = Hs10 */
   {
      par_indice = HE3_SPECTRUM;
      SI_factor = 1000;
   }
   else if (strncmp(parameter, "DHE", 3) == 0)             /* direction E10 (is ook gelijk richting Hs10) */
   {
      par_indice = DD_GEM_LFE_SPECTRUM;
      SI_factor = 1;
   }
   else if (strncmp(parameter, "HS7", 3) == 0)
   {
      par_indice = HS7_SPECTRUM;
      SI_factor = 1000;
   }
   else if (strncmp(parameter, "_FF", 3) == 0)
   {
      par_indice = FF_WIND;
      SI_factor = 1;

      /* cic files leeg dan data uit hwa files gebruiken */
      par_indice_hwa = FF_HWA_INDEX;                      /* als er geen cic data zijn dan naar hwa data kijken */
      SI_factor_hwa = 1 / M_S_TO_KNOTS;
   }
   else if (strncmp(parameter, "HSW", 3) == 0)    /* height swell */
   {
      par_indice = HSW_CID_INDEX;                 /* staat in cm */
      SI_factor = 100;                            /* nu in m */
   }
   else if (strncmp(parameter, "DSW", 3) == 0)    /* direction swell  */
   {
      par_indice = DSW_CID_INDEX;                 /* staat in graden */
      SI_factor = 1;
   }
   else if (strncmp(parameter, "PSW", 3) == 0)    /* period swell */
   {
      par_indice = PSW_CID_INDEX;                 /* staat in 0.1 sec */
      SI_factor = 10;                             /* nu in 1 sec */
   }



   /* obs value bepalen */
   for (v = start_indice; v < eind_indice; v++)
   {
      /* initialisatie */
      obs_value = MAXFLOAT;

      /* nb bij cid deining wordt gewerkt met cid_array[] en de andere cic data met cic_tma_spectrum[] */
      if ( (strncmp(parameter, "HSW", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0) || (strncmp(parameter, "PSW", 3) == 0) )
      {
         if (v == 0)
         {
            if ( (strcmp(cid_tma_array_0[par_indice], "\0") != 0) && (atoi(cid_tma_array_0[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_0[par_indice]) / SI_factor;
         }
         else if (v == 1)
         {
            if ( (strcmp(cid_tma_array_1[par_indice], "\0") != 0) && (atoi(cid_tma_array_1[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_1[par_indice]) / SI_factor;
         }
         else if (v == 2)
         {
            if ( (strcmp(cid_tma_array_2[par_indice], "\0") != 0) && (atoi(cid_tma_array_2[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_2[par_indice]) / SI_factor;
         }
         else if (v == 3)
         {
            if ( (strcmp(cid_tma_array_3[par_indice], "\0") != 0) && (atoi(cid_tma_array_3[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_3[par_indice]) / SI_factor;
         }
         else if (v == 4)
         {
            if ( (strcmp(cid_tma_array_4[par_indice], "\0") != 0) && (atoi(cid_tma_array_4[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_4[par_indice]) / SI_factor;
         }
         else if (v == 5)
         {
            if ( (strcmp(cid_tma_array_5[par_indice], "\0") != 0) && (atoi(cid_tma_array_5[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_5[par_indice]) / SI_factor;
         }
         else if (v == 6)
         {
            if ( (strcmp(cid_tma_array_6[par_indice], "\0") != 0) && (atoi(cid_tma_array_6[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_6[par_indice]) / SI_factor;
         }
         else if (v == 7)
         {
            if ( (strcmp(cid_tma_array_7[par_indice], "\0") != 0) && (atoi(cid_tma_array_7[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_7[par_indice]) / SI_factor;
         }
         else if (v == 8)
         {
            if ( (strcmp(cid_tma_array_8[par_indice], "\0") != 0) && (atoi(cid_tma_array_8[par_indice]) > 0) )
               obs_value = atof(cid_tma_array_8[par_indice]) / SI_factor;
         }
      } /* if ( (strncmp(parameter, "HSW", 3) == 0) etc. */
      else /* alle andere cic parameters */
      {
         /* 0 hr */
         if (v == 0)
         {
            if ( (strcmp(cic_tma_spectrum_0[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_0[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_0[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_0[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_0[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_0[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_0[par_indice_hwa]) / SI_factor_hwa;
         } /* if (v == 0) */

         /* +1 hr */
         else if ( v == 1)
         {
            if ( (strcmp(cic_tma_spectrum_1[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_1[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_1[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_1[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_1[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_1[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_1[par_indice_hwa]) / SI_factor_hwa;
         } /* else if ( v == 1) */

         /* + 2 hr */
         else if (v == 2)
         {
            if ( (strcmp(cic_tma_spectrum_2[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_2[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_2[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_2[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_2[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_2[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_2[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 2) */

         /* + 3 hr */
         else if (v == 3)
         {
            if ( (strcmp(cic_tma_spectrum_3[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_3[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_3[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_3[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_3[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_3[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_3[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 3) */

         /* + 4 hr */
         else if (v == 4)
         {
            if ( (strcmp(cic_tma_spectrum_4[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_4[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_4[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_4[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_4[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_4[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_4[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 4) */

         /* + 5 hr */
         else if (v == 5)
         {
            if ( (strcmp(cic_tma_spectrum_5[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_5[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_5[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_5[par_indice]) / SI_factor;\
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_5[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_5[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_5[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 5) */

         /* + 6 hr */
         else if (v == 6)
         {
            if ( (strcmp(cic_tma_spectrum_6[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_6[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_6[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_6[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_6[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_6[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_6[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 6) */

         /* + 7 hr */
         else if (v == 7)
         {
            if ( (strcmp(cic_tma_spectrum_7[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_7[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_7[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_7[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_7[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_7[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_7[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 7) */

         /* + 8 hr */
         else if (v == 8)
         {
            if ( (strcmp(cic_tma_spectrum_8[par_indice], "\0") != 0) && (atoi(cic_tma_spectrum_8[par_indice]) > 0) )
            {
               if ( (par_indice != FF_WIND) || ((par_indice == FF_WIND) && (atoi(cic_tma_spectrum_8[par_indice]) != 99)) )
                  obs_value = atof(cic_tma_spectrum_8[par_indice]) / SI_factor;
            }
            else if ( ((strncmp(parameter, "HM0", 3) == 0) || (strncmp(parameter, "TM0", 3) == 0) || (strncmp(parameter, "_FF", 3) == 0)) &&
                      (strcmp(hwa_tma_array_8[par_indice_hwa], "\0") != 0) && (strncmp(hwa_tma_array_8[par_indice_hwa], "//", 2) != 0) )
               obs_value = atof(hwa_tma_array_8[par_indice_hwa]) / SI_factor_hwa;
         } /* else if (v == 8) */

      } /* else (alle andere cic parameters) */


      if (obs_value != MAXFLOAT)
      {
         /* newpath (obs dot) */
         strcpy(line, "\0");
         strcpy(line, "newpath            %obs dot\n");
         fprintf(os_ps, "%s", line);

         /* color (obs dot) */
         strcpy(line, "\0");
         strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
         strcat(line, " setrgbcolor %obs dot\n");
         fprintf(os_ps, "%s", line);

         y_pos_obs = (int)(_y1_par + (obs_value * (double)scale_y / (double)y_interval) + 0.5); 
#if 0
         if ((strncmp(parameter, "DHE", 3) == 0) || (strncmp(parameter, "DSW", 3) == 0))
         {
            /* i.v.m schaalverdeling van 180 - 360 - 180 (i.p.v. 0 - 360) */
            if (obs_value >= 180.0)
               y_pos_obs = (int)(_y1_par + ((obs_value - 180) * (double)scale_y / (double)y_interval) + 0.5);
            else
               y_pos_obs = (int)(_y1_par + ((obs_value + 180) * (double)scale_y / (double)y_interval) + 0.5);
         }
         else
            y_pos_obs = (int)(_y1_par + (obs_value * (double)scale_y / (double)y_interval) + 0.5);
#endif

         /* arc (obs dot) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         buffer3[0] = '\0';
         sprintf(buffer1, "%d", x1_par + (v * scale_x));
         sprintf(buffer2, "%d", y_pos_obs);
         sprintf(buffer3, "%d", straal_dot);                             /* straal van de cirkel */
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " ");
         strcat(line, buffer3);
         strcat(line, " 0 360 arc   %obs dot\n");
         fprintf(os_ps, "%s", line);

         /* fill (obs dot) */
         strcpy(line, "\0");
         strcpy(line, "fill         %obs dot\n");
         fprintf(os_ps, "%s", line);

      } /* if (obs_value != MAXFLOAT) */
   } /* for (v = start_indice; v <= eind_indice; v++) */

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*           Bepaal_Filenamen_CIC_Temp_Files_voor TMA_Per_Locatie            */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenamen_CIC_Temp_Files_voor_TMA_Per_Locatie(const char* station)
{
   /* max. 7 files (van de uurlijkse files) worden geplot dus behorende bij (0, +1, +2, +3, +4, +5, +6, +7, +8 hr) */

   /* datum stempel van de laatse RUN is:  JJJJMMDDUU var. */
   /* dus de CIC files zijn deze + degene die LATER beschikbaar zijn */

   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   int uren_vooruit;
   int tijd_ok;
   struct tm time_struct;                    /* datum-tijd manipulatie */
   char date_time[100];
   char temp[256];
   int i;
   char temp_cic_dir[256];



   /* initialisatie */
   temp_cic_dir[0] = '\0';

   strcpy(cic_tma_temp_0, "\0");
   strcpy(cic_tma_temp_1, "\0");
   strcpy(cic_tma_temp_2, "\0");
   strcpy(cic_tma_temp_3, "\0");
   strcpy(cic_tma_temp_4, "\0");
   strcpy(cic_tma_temp_5, "\0");
   strcpy(cic_tma_temp_6, "\0");
   strcpy(cic_tma_temp_7, "\0");
   strcpy(cic_tma_temp_8, "\0");


   /* eerste deel tempfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
   {
      strcpy(temp_cic_dir, "temp_cic\\");
   }
   else                                                /* UNIX */
   {
      strcpy(temp_cic_dir, "temp_cic/");
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */



   /* hulp voor file namen van b.v 3 en 6 uur vooruit bepalen (t.o.v van 0 uur  = JJJJJMMDDUU) */

   /* JJJJ */
   strncpy(JJJJ, JJJJMMDDUU + 0, 4);
   JJJJ[4] = '\0';
   num_JJJJ = atoi(JJJJ);

   /* MM */
   strncpy(MM, JJJJMMDDUU + 4, 2);
   MM[2] = '\0';
   num_MM = atoi(MM);

   /* DD */
   strncpy(DD, JJJJMMDDUU + 6, 2);
   DD[2] = '\0';
   num_DD = atoi(DD);

   /* UU */
   strncpy(UU, JJJJMMDDUU + 8, 2);
   UU[2] = '\0';
   num_UU = atoi(UU);




   for (i = 0; i < aantal_cic_tma_temp_files; i++)
   {
      /* initialisatie */
      temp[0] = '\0';

      uren_vooruit = i * TMA_VERWACHTINGS_INTERVAL;

      time_struct.tm_year  = num_JJJJ - 1900;
      time_struct.tm_mon   = num_MM - 1;
      time_struct.tm_mday  = num_DD;
      time_struct.tm_hour  = num_UU + uren_vooruit;
      time_struct.tm_min   = 0;
      time_struct.tm_sec   = 0;
      time_struct.tm_isdst = -1;                      /* daylight saving time: -1 = no info available */

      tijd_ok = 1;
      if (mktime(&time_struct) == -1)
         tijd_ok = 0;

      if (tijd_ok == 1)
      {
         (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121006 */

         strcat(temp, temp_cic_dir);                 /* b.v. temp_cic\\ */
         strcat(temp, "CIC_LDS_");
         strcat(temp, station);
         strcat(temp, "_");
         strcat(temp, date_time);
         strcat(temp, ".TMP");
      } /* if (tijd_ok == 1) */

      if (i == 0)
         strcpy(cic_tma_temp_0, temp);               /* b.v. temp_cic\\CIC_LDS_ANA_2001121006.TMP */
      else if (i == 1)
         strcpy(cic_tma_temp_1, temp);               /* b.v. temp_cic\\CIC_LDS_ANA_2001121007.TMP */
      else if (i == 2)
         strcpy(cic_tma_temp_2, temp);               /* b.v. temp_cic\\CIC_LDS_ANA_2001121008.TMP */
      else if (i == 3)
         strcpy(cic_tma_temp_3, temp);
      else if (i == 4)
         strcpy(cic_tma_temp_4, temp);
      else if (i == 5)
         strcpy(cic_tma_temp_5, temp);
      else if (i == 6)
         strcpy(cic_tma_temp_6, temp);
      else if (i == 7)
         strcpy(cic_tma_temp_7, temp);
      else if (i == 8)
         strcpy(cic_tma_temp_8, temp);

/**************  test ************/
/*
               fprintf(stderr, "%s", temp);
               getchar();
*/
/**************** test ************/

   } /* for (i = 0; i < aantal_cic_temp_files; i++) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                   Initialiseer_CIC_TMA_Spectrum_Arrays                    */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_CIC_TMA_Spectrum_Arrays()
{
   int i;



   for (i = 0; i < NUM_SPECTRUM; i++)
   {
      strcpy(cic_tma_spectrum_0[i], "\0");
      strcpy(cic_tma_spectrum_1[i], "\0");
      strcpy(cic_tma_spectrum_2[i], "\0");
      strcpy(cic_tma_spectrum_3[i], "\0");
      strcpy(cic_tma_spectrum_4[i], "\0");
      strcpy(cic_tma_spectrum_5[i], "\0");
      strcpy(cic_tma_spectrum_6[i], "\0");
      strcpy(cic_tma_spectrum_7[i], "\0");
      strcpy(cic_tma_spectrum_8[i], "\0");

   } /* for (i = 0; i < NUM_SPECTRUM; i++) */


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                       Read_CIC_TMA_Temps_in_Spectrums                     */
/*                                                                           */
/*****************************************************************************/
int Read_CIC_TMA_Temps_in_Spectrums()
{
   FILE* temp;
   char temp_naam[256];
   int i;
   int j;
   char regel[NUM_SPECTRUM_2 + 1];
   int regel_lengte = NUM_SPECTRUM_2;
   /*char buffer[10];*/


   /*
   // NB als er een temp file niet is dan geen foutmelding in de log zetten, omdat afhankelijk van het tijdstip
   // niet altijd alle aangevraagde temp files er wel moeten zijn
   */


   for (i = 0; i < aantal_cic_tma_temp_files; i++)
   {
      /* initialisatie */
      temp_naam[0] = '\0';

      /* file naam bepalen cic temp inlees file */
      if (i == 0)
         strcpy(temp_naam, cic_tma_temp_0);           /* b.v. temp_cic\CIC_LDS_ANA_2001121006.TMP */
      else if (i == 1)
         strcpy(temp_naam, cic_tma_temp_1);           /* b.v. temp_cic\CIC_LDS_ANA_2001121007.TMP */
      else if (i == 2)
         strcpy(temp_naam, cic_tma_temp_2);           /* b.v. temp_cic\CIC_LDS_ANA_2001121008.TMP */
      else if (i == 3)
         strcpy(temp_naam, cic_tma_temp_3);
      else if (i == 4)
         strcpy(temp_naam, cic_tma_temp_4);
      else if (i == 5)
         strcpy(temp_naam, cic_tma_temp_5);
      else if (i == 6)
         strcpy(temp_naam, cic_tma_temp_6);
      else if (i == 7)
         strcpy(temp_naam, cic_tma_temp_7);
      else if (i == 8)
         strcpy(temp_naam, cic_tma_temp_8);




/**************  test ************/
/*
               fprintf(stderr, "%s", temp_naam);
               getchar();
*/
/**************** test ************/


      /* inlezen in cic_tma_spectrum arrays [LET OP I en J indices !!] */
      if ((temp = fopen(temp_naam, "r")) != NULL)       /* dus gelukt */
      {
         for (j = 0; j < NUM_SPECTRUM; j++)             /* zoveel spectrum array elementen zoveel regels in de temp file */
         {
            if (fread(regel, regel_lengte + 1, 1, temp) != 0)
            {
               regel[regel_lengte] = '\0';

               if (i == 0)
                  strcpy(cic_tma_spectrum_0[j], regel);
               else if (i == 1)
                  strcpy(cic_tma_spectrum_1[j], regel);
               else if (i == 2)
                  strcpy(cic_tma_spectrum_2[j], regel);
               else if (i == 3)
                  strcpy(cic_tma_spectrum_3[j], regel);
               else if (i == 4)
                  strcpy(cic_tma_spectrum_4[j], regel);
               else if (i == 5)
                  strcpy(cic_tma_spectrum_5[j], regel);
               else if (i == 6)
                  strcpy(cic_tma_spectrum_6[j], regel);
               else if (i == 7)
                  strcpy(cic_tma_spectrum_7[j], regel);
               else if (i == 8)
                  strcpy(cic_tma_spectrum_8[j], regel);

            } /* if (fread(regel, regel_lengte + 1, 1, temp) != NULL) */
         } /* for (j = 0; j < NUM_SPECTRUM; j++) */

         fclose(temp);

      } /* if ((temp = fopen(temp_naam, "r")) != NULL) */
   } /* for (i = 0; i < aantal_cic_temp_files; i++) */


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                    Read_TMA_Temps_in_TMA_Parameter_Arrays                 */
/*                                                                           */
/*****************************************************************************/
int Read_TMA_Temps_in_TMA_Parameter_Arrays()
{
   FILE* temp;
   char line1[256];
   int i;
   int j;



   if ((temp = fopen(tma_tempfilenaam, "r")) != NULL)    /* dus gelukt */
   {
      for (i = 0; i < NUM_TMA_PARAMETERS; i++)
      {
         for (j = 0; j < NUM_TMA_VERWACHTINGEN; j++)
         {
            if (fgets(line1, 255, temp) != NULL)
               strcpy(TMA_Parameter_Array[i][j], line1);
         } /* for (j = 0; j < NUM_TMA_VERWACHTINGEN; j++) */
      } /* for (i = 0; i < NUM_TMA_PARAMETERS; i++) */

      fclose(temp);

   }  /* if ((temp = fopen(tma_tempfilenaam, "r")) != NULL) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                         Bepaal_TMA_Tempfilenaam                           */
/*                                                                           */
/*****************************************************************************/
int Bepaal_TMA_Tempfilenaam(const char* station)
{


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(tma_tempfilenaam, "temp_nedwam\\");
   else                                                /* UNIX */
      strcpy(tma_tempfilenaam, "temp_nedwam/");

   strcat(tma_tempfilenaam, "NEDWAM_TMA_");
   strcat(tma_tempfilenaam, station);
   strcat(tma_tempfilenaam, "_");
   strcat(tma_tempfilenaam, JJJJMMDDUU);
   strcat(tma_tempfilenaam, ".TMP");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                               Bepaal_Scale_X                              */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Scale_X(double* scale_x)
{
   /* afstand tussen 2 marks op x-as (forecast uren) bepalen */
   /**scale_x = (double)lengte_x_as_par / ((double)NUM_VERWACHTINGEN -1);*/    /* bv 512 / (17 - 1) =  32 */

   /* afstand tussen twee forecasts (die per uur zijn voor TMA) bepalen */
   *scale_x = (double)lengte_x_as_par / ((double)((NUM_TMA_VERWACHTINGEN -1)));    /* bv 512 / ((49 - 1))  =  10,66666 */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                         Bepaal_UBA_Tempfilenaam                           */
/*                                                                           */
/*****************************************************************************/
int Bepaal_UBA_Tempfilenaam(const char* station)
{


   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(uba_tempfilenaam, "temp_hirlam\\");
   else                                                /* UNIX */
      strcpy(uba_tempfilenaam, "temp_hirlam/");

   strcat(uba_tempfilenaam, "HIRLAM_UBA_");
   strcat(uba_tempfilenaam, station);
   strcat(uba_tempfilenaam, "_");
   strcat(uba_tempfilenaam, JJJJMMDDUU);
   strcat(uba_tempfilenaam, ".TMP");


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                    Read_UBA_Temps_in_UBA_Parameter_Arrays                 */
/*                                                                           */
/*****************************************************************************/
int Read_UBA_Temps_in_UBA_Parameter_Arrays()
{
   FILE* temp;
   char line1[256];
   int i;
   int j;



   if ((temp = fopen(uba_tempfilenaam, "r")) != NULL)    /* dus gelukt */
   {
      for (i = 0; i < NUM_UBA_PARAMETERS; i++)
      {
         for (j = 0; j < NUM_UBA_VERWACHTINGEN; j++)
         {
            if (fgets(line1, 255, temp) != NULL)
               strcpy(UBA_Parameter_Array[i][j], line1);
         } /* for (j = 0; j < NUM_UBA_VERWACHTINGEN; j++) */
      } /* for (i = 0; i < NUM_UBA_PARAMETERS; i++) */

      fclose(temp);

   }  /* if ((temp = fopen(uba_tempfilenaam, "r")) != NULL) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*           Bepaal_Filenamen_CID_Temp_Files_voor TMA_Per_Locatie            */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenamen_CID_Temp_Files_voor_TMA_Per_Locatie(const char* station)
{
   /* in de CID files staan de swell data van de metingen */
   /* max. 7 files (van de uurlijkse files) worden geplot dus behorende bij (0, +1, +2, +3, +4, +5, +6, +7, +8 hr) */

   /* datum stempel van de laatse RUN is:  JJJJMMDDUU var. */
   /* dus de CID files zijn deze + degene die LATER beschikbaar zijn */

   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   int uren_vooruit;
   int tijd_ok;
   struct tm time_struct;                    /* datum-tijd manipulatie */
   char date_time[100];
   char temp[256];
   int i;
   char temp_cid_dir[256];



   /* initialisatie */
   temp_cid_dir[0] = '\0';

   strcpy(cid_tma_temp_0, "\0");
   strcpy(cid_tma_temp_1, "\0");
   strcpy(cid_tma_temp_2, "\0");
   strcpy(cid_tma_temp_3, "\0");
   strcpy(cid_tma_temp_4, "\0");
   strcpy(cid_tma_temp_5, "\0");
   strcpy(cid_tma_temp_6, "\0");
   strcpy(cid_tma_temp_7, "\0");
   strcpy(cid_tma_temp_8, "\0");


   /* eerste deel tempfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
   {
      strcpy(temp_cid_dir, "temp_cic\\");
   }
   else                                                /* UNIX */
   {
      strcpy(temp_cid_dir, "temp_cic/");
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */



   /* hulp voor file namen van b.v 3 en 6 uur vooruit bepalen (t.o.v van 0 uur  = JJJJJMMDDUU) */

   /* JJJJ */
   strncpy(JJJJ, JJJJMMDDUU + 0, 4);
   JJJJ[4] = '\0';
   num_JJJJ = atoi(JJJJ);

   /* MM */
   strncpy(MM, JJJJMMDDUU + 4, 2);
   MM[2] = '\0';
   num_MM = atoi(MM);

   /* DD */
   strncpy(DD, JJJJMMDDUU + 6, 2);
   DD[2] = '\0';
   num_DD = atoi(DD);

   /* UU */
   strncpy(UU, JJJJMMDDUU + 8, 2);
   UU[2] = '\0';
   num_UU = atoi(UU);



   for (i = 0; i < aantal_cid_tma_temp_files; i++)
   {
      /* initialisatie */
      temp[0] = '\0';

      uren_vooruit = i * TMA_VERWACHTINGS_INTERVAL;

      time_struct.tm_year  = num_JJJJ - 1900;
      time_struct.tm_mon   = num_MM - 1;
      time_struct.tm_mday  = num_DD;
      time_struct.tm_hour  = num_UU + uren_vooruit;
      time_struct.tm_min   = 0;
      time_struct.tm_sec   = 0;
      time_struct.tm_isdst = -1;                      /* daylight saving time: -1 = no info available */

      tijd_ok = 1;
      if (mktime(&time_struct) == -1)
         tijd_ok = 0;

      if (tijd_ok == 1)
      {
         (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121006 */

         strcat(temp, temp_cid_dir);                 /* b.v. temp_cic\\ */
         strcat(temp, "CIC_CID_");
         strcat(temp, station);
         strcat(temp, "_");
         strcat(temp, date_time);
         strcat(temp, ".TMP");
      } /* if (tijd_ok == 1) */

      if (i == 0)
         strcpy(cid_tma_temp_0, temp);               /* b.v. temp_cic\\CIC_CID_ANA_2001121006.TMP */
      else if (i == 1)
         strcpy(cid_tma_temp_1, temp);               /* b.v. temp_cic\\CIC_CID_ANA_2001121007.TMP */
      else if (i == 2)
         strcpy(cid_tma_temp_2, temp);               /* b.v. temp_cic\\CIC_CID_ANA_2001121008.TMP */
      else if (i == 3)
         strcpy(cid_tma_temp_3, temp);
      else if (i == 4)
         strcpy(cid_tma_temp_4, temp);
      else if (i == 5)
         strcpy(cid_tma_temp_5, temp);
      else if (i == 6)
         strcpy(cid_tma_temp_6, temp);
      else if (i == 7)
         strcpy(cid_tma_temp_7, temp);
      else if (i == 8)
         strcpy(cid_tma_temp_8, temp);

/**************  test ************/
/*
               fprintf(stderr, "%s", temp);
               getchar();
*/
/**************** test ************/

   } /* for (i = 0; i < aantal_cid_temp_files; i++) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                   Initialiseer_CID_TMA_Arrays                    */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_CID_TMA_Arrays()
{
   int i;


   for (i = 0; i < AANTAL_SWELL_CID_ELEMENTEN; i++)
   {
      strcpy(cid_tma_array_0[i], "\0");
      strcpy(cid_tma_array_1[i], "\0");
      strcpy(cid_tma_array_2[i], "\0");
      strcpy(cid_tma_array_3[i], "\0");
      strcpy(cid_tma_array_4[i], "\0");
      strcpy(cid_tma_array_5[i], "\0");
      strcpy(cid_tma_array_6[i], "\0");
      strcpy(cid_tma_array_7[i], "\0");
      strcpy(cid_tma_array_8[i], "\0");

   } /* for (i = 0; i < AANTAL_SWELL_CID_ELEMENTEN; i++) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                          Read_CID_TMA_Temps_in_Arrays                     */
/*                                                                           */
/*****************************************************************************/
int Read_CID_TMA_Temps_in_Arrays()
{
   FILE* temp;
   char temp_naam[256];
   int i;
   int j;
   char regel[256];
   /*char buffer[10];*/


   /*
   // NB als er een temp file niet is dan geen foutmelding in de log zetten, omdat afhankelijk van het tijdstip
   // niet altijd alle aangevraagde temp files er wel moeten zijn
   */


   for (i = 0; i < aantal_cid_tma_temp_files; i++)
   {
      /* initialisatie */
      temp_naam[0] = '\0';

      /* file naam bepalen cid temp inlees file */
      if (i == 0)
         strcpy(temp_naam, cid_tma_temp_0);           /* b.v. temp_cic\CIC_CID_ANA_2001121006.TMP */
      else if (i == 1)
         strcpy(temp_naam, cid_tma_temp_1);           /* b.v. temp_cic\CIC_CID_ANA_2001121007.TMP */
      else if (i == 2)
         strcpy(temp_naam, cid_tma_temp_2);           /* b.v. temp_cic\CIC_CID_ANA_2001121008.TMP */
      else if (i == 3)
         strcpy(temp_naam, cid_tma_temp_3);
      else if (i == 4)
         strcpy(temp_naam, cid_tma_temp_4);
      else if (i == 5)
         strcpy(temp_naam, cid_tma_temp_5);
      else if (i == 6)
         strcpy(temp_naam, cid_tma_temp_6);
      else if (i == 7)
         strcpy(temp_naam, cid_tma_temp_7);
      else if (i == 8)
         strcpy(temp_naam, cid_tma_temp_8);


/**************  test ************/
/*
               fprintf(stderr, "%s", temp_naam);
               getchar();
*/
/**************** test ************/


      /* inlezen in cid_tma arrays [LET OP I en J indices !!] */
      if ((temp = fopen(temp_naam, "r")) != NULL)           /* dus gelukt */
      {
         for (j = 0; j < AANTAL_SWELL_CID_ELEMENTEN; j++)   /* zoveel array elementen zoveel regels in de temp file */
         {
            if (fgets(regel, 512, temp) != NULL)
            {
               if (i == 0)
                  strcpy(cid_tma_array_0[j], regel);
               else if (i == 1)
                  strcpy(cid_tma_array_1[j], regel);
               else if (i == 2)
                  strcpy(cid_tma_array_2[j], regel);
               else if (i == 3)
                  strcpy(cid_tma_array_3[j], regel);
               else if (i == 4)
                  strcpy(cid_tma_array_4[j], regel);
               else if (i == 5)
                  strcpy(cid_tma_array_5[j], regel);
               else if (i == 6)
                  strcpy(cid_tma_array_6[j], regel);
               else if (i == 7)
                  strcpy(cid_tma_array_7[j], regel);
               else if (i == 8)
                  strcpy(cid_tma_array_8[j], regel);
            } /* if (fgets(regel, 512, in) != NULL) */
         } /* for (j = 0; j < AANTAL_SWELL_CID_ELEMENTEN; j++)  */

         fclose(temp);

      } /* if ((temp = fopen(temp_naam, "r")) != NULL) */
   } /* for (i = 0; i < aantal_cic_temp_files; i++) */


   return 0;
}


/* ////////// */

/*****************************************************************************/
/*                                                                           */
/*           Bepaal_Filenamen_HWA_Temp_Files_voor TMA_Per_Locatie            */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenamen_HWA_Temp_Files_voor_TMA_Per_Locatie(const char* station)
{
   /* in de HWA files staan de hm0, tm0-1, dd, ff van noordelijke stations */
   /* max. 7 files (van de uurlijkse files) worden geplot dus behorende bij (0, +1, +2, +3, +4, +5, +6, +7, +8 hr) */

   /* datum stempel van de laatse RUN is:  JJJJMMDDUU var. */
   /* dus de HWA files zijn deze + degene die LATER beschikbaar zijn */

   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   int uren_vooruit;
   int tijd_ok;
   struct tm time_struct;                    /* datum-tijd manipulatie */
   char date_time[100];
   char temp[256];
   int i;
   char temp_hwa_dir[256];


   /* initialisatie */
   temp_hwa_dir[0] = '\0';

   strcpy(hwa_tma_temp_0, "\0");
   strcpy(hwa_tma_temp_1, "\0");
   strcpy(hwa_tma_temp_2, "\0");
   strcpy(hwa_tma_temp_3, "\0");
   strcpy(hwa_tma_temp_4, "\0");
   strcpy(hwa_tma_temp_5, "\0");
   strcpy(hwa_tma_temp_6, "\0");
   strcpy(hwa_tma_temp_7, "\0");
   strcpy(hwa_tma_temp_8, "\0");


   /* eerste deel tempfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
   {
      strcpy(temp_hwa_dir, "temp_cic\\");
   }
   else                                                /* UNIX */
   {
      strcpy(temp_hwa_dir, "temp_cic/");
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */



   /* hulp voor file namen van b.v 3 en 6 uur vooruit bepalen (t.o.v van 0 uur  = JJJJJMMDDUU) */

   /* JJJJ */
   strncpy(JJJJ, JJJJMMDDUU + 0, 4);
   JJJJ[4] = '\0';
   num_JJJJ = atoi(JJJJ);

   /* MM */
   strncpy(MM, JJJJMMDDUU + 4, 2);
   MM[2] = '\0';
   num_MM = atoi(MM);

   /* DD */
   strncpy(DD, JJJJMMDDUU + 6, 2);
   DD[2] = '\0';
   num_DD = atoi(DD);

   /* UU */
   strncpy(UU, JJJJMMDDUU + 8, 2);
   UU[2] = '\0';
   num_UU = atoi(UU);



   for (i = 0; i < aantal_hwa_tma_temp_files; i++)
   {
      /* initialisatie */
      temp[0] = '\0';

      uren_vooruit = i * TMA_VERWACHTINGS_INTERVAL;

      time_struct.tm_year  = num_JJJJ - 1900;
      time_struct.tm_mon   = num_MM - 1;
      time_struct.tm_mday  = num_DD;
      time_struct.tm_hour  = num_UU + uren_vooruit;
      time_struct.tm_min   = 0;
      time_struct.tm_sec   = 0;
      time_struct.tm_isdst = -1;                      /* daylight saving time: -1 = no info available */

      tijd_ok = 1;
      if (mktime(&time_struct) == -1)
         tijd_ok = 0;

      if (tijd_ok == 1)
      {
         (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121006 */

         strcat(temp, temp_hwa_dir);                 /* b.v. temp_cic\\ */
         strcat(temp, "CIC_HWA_");
         strcat(temp, station);
         strcat(temp, "_");
         strcat(temp, date_time);
         strcat(temp, ".TMP");
      } /* if (tijd_ok == 1) */

      if (i == 0)
         strcpy(hwa_tma_temp_0, temp);               /* b.v. temp_cic\\CIC_HWA_ANA_2001121006.TMP */
      else if (i == 1)
         strcpy(hwa_tma_temp_1, temp);               /* b.v. temp_cic\\CIC_HWA_ANA_2001121007.TMP */
      else if (i == 2)
         strcpy(hwa_tma_temp_2, temp);               /* b.v. temp_cic\\CIC_HWA_ANA_2001121008.TMP */
      else if (i == 3)
         strcpy(hwa_tma_temp_3, temp);
      else if (i == 4)
         strcpy(hwa_tma_temp_4, temp);
      else if (i == 5)
         strcpy(hwa_tma_temp_5, temp);
      else if (i == 6)
         strcpy(hwa_tma_temp_6, temp);
      else if (i == 7)
         strcpy(hwa_tma_temp_7, temp);
      else if (i == 8)
         strcpy(hwa_tma_temp_8, temp);

/**************  test ************/
/*
               fprintf(stderr, "%s", temp);
               getchar();
*/
/**************** test ************/

   } /* for (i = 0; i < aantal_hwa_temp_files; i++) */


   return 0;
}

/*////////////////*/
/*****************************************************************************/
/*                                                                           */
/*                          Read_HWA_TMA_Temps_in_Arrays                     */
/*                                                                           */
/*****************************************************************************/
int Read_HWA_TMA_Temps_in_Arrays()
{
   FILE* temp;
   char temp_naam[256];
   int i;
   int j;
   char regel[256];
   /*char buffer[10];*/


   /*
   // NB als er een temp file niet is dan geen foutmelding in de log zetten, omdat afhankelijk van het tijdstip
   // niet altijd alle aangevraagde temp files er wel moeten zijn
   */


   for (i = 0; i < aantal_hwa_tma_temp_files; i++)
   {
      /* initialisatie */
      temp_naam[0] = '\0';

      /* file naam bepalen hwa temp inlees file */
      if (i == 0)
         strcpy(temp_naam, hwa_tma_temp_0);           /* b.v. temp_cic\CIC_HWA_ANA_2001121006.TMP */
      else if (i == 1)
         strcpy(temp_naam, hwa_tma_temp_1);           /* b.v. temp_cic\CIC_HWA_ANA_2001121007.TMP */
      else if (i == 2)
         strcpy(temp_naam, hwa_tma_temp_2);           /* b.v. temp_cic\CIC_HWA_ANA_2001121008.TMP */
      else if (i == 3)
         strcpy(temp_naam, hwa_tma_temp_3);
      else if (i == 4)
         strcpy(temp_naam, hwa_tma_temp_4);
      else if (i == 5)
         strcpy(temp_naam, hwa_tma_temp_5);
      else if (i == 6)
         strcpy(temp_naam, hwa_tma_temp_6);
      else if (i == 7)
         strcpy(temp_naam, hwa_tma_temp_7);
      else if (i == 8)
         strcpy(temp_naam, hwa_tma_temp_8);


/**************  test ************/
/*
               fprintf(stderr, "%s", temp_naam);
               getchar();
*/
/**************** test ************/


      /* inlezen in hwa_tma arrays [LET OP I en J indices !!] */
      if ((temp = fopen(temp_naam, "r")) != NULL)           /* dus gelukt */
      {
         for (j = 0; j < AANTAL_HWA_ELEMENTEN; j++)   /* zoveel array elementen zoveel regels in de temp file */
         {
            if (fgets(regel, 512, temp) != NULL)
            {
               if (i == 0)
                  strcpy(hwa_tma_array_0[j], regel);
               else if (i == 1)
                  strcpy(hwa_tma_array_1[j], regel);
               else if (i == 2)
                  strcpy(hwa_tma_array_2[j], regel);
               else if (i == 3)
                  strcpy(hwa_tma_array_3[j], regel);
               else if (i == 4)
                  strcpy(hwa_tma_array_4[j], regel);
               else if (i == 5)
                  strcpy(hwa_tma_array_5[j], regel);
               else if (i == 6)
                  strcpy(hwa_tma_array_6[j], regel);
               else if (i == 7)
                  strcpy(hwa_tma_array_7[j], regel);
               else if (i == 8)
                  strcpy(hwa_tma_array_8[j], regel);
            } /* if (fgets(regel, 512, in) != NULL) */
         } /* for (j = 0; j < AANTAL_HWA_ELEMENTEN; j++)  */

         fclose(temp);

      } /* if ((temp = fopen(temp_naam, "r")) != NULL) */
   } /* for (i = 0; i < aantal_hwa_temp_files; i++) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                   Initialiseer_HWA_TMA_Arrays                    */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_HWA_TMA_Arrays()
{
   int i;


   for (i = 0; i < AANTAL_HWA_ELEMENTEN; i++)
   {
      strcpy(hwa_tma_array_0[i], "\0");
      strcpy(hwa_tma_array_1[i], "\0");
      strcpy(hwa_tma_array_2[i], "\0");
      strcpy(hwa_tma_array_3[i], "\0");
      strcpy(hwa_tma_array_4[i], "\0");
      strcpy(hwa_tma_array_5[i], "\0");
      strcpy(hwa_tma_array_6[i], "\0");
      strcpy(hwa_tma_array_7[i], "\0");
      strcpy(hwa_tma_array_8[i], "\0");

   } /* for (i = 0; i < AANTAL_HWA_ELEMENTEN; i++) */


   return 0;
}





