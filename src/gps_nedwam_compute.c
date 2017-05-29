#if !defined(gps_h)                         /* Sentry use file only if it's not already included. */
#include "gps.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>                           /* o.a. cos, sin */
#include <values.h>                         /* MAXFLOAT */

#if defined(WINDOWS)                        /* zie gps.h */
#include <dir.h>                            /* o.a. getcwd() in windows */
#else
#include <unistd.h>                         /* o.a. getcwd() in UNIX */
#endif


/* function prototypes in andere module */
extern int Write_Log(char* bericht);                   /* gpsmain.c */
extern int Inlezen_Locaties_File(void);                /* gpsmain.c */
extern int Initialiseer_TSA_Spectrum_Array(void);      /* gps_nedwam_input.c */
extern int Allocate_TSA_Spectrum_Array(void);          /* gps_nedwam_input.c */
extern int De_Allocate_TSA_Spectrum_Array(void);       /* gps_nedwam_input.c */

/* functions in deze module */
int Read_Temp_File_In_TSA_Spectrum_Array(const char* TSA_tempfilenaam);
int Bepaal_Filenaam_TSA_Temp_File(const char* station, char* TSA_tempfilenaam, int* TSA_tempfile_aanwezig);
/*int Bepaal_Filenaam_TSA_Temp_File(const char* station, char* TSA_tempfilenaam);*/
int Initial_NEDWAM_Computations(void);
int Allocate_NEDWAM_Arrays(void);
int De_Allocate_NEDWAM_Arrays(void);
int NEDWAM_Computations(void);

/* externe var's */
extern char JJJJMMDDUU[11];                         /* via argument list */
extern char OS[8];
extern char stations[NUM_STATIONS][NUM_STATIONS_2]; /* NUM_STATIONS arrayplaatsen van elk 4 char */
extern char**** TSA_spectrum;
extern int NUM_RICHTINGEN;
extern int NUM_FREQUENTIES;
extern int NUM_VERWACHTINGEN;
extern double CO;                                   /* uit gps_nedwam_tsa_input.c */
extern double* FR;                                  /* uit gps_nedwam_tsa_input.c */

/* var's globaal binnen deze module */
double DELTH;                                       /* NEDWAM berekeningen */
double* DFIM;                                       /* NEDWAM berekeningen */
double* COSTH;                                      /* NEDWAM berekeningen */
double* SINTH;                                      /* NEDWAM berekeningen */
double* TH;                                         /* NEDWAM berekeningen */
double* EMEAN;                                      /* NEDWAM berekeningen */
double** EMN1DIM;                                   /* NEDWAM berekeningen */
double* E10;                                        /* NEDWAM berekeningen */
double** SISP;                                      /* NEDWAM berekeningen */
double** CISP;                                      /* NEDWAM berekeningen */
double* SI;                                         /* NEDWAM berekeningen */
double* CI;                                         /* NEDWAM berekeningen */
double** THQSP;                                     /* NEDWAM berekeningen */
double* THQ;                                        /* NEDWAM berekeningen */
double* E10D;                                       /* NEDWAM berekeningen */
double* FMEAN;                                      /* NEDWAM berekeningen */
double* TEMP2;                                      /* NEDWAM berekeningen */



