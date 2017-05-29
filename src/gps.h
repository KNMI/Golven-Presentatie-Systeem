/* sentry */
#define gps_h

/* hier moet staan WINDOWS of UNIX */
#define WINDOWS

/* array afmetingen van CIC spectrum[NUM_SPECTRUM][NUM_SPECTRUM_2] */
#define NUM_SPECTRUM                                   100     /* AANTAL array plaatsen van eerste indice (DIT IS DE HOOGSTE INDICE + 1) */
#define NUM_SPECTRUM_2                                 11      /* AANTAL array plaatsen van tweede indice */
#define STATION_SPECTRUM                               0       /* indice nummer stations(code) spectrum-array */
#define DATUMTIJD_SPECTRUM                             1       /* indice datumtijd groep spectrum-array */
#define BEGIN_SPECTRUM_ENERGY                          2       /* indice eerste energy waarden spectrum-array (begin telling bij 0) */
#define TUSSEN_SPECTRUM_ENERGY                         17      /* indice begin tweede deel energy waarden spectrum-array */
#define END_SPECTRUM_ENERGY                            42      /* indice laatste energy waarden spectrum-array (eind telling bij 0) */
#define HM0_SPECTRUM                                   43      /* indice nummer Hm0 spectrum-array */
#define TM0_1_SPECTRUM                                 44      /* indice nummer Tmo-1 spectrum-array */
#define HE3_SPECTRUM                                   45      /* indice nummer HE3 spectrum-array */
#define SPECTRUM_ENERGY_0_4                            42      /* indice spectrum t.m 0.4 Hz (afh van max_scale_x) */
#define SPECTRUM_ENERGY_0_3                            32      /* indice spectrum t.m 0.3 Hz (afh van max_scale_x) */
#define SPECTRUM_ENERGY_0_2                            22      /* indice spectrum t.m 0.2 Hz (afh van max_scale_x) */
#define FF_WIND                                        46      /* indice nummer ff (wind) spectrum-array */
#define DD_WIND                                        47      /* indice nummer dd (wind) spectrum-array */
#define BEGIN_SPECTRUM_RICHTING                        48      /* indice eerste richting waarden spectrum-array (begin telling bij 0) */
#define END_SPECTRUM_RICHTING                          88      /* indice laatste richting waarden spectrum-array (eind telling bij 0) */
#define SPECTRUM_RICHTING_0_4                          88      /* indice richting spectrum t.m 0.4 Hz (afh van max_scale_x) */
#define SPECTRUM_RICHTING_0_3                          78      /* indice richting spectrum t.m 0.3 Hz (afh van max_scale_x) */
#define SPECTRUM_RICHTING_0_2                          68      /* indice richting spectrum t.m 0.2 Hz (afh van max_scale_x) */
#define DD_GEM_SPECTRUM                                89      /* indice nummer dd gem. spectrum-array */
#define DD_GEM_LFE_SPECTRUM                            90      /* indice nummer dd gem.-LFE spectrum-array */
#define HS7_SPECTRUM                                   91      /* indice nummer Hs7 spectrum-array */
#define FP_SPECTRUM                                    92      /* indice nummer fp */
#define HMAX_SPECTRUM                                  93      /* indice nummer Hmax */
#define BFI_SPECTRUM                                   94      /* indice nummer BFI (voor freak waves) */
#define C4_SPECTRUM                                    95      /* indice nummer C4 (voor freak waves) */
#define P_SPECTRUM                                     96      /* indice nummer P (voor freak waves) */
#define H_1_50_SPECTRUM                                97      /* indice nummer H(1/50) */
#define T_HMAX_SPECTRUM                                98      /* indice nummer T(Hmax) */
#define AANTAL_GOLVEN_SPECTRUM                         99      /* indice nummer aantal golven (AG) */



