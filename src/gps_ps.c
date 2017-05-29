#if !defined(gps_h)                                  /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>                                  /* MAXINT */
#include <time.h>

#if defined(WINDOWS)                                 /* zie gps.h */
#include <dir.h>                                     /* o.a. getcwd() in windows */
#else
#include <unistd.h>                                  /* o.a. getcwd() in UNIX */
#endif



/* globale var's in andere module */
extern char OS[8];                                   /* gpsmain.c; operating system */
extern char JJJJMMDDUU[11];                          /* gpsmain.c;via argument list */
extern char spectrum[NUM_SPECTRUM][NUM_SPECTRUM_2];  /* gps_input.c; CIC spectrum */
extern char stations[NUM_STATIONS][NUM_STATIONS_2];  /* gpsmain.c; NUM_STATIONS arrayplaatsen van elk 4 char */
extern char MODE[4];                                 /* gpsmain.c; b.v. CIC of TSA; uit: gpsmain.c */
extern char tempfilenaam_0[256];                     /* gpsmain.c;0 uur terug */
extern char tempfilenaam_1[256];                     /* gpsmain.c;1 uur (of 3 uur in TSA mode) terug */
extern char tempfilenaam_2[256];                     /* gpsmain.c;2 uur (of 6 uur in TSA mode) terug */
extern int x1;                                       /* gpsmain.c */
extern int x2;                                       /* gpsmain.c */
extern int x3;                                       /* gpsmain.c */
extern int x4;                                       /* gpsmain.c */
extern int _y1;                                      /* gpsmain.c; Op de bdawo8 geeft y1 een foutmelding i.v.m. previous declaration in math.h */
extern int y2;                                       /* gpsmain.c */
extern char font_keuze[50];                          /* gpsmain.c;om te kunnen kiezen b.v. onder UNIX een bold font gebruiken */
extern FILE* os_ps;                                  /* gpsmain.c */

/* function prototypes in andere module */
extern int Write_Log(char* bericht);
extern int Initialiseer_Spectrum_Array(void);
extern double Bepaal_Diepte(const char* station);

/* function prototypes voor deze module */
int Write_Postscript_Files(void);
int Write_Postscript_General(void);
int Write_Postscript_End(void);
int Write_Log(char* bericht);
int Write_Postscript_Raster_Box(char* grafiek_orientatie, const int scale_y_max, const int scale_x_max, const int pag_no);
int Bepaal_Filenamen_Temp_Files_Per_Locatie(char* station, int cic_uren_interval);
int Bepaal_Outputfilenaam_Per_Locatie(char* station);
int Bepaal_Max_Energy_Temp_Files(int* max_energy);
int Bepaal_Scale_Y_X_Max(int max_energy, int *scale_y_max, int *scale_x_max);
int Temp_Files_To_Postscript(char* grafiek_orientatie, const int scale_x_max, const int scale_y_max, int cic_uren_interval);

/* variablen voor deze module */
char cic_outputfilenaam[256];
int of3;                              /* afstand tussen marks op y-as van Energie grafiek */
int of3b;                             /* afstand tussen marks op y-as van richtingen grafiek */
int of4;                              /* afstand tussen marks op x-as van Energie en richting grafiek */
float y_factor_energy;
int y_factor_richting;
int x_factor;
/*char font_keuze[50]; */                 /* om te kunnen kiezen b.v. onder UNIX een bold font gebruiken */
int hor_afstand;                      /* of10, of10b of of10c (hor. afstand tot y-as (hier begint bijv. 0,6 van schaalaanduiding) hier begint ook summary regel */






