/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                           last update 22-03-2010                           */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* LET OP ONDER SUZE LINUX GAAT BV HET VOLGENDE NIET GOED                     */
/*                                                                            */
/*             *max_energy = atof(TSA_spectrum[k][m][v]);                     */
/*                                                                            */
/* DIT VERANDEREN IN:                                                         */
/*                                                                            */
/*             sscanf(TSA_spectrum[k][m][v], "%le", &hulp_spectrum_waarde);   */
/*             *max_energy = hulp_spectrum_waarde;                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/*  NB files als CIC_LDS_DWE_2005110815.TMP worden onafhankelijk van locations.txt altijd aangemaakt                    */
/*     andere temp files bv WAVE_CID_2005110815.TMP (alleen als er ook richtingen zijn bij dat station)                 */
/*                          WAVE_HWA_2005110815.TMP (alleen als ze ook specifiek in de code staan [zie gps_hwa_input.c] */
/*                          WAVE_DSP_2005110815.TMP (alleen als er ook richtingen zijn bij dat station)                 */
/*                          NEDWAM_TSA_MPN_2005111606.TMP zijn wel afhankelijk van locations.txt                        */



#define _ANSI_C_SOURCE

#if !defined(gps_h)                                    /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>                                      /* struct time_t */
#include <stdlib.h>                                    /* exit */
#include <math.h>
#include <values.h>                                    /* MAXINT /MAXFLOAT */

#if defined(WINDOWS)                                   /* zie gps.h */
#include <dir.h>                                       /* o.a. getcwd() in windows */
#else
#include <unistd.h>                                    /* o.a. getcwd() in UNIX */
#endif


/* function prototypes in deze main module */
int Write_Log(char* bericht);
int Inlezen_Locaties_File(void);
int Bereken_BFI(double freq_array[], const int aantal_freq_array_elementen, const double fp, const double num_Tm0_1, double* BFI, double* K40, double* P, const char* filenaam, const double num_Hm0, const char* station, const double num_Hmax, const double Hmax_Hm0);
double Bepaal_Diepte(const char* station);


/* function prototypes in andere modulen */
extern int Write_Postscript_Files(void);
extern int Read_Input_Files(void);
extern int Read_TSA_Input_Files(int* inlees_result);
extern int Compute_TSA(void);
extern int Allocate_TSA_Frequenties_Array(void);                      /* dynamisch array met gebruikte (vaste) frequenties) */
extern int De_Allocate_TSA_Frequenties_Array(void);
extern int Read_Temp_File_In_TSA_Spectrum_Array(const char* TSA_tempfilenaam);
extern int Bepaal_Filenaam_TSA_Temp_File(const char* station, char* TSA_tempfilenaam, int* TSA_tempfile_aanwezig);
extern int Initial_NEDWAM_Computations(void);
extern int Allocate_NEDWAM_Arrays(void);
extern int De_Allocate_NEDWAM_Arrays(void);
extern int NEDWAM_Computations(void);
extern int Initialiseer_TSA_Spectrum_Array(void);                      /* gps_nedwam_input.c */
extern int Allocate_TSA_Spectrum_Array(void);                          /* gps_nedwam_input.c */
extern int Initialiseer_HIRLAM_Arrays(void);                           /* gps_nedwam_input.c */
extern int De_Allocate_TSA_Spectrum_Array(void);                       /* gps_nedwam_input.c */
extern int Write_TSA_Postscript_Files(const char* station);
extern int Bepaal_Hirlam_filenaam(void);
extern int Verwerk_Hirlam(void);
extern int Bepaal_Filenaam_Hirlam_Temp_File(const char* station, char* hirlam_tempfilenaam);
extern int Allocate_HIRLAM_Arrays(void);
extern int De_Allocate_HIRLAM_Arrays(void);
extern int Read_Temp_File_In_Hirlam_Arrays(const char* hirlam_tempfilenaam);
extern int Write_HTML(const char* station);
extern int Write_TSA_Polair_Postscript_Files(const char* station);
extern int Read_DSP_Input_Files(void);
extern int Write_DSP_Polair_Postscript_Files(void);
extern int Read_TMA_Input_Files(void);                                 /* gps_nedwam_tma_input.c */
extern int Write_Nedwam_TMA_UBA_Postscript_Files(void);                /* gps_nedwam_tma_ps.c */
extern int Read_UBA_Input_Files(void);                                 /* gps_hirlam_uba_input.c */
extern int Read_CID_Input_Files(void);                                 /* gps_cid_input.c */
extern int Read_HWA_Input_Files(void);                                 /* gps_hwa_input.c */

