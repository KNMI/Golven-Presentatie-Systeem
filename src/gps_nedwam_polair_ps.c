#if !defined(gps_h)                         /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <stdio.h>
#include <math.h>                           /* o.a. cos, sin */
#include <values.h>                         /* MAXFLOAT */
#include <string.h>

#if defined(WINDOWS)                        /* zie gps.h */
#include <dir.h>                            /* o.a. getcwd() in windows */
#else
#include <unistd.h>                         /* o.a. getcwd() in UNIX */
#endif

/*
// LET OP  I.V.M. VOORKOMEN RUN_TIME ERROR
// definitie "extern FILE* os_ps" omdat i.v.m. o.a. aanroep Write_Postscript_General();
// (hier is FILE* os_ps al gedefinieerd
/*

/* variabelen voor deze module */
char nedwam_polair_outputfilenaam[256];


/* functions in deze module */
int Write_TSA_Polair_Postscript_Files(const char* station);
int Bepaal_Polair_TSA_Outputfilenaam_Per_Locatie(const char* station, int pag_no);
int Write_Postscript_Polair(char* grafiek_orientatie);
int Bepaal_Max_Energy_Per_Verwachting(double* max_energy, const int pag_no, const char* grafiek_orientatie);
int Write_Postscript_Polair_Richtingen(const double max_polair_hz);
int Normalize_Energy_Per_Verwachting_Of_Meting(const double max_energy);
char* Set_Normalized_Energy_Class_Color(int norm_class);
int TSA_Temp_Files_To_Polair_Postscript(const char* grafiek_orientatie, int pag_no, const char* station, const double scale_hz);
int Write_Postscript_Polair_Index(void);
int Bepaal_Polair_Summary_Per_Grafiek(const int v, char* hm0_summary, char* tm0_1_summary, char* lfe_summary,
                                                   char* ff_summary, char* dd_summary, char* dir_tot_summary, char* dir_lfe_summary);
int Bepaal_Max_Scale_Hz_Verwachting(const double max_energy, double *max_polair_hz);


/* globale var's in andere module */
extern double max_norm_class_limit[NUMBER_ENERGY_CLASSES]; /* gpsmain.c */
extern char OS[8];                              /* gpsmain.c */
extern int x1p;                                  /* gpsmain.c */
extern int x2p;                                  /* gpsmain.c */
extern int y2p;                                  /* gpsmain.c */
extern int _y1p;                                 /* gpsmain.c */
extern char font_keuze[50];                     /* gpsmain.c */
extern FILE* os_ps;                             /* gpsmain.c */
extern int xe0;                                 /* gpsmain.c; oorsprong polair plot */
extern int ye0;                                 /* gpsmain.c; oorsprong polair plot */
extern const int lengte_y_as_polair;            /* gpsmain.c */
extern const int lengte_x_as_polair;            /* gpsmain.c */
extern double** EMN1DIM;                        /* 1-dim. spectrum (via module: gps_nedwam_compute.c) */
extern int NUM_FREQUENTIES;                     /* aantal frequenties */
extern int NUM_VERWACHTINGEN;                   /* aantal verwachtingen */
extern int NUM_RICHTINGEN;                      /* aantal richtingen */
extern double* FR;                              /* uit gps_nedwam_tsa_input.c, hierin alle frequenties (0 - NUM_FREQUENTIES) */
extern char**** TSA_spectrum;                   /* uit gps_nedwam_tsa_input.c */
extern double* EMEAN;                           /* uit NEDWAM berekeningen */
extern double* THQ;                             /* uit NEDWAM berekeningen */
extern double* E10D;                            /* uit NEDWAM berekeningen */
extern double* FMEAN;                           /* uit NEDWAM berekeningen */
extern double* E10;                             /* uit NEDWAM berekeningen */
extern double* FF_hirlam;                       /* uit hirlam wind-snelheden */
extern double* DDD_hirlam;                      /* hirlam wind-richtingen */


/* prototype in andere module */
extern int Write_Log(char* bericht);
extern int Write_Postscript_General(void);
extern int Write_Postscript_End(void);
extern int Bepaal_Title_Per_Grafiek(const char* grafiek_orientatie, const int pag_no, char* title, const char* station);