/*****************************************************************************/
/*                                                                           */
/*                    Bepaal_Filenaam_TSA_Temp_File                          */
/*                                                                           */
/*****************************************************************************/
int Bepaal_Filenaam_TSA_Temp_File(const char* station, char* TSA_tempfilenaam, int* TSA_tempfile_aanwezig)
{
   FILE* temp;


   /* filenaam bv.: NEDWAM_TSA_MUN_JJJJMMDDUU.TMP */

   /* initialisatie */
   strcpy(TSA_tempfilenaam, "\0");

   /* eerste deel tempfile file naam bepalen */
   if (strcmp(OS, "WINDOWS") == 0)                     /* WINDOWS */
      strcpy(TSA_tempfilenaam, "temp_nedwam\\");
   else                                                /* UNIX */
      strcpy(TSA_tempfilenaam, "temp_nedwam/");
   /* n.b. onder windows met unix manier gaat ook wel goed, maar niet andersom ! */



   /* file naam TSA temp file */
   strcat(TSA_tempfilenaam, "NEDWAM_TSA_");
   strcat(TSA_tempfilenaam, station);                  /* bv: MUN */
   strcat(TSA_tempfilenaam, "_");
   strcat(TSA_tempfilenaam, JJJJMMDDUU);
   strcat(TSA_tempfilenaam, ".TMP");

/*****************************************************/
   /* testen of de temp file wel aanwezig is */
   if ((temp = fopen(TSA_tempfilenaam, "r")) != NULL)
   {
      *TSA_tempfile_aanwezig = 1;
      fclose(temp);
   }
   else
   {
      *TSA_tempfile_aanwezig = 0;
   }


   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                  Read_Temp_File_In_TSA_Spectrum_Array                     */
/*                                                                           */
/*****************************************************************************/
int Read_Temp_File_In_TSA_Spectrum_Array(const char* TSA_tempfilenaam)
{
   FILE* temp;
   char regel[NUM_REALS + 1];
   int regel_lengte = NUM_REALS;
   int m;                                /* teller frequenties   */
   int k;                                /* teller richtingen    */
   int v;                                /* teller verwachtingen */
   /*char buffer[10];*/

   /* NB ze zijn in temp file geschreven m.b.v. strlen (geen eol) en niet met sizeof (zie function: Write_TSA_Temp_File())*/


   if ((temp = fopen(TSA_tempfilenaam, "r")) != NULL)           /* dus gelukt */
   {
      /* inlezen temp file */
      for (k = 0; k < NUM_RICHTINGEN; k++)
         for (m = 0; m < NUM_FREQUENTIES; m++)
            for (v = 0; v < NUM_VERWACHTINGEN; v++)
            {
               if (fread(regel, regel_lengte, 1, temp) != 0)
               {
                  regel[regel_lengte] = '\0';



                      /**************  test ************/
                      /*
                     fprintf(stderr, "%s", regel);
                     getchar();
                     /*
                     /**************** test ************/

                  /*strcpy(TSA_spectrum[k][m][v], regel);*/
                  /* nb: strcpy(TSA_spectrum[k][m][v], regel); --> run-time error */
                  strncpy(TSA_spectrum[k][m][v], regel, LENGTE_NEDWAM_ENERGY_WAARDE);


/**************  test ************/
/*
               sprintf(buffer, "%d", k);
               fprintf(stderr, "%s", buffer);
               getchar();
*/
/**************** test ************/


/**************  test ************/
/*
               sprintf(buffer, "%d", m);
               fprintf(stderr, "%s", buffer);
               getchar();
*/
/**************** test ************/

/**************  test ************/
/*
               sprintf(buffer, "%d", v);
               fprintf(stderr, "%s", buffer);
               getchar();
*/
/**************** test ************/

/**************  test ************/
/*
                     fprintf(stderr, "%s",TSA_spectrum[k][m][v] );
                     getchar();
*/
/**************** test ************/


               } /* if (fread(regel, regel_lengte +1, 1, temp) != NULL) */


            } /* for (m = 0; m < NUM_VERWACHTINGEN; m++) */

      fclose(temp);

   } /* if ((temp = fopen(TSA_tempfilenaam, "r")) != NULL) */

   return 0;
}





/*****************************************************************************/
/*                                                                           */
/*                     TEST                                                  */
/*                                                                           */
/*****************************************************************************/
/*
//int Bepaal_Richting_Per_Frequentie(station);
//{
//   // template: TSA_SPECTRUM[NUM_RICHTINGEN][NUM_FREQUENTIES][NUM_VERWACHTINGEN][NUM_REALS]
//
//   int i;
//   int k;
//   int m;
//
//
//
//
//   // moet per frequentie een richting worden
//   for (i = 0; i < NUM_RICHTINGEN, i++)
//   {
//      for (k = 0; k < NUM_FREQUENTIES, k++)
//      {
//         for (m = 0; m < NUM_VERWACHTINGEN; m++)
//         {
//            gem_richting_per_frequentie[k][m] = gem_richting_per_frequentie[k][m] + TSA_spectrum[i][k][m],
//         }
//      }
//   }
//
//   return 0;
//}
*/



/*****************************************************************************/
/*                                                                           */
/*                            NEDWAM_computations                            */
/*                                                                           */
/*****************************************************************************/
int NEDWAM_Computations()
{
   /* template: TSA_SPECTRUM[NUM_RICHTINGEN][NUM_FREQUENTIES][NUM_VERWACHTINGEN][NUM_REALS] */
   int NFRE = NUM_FREQUENTIES;
   int NANG = NUM_RICHTINGEN;
   int m;                           /* teller frequenties   */
   int k;                           /* teller richtingen    */
   int v;                           /* teller verwachtingen */
   double* temp;                    /* hulp array */
   int M10;
   double A;
   double DELTH2;
   double SIH;
   double COH;
   double DF;
   double FD;
   double DELT25;
   /*char buffer[10];*/
   double SSI;
   double SCO;



   /*
   //
   // Integrate over frequencies and direction incl. 1-DIM spectrum
   // (bereken variance density; linker paneel ps pagina)
   //
   */

   /* reserveren geheugenruimte hulp array */
   temp = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double));  /* cleared to 0 */

   for (m = 0; m < NFRE; m++)                      /* NFRE = number of frequencies */
   {
      /* initiallisatie */
      for (v = 0; v < NUM_VERWACHTINGEN; v++)
         temp[v] = 0;

      for (k = 0; k < NANG; k++)                   /* NANG = number of angles (richtingen) */
      {
         for (v = 0; v < NUM_VERWACHTINGEN; v++)
         {
            temp[v] = temp[v] + atof(TSA_spectrum[k][m][v]);
         } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
      } /* for (k = 0; k < NANG; k++) */

      for (v = 0; v < NUM_VERWACHTINGEN; v++)
      {
         EMN1DIM[m][v] = DELTH * temp[v];
         EMEAN[v]      = EMEAN[v] + DFIM[m] * temp[v];
      } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */

   } /* for (m = 0; m < NFRE; m++) */

   /* vrijgeven geheugenruimte hulp array */
   free(temp);



   /*
   //
   // Calculate E10 parameter (E10 = LFE)
   //
   */
   M10 = 0;
   for (m = 0; m < NFRE; m++)
   {
      if (FR[m] < 0.10)
         M10 = M10 + 1;
   } /* for (m = 0, m < NFRE; m++)*/


   for (k = 0; k < NANG; k++)
   {
      for (m = 0; m < M10; m++)
         for (v = 0; v < NUM_VERWACHTINGEN; v++)
            E10[v] = E10[v] + DFIM[m] * atof(TSA_spectrum[k][m][v]);
   } /* for (k = 0; K < NANG; k++) */


   /*
   //
   // Initialize Sin and Cos arrays
   //
   */
   for (m = 0; m < NFRE; m++)
   {
      for (v = 0; v < NUM_VERWACHTINGEN; v++)
      {
         SISP[m][v] = 0.0;
         CISP[m][v] = 0.0;

         SI[v] = 0.0;
         CI[v] = 0.0;
      } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
   } /* for (m = 0; m < NFRE; m++) */

   for (v = 0; v < NUM_VERWACHTINGEN; v++)
   {
      SI[v] = 0.0;
      CI[v] = 0.0;
   } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */




#if 0
//   /*
//   //
//   // Integrate over frequencies and directions
//   //
//   */
//   for (m = 0; m < NFRE; m++)
//   {
//      for (k = 0; k < NANG; k++)
//      {
//         for (v = 0; v < NUM_VERWACHTINGEN; v++)
//         {
//            A = atof(TSA_spectrum[k][m][v]) * DFIM[m];
//            SISP[m][v] = SISP[m][v] + SINTH[k] * A;
//            CISP[m][v] = CISP[m][v] + COSTH[k] * A;
//         }
//      } /* for (k = 0; k < NANG; k++) */
//   } /* for (m = 0; m < NFRE; m++) */
//
//   for (m = 0; m < NFRE; m++)
//   {
//      for (v = 0; v < NUM_VERWACHTINGEN; v++)
//      {
//         SI[v] = SI[v] + SISP[m][v];
//         CI[v] = CI[v] + CISP[m][v];
//      }
//   } /* for (m = 0; m < NFRE; m++) */
#endif


   /*
   //
   // Integrate over frequencies and directions
   //
   */
   for (v = 0; v < NUM_VERWACHTINGEN; v++)
   {
      for (m = 0; m < NFRE; m++)
      {
         for (k = 0; k < NANG; k++)
         {
            A = atof(TSA_spectrum[k][m][v]) * DFIM[m];
            SISP[m][v] = SISP[m][v] + SINTH[k] * A;
            CISP[m][v] = CISP[m][v] + COSTH[k] * A;
         } /* for (k = 0; k < NANG; k++) */

         SI[v] = SI[v] + SISP[m][v];
         CI[v] = CI[v] + CISP[m][v];
      } /* for (m = 0; m < NFRE; m++) */
   } /* for (m = 0; m < NFRE; m++) */