/* variables in deze module (kunnen ook in andere module(s) voorkomen) */
char OS[8];                                            /* WINDOWS of __UNIX_ */
char JJJJMMDDUU[11];                                   /* b.v. 2001080606 (run/waarneming datum-tijd zonder minuten) */
char stations[NUM_STATIONS][NUM_STATIONS_2];           /* NUM_STATIONS arrayplaatsen van elk 4 char */
char MODE[4];                                          /* b.v. CIC of TSA */
int x1;
int x2;
int x3;
int x4;
int y2;
int _y1;
int x1p;
int x2p;
int _y1p;
int y2p;
char font_keuze[50];
FILE* os_ps;
char tempfilenaam_0[256];
char tempfilenaam_1[256];
char tempfilenaam_2[256];
int xe0;                                                      /* voor polaire plotjes */
int ye0;                                                      /* voor polaire plotjes */
const int lengte_y_as        = 160;
const int lengte_x_as        = 200;
const int lengte_y_as_polair = 200;                           /* voor polaire plotjes */
const int lengte_x_as_polair = 200;                           /* voor polaire plotjes */
double max_norm_class_limit[NUMBER_ENERGY_CLASSES];           /* voor polaire plotjes */

/* externe var's */
extern double* FR;                                            /* array met (vaste) frequentie waarden */




/*****************************************************************************/
/*                                                                           */
/*                                   Main                                    */
/*                                                                           */
/*****************************************************************************/

/*
// opstart modes:
//    CIC: CIC 1-dimensionaal spectrum (LDS files + CID files swell data voor monitoring bij TMA +
//         HWA: aanvullende rtwndb-data -dd,ff,hm0,tm0-1- voor noordelijke stations)
//    DSP: CIC 2-dimensionaal spectrum (polair)
//    TSA: NEDWAM 1-dimensionaal spectrum + NEDWAM 2-dimensionaal spectrum (polair)
//    TSV: NEDWAM archief (achteraf) 1-dimensionaal spectrum + CIC archief (achteraf) 1-dimensionaal spectrum
//    TMA: NEDWAM parameters (hm0, tm0-1) + hirlam parameters (ff)
//    HTM: alleen de dynamische html pagina's worden aangemaakt (voor testen)
*/

/*
//
//
// LFE = E10
// HE3 = Hs10
//
// formules: LFE = (HE3 / 4)**     ->      HE3 = wortel(LFE) * 4          )    zelfde
//           E10 = (Hs10 / 4)**    ->      Hs10 = wortel(E10) * 4         )    zelfde
//
*/