/* array afmetingen van DSP (CIC) spectrum */
#define NUM_DSP_FREQUENTIES                            48      /* indice (aantal) DSP spectrum */
#define NUM_DSP_RICHTINGEN                             36      /* indice (aantal) DSP spectrum */
#define NUM_DSP_REALS                                  11      /* indice (aantal) DSP spectrum */
#define DSP_FREQ_VERSCHIL                              0.01    /* (vaste) verschil tussen frequenties in DSP files */
#define DSP_FREQ_0_03                                  0       /* indice van 0.03 */
#define DSP_FREQ_0_40                                  37      /* indice van 0.40 */

/* array afmetingen CID_Array */
#define AANTAL_SWELL_CID_ELEMENTEN                     5       /* t.w.: station, datumtijd, HsSW, thSW, TmSW */
#define LENGTE_CID_SWELL_ELEMENTEN                     10      /* 10 char */
#define HSW_CID_INDEX                                  2       /* index nummer height swell */
#define DSW_CID_INDEX                                  3       /* index nummer dir swell */
#define PSW_CID_INDEX                                  4       /* index nummer period swell; zie ook AANTAL_SWELL_CID_ELEMENTEN */

/* array afmetingen HWA_Array (soort rtwndb backup voor cic 1-dimensionale data) */
#define AANTAL_HWA_ELEMENTEN                           6       /* t.w.: station, datumtijd, Hwa (Hm0), Pw(Tm0-1), dd, ff */
#define LENGTE_HWA_ELEMENTEN                           12      /* 12 char (in ieder geval meer dan 10 i.v.m. JJJJJMMDDUU */
#define DD_HWA_INDEX                                   2       /* index nummer */
#define FF_HWA_INDEX                                   3       /* index nummer */
#define HM0_HWA_INDEX                                  4       /* index nummer */
#define TM0_HWA_INDEX                                  5       /* index nummer */


/* array afmetingen van TSA (NEDWAM) spectrum [NUM_RICHTINGTEN][NUM_FREQUENTIES][NUM_VERWACHTINGEN][NUM_REALS] */
#define NUM_REALS                                      16      /* AANTAL array plaatsen (15 + 1 characters) */

/* werkelijk aantal char van een waarde in de NEDWAM TSA forecast file */
#define LENGTE_NEDWAM_ENERGY_WAARDE                    15

/* array afmetingen van stations[NUM_STATIONS][NUM_STATIONS_2] */
#define NUM_STATIONS                                   20      /* gebruikt voor TSA en CIC (hoort bij inlezen locaties file) */
#define NUM_STATIONS_2                                 4       /* char array (3 char voor stations code b.v. K13) */

/* array afmetingingen TMA (nedwam-parameters zoals hm0) en indice nummers inlees file */
#define NUM_TMA_PARAMETERS                             9       /* LET OP INDIEN GEWIJZIGD DIT OOK DOORVOEREN IN UVS [V_1APPPATH.H]; hm0, tm0-1, E10, dd_E10, Hs7, HSW, DSW, PSW, Dtotal */
#define NUM_TMA_VERWACHTINGEN                          49      /* LET OP INDIEN GEWIJZIGD DIT OOK DOORVOEREN IN UVS [V_1APPPATH.H]; LET OP MOET ZELFDE ZIJN ALS NUM_UBA_VERWACHTINGEN; van 00 t/m +48 hr */
#define NUM_TMA_CHAR                                   16      /* 10 char voor de uiteindelijke waarde opslag */
#define TMA_PARAMETER_INDEX_HM0                        0       /* HM0 op deze array plaats */
#define TMA_PARAMETER_INDEX_TM0_1                      1       /* TM0-1 op deze array plaats */
#define TMA_PARAMETER_INDEX_E10                        2       /* E10 op deze array plaats; Hs10 = wortel(E10) * 4 */
#define TMA_PARAMETER_INDEX_DD_E10                     3       /* richting van E10 deze array plaats  (is ook richting Hs10) */
#define TMA_PARAMETER_INDEX_HS7                        4       /* Hs7 op deze array plaats (zie ook NUM_TMA_PARAMETERS) */
#define TMA_PARAMETER_INDEX_HSW                        5       /* height swell op deze array plaats (zie ook NUM_TMA_PARAMETERS) */
#define TMA_PARAMETER_INDEX_DSW                        6       /* direction swell op deze array plaats (zie ook NUM_TMA_PARAMETERS) */
#define TMA_PARAMETER_INDEX_PSW                        7       /* period swell op deze array plaats */
#define TMA_PARAMETER_INDEX_DTOTAL                     8       /* dtotal swell op deze array plaats */
#define TMA_VERWACHTINGS_INTERVAL                      1       /* 1 uur tussen de verwachtingen punten (0, +1, +2, +3 etc. */