#if 0
//   /*
//   //
//   // Compute mean direction for freq. and for total spectrum
//   //
//   */
//   for (m = 0; m < NFRE; m++)
//   {
//      for (v = 0; v < NUM_VERWACHTINGEN; v++)
//      {
//         if (CISP[m][v] == 0.0)
//            THQSP[m][v] = MAXFLOAT;
//         else
//         {
//            THQSP[m][v] = atan2(SISP[m][v], CISP[m][v]);
//            if (THQSP[m][v] < 0.0)
//               THQSP[m][v] = THQSP[m][v] + ZPI;
//         }
//      } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
//   } /* for (m = 0; m < NFRE; m++) */
//
//   for (v = 0; v < NUM_VERWACHTINGEN; v++)
//   {
//      if (CI[v] == 0.0)
//         THQ[v] = MAXFLOAT;
//      else
//      {
//         THQ[v] = atan2(SI[v], CI[v]);
//         if (THQ[v] < 0.0)
//            THQ[v] = THQ[v] + ZPI;
//      }
//   } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
#endif


   /*
   //
   // Compute mean direction for freq. and for total spectrum
   //
   */
   for (v = 0; v < NUM_VERWACHTINGEN; v++)
   {
      for (m = 0; m < NFRE; m++)
      {
         if (CISP[m][v] == 0.0)
            THQSP[m][v] = MAXFLOAT;
         else
         {
            THQSP[m][v] = atan2(SISP[m][v], CISP[m][v]);
            if (THQSP[m][v] < 0.0)
              THQSP[m][v] = THQSP[m][v] + ZPI;
         }
      } /* for (m = 0; m < NFRE; m++) */
      if (CI[v] == 0.0)
         THQ[v] = MAXFLOAT;
      else
      {
         THQ[v] = atan2(SI[v], CI[v]);
         if (THQ[v] < 0.0)
            THQ[v] = THQ[v] + ZPI;
      }
   } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */


#if 0
//   /*
//   //
//   // Compute E10 direction
//   //
//   */
//   M10 = 0;
//   for (m = 0; m < NFRE; m++)
//   {
//      if (FR[m] < 0.10)
//         M10 = M10 + 1;
//  } /* for (m = 0; m < NFRE; m++) */
//
//   DELTH2 = DELTH / 2.0;
//   for (k = 0; k < NANG; k++)
//   {
//      for (v = 0; v < NUM_VERWACHTINGEN; v++)
//      {
//         SSI[v] = 0.0;
//         SCO[v] = 0.0;
//      }
//
//      SIH = SINTH[k];
//      COH = COSTH[k];
//
//      for (m = 0; m < M10; m++)
//      {
//         if (m > 0)
//            DF = (FR[m + 1] + FR [m - 1]) * DELTH2;
//         else
//            DF = (FR[m + 1] + FR [m]) * DELTH2;
//
//         for (v = 0; v < NUM_VERWACHTINGEN; v++)
//         {
//            SSI[v] = SSI[v] + atof(TSA_spectrum[k][m][v]) * SIH * DF;
//            SCO[v] = SCO[v] + atof(TSA_spectrum[k][m][v]) * COH * DF;
//         } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
//      } /* for (m = 0; m < M10; m++) */
//   } /* for (k = 0; K < NANG; k++) */
//
//   for (v = 0; v < NUM_VERWACHTINGEN; v++)
//   {
//      if (SCO[v] == 0.0)
//         E10D[v] = MAXFLOAT;
//      else
//      {
//         E10D[v] = atan2(SSI[v], SCO[v]);
//         if (E10D[v] < 0.0)
//            E10D[v] = E10D[v] + ZPI;
//      }
//   } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
#endif


   /*
   //
   // Compute E10 direction
   //
   */
   M10 = 0;
   DELTH2 = DELTH / 2.0;
   for (m = 0; m < NFRE; m++)
   {
      if (FR[m] < 0.10)
         M10 = M10 + 1;
   } /* for (m = 0; m < NFRE; m++) */

   for (v = 0; v < NUM_VERWACHTINGEN; v++)
   {
      SSI = 0.0;
      SCO = 0.0;

      for (k = 0; k < NANG; k++)
      {
         SIH = SINTH[k];
         COH = COSTH[k];

         for (m = 0; m < M10; m++)
         {
            if (m > 0)
               DF = (FR[m + 1] + FR[m - 1]) * DELTH2;
            else
               DF = (FR[m + 1] + FR[m]) * DELTH2;

               SSI = SSI + atof(TSA_spectrum[k][m][v]) * SIH * DF;
               SCO = SCO + atof(TSA_spectrum[k][m][v]) * COH * DF;
         }

         if (SCO == 0.0)
            E10D[v] = MAXFLOAT;
         else
         {
            E10D[v] = atan2(SSI, SCO);
            if (E10D[v] < 0.0)
               E10D[v] = E10D[v] + ZPI;
         }
      }
   }




   /*
   //
   //  Initialise mean frequency array
   //
   */
   for (v = 0; v < NUM_VERWACHTINGEN; v++)
      FMEAN[v] = 0.0;
   DELT25 = 0.20 * DELTH;

   /*
   //
   // Intergrate over frequencies and directions: FMEAN (= 1 / Tmo-1)
   //
   */
   for (m = 0; m < NFRE; m++)
   {
      FD = DFIM[m] / FR[m];

      /* initialisatie */
      for (v = 0; v < NUM_VERWACHTINGEN; v++)
         TEMP2[v] = 0.0;

      for (k = 0; k < NANG; k++)
      {
         for (v = 0; v < NUM_VERWACHTINGEN; v++)
         {
            TEMP2[v] = TEMP2[v] + atof(TSA_spectrum[k][m][v]);
         } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
      } /* for (k = 0; k < NANG; k++) */

      for (v = 0; v < NUM_VERWACHTINGEN; v++)
      {
         FMEAN[v] = FMEAN[v] + FD * TEMP2[v];  /* NB TM0-1 = 1 / FMEAN */
      } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */
   } /* for (m = 0; m < NFRE; m++) */



   /* add tail correction to mean frequency and normalize with total energy */
   for (v = 0; v < NUM_VERWACHTINGEN; v++)
   {
      FMEAN[v] = FMEAN[v] + DELT25 * TEMP2[v];

      if (FMEAN[v] != 0.0)                        /* martin */
         FMEAN[v] = EMEAN[v] / FMEAN[v];
      else
         FMEAN[v] = MAXFLOAT;   
   } /* for (v = 0; v < NUM_VERWACHTINGEN; v++) */



   return 0;
}



