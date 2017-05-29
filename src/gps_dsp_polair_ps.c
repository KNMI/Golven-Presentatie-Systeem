#if !defined(gps_h)                         /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <stdio.h>
#include <math.h>                           /* o.a. cos, sin */
#include <values.h>                         /* MAXFLOAT */
#include <time.h>                           /* tm structure */
#include <stdlib.h>
#include <string.h>

#if defined(WINDOWS)                        /* zie gps.h */
#include <dir.h>                            /* o.a. getcwd() in windows */
#else
#include <unistd.h>                         /* o.a. getcwd() in UNIX */
#endif

/*
// LET OP  I.V.M. VOORKOMEN RUN_TIME ERROR
// definitie "extern FILE* os_ps" omdat i.v.m. o.a. aanroep Write_Postscript_General();
// (hier is FILE* os_ps al gedefinieerd)
/*

/* variabelen voor deze module */
char dsp_outputfilenaam[256];
char cic_dsp_tempfilenaam_0[256];          /* voor dsp polair plotje */
char cic_dsp_tempfilenaam_1[256];          /* voor dsp polair plotje */
char cic_dsp_tempfilenaam_2[256];          /* voor dsp polair plotje */
char cic_lds_tempfilenaam_0[256];          /* voor lds summary naast dsp polair plotje */
char cic_lds_tempfilenaam_1[256];          /* voor lds summary naast dsp polair plotje */
char cic_lds_tempfilenaam_2[256];          /* voor lds summary naast dsp polair plotje */

/* functions in deze module */
int Write_DSP_Polair_Postscript_Files(void);
int Bepaal_Polair_DSP_Outputfilenaam_Per_Locatie(const char* station, int pag_no);
int Read_Tempfile_In_DSP_Spectrum(char* grafiek_orientatie);
int Bepaal_Max_Energy_Per_DSP_Tempfile(double* max_energy);
int Bepaal_DSP_Outputfilenaam_Per_Locatie(char* station);
int DSP_Temp_Files_To_Polair_Postscript(char* grafiek_orientatie, int cic_uren_interval, double scale_hz, const char* location_station);
int Bepaal_DSP_Polair_Summary_Per_Grafiek(char* grafiek_orientatie, char* hm0_summary, char* tm0_1_summary, char* lfe_summary,
                                          char* ff_summary, char* dd_summary, char* dir_tot_summary, char* dir_lfe_summary,
                                          char* hmax_summary, char* thmax_summary, char* h_1_50_summary, char* aantal_golven_summary, char* fp_summary);
int Bepaal_Filenamen_DSP_Temp_Files_Per_Locatie(char* station, int cic_uren_interval);
int Bepaal_Max_Scale_Hz_DSP(const double max_energy, double *max_polair_hz);

/* globale var's in andere module */
extern char OS[8];                                         /* gpsmain.c */
extern int x1p;                                            /* gpsmain.c */
extern int x2p;                                            /* gpsmain.c */
extern int y2p;                                            /* gpsmain.c */
extern int _y1p;                                           /* gpsmain.c */
extern char font_keuze[50];                                /* gpsmain.c */
extern FILE* os_ps;                                        /* gpsmain.c */
extern char DSP_spectrum[NUM_DSP_FREQUENTIES][NUM_DSP_RICHTINGEN][NUM_DSP_REALS];  /* [file: gps_dsp_input.c] */
extern char stations[NUM_STATIONS][NUM_STATIONS_2];        /* gpsmain.c; NUM_STATIONS arrayplaatsen van elk 4 char */
extern char JJJJMMDDUU[11];                                /* gpsmain.c; via argument list */
extern int xe0;                                            /* gpsmain.c; oorsprong polair plot */
extern int ye0;                                            /* gpsmain.c; oorsprong polair plot */
extern double max_norm_class_limit[NUMBER_ENERGY_CLASSES]; /* gpsmain.c */
extern const int lengte_y_as_polair;                       /* gpsmain.c */
extern const int lengte_x_as_polair;                       /* gpsmain.c */


/* prototype in andere module */
extern int Write_Log(char* bericht);
extern int Write_Postscript_General(void);
extern int Write_Postscript_End(void);
extern int Write_Postscript_Polair(char* grafiek_orientatie);                             /* file: gps_nedwam_polair_ps.c */
extern int Normalize_Energy_Per_Verwachting_Of_Meting(const double max_energy);           /* file: gps_nedwam_polair_ps.c */
extern int Write_Postscript_Polair_Richtingen(const double max_polair_hz);                /* file: gps_nedwam_polair_ps.c */
extern int Write_Postscript_Polair_Index(void);                                           /* file: gps_nedwam_polair_ps.c */
extern char* Set_Normalized_Energy_Class_Color(int norm_class);                           /* file: gps_nedwam_polair_ps.c */
extern int Initialiseer_DSP_Spectrum_Array(void);                                         /* file: gps_nedwam_polair_ps.c */