/* array afmetingingen UBA (hirlam-parameters zoals ff) en indice nummers inlees file */
#define NUM_UBA_PARAMETERS                             1       /* ff, dd etc. */
#define NUM_UBA_VERWACHTINGEN                          49      /* MOET ZELFDE ZIJN ALS NUM_TMA_VERWACHTINGEN; van 00 t/m +48 hr */
#define NUM_UBA_CHAR                                   16      /* 10 char voor de uiteindelijke waarde opslag */
#define UBA_PARAMETER_INDEX_FF                         0       /* ff op deze array plaats */

/* constanten voor NEDWAM berekeningen */
#define PI                                             3.14159265358978
#define ZPI                                            6.28318530717959   /* = 2 * PI   */
#define TWEE_PI                                        6.28318530717959   /* = 2 * PI   */
#define RAD                                            0.01745329251994   /* = PI / 180 */
#define DEG                                            57.2957795130823   /* = 180 / PI */
/*#define KTS_TO_TIENDEN_M_S                   5.1444444444 // kts -> 0.1 m/s */
#define M_S_TO_KNOTS                                   1.94384449 /* van m/s --> kn */
#define WORTEL_3                                       1.73205081
#define COS_0                                          1
#define COS_30                                         0.57735027         /* 1/3 V3 */
#define COS_60                                         0.5
#define SIN_0                                          0
#define SIN_30                                         0.5
#define SIN_60                                         0.57735027



/* blauw scenario */
#define R_G_B_color_pagina_ZEE                         "0.5 1 1"
#define R_G_B_color_pagina_kader_ZEE                   "0 0 0"
#define R_G_B_color_model_ZEE                          "0.8 1 1"
#define R_G_B_color_obs_ZEE                            "0.3 0.3 1"
#define R_G_B_color_x_y_box_ZEE                        "0.3 0.7 1"
#define R_G_B_color_x_y_box_kader_ZEE                  "0 0 0"
#define R_G_B_color_title_ZEE                          "0 0 0"

#define R_G_B_color_obs_in_forecast_ZEE                "1 0 0"                 /* red */
#define R_G_B_color_obs_summary_ZEE                    "0 0 0.5"               /* navy blue */
#define R_G_B_color_model_summary_ZEE                  "0 0 0.5"               /* navy blue */
#define R_G_B_color_dd                                 "0.4 0.6 0.6"           /* CadetBlue */  /*"0.8 0.8 0.8" */  /* ltgrey */
#define R_G_B_color_f_rule                             "0.4 0.6 0.6"           /* CadetBlue */  /*"0.8 0.8 0.8" */  /* ltgrey */
#define R_G_B_color_cic_dd_in_forecast_ZEE             "0.5 0 0"               /* dark red */
#define R_G_B_color_polair_raster                      "1 1 1"                 /* white */
#define R_G_B_color_x_y_box_polair_ZEE                 "0 0 0.5"               /* navy blue */
#define R_G_B_color_model_summary_polair               "0 0 0.5"               /*  */
#define R_G_B_color_dsp_summary_polair                 "0 0 0.5"               /*  */
#define R_G_B_color_model_parameter                    "0 0 0.5"               /*  */

