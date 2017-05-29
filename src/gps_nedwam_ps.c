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




/* globale var's in andere module */
extern char OS[8];                              /* gpsmain.c */
extern char JJJJMMDDUU[11];                     /* gpsmain.c; via argument list */
extern FILE* os_ps;                             /* gpsmain.c; schrijven postscript files */
extern char font_keuze[50];                     /* gpsmain.c */
extern int x1;                                  /* gpsmain.c */
extern int x2;                                  /* gpsmain.c */
extern int x3;                                  /* gpsmain.c */
extern int x4;                                  /* gpsmain.c */
extern int y2;                                  /* gpsmain.c */
extern int _y1;                                 /* gpsmain.c */
extern char tempfilenaam_0[256];                /* gpsmain.c; i.v.m. CIC files van JJJJMMDDUU (TSA mode) */
extern char tempfilenaam_1[256];                /* gpsmain.c; i.v.m. CIC files van JJJJMMDDUU + b.v. 3 uur (TSA mode) */
extern char tempfilenaam_2[256];                /* gpsmain.c; i.v.m. CIC files van JJJJMMDDUU + b.v. 6 uur (TSA mode) */
extern int NUM_FREQUENTIES;                     /* aantal frequenties */
extern int NUM_VERWACHTINGEN;                   /* aantal verwachtingen */
extern int INT_VERWACHTINGEN;                   /* uren interval tussen 2 opeenvolgende verwachtingen */
extern int of3;                                 /* zie ook function: Write_Postscript_Raster_Box() [file gps_ps.c] */
extern int of3b;                                /* zie ook function: Write_Postscript_Raster_Box() [file gps_ps.c] */
extern int of4;                                 /* zie ook function: Write_Postscript_Raster_Box() [file gps_ps.c] */
extern float y_factor_energy;                   /* zie ook function: Write_Postscript_Raster_Box() [file gps_ps.c] */
extern int y_factor_richting;                   /* zie ook function: Write_Postscript_Raster_Box() [file gps_ps.c] */
extern int x_factor;                            /* zie ook function: Write_Postscript_Raster_Box() [file gps_ps.c] */
extern double* FR;                              /* uit NEDWAM berekeningen, hierin alle frequenties (0 - NUM_FREQUENTIES) */
extern double** EMN1DIM;                        /* 1-dim. spectrum (via module: gps_nedwam_compute.c) */
extern double** THQSP;                          /* richtings spectrum */
extern double* E10;                             /* voor LFE summary ; uit NEDWAM berekeningen */
extern double* EMEAN;                           /* voor dir LFE summary; uit NEDWAM berekeningen */
extern double* FMEAN;                           /* voor TM0-1 summary; uit NEDWAM berekeningen */
extern double* THQ;                             /* voor dir.tot summary; uit NEDWAM berekeningen */
extern double* E10D;                            /* voor dir.LFE summary; uit NEDWAM berekeningen */
extern double* DDD_hirlam;                      /* hirlam wind-richtingen */
extern double* FF_hirlam;                       /* hirlam wind-snelheden */

/* function prototypes in andere module */
extern int Write_Log(char* bericht);
extern int Write_Postscript_General(void);
extern int Write_Postscript_End(void);
extern int Write_Postscript_Raster_Box(char* grafiek_orientatie, const int scale_y_max, const int scale_x_max, const int pag_no);
extern int Bereken_BFI(double freq_array[], const int aantal_freq_array_elementen, const double fp, const double num_Tm0_1, double* BFI, double* K40, double* P, const char* filenaam, const double num_Hm0, const char* station, const double num_Hmax, const double Hmax_Hm0);
extern double Bepaal_Diepte(const char* station);

/* function prototypes voor deze module */
int Bepaal_Filenamen_CIC_Temp_Files_voor_NEDWAM_Per_Locatie(const char* station, const int cic_uren_interval, const int pag_no) ;
int Write_TSA_Postscript_Files();
int Bepaal_Max_Energy_Per_Ps_Pagina(double* max_energy_nedwam, int pag_no);
int Bepaal_TSA_Outputfilenaam_Per_Locatie(const char* station, int pag_no);
int Bepaal_Title_Per_Grafiek(const char* grafiek_orientatie, const int pag_no, char* title, const char* station);
int TSA_Temp_Files_To_Postscript(const char* grafiek_orientatie, const int scale_x_max, const int scale_y_max, const int pag_no, const char* station);
int Bepaal_Scale_Y_X_Max_TSA(const double max_energy, int *scale_y_max, int *scale_x_max);
int Bepaal_Summary_Per_Grafiek(const int v, char* summary, const char* station);
int Read_CIC_Temps_in_Spectrums(void);                      /* i.v.m. cic files in verwachtingen plotjes */
int Initialiseer_CIC_Spectrum_Arrays(void);                 /* i.v.m. cic files in verwachtingen plotjes */
int Bepaal_Max_Energy_CIC_Spectrums(double* max_energy_cic);/* i.v.m. cic files in verwachtingen plotjes */

