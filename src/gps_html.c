#if !defined(gps_h)                         /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>                                  /* tolower() */
#include <time.h>


/* externe var's */
extern char stations[NUM_STATIONS][NUM_STATIONS_2]; /* gps_main.c; NUM_STATIONS arrayplaatsen van elk 4 char */
extern char OS[8];                                  /* gps_main.c */
extern char JJJJMMDDUU[11];                         /* gps_main.c; via argument list */

/* interne var's */
int START_UREN_TERUG_TOV_JJJJMMDDUU = 0;
int EIND_UREN_TERUG_TOV_JJJJMMDDUU  = 144;
int RUN_TERUG_VOOR_TSA_FILENAAM     = 48;           /* om de filenamen te noemen naar run tijden */
int html_mode = 0;                                  /* 0 = simple html (geen javascript); 1 = wm-intanet stijl */

/* function prototypes in andere module */
extern int Write_Log(char* bericht);

/* functions prototypes in deze module */
int Write_HTML(const char* station);
int Write_Location_HTML(char* html_station/*, int frame*/);
int Determine_HTML_RunTime(int uren_terug, int mode, char* run_time);
int Write_TSA_HTML(char* html_station/*, int frame*/);



int Write_HTML(const char* station)
{
   /*int frame; */                                      /* 1 is bovensteframe; 2 = onderste frame */
   char html_station[4];
   int c;                                               /* int waarde char voor tolower */
   int k;


   /* n.b. omdat vanaf april 2003 alles vogens WM- intranet layout zijn frames niet meer nodig */

   html_station[0] = '\0';
   strncpy(html_station, station, 3);

   /* eerst stations naam naar kleine letters converteren */
   /*lengte = strlen(html_station);*/
   for (k = 0; k < 3 /*lengte*/; k++)                             /* NUM_STATIONS_2 is b.v. 3 van MUN */
   {
      /*html_station[k] = tolower(html_station[k]);*/       /* geeft warning op verliezen bits */
      c = html_station[k];
      html_station[k] = (char)tolower(c);
   } /* for (k = 0; k < lengte; k++) */
   html_station[3/*lengte*/] = '\0';

   Write_Location_HTML(html_station/*, frame*/);                       /* b.v. mun_1.html (frame 1) */
   Write_TSA_HTML(html_station/*, frame*/);                            /* b.v.: tsa_mun_51_1.html (frame 1 van run 51 uur terug) */


   return 0;
}