#define R_G_B_color_polair_class_9_10                  "1.0 0.0 0.0"           /* red */
#define R_G_B_color_polair_class_8_9                   "1.0 0.6 0.0"           /* orange */
#define R_G_B_color_polair_class_7_8                   "1.0 1.0 0.0"           /* yellow */
#define R_G_B_color_polair_class_6_7                   "0.6 0.8 0.2"           /* yellow-green */
#define R_G_B_color_polair_class_5_6                   "0.1 0.5 0.1"           /* forest green */
#define R_G_B_color_polair_class_4_5                   "0.0 1.0 0.0"           /* green */
#define R_G_B_color_polair_class_3_4                   "0.6 0.7 0.8"           /* light sky bleu 3 */
#define R_G_B_color_polair_class_2_3                   "0.3 0.5 1.0"           /* royalbleu1 */
#define R_G_B_color_polair_class_1_2                   "0.0 0.0 1.0"           /* blue */
#define R_G_B_color_polair_class_0_1                   "0.0 0.0 0.5"           /* navy blue */
#define R_G_B_color_polair_index_kader                 "0 0 0"                 /* black */


/* polaire plotjes */
#define NUMBER_ENERGY_CLASSES                          10                      /* 10 genormaliseerde klassen (0 t/m 9) */
/*#define AANTAL_CIRKELS_HZ                              4 */                      /* b.v. de cirkels van 0.1 Hz + 0.2 Hz + 0. 3Hz + 0.4 Hz */
#define MAX_POLAIR_HZ                                  /*0.41145*/0.4          /* "buitenste cirkel" in polair plotje */

/* postscript files */
#define POSTSCRIPTFILE_ONDERSTE_GRAFIEK                "postcriptfile_onderste_grafiek"
#define POSTSCRIPTFILE_MIDDELSTE_GRAFIEK               "postcriptfile_middelste_grafiek"
#define POSTSCRIPTFILE_BOVENSTE_GRAFIEK                "postcriptfile_bovenste_grafiek"

/* HTML pagina's */
#define PATH_INTRANET_GPS_WINDOWS                      "i:\\www\\homepages\\stam\\pub_info\\gps"
/*#define PATH_INTRANET_GPS_UNIX                         "../../../IenI/webdata/www/homepages/stam/pub_info/gps/"  */

/* blauw scenario */
/*
#define R_G_B_color_pagina_ZEE                         "0.5 1 1"
#define R_G_B_color_pagina_kader_ZEE                   "0 0 0"
#define R_G_B_COLOR_MODEL_ZEE                          "0.8 1 1"
#define R_G_B_COLOR_CIC_ZEE                            "0.3 0.3 1"
#define R_G_B_COLOR_X_Y_BOX_ZEE                        "0.3 0.7 1"
#define R_G_B_COLOR_X_Y_BOX_KADER_ZEE                  "0 0 0"
#define R_G_B_COLOR_TITLE_ZEE                          "0 0 0"
#define R_G_B_color_dag_ZEE                            "0 0 0"
#define R_G_B_color_maand_ZEE                          "0 0 0"
#define R_G_B_color_bar_contour_ZEE                    "0 0 0"
#define R_G_B_color_forecast_1_ZEE                     R_G_B_color_model_ZEE
#define R_G_B_color_forecast_2_ZEE                     "0 0 0"
#define R_G_B_color_forecast_3_ZEE                     R_G_B_color_obs_ZEE
#define R_G_B_color_forecast_unverified_ZEE            "1 0 0"
*/

/* waterdiepten (meters) (zie ook spectra.h) zie function gps_main.h */
#define NC1_DIEPTE                                           161.0
#define NC2_DIEPTE                                           161.0
#define ANA_DIEPTE                                           86.0
#define AUK_DIEPTE                                           77.0
#define K13_DIEPTE                                           28.0
#define ELD_DIEPTE                                           28.0
#define SMN_DIEPTE                                           18.0
#define IJ5_DIEPTE                                           20.0
#define MUN_DIEPTE                                           24.0
#define MPN_DIEPTE                                           16.0
#define EPL_DIEPTE                                           30.0
#define DWE_DIEPTE                                           30.0
#define LEG_DIEPTE                                           20.0
#define E13_DIEPTE                                           22.0
#define Q11_DIEPTE                                           26.0
#define Q12_DIEPTE                                           26.0
#define BG2_DIEPTE                                           10.0
#define A12_DIEPTE                                           28.0

/* array voor kurtosis etc. */
#define AANTAL_CZZ10_ELEMENTEN                               41                 /* 0.0 - 0.4 Hz met stapjes van 0.01 Hz */