/* variabelen voor deze module */
/*FILE* os_ps;*/
char nedwam_outputfilenaam[256];
char spectrum_0[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU (TSA mode) */
char spectrum_1[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU + b.v. 3 uur (TSA mode) */
char spectrum_2[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU + b.v. 6 uur (TSA mode) */





/*****************************************************************************/
/*                                                                           */
/*                        Write_TSA_Postscript_Files                         */
/*                                                                           */
/*****************************************************************************/
int Write_TSA_Postscript_Files(const char* station)
{
   int num_ps_paginas;                         /* totaal aantal ps paginas met NEDWAM verwachtingen */
   int i;                                      /* teller ps paginas */
   double max_energy;                          /* max energy (hoogste van cic en nedwam gedeelte) */
   double max_energy_cic;                      /* max energy (cic gedeelte) */
   double max_energy_nedwam;                   /* max_energy (nedwam gedeelte) */
   int scale_x_max;
   int scale_y_max;
   char info[512];
   char volledig_path[512];
   char grafiek_orientatie[256];
   /*char buffer[10];*/
   int cic_uren_interval;


   /* altijd 3 verwachtingen op 1 ps pagina */
   if (NUM_VERWACHTINGEN % 3 != 0)
      num_ps_paginas = NUM_VERWACHTINGEN / 3 + 1;               /* b.v. 17 / 3 + 1 = 5.66 + 1 = 6.66 -> 6 (i.v.m. type 'int') */
   else
      num_ps_paginas = NUM_VERWACHTINGEN / 3;                   /* b.v. 18 / 3 = 6 */



   for (i = 0; i < num_ps_paginas; i++)
   {

      /* cic gedeelte bepalen alleen indien forecast interval = 3 uur (beveiliging voor de toekomst) */
      if (INT_VERWACHTINGEN == 3)
      {
         /*if (i == 0)                                                         /* alleen voor de eerste pagina */
         /*{*/
            /* bepalen CIC temp files die bij de verwachtingen horen */
            cic_uren_interval = INT_VERWACHTINGEN;            /* als de forecasts b.v. om de 3 uur den de metingingen ook om de 3 uur ophalen */
            Bepaal_Filenamen_CIC_Temp_Files_voor_NEDWAM_Per_Locatie((char*)station, cic_uren_interval, i);   /* zie: module deze */

            /* vullen CIC spectrum arrays */
            Initialiseer_CIC_Spectrum_Arrays();
            Read_CIC_Temps_in_Spectrums();                                   /* zie module: deze */

            /* max energy cic deel bepalen */
            Bepaal_Max_Energy_CIC_Spectrums(&max_energy_cic);                /* zie module: deze */
            max_energy_cic = max_energy_cic / 100000;                        /* van .1cm2s --> m2/Hz */
         /*}*/ /* if (i == 0)*/
         /*else*/
         /*   max_energy_cic = 0.0;*/
      } /* if (INT_VERWACHTINGEN == 3) */
      else
      {
         /* NB indien INT_VERWACHTINGEN echt != 3 dan wel verder uitwerken (zoals function: Bepaal_Max_Energy_CIC_Spectrums() zal verder uitgewerkt moeten worden) ! */
         max_energy_cic = 0.0;
      }

      /* max energy nedwam gedeelte bepalen */
      Bepaal_Max_Energy_Per_Ps_Pagina(&max_energy_nedwam, i);             /* zie module: deze */

      /* max energy hoogste van cic deel en nedwam deel */
      if (max_energy_cic > max_energy_nedwam)
         max_energy = max_energy_cic;
      else
         max_energy = max_energy_nedwam;



      Bepaal_Scale_Y_X_Max_TSA(max_energy, &scale_y_max, &scale_x_max);   /* zie module: deze */

      Bepaal_TSA_Outputfilenaam_Per_Locatie(station, i);                  /* zie module: deze */


      if ((os_ps = fopen(nedwam_outputfilenaam, "w")) == NULL)      /* dus mislukt */
      {
         getcwd(volledig_path, 512);

         /* bericht samen stellen */
         strcpy(info, "\0");
         strcat(info, volledig_path);

         if (strcmp(OS, "WINDOWS") == 0)
            strcat(info, "\\");
         else
            strcat(info, "/");

         strcat(info, nedwam_outputfilenaam);
         strcat(info, " niet te schrijven\n");

         /* naar log schrijven */
         Write_Log(info);

      } /* if ((os_ps = fopen(outputfilenaam, "w")) == NULL) */
      else /* outputfile is te schrijven */
      {

         /* ps pagina correct beginnen */
         Write_Postscript_General();

         /* onderste grafiek (I.V.M. 'TRANSLATE' ONDERSTE GRAFIEK ALS EERSTE DOEN) */
         strcpy(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK);
         Write_Postscript_Raster_Box(grafiek_orientatie, scale_y_max, scale_x_max, i);
         TSA_Temp_Files_To_Postscript(grafiek_orientatie, scale_x_max, scale_y_max, i, station);

         /* middelste grafiek (I.V.M. 'TRANSLATE' MIDDELSTE GRAFIEK ALS TWEEDE DOEN)*/
         strcpy(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK);
         Write_Postscript_Raster_Box(grafiek_orientatie, scale_y_max, scale_x_max, i);
         TSA_Temp_Files_To_Postscript(grafiek_orientatie, scale_x_max, scale_y_max, i, station);

         /* bovenste grafiek (I.V.M. 'TRANSLATE' MIDDELSTE GRAFIEK ALS LAATSTE DOEN)*/
         strcpy(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK);
         Write_Postscript_Raster_Box(grafiek_orientatie, scale_y_max, scale_x_max, i);
         TSA_Temp_Files_To_Postscript(grafiek_orientatie, scale_x_max, scale_y_max, i, station);

         /* ps pagina correct afsluiten */
         Write_Postscript_End();

         fclose(os_ps);
      } /* else (outputfile is te schrijven) */

   } /* for (i = 0; i < num_ps_paginas; i++) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                     Bepaal_Max_Energy_Per_Ps_Pagina                       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Energy_Per_Ps_Pagina(double* max_energy_nedwam, int pag_no)
{
   int start_indice;               /* begin indice juiste verwachtingen voor betref. pagina */
   int eind_indice;                /* eind indice juiste verwachtingen voor betref. pagina */
   int m;                          /* teller frequenties */
   int v;                          /* teller verwachtingen */
   /*char buffer[100];*/


   /* template: EMN1DIM[NUM_FREQUENTIES][NUM_VERWACHTINGEN] */

   /* per pagina (met 3 verwachtingen) de max energy er uit halen
   // bv.:
   // pagina 0:    0hr,  +3hr,  +6hr (resp. EMN1DIM-v-indice:  0, 1, 2)
   // pagina 1:   +9hr, +12hr, +15hr (resp. EMN1DIM-v-indice:  3, 4, 5)
   // pagina 2:  +18hr, +21hr, +24hr (resp. EMN1DIM-v-indice:  6, 7, 8)
   // pagina 3:  +27hr, +30hr, +33hr (resp. EMN1DIM-v-indice:  9,10,11)
   // pagina 4:  +36hr, +39hr, +42hr (resp. EMN1DIM-v-indice: 12,13,14)
   // pagina 5:  +45hr, +48hr        (resp. EMN1DIM-v-indice: 15,16   )
   */


   /* initialisatie */
   *max_energy_nedwam = 0.0;
   start_indice = pag_no * 3;                /* NB. pag_no begint bij 0 */

   eind_indice = start_indice + 2;           /* indices komen overeen met verwachtingen */
   if (eind_indice > NUM_VERWACHTINGEN -1)   /* op laatste pagina zouden maar 1 of  2 verwachtingen op kunnen staan (indices beginnen bij 0 ) */
      eind_indice = NUM_VERWACHTINGEN -1;


   for (m = 0; m < NUM_FREQUENTIES; m++)
   {
      for (v = start_indice; v <= eind_indice; v++)
      {
         if (EMN1DIM[m][v] > *max_energy_nedwam)
         {
            *max_energy_nedwam = EMN1DIM[m][v];
         }
      } /* for (v = start_indice; v <= eind_indice; v++) */
   } /* for (m = 0; m < NUM_FREQUENTIES; m++) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                Bepaal_TSA_Outputfilenaam_Per_Locatie()                    */
/*                                                                           */
/*****************************************************************************/
int Bepaal_TSA_Outputfilenaam_Per_Locatie(const char* station, int pag_no)
{

   char hulp_pag_no[10];                           /* char equavalent van pag_no */

   /* outputfilenaam (inclusief sub. dir.) bv.: output_nedwam/TSA_K13_0.PS */


   /* initialisatie */
   strcpy(nedwam_outputfilenaam, "\0");

   if (strcmp(OS, "WINDOWS") == 0)                 /* WINDOWS */
      strcpy(nedwam_outputfilenaam, "output_nedwam\\");
   else                                            /* UNIX */
      strcpy(nedwam_outputfilenaam, "output_nedwam/");


   strcat(nedwam_outputfilenaam, "TSA_");
   strcat(nedwam_outputfilenaam, station);
   strcat(nedwam_outputfilenaam, "_");

   sprintf(hulp_pag_no, "%d", pag_no);
   strcat(nedwam_outputfilenaam, hulp_pag_no);

   strcat(nedwam_outputfilenaam, ".PS");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                     TSA_Temp_Files_To_Postscript()                        */
/*                                                                           */
/*****************************************************************************/
int TSA_Temp_Files_To_Postscript(const char* grafiek_orientatie, const int scale_x_max, const int scale_y_max,
                                 const int pag_no, const char* station)
{
   /*
   // NB: DEZE PROCEDURE WORDT AANGEROEPEN PER STATION PER PS PAGINA PER GRAFIEK (bovenste, middelste, onderste)
   */

   char title[256];
   char line[256];
   char summary[256];
   char cic_summary[256];
   char buffer[10];
   char buffer1[10];
   char buffer2[10];
   char cic_hm0_summary[10];
   char cic_tm0_1_summary[10];
   char cic_bfi_summary[10];
   char cic_c4_summary[10];
   char cic_hmax_summary[10];
   char cic_ff_summary[10];
   char ff_hirlam_summary[10];
   char spectrum_hulp[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* CIC spectrum van JJJJMMDDUU + b.v. 6 uur (TSA mode) */

   const int of13        = 20;             /* vert. offset t.o.v. y2 voor title */
   const int of33        = 30;             /* vert. offset t.o.v. x-as (y1) voor summary regel */
   const int of34        = 40;             /* hor. offset t.o.v. x1 (y-as) voor summary regel aanduiding */
   const int of33b       = 43;             /* vert. offset t.o.v. x-as (y1) voor cic summary regel in model grafiek */
   const int of35        = 78;             /* vert. offset t.o.v. y1  voor ff-hirlam summary */
   const int of35b       = 65;             /* vert. offset t.o.v. y1  voor ff-cic summary */
   const int of37        = 20;             /* hor. offset t.o.v. x3 (links hoek onder richtingsgrafiek) voor ff-cic en ff-hirlam summary */
   int m;                                  /* teller frequenties */
   int v;                                  /* teller verwachtingen */
   int eind_indice;
   int num_grafiek_orientatie;
   int eind_index;
   int i;
   int cic_energy_spectrum_0_aanwezig;     /* boolean voor cic spectrum_0 */
   int cic_energy_spectrum_1_aanwezig;     /* boolean voor cic spectrum_1 */
   int cic_energy_spectrum_2_aanwezig;     /* boolean voor cic spectrum_2 */
   int cic_richtings_spectrum_0_aanwezig;  /* boolean voor cic spectrum_0 */
   int cic_richtings_spectrum_1_aanwezig;  /* boolean voor cic spectrum_1 */
   int cic_richtings_spectrum_2_aanwezig;  /* boolean voor cic spectrum_2 */
   int cic_hm0_spectrum_0_aanwezig;        /* boolean voor cic spectrum_0 */
   int cic_hm0_spectrum_1_aanwezig;        /* boolean voor cic spectrum_1 */
   int cic_hm0_spectrum_2_aanwezig;        /* boolean voor cic spectrum_2 */
   int obs_value_new;
   int obs_value_old;
   int start_punt_f_rule;                  /* i.v.m. f-rule */
   /*int cic_num_LFE; */                       /* cic summary regel */
   int cic_num_ff;                         /* cic summary regel */
   /*int cic_num_dd_gem_total; */              /* cic summary regel */
   /*int cic_num_dd_gem_LFE; */                /* cic summary regel */
   int hmax_ok;                            /* cic summary regel */
   int hm0_ok;                             /* cic summary regel */

   float f;                                /* i.v.m. f-rule */
   float f_waarde;                         /* i.v.m. f-rule */
   float f_eind_index;                     /* i.v.m. f-rule */

   const double max_richtings_shift = 120.0;
   double dir_new;
   double dir_old;
   /*char buffer[100];*/
   /*double cic_num_HE3; */                    /* cic summary regel */
   double cic_num_Hm0;                      /* cic summary regel */
   double cic_num_Tm0_1;                    /* cic summary regel */
   /*double cic_hulp_HE3_4;*/                   /* cic summary regel */  /* hulp (tussen) variable om van HE3 -> LFE te maken */
   double cic_num_Hmax;                       /* cic summary regel */


   /* forecast indice bepalen         */
   /* pag_no 0: indice 0, 1, 2        */
   /* pag_no 1: indice 3, 4, 5        */
   /* etc.                            */
   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 0;
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 1;
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 2;

   v = pag_no * 3 + num_grafiek_orientatie;         /* 3 verwachtingen op 1 pagina */


   if (v < NUM_VERWACHTINGEN)                       /* omdat op laatste pagina kunnen minder dan 3 verwachtingen staan */
   {

      /*
      //////////// title per grafiek (3 grafieken op 1 pagina) /////////////
      */

      Bepaal_Title_Per_Grafiek(grafiek_orientatie, pag_no, title, station);

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
      sprintf(buffer1, "%d", x1);
      sprintf(buffer2, "%d", y2 + of13);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %title\n");
      fprintf(os_ps, "%s", line);

      /* show (title) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, title);
      strcat(line, ")");
      strcat(line, " show      %title\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);



      /*
      //////// nedwam variance density  (linker paneel) //////////
      */

      /* newpath obs area */
      strcpy(line, "\0");
      strcpy(line, "newpath         %nedwam area\n");
      fprintf(os_ps, "%s", line);

      /* color obs area */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_obs_ZEE);
      strcat(line, " setrgbcolor %nedwam area\n");
      fprintf(os_ps, "%s", line);

      /* moveto oorsprong (x1,y1) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1);
      sprintf(buffer2, "%d", _y1);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %nedwam area oorsprong\n");
      fprintf(os_ps, "%s", line);



      /* afh. van scale_x_max soms niet alle frequentie waarden in de grafiek zetten */
      for (m = 1 ; m < NUM_FREQUENTIES; m++)
      {
         if ( FR[m] > ((double)scale_x_max / (double)x_factor) )   /* NB scale_x_max = 4, 3 of 2; x_factor = 10 */
         {
            eind_indice = m /*-1*/;      /* de laatste indice valt er dus net buiten, dit wordt dan in de grafiek precies de rechterkant van de grafiek */
            break;
         } /* if ( FR[m] > ((double)scale_x_max / (double)x_factor)) ) */
      } /* for (m = 1 ; m < NUM_FREQUENTIES; m++) */


      for (m = 0; m <= eind_indice - 1; m++)
      {
         /* lineto nedwam area */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';


         sprintf(buffer1, "%d", (int)(x1 + (FR[m] * (of4 * x_factor)) + 0.5) );      /* x-coordinaat */
         sprintf(buffer2, "%d", (int)(_y1 + (EMN1DIM[m][v] * of3 / y_factor_energy) + 0.5) );   /* y-coordinaat */


         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " lineto   %nedwam area\n");
         fprintf(os_ps, "%s", line);

      } /*  for (m = 0; m <= eind_indice -1; m++) */

      /* lineto eindpunt (x2,'snijding met rechter y-as') */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x2);
      /*sprintf(buffer2, "%d", _y1);*/
      sprintf(buffer2, "%d", (int)(_y1 + (EMN1DIM[eind_indice][v] * of3 / y_factor_energy) + 0.5) );   /* y-coordinaat */
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %nedwam area rechter y-as\n");
      fprintf(os_ps, "%s", line);

      /* lineto eindpunt (x2,y1) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x2);
      sprintf(buffer2, "%d", _y1);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %nedwam area eindpunt\n");
      fprintf(os_ps, "%s", line);

      /* closepath (nedwam area) */
      strcpy(line, "\0");
      strcpy(line, "closepath         %nedwam area\n");
      fprintf(os_ps, "%s", line);

      /* fill nedwam area */
      strcpy(line, "\0");
      strcpy(line, "fill           %nedwam area\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);



      /*
      ////////////// hirlam wind richting (rechter paneel) (nb moet voor NEDWAM golf richting) ////////////
      */

      /* hirlam wind richting in de grafiek zetten */
      if ((DDD_hirlam[v] >= 0) && (DDD_hirlam[v] <= 360))           /* 0 ok ?? */
      {
         /* newpath wind richting */
         strcpy(line, "\0");
         strcpy(line, "newpath         %dd (hirlam wind)\n");
         fprintf(os_ps, "%s", line);

         /* color wind richting */
         strcpy(line, "\0");
         strcpy(line, R_G_B_color_dd);
         strcat(line, " setrgbcolor %dd (hirlam wind)\n");
         fprintf(os_ps, "%s", line);

         /* setlinewidth wind richting */
         strcpy(line, "\0");
         strcpy(line, "0.2 setlinewidth   %dd (hirlam wind)\n");
         fprintf(os_ps, "%s", line);

         /* moveto wind richting */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", x3);                                                         /* x-coordinaat */
         sprintf(buffer2, "%d", _y1 + ( (int)(DDD_hirlam[v] * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " moveto   %dd (hirlam wind)\n");
         fprintf(os_ps, "%s", line);

         /* lineto wind richting */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", x4);                                                         /* x-coordinaat */
         sprintf(buffer2, "%d", _y1 + ( (int)(DDD_hirlam[v] * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " lineto   %dd (hirlam wind)\n");
         fprintf(os_ps, "%s", line);

         /* stroke wind richting */
         strcpy(line, "\0");
         strcat(line, "stroke   %dd (hirlam wind)\n");
         fprintf(os_ps, "%s", line);

         /* setlinewidth reset */
         strcpy(line, "\0");
         strcpy(line, "0.2 setlinewidth   %reset\n");
         fprintf(os_ps, "%s", line);

         /* lege regel */
         strcpy(line, "\n");
         fprintf(os_ps, "%s", line);

      } /* if ((DDD_hirlam[v] >= 0) && (DDD_hirlam[v] <= 360))  */


      /*
      /////////// nedwam golf direction  (rechter paneel) //////
      */

      /* newpath nedwam dir. */
      strcpy(line, "\0");
      strcpy(line, "newpath         %nedwam dir\n");
      fprintf(os_ps, "%s", line);

      /* color nedwam dir. */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_obs_ZEE);
      strcat(line, " setrgbcolor %nedwam dir\n");
      fprintf(os_ps, "%s", line);

      /* setlinewidth nedwam dir */
      strcpy(line, "\0");
      strcpy(line, "1.0 setlinewidth    %nedwam dir\n");
      fprintf(os_ps, "%s", line);


      /* afh. van scale_x_max soms niet alle frequentie waarden in de grafiek zetten */
      for (m = 1 ; m < NUM_FREQUENTIES; m++)
      {
         if ( FR[m] > ((double)scale_x_max / (double)x_factor) )   /* NB scale_x_max = 4, 3 of 2 */
         {
            eind_indice = m /*- 1*/;      /* laatste indice dus 'buiten grafiek' -> wordt opgellost bij plotten laatste punt */
            break;
         } /* if ( FR[m] > ((double)scale_x_max / (double)x_factor)) ) */
      } /* for (m = 1 ; m < NUM_FREQUENTIES; m++) */


      dir_old = MAXFLOAT;                     /* initialisatie */
      for (m = 0; m <= eind_indice - 1; m++)  /* richting(en) buiten grafiek gewoon niet plotten */
      {
         /* richting omzetten naar graden en ook de richting omzetten naar richting van-waar-vandaan */
         dir_new = (THQSP[m][v] * DEG) + 180.0;
         if (dir_new > 360.0)
            dir_new -= 360.0;

         if ((fabs(dir_new - dir_old) < max_richtings_shift) && (dir_old != MAXFLOAT) && (dir_new >= 0.0 && dir_new <= 360.0))   /* o.a. bij m = 0 (beginpunt) zal dit altijd het geval zijn */
         {
            /* lineto nedwam richting */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", (int)(x3 + (FR[m] * (of4 * x_factor)) + 0.5) );          /* x-coordinaat */
            sprintf(buffer2, "%d", (int)(_y1 + (dir_new * of3b / y_factor_richting) + 0.5) );   /* y-coordinaat */
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " lineto   %nedwam dir\n");
            fprintf(os_ps, "%s", line);

            /* stroke nedwam richting */
            strcpy(line, "\0");
            strcpy(line, "stroke      %nedwam dir\n");
            fprintf(os_ps, "%s", line);

            /* moveto nedwam richting */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", (int)(x3 + (FR[m] * (of4 * x_factor)) + 0.5) );          /* x-coordinaat */
            sprintf(buffer2, "%d", (int)(_y1 + (dir_new * of3b / y_factor_richting) + 0.5) );   /* y-coordinaat */
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %nedwam dir\n");
            fprintf(os_ps, "%s", line);
         } /*  if ((fabs(dir_new - dir_old) < max_richtings_shift) etc. */
         else /* 'nieuw' beginpunt */
         {
            /* moveto nedwam richting */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", (int)(x3 + (FR[m] * (of4 * x_factor)) + 0.5) );          /* x-coordinaat */
            sprintf(buffer2, "%d", (int)(_y1 + (dir_new * of3b / y_factor_richting) + 0.5) );   /* y-coordinaat */
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %nedwam dir\n");
            fprintf(os_ps, "%s", line);
         } /* else ('nieuw' beginpunt) */

         dir_old = dir_new;

      } /* for (m = 0; m <= eind_indice - 1; m++) */

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);



      /*
      ////////////// 0,0005*f**-5 (linker paneel) ////////////
      */

      /* newpath f-rule */
      strcpy(line, "\0");
      strcpy(line, "newpath         %f-rule\n");
      fprintf(os_ps, "%s", line);

      /* color f-rule */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_f_rule);
      strcat(line, " setrgbcolor %f-rule\n");
      fprintf(os_ps, "%s", line);

      /* setlinewidth f-rule */
      strcpy(line, "\0");
      strcpy(line, "0.5 setlinewidth   %f-rule\n");/*strcpy(line, "1.0 setlinewidth   %f-rule\n");*/
      fprintf(os_ps, "%s", line);

      if (scale_x_max == 4)                               /* t/m 0.4 Hz */
         f_eind_index = 0.4;
      else if (scale_x_max == 3)                          /* t/m 0.3 Hz */
         f_eind_index = 0.3;
      else if  (scale_x_max == 2)                         /* t/m 0.2 Hz */
         f_eind_index = 0.2;
      else
         f_eind_index = 0.4;                              /* als het goed is komt hij hier nooit in */


      start_punt_f_rule = 0;
      for (f = 0.05; f <= f_eind_index; f += 0.001)       /* 0.05 --> geeft met de f-rule al 1600m2/Hz */
      {
         f_waarde = 0.0005 / (f * f * f * f * f);         /* 0.0005*f**-5 --> 0.0005 * (1 / f**5) */

         if (f_waarde <= scale_y_max)
         {
            if (start_punt_f_rule == 0)                   /* dus het eerste te plotten punt */
            {
               /* moveto f-rule */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x1 + (int)(f * x_factor * of4 + 0.5));                     /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ((int)(f_waarde * of3 / y_factor_energy + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " moveto   %f-rule\n");
               fprintf(os_ps, "%s", line);

               start_punt_f_rule = 1;
            }
            else
            {
               /* lineto f-rule */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x1 + (int)(f * x_factor * of4 + 0.5));                     /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ((int)(f_waarde * of3 / y_factor_energy + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %f-rule\n");
               fprintf(os_ps, "%s", line);
            } /* else */
         } /* if (f_waarde <= scale_x_max) */
      } /* for (f = 0; f <= eind_index; f += 0.01) */

      /* lijn trekken als er minmaal 1 punt aanwezig is */
      if (start_punt_f_rule == 1)
      {
         /* stroke f-rule */
         strcpy(line, "\0");
         strcat(line, "stroke   %f-rule\n");
         fprintf(os_ps, "%s", line);
      }

      /* setlinewidth reset */
      strcpy(line, "\0");
      strcpy(line, "0.2 setlinewidth   %reset\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);


      /*
      ////////////// cic variance density (linker grafiek) ////////////
      */
      if (INT_VERWACHTINGEN == 3)
      {
         /* alleen indices uit spectrum array met energy inlezen */
         if (scale_x_max == 4)                           /* alleen uit het spectrum waarden t/m 0.4 Hz lezen */
            eind_index = SPECTRUM_ENERGY_0_4;
         else if (scale_x_max == 3)                      /* alleen uit het spectrum waarden t/m 0.3 Hz lezen */
            eind_index = SPECTRUM_ENERGY_0_3;
         else if  (scale_x_max == 2)                     /* alleen uit het spectrum waarden t/m 0.2 Hz lezen */
            eind_index = SPECTRUM_ENERGY_0_2;
         else
            eind_index = END_SPECTRUM_ENERGY;            /* als het goed is kont hij hier nooit in */


         /*
         // testen op welke CIC HM0 in spectrum arrays niet leeg is (bij HM0 betekend 0 echt golfhoogte 0)
         */

         /*
         // controle aanwezigheid CIC HM0 data bij de bovenste grafiek NEDWAM verwachting
         */
         cic_hm0_spectrum_0_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         if (strcmp(spectrum_0[HM0_SPECTRUM], "\0") != 0)            /* dus een niet lege array plaats */
         {
            cic_hm0_spectrum_0_aanwezig = 1;
         }

         /*
         //  controle aanwezigheid CIC HM0 data bij de middelste grafiek NEDWAM verwachting
         */
         cic_hm0_spectrum_1_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         if (strcmp(spectrum_1[HM0_SPECTRUM], "\0") != 0)            /* dus een niet lege array plaats */
         {
            cic_hm0_spectrum_1_aanwezig = 1;
         }

         /*
         // controle aanwezigheid CIC HM0 data bij de onderste grafiek NEDWAM verwachting
         */
         cic_hm0_spectrum_2_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         if (strcmp(spectrum_2[HM0_SPECTRUM], "\0") != 0)            /* dus een niet lege array gevonden */
         {
            cic_hm0_spectrum_2_aanwezig = 1;
         }



         /*
         // testen op welke cic spectrums niet leeg zijn en niet 0 zijn (niet aanwezig i= 0 voor energy bij Evert)
         */

         /*
         // controle aanwezigheid CIC energy data bij de bovenste grafiek NEDWAM verwachting
         */
         cic_energy_spectrum_0_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++)
         {
            if ( (strcmp(spectrum_0[i], "\0") != 0) && (atoi(spectrum_0[i]) > 0) )            /* dus een niet lege array plaats gevonden */
            {
                cic_energy_spectrum_0_aanwezig = 1;
                break;
            }
         } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++) */

         /*
         //  controle aanwezigheid CIC energy data bij de middelste grafiek NEDWAM verwachting
         */
         cic_energy_spectrum_1_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++)
         {
            if ( (strcmp(spectrum_1[i], "\0") != 0) && (atoi(spectrum_1[i]) > 0) )           /* dus een niet lege array plaats gevonden */
            {
                cic_energy_spectrum_1_aanwezig = 1;
                break;
            }
         } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++) */

         /*
         // controle energy aanwezigheid CIC data bij de onderste grafiek NEDWAM verwachting
         */
         cic_energy_spectrum_2_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++)
         {
            if ( (strcmp(spectrum_2[i], "\0") != 0) && (atoi(spectrum_2[i]) > 0) )          /* dus een niet lege array plaats gevonden */
            {
                cic_energy_spectrum_2_aanwezig = 1;
                break;
            }
         } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++) */


         /*
         // aanwezige CIC energy data bij de bovenste grafiek NEDWAM verwachting
         */
         if (num_grafiek_orientatie == 0 && cic_energy_spectrum_0_aanwezig == 1)
         {
            /* newpath obs line */
            strcpy(line, "\0");
            strcpy(line, "newpath         %obs line\n");
            fprintf(os_ps, "%s", line);

            /* color obs line */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %obs line\n");
            fprintf(os_ps, "%s", line);

            /* moveto obs line oorsprong (x1,y1) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1);
            sprintf(buffer2, "%d", _y1);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %obs line oorsprong\n");
            fprintf(os_ps, "%s", line);

            for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++)
            {
               /* lineto obs line */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x1 + ((i - BEGIN_SPECTRUM_ENERGY) * of4 / x_factor));      /* x-coordinaat */
               /*sprintf(buffer2, "%d", _y1 + ((int)(atof(regel) / 100000 * of3 / y_factor_energy + 0.5)) );*/   /* y-coordinaat */
               sprintf(buffer2, "%d", _y1 + ((int)(atof(spectrum_0[i]) / 100000 * of3 / y_factor_energy + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %obs line\n");
               fprintf(os_ps, "%s", line);
            } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++) */

            /* stroke cic obs energy */
            strcpy(line, "\0");
            strcat(line, "stroke   %obs\n");
            fprintf(os_ps, "%s", line);

            /* lege regel */
            strcpy(line, "\n");
            fprintf(os_ps, "%s", line);

         } /* if (num_grafiek_orientatie == 0 && spectrum_2_aanwezig == 1) */

         /*
         // aanwezige CIC energy data bij de middelste grafiek NEDWAM verwachting
         */
         if (num_grafiek_orientatie == 1 && cic_energy_spectrum_1_aanwezig == 1)
         {
            /* newpath obs line */
            strcpy(line, "\0");
            strcpy(line, "newpath         %obs line\n");
            fprintf(os_ps, "%s", line);

            /* color obs line */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %obs line\n");
            fprintf(os_ps, "%s", line);

            /* moveto obs line oorsprong (x1,y1) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1);
            sprintf(buffer2, "%d", _y1);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %obs line oorsprong\n");
            fprintf(os_ps, "%s", line);

            for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++)
            {
               /* lineto obs line */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x1 + ((i - BEGIN_SPECTRUM_ENERGY) * of4 / x_factor));      /* x-coordinaat */
               /*sprintf(buffer2, "%d", _y1 + ((int)(atof(regel) / 100000 * of3 / y_factor_energy + 0.5)) );*/   /* y-coordinaat */
               sprintf(buffer2, "%d", _y1 + ((int)(atof(spectrum_1[i]) / 100000 * of3 / y_factor_energy + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %obs line\n");
               fprintf(os_ps, "%s", line);
            } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++) */

            /* stroke cic obs energy */
            strcpy(line, "\0");
            strcat(line, "stroke   %obs\n");
            fprintf(os_ps, "%s", line);

            /* lege regel */
            strcpy(line, "\n");
            fprintf(os_ps, "%s", line);

         } /* if (num_grafiek_orientatie == 1 && spectrum_1_aanwezig == 1) */


         /*
         // aanwezige CIC energy data bij de onderste grafiek NEDWAM verwachting
         */
         if (num_grafiek_orientatie == 2 && cic_energy_spectrum_2_aanwezig == 1)
         {
            /* newpath obs line */
            strcpy(line, "\0");
            strcpy(line, "newpath         %obs line\n");
            fprintf(os_ps, "%s", line);

            /* color obs line */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %obs line\n");
            fprintf(os_ps, "%s", line);

            /* moveto obs line oorsprong (x1,y1) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1);
            sprintf(buffer2, "%d", _y1);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %obs line oorsprong\n");
            fprintf(os_ps, "%s", line);

            for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++)
            {
               /* lineto obs line */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x1 + ((i - BEGIN_SPECTRUM_ENERGY) * of4 / x_factor));      /* x-coordinaat */
               /*sprintf(buffer2, "%d", _y1 + ((int)(atof(regel) / 100000 * of3 / y_factor_energy + 0.5)) );*/   /* y-coordinaat */
               sprintf(buffer2, "%d", _y1 + ((int)(atof(spectrum_2[i]) / 100000 * of3 / y_factor_energy + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %obs line\n");
               fprintf(os_ps, "%s", line);
            } /* for (i = BEGIN_SPECTRUM_ENERGY; i <= eind_index; i++) */

            /* stroke cic obs energy */
            strcpy(line, "\0");
            strcat(line, "stroke   %obs\n");
            fprintf(os_ps, "%s", line);

            /* lege regel */
            strcpy(line, "\n");
            fprintf(os_ps, "%s", line);

         } /* if (num_grafiek_orientatie == 2 && spectrum_2_aanwezig == 1) */
      } /* if (INT_VERWACHTINGEN == 3) */



      /*
      ////////////// cic waves direction (rechter grafiek) ////////////
      */
      if (INT_VERWACHTINGEN == 3)
      {

         /* alleen regels (komt overeen met indices uit spectrum array) met richting inlezen */
         if (scale_x_max == 4)                           /* alleen uit het spectrum waarden t/m 0.4 Hz lezen */
            eind_index = SPECTRUM_RICHTING_0_4;
         else if (scale_x_max == 3)                      /* alleen uit het spectrum waarden t/m 0.3 Hz lezen */
            eind_index = SPECTRUM_RICHTING_0_3;
         else if  (scale_x_max == 2)                     /* alleen uit het spectrum waarden t/m 0.2 Hz lezen */
            eind_index = SPECTRUM_RICHTING_0_2;
         else
            eind_index = END_SPECTRUM_ENERGY;            /* als het goed is komt hij hier nooit in */


         /* testen op welke cic (richting)spectrums niet leeg zijn */

         /*
         // controle aanwezigheid CIC richtings data behorende bij de bovenste grafiek NEDWAM verwachting
         */
         cic_richtings_spectrum_0_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++)
         {
            if ( (strcmp(spectrum_0[i], "\0") != 0) && (atoi(spectrum_0[i]) > 0) )  /* dus een niet lege array plaats gevonden */
            {
                cic_richtings_spectrum_0_aanwezig = 1;
                break;
            }
         } /* for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++) */

         /*
         // controle aanwezigheid CIC richtings data bij de middelste grafiek NEDWAM verwachting
         */
         cic_richtings_spectrum_1_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++)
         {
            if ( (strcmp(spectrum_1[i], "\0") != 0) && (atoi(spectrum_1[i]) > 0) )  /* dus een niet lege array plaats gevonden */
            {
                cic_richtings_spectrum_1_aanwezig = 1;
                break;
            }
         } /* for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++) */

         /*
         // controle richtings aanwezigheid CIC data bij de onderste grafiek NEDWAM verwachting
         */
         cic_richtings_spectrum_2_aanwezig = 0;                            /* initialisatie = niet aanwezig */
         for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++)
         {
            if ( (strcmp(spectrum_2[i], "\0") != 0) && (atoi(spectrum_2[i]) > 0) ) /* dus een niet lege array plaats gevonden */
            {
                cic_richtings_spectrum_2_aanwezig = 1;
                break;
            }
         } /* for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++) */



         /*
         // aanwezige CIC golven richtings data bij de bovenste grafiek NEDWAM verwachting
         */
         if (num_grafiek_orientatie == 0 && cic_richtings_spectrum_0_aanwezig == 1)
         {
            /* newpath obs richting */
            strcpy(line, "\0");
            strcpy(line, "newpath         %obs richting\n");
            fprintf(os_ps, "%s", line);

            /* color obs richting */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %obs richting\n");
            fprintf(os_ps, "%s", line);


            /* setlinewidth obs-richting */
            /*
            //strcpy(line, "\0");
            //strcpy(line, "1.0 setlinewidth   %obs richting\n");
            //fprintf(os_ps, "%s", line);
            */

            /* initialisatie */
            obs_value_old = MAXINT;

            for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++)
            {
               if (strcmp(spectrum_0[i], "\0") != 0)           /* altijd TRUE ? */
               {
                  obs_value_new = atoi(spectrum_0[i]);

                  /* n.b. obs_value_new = 0 betekent hier (bij Evert Bouws) geen waarneming/niet bekende waarde */
                  if ( (obs_value_new > 0) && (obs_value_new <= 360) && (obs_value_old != MAXINT) && (abs(obs_value_new - obs_value_old) < max_richtings_shift) )
                  {
                     /* lineto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_0[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " lineto   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                     /* stroke obs richting */
                     strcpy(line, "\0");
                     strcat(line, " stroke   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                     /* moveto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_0[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " moveto   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                  } /* if ( (obs_value_old != MAXINT) && (abs(obs_value_new - obs_value_old) < max_richtings_shift) ) */
                  else
                  {
                     /* moveto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_0[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " moveto   %obs richting\n");
                     fprintf(os_ps, "%s", line);
                  } /* else */

                  obs_value_old = obs_value_new;

               } /* if ((strcmp(regel, "\0") != 0) */
               else /* lege regel */
                  obs_value_old = MAXINT;

            } /* for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++) */
         } /* if (num_grafiek_orientatie == 0 && cic_richtings_spectrum_0_aanwezig == 1) */



         /*
         // aanwezige CIC golven richtings data bij de middelste grafiek NEDWAM verwachting
         */
         if (num_grafiek_orientatie == 1 && cic_richtings_spectrum_1_aanwezig == 1)
         {
            /* newpath obs richting */
            strcpy(line, "\0");
            strcpy(line, "newpath         %obs richting\n");
            fprintf(os_ps, "%s", line);

            /* color obs richting */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %obs richting\n");
            fprintf(os_ps, "%s", line);


            /* setlinewidth obs-richting */
            /*
            //strcpy(line, "\0");
            //strcpy(line, "1.0 setlinewidth   %obs richting\n");
            //fprintf(os_ps, "%s", line);
            */

            /* initialisatie */
            obs_value_old = MAXINT;

            for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++)
            {
               if (strcmp(spectrum_1[i], "\0") != 0)           /* altijd TRUE ? */
               {
                  obs_value_new = atoi(spectrum_1[i]);

                  /* n.b. obs_value_new = 0 betekent hier (bij Evert Bouws) geen waarneming/niet bekende waarde */
                  if ( (obs_value_new > 0) && (obs_value_new <= 360) && (obs_value_old != MAXINT) && (abs(obs_value_new - obs_value_old) < max_richtings_shift) )
                  {
                     /* lineto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_1[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " lineto   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                     /* stroke obs richting */
                     strcpy(line, "\0");
                     strcat(line, " stroke   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                     /* moveto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_1[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " moveto   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                  } /* if ( (obs_value_old != MAXINT) && (abs(obs_value_new - obs_value_old) < max_richtings_shift) ) */
                  else
                  {
                     /* moveto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_1[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " moveto   %obs richting\n");
                     fprintf(os_ps, "%s", line);
                  } /* else */

                  obs_value_old = obs_value_new;

               } /* if ((strcmp(regel, "\0") != 0) */
               else /* lege regel */
                  obs_value_old = MAXINT;

            } /* for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++) */
         } /* if (num_grafiek_orientatie == 1 && cic_richtings_spectrum_1_aanwezig == 1) */


         /*
         // aanwezige CIC golven richtings data bij de onderste grafiek NEDWAM verwachting
         */
         if (num_grafiek_orientatie == 2 && cic_richtings_spectrum_2_aanwezig == 1)
         {
            /* newpath obs richting */
            strcpy(line, "\0");
            strcpy(line, "newpath         %obs richting\n");
            fprintf(os_ps, "%s", line);

            /* color obs richting */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %obs richting\n");
            fprintf(os_ps, "%s", line);


            /* setlinewidth obs-richting */
            /*
            //strcpy(line, "\0");
            //strcpy(line, "1.0 setlinewidth   %obs richting\n");
            //fprintf(os_ps, "%s", line);
            */

            /* initialisatie */
            obs_value_old = MAXINT;

            for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++)
            {
               if (strcmp(spectrum_2[i], "\0") != 0)           /* altijd TRUE ? */
               {
                  obs_value_new = atoi(spectrum_2[i]);

                  /* n.b. obs_value_new = 0 betekent hier (bij Evert Bouws) geen waarneming/niet bekende waarde */
                  if ( (obs_value_new > 0) && (obs_value_new <= 360) && (obs_value_old != MAXINT) && (abs(obs_value_new - obs_value_old) < max_richtings_shift) )
                  {
                     /* lineto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_2[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " lineto   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                     /* stroke obs richting */
                     strcpy(line, "\0");
                     strcat(line, " stroke   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                     /* moveto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_2[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " moveto   %obs richting\n");
                     fprintf(os_ps, "%s", line);

                  } /* if ( (obs_value_old != MAXINT) && (abs(obs_value_new - obs_value_old) < max_richtings_shift) ) */
                  else
                  {
                     /* moveto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(spectrum_2[i]) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " moveto   %obs richting\n");
                     fprintf(os_ps, "%s", line);
                  } /* else */

                  obs_value_old = obs_value_new;

               } /* if ((strcmp(regel, "\0") != 0) */
               else /* lege regel */
                  obs_value_old = MAXINT;

            } /* for (i = BEGIN_SPECTRUM_RICHTING; i <= eind_index; i++) */
         } /* if (num_grafiek_orientatie == 2 && cic_richtings_spectrum_2_aanwezig == 2) */
      } /* if (INT_VERWACHTINGEN == 3) */

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);


      /*
      ////////////// cic wind direction (rechter grafiek) ////////////
      */
      if (INT_VERWACHTINGEN == 3)
      {

         /*
         // aanwezige CIC wind richtings data bij de bovenste grafiek NEDWAM verwachting
         */
         /* NB spectrum_0[DD_WIND] staat in tientallen graden */
         if (num_grafiek_orientatie == 0 && (strcmp(spectrum_0[DD_WIND], "\0") != 0))     /* dus een niet leeg */
         {
            /* cic wind richting in de grafiek zetten */
            if ( ((atoi(spectrum_0[DD_WIND]) * 10) >= 0) && ((atoi(spectrum_0[DD_WIND]) * 10) <= 360) ) /* 0 ok ?? */
            {
               /* newpath cic wind richting */
               strcpy(line, "\0");
               strcpy(line, "newpath         %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* color cic wind  richting */
               strcpy(line, "\0");
               strcpy(line, R_G_B_color_cic_dd_in_forecast_ZEE );
               strcat(line, " setrgbcolor %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* setlinewidth cic wind richting */
               strcpy(line, "\0");
               strcpy(line, "0.2 setlinewidth   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* dash line cic wind richting */
               strcpy(line, "\0");
               strcat(line, "[2 3] 0 setdash  %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* moveto cic wind richting */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x3);                                                         /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ( (int)((atoi(spectrum_0[DD_WIND]) * 10) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " moveto   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* lineto cic wind richting */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x4);                                                         /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ( (int)((atoi(spectrum_0[DD_WIND]) * 10) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* stroke cic richting */
               strcpy(line, "\0");
               strcat(line, "stroke   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* reset (op solid) cic wind richting */
               strcpy(line, "\0");
               strcat(line, "[]0 setdash     %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* setlinewidth reset */
               strcpy(line, "\0");
               strcpy(line, "0.2 setlinewidth   %reset\n");
               fprintf(os_ps, "%s", line);

               /* lege regel */
               strcpy(line, "\n");
               fprintf(os_ps, "%s", line);

            } /* if ( ((atoi(spectrum_0[DD_WIND]) * 10) >= 0) && ((atoi(spectrum_0[DD_WIND]) * 10) <= 360) ) */
         } /* (num_grafiek_orientatie == 0 && (strcmp(spectrum_0[DD_WIND], "\0") != 0)) */


         /*
         // aanwezige CIC wind richtings data bij de middelste grafiek NEDWAM verwachting
         */
         /* NB spectrum_1[DD_WIND] staat in tientallen graden */
         if (num_grafiek_orientatie == 1 && (strcmp(spectrum_1[DD_WIND], "\0") != 0))            /* dus een niet leeg */
         {
            /* cic wind richting in de grafiek zetten */
            if ( ((atoi(spectrum_1[DD_WIND]) * 10) >= 0) && ((atoi(spectrum_1[DD_WIND]) * 10) <= 360) ) /* 0 ok ?? */
            {
               /* newpath cic wind richting */
               strcpy(line, "\0");
               strcpy(line, "newpath         %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* color cic wind  richting */
               strcpy(line, "\0");
               strcpy(line, R_G_B_color_cic_dd_in_forecast_ZEE );
               strcat(line, " setrgbcolor %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* setlinewidth cic wind richting */
               strcpy(line, "\0");
               strcpy(line, "0.2 setlinewidth   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* dash line cic wind richting */
               strcpy(line, "\0");
               strcat(line, "[2 3] 0 setdash  %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* moveto cic wind richting */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x3);                                                         /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ( (int)((atoi(spectrum_1[DD_WIND]) * 10) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " moveto   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* lineto cic wind richting */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x4);                                                         /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ( (int)((atoi(spectrum_1[DD_WIND]) * 10) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* stroke cic richting */
               strcpy(line, "\0");
               strcat(line, "stroke   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* reset (op solid) cic wind richting */
               strcpy(line, "\0");
               strcat(line, "[]0 setdash     %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* setlinewidth reset */
               strcpy(line, "\0");
               strcpy(line, "0.2 setlinewidth   %reset\n");
               fprintf(os_ps, "%s", line);

               /* lege regel */
               strcpy(line, "\n");
               fprintf(os_ps, "%s", line);

            } /* if ( ((atoi(spectrum_1[DD_WIND]) * 10) >= 0) && ((atoi(spectrum_1[DD_WIND]) * 10) <= 360) ) */
         } /* (num_grafiek_orientatie == 1 && (strcmp(spectrum_1[DD_WIND], "\0") != 0)) */


         /*
         // aanwezige CIC wind richtings data bij de onderste grafiek NEDWAM verwachting
         */
         /* NB spectrum_2[DD_WIND] staat in tientallen graden */
         if (num_grafiek_orientatie == 2 && (strcmp(spectrum_2[DD_WIND], "\0") != 0)) /* dus een niet leeg */
         {
            /* cic wind richting in de grafiek zetten */
            if ( ((atoi(spectrum_2[DD_WIND]) * 10) >= 0) && ((atoi(spectrum_2[DD_WIND]) * 10) <= 360) ) /* 0 ok ?? */
            {
               /* newpath cic wind richting */
               strcpy(line, "\0");
               strcpy(line, "newpath         %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* color cic wind  richting */
               strcpy(line, "\0");
               strcpy(line, R_G_B_color_cic_dd_in_forecast_ZEE );
               strcat(line, " setrgbcolor %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* setlinewidth cic wind richting */
               strcpy(line, "\0");
               strcpy(line, "0.2 setlinewidth   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* dash line cic wind richting */
               strcpy(line, "\0");
               strcat(line, "[2 3] 0 setdash  %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* moveto cic wind richting */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x3);                                                         /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ( (int)((atoi(spectrum_2[DD_WIND]) * 10) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " moveto   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* lineto cic wind richting */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x4);                                                         /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ( (int)((atoi(spectrum_2[DD_WIND]) * 10) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* stroke cic richting */
               strcpy(line, "\0");
               strcat(line, "stroke   %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* reset (op solid) cic wind richting */
               strcpy(line, "\0");
               strcat(line, "[]0 setdash     %dd (cic wind)\n");
               fprintf(os_ps, "%s", line);

               /* setlinewidth reset */
               strcpy(line, "\0");
               strcpy(line, "0.2 setlinewidth   %reset\n");
               fprintf(os_ps, "%s", line);

               /* lege regel */
               strcpy(line, "\n");
               fprintf(os_ps, "%s", line);

            } /* if ( ((atoi(spectrum_2[DD_WIND]) * 10) >= 0) && ((atoi(spectrum_2[DD_WIND]) * 10) <= 360) ) */
         } /* (num_grafiek_orientatie == 2 && strcmp(spectrum_2[DD_WIND], "\0") != 0) */
      } /* if (INT_VERWACHTINGEN == 3) */


      /*
      ////////////// cic summary regel (alleen als ook hm0 spectrum aanwezig is) ////////////
      */
      if (INT_VERWACHTINGEN == 3)
      {
         /*
         // summary CIC data behorende bij de bovenste grafiek NEDWAM verwachting
         */

         /* initialisatie */
         for (i = 0; i < NUM_SPECTRUM; i++)
            strcpy(spectrum_hulp[i], "\0");                      


         if (num_grafiek_orientatie == 0 && cic_hm0_spectrum_0_aanwezig == 1)
         {
            for (i = 0; i < NUM_SPECTRUM; i++)
               strcpy(spectrum_hulp[i], spectrum_0[i]);
         }


         if (num_grafiek_orientatie == 1 && cic_hm0_spectrum_1_aanwezig == 1)
         {
            for (i = 0; i < NUM_SPECTRUM; i++)
               strcpy(spectrum_hulp[i], spectrum_1[i]);
         }


         if (num_grafiek_orientatie == 2 && cic_hm0_spectrum_2_aanwezig == 1)
         {
            for (i = 0; i < NUM_SPECTRUM; i++)
               strcpy(spectrum_hulp[i], spectrum_2[i]);
         }


         if ( (num_grafiek_orientatie == 0 && cic_hm0_spectrum_0_aanwezig == 1) ||
              (num_grafiek_orientatie == 1 && cic_hm0_spectrum_1_aanwezig == 1) ||
              (num_grafiek_orientatie == 2 && cic_hm0_spectrum_2_aanwezig == 1) )

         {
            /* initialisatie */
            hm0_ok = 0;
            hmax_ok = 0;

            for (i = 0; i < NUM_SPECTRUM; i++)
            {
               /* Hm0 */
               if (i == HM0_SPECTRUM)
               {
                  cic_hm0_summary[0] = '\0';

                  if (strcmp(spectrum_hulp[i], "\0") != 0)
                  {
                     buffer[0] = '\0';

                     strcat(cic_hm0_summary, "Hm0 = ");
                     cic_num_Hm0 = atof(spectrum_hulp[i]) / 1000;
                     sprintf(buffer, "%.1f", cic_num_Hm0);

                     if (strncmp(buffer, "0.0", 3) == 0)                   /* bij cic data betekent 0 niet waargenomen */
                        strcat(cic_hm0_summary, "-");
                     else
                     {
                        strcat(cic_hm0_summary, buffer);
                        hm0_ok = 1;
                     }

                     strcat(cic_hm0_summary, "m");
                  } /* if (strcmp(spectrum_hulp[i], "\0") != 0) */
                  else
                  {
                     strcat(cic_hm0_summary, "Hm0 = -m");
                  } /* else */
               } /* if (i == HM0_SPECTRUM) */


               /* Tm0-1 */
               if (i == TM0_1_SPECTRUM)
               {
                  cic_tm0_1_summary[0] = '\0';

                  if (strcmp(spectrum_hulp[i], "\0") != 0)
                  {
                     buffer[0] = '\0';

                     strcat(cic_tm0_1_summary, "Tm0-1 = ");
                     cic_num_Tm0_1 = atof(spectrum_hulp[i]) / 10;                     /* van 0.1 sec --> sec (.1f doet dit automatisch correct )*/
                     sprintf(buffer, "%.1f", cic_num_Tm0_1);

                     if (strncmp(buffer, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen */
                        strcat(cic_tm0_1_summary, "-");
                     else
                        strcat(cic_tm0_1_summary, buffer);

                     strcat(cic_tm0_1_summary, "s");
                  } /* if (strcmp(spectrum_hulp[i], "\0") != 0) */
                  else
                  {
                     strcat(cic_tm0_1_summary, "Tm0-1 = -s");
                  } /* else */

               } /* if (i == TM0_1_SPECTRUM) */


               /* ff */
               if (i == FF_WIND)
               {
                  cic_ff_summary[0] = '\0';

                  if (strcmp(spectrum_hulp[i], "\0") != 0)
                  {
                     buffer[0] = '\0';
                     strcat(cic_ff_summary, "ff = ");
                     cic_num_ff = atoi(spectrum_hulp[i]);

                     if ((cic_num_ff == 0) || (cic_num_ff == 99))     /* 99 betekent hier niet waargenomen; bij 0 kan het zijn niet waargenomen en ook windstil */
                        strcat(cic_ff_summary, "-");
                     else
                     {
                        sprintf(buffer, "%d", cic_num_ff);
                        strcat(cic_ff_summary, buffer);
                     }

                     strcat(cic_ff_summary, "kn  ");
                  } /* if (strcmp(spectrum_hulp[i], "\0") != 0) */
                  else
                  {
                     strcat(cic_ff_summary, "ff = -kn  ");
                  } /* else */

               } /* if (i == FF_SPECTRUM) */


               /* BFI */
               if (i == BFI_SPECTRUM)
               {
                  cic_bfi_summary[0] = '\0';

                  if (strcmp(spectrum_hulp[i], "\0") != 0)
                  {
                     strcat(cic_bfi_summary, "BFI = ");
                     strcat(cic_bfi_summary, spectrum_hulp[i]);
                  }
                  else
                  {
                     strcat(cic_bfi_summary, "BFI = -");
                  } /* else */

               } /* if (i == BFI_SPECTRUM) */

               /* C4 */
               if (i == C4_SPECTRUM)
               {
                  cic_c4_summary[0] = '\0';
                  if (strcmp(spectrum_hulp[i], "\0") != 0)
                  {
                     strcat(cic_c4_summary, "C4 = ");
                     strcat(cic_c4_summary, spectrum_hulp[i]);
                  }
                  else
                  {
                     strcat(cic_c4_summary, "C4 = -");
                  } /* else */

               } /* if (i == C4_SPECTRUM) */


               /* Hmax */
               if (i == HMAX_SPECTRUM)
               {
                  cic_hmax_summary[0] = '\0';

                  if (strcmp(spectrum_hulp[i], "\0") != 0)
                  {
                     buffer[0] = '\0';
                     strcat(cic_hmax_summary, "Hmax = ");
                     cic_num_Hmax = atof(spectrum_hulp[i]) / 1000;
                     sprintf(buffer, "%.1f", cic_num_Hmax);                /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */

                     if (strncmp(buffer, "0.0", 3) == 0)                   /* bij cic data betekent 0 niet waargenomen */
                        strcat(cic_hmax_summary, "-");
                     else
                     {
                        strcat(cic_hmax_summary, buffer);
                        hmax_ok = 1;
                     }
                     strcat(cic_hmax_summary, "m");
                  } /* if (strcmp(spectrum_hulp[i], "\0") != 0) */
                  else
                  {
                     strcat(cic_hmax_summary, "Hmax = -m");
                  } /* else */
               } /* if (i == HMAX_SPECTRUM) */

            } /* for (i = 0; i < NUM_SPECTRUM; i++) */



            /* totale summary regel samenstellen */
            cic_summary[0] = '\0';

            strcat(cic_summary, cic_hm0_summary);
            strcat(cic_summary, "  ");
            strcat(cic_summary, cic_tm0_1_summary);
            strcat(cic_summary, "  ");
            strcat(cic_summary, cic_bfi_summary);
            strcat(cic_summary, "  ");
            strcat(cic_summary, cic_c4_summary);
            strcat(cic_summary, "  ");
            strcat(cic_summary, cic_hmax_summary);
            strcat(cic_summary, "  ");

            /* totale summary regel toevoegen: Hmax / Hm0 */
            if (hmax_ok == 1 && hm0_ok == 1)
            {
               strcat(cic_summary, "Hmax/Hm0 = ");

               buffer[0] = '\0';
               sprintf(buffer, "%.1f", cic_num_Hmax / cic_num_Hm0);   /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */
               strcat(cic_summary, buffer);
            }
            else
               strcat(cic_summary, "Hmax/Hm0 = -");

         } /* if (num_grafiek_orientatie == 0 && cic_hm0_spectrum_0_aanwezig == 1) || etc. */



         /*
         // alleen cic summary line zetten als hij gevuld/aangemaakt is
         */
         if ( (num_grafiek_orientatie == 0 && cic_hm0_spectrum_0_aanwezig == 1) ||
              (num_grafiek_orientatie == 1 && cic_hm0_spectrum_1_aanwezig == 1) ||
              (num_grafiek_orientatie == 2 && cic_hm0_spectrum_2_aanwezig == 1) )
         {
            /* newpath cic summary */
            strcpy(line, "\0");
            strcpy(line, "newpath         %cic summary\n");
            fprintf(os_ps, "%s", line);

            /* font (cic summary) */
            strcpy(line, "\0");
            strcpy(line, font_keuze);
            fprintf(os_ps, "%s", line);

            /* font (cic summary) */
            strcpy(line, "\0");
            strcpy(line, "12 scalefont       %cic summary\n");
            fprintf(os_ps, "%s", line);

            /* font (cic summary) */
            strcpy(line, "\0");
            strcpy(line, "setfont            %cic summary\n");
            fprintf(os_ps, "%s", line);

            /* color cic summary */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %cic summary\n");
            fprintf(os_ps, "%s", line);

            /* moveto (cic summary) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x1 - of34);
            sprintf(buffer2, "%d", _y1 - of33b);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %cic summary\n");
            fprintf(os_ps, "%s", line);

            /* show (cic summary) */
            strcpy(line, "\0");
            strcpy(line, "(");
            strcat(line, cic_summary);
            strcat(line, ")");
            strcat(line, " show      %cic summary\n");
            fprintf(os_ps, "%s", line);

            /* lege regel */
            strcpy(line, "\n");
            fprintf(os_ps, "%s", line);


            /* ff-cic komt niet in summary line maar op andere plaats */

            /* newpath (ff-cic summary) */
            strcpy(line, "\0");
            strcpy(line, "newpath         %ff-cic summary\n");
            fprintf(os_ps, "%s", line);

            /* color (ff-cic summary) */
            strcpy(line, "\0");
            strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
            strcat(line, " setrgbcolor %ff-cic summary\n");
            fprintf(os_ps, "%s", line);

            /* moveto (ff-cic summary) */
            strcpy(line, "\0");
            buffer1[0] = '\0';
            buffer2[0] = '\0';
            sprintf(buffer1, "%d", x2 + of37);
            sprintf(buffer2, "%d", _y1 + of35b);
            strcpy(line, buffer1);
            strcat(line, " ");
            strcat(line, buffer2);
            strcat(line, " moveto   %ff-cic summary\n");
            fprintf(os_ps, "%s", line);

            /* show (ff-cic summary) */
            strcpy(line, "\0");
            strcpy(line, "(");
            strcat(line, cic_ff_summary);
            strcat(line, ")");
            strcat(line, " show      %ff-cic summary\n");
            fprintf(os_ps, "%s", line);

            /* lege regel */
            strcpy(line, "\n");
            fprintf(os_ps, "%s", line);

         } /* if ( (num_grafiek_orientatie == 0 && cic_hm0_spectrum_0_aanwezig == 1) || etc. */
      } /* if (INT_VERWACHTINGEN == 3) */



      /*
      //
      // NEDWAM summary regel onder elke grafiek samenstellen
      //
      // b.v. Hm0 = 2.39m  Tm0-1 = 5.9s  LFE = 10cm2 ff = 12kts dir-tot = 230 dir-LFE= 234
      // b.v. Hm0 = 2.39m  Tm0-1 = 5.9s  BFI = .. C4 = .. Kmax = ..   Hmax/Hm0.= .. 
      */

      Bepaal_Summary_Per_Grafiek(v, summary, station);

      /* color van summary regel */
      /*strcpy(R_G_B_color_obs_summary, R_G_B_color_obs_summary_ZEE);*/

      /* newpath summary */
      strcpy(line, "\0");
      strcpy(line, "newpath         %summary\n");
      fprintf(os_ps, "%s", line);

      /* font (summary) */
      strcpy(line, "\0");
      strcpy(line, font_keuze);
      fprintf(os_ps, "%s", line);

      /* font (summary) */
      strcpy(line, "\0");
      strcpy(line, "12 scalefont       %summary\n");
      fprintf(os_ps, "%s", line);

      /* font (summary) */
      strcpy(line, "\0");
      strcpy(line, "setfont            %summary\n");
      fprintf(os_ps, "%s", line);

      /* color summary */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_model_summary_ZEE);
      strcat(line, " setrgbcolor %summary\n");
      fprintf(os_ps, "%s", line);

      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 - of34);
      sprintf(buffer2, "%d", _y1 - of33);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);


      /* ff-hirlam summary apart */

      /* ff (hirlam wind snelheid) */
      ff_hirlam_summary[0] = '\0';
      buffer[0] = '\0';
      strcat(ff_hirlam_summary, "ff = ");
      if (FF_hirlam[v] != MAXFLOAT)
         sprintf(buffer, "%.0f", FF_hirlam[v]);    /* (afgeronde) knopen */
      else
         sprintf(buffer, "%s", "-");
      strcat(ff_hirlam_summary, buffer);
      strcat(ff_hirlam_summary, "kn  ");

      /* newpath (ff-hirlam summary) */
      strcpy(line, "\0");
      strcpy(line, "newpath         %ff-hirlam summary\n");
      fprintf(os_ps, "%s", line);

      /* color (ff-hirlam summary) */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_dd);
      strcat(line, " setrgbcolor %ff-hirlam summary\n");
      fprintf(os_ps, "%s", line);

      /* moveto (ff-hirlam summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x2 + of37);
      sprintf(buffer2, "%d", _y1 + of35);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %ff-hirlam summary\n");
      fprintf(os_ps, "%s", line);

      /* show (ff-hirlam summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, ff_hirlam_summary);
      strcat(line, ")");
      strcat(line, " show      %ff-hirlam summary\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);

   } /* if (v < NUM_VERWACHTINGEN) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                            Bepaal_Title_Per_Grafiek                       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Title_Per_Grafiek(const char* grafiek_orientatie, const int pag_no, char* title, const char* station)
{
   int num_grafiek_orientatie;
   struct tm time_valid;
   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   int forecast_uren;                     /* b.v. +12 voor verwachting van 12 uur vooruit t.o.v. JJJJJMMDDUU */
   int tijd_ok;
   int grafiek_orientatie_ok;
   char date_time[100];                   /* NB 100 = willekeutige groot-genoeg waarde */
   char run_time[100];                    /* NB 100 = willekeutige groot-genoeg waarde */
   char char_forecast_uren[3];


   /* initialisatie */
   title[0]              = '\0';
   /*tijd_ok               = 1;*/     /* = OK */
   grafiek_orientatie_ok = 1;     /* = OK */
   char_forecast_uren[0] = '\0';


   /* 3 verwachtingen per pagina */
   /* pagina 0: verwachting 1, verwachting 2, verwachting 3 (bov grafiek, midd grafiek, onderste grafiek) */
   /* pagina 1: verwachting 4, verwachting 5, verwachting 6 (bov grafiek, midd grafiek, onderste grafiek) */
   /* etc. */

   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 0;
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 1;
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 2;
   else
   {
     grafiek_orientatie_ok = 0;
   }

   /* b.v.*/
   /* JJJJJMMDDUU       : 2001093018            */
   /* pag_no (vanaf 0)  : 4                     */
   /* grafiek_orientatie: middelste_grafiek = 1 */
   /* INT_WAARNEMINGEN  : 3                     */
   /* forecast_uren = ((4 * 3)+ 1) * 3 = 39     */


   forecast_uren = ((pag_no * 3) + num_grafiek_orientatie) * INT_VERWACHTINGEN;
   sprintf(char_forecast_uren, "%02d", forecast_uren);

   /* jaar van argument list */
   strncpy(JJJJ, JJJJMMDDUU, 4);
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

   if (tijd_ok == 1 && grafiek_orientatie_ok == 1)
   {
      (void)strftime(run_time, 100 /*sizeof(date_time)*/, "%d-%m-%Y %H.00 UTC", &time_valid);
   }


   /*
   // valid for time b.v. 2001093018 + 39 = 2001100209
   */
   time_valid.tm_year = num_JJJJ - 1900;
   time_valid.tm_mon  = num_MM - 1;
   time_valid.tm_mday = num_DD;
   time_valid.tm_hour = num_UU + forecast_uren;
   time_valid.tm_min = 0;
   time_valid.tm_sec = 0;
   time_valid.tm_isdst = -1;                     /* daylight saving time: -1 = no info available */

   /* 'VALID' datum aanmaken */
   tijd_ok = 1;
   if (mktime(&time_valid) == -1)
      tijd_ok = 0;

   if (tijd_ok == 1 && grafiek_orientatie_ok == 1)
   {
#if 0
      (void)strftime(title, 256 /*sizeof(title)*/, "variance density   VALID %d-%m-%Y %H.00 UTC    direction", &time_valid);
#endif
      (void)strftime(date_time, 100 /*sizeof(date_time)*/, "%d-%m-%Y %H.00 UTC", &time_valid);

      strcpy(title, "");
      strcpy(title, "RUN ");
      strcat(title, run_time);                  /* b.v. 30-09-2001 18.00 UTC */
      strcat(title, " ");
      strcat(title, station);
      strcat(title, " +");
      strcat(title, char_forecast_uren);
      strcat(title, "hr ");
      strcat(title, "VALID ");
      strcat(title, date_time);                 /* b.v. 02-10-2001 09.00 UTC */
      strcat(title, "    ");
      strcat(title, "");
   }
   else
      strcpy(title, "unknown");


/* DIT GAAT GOED GEEFT HUIDIGE DATUM_TIJD AAN */
/*   time(&time_valid); */
/*   strcpy(title, ctime(&time_valid)); */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                         Bepaal_Scale_Y_X_Max_TSA                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Scale_Y_X_Max_TSA(const double max_energy, int *scale_y_max, int *scale_x_max)
{
   int i;
   int doorgaan;


   /* verticale schaalverdeling (scale_y) bij energy (variance density):
   //
   // 0 - 1 m2/Hz        (.. -    100000 .1cm2s)   --> scale_y_max = 1
   // 1 - 2 m2/Hz        (.. -    200000 .1cm2s)   --> scale_y_max = 2
   // 2 - 3 m2/Hz        (.. -    300000 .1cm2s)   --> scale_y_max = 3
   //     etc.
   //     etc.
   //
   //  9 - 10 m2/Hz       (.. -   1000000 .1cm2s)  --> scale_y_max = 10
   // 10 - 20 m2/Hz       (.. -   2000000 .1cm2s)  --> scale_y_max = 20
   // 20 - 30 m2/Hz       (.. -   3000000 .1cm2s)  --> scale_y_max = 30
   // 30 - 40 m2/Hz       (.. -   4000000 .1cm2s)  --> scale_y_max = 40
   //     etc.
   //     etc.
   //
   // 90 - 100 m2/Hz      (.. -  10000000 .1cm2s)  --> scale_y_max = 100
   // 100 - 110 m2/Hz     (.. -  11000000 .1cm2s)  --> scale_y_max = 110
   //     etc.
   //
   // 990 - 1000 m2/Hz    (.. - 100000000 .1cm2s)  --> scale_y_max = 1000 (n.b. max van type int op PC: 2,147,483,647)
   */

   /* horizontale schaalverdeling (scale_x) (voor zowel energy als richtings grafiek):
   //
   // 0    -  5.0 m2/Hz  (0       -  500000 .1cm2s)  --> scale_x_max = 4  (0.4 Hz)
   // 5.0  - 50.0 m2/Hz  (500000  - 5000000 .1cm2s)  --> scale_x_max = 3  (0.3 Hz)
   //      > 50.0 m2/Hz  (        > 5000000 .1cm2s)  --> scale_x_max = 2  (0.2 Hz)
   */



   /*
   ////////////  scale_y_max  ////////////////
   */

   /* scale_y_max range: 1 t/m 9 m2/Hz */
   doorgaan = 1;                               /* uitgangs situatie */
   for (i = 1; i < 10; i++)
   {
      if (max_energy < (double)i)
      {
         *scale_y_max = i;
         doorgaan = 0;
         break;
      }
   } /* for (i = 1; i < 10; i++) */

   /* scale_y_max range: 10 t/m 1000 m2/Hz */
   if (doorgaan == 1)                          /* dus scale_y_max was nog niet gevonden */
   {
      for (i = 10; i <= 1000; i += 10)
      {
         if (max_energy < (double)i)
         {
            *scale_y_max = i;
            break;
         }
      } /* for (i = 10; i <= 1000; i += 10) */
   } /* if (doorgaan == 1) */



   /*
   ///////////////////   scale_x_max   /////////////
   */

   if (max_energy < 5.0)                     /* t/m 12-02-2002: 10 */
      *scale_x_max = 4;
   else  if (max_energy < 50.0)              /* t/m 12-02-2002: 100 */
      *scale_x_max = 3;
   else
      *scale_x_max = 2;


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                          Bepaal_Summary_Per_Grafiek                       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Summary_Per_Grafiek(const int v, char* summary, const char* station)
{
   char buffer[100];

   int aantal_freq_array_elementen;
   int m;

   double HM0;
   /*double dir_tot;*/
   /*double dir_lfe;*/
   double num_fp;
   double num_Hmax;
   double Hmax_Hm0;
   double BFI;
   double C4;
   double P;

   double* nedwam_energy_array;                   /* array energy voor BFI etc. */


   /* NB v = teller verwachtingen (indice nummer) */



   /* initialisatie */
   summary[0] = '\0';
   buffer[0] = '\0';


   /* summary regel samenstellen */

   /* HM0 ( = 4 * sqrt(Emean) ) */
   strcat(summary, "Hm0 = ");
   HM0 = 4 * sqrt(EMEAN[v]);
   sprintf(buffer, "%.1f", HM0);
   strcat(summary, buffer);
   strcat(summary, "m  ");

   /* TM0-1 (= 1 / FMEAN) */
   buffer[0] = '\0';
   strcat(summary, "Tm0-1 = ");
   if (FMEAN[v] != MAXFLOAT)
      sprintf(buffer, "%.1f", 1.0 / FMEAN[v]);
   else
      sprintf(buffer, "%s", "-");
   strcat(summary, buffer);
   strcat(summary, "s  ");

#if 0
//   /* LFE ( = E10) */
//   buffer[0] = '\0';
//   strcat(summary, "LFE = ");
//   sprintf(buffer, "%.0f", E10[v] * 10000);     /* van m2 -> cm2 */
//   strcat(summary, buffer);
//   strcat(summary, "cm2  ");
#endif

#if 0
//   /* ff (hirlam wind snelheid) */
//   buffer[0] = '\0';
//   strcat(summary, "ff = ");
//   if (FF_hirlam[v] != MAXFLOAT)
//      sprintf(buffer, "%.0f", FF_hirlam[v]);    /* (afgeronde) knopen */
//   else
//      sprintf(buffer, "%s", "-");
//   strcat(summary, buffer);
//   strcat(summary, "kn  ");
#endif

#if 0
//   /* dir.tot */
//   buffer[0] = '\0';
//   strcat(summary, "dir.tot = ");
//   if (THQ[v] != MAXFLOAT)
//   {
//      dir_tot = (THQ[v] * DEG) + 180;           /* omzetten neer deg + naar richting waar van daan */
//      if (dir_tot > 360.0)
//         dir_tot -= 360.0;
//      sprintf(buffer, "%03.0f", dir_tot);
//   }
//   else
//      sprintf(buffer, "%s", "-");
//
//   strcat(summary, buffer);
//   strcat(summary, "  ");
#endif

#if 0
//   /* dir. LFE */
//   buffer[0] = '\0';
//   strcat(summary, "dir.LFE = ");
//   if (E10D[v] != MAXFLOAT)
//   {
//      dir_lfe = (E10D[v] * DEG) + 180;          /* omzetten neer deg + naar richting waar van daan */
//      if (dir_lfe > 360.0)
//         dir_lfe -= 360.0;
//      sprintf(buffer, "%03.0f", dir_lfe);
//   }
//   else
//      sprintf(buffer, "%s", "-");
//
//   strcat(summary, buffer);
//   strcat(summary, "  ");
#endif

   /* alloceren dynamische geheugenruimte */
   nedwam_energy_array  = (double*)calloc(NUM_FREQUENTIES, sizeof(double));   /* cleared to 0 */


   /* 2-dim. energy met verwachting omzetten naar 1 dim. array met alleen energy */
   for (m = 0; m < NUM_FREQUENTIES; m++)
      nedwam_energy_array[m] = EMN1DIM[m][v];

   /* BFI, C4 (kurtosis) en P20(Hmax > 2 * Hm0) berekenen */
   num_fp = MAXFLOAT;                               /* geeft aan dat in function Bereken_BFI de fp uit de freq array gehaald moet worden */
   num_Hmax = MAXFLOAT;                             /* geeft aan dat Hmax in function Bereken_BFI niet gebruikt moet worden */
   Hmax_Hm0 = MAXFLOAT;                             /* geeft aan dat Hmax / Hm0 in function Bereken_BFI niet gebruikt moet worden */
   aantal_freq_array_elementen = NUM_FREQUENTIES;   /* voor een generieke aanroep (zo kan zowel cic als nedwam array energy deze function gebruiken) */
   Bereken_BFI(nedwam_energy_array, aantal_freq_array_elementen, num_fp, 1.0 / FMEAN[v], &BFI, &C4, &P, "later_invullen", HM0, station, num_Hmax, Hmax_Hm0);

   /* de-alloceren dynamische geheugenruimte */
   free(nedwam_energy_array);


   /* BFI */
   buffer[0] = '\0';
   strcat(summary, "BFI = ");
   if (BFI != MAXFLOAT)
   {
      sprintf(buffer, "%.2f", BFI);
      strcat(summary, buffer);
   }
   else
      strcat(summary, "-");
   strcat(summary, "  ");

   /* C4 */
   buffer[0] = '\0';
   strcat(summary, "C4 = ");
   if (C4 != MAXFLOAT)
   {
      sprintf(buffer, "%.2f", C4);
      strcat(summary, buffer);
   }
   else
      strcat(summary, "-");
   strcat(summary, "  ");

#if 0
//   /* P */
//   buffer[0] = '\0';
//   strcat(summary, "P20(Hmax>2*Hm0) = ");
//   if (P != MAXFLOAT)
//   {
//      sprintf(buffer, "%.2f", P);
//      strcat(summary, buffer);
//   }
//   else
//      strcat(summary, "-");
//   strcat(summary, "  ");
#endif



   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                         Read_CIC_Temps_in_Spectrums                       */
/*                                                                           */
/*****************************************************************************/
int Read_CIC_Temps_in_Spectrums()
{
   FILE* temp;
   int i;
   char regel[NUM_SPECTRUM_2 + 1];
   int regel_lengte = NUM_SPECTRUM_2;


   /*
   // als er een temp file niet is dan geen foutmelding in de log zetten, omdat afhankelijk van het tijdstip
   // niet altijd alle aangevraagde temp files er wel moeten zijn
   */


   if (INT_VERWACHTINGEN == 3)                          /* interval uren tussen forecasts tijdstippen ("VALID AT") */
   {
      /*
      // CIC data (energy, direction etc.) behorende bij forecast van +00 hr
      */
      if ((temp = fopen(tempfilenaam_0, "r")) != NULL)  /* dus gelukt */
      {
         for (i = 0; i < NUM_SPECTRUM; i++)             /* zoveel spectrum array elementen zoveel regels in de temp file */
         {
            if (fread(regel, regel_lengte + 1, 1, temp) != 0)
            {
               regel[regel_lengte] = '\0';
               strcpy(spectrum_0[i], regel);
            }
         } /* for (i = 0; i < NUM_SPECTRUM; i++) */

         fclose(temp);

      } /* if ((temp = fopen(tempfilenaam_0, "r")) != NULL) */

      /*
      // CIC data (energy, direction etc.) behorende bij forecast van +03hr
      */
      if ((temp = fopen(tempfilenaam_1, "r")) != NULL)  /* dus gelukt */
      {
         for (i = 0; i < NUM_SPECTRUM; i++)             /* zoveel spectrum array elementen zoveel regels in de temp file */
         {
            if (fread(regel, regel_lengte + 1, 1, temp) != 0)
            {
               regel[regel_lengte] = '\0';
               strcpy(spectrum_1[i], regel);
            }
         } /* for (i = 0; i < NUM_SPECTRUM; i++) */

         fclose(temp);

      } /* if ((temp = fopen(tempfilenaam_1, "r")) != NULL) */

      /*
      // CIC data (energy, direction etc.) behorende bij forecast van +06 hr
      */
      if ((temp = fopen(tempfilenaam_2, "r")) != NULL)  /* dus gelukt */
      {
         for (i = 0; i < NUM_SPECTRUM; i++)             /* zoveel spectrum array elementen zoveel regels in de temp file */
         {
            if (fread(regel, regel_lengte + 1, 1, temp) != 0)
            {
               regel[regel_lengte] = '\0';
               strcpy(spectrum_2[i], regel);
            }
         } /* for (i = 0; i < NUM_SPECTRUM; i++) */

         fclose(temp);

      } /* if ((temp = fopen(tempfilenaam_2, "r")) != NULL) */

   } /* if (INT_VERWACHTINGEN == 3) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                      Initialiseer_CIC_Spectrum_Arrays                     */
/*                                                                           */
/*****************************************************************************/
int Initialiseer_CIC_Spectrum_Arrays()
{
   int i;


   for (i = 0; i < NUM_SPECTRUM; i++)
   {
      strcpy(spectrum_0[i], "\0");                      /* NUM_SPECTRUM arrayplaatsen van elk 10 char */
      strcpy(spectrum_1[i], "\0");                      /* spectrum behorende bij forecast van b.v. +03hr */
      strcpy(spectrum_2[i], "\0");                      /* spectrum behorende bij forecast van b.v. +06hr */
   } /* for (i = 0; i < NUM_SPECTRUM; i++) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                      Bepaal_Max_Energy_CIC_Spectrums                      */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Energy_CIC_Spectrums(double* max_energy_cic)
{
   int i;


   /* NB. het kan zijn dat een spectrum geheel leeg is ("\0") alle alementen, dit gaat dan ook goed */

   /* initialisatie */
   *max_energy_cic = 0.0;


   /*
   // CIC spectrum behorende bij nedwam forecast van  +00hr
   */
   for (i = 0; i < NUM_SPECTRUM; i++)
   {
      if ((i >= BEGIN_SPECTRUM_ENERGY) && (i <= END_SPECTRUM_ENERGY))
         if (atof(spectrum_0[i]) > *max_energy_cic)
            *max_energy_cic = atof(spectrum_0[i]);               /* in .1cm2s !!! */
   } /* for (i = 0; i < NUM_SPECTRUM; i++) */

   /*
   // CIC spectrum behorende bij nedwam forecast van b.v. +03hr
   */
   for (i = 0; i < NUM_SPECTRUM; i++)
   {
      if ((i >= BEGIN_SPECTRUM_ENERGY) && (i <= END_SPECTRUM_ENERGY))
         if (atof(spectrum_1[i]) > *max_energy_cic)
            *max_energy_cic = atof(spectrum_1[i]);               /* in .1cm2s !!! */
   } /* for (i = 0; i < NUM_SPECTRUM; i++) */

   /*
   // CIC spectrum behorende bij nedwam forecast van b.v. +06hr
   */
   for (i = 0; i < NUM_SPECTRUM; i++)
   {
      if ((i >= BEGIN_SPECTRUM_ENERGY) && (i <= END_SPECTRUM_ENERGY))
         if (atof(spectrum_2[i]) > *max_energy_cic)
            *max_energy_cic = atof(spectrum_2[i]);               /* in .1cm2s !!! */
   } /* for (i = 0; i < NUM_SPECTRUM; i++) */



   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*       Bepaal_Filenamen_CIC_Temp_Files_voor NEDWAM_Per_Locatie             */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenamen_CIC_Temp_Files_voor_NEDWAM_Per_Locatie(const char* station, const int cic_uren_interval, const int pag_no)
{
   /* laatste 3 files (van de uurlijkse files) worden geplot dus dus van deze */
   /* laatse 3 de hoogste waarde bepalen i.v.m. latere schaalverdeling */

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


   /* initialisatie */
   strcpy(tempfilenaam_0, "\0");
   strcpy(tempfilenaam_1, "\0");
   strcpy(tempfilenaam_2, "\0");
   /*tijd_ok = 1;*/

   /* eerste deel tempfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
   {
      strcpy(tempfilenaam_0, "temp_cic\\");
      strcpy(tempfilenaam_1, "temp_cic\\");
      strcpy(tempfilenaam_2, "temp_cic\\");
   }
   else                                                /* UNIX */
   {
      strcpy(tempfilenaam_0, "temp_cic/");
      strcpy(tempfilenaam_1, "temp_cic/");
      strcpy(tempfilenaam_2, "temp_cic/");
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */



   /* hulp voor file namen van b.v 3 en 6 uur vooruit bepalen (t.o.v van 0 uur  = JJJJJMMDDUU) */
   /* dus de cic files die horen bij de verwachtingen van +00hr, +03hr en +06hr */

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




   /*
   /////// file naam bepalen van de cic files behorende bij bovenste grafiek op pagina (b.v. nedwam +09hr) /////
   */
   uren_vooruit = ((pag_no * cic_uren_interval) + 0) * cic_uren_interval;             /* b.v. ((1 * 3) + 0) * 3 = 9 */

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
      (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121000 */

      strcat(tempfilenaam_0, "CIC_LDS_");
      strcat(tempfilenaam_0, station);
      strcat(tempfilenaam_0, "_");
      strcat(tempfilenaam_0, date_time);
      strcat(tempfilenaam_0, ".TMP");
   } /* if (tijd_ok == 1) */



   /*
   /////// file naam bepalen van de cic files behorende bij middelste grafiek op pagina (b.v. nedwam +12hr) /////
   */
   uren_vooruit = ((pag_no * cic_uren_interval) + 1) * cic_uren_interval;             /* b.v. ((1 * 3) + 1) * 3 = 12 */

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
      (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121000 */

      strcat(tempfilenaam_1, "CIC_LDS_");
      strcat(tempfilenaam_1, station);
      strcat(tempfilenaam_1, "_");
      strcat(tempfilenaam_1, date_time);
      strcat(tempfilenaam_1, ".TMP");
   } /* if (tijd_ok == 1) */



   /*
   /////// file naam bepalen van de cic files behorende bij onderste grafiek op pagina (b.v. nedwam +15hr) /////
   */
   uren_vooruit = ((pag_no * cic_uren_interval) + 2) * cic_uren_interval;             /* b.v. ((1 * 3) + 2) * 3 = 15 */

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
      (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121000 */

      strcat(tempfilenaam_2, "CIC_LDS_");
      strcat(tempfilenaam_2, station);
      strcat(tempfilenaam_2, "_");
      strcat(tempfilenaam_2, date_time);
      strcat(tempfilenaam_2, ".TMP");
   } /* if (tijd_ok == 1) */


    return 0;
}