/*****************************************************************************/
/*                                                                           */
/*                         Initial_NEDWAM_Computations                       */
/*                                                                           */
/*****************************************************************************/
int Initial_NEDWAM_Computations()
{
   int KL = NUM_RICHTINGEN;
   int ML = NUM_FREQUENTIES;
   double TH0 = (360 / NUM_RICHTINGEN) * 0.5;   /* b.v. (360 / 12) * 0.5 = 15 */
   double CO1;
   int k;                                       /* teller richtingen */
   int m;                                       /* teller frequenties */


   /* NB i.v.m. arrays:                                                 */
   /*                                                                   */
   /* DELTH = 2                                                         */
   /* KL = 3                                                            */
   /*                                                                   */
   /* FORTRAN MANIER (array begint bij indice 1) (pseudo fortran code)  */
   /* for (k = 1; k <= KL; k++)                                         */
   /*   TH[k] = (k - 1) * DELTH   --> TH[1] = 0; TH[2] = 2; TH[3] = 4   */
   /*                                                                   */
   /* C MANIER (array begint bij indice 0)                              */
   /* for (k = 1; k < KL; k++)                                          */
   /*   TH[k] = (k) * DELTH       --> TH[0] = 0; TH[1] = 2; TH[2] = 4   */



   DELTH = ZPI / (double)KL;           /* b.v. 2 * pi / 12 = 0.523599 */
   for (k = 0; k < KL; k++)
   {
      TH[k] = k * DELTH + RAD * TH0;   /* shift angular discretization by TH0 with respect to spatial grid */
      COSTH[k] = cos(TH[k]);
      SINTH[k] = sin(TH[k]);
   } /* for (k = 0; k < KL; k++) */


   CO1 = 0.5 * (CO - 1.0) * DELTH;

   DFIM[0] = CO1 * FR[0];
   for (m = 1; m < ML - 1; m++)
      DFIM[m] = CO1 * (FR[m] + FR[m - 1]);
   DFIM[ML - 1] = CO1 * FR[ML - 1 - 1];


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                        Allocate_NEDWAM_Arrays                             */
/*                                                                           */
/*****************************************************************************/
int Allocate_NEDWAM_Arrays()
{
   int m;                                   /* teller frequenties */


   /*
   //
   // alloceren van geheugenruimte 1-dimensionale arrays
   //
   */
   TH    = (double*)calloc(NUM_RICHTINGEN, sizeof(double));    /* cleared to 0 */
   COSTH = (double*)calloc(NUM_RICHTINGEN, sizeof(double));    /* cleared to 0 */
   SINTH = (double*)calloc(NUM_RICHTINGEN, sizeof(double));    /* cleared to 0 */
   DFIM  = (double*)calloc(NUM_FREQUENTIES, sizeof(double));   /* cleared to 0 */
   EMEAN = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   E10   = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   SI    = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   CI    = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   THQ   = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   E10D  = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   FMEAN = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   TEMP2 = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */


   /*
   //
   // alloceren van geheugenruimte 2-dimensionaal arrays
   //
   */
   EMN1DIM = (double**)calloc(NUM_FREQUENTIES, sizeof(double*));
   for (m = 0; m < NUM_FREQUENTIES; m++)
   {
      EMN1DIM[m] = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   }

   SISP = (double**)calloc(NUM_FREQUENTIES, sizeof(double*));
   for (m = 0; m < NUM_FREQUENTIES; m++)
   {
      SISP[m] = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   }

   CISP = (double**)calloc(NUM_FREQUENTIES, sizeof(double*));
   for (m = 0; m < NUM_FREQUENTIES; m++)
   {
      CISP[m] = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   }

   THQSP = (double**)calloc(NUM_FREQUENTIES, sizeof(double*));
   for (m = 0; m < NUM_FREQUENTIES; m++)
   {
      THQSP[m] = (double*)calloc(NUM_VERWACHTINGEN, sizeof(double)); /* cleared to 0 */
   }


   return 0;
}


/*****************************************************************************/
/*                                                                           */
/*                       De_Allocate_NEDWAM_Arrays                           */
/*                                                                           */
/*****************************************************************************/
int De_Allocate_NEDWAM_Arrays()
{
   int m;                                   /* teller frequenties */


   /*
   // de-alloceren van geheugenruimte 1 dimensionale arrays
   */
   free(TH);
   free(COSTH);
   free(SINTH);
   free(DFIM);
   free(EMEAN);
   free(E10);
   free(SI);
   free(CI);
   free(THQ);
   free(E10D);
   free(FMEAN);
   free(TEMP2);

   /*
   // de-alloceren van geheugenruimte 2 dimensionale arrays
   */
   for (m = 0; m < NUM_FREQUENTIES; m++)
      free(EMN1DIM[m]);
   free(EMN1DIM);

   for (m = 0; m < NUM_FREQUENTIES; m++)
      free(SISP[m]);
   free(SISP);

   for (m = 0; m < NUM_FREQUENTIES; m++)
      free(CISP[m]);
   free(CISP);

   for (m = 0; m < NUM_FREQUENTIES; m++)
      free(THQSP[m]);
   free(THQSP);


   return 0;
}