int main (int argc, char *argv[])
{
   int inlees_result;
   int i;
   char TSA_tempfilenaam[256];
   char hirlam_tempfilenaam[256];
   int TSA_tempfile_aanwezig;
   char volledig_path[512];
   char info[1024];


   /*char buffer[10];*/

   /* in principe te gebruiken onder WINDOWS en UNIX */
   /* echter kleine verschillen soms (vooral bij path dir. en filenamen */

#if defined(WINDOWS)
   strcpy(OS, "WINDOWS");
#else
   strcpy(OS, "__UNIX_");
#endif



   /* dimensies 'main-boxen' bepalen */
   x1  = 55;                                             /* main box links */
   x2  = x1 + lengte_x_as;                               /* main box links */
   x3  = 361; /* (306 + 55) */                           /* main box rechts */
   x4  = x3 + lengte_x_as;                               /* main box rechts */
   _y1 = 50;                                             /* main box links + main box rechts */
   y2  = _y1 + lengte_y_as;                              /* main box links + main box rechts*/

   /* dimensies i.v.m. polaire plotjes */
   x1p  = 55;                                            /* polair plotje */
   x2p  = x1p + lengte_x_as_polair;                      /* polair plotje */
   _y1p = 20;                                            /* polair plotje */
   y2p  = _y1p + lengte_y_as_polair;                     /* polair plotje */


   /* oorsprong polair bepalen (xe0, ye0) */
   xe0 = x1p + (int)((double)lengte_x_as_polair / 2.0 + 0.5);
   ye0 = _y1p + (int)((double)lengte_y_as_polair / 2.0 + 0.5);


   if (argc == 3)
   {
      strcpy(JJJJMMDDUU, argv[1]);                       /* b.v. 2001080600 */
      strcpy(MODE, argv[2]);                             /* b.v. CIC, TSA */
   } /* if (argc == 3) */
   else
      exit(1);                                           /* alleen met juiste aantal arguments */


   if (strncmp(MODE, "HTM", 3) == 0)                     /* alleen dynamische html pagina's */
   {
      Inlezen_Locaties_File();
      for (i = 0; i < NUM_STATIONS; i++)
          if (strcmp(stations[i], "\0") != 0)            /* niet leeg */
             Write_HTML(stations[i]);
   } /* if (strncmp(MODE, "HTM", 3) == 0) */
   else if (strncmp(MODE, "CIC", 3) == 0)                /* CIC data (niet polair) */
   {
      /*
      // verwerken CIC LDS data
      */
      Inlezen_Locaties_File();
      Read_Input_Files();

      Read_HWA_Input_Files();                            /* aanvullende RTWNDB data (dd,ff,hm0,tm0-1) voor noordelijke stations */

      Write_Postscript_Files();

      /*
      // CID data (geintegreerde swell parameter data) wordt gebruikt bij "TMA mode"
      */
      Read_CID_Input_Files();

   } /* if (strncmp(MODE, "CIC", 3) == 0) */
   else if (strncmp(MODE, "DSP", 3) == 0)                /* CIC data polair */
   {
      /*
      // verwerken CIC DSP (cic spectrum) data (voor polaire plotjes)
      */
      Inlezen_Locaties_File();
      Read_DSP_Input_Files();
      Write_DSP_Polair_Postscript_Files();

   } /* else if (strncmp(MODE, "DSP", 3) == 0) */
   else if (strncmp(MODE, "TMA", 3) == 0)                /* nedwam hm0, tm0-1 en hirlam ff */
   {
      Inlezen_Locaties_File();
      Read_TMA_Input_Files();
      Read_UBA_Input_Files();
      Write_Nedwam_TMA_UBA_Postscript_Files();
   } /* else if (strncmp(MODE, "TMA", 3) == 0) */
   else if ( (strncmp(MODE, "TSA", 3) == 0) || (strncmp(MODE, "TSV", 3) == 0) /*|| (strncmp(MODE, "PAR", 3) == 0)*/ )  /* nedwam data (TSV = verification mode) */
   {
      /*
      // inlezen van de locaties waarvoor wat berekend moet worden
      */
      Inlezen_Locaties_File();

      /*
      // verwerken HIRLAM data (NB als de hirlam data er niet is dan gewoon door gaan)
      */
      Bepaal_Hirlam_filenaam();                          /* inputfile naam bepalen (b.v. hirver2001111218) */
      Verwerk_Hirlam();                                  /* onafh. van aantal verwachtingen (b.v. t.m +48hr) wordt alles per station verwerkt */

      /*
      // verwerken NEDWAM TSA data
      */
      Read_TSA_Input_Files(&inlees_result);              /* hier: bepalen NUM_VERWACHTINGEN etc & (via-via) Allocate_TSA_Frequenties_Array(), aanmaken temp files */

      if (inlees_result == 1)                            /* ok */
      {
         /* per locatie temp files uitlezen voor verdere berekeningen */
         for (i = 0; i < NUM_STATIONS; i++)
         {
            if (strcmp(stations[i], "\0") != 0)          /* niet leeg */
            {
               /* 1 temp files per locatie (bv.: NEDWAM_TSA_MUN_JJJJMMDDUU.TMP) uit temp directory inlezen */
               TSA_tempfile_aanwezig = 0;
               Bepaal_Filenaam_TSA_Temp_File(stations[i], TSA_tempfilenaam, &TSA_tempfile_aanwezig);

               /* als binnen function: Read_TSA_Input_Files() geen temp file voor een gevraagd station aanwezig is dan niet verder met dit station */
               if (TSA_tempfile_aanwezig == 1)
               {
                  /* reserveren geheugenruimte 4-dimensionaal array (voor alle verwachtingen) */
                  Allocate_TSA_Spectrum_Array();

                  /* initialiseren 4-dimensionaal array (voor alle verwachtingen) */
                  Initialiseer_TSA_Spectrum_Array();

                  /* deze opgehaalde temp file (weer) in TSA_spectrum inlezen (voor alle verwachtingen) */
                  Read_Temp_File_In_TSA_Spectrum_Array(TSA_tempfilenaam);

                  /* reserveren geheugenruimte arrays NEDWAM berekeningen (voor alle verwachtingen) */
                  Allocate_NEDWAM_Arrays();

                  /* diverse parameters verder berekenen (voor alle verwachtingen) */
                  Initial_NEDWAM_Computations();                        /* initialisatie berekeningen */
                  NEDWAM_Computations();                                /* berekenen 1-dim. spectrum etc. */

                  /* hirlam temp files per locatie */
                  Bepaal_Filenaam_Hirlam_Temp_File(stations[i], hirlam_tempfilenaam);

                  /* dynamische wind (DDD en FF) ruimte alloceren */
                  Allocate_HIRLAM_Arrays();

                  /* dynamische hirlam arrays initialiseren */
                  Initialiseer_HIRLAM_Arrays();

                  /* dynamische hirlam wind arrays vullen */
                  Read_Temp_File_In_Hirlam_Arrays(hirlam_tempfilenaam);

                  /* postscript files aanmaken (in TSA en TSV mode) */
                  if ( (strncmp(MODE, "TSA", 3) == 0) || (strncmp(MODE, "TSV", 3) == 0) )
                     Write_TSA_Postscript_Files(stations[i]);              /* 1-dimensionaal spectrum NEDWAM + HIRLAM data */

                  /* postscript files polaire plotjes + parameters aanmaken (niet in verificatie (TSV - achteraf) mode */
                  if (strncmp(MODE, "TSA", 3) == 0)
                  {
                     Write_TSA_Polair_Postscript_Files(stations[i]);       /* 2-dimensionaal spectrum alleen NEDWAM data */
                     /*Write_Nedwam_Par_Postscript_Files(stations[i]);*/       /* NEDWAM parameters (hm0, tm0-1 en ff-hirlam-22) */
                  }

                  /* postscript files nedwam parameters plotjes aanmaken */
                  /*if (strncmp(MODE, "PAR", 3) == 0)                  */
                  /*   Write_Nedwam_Par_Postscript_Files(stations[i]); */      /* NEDWAM parameters (hm0, tm0-1 en ff-hirlam-22) */

                  /* vrijgeven geheugenruimte arrays NEDWAM berekeningen (voor alle verwachtingen) */
                  De_Allocate_NEDWAM_Arrays();

                  /* vrijgeven geheugenruimte 4-dimensionaal array (voor alle verwachtingen) */
                  De_Allocate_TSA_Spectrum_Array();

                  /* vrijgeven geheugenruimte HIRLAM array */
                  De_Allocate_HIRLAM_Arrays();

                  /* alleen als de 'achteraf" (verificatie plaatjes gemaakt worden) html pagina's aanmaken/overschrijven */
                  if (strncmp(MODE, "TSV", 3) == 0)
                     Write_HTML(stations[i]);

               } /* if (TSA_tempfile_aanwezig == 1) */
               else /* geen TSA_tempfile_aanwezig */
               {
                  /* dus mislukt (geen TSA tussen file aanwezig voor specifiek station) */
                  getcwd(volledig_path, 512);

                  /* bericht samen stellen */
                  strcpy(info, "\0");
                  strcat(info, volledig_path);

                  if (strcmp(OS, "WINDOWS") == 0)
                     strcat(info, "\\");
                  else
                     strcat(info, "/");

                  strcat(info, TSA_tempfilenaam);
                  strcat(info, " niet aanwezig voor station: \n");
                  strcat(info, stations[i]);

                  /* naar log schrijven */
                  Write_Log(info);

               } /* else (geen TSA_tempfile_aanwezig) */
            } /* if (strcmp(stations[i], "\0") != 0) */
         } /* for (i = 0; i < NUM_STATIONS; i++) */
      } /* if (inlees_result == 1) */

      De_Allocate_TSA_Frequenties_Array();                         /* dynamisch array (was via-via function: Read_TSA_Input_Files() gealloceerd) */

   } /* else if (strncmp(MODE, "TSA", 3) == 0) */


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                                  Write_Log                                */
/*                                                                           */
/*****************************************************************************/
int Write_Log(char* bericht)
{
   FILE* log;
   int i;
   char logfilenaam[256];
   char datum[9];
   /*char jaar[5];  */
   /*char maand[3]; */
   /*char dag[3];   */

/*
   NB vanaf 21-03-2010 blijkt bij een nieuwe compilatie onderstaande niet meer te werken (alleen de dag wordt gegeven)

   time_t t;
   struct tm *tblock;


   t = time(NULL);
   tblock = localtime(&t);

   sprintf(jaar, "%04d", ((*tblock).tm_year) + 1900);
   sprintf(maand, "%02d", ((*tblock).tm_mon) + 1);
   sprintf(dag, "%02d", (*tblock).tm_mday);
*/

  time_t t;
  struct tm *tm_now;
  char buff[128];
  char datum_tijd[20];

  t = time(NULL);
  tm_now = localtime(&t);

  strftime(buff, sizeof buff, "%Y%m%d", tm_now);
  sprintf(datum_tijd, "%s\n", buff);
  datum_tijd[8]  = '\0';

  /* jaar[5]  = '\0'; */
  /* maand[3] = '\0'; */
  /* dag[3]   = '\0'; */



   /* initialisatie */
   strcpy(logfilenaam, "\0");

   /* datum samenstellen */
   strncpy(datum, JJJJMMDDUU + 0, 8);                  /* JJJJMMDD globaal aanwezig */
   datum[8] = '\0';


   /* log file naam bepalen (elke dag een nieuwe logfile) */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(logfilenaam, "log\\log.txt");
   else                                                /* UNIX */
   {
      /*strcpy(logfilenaam, "log/log.txt");*/
      strcpy(logfilenaam, getenv("ENV_GPS_LOG"));      /* ivm APL */
      strcat(logfilenaam, "_");
      /*strcat(logfilenaam, datum);*/                      /* JJJJMMDD */

/*
      strcat(logfilenaam, jaar);
      strcat(logfilenaam, maand);
      strcat(logfilenaam, dag);
*/
      strcat(logfilenaam, datum_tijd);


      strcat(logfilenaam, ".txt");                     
   }
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */


   /* log file openen */
   if ((log = fopen(logfilenaam, "a")) != NULL)
   {
      /* systeem tijd aanduiding */
      time(&t);
      fprintf(log, "%s", ctime(&t));

      /* de message */
      fprintf(log, "%s\n",  bericht);

      /* scheidingsregel */
      for (i = 0; i < 100; i++)
         fprintf(log, "*");

      fprintf(log, "\n");

      fclose(log);

   } /* if ((log = fopen(logfilenaam, "a")) != NULL) */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                           Inlezen_Locaties_File                           */
/*                                                                           */
/*****************************************************************************/
int Inlezen_Locaties_File()
{
   FILE* loc;
   char info[1024];
   char volledig_path[512];
   char regel[4];
   int regel_lengte = 3;
   char locatiesfilenaam[256];
   int teller;
   int i;
   int stoppen;


   /* initialiseer stations array */
   for (i = 0; i < NUM_STATIONS; i++)
      strcpy(stations[i], "\0");

   /* initialisatie */
   strcpy(locatiesfilenaam, "\0");

   /* locations (stations) file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                               /* WINDOWS */
      /*strcpy(locatiesfilenaam, "locations\\locations.txt");*/
      strcpy(locatiesfilenaam, "locations\\");
   else                                                          /* UNIX */
      /*strcpy(locatiesfilenaam, "locations/locations.txt");*/
      strcpy(locatiesfilenaam, getenv("ENV_GPS_LOCATIONS"));             /* i.vm. APL */

   strcat(locatiesfilenaam, "locations.txt");



   if ((loc = fopen(locatiesfilenaam, "r")) == NULL)
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

      strcat(info, locatiesfilenaam);
      strcat(info, " niet te lezen\n");

      /* naar log schrijven */
      Write_Log(info);

   } /* if ((loc = fopen(locatiesfilenaam, "r")) == NULL) */
   else
   {
      /* locaties (stations) file kan ingelezen worden */
      teller = 0;
      stoppen = 0;
      do
      {
         regel[0] = '\0';
         if (fread(regel, regel_lengte +1, 1, loc) != 0)
         {
            regel[3] = '\0';

            if (teller < NUM_STATIONS)                 /* voor de veiligheid */
               strcpy(stations[teller++], regel);
            else
               stoppen = 1;
         }
         else
            stoppen = 1;

      } while(stoppen == 0);/*while (!EOF);*/

      fclose(loc);
   } /* else */


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                            Bereken BFI                                    */
/*                                                                           */
/*****************************************************************************/
int Bereken_BFI(double freq_array[], const int aantal_freq_array_elementen, const double fp, const double num_Tm0_1,
                double* BFI, double* C4, double* P, const char* filenaam, const double num_Hm0, const char* station,
                const double num_Hmax, const double Hmax_Hm0)
{
   /* NB mode = bv CIC of NEDWAM */

   int i;
   int Epiek_indice;
   int Epiek_indice_links;
   int Epiek_indice_rechts;
   int parameters_ok;                            /* boolean */

   double w0;
   double k;
   double k0;
   double eps;
   double Epiek;
   double Epiek_links;
   double Epiek_rechts;
   double t;
   double d;
   double E0_61;
   double delta_x_links;
   double delta_x2_links;
   double delta_x_rechts;
   double delta_x2_rechts;
   double std;
   double N;
   /*double h;*/
   double periode;
   double golflengte;
   double IFP;
   double Hs_piek;
   double steepness;
   double sharpness;
   double num_fp;

   const double freq_interval = 0.01;           /* 0.01 Hz intervallen */
   const double g = 9.8;

   char buffer[256];
   char log_regel[1024];



   /*
                      Epiek
                       * ^\
                      /| | |\             / *
                     / | | |  \        /     \
                    /  | | |    \    /         \
                   /   | | y2     \/            \
                  /    | | |       *             \
                 /     | | v                      \
             -- /------|--------------------------- \---------------------------- 0.61 * Epiek
               /|< x2 >| |                              \
              /        | |delta_y_links                  * ------- *
             /         | |                           Epiek_rechts   \
            *          | v                                           \
   Epiek_links         |
                       |
   -------- |----------|----------|----------|----------|----------|
            |<-------->|                                |
            delta_x_links                               |
                                                        |
                       |<------------------------------>|
                                 delta_x_rechts



     delta_x = delta_x2            delta_x2 = delta_x  * delta_y2
     -------   --------   -->                 -------
     delta_y   delta_y2                       delta_y


     delta_x = freq_interval * aantal indices links
     delta_y = Epiek - Epiek_links
     delta_y2 = Epiek - (Epiek * 0.61)
   */



   /* parameters_ok wordt gebruikt om al of niet BFI, C4 te berkenen */
   /* voor de log print wordt ondanks dat parameters_ok = 0 toch geprint om toch info te verzamelen */


   /* initialisatie (kan later op getest worden) */
   *BFI = MAXFLOAT;
   *C4 = MAXFLOAT;
   *P = MAXFLOAT;
   golflengte = 0.0;
   IFP        = 0.0;
   Hs_piek    = 0.0;
   steepness  = 0.0;
   sharpness  = 0.0;
   parameters_ok = 1;      /* boolean */


   /*
   /////////////// de piekfrequentie kan als parameter worden doorgegeven (dan fp != MXFLOAT) of zit impliciet in de freq. array (dan fp == MAXFLOAT)
   */
   if (fp == MAXFLOAT)
   {
      num_fp = 0.0;
      Epiek = 0.0;

      for (i = 0; i < aantal_freq_array_elementen; i++)
      {
         if (freq_array[i] > Epiek)
         {
            Epiek = freq_array[i];
            num_fp = i * freq_interval;           /* stapjes van 10 mHz, indice 0 = 0 mHz; indice 1 = 1 * 10 mHz; indice 2 = 2 * 10 mHz etc. */
         }
      } /* for (int i = 0; i < aantal_freq_array_elementen; i++) */
   }
   else
      num_fp = fp;


   /*
   ////////////// check op num_fp
   */
   if (num_fp < 0.001)
      parameters_ok = 0;


   /*
   ////////// golfgetal berekenen
   */
   if (parameters_ok == 1)
   {
      d = Bepaal_Diepte(station);            /* bv 18m voor Schiermonnikoog */

      /* diepte check */
      if (d < 0 || d > 500)                  /* er is wat fout met de opgehaalde diepte */
         parameters_ok = 0;

      w0 = TWEE_PI * num_fp;                 /* TWEE_PI = 2 * PI */
      k0 = w0 * w0 / g;

      eps = 1;
      k = k0;
      while (eps > 0.000001)
      {
         t = tanh(d * k);
         eps = fabs(k0 - k * t);
         k = k0 / t;
      } /* while (eps > 0.000001) */
   } /* if (parameters_ok == 1) */


   /*
   ///////// standaard deviatie piek berekenen
   */
   Epiek = 0.0;
   Epiek_indice = 0;

   for (i = 0; i < aantal_freq_array_elementen; i++)
   {
      if (freq_array[i] > Epiek)
      {
         Epiek = freq_array[i];
         Epiek_indice = i;
      }
   } /* for (int i = 0; i < aantal_freq_array_elementen; i++) */


   /* Energy op 0.61 van Epiek */
   E0_61 = Epiek * 0.61;



   /* linker deel piek (10 is een willekeurige waarde die groot genoeg is) */
   Epiek_indice_links = 0;
   Epiek_links        = 0.0;
   delta_x_links      = 0.0;
   delta_x2_links     = 0.0;

   for (i = 1; i < 10; i++)
   {
      if (Epiek_indice >= i)
      {
         if (E0_61 > freq_array[Epiek_indice - i])
         {
            Epiek_indice_links = Epiek_indice - i;
            Epiek_links = freq_array[Epiek_indice_links];
            delta_x_links = freq_interval * i;

            if ((Epiek - Epiek_links) * (Epiek - E0_61) != 0)
               delta_x2_links = (delta_x_links / (Epiek - Epiek_links)) * (Epiek - E0_61);

            break;
         }
      } /* if (Epiek_indice >= i) */
   } /* for (i = 1; i < 10; i++) */

   /* rechter deel piek (10 is een willekeurige waarde die groot genoeg is) */
   Epiek_indice_rechts = 0;
   Epiek_rechts        = 0.0;
   delta_x_rechts      = 0.0;
   delta_x2_rechts     = 0.0;

   for (i = 1; i < 10; i++)
   {
      if (Epiek_indice <= (aantal_freq_array_elementen - 1) - i)   /* als b.v aantal_freq_array_elementen = 40 dan lopen de indices van 0 - 39 */
      {
         if (E0_61 > freq_array[Epiek_indice + i])
         {
            Epiek_indice_rechts = Epiek_indice + i;
            Epiek_rechts = freq_array[Epiek_indice_rechts];
            delta_x_rechts = freq_interval * i;

            if ((Epiek - Epiek_rechts) * (Epiek - E0_61) != 0)
               delta_x2_rechts = (delta_x_rechts / (Epiek - Epiek_rechts)) * (Epiek - E0_61);

            break;
         }
      } /* if (Epiek_indice <= (aantal_freq_array_elementen - 1) - i) */
   } /* for (i = 1; i < 10; i++) */

   std = 0.0;
   if (delta_x2_links != 0.0 && delta_x2_rechts != 0.0)
      std = (delta_x2_links + delta_x2_rechts) / 2.0;


   /*
   // N (aantal golven) voor later de kans op freak waves te kunnen berkenen
   */
   periode = 20.0;                                    /* periode in minuten; nemen 20 minuten aan */
   N = periode * (60.0 / num_Tm0_1);                  /* num_Tm0_1 in sec */



   /*
   ///////////// BFI en C4
   */
   if (parameters_ok == 1)
   {

      /*BFI = 2.2390 * k * (TWEE_PI * fp) / sqrt(std * TWEE_PI) * sqrt(Epiek / TWEE_PI);*/
      if (std != 0.0)
         *BFI = 2.2390 * (k * num_fp / sqrt(std)) * sqrt(Epiek);

      if (*BFI != MAXFLOAT)
      {
#if 0
   //      /* zie mail van Peter Janssen -> niet K40 maar C4 gebruiken ! */
   //      /*
   //      ////////////  kurtosis
   //      */
   //      *K40 = PI / sqrt(3.0) * (*BFI * *BFI);
   //
   //
   //      /*
   //      /////////// kans op freak waves (Hmax > 2 * Hm0)
   //      */
   //
   //      /* bv 20 minuten periode 15s golven geeft N = 80 */
   //
   //      h = 2.0;                                           /* dus Hmax > 2 * hs */
   //      periode = 20.0;                                    /* periode in minuten; nemen 20 minuten aan */
   //      N = periode * (60.0 / num_Tm0_1);                  /* num_Tm0_1 in sec */
   //
   //      *P = N * (1.0 + (2.0 /3.0) * *K40 * h * h * (h * h - 1.0)) * exp(-2.0 * h * h);
#endif
         *C4 = PI / (3 * sqrt(3.0)) * (*BFI * *BFI);
         /* deze C4 kan dan niet in de eerder gebruikte P (kans) formule */


      } /* if (*BFI != MAXFLOAT) */
   } /* if (parameters_ok == 1) */



   /*
   //////// als controle diverse interessante parameters geven (alleen voor de tussenresultaat files; worden verder niet gebruikt)
   */
   if (parameters_ok == 1)
   {
      /* voor initialisatie , zie begin van deze function */
      golflengte = (2.0 * PI) / k;
      if (*BFI != MAXFLOAT)
      {
         IFP = sqrt(TWEE_PI) * (std * TWEE_PI) * (Epiek / TWEE_PI);   /* IFP = integraal frequentiespectrum piek */
         Hs_piek = 4 * sqrt(IFP);
         steepness = 0.25 * k * Hs_piek;
         sharpness = std / num_fp;
      } /* if (*BFI != MAXFLOAT) */
   } /* if (parameters_ok == 1) */



   /*
   /////////////////// resultaten ook voor de test naar log schrijven
   */
   log_regel[0] = '\0';

   /*strcpy(log_regel, mode);  */                            /* CIC of NEDWAM */
   /*strcat(log_regel, " ");   */

   strcat(log_regel, station);
   strcat(log_regel, " ");

   strcat(log_regel, "file=");
   strcat(log_regel, filenaam);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "diepte=");
   sprintf(buffer, "%.2f", d);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "Hm0=");
   sprintf(buffer, "%.2f", num_Hm0);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "fp=");
   sprintf(buffer, "%.2f", num_fp);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "golfgetal=");
   sprintf(buffer, "%.2f", k);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "golflengte=");
   sprintf(buffer, "%.1f", golflengte);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "BFI=");
   if (*BFI != MAXFLOAT)
   {
      sprintf(buffer, "%.2f", *BFI);
      strcat(log_regel, buffer);
   }
   else
      strcat(log_regel, "-");
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "C4=");
   if (*C4 != MAXFLOAT)
   {
      sprintf(buffer, "%.2f", *C4);
      strcat(log_regel, buffer);
   }
   else
      strcat(log_regel, "-");
   strcat(log_regel, " ");