/*****************************************************************************/
/*                                                                           */
/*                   Write_DSP_Polair_Postscript_Files                       */
/*                                                                           */
/*****************************************************************************/
int Write_DSP_Polair_Postscript_Files()
{
   char info[512];
   char volledig_path[512];
   char grafiek_orientatie[256];
   int i;
   double max_energy;
   int cic_uren_interval;                                   /* in CIC mode om de 1uur metingen plotten (in TSA mode om de 3 uur) */
   double scale_hz;
   double max_polair_hz;

   /* initialisatie */
   cic_uren_interval = 1;                                   /* metingen om de 1 uur gebruiken */
   /*scale_hz = lengte_y_as_polair / 2.0 / (double)AANTAL_CIRKELS_HZ; */     /* bijv 200 / 2 / 4 = 25 */


   /* per locatie postscript files aanmaken */
   for (i = 0; i < NUM_STATIONS; i++)
   {
      if (strcmp(stations[i], "\0") != 0)                               /* niet leeg */
      {
         /* 3 temp files per locatie per ps-pagina (JJJJMMDDUU, JJJJMMDDUU - 1 uur en JJJMMDDUU - 2 uur) uit temp directory inlezen */
         Bepaal_Filenamen_DSP_Temp_Files_Per_Locatie(stations[i], cic_uren_interval); /* tempfilenaam_0 , tempfilenaam_1, tempfilenaam_2 t.o.v. JJJJMMDDUU worden bepaald */

         /* 1 output postscript file voor 3 temp files */
         Bepaal_DSP_Outputfilenaam_Per_Locatie(stations[i]);


         if ((os_ps = fopen(dsp_outputfilenaam, "w")) == NULL)              /* dus mislukt */
         {
            getcwd(volledig_path, 512);

            /* bericht samen stellen */
            strcpy(info, "\0");
            strcat(info, volledig_path);

            if (strcmp(OS, "WINDOWS") == 0)
               strcat(info, "\\");
            else
               strcat(info, "/");

            strcat(info, dsp_outputfilenaam);
            strcat(info, " (DSP) niet te schrijven\n");

            /* naar log schrijven */
            Write_Log(info);

         } /* if ((os_ps = fopen(outputfilenaam, "w")) == NULL) */
         else /* outputfile is te schrijven */
         {
            fclose(os_ps);    /* file is nu weer leeg */

            os_ps = fopen(dsp_outputfilenaam, "a");

            /* ps pagina correct beginnen */
            Write_Postscript_General();

            /* onderste grafiek (I.V.M. 'TRANSLATE' ONDERSTE GRAFIEK ALS EERSTE DOEN) */   /* tempfilenaam_2 */
            strcpy(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK);
            Write_Postscript_Polair(grafiek_orientatie);
            Read_Tempfile_In_DSP_Spectrum(grafiek_orientatie);
            Bepaal_Max_Energy_Per_DSP_Tempfile(&max_energy);

            Bepaal_Max_Scale_Hz_DSP(max_energy, &max_polair_hz);                       /* uitkomst 0.4 of 0.3 of 0.2 */
            scale_hz = lengte_y_as_polair / 2.0 / (double)(max_polair_hz * 10);        /* bijv 200 / 2 / (0.4 * 10) = 25 */

            Normalize_Energy_Per_Verwachting_Of_Meting(max_energy);
            DSP_Temp_Files_To_Polair_Postscript(grafiek_orientatie, cic_uren_interval, scale_hz, stations[i]);
            Write_Postscript_Polair_Richtingen(max_polair_hz);
            Write_Postscript_Polair_Index();

            /* middelste grafiek (I.V.M. 'TRANSLATE' MIDDELSE GRAFIEK ALS TWEEDE DOEN)*/
            strcpy(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK);
            Write_Postscript_Polair(grafiek_orientatie);
            Read_Tempfile_In_DSP_Spectrum(grafiek_orientatie);
            Bepaal_Max_Energy_Per_DSP_Tempfile(&max_energy);

            Bepaal_Max_Scale_Hz_DSP(max_energy, &max_polair_hz);                       /* uitkomst 0.4 of 0.3 of 0.2 */
            scale_hz = lengte_y_as_polair / 2.0 / (double)(max_polair_hz * 10);        /* bijv 200 / 2 / (0.4 * 10) = 25 */

            Normalize_Energy_Per_Verwachting_Of_Meting(max_energy);
            DSP_Temp_Files_To_Polair_Postscript(grafiek_orientatie, cic_uren_interval, scale_hz, stations[i]);
            Write_Postscript_Polair_Richtingen(max_polair_hz);
            Write_Postscript_Polair_Index();

            /* bovenste grafiek (I.V.M. 'TRANSLATE' MIDDELSE GRAFIEK ALS LAATSTE DOEN)*/
            strcpy(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK);
            Write_Postscript_Polair(grafiek_orientatie);
            Read_Tempfile_In_DSP_Spectrum(grafiek_orientatie);
            Bepaal_Max_Energy_Per_DSP_Tempfile(&max_energy);

            Bepaal_Max_Scale_Hz_DSP(max_energy, &max_polair_hz);                       /* uitkomst 0.4 of 0.3 of 0.2 */
            scale_hz = lengte_y_as_polair / 2.0 / (double)(max_polair_hz * 10);        /* bijv 200 / 2 / (0.4 * 10) = 25 */

            Normalize_Energy_Per_Verwachting_Of_Meting(max_energy);
            DSP_Temp_Files_To_Polair_Postscript(grafiek_orientatie, cic_uren_interval, scale_hz, stations[i]);
            Write_Postscript_Polair_Richtingen(max_polair_hz);
            Write_Postscript_Polair_Index();

            /* ps pagina correct afsluiten */
            Write_Postscript_End();

            fclose(os_ps);
         } /* else (outputfile is te schrijven) */

      } /* if (strcmp(stations[1], "\0") != 0)) */
   } /* for (i = 0; i < NUM_STATIONS, i++) */


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*           Read_Tempfile_In_DSP_Spectrum(grafiek_orientatie)               */
/*                                                                           */
/*****************************************************************************/
int Read_Tempfile_In_DSP_Spectrum(char* grafiek_orientatie)
{
   int k;                          /* teller richtingen */
   int m;                          /* teller frequenties */
   char line[513];
   char tempfilenaam[256];
   FILE* temp;


   /* initialisatie */
   tempfilenaam[0] = '\0';
   Initialiseer_DSP_Spectrum_Array();

   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
      strcpy(tempfilenaam, cic_dsp_tempfilenaam_0);                 /* meest recente op die ps bladzijde */
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
      strcpy(tempfilenaam, cic_dsp_tempfilenaam_1);
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
      strcpy(tempfilenaam, cic_dsp_tempfilenaam_2);                 /* minst recente op die ps bladzijde */


   if ((temp = fopen(tempfilenaam, "r")) != NULL)           /* dus gelukt */
   {
      for (m = 0; m < NUM_DSP_FREQUENTIES; m++)
      {
         for (k = 0; k < NUM_DSP_RICHTINGEN; k++)
         {
            if (fgets(line, 512, temp) != NULL)
               strcpy(DSP_spectrum[m][k], line);
         }
      } /* for (m = 0; m < NUM_DSP_FREQUENTIES; m++) */

      fclose(temp);

   } /* if ((temp = fopen(tempfilenaam, "r")) != NULL) */


/**************  test ************/
/*
               fprintf(stderr, "%s", tempfilenaam);
               getchar();
*/
/**************** test ************/




   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*         Bepaal_Max_Energy_Per_DSP_Tempfile (dus NIET per ps pagina)       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Energy_Per_DSP_Tempfile(double* max_energy)
{
   int k;                          /* teller richtingen */
   int m;                          /* teller frequenties */
   /*char buffer[10];*/

   /* initialisatie */
   *max_energy = 0.0;

   /* omdat t/m 0.4 Hz maar geplot wordt ook het maximum hier uit halen (en niet uit alle beschikbare metingen) */

   /*for (m = 0; m < NUM_DSP_FREQUENTIES; m++)*/
   for (m = 0; m <= DSP_FREQ_0_40; m++)            /* DSP_FREQ_0_40 = indice van 0.40 Hz uit DSP_spectrum */
   {
      for (k = 0; k < NUM_DSP_RICHTINGEN; k++)
      {
         if (atof(DSP_spectrum[m][k]) > *max_energy)
            *max_energy = atof(DSP_spectrum[m][k]);
      } /* for (k = 0; k < NUM_DSP_RICHTINGEN; k++) */
   } /* for (m = 0; m < NUM_DSP_FREQUENTIES; m++) */


/**************  test ************/
/*
               sprintf(buffer, "%lf", *max_energy);
               fprintf(stderr, "%s", buffer);
               getchar();
*/
/**************** test ************/

   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                   Bepaal_Outputfilenaam_Per_Locatie                       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_DSP_Outputfilenaam_Per_Locatie(char* station)
{
   /* b.v. output_cic/DSP_K13.PS */


   /* initialisatie */
   strcpy(dsp_outputfilenaam, "\0");

   /* postscript outputfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
   {
      strcpy(dsp_outputfilenaam, "output_cic\\");
   }
   else                                                /* UNIX */
   {
      strcpy(dsp_outputfilenaam, "output_cic/");
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(dsp_outputfilenaam, "DSP_");
   strcat(dsp_outputfilenaam, station);
   strcat(dsp_outputfilenaam, ".PS");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                 DSP_Temp_Files_To_Polair_Postscript                       */
/*                                                                           */
/*****************************************************************************/
int DSP_Temp_Files_To_Polair_Postscript(char* grafiek_orientatie, int cic_uren_interval, double scale_hz, const char* location_station)
{
   int richtings_interval_grad;               /* hoek waarvoor dsp cic richting geld (b.v. 10 graden) */
   char tempfilenaam[256];
   FILE* temp;
   int uren_terug;
   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   char datumtijd[100];
   struct tm time_struct;
   int tijd_ok;
   char title[256];
   const int of_title_dsp = 13;
   char line[256];
   char buffer1[10];
   char buffer2[10];
   char buffer3[10];
   char buffer4[10];
   char buffer5[10];
   char station[4];                               /* wordt alleen gebruikt als een temp file er niet is en dan toch een title maken */
   int k;                                         /* teller richtingen */
   int m;                                         /* teller frequenties */
   int straal_binnen;
   int straal_buiten;
   int eerste_richting_grad;
   int tweede_richting_grad;
   int eerste_ps_richting_grad;
   int tweede_ps_richting_grad;
   double begin_freq;
   double eind_freq;
   int xe1;
   int ye1;
   int xe4;
   int ye4;
   int norm_class;                                /* genormalisserd klase nummer (0 t/m 9 */
   int xs1;                                       /* eerste summary item (b.v. Hm0) */
   int ys1;                                       /* eerste summary item (b.v. Hm0) */
   const int of_x_eerste_summary = 180;           /* i.v.m. summary regels */
   const int of_y_eerste_summary = 20;            /* i.v.m. summary regels */  
   const int of_y_afstand_summary_lines = 15;     /* i.v.m. summary regels */
   char hm0_summary[128];
   char tm0_1_summary[128];
   char lfe_summary[128];
   char ff_summary[128];
   char dd_summary[128];
   char dir_tot_summary[128];
   char dir_lfe_summary[128];
   char hmax_summary[128];
   char thmax_summary[128];
   char h_1_50_summary[128];
   char aantal_golven_summary[128];
   char fp_summary[128];
   /*char buffer[100];*/



   /*
   // van belang is de volgende statische array:
   // DSP_spectrum[NUM_DSP_FREQUENTIES][NUM_DSP_RICHTINGEN][NUM_DSP_REALS];
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
   //                  |/ /   10 degr.              v
   // -----------------|-----------------------
   //                  |<-------------->
   //                  |       c
   //                  |
   //                  |
   //                  |   c = a cos 10
   //                  |   b = a sin 10
   //                  |
   //                  |
   //                  |
   //                  |
   //
   */





   /*
   // bijv. (als NUM_DSP_RICHTINGEN = 36 ;richtings_interval is dan 10 graden)
   // richting 0 = 0 - 10 graden
   // richting 1 = 20 - 30 graden
   //          ....
   // richting 35 = 350 - 360 graden
   */
   richtings_interval_grad = 360 / NUM_DSP_RICHTINGEN;


   /* NB oorsprong is al bepaald in main module bepalen (xe0, ye0) */


/**************  test ************/
/*
               sprintf(buffer1, "%d", _y1);
               fprintf(stderr, "%s", buffer1);    // ye0: 150 globaal; 120 lokaal; _y1 = 20 lokaal
               getchar();
*/
/**************** test ************/



   /* initialisatie */
   tempfilenaam[0] = '\0';
   title[0]        = '\0';
   datumtijd[0]    = '\0';



   /*
   // afh. van grafiek orientatie een temp file openen
   */
   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
   {
      strcpy(tempfilenaam, cic_dsp_tempfilenaam_0);                 /* laatste temp file */
      uren_terug = 0;
   }
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
   {
      strcpy(tempfilenaam, cic_dsp_tempfilenaam_1);                 /* 1 uur terug */
      uren_terug = 1 * cic_uren_interval;                   /* i.v.m. title (cic_uren_interval = 1 = normale CIC/DSP mode) */
   } /* else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0) */
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
   {
      strcpy(tempfilenaam, cic_dsp_tempfilenaam_2);                 /* 2 uur terug */
      uren_terug = 2 * cic_uren_interval;                   /* i.v.m. title (cic_uren_interval = 1 = normale CIC/DSP mode) */
   } /* else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0) */




   /*
   // temp file inlezen en schrijven naar ps output file
   */

   if ((temp = fopen(tempfilenaam, "r")) != NULL)  /* dus gelukt */
   {
      /*
      /////////// title (WEL aanwezige data) ///////////
      */

      /* hulp voor file namen van 1 en 2 uur terug bepalen (t.o.v van 0 uur terug = JJJJJMMDDUU) */

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

      time_struct.tm_year  = num_JJJJ - 1900;
      time_struct.tm_mon   = num_MM - 1;
      time_struct.tm_mday  = num_DD;
      time_struct.tm_hour  = num_UU - uren_terug;
      time_struct.tm_min   = 0;
      time_struct.tm_sec   = 0;
      time_struct.tm_isdst = -1;                      /* daylight saving time: -1 = no info available */

      tijd_ok = 1;
      if (mktime(&time_struct) == -1)
         tijd_ok = 0;

      if (tijd_ok == 1)
         (void)strftime(datumtijd, 100, "%d-%m-%Y %H.00 UTC", &time_struct);     /* b.v. 2001121000 */



      /*
      // title samenstellen
      */

      /* blanco deel */
      strcpy(title, "                     ");

      /* station */
      /*strncpy(station, tempfilenaam + (strlen(tempfilenaam) - 18), 3);   */
      /*station[3] = '\0';                                                 */
      strcat(title, location_station);
      strcat(title, " ");

      /* datum tijd */
      strcat(title, datumtijd);


      /*
      // naar ps output file
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
      sprintf(buffer1, "%d", x1p);
      sprintf(buffer2, "%d", y2p + of_title_dsp);
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
      //////////// plotten bandjes //////////
      */

      for (m = 0; m <= DSP_FREQ_0_40; m++)            /* DSP_FREQ_0_40 = indice van 0.40 Hz uit DSP_spectrum */
/********* test ***********/
/*      for (m = 0; m <= 48; m++)*/
/************************/
      {
         for (k = 0; k < NUM_DSP_RICHTINGEN; k++)
         {
            /* frequentie bandje */
            begin_freq = 0.03 + (m * DSP_FREQ_VERSCHIL) - (DSP_FREQ_VERSCHIL / 2.0);
            eind_freq  = 0.03 + (m * DSP_FREQ_VERSCHIL) + (DSP_FREQ_VERSCHIL / 2.0);


            /* NB scale_hz: aantal pixels tussen 2 Hz cirkels (deze cirkels lopen per 0.1 Hz van 0.1-0.2 Hz of 0.1-0.3 Hz of 0.1-0.4 Hz) */

            straal_binnen = (int)(scale_hz * 10 * begin_freq + 0.5);
            straal_buiten = (int)(scale_hz * 10 * eind_freq + 0.5);

            eerste_richting_grad = k * richtings_interval_grad;
            tweede_richting_grad = (k + 1) * richtings_interval_grad;


            /*
            // arc en arcn functies van postscript rekenen met andere coordinaten stelsel (vanaf de pos. x- as tegen klok richting )
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

            if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[0])
               norm_class = 0;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[1])
               norm_class = 1;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[2])
               norm_class = 2;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[3])
               norm_class = 3;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[4])
               norm_class = 4;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[5])
               norm_class = 5;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[6])
               norm_class = 6;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[7])
               norm_class = 7;
            else if (atof(DSP_spectrum[m][k]) <= max_norm_class_limit[8])
               norm_class = 8;
            else
               norm_class = 9;



/**************  test ************/
/*
            if (m == 48)
            {
               sprintf(buffer, "%d", straal_binnen);
               fprintf(stderr, "%s", buffer);
               getchar();

               sprintf(buffer, "%d", lengte_y_as_polair);
               fprintf(stderr, "%s", buffer);
               getchar();
            }
*/
/**************** test ************/


            /* alleen plotten binnen het polaire plaatje */
            if (straal_binnen < (lengte_x_as_polair / 2))
            {
               /* newpath dsp freq. bandje */
               strcpy(line, "\0");
               strcpy(line, "newpath         %dsp freq bandje\n");
               fprintf(os_ps, "%s", line);

               /* color dsp freq. bandje */
               strcpy(line, "\0");
               strcpy(line, Set_Normalized_Energy_Class_Color(norm_class));
               strcat(line, " setrgbcolor %dsp freq bandje\n");
               fprintf(os_ps, "%s", line);

               /* moveto dsp freq bandje (xe4,ye4) */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", xe4);
               sprintf(buffer2, "%d", ye4);
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " moveto   %dsp freq bandje (xe4,ye4)\n");
               fprintf(os_ps, "%s", line);

               /* arc dsp freq bandje (xe2,ye2) */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               buffer3[0] = '\0';
               buffer4[0] = '\0';
               buffer5[0] = '\0';
               sprintf(buffer1, "%d", xe0);
               sprintf(buffer2, "%d", ye0);
               sprintf(buffer3, "%d", straal_buiten);                             /* straal van de cirkel */
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
               strcat(line, " arc   %dsp freq bandje (xe2, ye2)\n");
               fprintf(os_ps, "%s", line);

               /* lineto dsp freq bandje (xe1,ye1) */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", xe1);
               sprintf(buffer2, "%d", ye1);
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %dsp freq bandje (xe1,ye1)\n");
               fprintf(os_ps, "%s", line);

               /* arc dsp freq bandje (xe3,ye3) */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               buffer3[0] = '\0';
               buffer4[0] = '\0';
               buffer5[0] = '\0';
               sprintf(buffer1, "%d", xe0);
               sprintf(buffer2, "%d", ye0);
               sprintf(buffer3, "%d", straal_binnen);                             /* straal van de cirkel */
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
               strcat(line, " arcn   %dsp freq bandje (xe3, ye3)\n");
               fprintf(os_ps, "%s", line);

               /* closepath (dsp freq bandje) */
               strcpy(line, "\0");
               strcpy(line, "closepath         %dsp freq bandje\n");
               fprintf(os_ps, "%s", line);

               /* fill (dsp freq bandje) */
               strcpy(line, "\0");
               strcpy(line, "fill           %dsp freq bandje\n");
               fprintf(os_ps, "%s", line);

               /* lege regel */
               strcpy(line, "\n");
               fprintf(os_ps, "%s", line);
               
            } /* if (straal_binnen < lengte_x_as_polair) */
         } /* for (k = 0; k < NUM_DSP_RICHTINGEN; k++) */
      } /* for (m = 0; m < NUM_DSP_FREQUENTIES; m++) */

      fclose(temp);



      /*
      //
      // CIC LDS(LFR) summary blok naast elke grafiek samenstellen
      //
      // b.v. Hm0 = 2.4m
      //      Tm0-1 = 5.9s
      //      dd = 216
      //      ff = 12 kn
      //      LFE = 10cm2
      //      dir-LFE= 234
      //      dir-tot = 230
      //      Hmax = 3.4m
      //      T(Hmax) = 6.7s
      //      h(1/50) = 3.1m
      //      aantal golven = 312
      //
      */

      Bepaal_DSP_Polair_Summary_Per_Grafiek(grafiek_orientatie, hm0_summary, tm0_1_summary, lfe_summary,
                                            ff_summary, dd_summary, dir_tot_summary, dir_lfe_summary,
                                            hmax_summary, thmax_summary, h_1_50_summary, aantal_golven_summary, fp_summary);


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
      strcpy(line, R_G_B_color_dsp_summary_polair);
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
      // dd
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
      strcat(line, dd_summary);
      strcat(line, "");
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /*
      // ff
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
      strcat(line, "");
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /*
      // fp
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
      strcat(line, fp_summary);
      strcat(line, "");
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
      sprintf(buffer2, "%d", ys1 - (5 * of_y_afstand_summary_lines));
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
      // dir LFE
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


      /*
      // dir tot
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (7 * of_y_afstand_summary_lines));
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
      // Hmax
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (8 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, hmax_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /*
      // T(Hmax)
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (9 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, thmax_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /*
      // H(1/50)
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (10 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, h_1_50_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /*
      // aantal golven
      */
      /* moveto (summary) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", xs1);
      sprintf(buffer2, "%d", ys1 - (11 * of_y_afstand_summary_lines));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, aantal_golven_summary);
      strcat(line, ")");
      strcat(line, " show      %summary\n");
      fprintf(os_ps, "%s", line);


      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);


   } /* if ((temp = fopen(tempfilenaam, "r")) != NULL) */
   else /* file niet te openen */
   {
      /*
      /////////// title (NIET aanwezige data) ///////////
      */

      /* dan toch een title neer zetten, da data komt dan niet uit de file, maar van de gezochte, niet aanwezige file naam */
      /* bv tempfilenaam: ../temp_cic/CIC_DSP_EPL_2001081711.TMP */
      /*                     ^--- eerste char van tempfilenaam (dus 25 char totaal) */

      /* blanco deel */
      strcpy(title, "                     ");

      /* station */
      strncpy(station, tempfilenaam + (strlen(tempfilenaam) - 18), 3);
      station[3] = '\0';
      strcat(title, station);
      strcat(title, " ");

      /* DD */
      strncpy(DD, tempfilenaam + (strlen(tempfilenaam) - 8), 2);
      DD[2] = '\0';
      strcat(title, DD);
      strcat(title, "-");

      /* MM */
      strncpy(MM, tempfilenaam + (strlen(tempfilenaam) - 10), 2);
      MM[2] = '\0';
      strcat(title, MM);
      strcat(title, "-");

      /* JJJJ */
      strncpy(JJJJ, tempfilenaam + (strlen(tempfilenaam) - 14), 4);
      JJJJ[4] = '\0';
      strcat(title, JJJJ);
      strcat(title, " ");

      /* UU */
      strncpy(UU, tempfilenaam + (strlen(tempfilenaam) - 6), 2);
      UU[2] = '\0';
      strcat(title, UU);
      strcat(title, ".00 UTC");



      /*
      // naar ps outputfile (niet aanwezige data)
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
      sprintf(buffer1, "%d", x1p);
      sprintf(buffer2, "%d", y2p + of_title_dsp);
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
      //
      // CIC LDS(LFR) summary blok naast elke grafiek samenstellen
      //
      // b.v. Hm0 = 2.4m
      //      Tm0-1 = 5.9s
      //      dd = 216
      //      ff = 12 kn
      //      LFE = 10cm2
      //      dir-LFE= 234
      //      dir-tot = 230
      //      Hmax = 3.4m
      //      T(Hmax) = 6.7s
      //      h(1/50) = 3.1m
      //      aantal golven = 312
      //
      */

      Bepaal_DSP_Polair_Summary_Per_Grafiek(grafiek_orientatie, hm0_summary, tm0_1_summary, lfe_summary,
                                            ff_summary, dd_summary, dir_tot_summary, dir_lfe_summary,
                                            hmax_summary, thmax_summary, h_1_50_summary, aantal_golven_summary, fp_summary);

      if ( (strncmp(hm0_summary, "\0", 1) != 0) && (strncmp(hm0_summary, "Hm0 = -", 7) != 0) )
      {
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
         strcpy(line, R_G_B_color_dsp_summary_polair);
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
         // dd
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
         strcat(line, dd_summary);
         strcat(line, "");
         strcat(line, ")");
         strcat(line, " show      %summary\n");
         fprintf(os_ps, "%s", line);


         /*
         // ff
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
         strcat(line, "");
         strcat(line, ")");
         strcat(line, " show      %summary\n");
         fprintf(os_ps, "%s", line);


         /*
         // fp
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
         strcat(line, fp_summary);
         strcat(line, "");
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
         sprintf(buffer2, "%d", ys1 - (5 * of_y_afstand_summary_lines));
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
         // dir LFE
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


         /*
         // dir tot
         */
         /* moveto (summary) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", xs1);
         sprintf(buffer2, "%d", ys1 - (7 * of_y_afstand_summary_lines));
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
         // Hmax
         */
         /* moveto (summary) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", xs1);
         sprintf(buffer2, "%d", ys1 - (8 * of_y_afstand_summary_lines));
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " moveto   %summary\n");
         fprintf(os_ps, "%s", line);

         /* show (summary) */
         strcpy(line, "\0");
         strcpy(line, "(");
         strcat(line, hmax_summary);
         strcat(line, ")");
         strcat(line, " show      %summary\n");
         fprintf(os_ps, "%s", line);


         /*
         // T(Hmax)
         */
         /* moveto (summary) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", xs1);
         sprintf(buffer2, "%d", ys1 - (9 * of_y_afstand_summary_lines));
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " moveto   %summary\n");
         fprintf(os_ps, "%s", line);

         /* show (summary) */
         strcpy(line, "\0");
         strcpy(line, "(");
         strcat(line, thmax_summary);
         strcat(line, ")");
         strcat(line, " show      %summary\n");
         fprintf(os_ps, "%s", line);


         /*
         // H(1/50)
         */
         /* moveto (summary) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", xs1);
         sprintf(buffer2, "%d", ys1 - (10 * of_y_afstand_summary_lines));
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " moveto   %summary\n");
         fprintf(os_ps, "%s", line);

         /* show (summary) */
         strcpy(line, "\0");
         strcpy(line, "(");
         strcat(line, h_1_50_summary);
         strcat(line, ")");
         strcat(line, " show      %summary\n");
         fprintf(os_ps, "%s", line);


         /*
         // aantal golven
         */
         /* moveto (summary) */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", xs1);
         sprintf(buffer2, "%d", ys1 - (11 * of_y_afstand_summary_lines));
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " moveto   %summary\n");
         fprintf(os_ps, "%s", line);

         /* show (summary) */
         strcpy(line, "\0");
         strcpy(line, "(");
         strcat(line, aantal_golven_summary);
         strcat(line, ")");
         strcat(line, " show      %summary\n");
         fprintf(os_ps, "%s", line);


         /* lege regel */
         strcpy(line, "\n");
         fprintf(os_ps, "%s", line);

      } /* if ( (strncmp(hm0_summary, "\0", 1) != 0) && (strncmp(hm0_summary, "Hm0 =-", 6) != 0) ) */

   } /* else (file niet te openen) */


   return 0;
}






int Bepaal_DSP_Polair_Summary_Per_Grafiek(char* grafiek_orientatie, char* hm0_summary, char* tm0_1_summary, char* lfe_summary,
                                          char* ff_summary, char* dd_summary, char* dir_tot_summary, char* dir_lfe_summary,
                                          char* hmax_summary, char* thmax_summary, char* h_1_50_summary, char* aantal_golven_summary, char* fp_summary)
{
   char cic_lds_tempfilenaam[256];
   char line[513];
   char buffer[100];
   double num_hm0;
   double num_Tm0_1;
   double num_HE3;
   double hulp_HE3_4;
   double num_hmax;
   double num_thmax;
   double num_h_1_50;
   double num_fp;
   double num_Tp;
   int num_LFE;
   int num_ff;
   int num_dd;
   int num_dd_gem_total;
   int num_dd_gem_LFE;
   int i;                                                 /* regel teller */
   FILE* in_lds_temp;


   /* i.v.m. aanvullende gegevens (hm0 etc.) op cic dsp polaire plotjes moeten ook */
   /* de cic_lds files geopend worden. Deze files heten bijna het zelfde */
   /* als de al bepaalde cic_dsp_tempfilenaam_0 etc. Alleen "DSP" moet vervangen worden door "LDS" */

   /* b.v. cic_dsp_tempfilenaam_0 : CIC_DSP_K13_2002052712.TMP -> cic_lds_tempfilenaam : CIC_LDS_K13_2002052712.TMP */
   /* b.v. cic_dsp_tempfilenaam_1 : CIC_DSP_K13_2002052711.TMP -> cic_lds_tempfilenaam : CIC_LDS_K13_2002052711.TMP */
   /* b.v. cic_dsp_tempfilenaam_2 : CIC_DSP_K13_2002052710.TMP -> cic_lds_tempfilenaam : CIC_LDS_K13_2002052710.TMP */


   /* initialisatie */
   cic_lds_tempfilenaam[0] = '\0';

   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
   {
      strncpy(cic_lds_tempfilenaam, cic_dsp_tempfilenaam_0, 13);
      cic_lds_tempfilenaam[13] = '\0';
      strcat(cic_lds_tempfilenaam, "LDS");
      strcat(cic_lds_tempfilenaam + 16, cic_dsp_tempfilenaam_0 + 16);
   }
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
   {
      strncpy(cic_lds_tempfilenaam, cic_dsp_tempfilenaam_1, 13);
      cic_lds_tempfilenaam[13] = '\0';
      strcat(cic_lds_tempfilenaam, "LDS");
      strcat(cic_lds_tempfilenaam + 16, cic_dsp_tempfilenaam_1 + 16);
   }
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
   {
      strncpy(cic_lds_tempfilenaam, cic_dsp_tempfilenaam_2, 13);
      cic_lds_tempfilenaam[13] = '\0';                                      /* anders soms een vreemd teken ertussen */
      strcat(cic_lds_tempfilenaam, "LDS");
      strcat(cic_lds_tempfilenaam + 16, cic_dsp_tempfilenaam_2 + 16);
   }


/****************** test *******/
/*
               fprintf(stderr, "%s", cic_lds_tempfilenaam);
               getchar();
*/
/**************** test ************/

   /* initialisatie */
   hm0_summary[0]           = '\0';
   tm0_1_summary[0]         = '\0';
   lfe_summary[0]           = '\0';
   ff_summary[0]            = '\0';
   dd_summary[0]            = '\0';
   dir_tot_summary[0]       = '\0';
   dir_lfe_summary[0]       = '\0';
   hmax_summary[0]          = '\0';
   thmax_summary[0]         = '\0';
   h_1_50_summary[0]        = '\0';
   aantal_golven_summary[0] = '\0';



   if ((in_lds_temp = fopen(cic_lds_tempfilenaam, "r")) != NULL)
   {
      i = 0;                                                 /* regel teller */
      while ( (fgets(line, 512, in_lds_temp) != NULL) )      /* 512 is willekeurig groot getal */
      {

         /* uitlezen waarden voor summary regel */
         if (i == HM0_SPECTRUM)
         {
            strcpy(hm0_summary, "Hm0 = ");
            num_hm0 = atof(line) / 1000;                     /* van mm --> m */

            buffer[0] = '\0';
            sprintf(buffer, "%.1f", num_hm0);                /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */

            if (strncmp(buffer, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen */
               strcat(hm0_summary, "-");
            else
               strcat(hm0_summary, buffer);

            strcat(hm0_summary, "m  ");
         } /* if (i == HM0_SPECTRUM) */

         if (i == TM0_1_SPECTRUM)
         {
            strcpy(tm0_1_summary, "Tm0-1 = ");
            num_Tm0_1 = atof(line) / 10;                     /* van 0.1 sec --> sec (.1f doet dit automatisch correct )*/

            buffer[0] = '\0';
            sprintf(buffer, "%.1f", num_Tm0_1);

            if (strncmp(buffer, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen */
               strcat(tm0_1_summary, "-");
            else
               strcat(tm0_1_summary, buffer);

            strcat(tm0_1_summary, "s  ");
         } /* if (i == TM0_1_SPECTRUM) */

         if (i == HE3_SPECTRUM)
         {
            /* HE3 omzetten naar LFE: LFE = (HE3 / 4)**2 */
            strcpy(lfe_summary, "LFE = ");
            num_HE3 = atof(line);
            /*num_LFE = (int)(pow(num_HE3 / 4.0, 2) + 0.5);*/ /* afronden op hele waarden -> pow geeft problemen op bdawo8 */
            hulp_HE3_4 = num_HE3 / 40.0 ;                     /* was ook in mm -> cm dus ook nog eens delen door 10 */
            num_LFE = (int)(hulp_HE3_4 * hulp_HE3_4 + 0.5);   /* afronden op hele waarden */

            buffer[0] = '\0';
            sprintf(buffer, "%d", num_LFE);

            if (strncmp(buffer, "0", 1) == 0)                /* 0 betekent hier niet waargenomen */
               strcat(lfe_summary, "-");
            else
               strcat(lfe_summary, buffer);

            strcat(lfe_summary, "cm2  ");
         } /* if (i == HM0_SPECTRUM) */

         if (i == FF_WIND)
         {
            strcpy(ff_summary, "ff = ");

            num_ff = atoi(line);
            if ((num_ff == 0) || (num_ff == 99))            /* 99 betekent hier niet waargenomen; bij 0 kan het zijn niet waargenomen en ook windstil */
               strcat(ff_summary, "-");
            else
            {
               buffer[0] = '\0';
               sprintf(buffer, "%d", num_ff);
               strcat(ff_summary, buffer);
            }

            strcat(ff_summary, "kn  ");
         } /* if (i == FF_WIND) */

         if (i == DD_WIND)
         {
            strcpy(dd_summary, "dd = ");

            num_dd = atoi(line) * 10;                              /* was in tientallen graden */
            if ((num_dd <= 0) || (num_dd > 360))                   /* 0 betekent hier niet waargenomen */
               strcat(dd_summary, "-");
            else
            {
               buffer[0] = '\0';
               sprintf(buffer, "%03d", num_dd);
               strcat(dd_summary, buffer);
            }

            strcat(dd_summary, " ");
         } /* if (i == DD_WIND) */

         if (i == DD_GEM_SPECTRUM)
         {
            strcpy(dir_tot_summary, "dir.tot = ");

            num_dd_gem_total = atoi(line);
            if ((num_dd_gem_total > 0) && (num_dd_gem_total <= 360)) /* 0 betekent hier niet waargenomen */
            {
               buffer[0] = '\0';
               sprintf(buffer, "%03d", num_dd_gem_total);           /* altijd 3 cijfers */
               strcat(dir_tot_summary, buffer);
            }
            else
               strcat(dir_tot_summary, "-");

            /* NB strcat(summary, "°  "); KOMT NIET OVER */
            strcat(dir_tot_summary, "  ");
         } /* if (i == DD_GEM_SPECTRUM) */

         if (i == DD_GEM_LFE_SPECTRUM)
         {
            strcat(dir_lfe_summary, "dir.LFE = ");
            num_dd_gem_LFE = atoi(line);

            if ((num_dd_gem_LFE > 0) && (num_dd_gem_LFE <= 360))      /* 0 betekent hier niet waargenomen */
            {
               buffer[0] = '\0';
               sprintf(buffer, "%03d", num_dd_gem_LFE);              /* altijd 3 cijfers */
               strcat(dir_lfe_summary, buffer);
            }
            else
               strcat(dir_lfe_summary, "-");

            strcat(dir_lfe_summary, " ");
         } /* if (i == DD_GEM_LFE_SPECTRUM) */

         if (i == HMAX_SPECTRUM)
         {
            strcat(hmax_summary, "Hmax = ");
            num_hmax = atof(line) / 1000;                     /* van mm --> m */

            buffer[0] = '\0';
            sprintf(buffer, "%.1f", num_hmax);                /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */

            if (strncmp(buffer, "0.0", 3) == 0)               /* 0 betekent hier niet waargenomen ? */
               strcat(hmax_summary, "-");
            else
               strcat(hmax_summary, buffer);

            strcat(hmax_summary, "m  ");
         } /* if (i == HMAX_SPECTRUM) */

         if (i == T_HMAX_SPECTRUM)
         {
            strcpy(thmax_summary, "T(Hmax) = ");
            num_thmax = atof(line) / 10;                     /* van 0.1 sec --> sec (.1f doet dit automatisch correct )*/

            buffer[0] = '\0';
            sprintf(buffer, "%.1f", num_thmax);

            if (strncmp(buffer, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen ? */
               strcat(thmax_summary, "-");
            else
               strcat(thmax_summary, buffer);

            strcat(thmax_summary, "s  ");
         } /* if (i == T_HMAX_SPECTRUM) */


         if (i == H_1_50_SPECTRUM)
         {
            strcat(h_1_50_summary, "H(1/50) = ");
            num_h_1_50 = atof(line) / 1000;                  /* van mm --> m */

            buffer[0] = '\0';
            sprintf(buffer, "%.1f", num_h_1_50);             /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */

            if (strncmp(buffer, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen ? */
               strcat(h_1_50_summary, "-");
            else
               strcat(h_1_50_summary, buffer);

            strcat(h_1_50_summary, "m  ");

         } /* if (i == H_1_50_SPECTRUM) */


         if (i == AANTAL_GOLVEN_SPECTRUM)
         {
            strcat(aantal_golven_summary, "N(waves) = ");
            strcat(aantal_golven_summary, line);
            strcat(aantal_golven_summary, " (20min)");
         } /* if (i == AANTAL_GOLVEN_SPECTRUM) */


         if (i == FP_SPECTRUM)
         {
            strcpy(fp_summary, "fp = ");
            num_fp = atof(line) / 1000;                        /* van 1 mHz --> Hz (.3f doet dit automatisch correct )*/

            buffer[0] = '\0';
            sprintf(buffer, "%.2f", num_fp);

            if (strncmp(buffer, "0.00", 4) == 0)              /* 0 betekent hier niet waargenomen ? */
               strcat(fp_summary, "-");
            else
               strcat(fp_summary, buffer);

            strcat(fp_summary, "Hz");

            /* ook de Tp (piek periode) er tussen haakjes bij zetten */
            strcat(fp_summary, " (Tp = ");

            num_Tp = 1.0 / num_fp;

            buffer[0] = '\0';
            sprintf(buffer, "%.1f", num_Tp);

            if (strncmp(buffer, "0.0", 3) == 0)                /* 0 betekent hier niet waargenomen ? */
               strcat(fp_summary, "-");
            else
               strcat(fp_summary, buffer);

            strcat(fp_summary, "s)");

         } /* if (i == FP_SPECTRUM) */


         /* volgende regel */
         i++;                                                  /* regel teller */

      } /* while ( (fgets(line1, 512, in_lds_temp) != NULL) ) */

      fclose(in_lds_temp);

   } /* if ((in_lds_temp = fopen(cic_lds_tempfilenaam, "r")) != NULL) */
   else /* cic lds file niet te openen */
   {

/****************** test *******/
/*
               fprintf(stderr, "%s\n", cic_lds_tempfilenaam);
               getchar();
*/
/**************** test ************/


      /* hm0 */
      strcpy(hm0_summary, "Hm0 = ");
      strcat(hm0_summary, "-");

      /* tm0-1 */
      strcpy(tm0_1_summary, "Tm0-1 = ");
      strcat(tm0_1_summary, "-");

      /* lfe */
      strcpy(lfe_summary, "LFE = ");
      strcat(lfe_summary, "-");

      /* ff */
      strcpy(ff_summary, "ff = ");
      strcat(ff_summary, "-");

      /* dd */
      strcpy(dd_summary, "dd = ");
      strcat(dd_summary, "-");

      /* dir total */
      strcpy(dir_tot_summary, "dir.tot = ");
      strcat(dir_tot_summary, "-");

      /* dir lfe */
      strcpy(dir_lfe_summary, "dir.LFE = ");
      strcat(dir_lfe_summary, "-");

   } /* else (cic lds file niet te openen) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*             Bepaal_Filenamen_DSP_Temp_Files_Per_Locatie                   */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenamen_DSP_Temp_Files_Per_Locatie(char* station, int cic_uren_interval)
{
   /* laatste 3 files (van de uurlijkse files) worden geplot dus dus van deze */
   /* laatse 3 de hoogste waarde bepalen i.v.m. latere schaalverdeling */

   /* datum stempel van de laatse file is:  JJJJMMDDUU var. */


   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   int uren_terug;
   int tijd_ok;
   struct tm time_struct;                    /* datum-tijd manipulatie */
   char date_time[100];



   /* initialisatie */
   strcpy(cic_dsp_tempfilenaam_0, "\0");
   strcpy(cic_dsp_tempfilenaam_1, "\0");
   strcpy(cic_dsp_tempfilenaam_2, "\0");


   /* eerste deel tempfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
   {
      strcpy(cic_dsp_tempfilenaam_0, "temp_cic\\");
      strcpy(cic_dsp_tempfilenaam_1, "temp_cic\\");
      strcpy(cic_dsp_tempfilenaam_2, "temp_cic\\");
   }
   else                                                /* UNIX */
   {
      strcpy(cic_dsp_tempfilenaam_0, "temp_cic/");
      strcpy(cic_dsp_tempfilenaam_1, "temp_cic/");
      strcpy(cic_dsp_tempfilenaam_2, "temp_cic/");
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */




   /* hulp voor file namen van 1 en 2 uur terug bepalen (t.o.v van 0 uur terug = JJJJJMMDDUU) */

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
   /////// file naam bepalen van de files van 0 uur terug /////
   */

   /* file naam temp file van 0 uur terug */
   strcat(cic_dsp_tempfilenaam_0, "CIC_DSP_");
   strcat(cic_dsp_tempfilenaam_0, station);
   strcat(cic_dsp_tempfilenaam_0, "_");
   strcat(cic_dsp_tempfilenaam_0, JJJJMMDDUU);
   strcat(cic_dsp_tempfilenaam_0, ".TMP");


   /*
   /////// file naam bepalen van de files van 1 uur terug /////
   */
   uren_terug = 1 * cic_uren_interval;             /* in CIC mode: 1 * 1 = 1; ??in TSA mode 1 * 3 = 3??komt tsa hier wel? */

   time_struct.tm_year  = num_JJJJ - 1900;
   time_struct.tm_mon   = num_MM - 1;
   time_struct.tm_mday  = num_DD;
   time_struct.tm_hour  = num_UU - uren_terug;
   time_struct.tm_min   = 0;
   time_struct.tm_sec   = 0;
   time_struct.tm_isdst = -1;                      /* daylight saving time: -1 = no info available */

   tijd_ok = 1;
   if (mktime(&time_struct) == -1)
      tijd_ok = 0;

   if (tijd_ok == 1)
   {
      (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121000 */

      strcat(cic_dsp_tempfilenaam_1, "CIC_DSP_");
      strcat(cic_dsp_tempfilenaam_1, station);
      strcat(cic_dsp_tempfilenaam_1, "_");
      strcat(cic_dsp_tempfilenaam_1, date_time);
      strcat(cic_dsp_tempfilenaam_1, ".TMP");
   } /* if (tijd_ok == 1) */


   /*
   /////// file naam bepalen van de files van 2 uur terug /////
   */
   uren_terug = 2 * cic_uren_interval;            /* in CIC mode: 2 * 1 = 1; in TSA mode 2 * 3 = 6 */

   time_struct.tm_year  = num_JJJJ - 1900;
   time_struct.tm_mon   = num_MM - 1;
   time_struct.tm_mday  = num_DD;
   time_struct.tm_hour  = num_UU - uren_terug;
   time_struct.tm_min   = 0;
   time_struct.tm_sec   = 0;
   time_struct.tm_isdst = -1;                      /* daylight saving time: -1 = no info available */

   tijd_ok = 1;
   if (mktime(&time_struct) == -1)
      tijd_ok = 0;

   if (tijd_ok == 1)
   {
      (void)strftime(date_time, 100, "%Y%m%d%H", &time_struct);     /* b.v. 2001121000 */

      strcat(cic_dsp_tempfilenaam_2, "CIC_DSP_");
      strcat(cic_dsp_tempfilenaam_2, station);
      strcat(cic_dsp_tempfilenaam_2, "_");
      strcat(cic_dsp_tempfilenaam_2, date_time);
      strcat(cic_dsp_tempfilenaam_2, ".TMP");
   } /* if (tijd_ok == 1) */


   return 0;
}


/*/////////////////////////////////////////////// test ////////////////////////////////////////////////*/
int Bepaal_Max_Scale_Hz_DSP(const double max_energy, double *max_polair_hz)
{
   /*char buffer[100];*/
   /* loopt gedeeltelijk (m2/Hz versus m2/Hz/rad) parallel met function: Bepaal_Scale_Y_X_Max(int max_energy, int *scale_y_max, int *scale_x_max) [file: gps+ps.c] */
   /* verder staan de eenheden in de (CIC)DSP files anders dan de (CIC)LDS files

   /*
   //
   // 0    -  5.0 m2/Hz/rad  (0       -  500000 .1cm2s/rad)  --> max_polair_hz 0.4 Hz)
   // 5.0  - 50.0 m2/Hz/rad  (500000  - 5000000 .1cm2s/rad)  --> max_polair_hz 0.3 Hz)
   //      > 50.0 m2/Hz/rad  (        > 5000000 .1cm2s/rad)  --> max_polair_hz 0.2 Hz)
   */


   if (max_energy < 5.0)
      *max_polair_hz = 0.4;
   else if (max_energy < 50.0)
      *max_polair_hz = 0.3;
   else                     /* dus dan max_energy extreem groot */
      *max_polair_hz = 0.2;


/**************  test ************/
/*
                 buffer[0] = '\0';
                 sprintf(buffer, "%lf", max_energy);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************** test ************/








#if 0
   /* controleren of de max_energy wel op een frequentie (freq_max_energy) zit die binnen de nu bepaalde max_scale_hz valt */
   /* scale_hz = aantal pixels tussen twee opeenvolgende frequentie cirkels */
   //if (max_energy_freq < max_scale_hz)


   //   *scale_hz = lengte_y_as_polair / 2.0 / ((double)max_polair_hz * 10);      /* bijv 200 / 2 / (0.4 * 10) = 25 */


   //else  /* default gebruiken (t/m 0.4 Hz) */
   //   *scale_hz = lengte_y_as_polair / 2.0 / (double)AANTAL_CIRKELS_HZ;        /* bijv 200 / 2 / 4 = 25 */


   //scale_hz = lengte_y_as_polair / 2.0 / (double)AANTAL_CIRKELS_HZ;      /* bijv 200 / 2 / 4 = 25 */
#endif

   return 0;
}
/*///////////////////////////////////////////////// test /////////////////////////////////////////////////////*/


