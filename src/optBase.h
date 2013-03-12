/*
 * EPSON ESC/P-R Printer Driver for Linux
 * Copyright (C) 2002-2005 AVASYS CORPORATION.
 * Copyright (C) Seiko Epson Corporation 2002-2005.
 *
 *  This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA.
 *
 * As a special exception, AVASYS CORPORATION gives permission to
 * link the code of this program with libraries which are covered by
 * the AVASYS Public License and distribute their linked
 * combinations.  You must obey the GNU General Public License in all
 * respects for all of the code used other than the libraries which
 * are covered by AVASYS Public License.
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef N_
#define N_(magic_word)      (magic_word)
#endif

#define END_ARRAY -1			

#include "epson-escpr-media.h"

typedef struct _OPTION_DATA
{
	int value;
	char *rsc_name;
	char *x_name;
	char *x_name_jp;
} OPTION_DATA;

OPTION_DATA mediaSizeData[] = {
    {EPS_MSID_A4,            "A4",       N_("A4  210x297 mm")},
    {EPS_MSID_LETTER,        "LT",   N_("Letter  8 1/2x11 in")},
    {EPS_MSID_LEGAL,         "LGL",      N_("Legal  8 1/2x14 in")},
    {EPS_MSID_A5,            "A5",       N_("A5  148x210 mm")},
    {EPS_MSID_A6,            "A6",       N_("A6  105x148 mm")},
    {EPS_MSID_B5,            "B5",       N_("B5  182x257 mm")},
    {EPS_MSID_EXECUTIVE,     "EXE",      N_("Executive  7 1/4x10 1/2 in")},
    {EPS_MSID_HLT,    "HLT",      N_("Half Letter  5 1/2x8 1/2 in")},
    {EPS_MSID_PANORAMIC,     "PANORAM",  N_("Panoramic 210x594 mm")},
    {EPS_MSID_TRIM_4X6,      "4X6",      N_("4x6 100x152 mm")},
    {EPS_MSID_4X6,           "4X6FULL",  N_("Photo Paper 4x6 in  No Perforations")},
    {EPS_MSID_5X8,           "INDEX5",   N_("Index card  5x8 in")                },
    {EPS_MSID_8X10,          "8x10",   N_("Index card  8x10 in")               },
	{EPS_MSID_F4,				"F4", N_("F4")},
	{EPS_MSID_ENVY0,			"ENVY0", N_("Yougata 0 120 x 235 mm")},
	{EPS_MSID_ENVY6,			"ENVY6", N_("Yougata 6 98 x 190 mm")},
	{EPS_MSID_ENVK2,			"ENVK2", N_("Kakugata 2 240  x 332 mm")},
	{EPS_MSID_4PC,				"4PC", N_("4PC")},		
    {EPS_MSID_10X15,         "PP100",    N_("Photo Paper 100x150 mm")            },
    {EPS_MSID_200X300,       "PP200",    N_("Photo Paper 200x300 mm")            },
    {EPS_MSID_L,             "L",        N_("3.5x5 in  89x127 mm")               },
    {EPS_MSID_POSTCARD,      "POSTCARD", N_("Post Card  100x148 mm")             },
    {EPS_MSID_DBLPOSTCARD,   "DPC",      N_("Double Post Card  200x148 mm")      },
    {EPS_MSID_ENV_10_L,      "ENV10_L",  N_("Envelope #10(Landscape) 4 1/8x9 1/2 in")      },
    {EPS_MSID_ENV_C6_L,      "ENVC6_L",  N_("Envelope C6(Landscape) 114x162 mm")           },
    {EPS_MSID_ENV_DL_L,      "ENVDL_L",  N_("Envelope DL(Landscape) 110x220 mm")           },
    {EPS_MSID_NEWEVN_L,      "ENV5X8_L", N_("Envelope(Landscape) 132x220 mm")              },
    {EPS_MSID_CHOKEI_3,      "ENVC3",    N_("Envelope  120x235 mm")              },
    {EPS_MSID_CHOKEI_4,      "ENVC4",    N_("Envelope  90x205 mm")               },
    {EPS_MSID_YOKEI_1,       "ENVY1",    N_("Envelope  120x176 mm")              },
    {EPS_MSID_YOKEI_2,       "ENVY2",    N_("Envelope  114x162 mm")              },
    {EPS_MSID_YOKEI_3,       "ENVY3",    N_("Envelope  98x148 mm")               },
    {EPS_MSID_YOKEI_4,       "ENVY4",    N_("Envelope  105x235 mm")              },
    {EPS_MSID_2L,            "2L",       N_("5x7 in  127x178 mm")                },
    //{EPS_MSID_ENV_10,        "ENV10",    N_("Envelope #10  4 1/8x9 1/2 in")      },
    //{EPS_MSID_ENV_C6,        "ENVC6",    N_("Envelope C6  114x162 mm")           },
    //{EPS_MSID_ENV_DL,        "ENVDL",  	N_("Envelope DL  110x220 mm")           },
    //{EPS_MSID_NEWENV,        "ENV5X8",	N_("Envelope 132x220 mm")       	},
    {EPS_MSID_BUZCARD_89X50, "NC",       N_("Business Card 89x50 mm")            },
    //{EPS_MSID_BUZCARD_89X55, "NC55",     N_("Business Card 89x50 mm")            },
    {EPS_MSID_CARD_54X86,    "CARD",     N_("Card 54x86 mm")                       },
    {EPS_MSID_BUZCARD_55X91, "NC91",     N_("Business Card 55x91 mm")              },
    {EPS_MSID_ALBUM_A5,      "PAA5",     N_("Cover of Photo Album L 127 x 198 mm")     },
    {EPS_MSID_ALBUM_L,       "PAL",      N_("Cover of Photo Album A5 210 x 321 mm") },
    {EPS_MSID_PALBUM_L_L,    "PALL",     N_("P.Album L (Landscape) 127 x 89 mm") },
    {EPS_MSID_PALBUM_2L,     "PA2L",     N_("P.Album 2L 127 x 178 mm")      },
    {EPS_MSID_PALBUM_A5_L,   "PAA5L",    N_("P.Album A5 (Landscape) 210 x 148 mm") },
    {EPS_MSID_PALBUM_A4,     "PAA4",     N_("P.Album A4 210 x 297 mm")           },
    //{EPS_MSID_4X7,           "4X7",      N_("16x9 wide size (102 x 181 mm)")},
    {EPS_MSID_A3NOBI,        "A3+",      N_("A3+ 329x483 mm")          },
    {EPS_MSID_A3,            "A3",       N_("A3  297x420 mm")                    },
    {EPS_MSID_B4,            "B4",       N_("B4  257x364 mm")                    },
    {EPS_MSID_USB,           "USB",      N_("US B(11x17 in)")},
    {EPS_MSID_11X14,         "11X14",    N_("11x14 in")},
    {EPS_MSID_B3,            "B3",       N_("B3 364 x 514 mm")},
    {EPS_MSID_A2,            "A2",       N_("A2 420 x 594 mm")},
    {EPS_MSID_USC,           "USC",      N_("US C 17 x 22 in")},
    {EPS_MSID_10X12,	    "10X12",	N_("10x12 in / 254x305 mm")},
    {EPS_MSID_12X12,	    "12X12",	N_("30 x 30 cm (12 x 12 in)")},
    {EPS_MSID_GLG,	    "GLG",	N_("8 x 13 in)")},
    {EPS_MSID_GLT,	    "GLT",	N_("8.5 x 10.5 in")},
    {EPS_MSID_USER,          "USER",     N_("User Defined")},
	 {EPS_MSID_MON,		"MON",	""},
	 {EPS_MSID_C10,		"C10", ""},
	 {EPS_MSID_DL,			"DL", ""},
	 {EPS_MSID_C5,			"C5",""},
	 {EPS_MSID_C6,			"C6",""},
	 {EPS_MSID_IB5,		"IB5",""},
    {EPS_MSID_UNKNOWN,       "UNKNOWN",  N_("Unknown Paper Size")},
    {END_ARRAY,                "",         ""                                      }
};


OPTION_DATA mediaTypeData[] = {

    { EPS_MTID_PLAIN,         "PLAIN",    N_("plain papers")                      },
    { EPS_MTID_360INKJET,     "FINE",     N_("360dpi Ink Jet Paper")              },
    { EPS_MTID_IRON,          "IRON",     N_("Iron-On Cool Peel Transfer Paper")  },
    { EPS_MTID_PHOTOINKJET,   "SFINE",    N_("Photo Quality Ink Jet Paper")       },
    { EPS_MTID_PHOTOADSHEET,  "PADSHEET", N_("Photo Quality Self Adhesive Sheets")},
    { EPS_MTID_MATTE,         "PMMATT",   N_("EPSON Matte")                       },
    { EPS_MTID_PHOTO,         "GPPAPER",  N_("Photo Paper")            	 },  /* chenged Wed Jan 28 2009 */
    { EPS_MTID_PHOTOFILM,     "GFILM",    N_("Photo Quality Glossy Film")         },
    { EPS_MTID_MINIPHOTO,     "MINIPHOTO",N_("Epson Photo Stickers")		 },
    { EPS_MTID_OHP,           "OHP",      N_("Ink Jet Transparencies")            },
    { EPS_MTID_BACKLIGHT,     "BLFILM",   N_("Ink Jet Back Light Film")           },
    { EPS_MTID_PGPHOTO,       "PMPHOTO",  N_("EPSON Premium Glossy")        },
    { EPS_MTID_PSPHOTO,       "PSGLOS",   N_("Epson Premium Semigloss")     },
    { EPS_MTID_PLPHOTO,       "MCLP",     N_("Premium Luster Photo Paper")    	 },
    { EPS_MTID_MCGLOSSY,      "PHOTOW",   N_("Glossy Paper - Photo Weight")         },
    { EPS_MTID_ARCHMATTE,     "ARCMATT",  N_("Archival Matte Paper")              },
    { EPS_MTID_WATERCOLOR,    "WATERCOLOR", N_("Watercolor Paper - Radiant White")},
    { EPS_MTID_PROGLOSS,      "PROGLS", 	 N_("Professional Glossy Paper")},
    { EPS_MTID_MATTEBOARD,    "MBOARD",   N_("Matte Board Paper")},
    { EPS_MTID_PHOTOGLOSS,    "PHOTOGLS", N_("Photo Quality Glossy Paper")},
    { EPS_MTID_SEMIPROOF,     "SEMIPROOF",N_("Dupont/EPSONSemigloss Proofing Paper-A")},
    { EPS_MTID_SEMITHICK,      "SEMITHICK", ""}, //lam dai hiep add
    { EPS_MTID_PREPRINTED,      "PREPRINTED", ""}, //lam dai hiep add
    { EPS_MTID_SUPERFINE2,    "SFINE2",   N_("Photo Quality Ink Jet Paper Plus")},
    { EPS_MTID_DSMATTE,       "DBSMATT",  N_("Double-Sided Matte Paper")},
    { EPS_MTID_CLPHOTO,       "CLPP",     N_("ColorLife Photo Paper")},
    { EPS_MTID_ECOPHOTO,      "EPHOTO",   N_("Economy Photo Paper")},
    { EPS_MTID_VELVETFINEART, "VELVET_FINE", N_("Velvet Fine Art Paper")},
    { EPS_MTID_PROOFSEMI,     "PROOF_S",  N_("Proofing Paper Semimatte")},
    { EPS_MTID_HAGAKIRECL,    "RCPC",     N_("Recycled Paper Postcard")},
    { EPS_MTID_HAGAKIINKJET,  "IJPC",     N_("Ink Jet Postcard")},
    { EPS_MTID_PHOTOINKJET2,  "PIJ2", 	 N_("Epson Photo Quality Ink Jet")},
    { EPS_MTID_DURABRITE,     "EPP_P",    N_("DURABrite Photo Paper")},
    { EPS_MTID_MATTEMEISHI,   "MMEISHI",  N_("Matte Business Card")},
    { EPS_MTID_HAGAKIATENA,   "ADDRESS",  N_("Post Card Address Side")},
    { EPS_MTID_PHOTOALBUM,    "P_ALBUM",  N_("Photo Album")},
    { EPS_MTID_PHOTOSTAND,    "P_BOARD",  N_("Photo Board Paper")},
    { EPS_MTID_RCB,           "RCB",	 N_("RC-B")},
    { EPS_MTID_PGPHOTOEG,     "PGPHOTOEG",N_("Premium Glossy Photo Paper_2")},
    { EPS_MTID_ENVELOPE,      "ENV", 	 N_("Envelope")},
    { EPS_MTID_PLATINA,       "PLATINA",  N_("Epson Ultra Glossy")},
    { EPS_MTID_ULTRASMOOTH,   "USF_ART",  N_("UltraSmooth Fine Art Paper")},