#if 0
//   buffer[0] = '\0';
//   strcat(log_regel, "P20(Hmax>2*Hm0)=");
//   if (*P != MAXFLOAT)
//   {
//      sprintf(buffer, "%.2f", *P);
//      strcat(log_regel, buffer);
//   }
//   else
//      strcat(log_regel, "-");
//   strcat(log_regel, " ");
#endif


   buffer[0] = '\0';
   strcat(log_regel, "N20=");
   /*if (*P != MAXFLOAT)        */                     /* dan was ook N ok */
   /*{                          */
      sprintf(buffer, "%.0f", N);
      strcat(log_regel, buffer);
   /*}*/
   /*else                      */
   /*   strcat(log_regel, "-");*/
   strcat(log_regel, " ");


   buffer[0] = '\0';
   strcat(log_regel, "Epiek=");
   sprintf(buffer, "%.2f", Epiek);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "Epiek_indice=");
   sprintf(buffer, "%d", Epiek_indice);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   buffer[0] = '\0';
   strcat(log_regel, "E0_61=");
   sprintf(buffer, "%.2f", E0_61);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   if (Epiek_indice_links != 0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "Epiek_indice_links=");
      sprintf(buffer, "%d", Epiek_indice_links);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (Epiek_links != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "Epiek_links=");
      sprintf(buffer, "%.2f", Epiek_links);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (delta_x_links != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "delta_x_links=");
      sprintf(buffer, "%.3f", delta_x_links);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (delta_x2_links != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "delta_x2_links=");
      sprintf(buffer, "%.3f", delta_x2_links);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }   

   if (Epiek_indice_rechts != 0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "Epiek_indice_rechts=");
      sprintf(buffer, "%d", Epiek_indice_rechts);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (Epiek_rechts != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "Epiek_rechts=");
      sprintf(buffer, "%.2f", Epiek_rechts);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (delta_x_rechts != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "delta_x_rechts=");
      sprintf(buffer, "%.3f", delta_x_rechts);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (delta_x2_rechts != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "delta_x2_rechts=");
      sprintf(buffer, "%.3f", delta_x2_rechts);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   buffer[0] = '\0';
   strcat(log_regel, "std=");
   sprintf(buffer, "%.3f", std);
   strcat(log_regel, buffer);
   strcat(log_regel, " ");

   if (IFP != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "IFP=");
      sprintf(buffer, "%.2f", IFP);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (Hs_piek != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "Hs_piek=");
      sprintf(buffer, "%.2f", Hs_piek);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (steepness != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "steepness_peak=");
      sprintf(buffer, "%.2f", steepness);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (sharpness != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "sharpness_peak=");
      sprintf(buffer, "%.2f", sharpness);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (num_Hmax != MAXFLOAT && num_Hmax != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "Hmax=");
      sprintf(buffer, "%.2f", num_Hmax);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }

   if (Hmax_Hm0 != MAXFLOAT && num_Hmax != 0.0 && Hmax_Hm0 != 0.0)
   {
      buffer[0] = '\0';
      strcat(log_regel, "Hmax/Hm0=");
      sprintf(buffer, "%.2f", Hmax_Hm0);
      strcat(log_regel, buffer);
      strcat(log_regel, " ");
   }


   /* naar log file */
   Write_Log(log_regel);


   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                             Bepaal_Diepte                                 */