int Write_Location_HTML(char* html_station/*, int frame*/)
{
   /*
   // Hier worden files geschreven zoals mun_1.html
   //                                    mun_2.html
   //                                    epl_1.html
   //                                    etc.
   */

   FILE* os_html_location;
   char env_info_knmi_gps_index[512];
   char env_info_knmi_gps_home[512];
   char env_info_knmi_gps_linker_kolom[512];
   char html_filenaam[256];
   char info[255];
   char html_regel[255];
   char previous_buffer[4];                                 /* voor opbouw html file naam */
   char run_time[100];
   int uren_terug;                                          /* teller */
   int mode;                                                /* hoe datum tijd weergeven */



   /* initialisatie */
   strcpy(html_filenaam, "\0");

   if (strcmp(OS, "WINDOWS") == 0)
      /* alleen tijdens test fase */
      strcpy(html_filenaam, "C:\\program files\\BC5\\PROJECTS\\GPS\\output_nedwam\\");
      /*strcat(info, PATH_INTRANET_GPS_WINDOWS);*/              /* b.v.: "i:\\info\\people\\wm-am\stam\\gps\\" */
   else
      /*strcpy(html_filenaam, PATH_INTRANET_GPS_UNIX);*/        /* b.v.: "../../../webdata/INFO/people/wm-am/stam/gps/" */
      strcpy(html_filenaam, getenv("PATH_INTRANET_GPS_UNIX"));
                                                                /* i.v.m. APL */
   strcat(html_filenaam, html_station);
   strcat(html_filenaam, "_");
   strcat(html_filenaam, "1");
   strcat(html_filenaam, ".html");


   if ((os_html_location = fopen(html_filenaam, "wt")) == NULL)          /* mislukt */
   {
      /* bericht samen stellen */
      strcpy(info, "\0");
      strcat(info, html_filenaam);
      strcat(info, " niet te schrijven\n");

      /* naar log schrijven */
      Write_Log(info);
   }
   else /* file is te schrijven */
   {
      /*
      // HTML regels inleiding
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "<html>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML lege regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML head begin regel(blok)
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "<head>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      if (html_mode == 1)                         /* 1 = WM-intranet stijl */
      {
         strcpy(html_regel, "\0");
         strcpy(html_regel, "  <title>WM-Intranet: GPS</title>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  <link rel=stylesheet type=\"text/css\" href=\"http://info.knmi.nl/wm/WMstyle.css\">\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  <script language=\"JavaScript1.2\" src=\"http://info.knmi.nl/wm/fw_menu.js\"> </script>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  <script language=\"JavaScript1.2\" src=\"http://info.knmi.nl/wm/WMweb.js\"> </script>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  <script language=\"JavaScript1.2\" src=\"http://info.knmi.nl/wm/WMhead.js\"> </script>\r\n");
         fprintf(os_html_location, "%s", html_regel);
      } /* if (html_mode == 1) */

      /*
      // HTML head eind regel(blok)
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "</head>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML lege regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // body regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "<body>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML lege regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "\r\n");
      fprintf(os_html_location, "%s", html_regel);

      if (html_mode == 1)                    /* 1 = WM-inranet stijl */
      {
         /*
         // script begin(blok)
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "<script>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  WMpathini(\"http://info.knmi.nl/wm/modellen/nl/mm_model_index.html\", \"Maritieme Modellen\",\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "            \"http://info.knmi.nl/wm/modellen/nl/ov_nedwam.html\", \"Beschrijving NEDWAM\",\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /* bv  "http://info.knmi.nl/~stam/gps/index.html", "GPS", */
         strcpy(html_regel, "\0");
         strcpy(env_info_knmi_gps_index, "\0");
         strcpy(env_info_knmi_gps_index, getenv("ENV_INFO_KNMI_GPS_INDEX"));              /* i.v.m. APL */
         strcpy(html_regel, "            \"");
         strcat(html_regel, env_info_knmi_gps_index);
         strcat(html_regel, "\", \"GPS\",\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /* bv "http://info.knmi.nl/~stam/gps/mpn_1.html", "mpn") */
         strcpy(html_regel, "\0");
         strcpy(env_info_knmi_gps_home, "\0");
         strcpy(env_info_knmi_gps_home, getenv("ENV_INFO_KNMI_GPS_HOME"));                 /* i.v.m. APL */
         strcpy(html_regel,  "            \"");
         strcat(html_regel, env_info_knmi_gps_home);
         strcat(html_regel, html_station);
         strcat(html_regel, "_1.html\", ");
         strcat(html_regel, "\"");
         strcat(html_regel, html_station);
         strcat(html_regel, "\")\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /* bv WMlink("http://info.knmi.nl/~stam/gps/png_charts/gps_linker_kolom.png", "http://info.knmi.nl/wm/img/modellen.gif") */
         strcpy(html_regel, "\0");
         /*strcpy(html_regel, "  WMlink(\"http://info.knmi.nl/~stam/gps/png_charts/gps_linker_kolom.png\", \"http://info.knmi.nl/wm/img/modellen.gif\")\r\n");*/
         strcpy(env_info_knmi_gps_linker_kolom, "\0");
         strcpy(env_info_knmi_gps_linker_kolom, getenv("ENV_INFO_KNMI_GPS_LINKER_KOLOM")); /* i.v.m. APL */
         strcpy(html_regel, "  WMlink(\"");
         strcat(html_regel, env_info_knmi_gps_linker_kolom);
         strcat(html_regel, "\", \"http://info.knmi.nl/wm/img/modellen.gif\")\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // script eind(blok)
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "</script>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML lege regel
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "\r\n");
         fprintf(os_html_location, "%s", html_regel);

      } /* if (html_mode == 1) */


      /*
      // HTML main-chart regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "<p><a href=index.html>main chart</a></p>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML lege regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "\r\n");
      fprintf(os_html_location, "%s", html_regel);



      /*
      // HTML regel station b.v. AUK
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "<H1>");

      if (strncmp(html_station, "nc2", 3) == 0)
         strcat(html_regel, "North Cormorant");
      else if (strncmp(html_station, "ana", 3) == 0)
         strcat(html_regel, "Anasuria");
      else if (strncmp(html_station, "auk", 3) == 0)
         strcat(html_regel, "AUK");
      else if (strncmp(html_station, "k13", 3) == 0)
         strcat(html_regel, "K13");
      else if (strncmp(html_station, "eld", 3) == 0)
         strcat(html_regel, "Eierlandse Gat");
      else if (strncmp(html_station, "smn", 3) == 0)
         strcat(html_regel, "Schiermonnikoog");
      else if (strncmp(html_station, "mun", 3) == 0)
         strcat(html_regel, "IJmuiden (munitiestortplaats)");
      else if (strncmp(html_station, "epl", 3) == 0)
         strcat(html_regel, "Euro");
      else if (strncmp(html_station, "bg2", 3) == 0)
         strcat(html_regel, "Brouwershavense Gat");
      else if (strncmp(html_station, "a12", 3) == 0)
         strcat(html_regel, "A12 Platform");
      else if (strncmp(html_station, "Q12", 3) == 0)
         strcat(html_regel, "Hoorn Platform");
      else
         strcat(html_regel, html_station);

      strcat(html_regel, "</H1>\r\n");
      fprintf(os_html_location, "%s", html_regel);



      /*
      // HTML regels observations
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <H2>observations</H2>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <UL>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /* BIJVOORBEELD: <LI><A HREF=cic_all_auk_1.html>(auk; -1hr -2hr -3hr ... -12hr) [wave spectrum]</a> */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <LI><a href=cic_all_");
      strcat(html_regel, html_station);
      strcat(html_regel, "_1.html>(");
      strcat(html_regel, html_station);
      strcat(html_regel, "; -1hr -2hr -3hr ... -12hr) [wave spectrum]</a>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      strcpy(html_regel, "\0");
      strcpy(html_regel, "  </UL>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML lege regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "\r\n");
      fprintf(os_html_location, "%s", html_regel);




      /*
      // HTML regels actual forecasts NEDWAM
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <H2>actual forecasts NEDWAM</H2>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <UL>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /* BIJVOORBEELD: <LI><a href=tsa_all_auk_1.html>(auk; 0hr +3hr +6hr ... +48hr) [wave spectrum]</a> */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <LI><a href=tsa_all_");
      strcat(html_regel, html_station);
      strcat(html_regel, "_1.html>(");
      strcat(html_regel, html_station);
      strcat(html_regel, "; 0hr +3hr +6hr ... +48hr) [wave spectrum]</a>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /* BIJVOORBEELD: <LI><a href=par_auk_1.html>(auk; 0hr +1hr +2hr ... +48hr) [hm0, ff, tm0-1]</a> */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <LI><a href=par_");
      strcat(html_regel, html_station);
      strcat(html_regel, "_1.html>(");
      strcat(html_regel, html_station);
      strcat(html_regel, "; 0hr +1hr +2hr ... +48hr) [hm0, ff, tm0-1]</a>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /* BIJVOORBEELD: <LI><a href=par_b_auk_1.html>(auk; 0hr +1hr +2hr ... +48hr) [hs10, dir hs10, hs7]</a> */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <LI><a href=par_b_");
      strcat(html_regel, html_station);
      strcat(html_regel, "_1.html>(");
      strcat(html_regel, html_station);
      strcat(html_regel, "; 0hr +1hr +2hr ... +48hr) [hs10, dir hs10, hs7]</a>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /* BIJVOORBEELD: <LI><a href=par_c_auk_1.html>(auk; 0hr +1hr +2hr ... +48hr) [height swell, dir swell, period swell]</a> */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <LI><a href=par_c_");
      strcat(html_regel, html_station);
      strcat(html_regel, "_1.html>(");
      strcat(html_regel, html_station);
      strcat(html_regel, "; 0hr +1hr +2hr ... +48hr) [height swell, dir swell, period swell]</a>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      strcpy(html_regel, "\0");
      strcpy(html_regel, "  </UL>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML lege regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "\r\n");
      fprintf(os_html_location, "%s", html_regel);



      /*
      // HTML regels previous forecasts NEDWAM
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <H2>previous forecasts NEDWAM</H2>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      strcpy(html_regel, "\0");
      strcpy(html_regel, "  <UL>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /* 7 dagen terug t.o.v. JJJJMMDDUU */
      /* JJJJMMDDUU is b.v. al 51 uur terug t.o.v. systeem tijd (de run van 48 uren terug) */
      /* dus 0 uren_terug wil zeggen 0 uren terug t.o.v. JJJJMMDDUU */
      for (uren_terug = START_UREN_TERUG_TOV_JJJJMMDDUU; uren_terug <= EIND_UREN_TERUG_TOV_JJJJMMDDUU; uren_terug += 6)
      {
         strcpy(previous_buffer, "\0");
         sprintf(previous_buffer, "%03d", uren_terug + RUN_TERUG_VOOR_TSA_FILENAAM);    /* filenamen beginnen met _48 , _54 etc. */
         previous_buffer[3] = '\0';

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  <LI><a href=tsa_");
         strcat(html_regel, html_station);
         strcat(html_regel, "_1_");
         strcat(html_regel, previous_buffer);
         strcat(html_regel, ".html>(");

         mode = 0;                            /* 0: leesbare mode (b.v. 20-12-2002 12.00 UTC) */
         /*strcat(html_regel, Determine_HTML_RunTime(uren_terug, mode, run_time));*/
         run_time[0] = '\0';
         Determine_HTML_RunTime(uren_terug, mode, run_time);
         strcat(html_regel, run_time);

         strcat(html_regel, "; ");
         strcat(html_regel, html_station);
         strcat(html_regel, "; 0hr +3hr +6hr ... +48hr) [wave spectrum]</a>\r\n");
         fprintf(os_html_location, "%s", html_regel);
      } /* for (uren_terug = START_UREN_TERUG_TOV_JJJJMMDDUU; etc. */

      strcpy(html_regel, "\0");
      strcpy(html_regel, "  </UL>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      /*
      // HTML lege regel
      */
      strcpy(html_regel, "\0");
      strcpy(html_regel, "\r\n");
      fprintf(os_html_location, "%s", html_regel);


      if (html_mode == 1)                    /* 1 = WM-inranet stijl */
      {
         /*
         // HTML script begin(blok)
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "<script>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "  WMtail(\"Updated hourly\",\"Martin Stam\", \"martin.stam@knmi.nl?subject=GPS\")\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML script eind(blok)
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "</script>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML lege regel
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "\r\n");
         fprintf(os_html_location, "%s", html_regel);

      } /* if (html_mode == 1) */


      if (html_mode == 0)                 /* 0 = geen javascript e.d. */
      {
         strcpy(html_regel, "\0");
         strcpy(html_regel, "  Updated hourly, ");
         strcat(html_regel, "  <a href=\"mailto:martin.stam@knmi.nl\">Martin.Stam</a>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML lege regel
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "\r\n");
         fprintf(os_html_location, "%s", html_regel);

      } /* if (html_mode == 0) */


      strcpy(html_regel, "\0");
      strcpy(html_regel, "</body>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      strcpy(html_regel, "\0");
      strcpy(html_regel, "</html>\r\n");
      fprintf(os_html_location, "%s", html_regel);

      fclose(os_html_location);
   } /* else (file is te schrijven) */


   return 0;
}



int Determine_HTML_RunTime(int uren_terug, int mode, char* run_time)
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
   /*char run_time[100];*/                    /* NB 100 = willekeutige groot-genoeg waarde */


   /* NB JJJJMMDDUU is niet de systeem tijd maar is bijvoorbeeld al de 48 terug run-time */
   /* dus 'uren_terug = 0' wil zeggen 0 uren terug t.o.v. JJJJMMDDUU */


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
   time_valid.tm_hour = num_UU - uren_terug;
   time_valid.tm_min = 0;
   time_valid.tm_sec = 0;
   time_valid.tm_isdst = -1;                     /* daylight saving time: -1 = no info available */

   /* 'RUN' datum aanmaken */
   tijd_ok = 1;
   if (mktime(&time_valid) == -1)
      tijd_ok = 0;

   if (tijd_ok == 1)
   {
      if (mode == 0)       /* leesbare mode (b.v. 20-12-2002 12.00 UTC) */
         (void)strftime(run_time, 100 /* voldoende grootte waarde */, "%d-%m-%Y %H.00 UTC", &time_valid);
      else                 /* mode voor file naam */
         (void)strftime(run_time, 100 /* voldoende grootte waarde */, "%Y%m%d%H", &time_valid);
   } /* if (tijd_ok == 1) */
   else
      strcpy(run_time, "unknown");


   /*return &run_time[0];*/
   return 0;
}




int Write_TSA_HTML(char* html_station/*, int frame*/)
{
   /*
   // Hier worden files geschreven zoals tsa_mun_1_048.html
   //                                    tsa_mun_1_054.html
   //                                    etc.
   */

   FILE* os_html_location;
   char html_filenaam[256];
   char info[255];
   char html_regel[255];
   char previous_buffer[4];                                 /* voor obouw html file naam */
   int uren_terug;                                          /* teller */
   int mode;
   int png_pagina;
   char char_png_pagina[2];
   char run_time[100];


   /* vanaf 2 dagen geleden een week terug; t.o.v JJJJMDDUU dus 0 - 144 hr terug */
   /* NB uren_terug is t.o.v. JJJJMMDDUU (deze geeft bv. al een run van 48 hr geleden aan) */
   /*for (uren_terug = 24; uren_terug <= 168; uren_terug += 6)*/
   for (uren_terug = START_UREN_TERUG_TOV_JJJJMMDDUU; uren_terug <= EIND_UREN_TERUG_TOV_JJJJMMDDUU; uren_terug += 6)
   {
      strcpy(previous_buffer, "\0");
      sprintf(previous_buffer, "%03d", uren_terug + RUN_TERUG_VOOR_TSA_FILENAAM);   /* bij file naam wel rekening houden met JJJJMMDDUU */
      previous_buffer[3] = '\0';

      /* bepalen tsa filenaam */
      strcpy(html_filenaam, "\0");

      if (strcmp(OS, "WINDOWS") == 0)
         /* alleen tijdens test fase */
         strcpy(html_filenaam, "C:\\program files\\BC5\\PROJECTS\\GPS\\output_nedwam\\");
         /*strcat(info, PATH_INTRANET_GPS_WINDOWS); */             /* b.v.: "i:\\info\\people\\wm-am\stam\\gps\\" */
      else
         /*strcpy(html_filenaam, PATH_INTRANET_GPS_UNIX);*/        /* b.v.: "../../../webdata/INFO/people/wm-am/stam/gps/" */
         strcpy(html_filenaam, getenv("PATH_INTRANET_GPS_UNIX"));  /* i.v.m. APL */

      strcat(html_filenaam, "tsa_");
      strcat(html_filenaam, html_station);
      strcat(html_filenaam, "_");
      strcat(html_filenaam, "1");
      strcat(html_filenaam, "_");
      strcat(html_filenaam, previous_buffer);
      strcat(html_filenaam, ".html");


      if ((os_html_location = fopen(html_filenaam, "wt")) == NULL)          /* mislukt */
      {
         /* bericht samen stellen */
         strcpy(info, "\0");
         strcat(info, html_filenaam);
         strcat(info, " niet te schrijven\n");

         /* naar log schrijven */
         Write_Log(info);
      } /* if ((os_html_location = fopen(html_filenaam, "w")) == NULL) */
      else /* file is te schrijven */
      {
         /*
         // HTML regels
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "<HTML>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "<HEAD>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*strcpy(html_regel, "\0");                                                    */
         /*strcpy(html_regel, "<META HTTP-EQUIV=\"REFRESH\" CONTENT=\"300\">\r\n");     */
         /*fprintf(os_html_location, "%s", html_regel);                                 */

         strcpy(html_regel, "\0");
         strcpy(html_regel, "<META HTTP-EQUIV=\"Cache-Control\" CONTENT=\"no-cache\">\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "<META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "</HEAD>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "<BODY  BGCOLOR=\"#000080\" VLINK=\"yellow\" LINK=\"yellow\" ALINK=\"white\">\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML lege regel
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML regels previous forecasts
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "<DIV ALIGN=\"center\">\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "<P><a href=index.html>main chart</a></P>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "</DIV>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML lege regel
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "\r\n");
         fprintf(os_html_location, "%s", html_regel);


         /* 6 verwijzingen naar png pagina's */
         for (png_pagina = 0; png_pagina < 6; png_pagina++)
         {
            /* begin paragraaf (om de plaatjes in center beeldscherm te krijgen en niet naast elkaar)*/
            strcpy(html_regel, "\0");
            strcpy(html_regel, "<DIV ALIGN=\"center\">\r\n");
            fprintf(os_html_location, "%s", html_regel);

            /* html regel met verwijzing naar plaatje */
            sprintf(char_png_pagina, "%d", png_pagina);
            char_png_pagina[1] = '\0';

            strcpy(html_regel, "\0");
            strcpy(html_regel, "<IMG src=\"./png_graphs/archive/tsa_");
            strcat(html_regel, html_station);
            strcat(html_regel, "_");
            strcat(html_regel, char_png_pagina);
            strcat(html_regel, "_");

            mode = 1;                            /* 1: file mode (b.v. 2002121200) */
            run_time[0] = '\0';
            Determine_HTML_RunTime(uren_terug, mode, run_time);
            strcat(html_regel, run_time);
            strcat(html_regel, ".png\">\r\n");

            fprintf(os_html_location, "%s", html_regel);

            /* eind paragraaf */
            strcpy(html_regel, "\0");
            strcpy(html_regel, "</DIV>\r\n");
            fprintf(os_html_location, "%s", html_regel);

            /*
            // HTML lege regel
            */
            strcpy(html_regel, "\0");
            strcpy(html_regel, "\r\n");
            fprintf(os_html_location, "%s", html_regel);

         } /* for (png_pagina = 0; png_pagina < 6; png++) */

         strcpy(html_regel, "\0");
         strcpy(html_regel, "<DIV ALIGN=\"center\">\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "<P><a href=index.html>main chart</a></P>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "</DIV>\r\n");
         fprintf(os_html_location, "%s", html_regel);


         /*
         // HTML lege regel
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "\r\n");
         fprintf(os_html_location, "%s", html_regel);

         /*
         // HTML regels
         */
         strcpy(html_regel, "\0");
         strcpy(html_regel, "</BODY>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         strcpy(html_regel, "\0");
         strcpy(html_regel, "</HTML>\r\n");
         fprintf(os_html_location, "%s", html_regel);

         fclose(os_html_location);

      } /* else (file is te schrijven) */
   } /* for (uren_terug = START_UREN_TERUG_TOV_JJJJMMDDUU etc. */


   return 0;
}