/* add Wed Jan 28 2009 v */
    { EPS_MTID_SFHAGAKI,   	"SFHAGAKI",  N_("Super Fine Postcard")},
    { EPS_MTID_PHOTOSTD,   	"PHOTOSTD",  N_("Premium Glossy Photo Paper")},
    { EPS_MTID_GLOSSYHAGAKI, "GLOSSYHAGAKI", N_("Glossy Postcard")},
	{ EPS_MTID_POSTCARD, 	 "POSTCARD", N_("Postcard")}, 
    { EPS_MTID_GLOSSYPHOTO,  "GLOSSYPHOTO",  N_("Glossy Photo Paper")},
    { EPS_MTID_GLOSSYCAST,	"GLOSSYCAST",  N_("Epson Photo")},
    { EPS_MTID_LETTERHEAD,    "LETTERHEAD", ""},
    { EPS_MTID_RECYCLED,    "RECYCLED", ""},
    { EPS_MTID_COLOR,    "COLOR", ""},
    { EPS_MTID_LABEL,    "LABEL", ""},
    { EPS_MTID_THICK,    "THICK", ""},
	 {EPS_MTID_TRANSPARENT, "TRANSPARENT", ""},
    { EPS_MTID_EXTHICK,    "EXTHICK", ""},
    { EPS_MTID_UNSPECIFIED, "UNSPECIFIED", ""},
    { EPS_MTID_COATED,    "COATED", ""},
    { EPS_MTID_SPECIAL,    "SPECIAL", ""},
    { EPS_MTID_BUSINESSCOAT, "BUSINESSCOAT", N_("Business Ink Jet Coat Paper")},
/* add Wed Jan 28 2009 ^ */
/* del Wed Jan 28 2009 { EPS_MTID_GPHOTO,        "GPHOTO",   N_("Glossy Photo Paper_2")},*/
/* del Wed Jan 28 2009 { EPS_MTID_PHOTOEGCP,     "PHOTOEGCP",N_("Epson Photo")},*/
    { EPS_MTID_CDDVD,         "CDR",      N_("CD/DVD")},
    { EPS_MTID_CDDVDHIGH,     "CDRH",     N_("CD/DVD Premium Surface")},
    { EPS_MTID_CLEANING,      "CLEAN",    N_("Cleaning Sheet")},
    { EPS_MTID_UNKNOWN,       "UNKNOWN",  N_("Unknown Paper Type")},
    { END_ARRAY,             "",         ""                       }
};