/*             (NB in spectra_main.c staat ook zo'n function)                */
/*                                                                           */
/*****************************************************************************/
double Bepaal_Diepte(const char* station)
{
   double DPT;

   /* NB zie file locations.txt voor de stations waarvoor een waterdiepte aanwezig moet zijn */
   /* dit is dus voor minder locaties dan in spectra_main.c, alleen voor nc1/nc2 en q11/Q12 uitzondering? */
   /* wordt gecheckt op 3 char van station maar nc en q1 hebben er in principe maar 2) */

   if (strncmp(station, "NC1", 3) == 0)
      DPT = NC1_DIEPTE;
   else if (strncmp(station, "NC2", 3) == 0)
      DPT = NC2_DIEPTE;
   else if (strncmp(station, "ANA", 3) == 0)
      DPT = ANA_DIEPTE;
   else if (strncmp(station, "AUK", 3) == 0)
      DPT = AUK_DIEPTE;
   else if (strncmp(station, "K13", 3) == 0)
      DPT = K13_DIEPTE;
   else if (strncmp(station, "ELD", 3) == 0)
      DPT = ELD_DIEPTE;
   else if (strncmp(station, "SMN", 3) == 0)
      DPT = SMN_DIEPTE;
   else if (strncmp(station, "IJ5", 3) == 0)
      DPT = IJ5_DIEPTE;
   else if (strncmp(station, "MUN", 3) == 0)
      DPT = MUN_DIEPTE;
   else if (strncmp(station, "MPN", 3) == 0)
      DPT = MPN_DIEPTE;
   else if (strncmp(station, "EPL", 3) == 0)
      DPT = EPL_DIEPTE;
   else if (strncmp(station, "DWE", 3) == 0)
      DPT = DWE_DIEPTE;
   else if (strncmp(station, "LEG", 3) == 0)
      DPT = LEG_DIEPTE;
   else if (strncmp(station, "E13", 3) == 0)
      DPT = E13_DIEPTE;
   else if (strncmp(station, "Q11", 3) == 0)
      DPT = Q11_DIEPTE;
   else if (strncmp(station, "Q12", 3) == 0)
      DPT = Q12_DIEPTE;
   else if (strncmp(station, "BG2", 3) == 0)
      DPT = BG2_DIEPTE;
   else if (strncmp(station, "A12", 3) == 0)
      DPT = A12_DIEPTE;
   else
      DPT = 10.0;


   return DPT;
}