/*****************************************************************************/
/*                                                                           */
/*                         Write_Postscript_Files                            */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_Files()
{
   char info[512];
   char volledig_path[512];
   char grafiek_orientatie[256];
   int i;
   int max_energy;
   int scale_y_max;
   int scale_x_max;
   int cic_uren_interval;                                   /* in CIC mode om de 1uur metingen plotten (in TSA mode om de 3 uur) */


   /* initialisatie */
   cic_uren_interval = 1;                                   /* metingen om de 1 uur gebruiken */


   /* inlezen van de locaties waarvoor wat aangemaakt moet worden */
   /*Inlezen_Locaties_File(); */                                /* array: "stations[NUM_STATIONS][NUM_STATIONS_2]" wordt gevuld */

   /* per locatie postscript files aanmaken */
   for (i = 0; i < NUM_STATIONS; i++)
   {
      if (strcmp(stations[i], "\0") != 0)                               /* niet leeg */
      {
         /* 3 temp files per locatie per ps-pagina (JJJJMMDDUU, JJJJMMDDUU - 1 uur en JJJMMDDUU - 2 uur) uit temp directory inlezen */
         Bepaal_Filenamen_Temp_Files_Per_Locatie(stations[i], cic_uren_interval);          /* tempfilenaam_0 , tempfilenaam_1, tempfilenaam_2 worden bepaald */

         /* van de net bepaalde temp files de max energy waarde er uit halen */
         Bepaal_Max_Energy_Temp_Files(&max_energy);                     /* hoogste waarde van de 3 files (1 waarde voor de 3 files) */

         /* max waarde y-as */
         Bepaal_Scale_Y_X_Max(max_energy, &scale_y_max, &scale_x_max);

         /* 1 output postscript file voor 3 temp files */
         Bepaal_Outputfilenaam_Per_Locatie(stations[i]);


         if ((os_ps = fopen(cic_outputfilenaam, "w")) == NULL)              /* dus mislukt */
         {
            getcwd(volledig_path, 512);

            /* bericht samen stellen */
            strcpy(info, "\0");
            strcat(info, volledig_path);

            if (strcmp(OS, "WINDOWS") == 0)
               strcat(info, "\\");
            else
               strcat(info, "/");

            strcat(info, cic_outputfilenaam);
            strcat(info, " niet te schrijven\n");

            /* naar log schrijven */
            Write_Log(info);

         } /* if ((os_ps = fopen(outputfilenaam, "w")) == NULL) */
         else /* outputfile is te schrijven */
         {
            fclose(os_ps);    /* file is nu weer leeg */

            os_ps = fopen(cic_outputfilenaam, "a");

            /* ps pagina correct beginnen */
            Write_Postscript_General();

            /* onderste grafiek (I.V.M. 'TRANSLATE' ONDERSTE GRAFIEK ALS EERSTE DOEN) */
            strcpy(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK);
            Write_Postscript_Raster_Box(grafiek_orientatie, scale_y_max, scale_x_max, MAXINT);
            Temp_Files_To_Postscript(grafiek_orientatie, scale_x_max, scale_y_max, cic_uren_interval);

            /* middelste grafiek (I.V.M. 'TRANSLATE' MIDDELSE GRAFIEK ALS TWEEDE DOEN)*/
            strcpy(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK);
            Write_Postscript_Raster_Box(grafiek_orientatie, scale_y_max, scale_x_max, MAXINT);
            Temp_Files_To_Postscript(grafiek_orientatie, scale_x_max, scale_y_max, cic_uren_interval);

            /* bovenste grafiek (I.V.M. 'TRANSLATE' MIDDELSE GRAFIEK ALS LAATSTE DOEN)*/
            strcpy(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK);
            Write_Postscript_Raster_Box(grafiek_orientatie, scale_y_max, scale_x_max, MAXINT);
            Temp_Files_To_Postscript(grafiek_orientatie, scale_x_max, scale_y_max, cic_uren_interval);

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
/*                        Write_Postscript_General                           */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_General()
{
   char line[256];
   const char knmi_logo[]               = {"(c)knmi"};

   /*
   // NB alle pagina maten hier gebruikt zijn afgestemd op US letter type !!! (dit is anders dan A4)
   // (het kan daarom nodig zijn de flag "-page letter" mee te geven aan convert)
   */



   /* fonts kunnen (afh. van gebruikte ghostscript versie) per os verschillend overkomen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(font_keuze, "/Courier findfont  %font\n");             /* t/m 24-10-2002: strcpy(font_keuze, "\/Courier findfont  %font\n");*/
   else                                                /* UNIX */
      /*strcpy(font_keuze, "\/Courier-Bold findfont  %font\n");*/  /* omdat via UNIX (BDAWO8) alles wat dunner overkomt (bij ghostscript versie 3.01)*/
      strcpy(font_keuze, "/Courier findfont  %font\n");             /* t/m 24-10-2002: strcpy(font_keuze, "\/Courier findfont  %font\n");*/


   /*
   //////// general (deze info komt in elke postsript file; font e.d.) ///////
   */

   /* nodig voor het eventueel ook kunnen printen */
   strcpy(line, "\0");
   strcpy(line, "%!PS-Adobe -2.0    %herkenning voor een ps printer\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);

   /* save current graphic state */
   strcpy(line, "\0");
   strcpy(line, "gsave              %save graphic state\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   //////// font ///////////
   */

   /* font */
   strcpy(line, "\0");
   strcpy(line, font_keuze);
   fprintf(os_ps, "%s", line);

   /* font */
   strcpy(line, "\0");
   strcpy(line, "10 scalefont       %font\n");
   fprintf(os_ps, "%s", line);

   /* font */
   strcpy(line, "\0");
   strcpy(line, "setfont            %font\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   /////////// pagina kleur ///////////
   */
   strcpy(line, "\0");
   strcpy(line, "newpath        %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* color (pagina pagina) */
   strcpy(line, "\0");
   /*strcpy(line, strcat(R_G_B_color_pagina, " setrgbcolor %pagina kleur\n"));*/
   strcpy(line, R_G_B_color_pagina_ZEE);
   strcat(line, " setrgbcolor %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* moveto (pagina color) */
   strcpy(line, "\0");
   strcpy(line, "0 1 moveto     %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* lineto (pagina color) */
   strcpy(line, "\0");
   strcpy(line, "612 1 lineto   %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* lineto (pagina color) */
   strcpy(line, "\0");
   strcpy(line, "612 792 lineto %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* lineto (pagina color) */
   strcpy(line, "\0");
   strcpy(line, "0 792 lineto   %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* closepath (pagina color) */
   strcpy(line, "\0");
   strcpy(line, "closepath      %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* fill (pagina color) */
   strcpy(line, "\0");
   strcpy(line, "fill           %pagina kleur\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   /////////// pagina kader ///////////
   */
   strcpy(line, "\0");
   strcpy(line, "newpath        %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* color (pagina kader) */
   strcpy(line, "\0");
   /*strcpy(line, strcat(R_G_B_color_pagina_kader, " setrgbcolor %pagina kader\n"));*/
   strcpy(line, R_G_B_color_pagina_kader_ZEE);
   strcat(line, " setrgbcolor %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* moveto (pagina kader) */
   strcpy(line, "\0");
   strcpy(line, "0 1 moveto     %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (pagina kader) */
   strcpy(line, "\0");
   strcpy(line, "611 1 lineto   %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (pagina kader) */
   strcpy(line, "\0");
   strcpy(line, "611 792 lineto %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (pagina kader) */
   strcpy(line, "\0");
   strcpy(line, "0 792 lineto   %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* closepath (pagina kader) */
   strcpy(line, "\0");
   strcpy(line, "closepath      %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* stroke (pagina kader) */
   strcpy(line, "\0");
   strcpy(line, "stroke         %pagina kader\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   /////////// deelijnen ////////////
   */
   /* moveto */
   strcpy(line, "\0");
   strcpy(line, "0 264 moveto   %deellijn 1\n");
   fprintf(os_ps, "%s", line);

   /* lineto */
   strcpy(line, "\0");
   strcpy(line, "611 264 lineto %deellijn 1\n");
   fprintf(os_ps, "%s", line);

   /* stroke */
   strcpy(line, "\0");
   strcpy(line, "stroke         %deellijn 1\n");
   fprintf(os_ps, "%s", line);

   /* moveto */
   strcpy(line, "\0");
   strcpy(line, "0 528 moveto   %deellijn 2\n");
   fprintf(os_ps, "%s", line);

   /* lineto */
   strcpy(line, "\0");
   strcpy(line, "611 528 lineto %deellijn 2\n");
   fprintf(os_ps, "%s", line);

   /* stroke */
   strcpy(line, "\0");
   strcpy(line, "stroke         %deellijn 2\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   ///////// knmi naam/logo //////////
   */

   /* moveto (knmi-logo onderste grafiek) */
   strcpy(line, "\0");
   /*strcpy(line, "565 5 moveto   %knmi logo\n");*/        /* rechts onderin het betreffende vak */
   strcpy(line, "565 251 moveto   %knmi logo\n");          /* rechts bovenin het betreffende vak */
   fprintf(os_ps, "%s", line);

   /* show (knmi-logo onderste grafiek) */
   strcpy(line, "\0");
   strcpy(line, "(");
   strcat(line, knmi_logo);
   strcat(line, ") show     %knmi-logo\n");
   fprintf(os_ps, "%s", line);

   /* moveto (knmi-logo middelste grafiek) */
   strcpy(line, "\0");
   /*strcpy(line, "565 269 moveto   %knmi logo\n");*/
   strcpy(line, "565 515 moveto   %knmi logo\n");
   fprintf(os_ps, "%s", line);

   /* show (knmi-logo middelste grafiek) */
   strcpy(line, "\0");
   strcpy(line, "(");
   strcat(line, knmi_logo);
   strcat(line, ") show     %knmi-logo\n");
   fprintf(os_ps, "%s", line);

   /* moveto (knmi-logo bovenste grafiek) */
   strcpy(line, "\0");
   /*strcpy(line, "565 533 moveto   %knmi logo\n");*/
   strcpy(line, "565 779 moveto   %knmi logo\n");
   fprintf(os_ps, "%s", line);

   /* show (knmi-logo bovenste grafiek) */
   strcpy(line, "\0");
   strcpy(line, "(");
   strcat(line, knmi_logo);
   strcat(line, ") show     %knmi-logo\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                     Write_Postscript_End                                  */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_End()
{
   char line[256];



   /* restore the saved graphic state */
   strcpy(line, "\0");
   strcpy(line, "grestore       %restore graphic state\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);

   /* showpage */
   strcpy(line, "\0");
   strcpy(line, "showpage       %toon blz\n");
   fprintf(os_ps, "%s", line);


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                    Write_Postscript_Raster_Box                            */
/*                                                                           */
/*****************************************************************************/
int Write_Postscript_Raster_Box(char* grafiek_orientatie, const int scale_y_max, const int scale_x_max, const int pag_no)
{
   /* NB in CIC mode wordt inkomende argument pag_no niet gebruikt ! */



   /*
   //  (x1,y2)         (x2,y2)           (x3,y2)           (x4,y2)
   //       *-----------*                      *-----------*
   //       |           |                      |           |
   //       |           |                      |           |
   //       *-----------*                      *-----------*
   //  (x1,y1)         (x2,y1)           (x3,y1)           (x4,y1)
   //
   */

   int i;
   char line[256];
   char buffer1[10];                                /* opslag tussen waarden */
   char buffer2[10];                                /* opslag tussen waarden */

   /*char R_G_B_color_x_y_box[256];      */
   /*char R_G_B_color_x_y_box_kader[256];*/             /* + marks + tekst */
   /*char R_G_B_color_model[256];        */
   /*char R_G_B_color_obs[256];          */             /* - (niet de verdere text) */

   const int lengte_y_as = 160;
   const int lengte_x_as = 200;
   const int mark_lengte = 6;
   const int of5         = 16;                      /* Hz (0.1 0.2 etc.) offset tot x-as om Hz precies onder mark te krijgen */
   const int of6         = 8;                       /* Hz (0.1 0.2 etc.) offset rel. hor. afstand t.o.v. marks */
   const int of10        = 20;                      /* hor. ofset unit (m2/Hz) aanduiding tot y-as bij energie grafiek */
   const int of10b       = 30;                      /* hor. ofset unit (m2/Hz) aanduiding tot y-as bij energie grafiek */
   const int of10c       = 40;                      /* hor. ofset unit (m2/Hz) aanduiding tot y-as bij energie grafiek */
   const int of11        = 3;                       /* vert. ofset unit (m2/Hz) aanduiding y-as bij energie grafiek */
   const int of14        = 12;                      /* vert. ofset legenda kader t.o.v. y2 */
   int of15;                                        /* hor. ofset legenda kader t.o.v. y-as (x1 of x3) */
   const int of16        = 6;                       /* vert. ofset legenda tekst model t.o.v. y2 */
   const int of18        = 5;                       /* hor. ofset legenda tekst t.o.v. x1 */
   const int of19        = 25;                      /* (sub) ofset legenda x richting (tussen - en tekst) */
   const int of20        = 3;                       /* (sub) ofset legenda y richting (tekst zakken t.o.v. -) */
   int of21;                                        /* hor. afstand t.o.v. y-as voor tweede aanduiding in legenda */
   const int of22        = 145;                     /* hor. afstand t.o.v. y-as voor derde aanduiding in legenda */
   const int of27        = 10;                      /* hor. ofset voor algemen x-as aanduiding (Hz) */
   const int of_hook     = 4;                       /* legenda (geeft mooie geschuinde hoek) */
   int of40;                                        /* vert. offset t.o.v. _y1 voor y-as aanduiding (b.v. m2/Hz) */
   int of41;                                        /* hort. offset t.o.v. x1 voor y-as aanduiding (b.v. m2/Hz) */
   const int of42        = 5;                       /* hor ofset t.o.v. x4 + mark_lengte voor rechtse schaal aanduidingen graden (richtigen grafiek) */


   /* afh. van mode */
   if ( (strncmp(MODE, "TSA", 3) == 0) && (pag_no == 0) ) /* ALLEEN in TSA mode eerste pagina een 3de legenda aanduiding !! */
      of15 = lengte_x_as;
   else if (strncmp(MODE, "TSV", 3) == 0)                 /* TSA verificatie mode (dan ook 3de aanduiding ) */
      of15 = lengte_x_as;
   else
      of15 = lengte_x_as;  /* 150; */

   if (strncmp(MODE, "CIC", 3) == 0)
      of21 = 90;
   else
      of21 = 70;


   /* afstand tussen marks bepalen */
   of3  = lengte_y_as / 10;                         /* afstand tussen marks op y-as van Energie grafiek */
   of4  = (int)((float)lengte_x_as / (float)scale_x_max + 0.5);  /* afstand tussen marks op x-as van Energie en richting grafiek */
   of3b = lengte_y_as / 8;                          /* afstand tussen marks op y-as van richtingen grafiek */

   /* y-as aanduiding offsets (bv. m2/Hz) bepalen */
   of40 = lengte_y_as / 2 - 10;                     /* vert. offset t.o.v. _y1 voor y-as aanduiding */
   of41 = 40;                                       /* vert. offset t.o.v. x1 voor y-as aanduiding */


   /* kleuren bepalen */
   /*strcpy(R_G_B_color_x_y_box, R_G_B_color_x_y_box_ZEE);*/
   /*strcpy(R_G_B_color_x_y_box_kader, R_G_B_color_x_y_box_kader_ZEE);*/   /* + marks + tekst */
   /*strcpy(R_G_B_color_model, R_G_B_color_model_ZEE); */
   /*strcpy(R_G_B_color_obs, R_G_B_color_obs_ZEE);*/                       /* - (niet de verdere text) */


   /* dimensies 'main-boxen' bepalen */
   /*x1  = 55;                     */               /* main box links */
   /*x2  = x1 + lengte_x_as;       */               /* main box links */
   /*x3  = 361; */ /* (306 + 55) */                  /* main box rechts */
   /*x4  = x3 + lengte_x_as;      */                /* main box rechts */
   /*_y1 = 50;                    */                /* main box links + main box rechts */
   /*y2  = _y1 + lengte_y_as;     */                /* main box links + main box rechts*/





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
   /*                             x-y box links                              */
   /*                                                                        */
   /**************************************************************************/


   /*
   ///////////// color x-y box links //////////////
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
   sprintf(buffer1, "%d", x1);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1);
   sprintf(buffer2, "%d", y2);
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
   sprintf(buffer1, "%d", x1);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x2);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1);
   sprintf(buffer2, "%d", y2);
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
   /////// x-as indeling linkse box /////////
   */

   x_factor = 10;                                   /* schaal x-as om de 0.1 Hz echter waarden zijn om de 0.01 Hz */


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
   //        '0.0 0.1'               } 5
   //
   //
   */


   /* x-as schaal verdeling:
   //    van 0.0 - 0.4 per 0.1 oplopend
   // of van 0.0 - 0.3 per 0.1 oplopend
   // of van 0.0 - 0.2 per 0.1 oplopend
   */
   for (i = 0; i <= scale_x_max; i++)                   /* loop over Hz (0.0 t/m 0.5 Hz) */
   {
      /* moveto (mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 + (i * of4));
      sprintf(buffer2, "%d", _y1);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %Hz-mark\n");
      fprintf(os_ps, "%s", line);

      /* lineto (mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 + (i * of4));
      sprintf(buffer2, "%d", _y1 - mark_lengte);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %Hz-mark\n");
      fprintf(os_ps, "%s", line);

      /* stroke (mark) */
      strcpy(line, "\0");
      strcpy(line, "stroke         %Hz-mark\n");
      fprintf(os_ps, "%s", line);

      /* moveto (Hz aanduiding) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 + (i * of4) - of6);
      sprintf(buffer2, "%d", _y1 - of5);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %Hz\n");
      fprintf(os_ps, "%s", line);

      /* show (Hz aanduiding) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      sprintf(buffer1, "%0.1f", (float)i / x_factor);
      strcpy(line, "(");
      strcat(line, buffer1);
      strcat(line, ")");
      strcat(line, " show      %Hz\n");
      fprintf(os_ps, "%s", line);

   } /* for (i = 0; i <= scale_x_max; i++) */


   /* algemene Hz aanduiding x-as linkse box */

   /* moveto (algemene as-aanduiding) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + (scale_x_max * of4) + of27);
   sprintf(buffer2, "%d", _y1 - of5);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto    %x-as-eenheden aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* show (algemene as-aanduiding) */
   strcpy(line, "\0");
   strcpy(line, "( Hz) show    %x-as-eenheden aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   /////////  y - as indeling linkse box //////////
   */


   /* y_factor geeft aan waarmee de marks langs y-as moeten worden voorzien/vermenigvuldigd */
   y_factor_energy = (float)scale_y_max / 10.0;
                                          /* VOORBEELD 1 */
                                          /* scale_y_max; range: 1 t/m 9 (per 1) --> y_factor; range 0.1 - 0.9 */
                                          /* b.v scale_y_max  = 7 --> y_factor = 0.7 */
                                          /* --> schaal verdeling y-as: 0.7 - 0.14 - 0.21 - 0.28 etc. */
                                          /* -- */
                                          /* VOORBEELD 2 */
                                          /* scale_y_max; range: 10 t/m 1000 (per 10) --> y_factor; range 1 - 100 */
                                          /* b.v scale_y_max  = 30 --> y_factor = 3 */
                                          /* --> schaal verdeling y-as: 0 - 3 - 6 - 9 - 12 etc. */


   /* afh van max waarde de cijfers iets verder van y-as afzetten */
   if (scale_y_max < 10)                 /* tienden */
      hor_afstand = of10b;
   else if (scale_y_max < 100)
      hor_afstand = of10;
   else if (scale_y_max < 1000)
      hor_afstand = of10b;
   else
      hor_afstand = of10c;


   for (i = 0; i < 11; i++)            /* loop over m2/Hz (b.v. 0 t/m 30 Hz) */
   {
      /* linkse-as moveto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1);
      sprintf(buffer2, "%d", _y1 + (i * of3));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark links\n");
      fprintf(os_ps, "%s", line);

      /* linkse-as lineto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 - mark_lengte);
      sprintf(buffer2, "%d", _y1 + (i * of3));
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
      sprintf(buffer1, "%d", x1 - hor_afstand);
      sprintf(buffer2, "%d", _y1 + (i * of3) - of11);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark links\n");
      fprintf(os_ps, "%s", line);

      if (scale_y_max < 10)           /* schaal in tienden */
      {
         /* linkse-as show */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         sprintf(buffer1, "%03.1f", (float)i * y_factor_energy);
         strcpy(line, "(");
         strcat(line, buffer1);
         strcat(line, ")");
         strcat(line, " show      %par-mark links\n");
         fprintf(os_ps, "%s", line);
      } /* if (scale_y_max < 10) */
      else                           /* schaal in gehele getallen */
      {
         /* linkse-as show */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         /*sprintf(buffer1, "%03.1f", (float)i / 1.0);*/
         sprintf(buffer1, "%d", i * (int)y_factor_energy);
         strcpy(line, "(");
         strcat(line, buffer1);
         strcat(line, ")");
         strcat(line, " show      %par-mark links\n");
         fprintf(os_ps, "%s", line);
      } /* else (schaal in gehele getallen) */

   } /* for (i = 0; i < 11 i++) */


   /* moveto (y-as aanduiding) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 - of41);
   sprintf(buffer2, "%d", _y1 + of40);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto     %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* rotate (y-as aanduiding) */
   strcpy(line, "\0");
   strcat(line, "90 rotate        %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* show (y-as aanduiding) */
   strcpy(line, "\0");
   strcat(line, "(m2/Hz) show    %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* rotate (y-as aanduiding) [weer terugzetten]*/
   strcpy(line, "\0");
   strcat(line, "-90 rotate       %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);


   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   ///////////// legenda linkse box //////////////
   */


   /*
   //
   //                                                             _____ of_hook
   //                                                            |
   //                  ________________________________________|_V_|_ _ _ _ _
   //                /         ______ of19________________     \       } of_hook
   //              /          |              |            |      \  _ _} _ _ _
   //             |           V              V            V        |   }
   //             |        --- item1      --- item2    --- item3   |   } of14
   //      of16 { |________________________________________________|___}____________ y2
   //             |<--------------------- of15 ------------------->|
   //             |<-of18->|
   //             |<------- of21 -------->|
   //             |<--------------- of22 ------------->|
   //             |
   //            x1
   //
   //
   // NB item 3 alleen eerste pagina TSA mode
   */


   /* newpath (legenda) */
   strcpy(line, "\0");
   strcpy(line, "newpath            %legenda\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text ) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of15);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto     %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of15);
   sprintf(buffer2, "%d", y2 + of14 - of_hook);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of15 - of_hook);
   sprintf(buffer2, "%d", y2 + of14);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of_hook);
   sprintf(buffer2, "%d", y2 + of14);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1);
   sprintf(buffer2, "%d", y2 + of14 - of_hook);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader\n");
   fprintf(os_ps, "%s", line);

   /* stroke (legenda kader) */
   strcpy(line, "\0");
   strcpy(line, "stroke            %legenda kader\n");
   fprintf(os_ps, "%s", line);




   /*
   ///////// 1ste aanduiding in legenda ////////////
   */

   /* color (- legenda text) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_obs_ZEE);
   strcat(line, " setrgbcolor %legenda text\n");
   fprintf(os_ps, "%s", line);

   /* moveto (- legenda text) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of18);
   sprintf(buffer2, "%d", y2 + of16);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text\n");
   fprintf(os_ps, "%s", line);

   /* show (- legenda text) */
   strcpy(line, "\0");
   strcpy(line, "( __) show    %legenda text\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda text\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda text) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of18 + of19);
   sprintf(buffer2, "%d", y2 + of16 - of20);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text\n");
   fprintf(os_ps, "%s", line);

   /* show (legenda text) */
   strcpy(line, "\0");
   if ( (strncmp(MODE, "TSA", 3) == 0) || (strncmp(MODE, "TSV", 3) == 0) )
      strcat(line, "(nedwam) show    %legenda text\n");
   else
      strcat(line, "(obs waves) show    %legenda text\n");
   fprintf(os_ps, "%s", line);


   /*
   ///////// 2de aanduiding in legenda ////////////
   */

   /* color (- legenda text f-rule) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_f_rule);
   strcat(line, " setrgbcolor %legenda text f-rule\n");
   fprintf(os_ps, "%s", line);

   /* moveto (- legenda text f-rule) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of21);
   sprintf(buffer2, "%d", y2 + of16);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text f-rule\n");
   fprintf(os_ps, "%s", line);

   /* show (- legenda text f-rule) */
   strcpy(line, "\0");
   strcpy(line, "( __) show    %legenda text f-rule\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text f-rule) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda text f-rule\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda text f-rule) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x1 + of21 + of19);
   sprintf(buffer2, "%d", y2 + of16 - of20);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text f-rule\n");
   fprintf(os_ps, "%s", line);

   /* show (legenda text f-rule) */
   strcpy(line, "\0");
   strcat(line, "(f-5 rule) show    %legenda text f-rule\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);


   /*
   ///////// 3de aanduiding in legenda ////////////
   */
   if ( ((strncmp(MODE, "TSA", 3) == 0) && (pag_no == 0)) || (strncmp(MODE, "TSV", 3) == 0) ) /* ALLEEN eerste pagina in TSA mode een 3de aanduiding !! */
   {
      /* color (- legenda text obs in forecast) */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
      strcat(line, " setrgbcolor %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* moveto (- legenda text obs in forecast) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 + of22);
      sprintf(buffer2, "%d", y2 + of16);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto      %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* show (- legenda text obs in forecast) */
      strcpy(line, "\0");
      strcpy(line, "( __) show    %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* color (legenda text obs in forecast) */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
      strcat(line, " setrgbcolor %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* moveto (legenda text obs in forecast) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 + of22 + of19);
      sprintf(buffer2, "%d", y2 + of16 - of20);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto      %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* show (legenda text obs in forecast) */
      strcpy(line, "\0");
      strcat(line, "(obs) show    %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);

   } /* if ( (strncmp(MODE, "TSA", 3) == 0) && (pag_no == 0)) || (strncmp(MODE, "TSV", 3) == 0) )*/



   /**************************************************************************/
   /*                                                                        */
   /*                             x-y box rechts                             */
   /*                                                                        */
   /**************************************************************************/


   /*
   ///////////// color x-y box rexhts //////////////
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
   sprintf(buffer1, "%d", x3);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x4);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x4);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %color x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (color x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3);
   sprintf(buffer2, "%d", y2);
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
   /////////// font zetten rechtse box ////////////
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
   ////////////// kader (contouren) tekenen rechtse box //////////////
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
   sprintf(buffer1, "%d", x3);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x4);
   sprintf(buffer2, "%d", _y1);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x4);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto   %kader x-y box\n");
   fprintf(os_ps, "%s", line);

   /* lineto (kader x-y box)*/
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3);
   sprintf(buffer2, "%d", y2);
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
   /////// x-as indeling rechtse box /////////
   */

   x_factor = 10;                                   /* schaal x-as om de 0.1 Hz echter waarden zijn om de 0.01 Hz */


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
   //        '0.0 0.1'               } 5
   //
   //
   */


   /* x-as schaal verdeling:
   //    van 0.0 - 0.4 per 0.1 oplopend
   // of van 0.0 - 0.3 per 0.1 oplopend
   // of van 0.0 - 0.2 per 0.1 oplopend
   */
   for (i = 0; i <= scale_x_max; i++)                   /* loop over Hz (0.0 t/m 0.5 Hz) */
   {
      /* moveto (mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x3 + (i * of4));
      sprintf(buffer2, "%d", _y1);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %Hz-mark\n");
      fprintf(os_ps, "%s", line);

      /* lineto (mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x3 + (i * of4));
      sprintf(buffer2, "%d", _y1 - mark_lengte);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %Hz-mark\n");
      fprintf(os_ps, "%s", line);

      /* stroke (mark) */
      strcpy(line, "\0");
      strcpy(line, "stroke         %Hz-mark\n");
      fprintf(os_ps, "%s", line);

      /* moveto (Hz aanduiding) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x3 + (i * of4) - of6);
      sprintf(buffer2, "%d", _y1 - of5);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %Hz\n");
      fprintf(os_ps, "%s", line);

      /* show (Hz aanduiding) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      sprintf(buffer1, "%0.1f", (float)i / x_factor);
      strcpy(line, "(");
      strcat(line, buffer1);
      strcat(line, ")");
      strcat(line, " show      %Hz\n");
      fprintf(os_ps, "%s", line);

   } /* for (i = 0; i <= scale_x_max; i++) */


   /* algemene Hz aanduiding x-as */

   /* moveto (algemene as-aanduiding) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + (scale_x_max * of4) + of27);
   sprintf(buffer2, "%d", _y1 - of5);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto    %x-as-eenheden aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* show (algemene as-aanduiding) */
   strcpy(line, "\0");
   strcpy(line, "( Hz) show    %x-as-eenheden aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   /////////  y - as indeling rechtse box //////////
   */
   y_factor_richting = 45;            /* afstand tussen marks op y-as is 45 graden */

   hor_afstand = of10b;               /* afstand tot x-as (x3) waar de degrees aanduidingen (b.v. 90 degr.) beginnen */

   for (i = 0; i < 9; i++)            /* loop over degrees (0 t/m 360 degr. per 45 degr.) */
   {
      /* linkse-as moveto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x4);
      sprintf(buffer2, "%d", _y1 + (i * of3b));
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark links\n");
      fprintf(os_ps, "%s", line);

      /* linkse-as lineto (par. mark) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x4 + mark_lengte);
      sprintf(buffer2, "%d", _y1 + (i * of3b));
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
      sprintf(buffer1, "%d", x4 + mark_lengte + of42);
      sprintf(buffer2, "%d", _y1 + (i * of3b) - of11);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %par-mark links\n");
      fprintf(os_ps, "%s", line);

      /* linkse-as show */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      sprintf(buffer1, "%d", i * y_factor_richting);
      strcpy(line, "(");
      strcat(line, buffer1);
      strcat(line, ")");
      strcat(line, " show      %par-mark links\n");
      fprintf(os_ps, "%s", line);

   } /* for (i = 0; i < 9 i++) */


   /* moveto (algemene y-as aanduiding) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x4 + of41);
   sprintf(buffer2, "%d", _y1 + of40);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto     %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* rotate (y-as aanduiding) */
   strcpy(line, "\0");
   strcat(line, "90 rotate        %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* show (y-as aanduiding) */
   strcpy(line, "\0");
   strcat(line, "(degr.) show    %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* rotate (y-as aanduiding) [weer terugzetten]*/
   strcpy(line, "\0");
   strcat(line, "-90 rotate       %y-as aanduiding\n");
   fprintf(os_ps, "%s", line);

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);



   /*
   ///////////// legenda rechtse box //////////////
   */



   /*
   //
   //                                                             _____ of_hook
   //                                                            |
   //                  ________________________________________|_V_|_ _ _ _ _
   //                /         ______ of19________________     \       } of_hook
   //              /          |              |            |      \  _ _} _ _ _
   //             |           V              V            V        |   }
   //             |        --- item1      --- item2    --- item3   |   } of14
   //      of16 { |________________________________________________|___}____________ y2
   //             |<--------------------- of15 ------------------->|
   //             |<-of18->|
   //             |<------- of21 -------->|
   //             |<--------------- of22 ------------->|
   //             |
   //            x3
   //
   //
   // NB item 3 alleen eerste pagina TSA mode
   */




   /* newpath (legenda richting) */
   strcpy(line, "\0");
   strcpy(line, "newpath            %legenda richting richting\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text richting) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda kader richting\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda kader richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of15);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto     %legenda kader richting\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of15);
   sprintf(buffer2, "%d", y2 + of14 - of_hook);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader richting\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of15 - of_hook);
   sprintf(buffer2, "%d", y2 + of14);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader richting\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of_hook);
   sprintf(buffer2, "%d", y2 + of14);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader richting\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3);
   sprintf(buffer2, "%d", y2 + of14 - of_hook);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader richting\n");
   fprintf(os_ps, "%s", line);

   /* lineto (legenda kader richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3);
   sprintf(buffer2, "%d", y2);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " lineto     %legenda kader richting\n");
   fprintf(os_ps, "%s", line);

   /* stroke (legenda kader richting) */
   strcpy(line, "\0");
   strcpy(line, "stroke            %legenda kader richting\n");
   fprintf(os_ps, "%s", line);



   /*
   //////////   1ste aanduiding in legenda ////////////
   */

   /* color (- legenda text richting) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_obs_ZEE);
   strcat(line, " setrgbcolor %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* moveto (- legenda text richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of18);
   sprintf(buffer2, "%d", y2 + of16);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* show (- legenda text richting) */
   strcpy(line, "\0");
   strcpy(line, "( __) show    %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text richting) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda text richting richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of18 + of19);
   sprintf(buffer2, "%d", y2 + of16 - of20);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* show (legenda text richting richting) */
   strcpy(line, "\0");
   if ( (strncmp(MODE, "TSA", 3) == 0) || (strncmp(MODE, "TSV", 3) == 0) )
      strcat(line, "(nedwam) show    %legenda text richting\n");
   else
      strcat(line, "(obs waves) show    %legenda text richting\n");
   fprintf(os_ps, "%s", line);



   /*
   ///////// 2de aanduiding in legenda ////////////
   */

   /* color (- legenda text richting) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_dd);
   strcat(line, " setrgbcolor %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* moveto (- legenda text richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of21);
   sprintf(buffer2, "%d", y2 + of16);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* show (- legenda text richting) */
   strcpy(line, "\0");
   strcpy(line, "( __) show    %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* color (legenda text richting) */
   strcpy(line, "\0");
   strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
   strcat(line, " setrgbcolor %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* moveto (legenda text richting richting) */
   strcpy(line, "\0");
   buffer1[0] = '\0';
   buffer2[0] = '\0';
   sprintf(buffer1, "%d", x3 + of21 + of19);
   sprintf(buffer2, "%d", y2 + of16 - of20);
   strcpy(line, buffer1);
   strcat(line, " ");
   strcat(line, buffer2);
   strcat(line, " moveto      %legenda text richting\n");
   fprintf(os_ps, "%s", line);

   /* show (legenda text richting richting) */
   strcpy(line, "\0");
   if ( (strncmp(MODE, "TSA", 3) == 0) || (strncmp(MODE, "TSV", 3) == 0) )
      strcat(line, "(hirlam22) show    %legenda text richting\n");
   else
      strcat(line, "(obs wind) show    %legenda text richting\n");
   fprintf(os_ps, "%s", line);


   /*
   ///////// 3de aanduiding in legenda ////////////
   */
   if ( ((strncmp(MODE, "TSA", 3) == 0) && (pag_no == 0)) || (strncmp(MODE, "TSV", 3) == 0) ) /* ALLEEN eerste pagina in TSA mode een 3de aanduiding !! */
   {
      /* color (- legenda text obs in forecast) */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_obs_in_forecast_ZEE);
      strcat(line, " setrgbcolor %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* moveto (- legenda text obs in forecast) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x3 + of22);
      sprintf(buffer2, "%d", y2 + of16);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto      %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* show (- legenda text obs in forecast) */
      strcpy(line, "\0");
      strcpy(line, "( __) show    %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* color (legenda text obs in forecast) */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_x_y_box_kader_ZEE);
      strcat(line, " setrgbcolor %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* moveto (legenda text obs in forecast) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x3 + of22 + of19);
      sprintf(buffer2, "%d", y2 + of16 - of20);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto      %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);

      /* show (legenda text obs in forecast) */
      strcpy(line, "\0");
      strcat(line, "(obs) show    %legenda text obs in forecast\n");
      fprintf(os_ps, "%s", line);
   } /* if ( ((strncmp(MODE, "TSA", 3) == 0) && (pag_no == 0)) || (strncmp(MODE, "TSV", 3) == 0) ) */

   /* lege regel */
   strcpy(line, "\n");
   fprintf(os_ps, "%s", line);

   
   return 0;
}







/*
Evert hanteert als grenzen voor het bepalen van het maximum voor het te tekenen spectrum :
0.02 m2/Hz (minder niet doen op verticale schaal

en pas bij 0.05 Hz beginnen voor maximum bepalen

*/





/*****************************************************************************/
/*                                                                           */
/*                 Bepaal_Filenamen_Temp_Files_Per_Locatie                   */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenamen_Temp_Files_Per_Locatie(char* station, int cic_uren_interval)
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
   strcpy(tempfilenaam_0, "\0");
   strcpy(tempfilenaam_1, "\0");
   strcpy(tempfilenaam_2, "\0");


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




   /* hulp voor file namen van 1 (of 3 in TSA mode) en 2 (of 6 in TSA mode) uur terug bepalen (t.o.v van 0 uur terug = JJJJJMMDDUU) */

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
   /*if (strncmp(MODE, "DSP", 3) == 0)       */
   /*   strcat(tempfilenaam_0, "CIC_DSP_");  */
   /*else if (strncmp(MODE, "CIC", 3) == 0)  */
   strcat(tempfilenaam_0, "CIC_LDS_");
   strcat(tempfilenaam_0, station);
   strcat(tempfilenaam_0, "_");
   strcat(tempfilenaam_0, JJJJMMDDUU);
   strcat(tempfilenaam_0, ".TMP");


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

      /*if (strncmp(MODE, "DSP", 3) == 0)         */
      /*   strcat(tempfilenaam_1, "CIC_DSP_");    */
      /*else if (strncmp(MODE, "CIC", 3) == 0)    */
      strcat(tempfilenaam_1, "CIC_LDS_");
      strcat(tempfilenaam_1, station);
      strcat(tempfilenaam_1, "_");
      strcat(tempfilenaam_1, date_time);
      strcat(tempfilenaam_1, ".TMP");
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

      /*if (strncmp(MODE, "DSP", 3) == 0)       */
      /*   strcat(tempfilenaam_2, "CIC_DSP_");  */
      /*else if (strncmp(MODE, "CIC", 3) == 0)  */
      strcat(tempfilenaam_2, "CIC_LDS_");
      strcat(tempfilenaam_2, station);
      strcat(tempfilenaam_2, "_");
      strcat(tempfilenaam_2, date_time);
      strcat(tempfilenaam_2, ".TMP");
   } /* if (tijd_ok == 1) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                   Bepaal_Outputfilenaam_Per_Locatie                       */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Outputfilenaam_Per_Locatie(char* station)
{
   /* b.v. output_cic/CIC_K13.PS */


   /* initialisatie */
   strcpy(cic_outputfilenaam, "\0");

   /* postscript outputfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
   {
      strcpy(cic_outputfilenaam, "output_cic\\");
   }
   else                                                /* UNIX */
   {
      strcpy(cic_outputfilenaam, "output_cic/");
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */

   strcat(cic_outputfilenaam, "CIC_");
   strcat(cic_outputfilenaam, station);
   strcat(cic_outputfilenaam, ".PS");


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                    Bepaal_Max_Energy_Temp_Files                           */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Max_Energy_Temp_Files(int* max_energy)
{
   FILE* temp;
   int i;
   char regel[NUM_SPECTRUM_2 + 1];
   int regel_lengte = NUM_SPECTRUM_2;
   char tempfilenaam[256];
   int k;



   /* 3 temp files vormen 1 postscript pagina, moeten dus allemaal de zelfde */
   /* schaalverdeling hebben, daarom max van deze files bepalen */


   /* initialisatie */
   *max_energy = 0;


   for (k = 0; k < 3; k++)
   {
      /* initialisatie */
      strcpy(tempfilenaam, "\0");

      if (k == 0)
         strcpy(tempfilenaam, tempfilenaam_0);
      else if (k == 1)
         strcpy(tempfilenaam, tempfilenaam_1);
      else if (k == 2)
         strcpy(tempfilenaam, tempfilenaam_2);


      if ((temp = fopen(tempfilenaam, "r")) != NULL)           /* dus gelukt */
      {
         /* spectrum weer inlezen */
         for (i = 0; i < NUM_SPECTRUM; i++)  /* zoveel spectrum array elementen zoveel regels in de temp file */
         {
            if (fread(regel, regel_lengte +1, 1, temp) != 0)
            {
               regel[regel_lengte] = '\0';

               if ((i >= BEGIN_SPECTRUM_ENERGY) && (i <= END_SPECTRUM_ENERGY))
                  if (atoi(regel) > *max_energy)
                     *max_energy = atoi(regel);               /* in .1cm2s !!! */

            } /*  if (fread(regel, regel_lengte +1, 1, temp) != NULL) */
         } /* for (i = 0; i < NUM_SPECTRUM; i++) */

         fclose(temp);

      } /* if ((temp = fopen(tempfilenaam_0, "r")) != NULL) */
   } /* for (k = 0; k < 3; k++) */


   return 0;
}




/*****************************************************************************/
/*                                                                           */
/*                          Bepaal_Scale_Y_X_Max                             */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Scale_Y_X_Max(int max_energy, int *scale_y_max, int *scale_x_max)
{
   float hulp_max_energy;
   int i;
   int doorgaan;
   /*char buffer[100];*/


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

   /* max_energy omzetten van .1cm2s --> m2/Hz en afronden naar boven */
   hulp_max_energy = (float)max_energy / 100000;

   /* scale_y_max range: 1 t/m 9 m2/Hz */
   doorgaan = 1;                               /* uitgangs situatie */
   for (i = 1; i < 10; i++)
   {
      if (hulp_max_energy < (float)i)
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
         if (hulp_max_energy < (float)i)
         {
            *scale_y_max = i;
            break;
         }
      } /* for (i = 10; i <= 1000; i += 10) */
   } /* if (doorgaan == 1) */



   /*
   ///////////////////   scale_x_max   /////////////
   */

   if (max_energy < 500000)        /* t/m 12-02-2002: 1000000*/
      *scale_x_max = 4;
   else if (max_energy < 5000000)  /* t/m 12-2-2002: 10000000*/
      *scale_x_max = 3;
   else
      *scale_x_max = 2;

/**************  test ************/
/*
                 buffer[0] = '\0';
                 sprintf(buffer, "%d", max_energy);
                 fprintf(stderr, "%s", buffer);
                 getchar();
*/
/**************** test ************/





   return 0;
}




/******************************************************************************/
/*                                                                            */
/*                          Temp_Files_To_Postscript                          */
/*                                                                            */
/******************************************************************************/
int Temp_Files_To_Postscript(char* grafiek_orientatie, const int scale_x_max, const int scale_y_max, int cic_uren_interval)
{
   FILE* temp;
   struct tm time_struct;

   const int of13        = 20;                      /* vert. offset t.o.v. y2 voor title */
   const int of33        = 30;                      /* vert. offset t.o.v. x-as (y1) voor eerste summary regel */
   /*const int of33b       = 43;*/                      /* vert. offset t.o.v. x-as (y1) voor tweede summary regel  */
   const int of34        = 40;                      /* hor. offset t.o.v. x1 (y-as) voor eerste summary regel aanduiding */
   /*const int of34b       = -100;*/                    /* hor. offset t.o.v. x1 (y-as) voor tweede summary regel aanduiding */
   const int max_richtings_shift = 120;             /* bij richtingen indien verschil met vorige groter dan deze waarde dan de vebindingslijn niet doortrekken */
   /*const int AANTAL_CZZ10_ELEMENTEN = 41;*/ /* 0.0 t/m 0.4 Hz (stapjes 0.01 Hz) */ /* SPECTRUM_ENERGY_0_4 + 1 = BEGIN_SPECTRUM_ENERGY;*/ /* NB minder dan in project spectra omdat niet alle beschikbare freq. waarden worden meegenomen */

   int i;
   int regel_lengte = NUM_SPECTRUM_2;
   int num_JJJJ;
   int num_MM;
   int num_DD;
   int num_UU;
   /*int num_LFE;*/
   int eind_index;
   /*int num_ff;*/
   /*int num_dd_gem_total;*/
   /*int num_dd_gem_LFE;*/
   int obs_value_old;
   int obs_value_new;
   int num_dd;
   int start_punt_f_rule;                           /* f-rule */
   int uren_terug;
   int tijd_ok;
   /*int aantal_freq_array_elementen;*/                 /* BFI etc. */

   char regel[NUM_SPECTRUM_2 + 1];
   char line[256];
   char buffer1[10];                                /* opslag tussen waarden */
   char buffer2[10];                                /* opslag tussen waarden */
   char tempfilenaam[256];
   char title[256];
   char JJJJ[5];
   char MM[3];
   char DD[3];
   char UU[3];
   char datumtijd[100];
   char summary_1[256];                             /* summary regel */
   char summary_2[256];                             /* summary regel */
   char station[4];                                 /* wordt alleen gebruikt als een temp file er niet is en dan toch een title maken */

   float f;                                         /* f-rule */
   float f_waarde;                                  /* f-rule */
   float f_eind_index;                              /* f-rule */

   /*double num_HE3;*/
   double num_Hm0;
   double num_Tm0_1;
   /*double hulp_HE3_4; */                              /* hulp (tussen) variable om van HE3 -> LFE te maken */
   /*double num_Czz10_array[AANTAL_CZZ10_ELEMENTEN];*/  /* BFI etc. */
   /*double BFI;*/
   /*double num_fp;*/
   /*double C4;*/
   /*double P;*/
   double num_Hmax;
   double num_BFI;
   double num_C4;


   /* station is bijv. ANA, K13 */

   /* alleen naar postscript omzetten als er een nieuwe temp file is, dus JJJJMMDDUU moet geldig zijn */
   /* daarom als openen mislukt dan wordt er geen melding gegeven in de log file */
   /* in eerdere deel van de verwerking is dit dan al gebleken en in log geschreven */


   /* initialisatie */
   tempfilenaam[0] = '\0';
   title[0]        = '\0';
   datumtijd[0]    = '\0';
   summary_1[0]    = '\0';
   summary_2[0]    = '\0';


   /*
   // afh. van grafiek orientatie een temp file openen
   */
   if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_BOVENSTE_GRAFIEK) == 0)
   {
      strcpy(tempfilenaam, tempfilenaam_0);                 /* laatste temp file */
      uren_terug = 0;
   }
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0)
   {
      strcpy(tempfilenaam, tempfilenaam_1);                 /* 1 uur terug */
      uren_terug = 1 * cic_uren_interval;                   /* i.v.m. title (cic_uren_interval = 1 = normale CIC mode) */
   } /* else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_MIDDELSTE_GRAFIEK) == 0) */
   else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0)
   {
      strcpy(tempfilenaam, tempfilenaam_2);                 /* 2 uur terug */
      uren_terug = 2 * cic_uren_interval;                   /* i.v.m. title (cic_uren_interval = 1 = normale CIC mode) */
   } /* else if (strcmp(grafiek_orientatie, POSTSCRIPTFILE_ONDERSTE_GRAFIEK) == 0) */



   /*
   // i.v.m. title
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
   // temp file inlezen en schrijven naar ps output file
   */

   if ((temp = fopen(tempfilenaam, "r")) != NULL)  /* dus gelukt */
   {
      /*
      //////////// spectrum energy (linker paneel) //////////
      */

      /* newpath obs area */
      strcpy(line, "\0");
      strcpy(line, "newpath         %obs area\n");
      fprintf(os_ps, "%s", line);

      /* color obs area */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_obs_ZEE);
      strcat(line, " setrgbcolor %obs area\n");
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
      strcat(line, " moveto   %obs area oorsprong\n");
      fprintf(os_ps, "%s", line);



      /* alleen regels (komt overeen met indices uit spectrum array) met energy inlezen */
      if (scale_x_max == 4)                           /* alleen uit het spectrum waarden t/m 0.4 Hz lezen */
         eind_index = SPECTRUM_ENERGY_0_4;
      else if (scale_x_max == 3)                      /* alleen uit het spectrum waarden t/m 0.3 Hz lezen */
         eind_index = SPECTRUM_ENERGY_0_3;
      else if  (scale_x_max == 2)                     /* alleen uit het spectrum waarden t/m 0.2 Hz lezen */
         eind_index = SPECTRUM_ENERGY_0_2;
      else
         eind_index = END_SPECTRUM_ENERGY;            /* als het goed is kont hij hier nooit in */


      for (i = 0; i < NUM_SPECTRUM; i++)              /* zoveel spectrum array elementen zoveel regels in de temp file */
      {
         if (fread(regel, regel_lengte +1, 1, temp) != 0)
         {
            regel[regel_lengte] = '\0';

            if (i == 0)                               /* stations naam indice */
            {
               title[0]        = '\0';
               strcpy(title, "variance density     ");
               strcat(title, regel);
               strcat(title, " ");

               strcpy(station, regel);                /* ook nog nodig voor BFI et. berekening*/
            } /* if (i == 0) */

            if (i == 1)                               /* datum-tijd waarneming indice */
            {
               strcat(title, datumtijd);
               strcat(title, "      direction");
            } /* if (i == 1) */

            if ((i >= BEGIN_SPECTRUM_ENERGY) && (i <= eind_index))
            {
               /* lineto obs area */
               strcpy(line, "\0");
               buffer1[0] = '\0';
               buffer2[0] = '\0';
               sprintf(buffer1, "%d", x1 + ((i - BEGIN_SPECTRUM_ENERGY) * of4 / x_factor));      /* x-coordinaat */
               sprintf(buffer2, "%d", _y1 + ( (int)(atof(regel) / 100000 * of3 / y_factor_energy + 0.5)) );   /* y-coordinaat */
               strcpy(line, buffer1);
               strcat(line, " ");
               strcat(line, buffer2);
               strcat(line, " lineto   %obs area\n");
               fprintf(os_ps, "%s", line);

            } /* if ((i >= BEGIN_SPECTRUM_ENERGY) && (i <= END_SPECTRUM_ENERGY)) */


            /*
            // uitlezen waarden voor summary regel
            */
            if (i == HM0_SPECTRUM)                                /* voor summary regel */
               num_Hm0 = atof(regel) / 1000;                      /* van mm --> m */

            if (i == TM0_1_SPECTRUM)                              /* voor summary regel */
               num_Tm0_1 = atof(regel) / 10;                      /* van 0.1 sec --> sec (.1f doet dit automatisch correct )*/
#if 0
//            if (i == HE3_SPECTRUM)                                /* voor summary regel */
//               num_HE3 = atof(regel);
//
//            if (i == FF_WIND)                                     /* voor summary regel */
//               num_ff = atoi(regel);
//
//            if (i == DD_GEM_SPECTRUM)                             /* voor summary regel */
//               num_dd_gem_total = atoi(regel);
//
//            if (i == DD_GEM_LFE_SPECTRUM)                         /* voor summary regel */
//               num_dd_gem_LFE = atoi(regel);
#endif
            if (i == HMAX_SPECTRUM)                               /* voor summary regel */
               num_Hmax = atof(regel) / 1000;                     /* mm -> m */

            if (i == BFI_SPECTRUM)                                /* voor summary regel */
               num_BFI = atof(regel);

            if (i == C4_SPECTRUM)                                 /* voor summary regel */
               num_C4 = atof(regel);


         } /*  if (fread(regel, regel_lengte +1, 1, temp) != NULL) */
      } /* for (i = 0; i < NUM_SPECTRUM; i++) */


      /* lineto eindpunt (x2,y1) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x2);
      sprintf(buffer2, "%d", _y1);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " lineto   %obs area eindpunt\n");
      fprintf(os_ps, "%s", line);

      /* closepath (obs area) */
      strcpy(line, "\0");
      strcpy(line, "closepath         %obs area\n");
      fprintf(os_ps, "%s", line);

      /* fill obs area */
      strcpy(line, "\0");
      strcpy(line, "fill           %obs area\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);


      /*
      ////////////// 0,0005*f**-5 ////////////
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
      ////////////////// EERSTE summary regel: Hm0, Tm0-1, BFI etc.
      //
      // b.v. Hm0 = 2.39m  Tm0-1 = 5.9s  BFI = 0.21  C4 = 0.01  Hmax = 2.32  Hmax/Hm0 = 1.7
      */


      /* Hm0 */
      buffer1[0] = '\0';
      strcat(summary_1, "Hm0 = ");
      sprintf(buffer1, "%.1f", num_Hm0);                /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */

      if (strncmp(buffer1, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen */
         strcat(summary_1, "-");
      else
         strcat(summary_1, buffer1);

      strcat(summary_1, "m  ");

      /* TM0-1 */
      buffer1[0] = '\0';
      strcat(summary_1, "Tm0-1 = ");
      sprintf(buffer1, "%.1f", num_Tm0_1);

      if (strncmp(buffer1, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen */
         strcat(summary_1, "-");
      else
         strcat(summary_1, buffer1);

      strcat(summary_1, "s  ");

      /* BFI */
      buffer1[0] = '\0';
      strcat(summary_1, "BFI = ");
      if (num_BFI != MAXFLOAT)
      {
         sprintf(buffer1, "%.2f", num_BFI);
         strcat(summary_1, buffer1);
      }
      else
         strcat(summary_1, "-");
      strcat(summary_1, "  ");

      /* C4 */
      buffer1[0] = '\0';
      strcat(summary_1, "C4 = ");
      if (num_C4 != MAXFLOAT)
      {
         sprintf(buffer1, "%.2f", num_C4);
         strcat(summary_1, buffer1);
      }
      else
         strcat(summary_1, "-");
      strcat(summary_1, "  ");

      /* Hmax */
      buffer1[0] = '\0';
      strcat(summary_1, "Hmax = ");
      sprintf(buffer1, "%.1f", num_Hmax);               /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */

      if (strncmp(buffer1, "0.0", 3) == 0)              /* 0 betekent hier niet waargenomen */
         strcat(summary_1, "-");
      else
         strcat(summary_1, buffer1);

      strcat(summary_1, "m  ");

      /* Hmax/Hm0 */
      strcat(summary_1, "Hmax/Hm0 = ");

      sprintf(buffer1, "%.1f", num_Hmax);               /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */
      sprintf(buffer2, "%.1f", num_Hm0);                /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */

      if ( (strncmp(buffer1, "0.0", 3) != 0) && (strncmp(buffer2, "0.0", 3) != 0) && (num_Hm0 != 0.0) && (num_Hmax != 0.0) )
      {
          buffer1[0] = '\0';
          sprintf(buffer1, "%.1f", num_Hmax / num_Hm0);   /* afronden 1 cijfer achter de komma (.1f doet dit automatisch correct ) */
          strcat(summary_1, buffer1);
      }
      else
          strcat(summary_1, "-");

      strcat(summary_1, " ");



#if 0
//      /* P */      /* moet een andere formule worden omadat nu met C4 en niet K40 wordt gewerkt */
//      buffer1[0] = '\0';
//      strcat(summary, "P20(Hmax>2*Hm0) = ");
//      if (P != MAXFLOAT)
//      {
//         sprintf(buffer1, "%.2f", P);
//         strcat(summary, buffer1);
//      }
//      else
//         strcat(summary, "-");
//      strcat(summary, "  ");
#endif


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
      strcpy(line, R_G_B_color_obs_summary_ZEE);
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
      strcat(line, " moveto   %summary_1\n");
      fprintf(os_ps, "%s", line);

      /* show (summary) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, summary_1);
      strcat(line, ")");
      strcat(line, " show      %summary_1\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      /*strcpy(line, "\n");*/
      /*fprintf(os_ps, "%s", line);*/

#if 0
      /*
      ////////////////// TWEEDE summary regel: ff, LFE etc.
      //
      // b.v. ff = 12 kn  LFE = 10cm2 dir-tot = 230 dir-LFE = 234
      */

      /* ff */
      buffer1[0] = '\0';
      strcat(summary_2, "ff = ");

      if ((num_ff == 0) || (num_ff == 99))                     /* 99 betekent hier niet waargenomen; bij 0 kan het zijn niet waargenomen en ook windstil */
         strcat(summary_2, "-");
      else
      {
         sprintf(buffer1, "%d", num_ff);
         strcat(summary_2, buffer1);
      }

      strcat(summary_2, "kn  ");

      /* LFE (HE3 omzetten naar LFE: LFE = (HE3 / 4)**2) */
      buffer1[0] = '\0';
      strcat(summary_2, "LFE = ");
      /*num_LFE = (int)(pow(num_HE3 / 4.0, 2) + 0.5);*/ /* afronden op hele waarden -> pow geeft problemen op bdawo8 */
      hulp_HE3_4 = num_HE3 / 40.0 ;                     /* was ook in mm -> cm dus ook nog eens delen door 10 */
      num_LFE = (int)(hulp_HE3_4 * hulp_HE3_4 + 0.5);   /* afronden op hele waarden */
      sprintf(buffer1, "%d", num_LFE);

      if (strncmp(buffer1, "0", 1) == 0)                /* 0 betekent hier niet waargenomen */
         strcat(summary_2, "-");
      else
         strcat(summary_2, buffer1);

      strcat(summary_2, "cm2  ");

      /* dir tot */
      buffer1[0] = '\0';
      strcat(summary_2, "dir.tot = ");

      if ((num_dd_gem_total > 0) && (num_dd_gem_total <= 360)) /* 0 betekent hier niet waargenomen */
      {
         sprintf(buffer1, "%03d", num_dd_gem_total);           /* altijd 3 cijfers */
         strcat(summary_2, buffer1);
      }
      else
         strcat(summary_2, "-");

      strcat(summary_2, "  ");

      /* dir LFE */
      buffer1[0] = '\0';
      strcat(summary_2, "dir.LFE = ");

      if ((num_dd_gem_LFE > 0) && (num_dd_gem_LFE <= 360))      /* 0 betekent hier niet waargenomen */
      {
         sprintf(buffer1, "%03d", num_dd_gem_LFE);             /* altijd 3 cijfers */
         strcat(summary_2, buffer1);
      }
      else
         strcat(summary_2, "-");

      strcat(summary_2, "  ");


      /* moveto (summary 2) */
      strcpy(line, "\0");
      buffer1[0] = '\0';
      buffer2[0] = '\0';
      sprintf(buffer1, "%d", x1 - of34b);
      sprintf(buffer2, "%d", _y1 - of33b);
      strcpy(line, buffer1);
      strcat(line, " ");
      strcat(line, buffer2);
      strcat(line, " moveto   %summary_2\n");
      fprintf(os_ps, "%s", line);

      /* show (summary 2) */
      strcpy(line, "\0");
      strcpy(line, "(");
      strcat(line, summary_2);
      strcat(line, ")");
      strcat(line, " show      %summary_2\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);

#endif

      /*
      /////////// title (wel aanwezige data) ///////////
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


      fclose(temp);

   } /*  if ((temp = fopen(tempfilenaam, "r")) != NULL) */
   else /* file niet te openen */
   {
      /* dan toch een title neer zetten, da data komt dan niet uit de file, maar van de gezochte, niet aanwezige file naam */
      /* bv tempfilenaam; .../temp_cic/CIC_LDS_EPL_2001081711.TMP */

      /* initialisatie */
      title[0]        = '\0';

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
      /////////// title (niet aanwezige data) ///////////
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

   } /* else (file niet te openen) */



   /*
   ///////// nogmaals lezen van de temp file maar nu alleen om de richtingen er uit te halen ///////
   */

   /* temp file inlezen en schrijven naar ps output file */
   if ((temp = fopen(tempfilenaam, "r")) != NULL)  /* dus gelukt */
   {

      /*
      ////////////// wind richting in rechter paneel (nb eerst wind richting dan golfrichting plotten) ///////////////
      */
      num_dd = -1;                                            /* initialisatie */
      for (i = 0; i < NUM_SPECTRUM; i++)  /* zoveel spectrum array elementen zoveel regels in de temp file */
      {
         if (fread(regel, regel_lengte +1, 1, temp) != 0)
         {
            regel[regel_lengte] = '\0';

            if (i == DD_WIND)
            {
               num_dd = atoi(regel) * 10;                     /* was in tientallen graden */
               break;
            } /* if (i == DD_WIND) */
         } /* if (fread(regel, regel_lengte +1, 1, temp) != NULL) */
      } /* for (i = 0; i < NUM_SPECTRUM; i++) */

      if ((num_dd > 0) && (num_dd <= 360))                    /* 0 betekent hier niet waargenomen */
      {
         /* newpath wind richting */
         strcpy(line, "\0");
         strcpy(line, "newpath         %dd (wind)\n");
         fprintf(os_ps, "%s", line);

         /* color wind richting */
         strcpy(line, "\0");
         strcpy(line, R_G_B_color_dd);
         strcat(line, " setrgbcolor %dd (wind)\n");
         fprintf(os_ps, "%s", line);

         /* setlinewidth wind richting */
         strcpy(line, "\0");
         strcpy(line, "0.2 setlinewidth   %dd (wind)\n");
         fprintf(os_ps, "%s", line);

         /* moveto wind richting */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", x3);                                                         /* x-coordinaat */
         sprintf(buffer2, "%d", _y1 + ( (int)(num_dd * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " moveto   %dd (wind)\n");
         fprintf(os_ps, "%s", line);

         /* lineto wind richting */
         strcpy(line, "\0");
         buffer1[0] = '\0';
         buffer2[0] = '\0';
         sprintf(buffer1, "%d", x4);                                                         /* x-coordinaat */
         sprintf(buffer2, "%d", _y1 + ( (int)(num_dd * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
         strcpy(line, buffer1);
         strcat(line, " ");
         strcat(line, buffer2);
         strcat(line, " lineto   %dd (wind)\n");
         fprintf(os_ps, "%s", line);

         /* stroke wind richting */
         strcpy(line, "\0");
         strcat(line, "stroke   %dd (wind)\n");
         fprintf(os_ps, "%s", line);

         /* lege regel */
         strcpy(line, "\n");
         fprintf(os_ps, "%s", line);

      } /* if ((num_ff > 0) && (num_ff <= 360)) */


      /*
      //////////// golfrichting in rechter paneel ////////////
      */

      /* newpath obs richting */
      strcpy(line, "\0");
      strcpy(line, "newpath         %obs richting\n");
      fprintf(os_ps, "%s", line);

      /* color obs richting */
      strcpy(line, "\0");
      strcpy(line, R_G_B_color_obs_ZEE);
      strcat(line, " setrgbcolor %obs richting\n");
      fprintf(os_ps, "%s", line);

      /* setlinewidth obs-richting */
      strcpy(line, "\0");
      strcpy(line, "1.0 setlinewidth   %obs richting\n");
      fprintf(os_ps, "%s", line);


      /* alleen regels (komt overeen met indices uit spectrum array) met richting inlezen */
      if (scale_x_max == 4)                           /* alleen uit het spectrum waarden t/m 0.4 Hz lezen */
         eind_index = SPECTRUM_RICHTING_0_4;
      else if (scale_x_max == 3)                      /* alleen uit het spectrum waarden t/m 0.3 Hz lezen */
         eind_index = SPECTRUM_RICHTING_0_3;
      else if  (scale_x_max == 2)                     /* alleen uit het spectrum waarden t/m 0.2 Hz lezen */
         eind_index = SPECTRUM_RICHTING_0_2;
      else
         eind_index = END_SPECTRUM_ENERGY;            /* als het goed is kont hij hier nooit in */

      /* initialisatie */
      obs_value_old = MAXINT;

      /* reset; eerst file pointer weer op nul zetten (file was al geopned i.v.m. dd ingelezen) */
      rewind(temp);

      for (i = 0; i < NUM_SPECTRUM; i++)  /* zoveel spectrum array elementen zoveel regels in de temp file */
      {
         if (fread(regel, regel_lengte +1, 1, temp) != 0)
         {
            regel[regel_lengte] = '\0';

            if ((i >= BEGIN_SPECTRUM_RICHTING) && (i <= eind_index))
            {
               if ( (strcmp(regel, "\0") != 0) && !(atoi(regel) < 0) )        /* -1 kan voorkomen */
               {
                  obs_value_new = atoi(regel);

                  /* n.b. obs_value_new = 0 betekent hier (bij Evert Bouws) geen waarneming/niet bekende waarde */
                  if ( (obs_value_new > 0) && (obs_value_new <= 360) && (obs_value_old != MAXINT) && (abs(obs_value_new - obs_value_old) < max_richtings_shift) )
                  {
                     /* lineto obs richting */
                     strcpy(line, "\0");
                     buffer1[0] = '\0';
                     buffer2[0] = '\0';
                     sprintf(buffer1, "%d", x3 + ((i - BEGIN_SPECTRUM_RICHTING) * of4 / x_factor));  /* x-coordinaat */
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(regel) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
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
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(regel) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
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
                     sprintf(buffer2, "%d", _y1 + ( (int)(atof(regel) * of3b / y_factor_richting + 0.5)) );   /* y-coordinaat */
                     strcpy(line, buffer1);
                     strcat(line, " ");
                     strcat(line, buffer2);
                     strcat(line, " moveto   %obs richting\n");
                     fprintf(os_ps, "%s", line);
                  }

                  obs_value_old = obs_value_new;

               } /* if ( (strcmp(regel, "\0") != 0) && !(atoi(regel) < 0) ) */
               else /* lege regel */
                  obs_value_old = MAXINT;

            } /* if ((i >= BEGIN_SPECTRUM_RICHTING) && (i <= END_SPECTRUM_RICHTING)) */

         } /*  if (fread(regel, regel_lengte +1, 1, temp) != NULL) */
         else
            obs_value_old = MAXINT;

      } /* for (i = 0; i < NUM_SPECTRUM; i++) */

      /* setlinewidth reset */
      strcpy(line, "\0");
      strcpy(line, "0.2 setlinewidth   %reset\n");
      fprintf(os_ps, "%s", line);

      /* lege regel */
      strcpy(line, "\n");
      fprintf(os_ps, "%s", line);

      fclose(temp);

   } /*  if ((temp = fopen(tempfilenaam, "r")) != NULL) */


   return 0;
}