/*****************************************************************************/
/*                                                                           */
/*                   Write_TSA_Polair_Postscript_Files                       */
/*                                                                           */
/*****************************************************************************/
int Write_TSA_Polair_Postscript_Files(const char* station)
{
   int num_ps_paginas;                         /* totaal aantal ps paginas met NEDWAM verwachtingen */
   int i;                                      /* teller ps pagina's */
   double max_energy;                          /* max energy (hoogste van nedwam gedeelte) */
   char info[512];
   char volledig_path[512];
   char grafiek_orientatie[256];
   double scale_hz;
   /*char buffer[128];*/
   double max_polair_hz;
   double hulp_max_polair_hz;
   

   /* altijd 3 verwachtingen op 1 ps pagina */
   if (NUM_VERWACHTINGEN % 3 != 0)
      num_ps_paginas = NUM_VERWACHTINGEN / 3 + 1;               /* b.v. 17 / 3 + 1 = 5.66 + 1 = 6.66 -> 6 (i.v.m. type 'int') */
   else
      num_ps_paginas = NUM_VERWACHTINGEN / 3;                   /* b.v. 18 / 3 = 6 */

   /* geldig voor alle verwachtingen en paginas */
   /* scale_hz = lengte_y_as_polair / 2.0 / (double)AANTAL_CIRKELS_HZ; */     /* bijv 200 / 2 / 4 = 25 */

   /* per pagina verder uitwerken */
   for (i = 0; i < num_ps_paginas; i++)
   {
      Bepaal_Polair_TSA_Outputfilenaam_Per_Locatie(station, i);              /* zie module: deze */

      if ((os_ps = fopen(nedwam_polair_outputfilenaam, "w")) == NULL)        /* dus mislukt */
      {
         getcwd(volledig_path, 512);

         /* bericht samen stellen */
         strcpy(info, "\0");
         strcat(info, volledig_path);

         if (strcmp(OS, "WINDOWS") == 0)
            strcat(info, "\\");
         else
            strcat(info, "/");

         strcat(info, nedwam_polair_outputfilenaam);
         strcat(info, " niet te schrijven\n");

         /* naar log schrijven */
         Write_Log(info);                                                                     /* zie module: gpsmain.c */

      } /* if ((os_ps = fopen(outputfilenaam, "w")) == NULL) */
      else /* outputfile is te schrijven */
      {
         /* ps pagina correct beginnen */
         Write_Postscript_General();                                                          /* zie module gps_ps.c */

         /* onderste grafiek (I.V.M. 'TRANSLATE' ONDERSTE GRAFIEK ALS EERSTE DOEN) */
         strcpy(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK);
         Write_Postscript_Polair(grafiek_orientatie);                                                /* zie module: deze */
         Bepaal_Max_Energy_Per_Verwachting(&max_energy, i, grafiek_orientatie);                      /* zie module: deze */

         Bepaal_Max_Scale_Hz_Verwachting(max_energy, &max_polair_hz);                                /* uitkomst 0.4 of 0.3 of 0.2 */
         /* gaat op ALPHA wel goed bij LINUX niet!!!!! scale_hz = lengte_y_as_polair / 2.0 / (double)(max_polair_hz * 10); */   /* bijv 200 / 2 / (0.4 * 10) = 25 */
         hulp_max_polair_hz = max_polair_hz * 10.0;
         scale_hz = (lengte_y_as_polair / 2.0) / hulp_max_polair_hz;                                 /* bijv 200 / 2 / (0.4 * 10) = 25 */


/**************  test ************/
/*
                 buffer[0] = '\0';
                 sprintf(buffer, "%le", scale_hz);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************** test ************/


         Normalize_Energy_Per_Verwachting_Of_Meting(max_energy);                                     /* zie module: deze */
         TSA_Temp_Files_To_Polair_Postscript(grafiek_orientatie, i, station, scale_hz);              /* zie module: deze */
         Write_Postscript_Polair_Richtingen(max_polair_hz);                                          /* zie module: deze */
         Write_Postscript_Polair_Index();                                                            /* zie module: deze */

         /* middelste grafiek (I.V.M. 'TRANSLATE' MIDDELSTE GRAFIEK ALS TWEEDE DOEN)*/
         strcpy(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK);
         Write_Postscript_Polair(grafiek_orientatie);                                                /* zie module: deze */
         Bepaal_Max_Energy_Per_Verwachting(&max_energy, i, grafiek_orientatie);                      /* zie module: deze */

         Bepaal_Max_Scale_Hz_Verwachting(max_energy, &max_polair_hz);                                /* uitkomst 0.4 of 0.3 of 0.2 */
         /* gaat op ALPHA wel goed bij LINUX niet!!!!! scale_hz = lengte_y_as_polair / 2.0 / (double)(max_polair_hz * 10);*/ /* bijv 200 / 2 / (0.4 * 10) = 25 */
         hulp_max_polair_hz = max_polair_hz * 10.0;
         scale_hz = (lengte_y_as_polair / 2.0) / hulp_max_polair_hz;                                 /* bijv 200 / 2 / (0.4 * 10) = 25 */

         Normalize_Energy_Per_Verwachting_Of_Meting(max_energy);                                     /* zie module: deze */
         TSA_Temp_Files_To_Polair_Postscript(grafiek_orientatie, i, station, scale_hz);              /* zie module: deze */
         Write_Postscript_Polair_Richtingen(max_polair_hz);                                          /* zie module: deze */
         Write_Postscript_Polair_Index();                                                            /* zie module: deze */

         /* bovenste grafiek (I.V.M. 'TRANSLATE' MIDDELSTE GRAFIEK ALS LAATSTE DOEN)*/
         strcpy(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK);
         Write_Postscript_Polair(grafiek_orientatie);                                                /* zie module: deze */
         Bepaal_Max_Energy_Per_Verwachting(&max_energy, i, grafiek_orientatie);                      /* zie module: deze */

         Bepaal_Max_Scale_Hz_Verwachting(max_energy, &max_polair_hz);                                /* uitkomst 0.4 of 0.3 of 0.2 */
         /* gaat op ALPHA wel goed bij LINUX niet!!!!!scale_hz = lengte_y_as_polair / 2.0 / (double)(max_polair_hz * 10);*/ /* bijv 200 / 2 / (0.4 * 10) = 25 */
         hulp_max_polair_hz = max_polair_hz * 10.0;
         scale_hz = (lengte_y_as_polair / 2.0) / hulp_max_polair_hz;                                 /* bijv 200 / 2 / (0.4 * 10) = 25 */

         Normalize_Energy_Per_Verwachting_Of_Meting(max_energy);                                     /* zie module: deze */
         TSA_Temp_Files_To_Polair_Postscript(grafiek_orientatie, i, station, scale_hz);              /* zie module: deze */
         Write_Postscript_Polair_Richtingen(max_polair_hz);                                          /* zie module: deze */
         Write_Postscript_Polair_Index();                                                            /* zie module: deze */

         /* ps pagina correct afsluiten */
         Write_Postscript_End();                                                              /* zie module gps_ps.c */

         fclose(os_ps);
      } /* else (outputfile is te schrijven) */

   } /* for (i = 0; i < num_ps_paginas; i++) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*            Bepaal_Polair_TSA_Outputfilenaam_Per_Locatie()                 */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Polair_TSA_Outputfilenaam_Per_Locatie(const char* station, int pag_no)
{
   char hulp_pag_no[10];                           /* char equavalent van pag_no */

   /* outputfilenaam (inclusief sub. dir.) bv.: output_nedwam/TSA_POLAIR_K13_0.PS */


   /* initialisatie */
   strcpy(nedwam_polair_outputfilenaam, "\0");

   if (strcmp(OS, "WINDOWS") == 0)                 /* WINDOWS */
      strcpy(nedwam_polair_outputfilenaam, "output_nedwam\\");
   else                                            /* UNIX */
      strcpy(nedwam_polair_outputfilenaam, "output_nedwam/");

   strcat(nedwam_polair_outputfilenaam, "TSA_POLAIR_");
   strcat(nedwam_polair_outputfilenaam, station);
   strcat(nedwam_polair_outputfilenaam, "_");

   sprintf(hulp_pag_no, "%d", pag_no);
   strcat(nedwam_polair_outputfilenaam, hulp_pag_no);

   strcat(nedwam_polair_outputfilenaam, ".PS");


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                            Write_Postscript_Polair                        */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_Polair(char* grafiek_orientatie)
{


   /*
   //  (x1,y2)         (x2,y2)
   //       *-----------*
   //       |           |
   //       |           |
   //       *-----------*
   //  (x1,y1)         (x2,y1)           
   //
   */

   char line[256];
   char buffer1[10];                                /* opslag tussen waarden */
   char buffer2[10];                                /* opslag tussen waarden */


#if 0
   /* dimensies 'main-boxen' bepalen */
   x1  = 55;                                    /* main box links */
   x2  = x1 + lengte_x_as_polair;               /* main box links */
   _y1 = 20; /*50;*/                            /* main box links + main box rechts */
   y2  = _y1 + lengte_y_as_polair;              /* main box links + main box rechts*/
#endif



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



   /**************************************************************************/
   /*                                                                        */
   /*                                x-y box links                           */
   /*                                                                        */
   /**************************************************************************/


   /*
   ///////////// color x-y box links //////////////
   */

   /* newpath (color x-y box links) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* color (color x-y box) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_polair_ZEE);
   strcat(line, " setrgbcolor %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* moveto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p);
   sprintf(buffer2, "%d", _y1p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p);
   sprintf(buffer2, "%d", _y1p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p);
   sprintf(buffer2, "%d", y2p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p);
   sprintf(buffer2, "%d", y2p);
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
   /////////// font zetten linkse box ////////////
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
   ////////////// kader (contouren) tekenen linkse box //////////////
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
   sprintf(buffer1, "%d", x1p);
   sprintf(buffer2, "%d", _y1p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p);
   sprintf(buffer2, "%d", _y1p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p);
   sprintf(buffer2, "%d", y2p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p);
   sprintf(buffer2, "%d", y2p);
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


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                       Write_Postscript_Polair_Richtingen()                */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_Polair_Richtingen(const double max_polair_hz)
{
   char line[256];
   char buffer1[10];                                /* opslag tussen waarden */
   char buffer2[10];                                /* opslag tussen waarden */
   char buffer3[10];                                /* opslag tussen waarden */
   int i;
   int straal_per_hz;                               /* straal voor 0.1, 0.2, 0.3 en 0.4 Hz */
   const int of_x_links_richtings_label  = 21/*20*/;/* i.v.m. richtings label (b.v. 270) */
   const int of_y_links_richtings_label  = 4;       /* i.v.m. richtings label (b.v. 270) */
   const int of_x_boven_richtings_label  = 10;      /* i.v.m. richtings label (b.v. 360) */
   const int of_y_boven_richtings_label  = 4/*2*/;  /* i.v.m. richtings label (b.v. 360) */
   const int of_x_rechts_richtings_label = 3/*2*/;  /* i.v.m. richtings label (b.v. 090) */
   const int of_y_rechts_richtings_label = 4;       /* i.v.m. richtings label (b.v. 090) */
   const int of_x_onder_richtings_label  = 10;      /* i.v.m. richtings label (b.v. 180) */
   const int of_y_onder_richtings_label  = 10 /*9*/;/* i.v.m. met richtings label (b.v. 180) */
   const int of_x_label_hz_cirkel        = 14;      /* i.v.m. label hz aanduiding */
   const int of_y_label_hz_cirkel        = 2;       /* i.v.m. label hz aanduiding */
   const int of_y_rechtsboven_richtings_label = 3;
   const int of_y_linksboven_richtings_label = 3;
   int x_hz_label;                                  /* i.v.m. label hz aanduiding */
   int y_hz_label;                                  /* i.v.m. label hz aanduiding */
   /*char buffer[100];*/
   int int_max_polair_hz;

   /*
   //
   //             hulp_lengte
   //                  |
   //                  |
   //                  |
   //              0   v   30
   // y2 ->        |-----/
   //              |    /
   //              |   /
   //              |  /
   /               | /
   //  --------------------------
   //              |
   //              |
   //              |
   //  ^           |             ^
   //  |   y1->    |             |
   //  x1                        x2
   //
   //
   */


   int hulp_lengte;
   hulp_lengte = (int)((double)lengte_x_as_polair / 2 / WORTEL_3 + 0.5);



   /* color (richtingslijnen) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_polair_raster);
   strcat(line, " setrgbcolor %color polair raster\n");
   fprintf(os_ps, "%s", line);




   /*************************************************************************
   //
   // richtings graden lijnen
   //
   *************************************************************************/

   /*
   //   lijn 270 - 090
   */

   /* newpath (richtings lijn 270 - 090) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %richtings lijnen\n");
   fprintf(os_ps, "%s", line);

   /* moveto (richtings lijn 270 - 090) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p);
   sprintf(buffer2, "%d", _y1p + lengte_y_as_polair / 2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings lijn 270-090\n");
   fprintf(os_ps, "%s", line);

   /* lineto (richtings lijn 270 - 090)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p);
   sprintf(buffer2, "%d", _y1p + lengte_y_as_polair / 2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %richtings lijn 270-090\n");
   fprintf(os_ps, "%s", line);

   /* stroke (richtings lijn 270-090) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %richtings lijn 270-090\n");
   fprintf(os_ps, "%s", line);



   /*
   //   lijn 360 -180
   */

   /* newpath (richtings lijn 360 - 180) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %richtings lijnen\n");
   fprintf(os_ps, "%s", line);

   /* moveto (richtings lijn 360 - 180) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p + lengte_x_as_polair / 2);
   sprintf(buffer2, "%d", y2p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings lijn 360-180\n");
   fprintf(os_ps, "%s", line);

   /* lineto (richtings lijn 360 - 180)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p + lengte_x_as_polair / 2);
   sprintf(buffer2, "%d", _y1p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %richtings lijn 360-180\n");
   fprintf(os_ps, "%s", line);

   /* stroke (richtings lijn 360-180) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %richtings lijn 360-180\n");
   fprintf(os_ps, "%s", line);


   /*
   //   lijn 30 -210
   */

   /* newpath (richtings lijn 30 - 210) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %richtings lijnen\n");
   fprintf(os_ps, "%s", line);

   /* moveto (richtings lijn 30 - 210) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p + lengte_x_as_polair / 2 + hulp_lengte);
   sprintf(buffer2, "%d", y2p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings lijn 30-210\n");
   fprintf(os_ps, "%s", line);

   /* lineto (richtings lijn 30 - 210)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p + lengte_x_as_polair / 2 - hulp_lengte);
   sprintf(buffer2, "%d", _y1p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %richtings lijn 30-210\n");
   fprintf(os_ps, "%s", line);

   /* stroke (richtings lijn 360-180) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %richtings lijn 30-210\n");
   fprintf(os_ps, "%s", line);


   /*
   //   lijn 60 - 240
   */

   /* newpath (richtings lijn 60 - 240) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %richtings lijnen\n");
   fprintf(os_ps, "%s", line);

   /* moveto (richtings lijn 60 - 240) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p);
   sprintf(buffer2, "%d", _y1p + lengte_y_as_polair / 2 + hulp_lengte);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings lijn 60-240\n");
   fprintf(os_ps, "%s", line);

   /* lineto (richtings lijn 60 - 240)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p);
   sprintf(buffer2, "%d", _y1p + lengte_y_as_polair / 2 - hulp_lengte);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %richtings lijn 60-240\n");
   fprintf(os_ps, "%s", line);

   /* stroke (richtings lijn 60-240) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %richtings lijn 60-240\n");
   fprintf(os_ps, "%s", line);


   /*
   //   lijn 330 -150
   */

   /* newpath (richtings lijn 330 - 150) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %richtings lijnen\n");
   fprintf(os_ps, "%s", line);

   /* moveto (richtings lijn 330 - 150) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p + lengte_x_as_polair / 2 - hulp_lengte);
   sprintf(buffer2, "%d", y2p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings lijn 330-150\n");
   fprintf(os_ps, "%s", line);

   /* lineto (richtings lijn 330 - 150)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p + lengte_x_as_polair / 2 + hulp_lengte);
   sprintf(buffer2, "%d", _y1p);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %richtings lijn 330-150\n");
   fprintf(os_ps, "%s", line);

   /* stroke (richtings lijn 330-150) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %richtings lijn 330-150\n");
   fprintf(os_ps, "%s", line);


   /*
   //   lijn 300 -120
   */

   /* newpath (richtings lijn 300 - 120) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %richtings lijnen\n");
   fprintf(os_ps, "%s", line);

   /* moveto (richtings lijn 300 - 120) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p);
   sprintf(buffer2, "%d", _y1p + lengte_y_as_polair / 2 + hulp_lengte);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings lijn 300-120\n");
   fprintf(os_ps, "%s", line);

   /* lineto (richtings lijn 300 - 120)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p);
   sprintf(buffer2, "%d", _y1p + lengte_y_as_polair / 2 - hulp_lengte);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %richtings lijn 300-120\n");
   fprintf(os_ps, "%s", line);

   /* stroke (richtings lijn 300-120) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %richtings lijn 300-120\n");
   fprintf(os_ps, "%s", line);


   /***************************************************************
   //
   // hz cirkels
   //
   ****************************************************************/

/*********** TEST ************************/
/*

                 buffer[0] = '\0';
                 sprintf(buffer, "%lf", max_polair_hz);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************************************/

   /*for (i = 1; i <= (int)(max_polair_hz * 10); i++)*/                    /* max_polair_hz: 0.4 of 0.3 of 0.2 */


   /* i.v.m. correcte afronding (vooral voor op de PC, op de bdawo8 ging dit ook op "oude manier" goed) */
   int_max_polair_hz = (int)(max_polair_hz * 10.0 + 0.5);

/*********** TEST ************************/
/*
                 buffer[0] = '\0';
                 sprintf(buffer, "%d", int_max_polair_hz);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************************************/



   for (i = 1; i <= int_max_polair_hz; i++)
   {

/*********** TEST ************************/
/*

                 buffer[0] = '\0';
                 sprintf(buffer, "%d", int_max_polair_hz);
                 fprintf(stderr, "%s", buffer);
                 getchar();

                 buffer[0] = '\0';
                 sprintf(buffer, "%d", i);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************************************/



      /* straal_per_hz */
      /*straal_per_hz = (int)((lengte_y_as_polair / (double)(max_polair_hz * 10.0 * 2.0)) * (double)i) + 0.5;*/
      straal_per_hz =  (int)(((double)lengte_y_as_polair / ((double)int_max_polair_hz * 2)) * (double)i + 0.5);

      /* newpath (Hz cirkel) */
      strcpy(line, "\0");
      strcpy(line, "newpath        %Hz cirkel\n");
      fprintf(os_ps, "%s", line);

      /* arc (Hz cirkel) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      buffer3[0] = '\0';
      sprintf(buffer1, "%d", x1p + lengte_y_as_polair / 2);
      sprintf(buffer2, "%d", _y1p + lengte_y_as_polair / 2);
      sprintf(buffer3, "%d", straal_per_hz);                             /* straal van de cirkel */
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " ");
      strcat(line, buffer3);
      strcat(line, " 0 360 arc   %Hz cirkel\n");
      fprintf(os_ps, "%s", line);

      /* stroke (Hz cirkel) */
      strcpy(line, "\0");
      strcpy(line, "stroke         %Hz cirkel\n");
      fprintf(os_ps, "%s", line);
   } /* for (i = 1; i <= int_max_polair_hz; i++) */



   /***************************************************************
   //
   // hz cirkel labels
   //
   ****************************************************************/
   /* newpath (Hz cirkel label) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %Hz cirkel label\n");
   fprintf(os_ps, "%s", line);

   /* color (Hz cirkel label) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_polair_raster);
   strcat(line, " setrgbcolor %Hz cirkel label\n");
   fprintf(os_ps, "%s", line);

   /* font (Hz cirkel label) */
   strcpy(line, "\0");
   strcpy(line, font_keuze);
   fprintf(os_ps, "%s", line);

   /* font (Hz cirkel label) */
   strcpy(line, "\0");
   strcpy(line, "10 scalefont       %Hz cirkel label\n");
   fprintf(os_ps, "%s", line);

   /* font (Hz cirkel label) */
   strcpy(line, "\0");
   strcpy(line, "setfont            %Hz cirkel label\n");
   fprintf(os_ps, "%s", line);

   /*for (i = 1; i <= (int)(max_polair_hz * 10); i++)*/
   for (i = 1; i <= int_max_polair_hz; i++)
   {
      /*straal_per_hz = (int)(lengte_y_as_polair / (double)(max_polair_hz * 10 * 2) * (double)i + 0.5);*/
      straal_per_hz =  (int)(((double)lengte_y_as_polair / ((double)int_max_polair_hz * 2)) * (double)i + 0.5);

      x_hz_label = (x1p + lengte_y_as_polair / 2) + (straal_per_hz);
      y_hz_label = (_y1p + lengte_y_as_polair / 2);

      /* newpath (Hz cirkel label) */
      strcpy(line, "\0");
      strcpy(line, "newpath        %Hz cirkel label\n");
      fprintf(os_ps, "%s", line);

      /* moveto (Hz cirkel label) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x_hz_label - of_x_label_hz_cirkel);
      sprintf(buffer2, "%d", y_hz_label + of_y_label_hz_cirkel);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %Hz cirkel label\n");
      fprintf(os_ps, "%s", line);

      /* show (Hz cirkel label) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      sprintf(buffer1, "%0.1f", (float)i / 10);
      strcpy(line, "(");
      strcat(line, buffer1 + 1);
      strcat(line, ")");
      strcat(line, " show      %Hz cirkel label\n");
      fprintf(os_ps, "%s", line);

   } /* for (i = 1; i <= int_max_polair_hz; i++) */



   /*************************************************************************
   //
   // richtings labels 
   //
   *************************************************************************/

   /* newpath (richtings label) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* color (richtings label) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_polair_index_kader);
   strcat(line, " setrgbcolor %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* font (richtings label) */
   strcpy(line, "\0");
   strcpy(line, font_keuze);
   fprintf(os_ps, "%s", line);

   /* font (richtings label) */
   strcpy(line, "\0");
   strcpy(line, "10 scalefont       %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* font (richtings label) */
   strcpy(line, "\0");
   strcpy(line, "setfont            %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   // label 360
   */
   /* moveto (richtings label 360) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", (x1p + lengte_x_as_polair / 2) - of_x_boven_richtings_label);
   sprintf(buffer2, "%d", y2p + of_y_boven_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 360);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   // label 30
   */
   /* moveto (richtings label 30) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", (x1p + lengte_x_as_polair / 2 + hulp_lengte) - of_x_boven_richtings_label);
   sprintf(buffer2, "%d", y2p + of_y_boven_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 30);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);

   /*
   // label 60
   */
   /* moveto (richtings label 60) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p + of_x_rechts_richtings_label );
   sprintf(buffer2, "%d", (_y1p + lengte_y_as_polair / 2 + hulp_lengte) - of_y_rechtsboven_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 60) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 60);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   // label 90
   */
   /* moveto (richtings label 90) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p + of_x_rechts_richtings_label);
   sprintf(buffer2, "%d", (_y1p + lengte_y_as_polair / 2) - of_y_rechts_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 90) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 90);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   // label 120
   */
   /* moveto (richtings label 120) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2p + of_x_rechts_richtings_label);
   sprintf(buffer2, "%d", (_y1p + lengte_y_as_polair / 2 - hulp_lengte) - of_y_rechts_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 120) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 120);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);

   /*
   // label 150
   */
   /* moveto (richtings label 150) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", (x1p + lengte_x_as_polair / 2 + hulp_lengte) - of_x_onder_richtings_label);
   sprintf(buffer2, "%d", _y1p - of_y_onder_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richting label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 150) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 150);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   // label 180
   */
   /* moveto (richtings label 180) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", (x1p + lengte_x_as_polair / 2) - of_x_onder_richtings_label);
   sprintf(buffer2, "%d", _y1p - of_y_onder_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 180) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 180);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);

   /*
   // label 210
   */
   /* moveto (richtings label 210) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", (x1p + lengte_x_as_polair / 2 - hulp_lengte) - of_x_onder_richtings_label);
   sprintf(buffer2, "%d", _y1p - of_y_onder_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richting label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 210) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 210);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   // label 240
   */
   /* moveto (richtings label 240) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p - of_x_links_richtings_label);
   sprintf(buffer2, "%d", (_y1p + lengte_y_as_polair / 2 - hulp_lengte) - of_y_links_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 240) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 240);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   //   label 270
   */
   /* moveto (richtings label 270) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p - of_x_links_richtings_label);
   sprintf(buffer2, "%d", (_y1p + lengte_y_as_polair / 2) - of_y_links_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 270);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   /*
   // label 300
   */
   /* moveto (richtings label 300) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1p - of_x_links_richtings_label);
   sprintf(buffer2, "%d", (_y1p + lengte_y_as_polair / 2 + hulp_lengte) - of_y_linksboven_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richtings label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label 300) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 300);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);



   /*
   // 330 label
   */
   /* moveto (richtings label 330) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", (x1p + lengte_x_as_polair / 2 - hulp_lengte) - of_x_boven_richtings_label);
   sprintf(buffer2, "%d", y2p + of_y_boven_richtings_label);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %richting label\n");
   fprintf(os_ps, "%s", line);

   /* show (richtings label) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   sprintf(buffer1, "%03d", 330);
   strcpy(line, "(");
   strcat(line, buffer1);
   strcat(line, ")");
   strcat(line, " show      %richtings label\n");
   fprintf(os_ps, "%s", line);


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                           Normalize_Energy                                */
/*                                                                           */
/*****************************************************************************/
int Normalize_Energy_Per_Verwachting_Of_Meting(const double max_energy)
{
   double norm_scale;
   int i;
   /*char buffer[128];*/

   /*
   //  b.v max_energy = 11:
   //
   //  dan norm_scale = 11 / 10 = 1.1
   //
   //  0 - 1.1   = klasse 1  (max_norm_class_limit[0] = 1.1)
   //  1.1 - 2.2 = klasse 2  (max_norm_class_limit[1] = 2.2)
   //  2.2 - 3.3 = klasse 3
   //      ...
   //  9.9 - 11  = klasse 10 (max_norm_class_limit[9] = 11)
   //
   */


   for (i = 0; i < NUMBER_ENERGY_CLASSES; i++)
      max_norm_class_limit[i] = 0.0;





   /* vanaf 05-08-2003 */
   /* norm_scale = max_energy / 10.0;   --> tot 5-8-2003 */
   if (max_energy > 0.05)
      norm_scale = max_energy / 10.0;
   else
      norm_scale = 0.05 / 10.0;
   /* eind vanaf 05-08-2003 */


   for (i = 0; i < NUMBER_ENERGY_CLASSES; i++)
   {
      max_norm_class_limit[i] = (i + 1) * norm_scale;

/**************  test ************/
/*
                 buffer[0] = '\0';
                 sprintf(buffer, "%le", max_norm_class_limit[i]);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************** test ************/

   } /* for (i = 0; i < NUMBER_ENERGY_CLASSES; i++) */


/**************  test ************/
/*
                 buffer[0] = '\0';
                 sprintf(buffer, "%le", max_energy);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************** test ************/





   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                   Set_Normalized_Energy_Class_Colors                      */
/*                                                                           */
/*****************************************************************************/
char* Set_Normalized_Energy_Class_Color(int norm_class)
{
  
   char* energy_color;

   /*char* norm_class_colors[NUMBER_ENERGY_CLASSES];*/

   /*energy_color[0] = '\0';*/
   switch (norm_class)
   {
      case 0 : energy_color = R_G_B_color_polair_class_0_1;
               break;
      case 1 : energy_color = R_G_B_color_polair_class_1_2;
               break;
      case 2 : energy_color = R_G_B_color_polair_class_2_3;
               break;
      case 3 : energy_color = R_G_B_color_polair_class_3_4;
               break;
      case 4 : energy_color = R_G_B_color_polair_class_4_5;
               break;
      case 5 : energy_color = R_G_B_color_polair_class_5_6;
               break;
      case 6 : energy_color = R_G_B_color_polair_class_6_7;
               break;
      case 7 : energy_color = R_G_B_color_polair_class_7_8;
               break;
      case 8 : energy_color = R_G_B_color_polair_class_8_9;
               break;
      case 9 : energy_color = R_G_B_color_polair_class_9_10;
               break;
      default : break;
   } /* switch(norm_class) */
   /*energy_color[11] = '\0';*/


   return energy_color;
}




/*****************************************************************************/
/*                                                                           */
/*                      TSA_Temp_Files_To_Polair_Postscript                  */
/*                                                                           */
/*****************************************************************************/
int TSA_Temp_Files_To_Polair_Postscript(const char* grafiek_orientatie, int pag_no, const char* station, const double scale_hz)
{
   int num_grafiek_orientatie;
   int k;                                     /* teller richtingen */
   int m;                                     /* teller frequenties */
   int v;                                     /* teller verwachtingen */
   int richtings_interval_grad;               /* hoek waarvoor nedwam richting geld (b.v. 30 graden) */
   int norm_class;                            /* genormalisserd klase nummer (0 t/m 9 */
   double begin_freq;                         /* begin grequentie bandje */
   double eind_freq;                          /* eind frequentie bandje */
   int xe1;
   int xe4;
   int ye1;
   int ye4;
   char line[256];
   char buffer1[10];
   char buffer2[10];
   char buffer3[10];
   char buffer4[10];
   char buffer5[10];
   int straal_binnen;                      
   int straal_buiten;
   int eerste_richting_grad;
   int tweede_richting_grad;
   int eerste_ps_richting_grad;
   int tweede_ps_richting_grad;
   char title[256];
   const int ofp1 = 18;                           /* veriticale afstand y2 - titel regel */
   char hm0_summary[128];
   char tm0_1_summary[128];
   char lfe_summary[128];
   char ff_summary[128];
   char dd_summary[128];
   char dir_tot_summary[128];
   char dir_lfe_summary[128];
   int xs1;                                       /* eerste summary item (b.v. Hm0) */
   int ys1;                                       /* eerste summary item (b.v. Hm0) */
   const int of_x_eerste_summary = 180;           /* i.v.m. summary regels */
   const int of_y_eerste_summary = 60;            /* i.v.m. summary regels */
   const int of_y_afstand_summary_lines = 15;     /* i.v.m. summary regels */
   double hulp_spectrum_waarde;
   /*char buffer[128];*/



   /*
   // van belang zijn de volgende 2 dynamische arrays:
   // FR[m]                    (m = num_frequentie)
   // TSA_spectrum[k][m][v]    (k = num_richting; m = num_frequentie; v = num_verwachting)
   //
   */

   /*
   //                  |
   //                  |         /                  xe4,ye4
   //                  |        / --------------  /
   //                  |       /               /
   //                  |      /             /
   //                xe1,ye1 /---------  /
   //                  |    /         /             ^
   //                  |   /       /                |
   //                  |  /     /   a               |  b
   //                  | /   /                      |
   //                  |/ /   30 degr.              v
   // -----------------|-----------------------
   //                  |<-------------->
   //                  |       c
   //                  |
   //                  |
   //                  |   c = a cos 30
   //                  |   b = a sin 30
   //                  |
   //                  |
   //                  |
   //                  |
   //
   */


   /*
   // bijv. (als NUM_RICHTINGEN = 12 ;richtings_interval is dan 30 graden)
   // richting 0 = 0 - 30 graden
   // richting 1 = 30 - 60 graden
   //          ....
   // richting 11 = 330 - 360 graden
   */
   richtings_interval_grad = 360 / NUM_RICHTINGEN;             /* b.v. */


   /* oorsprong bepalen (xe0, ye0) */
   /*xe0 = x1 + (int)((double)lengte_x_as_polair / 2.0 + 0.5);*/
   /*ye0 = _y1 + (int)((double)lengte_y_as_polair / 2.0 + 0.5);*/

   /*
   // forecast indice bepalen
   // pag_no 0: indice 0, 1, 2
   // pag_no 1: indice 3, 4, 5
   // etc.
   */
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
      sprintf(buffer1, "%d", x1p);
      sprintf(buffer2, "%d", y2p + ofp1);
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



      for (k = 0; k < NUM_RICHTINGEN; k++)
      {
         for (m = 0; m < NUM_FREQUENTIES; m++)
         {
            /* frequentie bandje bepalen
            //if ((m - 1) >= 0)
            //   begin_freq = FR[m] - ((FR[m] - FR[m - 1]) / 2);
            //else
            //   begin_freq = 0;
            //
            //if ((m + 1) < NUM_FREQUENTIES)
            //   eind_freq  = FR[m] + ((FR[m + 1] - FR[m]) / 2);
            //else
            //   eind_freq = FR[m];
            */

            /* energy is NIET gecentreerd om de frequenties maar lopen van-tot de eerst volgende (info van Jeanette Onvlee) */
            /*****if ((FR[m] < MAX_POLAIR_HZ) && (FR[m + 1] < MAX_POLAIR_HZ))***/   /* allerlaatste bandje wordt niet geplot op deze manier */
            if (m + 1 < NUM_FREQUENTIES)       /* zo worden alle frequenties geplot */
            {
               begin_freq = FR[m];
               eind_freq  = FR[m + 1];

               /* NB scale_hz: aantal pixels tussen 2 Hz cirkels (deze cirkels lopen per 0.1 Hz van 0.1 Hz - 0.4 Hz) */

               straal_binnen = (int)(scale_hz * 10 * begin_freq + 0.5);/*straal_binnen = scale_hz * 10.0 * begin_freq;*/
               straal_buiten = (int)(scale_hz * 10 * eind_freq + 0.5); /*straal_buiten = scale_hz * 10.0 * eind_freq;*/

               eerste_richting_grad = k * richtings_interval_grad;
               tweede_richting_grad = (k + 1) * richtings_interval_grad;

               /*
               // NEDWAM geeft de richtingen waar naar toe, deze omzetten naar waar vandaan (uniformiteit)
               */
               eerste_richting_grad += 180;
               if (eerste_richting_grad > 360)
                  eerste_richting_grad -= 360;

               tweede_richting_grad += 180;
               if (tweede_richting_grad > 360)
                  tweede_richting_grad -= 360;


               /*
               // arc en arcn functies van postscript reken met andere coordinaten stelsel (vanaf de pos. x- as tegen klok richting )
               */
               eerste_ps_richting_grad = 450 - eerste_richting_grad;
               if (eerste_ps_richting_grad > 360)
                  eerste_ps_richting_grad -= 360;

               tweede_ps_richting_grad = 450 - tweede_richting_grad;
               if (tweede_ps_richting_grad > 360)
                  tweede_ps_richting_grad -= 360;


               xe1 = (int)(xe0 + (straal_binnen) * sin((double)eerste_richting_grad * RAD) + 0.5);
               ye1 = (int)(ye0 + (straal_binnen) * cos((double)eerste_richting_grad * RAD) + 0.5);

               xe4 = (int)(xe0 + (straal_buiten) * sin((double)tweede_richting_grad * RAD) + 0.5);
               ye4 = (int)(ye0 + (straal_buiten) * cos((double)tweede_richting_grad * RAD) + 0.5);



               /* bepaal normalisatie klasse (loopt van 0 t/m 9 */
               /*norm_class = 0;*/                     /* initialistie */
               
               hulp_spectrum_waarde = 0.0;
               /* hulp_spectrum_waarde = atof(TSA_spectrum[k][m][v]);           -----> gaat niet goed onder LINUX */
               sscanf(TSA_spectrum[k][m][v], "%lf", &hulp_spectrum_waarde);
 
               if (hulp_spectrum_waarde <= max_norm_class_limit[0])
                  norm_class = 0;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[1])
                  norm_class = 1;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[2])
                  norm_class = 2;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[3])
                  norm_class = 3;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[4])
                  norm_class = 4;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[5])
                  norm_class = 5;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[6])
                  norm_class = 6;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[7])
                  norm_class = 7;
               else if (hulp_spectrum_waarde <= max_norm_class_limit[8])
                  norm_class = 8;
               else /*if (hulp_spectrum_waarde <= max_norm_class_limit[9])*/
                  norm_class = 9;



/**************  test ************/
/*
                 sprintf(buffer, "%lf", hulp_spectrum_waarde);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************** test ************/



/**************  test ************/
/*
                 if (norm_class == 9)
                 {
                 sprintf(buffer, "%d", norm_class);
                 fprintf(stderr, "%s", buffer);
                 getchar();
                 }
*/
/**************** test ************/

               /* alleen plotten als het binnen het polaire plaatje valt */
               if (straal_binnen < (lengte_x_as_polair / 2))
               {
                  /* newpath nedwam freq. bandje */
                  strcpy(line, "\0");
                  strcpy(line, "newpath         %nedwam freq bandje\n");
                  fprintf(os_ps, "%s", line);

                  /* color nedwam freq. bandje */
                  strcpy(line, "\0");
                  strcpy(line, Set_Normalized_Energy_Class_Color(norm_class));
                  strcat(line, " setrgbcolor %nedwam freq bandje\n");
                  fprintf(os_ps, "%s", line);

                  /* moveto nedwam freq bandje (xe4,ye4) */
                  strcpy(line, "\0");
                  buffer1[0] = '\0';
                  buffer2[0] = '\0';
                  sprintf(buffer1, "%d", xe4);
                  sprintf(buffer2, "%d", ye4);
                  strcpy(line, buffer1);
                  strcat(line, " ");
                  strcat(line, buffer2);
                  strcat(line, " moveto   %nedwam freq bandje (xe4,ye4)\n");
                  fprintf(os_ps, "%s", line);

                  /* arc nedwam freq bandje (xe2,ye2) */
                  strcpy(line, "\0");
                  buffer1[0] = '\0';
                  buffer2[0] = '\0';
                  buffer3[0] = '\0';
                  buffer4[0] = '\0';
                  buffer5[0] = '\0';
                  sprintf(buffer1, "%d", xe0);
                  sprintf(buffer2, "%d", ye0);
                  /*int_straal_buiten = (int)straal_buiten;*/
                  sprintf(buffer3, "%d", straal_buiten);/*sprintf(buffer3, "%d", int_straal_buiten);*/                            /* straal van de cirkel */
                  sprintf(buffer4, "%d", tweede_ps_richting_grad);
                  sprintf(buffer5, "%d", eerste_ps_richting_grad);
                  strcpy(line, buffer1);
                  strcat(line, " ");
                  strcat(line, buffer2);
                  strcat(line, " ");
                  strcat(line, buffer3);
                  strcat(line, " ");
                  strcat(line, buffer4);
                  strcat(line, " ");
                  strcat(line, buffer5);
                  strcat(line, " arc   %nedwam freq bandje (xe2, ye2)\n");
                  fprintf(os_ps, "%s", line);

                  /* lineto nedwam freq bandje (xe1,ye1) */
                  strcpy(line, "\0");
                  buffer1[0] = '\0';
                  buffer2[0] = '\0';
                  sprintf(buffer1, "%d", xe1);
                  sprintf(buffer2, "%d", ye1);
                  strcpy(line, buffer1);
                  strcat(line, " ");
                  strcat(line, buffer2);
                  strcat(line, " lineto   %nedwam freq bandje (xe1,ye1)\n");
                  fprintf(os_ps, "%s", line);

                  /* arc nedwam freq bandje (xe3,ye3) */
                  strcpy(line, "\0");
                  buffer1[0] = '\0';
                  buffer2[0] = '\0';
                  buffer3[0] = '\0';
                  buffer4[0] = '\0';
                  buffer5[0] = '\0';
                  sprintf(buffer1, "%d", xe0);
                  sprintf(buffer2, "%d", ye0);
                  /*int_straal_binnen = (int)straal_binnen;*/                            /* afronden (maar zonder 0.5 er bij tellen dat kan hier fout gaan door bijzonder klein getal)*/
                  sprintf(buffer3, "%d", straal_binnen);  /*sprintf(buffer3, "%d", int_straal_binnen);*/                            /* straal van de cirkel */
                  sprintf(buffer4, "%d", eerste_ps_richting_grad);
                  sprintf(buffer5, "%d", tweede_ps_richting_grad);
                  strcpy(line, buffer1);
                  strcat(line, " ");
                  strcat(line, buffer2);
                  strcat(line, " ");
                  strcat(line, buffer3);
                  strcat(line, " ");
                  strcat(line, buffer4);
                  strcat(line, " ");
                  strcat(line, buffer5);
                  strcat(line, " arcn   %nedwam freq bandje (xe3, ye3)\n");
                  fprintf(os_ps, "%s", line);

                  /* closepath (nedwam freq bandje) */
                  strcpy(line, "\0");
                  strcpy(line, "closepath         %nedwam freq bandje\n");
                  fprintf(os_ps, "%s", line);

                  /* fill (nedwam freq bandje) */
                  strcpy(line, "\0");
                  strcpy(line, "fill           %nedwam freq bandje\n");
                  fprintf(os_ps, "%s", line);

                  /* lege regel */
                  strcpy(line, "\n");
                  fprintf(os_ps, "%s", line);

               } /* if (straal_binnen < lengte_x_as) */
            } /* if ((FR[m] < MAX_POLAIR_HZ) && (FR[m + 1] < MAX_POLAIR_HZ)) */
         } /* for (m = 0; m < NUM_FREQUENTIES; m++) */
      } /* for (k = 0; k < NUM_RICHTINGEN; k++) */


      /*
      //
      // NEDWAM summary naast elke grafiek samenstellen
      //
      // b.v. Hm0 = 2.4m
      //      Tm0-1 = 5.9s
      //      LFE = 10cm2
      //      ff = 12 kn [hirlam-22]
      //      dd = 216 [hirlam-22]
      //      dir-tot = 230
      //      dir-LFE= 234
      //
      */

      Bepaal_Polair_Summary_Per_Grafiek(v, hm0_summary, tm0_1_summary, lfe_summary,
                                        ff_summary, dd_summary, dir_tot_summary, dir_lfe_summary);

      /* begin waarde (eerste summary item (b.v. Hm0) */
      xs1 = x2p + of_x_eerste_summary;
      ys1 = y2p - of_y_eerste_summary;



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
      strcpy(line, R_G_B_color_model_summary_polair);
      strcat(line, " setrgbcolor %summary\n");
      fprintf(os_ps, "%s", line);

      /*
      // Hm0
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, hm0_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);

      /*
      // Tm0-1
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - of_y_afstand_summary_lines);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, tm0_1_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);

      /*
      // LFE
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (2 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, lfe_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);

      /*
      // ff [hirlam]
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (3 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, ff_summary);
      strcat(line, "[hirlam-22]");
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);

      /*
      // dd [hirlam]
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (4 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, dd_summary);
      strcat(line, "[hirlam-22]");
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /*
      // dir tot
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (5 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, dir_tot_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /*
      // dir lfe
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (6 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, dir_lfe_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);


   } /* if (v < NUM_VERWACHTINGEN) */


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                    Write_Postscript_Polair_Index                          */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_Polair_Index()
{
   int xi1;
   int xi2;
   int yi1;
   int index_blok_hoogte;
   int index_blok_breedte;
   int i;                                           /* teller genormalisserde klassen / index blokjes */
   int x_links_onder;
   int y_links_onder;
   int x_links_boven;
   int y_links_boven;
   int x_rechts_boven;
   int y_rechts_boven;
   int x_rechts_onder;
   int y_rechts_onder;
   char line[256];
   char buffer1[10];                                /* opslag tussen waarden */
   char buffer2[10];                                /* opslag tussen waarden */
   int y_hoogte;
   int x_label;
   int y_label;
   const int of_label_x = 3;                        /* i.v.m. index label */
   const int of_label_y = 4;                        /* i.v.m. index label */
   const int of_index = 80;                         /* horiz. afstand rechter kant plotje (x2) tot index balk */
   const int of_x_normalisatie_text = 7;            /* i.v.m. text "normalized wave energy" naast index bal */
   const int of_y_normalisatie_text = 20;           /* i.v.m. text "normalized wave energy" naast index bal */

   /*
   //
   //        ----
   //       |    |    1.0
   //       |    |
   //       |----|
   //       |    |    ...
   //       |    |
   //       |----|
   //       |    |    0.2
   //       |    |
   //       |----|
   //       |    |    0.1
   //       |    |
   //        ----    yi1
   //      xi1   xi2
   //
   //      normalized wave energy
   */


   /* settings maken */
   index_blok_hoogte  = lengte_y_as_polair / NUMBER_ENERGY_CLASSES;
   index_blok_breedte = lengte_y_as_polair / NUMBER_ENERGY_CLASSES;

   xi1 = x2p + of_index;
   xi2 = xi1 + index_blok_breedte;
   yi1 = _y1p;


   /* per genormaliseerde klasse een index blokje maken + inkleuren */
   for (i = 0; i < NUMBER_ENERGY_CLASSES; i++)
   {
      /* eerst hoek punten per index (genormaliseerde klasse) blokje bepalen */
      x_links_onder  = xi1;
      y_links_onder  = yi1 + (i * index_blok_hoogte);
      x_links_boven  = xi1;
      y_links_boven  = yi1 + ((i + 1)* index_blok_hoogte);
      x_rechts_boven = xi2;
      y_rechts_boven = yi1 + ((i + 1)* index_blok_hoogte);
      x_rechts_onder = xi2;
      y_rechts_onder = yi1 + (i * index_blok_hoogte);


      /* newpath (index color) */
      strcpy(line, "\0");
      strcpy(line, "newpath        %index color\n");
      fprintf(os_ps, "%s", line);

      /* color (index color) */
      strcpy(line, "\0");
      strcpy(line, Set_Normalized_Energy_Class_Color(i));
      strcat(line, " setrgbcolor %index color\n");
      fprintf(os_ps, "%s", line);

      /* moveto (index color) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x_links_onder);
      sprintf(buffer2, "%d", y_links_onder);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %index color\n");
      fprintf(os_ps, "%s", line);

      /* lineto (index color) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x_links_boven);
      sprintf(buffer2, "%d", y_links_boven);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %index color\n");
      fprintf(os_ps, "%s", line);

      /* lineto (index color) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x_rechts_boven);
      sprintf(buffer2, "%d", y_rechts_boven);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %index color\n");
      fprintf(os_ps, "%s", line);

      /* lineto (index color) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x_rechts_onder);
      sprintf(buffer2, "%d", y_rechts_onder);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %index color\n");
      fprintf(os_ps, "%s", line);

      /* closepath (index color) */
      strcpy(line, "\0");
      strcpy(line, "closepath      %index color\n");
      fprintf(os_ps, "%s", line);

      /* fill (index color) */
      strcpy(line, "\0");
      strcpy(line, "fill           %index color\n");
      fprintf(os_ps, "%s", line);

   }

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   // deellijnen tussen index blokjes
   */

   /* newpath (index deellijn) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %index deellijn\n");
   fprintf(os_ps, "%s", line);

   /* color (index deellijn) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_polair_index_kader);
   strcat(line, " setrgbcolor %index deellijn\n");
   fprintf(os_ps, "%s", line);

   for (i = 1; i < NUMBER_ENERGY_CLASSES; i++)
   {
      /* eerst begin en eindpunt per deellijnindex (genormaliseerde klasse) blokje bepalen */
      y_hoogte =  yi1 + (i * index_blok_hoogte);

      /* moveto (index deellijn) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xi1);
      sprintf(buffer2, "%d", y_hoogte);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %index deellijn\n");
      fprintf(os_ps, "%s", line);

      /* lineto (index deellijn) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xi2);
      sprintf(buffer2, "%d", y_hoogte);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %index deellijn\n");
      fprintf(os_ps, "%s", line);

      /* stroke (index deellijn) */
      strcpy(line, "\0");
      strcat(line, "stroke   %index deellijn\n");
      fprintf(os_ps, "%s", line);
   } /* for (i = 1; i < NUMBER_ENERGY_CLASSES - 1; i++) */

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   // kader om de index balk zetten
   */

   /* newpath (index kader) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %index kader\n");
   fprintf(os_ps, "%s", line);

   /* color (index kader) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_polair_index_kader);
   strcat(line, " setrgbcolor %index kader\n");
   fprintf(os_ps, "%s", line);

   /* moveto (index kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", xi1);
   sprintf(buffer2, "%d", yi1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %index kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (index kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", xi1);
   sprintf(buffer2, "%d", yi1 + lengte_y_as_polair);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %index kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (index kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", xi2);
   sprintf(buffer2, "%d", yi1 + lengte_y_as_polair);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %index kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (index kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", xi2);
   sprintf(buffer2, "%d", yi1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %index kader\n");
   fprintf(os_ps, "%s", line);

   /* closepath (index kader) */
   strcpy(line, "\0");
   strcpy(line, "closepath      %index kader\n");
   fprintf(os_ps, "%s", line);

   /* stroke (index kader) */
   strcpy(line, "\0");
   strcat(line, "stroke   %index kader\n");
   fprintf(os_ps, "%s", line);

    /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   // index waarden (labels) (0.1 - 1.0 bij de index balk zetten)
   */

   /* newpath (index label) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %index label\n");
   fprintf(os_ps, "%s", line);

   /* color (index label) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_polair_index_kader);
   strcat(line, " setrgbcolor %index label\n");
   fprintf(os_ps, "%s", line);

   /* font (index label) */
   strcpy(line, "\0");
   strcpy(line, font_keuze);
   fprintf(os_ps, "%s", line);

   /* font (index label) */
   strcpy(line, "\0");
   strcpy(line, "10 scalefont       %index label\n");
   fprintf(os_ps, "%s", line);

   /* font (index label) */
   strcpy(line, "\0");
   strcpy(line, "setfont            %index label\n");
   fprintf(os_ps, "%s", line);

   for (i = 0; i < NUMBER_ENERGY_CLASSES + 1; i++)
   {
      /* eerst begin en eindpunt per deellijnindex (genormaliseerde klasse) blokje bepalen */
      x_label = xi2 + of_label_x;
      y_label = yi1 + (i * index_blok_hoogte) - of_label_y;

      /* moveto (index label) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x_label);
      sprintf(buffer2, "%d", y_label);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %index label\n");
      fprintf(os_ps, "%s", line);

      /* show (index label) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      sprintf(buffer1, "%0.1f", (float)i / NUMBER_ENERGY_CLASSES);
      strcpy(line, "(");
      strcat(line, buffer1);
      strcat(line, ")");
      strcat(line, " show      %index_label\n");
      fprintf(os_ps, "%s", line);

   } /* for (i = 0; i < NUMBER_ENERGY_CLASSES + 1; i++) */


   /*
   //  normalistaie text bij index balk zetten
   */

   /* newpath (normalisatie text) */
   strcpy(line, "\0");
   strcpy(line, "newpath        %normalisatie text\n");
   fprintf(os_ps, "%s", line);

   /* color (normalisatie text) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_polair_index_kader);
   strcat(line, " setrgbcolor %normalisatie text\n");
   fprintf(os_ps, "%s", line);

   /* font (normalisatie text) */
   strcpy(line, "\0");
   strcpy(line, font_keuze);
   fprintf(os_ps, "%s", line);

   /* font (normalisatie text) */
   strcpy(line, "\0");
   strcpy(line, "12 scalefont       %normalisatie text\n");
   fprintf(os_ps, "%s", line);

   /* font (normalisatie text) */
   strcpy(line, "\0");
   strcpy(line, "setfont            %normalisatie text\n");
   fprintf(os_ps, "%s", line);

   /* moveto (normalisatie text) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", xi1 - of_x_normalisatie_text);
   sprintf(buffer2, "%d", yi1 + of_y_normalisatie_text);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto     %normalisatie text\n");
   fprintf(os_ps, "%s", line);

   /* rotate (normalisatie text) */
   strcpy(line, "\0");
   strcat(line, "90 rotate        %normalisatie text\n");
   fprintf(os_ps, "%s", line);

   /* show (normalisatie text) */
   strcpy(line, "\0");
   strcat(line, "(normalized wave energy) show    %normalisatie text\n");
   fprintf(os_ps, "%s", line);

   /* rotate (normalisatie text) [weer terugzetten]*/
   strcpy(line, "\0");
   strcat(line, "-90 rotate       %normalisatie text\n");
   fprintf(os_ps, "%s", line);



   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                     Bepaal_Polair_Summary_Per_Grafiek                     */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Polair_Summary_Per_Grafiek(const int v, char* hm0_summary, char* tm0_1_summary, char* lfe_summary,
                                                   char* ff_summary, char* dd_summary, char* dir_tot_summary, char* dir_lfe_summary)
{
   char buffer[100];
   double HM0;
   double dir_tot;
   double dir_lfe;
   /* v = teller verwachtingen (indice nummer) */


   /*initialisatie */
   buffer[0] = '\0';


   /* summary regels samenstellen */

   /*
   // HM0 ( = 4 * sqrt(Emean) )
   */
   hm0_summary[0] = '\0';
   buffer[0]      = '\0';
   strcat(hm0_summary, "Hm0 = ");
   HM0 = 4 * sqrt(EMEAN[v]);
   sprintf(buffer, "%.1f", HM0);
   strcat(hm0_summary, buffer);
   strcat(hm0_summary, "m  ");

   /*
   // TM0-1 (= 1 / FMEAN)
   */
   tm0_1_summary[0] = '\0';
   buffer[0]        = '\0';
   strcat(tm0_1_summary, "Tm0-1 = ");
   if (FMEAN[v] != MAXFLOAT)
      sprintf(buffer, "%.1f", 1.0 / FMEAN[v]);
   else
      sprintf(buffer, "%s", "-");
   strcat(tm0_1_summary, buffer);
   strcat(tm0_1_summary, "s  ");

   /*
   // LFE ( = E10)
   */
   lfe_summary[0] = '\0';
   buffer[0]      = '\0';
   strcat(lfe_summary, "LFE = ");
   sprintf(buffer, "%.0f", E10[v] * 10000);     /* van m2 -> cm2 */
   strcat(lfe_summary, buffer);
   strcat(lfe_summary, "cm2  ");

   /*
   // ff (hirlam wind snelheid)
   */
   ff_summary[0] = '\0';
   buffer[0]     = '\0';
   strcat(ff_summary, "ff = ");
   if (FF_hirlam[v] != MAXFLOAT)
      sprintf(buffer, "%.0f", FF_hirlam[v]);    /* (afgeronde) knopen */
   else
      sprintf(buffer, "%s", "-");
   strcat(ff_summary, buffer);
   strcat(ff_summary, "kn  ");


   /*
   // dd (hirlam wind richting)
   */
   dd_summary[0] = '\0';
   buffer[0]         = '\0';
   strcat(dd_summary, "dd = ");
   if (DDD_hirlam[v] != MAXFLOAT)
      sprintf(buffer, "%03.0f", DDD_hirlam[v]);
   else
      sprintf(buffer, "%s", "-");
   strcat(dd_summary, buffer);
   strcat(dd_summary, "  ");


   /*
   // dir.tot
   */
   dir_tot_summary[0] = '\0';
   buffer[0]         = '\0';
   strcat(dir_tot_summary, "dir.tot = ");
   if (THQ[v] != MAXFLOAT)
   {
      dir_tot = (THQ[v] * DEG) + 180;           /* omzetten neer deg + naar richting waar van daan */
      if (dir_tot > 360.0)
         dir_tot -= 360.0;
      sprintf(buffer, "%03.0f", dir_tot);
   }
   else
      sprintf(buffer, "%s", "-");

   strcat(dir_tot_summary, buffer);
   strcat(dir_tot_summary, "  ");

   /*
   // dir. LFE
   */
   dir_lfe_summary[0] = '\0';
   buffer[0]          = '\0';
   strcat(dir_lfe_summary, "dir.LFE = ");
   if (E10D[v] != MAXFLOAT)
   {
      dir_lfe = (E10D[v] * DEG) + 180;          /* omzetten neer deg + naar richting waar van daan */
      if (dir_lfe > 360.0)
         dir_lfe -= 360.0;
      sprintf(buffer, "%03.0f", dir_lfe);
   }
   else
      sprintf(buffer, "%s", "-");

   strcat(dir_lfe_summary, buffer);
   strcat(dir_lfe_summary, "  ");


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                      Bepaal_Max_Scale_Hz_Verwachting                      */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Scale_Hz_Verwachting(const double max_energy, double *max_polair_hz)
{
   /* loopt gedeeltelijk (m2/Hz versus m2/Hz/rad) parallel met function: int Bepaal_Scale_Y_X_Max_TSA(const double max_energy, int *scale_y_max, int *scale_x_max) [file: gps_nedwam_ps.c] */
   /*char buffer[128];*/

   /*
   //
   // 0    -  5.0 m2/Hz/rad  (0       -  500000 .1cm2s/rad)  --> max_polair_hz 0.4 Hz)
   // 5.0  - 50.0 m2/Hz/rad  (500000  - 5000000 .1cm2s/rad)  --> max_polair_hz 0.3 Hz)
   //      > 50.0 m2/Hz/rad  (        > 5000000 .1cm2s/rad)  --> max_polair_hz 0.2 Hz)
   */

   if (max_energy < 5.0)
      *max_polair_hz = 0.4;
   else  if (max_energy < 50.0)
      *max_polair_hz = 0.3;
   else
      *max_polair_hz = 0.2;


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*         Bepaal_Max_Energy_Per_Verwachting (dus NIET per ps pagina)        */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Energy_Per_Verwachting(double* max_energy, const int pag_no, const char* grafiek_orientatie)
{
   int k;                          /* teller richtingen */
   int m;                          /* teller frequenties */
   int v;                          /* teller verwachtingen */
   /*char buffer[128];*/
   int num_grafiek_orientatie;
   double hulp_spectrum_waarde;


   /* template:  */

   /* per pagina (met 3 verwachtingen) per verwachting de max energy er uit halen
   // bv.:
   // pagina 0:    0hr,  +3hr,  +6hr
   // pagina 1:   +9hr, +12hr, +15hr
   // pagina 2:  +18hr, +21hr, +24hr
   // pagina 3:  +27hr, +30hr, +33hr
   // pagina 4:  +36hr, +39hr, +42hr
   // pagina 5:  +45hr, +48hr
   //
   // forecast indice bepalen
   // pag_no 0: indice 0, 1, 2
   // pag_no 1: indice 3, 4, 5
   // etc.
   */

   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 0;
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 1;
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
      num_grafiek_orientatie = 2;

   v = pag_no * 3 + num_grafiek_orientatie;         /* 3 verwachtingen op 1 pagina */

   /* initialisatie */
   *max_energy = 0.0;


   if (v < NUM_VERWACHTINGEN)                       /* omdat op laatste pagina kunnen minder dan 3 verwachtingen staan */
   {
      for (k = 0; k < NUM_RICHTINGEN; k++)
      {
         for (m = 0; m < NUM_FREQUENTIES; m++)
         {

            /* hulp_spectrum_waarde = atof(TSA_spectrum[k][m][v]);---->   GAAT NIET GOED ONDER SUZE LINUX */ 
            sscanf(TSA_spectrum[k][m][v], "%le", &hulp_spectrum_waarde);

            /* if (atof(TSA_spectrum[k][m][v]) > *max_energy)      ---->   GAAT NIET GOED ONDER SUZE LINUX */
            if (hulp_spectrum_waarde > *max_energy)
            {
               /* *max_energy = atof(TSA_spectrum[k][m][v]);       ---->   GAAT NIET GOED ONDER SUZE LINUX */
               *max_energy = hulp_spectrum_waarde;


/**************  test ************/
/*
                 printf("max energy: %le\n", *max_energy);
                 getchar();
*/
/**************  test ************/

            }
         } /* for (m = 0; m < NUM_FREQUENTIES; m++) */
      } /* for (k = 0; k < NUM_RICHTINGEN; k++) */
   } /* if (v < NUM_VERWACHTINGEN) */


   return 0;
}


