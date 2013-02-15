/*_______________________________  epson-escpage-mono.c   ______________________________*/

/*       1         2         3         4         5         6         7         8        */
/*34567890123456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*******************************************|********************************************/
/*
 *   Copyright (c) 2010  Seiko Epson Corporation                 All rights reserved.
 *
 *   Copyright protection claimed includes all forms and matters of
 *   copyrightable material and information now allowed by statutory or judicial
 *   law or hereinafter granted, including without limitation, material generated
 *   from the software programs which are displayed on the screen such as icons,
 *   screen display looks, etc.
 *
 */

/*------------------------------------  Includes   -------------------------------------*/
/*******************************************|********************************************/
#include "epson-escpr-err.h"
#include "epson-escpr-media.h"
#include "epson-escpr-mem.h"
#include "epson-escpage.h"
#include "epson-escpage-mono.h"

/*-----------------------------  Local Macro Definitions -------------------------------*/
/*******************************************|********************************************/
#ifdef EPS_LOG_MODULE_PAGE
#define EPS_LOG_MODULE	EPS_LOG_MODULE_PAGE
#else
#define EPS_LOG_MODULE	0
#endif

/*--------------------------------  ESC/PAGE commands ----------------------------------*/
/*******************************************|********************************************/
const static EPS_UINT8 sbCR[]       = {0x0D};											/* Carriage return "CR" */
const static EPS_UINT8 sbFF[]		= {0x0C};											/* Form Feed "FF" */
const static EPS_UINT8 sbIP[]		= {0x1D, 'r', 'h', 'E'};

const static EPS_UINT8 sbCSE[]		= {0x1D, '1', 'c', 's', 'E'};	

						/* Initialize printer */
const static EPS_UINT8 sbSU150[]    = {0x1D, '0', ';', '.', '4', '8', 'm', 'u', 'E'};	/* Set unit to 150 */
const static EPS_UINT8 sbSU300[]    = {0x1D, '0', ';', '.', '2', '4', 'm', 'u', 'E'};	/* Set unit to 300 */
const static EPS_UINT8 sbSU600[]    = {0x1D, '0', ';', '.', '1', '2', 'm', 'u', 'E'};	/* Set unit to 600 */
const static EPS_UINT8 sbSU1200[]   = {0x1D, '0', ';', '.', '0', '6', 'm', 'u', 'E'};	/* Set unit to 1200 */

const static EPS_UINT8 sbHTME300[] = {0x1D, '1', ';', '4', '5', ';', '7', '1', 'h', 't', 'm', 'E'};
const static EPS_UINT8 sbHTME600[] = {0x1D, '1', ';', '4', '5', ';', '1', '0', '6', 'h', 't', 'm', 'E'};
const static EPS_UINT8 sbHTME1200[] = {0x1D, '1', ';', '4', '5', ';', '1', '5', '6', 'h', 't', 'm', 'E'};

const static EPS_UINT8 sbBGPG[]		= {0x1D, '1', 'b', 'g', 'p', 'G'};

const static EPS_UINT8 sbCAPG[] 	= {0x1D, '1', ';', '2', 'c', 'a', 'p', 'G'};


const static EPS_UINT8 sbSDE0[]     = {0x1D, '0', 's', 'd', 'E'};						/* Set Duplex OFF */
const static EPS_UINT8 sbSDE1[]     = {0x1D, '1', 's', 'd', 'E'};						/* Set Duplex ON */
const static EPS_UINT8 sbBDE0[]     = {0x1D, '0', 'b', 'd', 'E'};						/* Set Duplex long */
const static EPS_UINT8 sbBDE1[]     = {0x1D, '1', 'b', 'd', 'E'};						/* Set Duplex short */

const static EPS_UINT8 sbPSSL[]     = {0x1D, '3', '0', 'p', 's', 'E'};					/* Set Letter paper */
const static EPS_UINT8 sbPSSA4[]	= {0x1D, '1', '4', 'p', 's', 'E'};					/* Set A4 paper */
const static EPS_UINT8 sbPSSB5[]    = {0x1D, '2', '5', 'p', 's', 'E'};					/* Set B5 paper */
const static EPS_UINT8 sbPSSLE[]	= {0x1D, '3', '2', 'p', 's', 'E'};					/* Set Legal paper */
const static EPS_UINT8 sbPSSA3[]	= {0x1D, '1', '3', 'p', 's', 'E'};					/* Set A3 paper */
const static EPS_UINT8 sbPSSB4[]    = {0x1D, '2', '4', 'p', 's', 'E'};					/* Set B4 paper */
const static EPS_UINT8 sbPSSPS[]    = {0x1D, '3', '8', 'p', 's', 'E'};					/* Set B4 paper */
const static EPS_UINT8 sbPSSF4[]    = {0x1D, '3', '7', 'p', 's', 'E'};					/* Set F4 paper */
const static EPS_UINT8 sbPSSA5[]    = {0x1D, '1', '5', 'p', 's', 'E'};					/* Set A5 paper */
const static EPS_UINT8 sbPSSHLT[]    = {0x1D, '3', '1', 'p', 's', 'E'};					/* Set HLT paper */
const static EPS_UINT8 sbPSSEXE[]    = {0x1D, '3', '3', 'p', 's', 'E'};					/* Set EXE paper */
const static EPS_UINT8 sbPSSB[]    = {0x1D, '3', '6', 'p', 's', 'E'};					/* Set B paper */
const static EPS_UINT8 sbPSSGLT[]    = {0x1D, '3', '5', 'p', 's', 'E'};					/* Set GLT paper */
const static EPS_UINT8 sbPSSGLG[]    = {0x1D, '3', '4', 'p', 's', 'E'};					/* Set GLG paper */
const static EPS_UINT8 sbPSSMON[]    = {0x1D, '8', '0', 'p', 's', 'E'};					/* Set MON paper */
const static EPS_UINT8 sbPSSC10[]    = {0x1D, '8', '1', 'p', 's', 'E'};					/* Set C10 paper */
const static EPS_UINT8 sbPSSDL[]    = {0x1D, '9', '0', 'p', 's', 'E'};					/* Set DL paper */
const static EPS_UINT8 sbPSSC5[]    = {0x1D, '9', '1', 'p', 's', 'E'};					/* Set C5 paper */
const static EPS_UINT8 sbPSSC6[]    = {0x1D, '9', '2', 'p', 's', 'E'};					/* Set C6 paper */
const static EPS_UINT8 sbPSSIB5[]    = {0x1D, '9', '9', 'p', 's', 'E'};					/* Set IB5 paper */


const static EPS_UINT8 sbIUE1[]    = {0x1D, '0', ';', '0', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE0[]    = {0x1D, '1', '6', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE2[]    = {0x1D, '1', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE3[]    = {0x1D, '2', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE4[]    = {0x1D, '3', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE5[]    = {0x1D, '4', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE6[]    = {0x1D, '5', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE7[]    = {0x1D, '6', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE8[]    = {0x1D, '7', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE9[]    = {0x1D, '8', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE10[]    = {0x1D, '9', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE11[]    = {0x1D, '1', '0', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE12[]    = {0x1D, '1', '1', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE13[]    = {0x1D, '1', '2', ';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE14[]    = {0x1D, '1', '3',';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE15[]    = {0x1D, '2', '4',';', '1', 'i', 'u', 'E'};
const static EPS_UINT8 sbIUE16[]    = {0x1D, '2', '5',';', '1', 'i', 'u', 'E'};


const static EPS_UINT8 sbPTE0[]		= {0x1D, '0', 'p', 't', 'E'};						/* select paper type 0:other */
const static EPS_UINT8 sbPTE1[]		= {0x1D, '1', 'p', 't', 'E'};						/* select paper type 1:plane */
const static EPS_UINT8 sbPTE2[]		= {0x1D, '1', '3', 'p', 't', 'E'};						/* select paper type 1:semi thick */
const static EPS_UINT8 sbPTE3[]		= {0x1D, '2', 'p', 't', 'E'};						/* select paper type 1:preprinter */
const static EPS_UINT8 sbPTE4[]		= {0x1D, '3', 'p', 't', 'E'};						/* select paper type 1:letterhead */
const static EPS_UINT8 sbPTE5[]		= {0x1D, '6', 'p', 't', 'E'};						/* select paper type 1:Recycled */
const static EPS_UINT8 sbPTE6[]		= {0x1D, '7', 'p', 't', 'E'};						/* select paper type 1:color */
const static EPS_UINT8 sbPTE7[]		= {0x1D, '9', 'p', 't', 'E'};						/* select paper type 1:labels */
const static EPS_UINT8 sbPTE8[]		= {0x1D, '1', '2', 'p', 't', 'E'};						/* select paper type 1:thick */
const static EPS_UINT8 sbPTE9[]		= {0x1D, '1', '6', 'p', 't', 'E'};						/* select paper type 1:coated */
const static EPS_UINT8 sbPTE10[]		= {0x1D, '1', '0', 'p', 't', 'E'};						/* select paper type 1:special */
const static EPS_UINT8 sbPTE11[]		= {0x1D, '1', '7', 'p', 't', 'E'};						/* select paper type 1:transparent */



const static EPS_UINT8 sbCMS[]      = {0x1D, '1', 'c', 'm', 'E'};	/* Clip Mode Set */
const static EPS_UINT8 sbSDS300[]   = {0x1D, '0', ';', '3', '0', '0', ';', '3', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(300) */
const static EPS_UINT8 sbSDS600[]   = {0x1D, '0', ';', '6', '0', '0', ';', '6', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(600) */
const static EPS_UINT8 sbSDS1200[]   = {0x1D, '0', ';', '1', '2', '0', '0', ';', '1', '2', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(600) */
const static EPS_UINT8 sbSDS150[]   = {0x1D, '0', ';', '1', '5', '0', ';', '1', '5', '0', 'd', 'r', 'E'};	/* Select Dot Size(150) */
const static EPS_UINT8 sbSDS300_0[]   = {0x1D, '0', ';', '3', '0', '0', ';', '3', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(300) */
const static EPS_UINT8 sbSDS300_1[]   = {0x1D, '1', ';', '3', '0', '0', ';', '3', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(300) */
const static EPS_UINT8 sbSDS300_2[]   = {0x1D, '2', ';', '3', '0', '0', ';', '3', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(300) */
const static EPS_UINT8 sbSDS300_9[]   = {0x1D, '9', ';', '3', '0', '0', ';', '3', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(300) */
const static EPS_UINT8 sbSDS600_0[]   = {0x1D, '0', ';', '6', '0', '0', ';', '6', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(600) */
const static EPS_UINT8 sbSDS600_1[]   = {0x1D, '1', ';', '6', '0', '0', ';', '6', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(600) */
const static EPS_UINT8 sbSDS600_2[]   = {0x1D, '2', ';', '6', '0', '0', ';', '6', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(600) */
const static EPS_UINT8 sbSDS600_9[]   = {0x1D, '9', ';', '6', '0', '0', ';', '6', '0', '0', 'd', 'r', 'E'};	/* Select Dot Size(600) */
const static EPS_UINT8 sbSDS1200_0[]   = {0x1D, '0', ';', '1', '2', '0', '0', ';', '1', '2', '0', '0', 'd', 'r', 'E'};
const static EPS_UINT8 sbSDS1200_1[]   = {0x1D, '1', ';', '1', '2', '0', '0', ';', '1', '2', '0', '0', 'd', 'r', 'E'};
const static EPS_UINT8 sbSDS1200_2[]   = {0x1D, '2', ';', '1', '2', '0', '0', ';', '1', '2', '0', '0', 'd', 'r', 'E'};
const static EPS_UINT8 sbSDS1200_9[]   = {0x1D, '9', ';', '1', '2', '0', '0', ';', '1', '2', '0', '0', 'd', 'r', 'E'};
const static EPS_UINT8 sbSS1200[]	   = {0x1D, '1', ';', '1', ';', 'r', 'a', 'E'};

const static EPS_UINT8 sbMMS[]		= {0x1D, '0', 'm', 'm', 'E'};						/* Select Page memory mode */
const static EPS_UINT8 sbPDS0[]      = {0x1D, '0', 'p', 'o', 'E'};						/* Set paper direction to normal */
const static EPS_UINT8 sbPDS1[]      = {0x1D, '1', 'p', 'o', 'E'};						/* Set paper direction to rotate */
const static EPS_UINT8 sbACRLFS[]	= {0x1D, '0', 'a', 'l', 'f', 'P'};					/* Does not perform auto carriage return */
const static EPS_UINT8 sbAFFS[]		= {0x1D, '0', 'a', 'f', 'f', 'P'};			

const static EPS_UINT8 sbCLFP[]	= {0x1D, '0', ';', '0', ';', '0', 'c', 'l', 'f', 'P'};			
		/* Does not perform auto Form Feed */
const static EPS_UINT8 sbABPS[]		= {0x1D, '0', 'a', 'b', 'P'};						/* Set current position to 0 after bit image draw */
const static EPS_UINT8 sbSARGAS[]	= {0x1D, '0', 's', 'a', 'r', 'G'};					/* Set Absolute Graph Coordinate mode */
const static EPS_UINT8 sbPMPPS[]	= {0x1D, '1', 'p', 'm', 'P'};						/* Set current move mode to print pitch mode */

const static EPS_UINT8 sbSPRCS[]	= {0x1D, '1', '6', '0', 'i', 's', 'E'};				/* Screen Pattern Record Count set to 160 */
const static EPS_UINT8 sbFASCS[]	= {0x1D, '7', 'i', 'a', 'F'};						/* Font Attribute Store Count set to 7 */
const static EPS_UINT8 sbCPSCS[]	= {0x1D, '5', 'i', 'p', 'P'};						/* Current Position Store Count set to 5 */
const static EPS_UINT8 sbMRCS[]		= {0x1D, '1', '6', '0', 'i', 'm', 'M'};				/* Macro Record Store Count set to 160 */
const static EPS_UINT8 sbBIRCS[]	= {0x1D, '2', '3', '2', '8', 'i', 'b', 'I'};		/* Bit Image Count Store Count set to 2328 */
const static EPS_UINT8 sbBIOS[]		= {0x1D, '0', 'b', 'o', 'P'};						/* Set the bit image drawing offset to 0 */
const static EPS_UINT8 sbILGS[]		= {0x1D, '1', '6', '0', 'i', 'l', 'G'};				/* Set line type record count to 160 */
const static EPS_UINT8 sbTSESS[]	= {0x1D, '1', 't', 's', 'E'};						/* Set Screen Mode */
const static EPS_UINT8 sbSPES[]		= {0x1D, '1', ';', '0', ';', '1', '0', '0', 's', 'p', 'E'};	/* Set Screen Pattern */
const static EPS_UINT8 sbOWES[]		= {0x1D, '2', 'o', 'w', 'E'};						/* Set Superimpose Mode */

const static EPS_UINT8 sbCCE[]		= {0x1D, '1', ';', '0', ';', '0', 'c', 'c', 'E'};

const static EPS_UINT8 sbBCI1S[]	= {0x1D, '1', 'b', 'c', 'I'};						/* Select Data
 compression type #1 */

const static EPS_UINT8 sbMLG[]		= {0x1D, '1', ';', '1', '0', 'm', 'l', 'G'};

const static EPS_INT8 sbBID[]      = "\x1D""%d;%d;1;0bi{I";								/* Output data */

/*-----------------------------  Local Macro Definitions -------------------------------*/
/*******************************************|********************************************/
#define EPS_EJL_LINEMAX				(256)
#define EPS_PAGE_CMDBUFF_SIZE		EPS_EJL_LINEMAX

#define EMIT_TIFF_REPEAT(n, x)          \
{                                       \
    nRowTIFF += 3;                      \
    if (RowTIFF)                        \
    {                                   \
		*RowTIFF++ = x;                 \
		*RowTIFF++ = x;                 \
		*RowTIFF++ = (EPS_UINT8)n;		\
    }                                   \
}

#define EMIT_TIFF_LITERAL(n, p)         \
{                                       \
    nRowTIFF += n;                      \
    if (RowTIFF)                        \
    {                                   \
        memcpy(RowTIFF, p, n);          \
        RowTIFF += n;                   \
    }                                   \
}

#define TIFF_MAXIMUM_LITERAL 128
#define TIFF_MAXIMUM_REPEAT  129

#define CALCULATE_INTENSITY(r,g,b)  (((((b)<<8) * 11)/100 + (((g)<<8) * 59)/100 + (((r)<<8) * 30)/100)/256)
#define MAX_8           ((1 << 8) - 1)
#define DOT_K           100
#define E_MAX           (1 << 13)
#define E_MID           (E_MAX >> 1)

/* The following table is used to convert a from an intensity we desire	*/
/* to an intensity to tell the device. It is calibrated for the			*/
/* error-diffusion code.												*/
const EPS_UINT8 Intensity2Intensity[256] = 
{
	0,  3,  7,  10,  14,  17,  21,  24,  28,  35,  42,  49,  56,  63,  70,  77,  
 85,  88,  92,  95,  99,  103,  106,  110,  114,  117,  121,  125,  129,  132,  136,  140,  
 144,  144,  144,  144,  145,  145,  145,  145,  146,  147,  149,  151,  153,  154,  156,  158,  
 160,  161,  163,  164,  166,  167,  169,  170,  172,  173,  174,  175,  176,  177,  178,  179,  
 180,  181,  182,  183,  184,  185,  186,  187,  188,  188,  189,  190,  191,  192,  193,  194,  
 195,  195,  196,  196,  197,  198,  198,  199,  200,  200,  201,  202,  203,  203,  204,  205,  
 206,  206,  206,  206,  207,  207,  207,  207,  208,  208,  208,  208,  209,  209,  209,  209,  
 210,  210,  211,  211,  212,  212,  213,  213,  214,  214,  214,  214,  215,  215,  215,  215,  
 216,  216,  217,  217,  218,  218,  219,  219,  220,  220,  220,  220,  221,  221,  221,  221,  
 222,  222,  222,  222,  223,  223,  223,  223,  224,  224,  225,  225,  226,  226,  227,  227,  
 228,  228,  228,  228,  229,  229,  229,  229,  230,  230,  230,  230,  231,  231,  231,  231,  
 232,  232,  232,  232,  233,  233,  233,  233,  234,  234,  234,  234,  235,  235,  235,  235,  
 236,  236,  236,  236,  237,  237,  237,  237,  238,  238,  239,  239,  240,  240,  241,  241,  
 242,  242,  242,  242,  243,  243,  243,  243,  244,  244,  244,  244,  245,  245,  245,  245,  
 246,  246,  246,  246,  247,  247,  247,  247,  248,  248,  248,  248,  249,  249,  249,  249,  
 250,  250,  251,  251,  252,  252,  253,  253,  254,  254,  254,  254,  254,  254,  255,  255
};

/* Used in color correction */
const static EPS_INT32 BitMask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


/*---------------------------  Data Structure Declarations   ---------------------------*/
/*******************************************|********************************************/
/* Band data */
typedef struct _tagEPS_PAGE_BAND_M_ {
    EPS_INT32   WidthInBytes;
    EPS_INT32   WidthInPixels;
	EPS_INT32	currentWidthInPixels;
    EPS_UINT8	*pRasterData;
    EPS_UINT8	*pIntensity;
    EPS_INT32   encDataSize;
    EPS_UINT8	*pEncData;
	EPS_UINT8	*pZeroRow;
}EPS_PAGE_BAND_M;

/* Error Diffusion */
typedef struct tagHT_PARAM{
	EPS_INT32	iDK;
	EPS_INT16	*pEK;
	EPS_INT32	iOrder;
	EPS_INT32	iScanPels;
}   HT_PARAM;


/*----------------------------  ESC/P-R Lib Global Variables  --------------------------*/
/*******************************************|********************************************/
extern EPS_CMN_FUNC		epsCmnFnc;
extern EPS_PRINT_JOB	printJob;

/*----------------------------------   Local Variables  --------------------------------*/
/*******************************************|********************************************/
static EPS_PAGE_BAND_M		band;
static EPS_COMMAND_BUFFER	cmdBuf;
static EPS_UINT8*			Palette2DeviceIntensity = NULL;
static EPS_INT32			iRow = 0;
static EPS_INT32			iNumBytes = 0;
static EPS_INT32			iNumDots = 0;
static EPS_INT32			pageCount = 0;	/* Current Page number */

#if EPS_PAGE_HT_ERR_DIFFUSION
HT_PARAM	htParam;
#else
EPS_UINT8 * pP = 0;
EPS_INT32 iOrder;
EPS_INT32 iScanPels;
#endif

/*--------------------------  Local Functions Declaration   ----------------------------*/
/*******************************************|********************************************/
static EPS_ERR_CODE CmdBuffInit		(EPS_COMMAND_BUFFER *pCmdBuff);
static EPS_ERR_CODE CmdBuffGrow		(EPS_COMMAND_BUFFER *pCmdBuff, EPS_INT32 addSize);
static void			CmdBuffTerm		(EPS_COMMAND_BUFFER *pCmdBuff);

static void			ConvertBYTE2Intensity	(const EPS_BANDBMP* pInBmp, EPS_PAGE_BAND_M *pBand);
static EPS_ERR_CODE ConvertPaletteToIntensity(EPS_UINT16 paletteSize, EPS_UINT8 *paletteData);

static EPS_ERR_CODE BandInit		(EPS_PAGE_BAND_M *pBand, EPS_INT32 widthInPixels);
static void			BandTerm		(EPS_PAGE_BAND_M* pBand);
static void			BandEncode		(EPS_PAGE_BAND_M* pBand);
static EPS_ERR_CODE BandEmit		(EPS_PAGE_BAND_M *pBand, EPS_INT32 iNByte, EPS_INT32 iNDot);	

static EPS_ERR_CODE HT_Init			(EPS_INT32 WidthPixels);
static void			HT_End			(void);
static void			HT_StartPage	(void);
static void			HT_Scan			(EPS_UINT8 *Con, EPS_UINT8 *Bin, EPS_INT32 widthInPixels);

static EPS_INT32 DoDeltaRow			(EPS_UINT8 *Row, EPS_INT32 nRow, EPS_UINT8 *RowDeltaRow, EPS_UINT8 *Seed);


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%                                             %%%%%%%%%%%%%%%%%%%%%*/
/*--------------------              Public Functions               ---------------------*/
/*%%%%%%%%%%%%%%%%%%%%                                             %%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageAllocBuffer_M()			                                        */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* N/A																					*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*                                                                                      */
/* Description:                                                                         */
/*      Allocate buffer for ESC/Page Job.												*/
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageAllocBuffer_M(void)
{
	EPS_ERR_CODE	ret = EPS_ERR_NONE;

	EPS_LOG_FUNCIN

	ret = BandInit(&band, printJob.printableAreaWidth);
	if ( EPS_ERR_NONE != ret ) {
		EPS_RETURN( ret )
	}
	
	if (printJob.attr.colorPlane == EPS_CP_256COLOR){
		ret = ConvertPaletteToIntensity(printJob.attr.paletteSize, printJob.attr.paletteData);
		if ( EPS_ERR_NONE != ret ) {
			EPS_RETURN( ret )
		}
	}

	/* Halftoning Initialization */
	ret = HT_Init( printJob.printableAreaWidth );
	if ( EPS_ERR_NONE != ret ) {
		EPS_RETURN( ret )
	}

	ret = CmdBuffInit(&cmdBuf);

	EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageRelaseBuffer_M()		                                        */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* N/A																					*/
/*                                                                                      */
/* Return value:                                                                        */
/*      N/A                                                                             */
/*                                                                                      */
/* Description:                                                                         */
/*      Free buffer for ESC/Page Job.			                                        */
/*                                                                                      */
/*******************************************|********************************************/
void pageRelaseBuffer_M(void)
{
 	EPS_LOG_FUNCIN

	BandTerm(&band);
	
	EPS_SAFE_RELEASE( Palette2DeviceIntensity );

	CmdBuffTerm(&cmdBuf);

	/* Halftoning Ending process */
	HT_End();

	EPS_RETURN_VOID
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageStartJob_M()													*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* N/A																					*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page start job commands.				                                */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageStartJob_M(void)
{
	EPS_ERR_CODE	ret = EPS_ERR_NONE;
	EPS_UINT32      retBufSize = 0;

	EPS_LOG_FUNCIN

#define pgStartJob_ADDCMD(CMD) {							\
		ret = CmdBuffGrow(&cmdBuf, sizeof(CMD) );			\
		if( EPS_ERR_NONE != ret){EPS_RETURN( ret )}			\
		memcpy(cmdBuf.p + cmdBuf.len, CMD, sizeof(CMD));	\
		cmdBuf.len += sizeof(CMD);							\
	}

	cmdBuf.len = 0;	/* reset */
	debug_msg("start Job MONO\n");
	ret = ejlStart(&cmdBuf, CmdBuffGrow);
	if( EPS_ERR_NONE != ret){
		EPS_RETURN( ret )
	}
	debug_msg("step 10.0\n");
    /* Step 1 - Start Job */
    /* Initialize the printer. */
	pgStartJob_ADDCMD(sbIP)
	/* Set Memory Mode Selection to Page memory mode */
	pgStartJob_ADDCMD(sbMMS);
	pgStartJob_ADDCMD(sbCSE);
    /* Set the minimum Unit Setting (600 or 300 resolution)	 */
	/* Dot Pattern Resolution Selection						 */
	if(       EPS_IR_600X600 == printJob.attr.inputResolution) {
		
		pgStartJob_ADDCMD(sbSU600);
		pgStartJob_ADDCMD(sbHTME600);
 		pgStartJob_ADDCMD(sbSDS600_9);
		pgStartJob_ADDCMD(sbSDS600_2);
		pgStartJob_ADDCMD(sbSDS600_1);
		pgStartJob_ADDCMD(sbSDS600_0);
	} 
	else if(EPS_IR_1200X1200 == printJob.attr.inputResolution) {
		
		pgStartJob_ADDCMD(sbSU1200);
		pgStartJob_ADDCMD(sbHTME1200);	
		pgStartJob_ADDCMD(sbSDS1200_9);
		pgStartJob_ADDCMD(sbSDS1200_2);
		pgStartJob_ADDCMD(sbSDS1200_1);
		pgStartJob_ADDCMD(sbSDS1200_0); 
		pgStartJob_ADDCMD(sbSS1200);
	}
	else if(EPS_IR_300X300 == printJob.attr.inputResolution) {
		
		pgStartJob_ADDCMD(sbSU300);
		pgStartJob_ADDCMD(sbHTME300);	
		pgStartJob_ADDCMD(sbSDS300_9);
		pgStartJob_ADDCMD(sbSDS300_2);
		pgStartJob_ADDCMD(sbSDS300_1);
		pgStartJob_ADDCMD(sbSDS300_0);
	}else{
		pgStartJob_ADDCMD(sbSU150);
		pgStartJob_ADDCMD(sbSDS150);
	}

	/* Set Screen Mode */

	pgStartJob_ADDCMD(sbTSESS);

		
	/* Set Screen Pattern */
	pgStartJob_ADDCMD(sbOWES);

	

		/* Does not perform automatic carriage return line feed */
	pgStartJob_ADDCMD(sbACRLFS);

	/* Does not perform automatic Form Feed */
	pgStartJob_ADDCMD(sbAFFS);
	
	pgStartJob_ADDCMD(sbCLFP);
	
		/* Set current position move mode to print pitch */
	pgStartJob_ADDCMD(sbPMPPS);
	
	/* Set Bit Image Record Store Count */
	pgStartJob_ADDCMD(sbBIRCS);

		/* Set Clip Mode */
	pgStartJob_ADDCMD(sbCMS);

	   /* Select compression type */
	pgStartJob_ADDCMD(sbBCI1S);

	pgStartJob_ADDCMD(sbMLG);

		/* Set Absolute graph coordinate mode */
	pgStartJob_ADDCMD(sbSARGAS);

		/* Set Screen Pattern */
	pgStartJob_ADDCMD(sbSPES);

	pgStartJob_ADDCMD(sbCCE);

	if(		EPS_IR_1200X1200 == printJob.attr.inputResolution){
		pgStartJob_ADDCMD(sbHTME1200);	
	}
	else if(EPS_IR_300X300 == printJob.attr.inputResolution){
		pgStartJob_ADDCMD(sbHTME300);
	}
	else if(EPS_IR_600X600 == printJob.attr.inputResolution){
		pgStartJob_ADDCMD(sbHTME600);
	}

	//pgStartJob_ADDCMD(sbBGPG);

	pgStartJob_ADDCMD(sbCAPG);

	/* set duplex */
	if(printJob.attr.duplex == EPS_DUPLEX_NONE){
		pgStartJob_ADDCMD(sbSDE0);
	} else{
		pgStartJob_ADDCMD(sbSDE1);
		if(printJob.attr.duplex == EPS_DUPLEX_SHORT){
			pgStartJob_ADDCMD(sbBDE1);
		} else{
			pgStartJob_ADDCMD(sbBDE0);
		}
	}



	/* Set Screen Record Count */
	pgStartJob_ADDCMD(sbSPRCS);
	
	/* Set Font Attribute Store Count */
	pgStartJob_ADDCMD(sbFASCS);

	/* Set Current Position Store Count */
	pgStartJob_ADDCMD(sbCPSCS);
	
	

	/* Set Macro Record Store Count */
	pgStartJob_ADDCMD(sbMRCS);
	
	
	/* Set Manual Feed */
	if(printJob.attr.manualFeed == 1)
	{
		pgStartJob_ADDCMD(sbIUE0);	/* manual */
	} else{
		switch(printJob.attr.paperSource){
		case IPS_MPTID_AUTO:
			pgStartJob_ADDCMD(sbIUE1);	
			break;
		case IPS_MPTID_TRAY1:
			pgStartJob_ADDCMD(sbIUE2);
			break;
		case IPS_MPTID_TRAY2:
			pgStartJob_ADDCMD(sbIUE3);
			break;
		case IPS_MPTID_TRAY3:
			pgStartJob_ADDCMD(sbIUE4);
			break;
		case IPS_MPTID_TRAY4:
			pgStartJob_ADDCMD(sbIUE5);
			break;
		case IPS_MPTID_TRAY5:
			pgStartJob_ADDCMD(sbIUE6);
			break;
		case IPS_MPTID_TRAY6:
			pgStartJob_ADDCMD(sbIUE7);
			break;
		case IPS_MPTID_TRAY7:
			pgStartJob_ADDCMD(sbIUE8);
			break;
		case IPS_MPTID_TRAY8:
			pgStartJob_ADDCMD(sbIUE9);
			break;
		case IPS_MPTID_TRAY9:
			pgStartJob_ADDCMD(sbIUE10);
			break;
		case IPS_MPTID_TRAY10:
			pgStartJob_ADDCMD(sbIUE11);
			break;
		case IPS_MPTID_TRAY11:
			pgStartJob_ADDCMD(sbIUE12);
			break;
		case IPS_MPTID_TRAY12:
			pgStartJob_ADDCMD(sbIUE13);
			break;
		case IPS_MPTID_TRAY13:
			pgStartJob_ADDCMD(sbIUE14);
			break;
		case IPS_MPTID_TRAY14:
			pgStartJob_ADDCMD(sbIUE15);
			break;
		case IPS_MPTID_TRAY15:
			pgStartJob_ADDCMD(sbIUE16);
			break;
		default:
			pgStartJob_ADDCMD(sbIUE1);
			break;
		}
	} 



	debug_msg("step 1\n");
	/* Set Paper Size */
	switch ( printJob.attr.mediaSizeIdx ) {
	case EPS_MSID_A4:				pgStartJob_ADDCMD(sbPSSA4);	break;
	case EPS_MSID_A5:  			pgStartJob_ADDCMD(sbPSSA5); 	break;
	case EPS_MSID_HLT: 			pgStartJob_ADDCMD(sbPSSHLT); 	break;
	case EPS_MSID_EXECUTIVE: 	pgStartJob_ADDCMD(sbPSSEXE);	break;
	case EPS_MSID_B:				pgStartJob_ADDCMD(sbPSSB);		break;
	case EPS_MSID_GLT:			pgStartJob_ADDCMD(sbPSSGLT);	break;
	case EPS_MSID_GLG:			pgStartJob_ADDCMD(sbPSSGLG);	break;
	case EPS_MSID_MON:			pgStartJob_ADDCMD(sbPSSMON);	break;
	case EPS_MSID_C10:			pgStartJob_ADDCMD(sbPSSC10);	break;
	case EPS_MSID_DL:				pgStartJob_ADDCMD(sbPSSDL);	break;
	case EPS_MSID_C5:				pgStartJob_ADDCMD(sbPSSC5);	break;
	case EPS_MSID_C6:				pgStartJob_ADDCMD(sbPSSC6);	break;
	case EPS_MSID_IB5:			pgStartJob_ADDCMD(sbPSSIB5);	break;
	case EPS_MSID_F4:				pgStartJob_ADDCMD(sbPSSF4);	break;
	case EPS_MSID_B5:				pgStartJob_ADDCMD(sbPSSB5);	break;
	case EPS_MSID_B4:				pgStartJob_ADDCMD(sbPSSB4);	break;	
	case EPS_MSID_A3:				pgStartJob_ADDCMD(sbPSSA3);	break;
	case EPS_MSID_LETTER:		pgStartJob_ADDCMD(sbPSSL);	break;
	case EPS_MSID_LEGAL:			pgStartJob_ADDCMD(sbPSSLE);	break;
	//case EPS_MSID_POSTCARD:	pgStartJob_ADDCMD(sbPSSPS);	break;
	default:							pgStartJob_ADDCMD(sbPSSA4);	break;
	}

	/* select paper type */

	switch(printJob.attr.mediaTypeIdx){
	case EPS_MTID_PLAIN: 		pgStartJob_ADDCMD(sbPTE1); 	break;
	case EPS_MTID_SEMITHICK:	pgStartJob_ADDCMD(sbPTE2); 	break;
	case EPS_MTID_PREPRINTED:	pgStartJob_ADDCMD(sbPTE3); 	break;
	case EPS_MTID_LETTERHEAD:	pgStartJob_ADDCMD(sbPTE4); 	break;
	case EPS_MTID_RECYCLED: 	pgStartJob_ADDCMD(sbPTE5); 	break;
	case EPS_MTID_COLOR: 		pgStartJob_ADDCMD(sbPTE6); 	break;
	case EPS_MTID_LABEL: 		pgStartJob_ADDCMD(sbPTE7); 	break;
	case EPS_MTID_THICK: 		pgStartJob_ADDCMD(sbPTE8); 	break;
	case EPS_MTID_COATED: 		pgStartJob_ADDCMD(sbPTE9); 	break;
	case EPS_MTID_SPECIAL:		pgStartJob_ADDCMD(sbPTE10); 	break;
	case EPS_MTID_TRANSPARENT:	pgStartJob_ADDCMD(sbPTE11); 	break;
	case EPS_MTID_UNSPECIFIED:	pgStartJob_ADDCMD(sbPTE0);	break;
	default:							pgStartJob_ADDCMD(sbPTE0);		break;
	}

	/* Set paper direction */
	if(printJob.attr.orienTation == 1) 
	{
		pgStartJob_ADDCMD(sbPDS1);  /* Landscape */
	} else{
		pgStartJob_ADDCMD(sbPDS0);	/* Potrait */
	}

	ret = ejlPageEsc(&cmdBuf, CmdBuffGrow);
	if( EPS_ERR_NONE != ret){
		EPS_RETURN( ret )
	}
	debug_msg("step 2\n");

	

	/* Set the bit image drawing offset to 0 */
	pgStartJob_ADDCMD(sbBIOS);

	/* Set current position to 0 after Bit image draw */
	pgStartJob_ADDCMD(sbABPS);

	/* Set line type record count to 160 */
	pgStartJob_ADDCMD(sbILGS);

    /* Step 4 - Adjust Vertical Print Position (if necessary) */
    /* CR */
    pgStartJob_ADDCMD(sbCR);
    
 






	ret = SendCommand((EPS_UINT8*)cmdBuf.p, cmdBuf.len, &retBufSize, TRUE);

    pageCount = 0;

	EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageEndJob_M()														*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* N/A																					*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page end job commands.					                                */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageEndJob_M()
{
	EPS_ERR_CODE	ret = EPS_ERR_NONE;           /* Return status of internal calls  */
	EPS_UINT32      retBufSize = 0;

	EPS_LOG_FUNCIN

	cmdBuf.len = 0;	/* reset */

	/* Check empty document */
	if( iRow != 0 ) {
		/* Formfeed (FF) */
		memcpy((EPS_INT8*)cmdBuf.p, sbFF, sizeof(sbFF));
		cmdBuf.len += sizeof(sbFF);

		/* Send command to initialize the printer. */
		memcpy((EPS_INT8*)cmdBuf.p+cmdBuf.len, sbIP, sizeof(sbIP));
		cmdBuf.len += sizeof(sbIP);
	}

	/*** Make EJL Command ***/
	ret = ejlEnd(&cmdBuf, CmdBuffGrow, pageCount);
	if( EPS_ERR_NONE != ret){
		EPS_RETURN( ret )
	}

	ret = SendCommand((EPS_UINT8*)cmdBuf.p, cmdBuf.len, &retBufSize, TRUE);

	EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageStartPage_M()													*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* N/A																					*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page start page commands.					                            */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageStartPage_M()
{
	debug_msg("check call function pageStartPage_M \n");
	EPS_ERR_CODE ret = EPS_ERR_NONE;
	EPS_UINT32  retBufSize = 0;

	EPS_LOG_FUNCIN

	HT_StartPage();
    iRow = 0;

	if (pageCount++) {

		cmdBuf.len = 0;	/* reset */

		memcpy(cmdBuf.p, sbFF, sizeof(sbFF));
		cmdBuf.len += sizeof(sbFF);

		ret = ejlPageEsc(&cmdBuf, CmdBuffGrow);
		if( EPS_ERR_NONE != ret){
			EPS_RETURN( ret )
		}

		ret = SendCommand((EPS_UINT8*)cmdBuf.p, cmdBuf.len, &retBufSize, TRUE);
    }

	EPS_RETURN( ret );
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageColorRow_M()													*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* N/A																					*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page raster commands.						                            */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageColorRow_M(        
						  
		const EPS_BANDBMP*  pInBmp,
        EPS_RECT*           pBandRec

){
 	EPS_ERR_CODE	ret = EPS_ERR_NONE;

	EPS_LOG_FUNCIN

    if( (EPS_UINT32)(pBandRec->right - pBandRec->left) <= printJob.printableAreaWidth){
        band.currentWidthInPixels = (EPS_UINT16)(pBandRec->right - pBandRec->left);
    } else{
        band.currentWidthInPixels = (EPS_UINT16) printJob.printableAreaWidth;
    }

	ConvertBYTE2Intensity( pInBmp, &band );

    memset( band.pRasterData, 0, band.WidthInBytes);
	HT_Scan( band.pIntensity, (EPS_UINT8*)band.pRasterData, band.currentWidthInPixels);

	iNumBytes = 0;
	iNumDots = 0;

	BandEncode(&band);

    if( band.encDataSize ) {
		ret = BandEmit(&band, iNumBytes, iNumDots);
    }

	iRow++;

	EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageSendLeftovers_M()												*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* N/A																					*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_INVALID_CALL            - This call was unnecessary                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      send leftovers data.                                                            */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageSendLeftovers_M(
							   
		   void
							   
){
	EPS_ERR_CODE	ret = EPS_ERR_NONE;
	EPS_UINT32      retBufSize = 0;

	EPS_LOG_FUNCIN

	if( NULL != printJob.contData.sendData && 0 < printJob.contData.sendDataSize){
		/* send command */
		ret = SendCommand(printJob.contData.sendData, 
								printJob.contData.sendDataSize, &retBufSize, TRUE);
	} else{
		ret = EPS_ERR_INVALID_CALL;
	}
	
 	EPS_RETURN( ret )
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%                                             %%%%%%%%%%%%%%%%%%%%%*/
/*--------------------               Local Functions               ---------------------*/
/*%%%%%%%%%%%%%%%%%%%%                                             %%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   CmdBuffInit()														*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* pCmdBuff     EPS_COMMAND_BUFFER*     I: command buffer strcture                      */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*                                                                                      */
/* Description:                                                                         */
/*      Allocate command buffer.			                                            */
/*                                                                                      */
/*******************************************|********************************************/
static EPS_ERR_CODE CmdBuffInit(
								
		EPS_COMMAND_BUFFER *pCmdBuff
		
){
	pCmdBuff->size = EPS_EJL_LINEMAX*2;
	pCmdBuff->p = (EPS_INT8*)EPS_ALLOC(pCmdBuff->size);
	if( NULL == pCmdBuff->p){
		return EPS_ERR_MEMORY_ALLOCATION;
	}

	pCmdBuff->len = 0;

	return EPS_ERR_NONE;
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   CmdBuffGrow()														*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* pCmdBuff     EPS_COMMAND_BUFFER*     I: command buffer strcture                      */
/* addSize      EPS_INT32			    I: growing size                                 */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*                                                                                      */
/* Description:                                                                         */
/*      ReAllocate command buffer.			                                            */
/*                                                                                      */
/*******************************************|********************************************/
static EPS_ERR_CODE CmdBuffGrow(
								
		EPS_COMMAND_BUFFER *pCmdBuff, 
		EPS_INT32			addSize
		
){
	EPS_INT32	block, sizeNew;

	if(pCmdBuff->size < pCmdBuff->len + addSize){
		block = (((pCmdBuff->len+addSize) / EPS_PAGE_CMDBUFF_SIZE) + 1);
		sizeNew = block * EPS_PAGE_CMDBUFF_SIZE;
		pCmdBuff->p =  memRealloc(pCmdBuff->p, pCmdBuff->size, sizeNew);
		if( NULL == pCmdBuff->p){
			return EPS_ERR_MEMORY_ALLOCATION;
		}
		pCmdBuff->size = sizeNew;
	}

	return EPS_ERR_NONE;
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   CmdBuffTerm()														*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* pCmdBuff     EPS_COMMAND_BUFFER*     I: command buffer strcture                      */
/*                                                                                      */
/* Return value:                                                                        */
/* N/A																					*/
/*                                                                                      */
/* Description:                                                                         */
/*      Free command buffer.				                                            */
/*                                                                                      */
/*******************************************|********************************************/
static void CmdBuffTerm		(
									 
		EPS_COMMAND_BUFFER	*pCmdBuff
									 
){
	EPS_SAFE_RELEASE(pCmdBuff->p);
	pCmdBuff->len = 0;
	pCmdBuff->size = 0;
	pCmdBuff->pExtent = NULL;
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   ConvertBYTE2Intensity()												*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* paletteSize  EPS_UINT16			    I: Palette Size									*/
/* paletteData  EPS_UINT8*			    I: Palette Size									*/
/*                                                                                      */
/* Return value:                                                                        */
/* N/A																					*/
/*                                                                                      */
/* Description:                                                                         */
/*      Convert palette to Intensity.													*/
/*                                                                                      */
/*******************************************|********************************************/
static void ConvertBYTE2Intensity(
								  
		const EPS_BANDBMP* pInBmp, 
		EPS_PAGE_BAND_M *pBand
		
){
	EPS_UINT32 idx;
	EPS_INT32 i, j, widthBytes;

	EPS_LOG_FUNCIN;

	widthBytes = pBand->currentWidthInPixels * printJob.bpp;
	j = 0;
		
	debug_msg("printJob.bpp = %d\n", printJob.bpp);
	if(printJob.bpp == 3){
		for (i = 0; i < widthBytes; i += printJob.bpp) {
			/* idx = CALCULATE_INTENSITY(pInBmp->bits[i], pInBmp->bits[i+1], pInBmp->bits[i+2]);*/
			idx = pInBmp->bits[i];
			idx = 255 - idx;
			if (idx > 255) idx = 255;

			pBand->pIntensity[j++] = 255-Intensity2Intensity[255-idx];

			if(j >= pBand->WidthInPixels){
				break;
			}
	   }
	} else{
		for (i = 0; i < widthBytes; i += printJob.bpp) {
			idx = pInBmp->bits[i];
			pBand->pIntensity[j++] = Palette2DeviceIntensity[idx];
			if(j >= pBand->WidthInPixels){
				break;
			}
		}
	}
 
   EPS_RETURN_VOID
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   ConvertPaletteToIntensity()											*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* paletteSize  EPS_UINT16			    I: Palette Size									*/
/* paletteData  EPS_UINT8*			    I: Palette Size									*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*                                                                                      */
/* Description:                                                                         */
/*      Convert palette to Intensity.													*/
/*                                                                                      */
/*******************************************|********************************************/
static EPS_ERR_CODE ConvertPaletteToIntensity(

		EPS_UINT16  paletteSize,
		EPS_UINT8	*paletteData
					   
){
	EPS_UINT32 idx;
	EPS_UINT32 i, j;
	EPS_LOG_FUNCIN;

	Palette2DeviceIntensity = (EPS_UINT8*)EPS_ALLOC( (paletteSize/3)*sizeof(EPS_UINT8) );
	if (!Palette2DeviceIntensity) {
		EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
	}
	j = 0;
    for (i = 0; i < paletteSize; i += 3) {
		/* idx = CALCULATE_INTENSITY(paletteData[i], paletteData[i+1], paletteData[i+2]); */
		idx = paletteData[i];
		idx = 255 - idx;
        if (idx > 255) idx = 255;
		Palette2DeviceIntensity[j++] = Intensity2Intensity[idx];
	}

	EPS_RETURN( EPS_ERR_NONE )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   BandInit()															*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* pBand		EPS_PAGE_BAND_M*		I: band data strcture		                    */
/* widthInPixels EPS_INT32			    I: width in Pixels								*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*                                                                                      */
/* Description:                                                                         */
/*      Allocate band data buffers.														*/
/*                                                                                      */
/*******************************************|********************************************/
static EPS_ERR_CODE BandInit(
							 
		EPS_PAGE_BAND_M*	pBand, 
		EPS_INT32		widthInPixels
		
){
	EPS_ERR_CODE	ret = EPS_ERR_NONE;
	EPS_INT32 maxEpsonEncodedData;

	EPS_LOG_FUNCIN;

	memset(pBand, 0, sizeof(pBand));

    pBand->WidthInPixels = widthInPixels;
    pBand->WidthInBytes = (pBand->WidthInPixels+7)/8;

    /* We leave room for an entire TIFF literal -plus- room	*/
    /* for the longest possible XFER header (3 bytes).		*/
    maxEpsonEncodedData = 256 + 3 + pBand->WidthInBytes
        + (pBand->WidthInBytes + TIFF_MAXIMUM_LITERAL - 1)/TIFF_MAXIMUM_LITERAL;

    pBand->pRasterData       = (EPS_UINT8*)EPS_ALLOC(pBand->WidthInBytes);
    if (NULL == pBand->pRasterData) {
		EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
    }

	pBand->pEncData = (EPS_UINT8*)EPS_ALLOC(maxEpsonEncodedData);
    if (NULL == pBand->pEncData) {
		EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
    }

	pBand->pZeroRow			 = (EPS_UINT8*)EPS_ALLOC(pBand->WidthInBytes);
    if (NULL == pBand->pZeroRow) {
		EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
    }
    memset(pBand->pZeroRow, 0, pBand->WidthInBytes);

    pBand->pIntensity        = (EPS_UINT8*)EPS_ALLOC(pBand->WidthInPixels);
	if (!pBand->pIntensity){
		EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
	}

	EPS_RETURN(ret)
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   BandTerm()															*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* pBand		EPS_PAGE_BAND_M*		I: band data strcture		                    */
/*                                                                                      */
/* Return value:                                                                        */
/* N/A																					*/
/*                                                                                      */
/* Description:                                                                         */
/*      free band data buffers.															*/
/*                                                                                      */
/*******************************************|********************************************/
static void BandTerm(
					 
		EPS_PAGE_BAND_M* pBand
		
){
	EPS_LOG_FUNCIN

	EPS_SAFE_RELEASE( pBand->pRasterData );
	EPS_SAFE_RELEASE( pBand->pEncData );
	EPS_SAFE_RELEASE( pBand->pZeroRow );
	EPS_SAFE_RELEASE( pBand->pIntensity );

	EPS_RETURN_VOID
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   BandEmit()															*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* pBand		EPS_PAGE_BAND_M*		I: band data strcture		                    */
/* pCmdBuff     EPS_COMMAND_BUFFER*     I: command buffer strcture                      */
/* iNByte		EPS_INT32				I: number of data bytes							*/
/* iNDot		EPS_INT32				I: number of data dots							*/
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success										*/
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Emit data to printer.															*/
/*                                                                                      */
/*******************************************|********************************************/
static EPS_ERR_CODE BandEmit(
							 
		EPS_PAGE_BAND_M*		pBand, 
		EPS_INT32			iNByte, 
		EPS_INT32			iNDot
							 
){
	EPS_ERR_CODE	ret = EPS_ERR_NONE;
	EPS_INT8		*p;
	EPS_UINT32      retBufSize = 0;

	EPS_LOG_FUNCIN

	cmdBuf.len = 0;	/* reset */
	p = (EPS_INT8*)cmdBuf.p;

	EPS_DBGPRINT(("Y = %d \r\n", iRow));
	sprintf(p, "\x1D%dY", iRow);
	p += strlen(p);

	/* Number of data bytes	 */
	/* Bit image width		 */
	/* Bit image height		 */
	/* Angle of rotation	 */
	sprintf(p, sbBID, iNByte, iNDot);
	p += strlen(p);

	cmdBuf.len += (EPS_UINT32)(p - cmdBuf.p);	
	ret = CmdBuffGrow(&cmdBuf, pBand->encDataSize);
	if( EPS_ERR_NONE != ret){
		EPS_RETURN( ret )
	}
	p = cmdBuf.p + cmdBuf.len;

	memcpy(p, pBand->pEncData, pBand->encDataSize);
	cmdBuf.len += pBand->encDataSize;

	pBand->encDataSize = 0;

	ret = SendCommand((EPS_UINT8*)cmdBuf.p, cmdBuf.len, &retBufSize, TRUE);

	EPS_RETURN(ret)
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   BandEncode()														*/
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:		Type:					Description:                                    */
/* pBand		EPS_PAGE_BAND_M*		I: band data strcture		                    */
/*                                                                                      */
/* Return value:                                                                        */
/* N/A																					*/
/*                                                                                      */
/* Description:                                                                         */
/*      Do Compression.																	*/
/*                                                                                      */
/*******************************************|********************************************/
static void BandEncode(
					   
		EPS_PAGE_BAND_M* pBand
		
){
	EPS_BOOL isZero = FALSE;
    EPS_INT32 widthInBytes = (pBand->currentWidthInPixels+7)/8;

	EPS_LOG_FUNCIN

	/* Check blank line */
	isZero = (memcmp(pBand->pRasterData, pBand->pZeroRow, widthInBytes) == 0)?TRUE:FALSE;

	/* No compression for blank line */
	if( isZero ) {
		pBand->encDataSize = 0;
	} else{
       	pBand->encDataSize = DoDeltaRow( pBand->pRasterData,
                                    widthInBytes,
                                    pBand->pEncData,
                                    pBand->pZeroRow
                                  );
	}

	EPS_RETURN_VOID
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      HT_Init											   */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Halftoning initialization						   */
/*                                                         */
/*=========================================================*/
static EPS_ERR_CODE HT_Init(EPS_INT32 WidthPixels)
{
	EPS_LOG_FUNCIN
	 
	srand(119);

    htParam.pEK			= NULL;
    htParam.iScanPels	= WidthPixels;
    htParam.iOrder		= 5;
    htParam.iDK			= DOT_K * E_MAX / 100;

	htParam.pEK = (EPS_INT16*)EPS_ALLOC((htParam.iScanPels + 2)*sizeof(EPS_INT16));
	if( NULL == htParam.pEK){
		EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
	}

	EPS_RETURN( EPS_ERR_NONE )
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      HT_End											   */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Halftoning Ending process						   */
/*                                                         */
/*=========================================================*/
static void HT_End()
{
EPS_LOG_FUNCIN
	EPS_SAFE_RELEASE( htParam.pEK );
EPS_RETURN_VOID
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      HT_StartPage									   */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Halftoning start page process					   */
/*                                                         */
/*=========================================================*/
static void HT_StartPage()
{
	EPS_LOG_FUNCIN

	if (htParam.pEK){
        memset(htParam.pEK, 0, (htParam.iScanPels + 2) * sizeof(EPS_INT16));
	}

	EPS_RETURN_VOID
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      HT_Scan											   */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Check halftoning method							   */
/*                                                         */
/*=========================================================*/
static void HT_Scan(EPS_UINT8 *Con, EPS_UINT8 *Bin, EPS_INT32 widthInPixels)
{
/*      Error Diffusion									   */
    EPS_INT32 i, j, k;
    EPS_INT32 iE, iE1, iE3, iE5, iE7, iR;
    EPS_INT32 iCon;
    EPS_INT32 iDot;
	EPS_INT32 iF;
    EPS_INT16 *p;

	EPS_LOG_FUNCIN

	p    = htParam.pEK + 1;
	iDot = htParam.iDK;

    iE1 = 0;
    iE7 = 0;
    iR  = 0;

	i = 0;
	j = 1;
    
    for (k = 0; k < widthInPixels; k++) {
        if (Con[i] == 0) {
        } else if (Con[i] == MAX_8) {
            Bin[i >> 3] |= BitMask[i & 7];
        } else {
            iCon = Con[i] * (E_MAX / (MAX_8 + 1));

            iE = rand() * (iCon - E_MID) / RAND_MAX;
            if (((iR > 0) && (iE > 0)) || ((iR < 0) && (iE < 0))) {
                iE = -iE;
			}
            iR = iE;

            iCon += p[i] + iE7;
            
            if (iCon + iR > E_MID) {
                Bin[i >> 3] |= BitMask[i & 7];
                iE = iCon - iDot;
            } else {
                iE = iCon;
            }

			iF = iE / 16;
            iE3 = iF * 3;
            iE5 = iF * 5;

            p[i]      = iE5 + iE1;
            p[i - j] += iE3;

            iE1 = iF;
            iE7 = iE - iE1 - iE3 - iE5;
        }

        i += j;
    }

    p[i - j] += iE7 + iE1;

    p[0] += p[-1];
    p[htParam.iScanPels - 1] += p[htParam.iScanPels];

    p[-1] = 0;
    p[htParam.iScanPels] = 0;
	
	EPS_RETURN_VOID
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      DoTIFF                                             */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Do TIFF compression                                */
/*                                                         */
/*=========================================================*/
EPS_INT32 DoTIFF(EPS_UINT8 *Row, EPS_INT32 nRow, EPS_UINT8 *RowTIFF)
{
    EPS_INT32 nRowTIFF = 0;      /* Total size of TIFF encoded row. */
    EPS_INT32 nLiteral = 0;      /* Count of literal bytes. */
    EPS_INT32 nRepeat  = 0;      /* Count of repeated byte. */
	EPS_UINT8 ch; 
	EPS_INT32 n;

    /* Load first byte from row into parser. */
    nRepeat++;
	nLiteral++;
    ch = *Row++;

    for( ; nRow--; ch = *Row++ ) {
        if( nLiteral > TIFF_MAXIMUM_LITERAL ) {
            /* Literal part has grown too large, emit the largest literal pattern we can. */
            EMIT_TIFF_LITERAL( TIFF_MAXIMUM_LITERAL, Row-nLiteral );
            nLiteral -= TIFF_MAXIMUM_LITERAL;
        } else if( nRepeat > TIFF_MAXIMUM_REPEAT ) {
            /* Repeated part has grown too large, emit the literal (if present) and then  */
			/* emit the largest repeated pattern we can. */
			EMIT_TIFF_REPEAT( TIFF_MAXIMUM_REPEAT, ch );
            nRepeat -= TIFF_MAXIMUM_REPEAT;
        }
        if( ch == *Row ) {
			if( nLiteral ) {
				if( nLiteral >= 2 ) {

					EMIT_TIFF_LITERAL( nLiteral-1, Row-nLiteral );
				}
				nLiteral = 1;
			}
            nRepeat++;
        } else {
			if( nRepeat >= 2 ) {
				EMIT_TIFF_REPEAT( nRepeat, ch );
				nRepeat = 1;
				nLiteral = 0;
			}
			nLiteral++;
        }
    }
    if( nRepeat == 1 ) {
        nRepeat  = 0;
    }
	if( nLiteral == 1 && nRepeat >= 2 ) {
        nLiteral = 0;
    }

    while (nLiteral) {
        n = nLiteral;
        if (n > TIFF_MAXIMUM_LITERAL) {
            n = TIFF_MAXIMUM_LITERAL;
        }
		if( nRepeat ) {
			EMIT_TIFF_LITERAL(n-1, Row-nLiteral);
		} else {
			EMIT_TIFF_LITERAL(n-1, Row-nLiteral);
		}
        nLiteral -= n;
    }

    while (nRepeat) {
        n = nRepeat;
        if (n > TIFF_MAXIMUM_REPEAT)  {
            n = TIFF_MAXIMUM_REPEAT;
        }
        EMIT_TIFF_REPEAT(n, ch);
        nRepeat -= n;
    }

	return nRowTIFF;
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      DeltaRowEmitXFER                                   */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Construct DeltaRow control code and data           */
/*                                                         */
/*=========================================================*/
void DeltaRowEmitXFER(EPS_UINT8 *Row, EPS_INT32 nXFER, EPS_INT32 nXFERTIFF, EPS_INT32 *nRowDeltaRow, EPS_UINT8 **RowDeltaRow)
{
	EPS_LOG_FUNCIN;

    /* Get data length and send before all data of the current row */
    iNumBytes += nXFERTIFF;
	iNumDots += nXFER * 8;

    /* TIFF data for XFER */
    *nRowDeltaRow += nXFERTIFF;
    if (*RowDeltaRow)
    {
        DoTIFF(Row, nXFER, *RowDeltaRow);
        *RowDeltaRow += nXFERTIFF;
    }

	EPS_RETURN_VOID
}

/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      DeltaRowEmitMOVX                                   */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Construct DeltaRow horizontal position movement    */
/*                                                         */
/*=========================================================*/
void DeltaRowEmitMOVX(EPS_INT32 nSkip, EPS_INT32 *nRowDeltaRow, EPS_UINT8 **RowDeltaRow)
{
    EPS_UINT8 N;
    EPS_INT32 num=0;
    EPS_INT32 num1=0;

	EPS_LOG_FUNCIN;

    /* Send repeat "00" instead. Send 255 "00" the most each time */
    if( nSkip == 0 ) {
		EPS_RETURN_VOID
    }
    
    if( nSkip > 255 ) {
        num1 = nSkip - 255;
        num = 255;
    } else {
        num = nSkip;
    }
    *nRowDeltaRow += 3;
    iNumBytes += 3;
	iNumDots += num * 8;

    if (*RowDeltaRow) {
        N = num;
        *((*RowDeltaRow)++) = 0x00;
		*((*RowDeltaRow)++) = 0x00;
		*((*RowDeltaRow)++) = N;
    }
    if( num1 > 255 ) {
        /* Still have more than 255 "00" */
        DeltaRowEmitMOVX(num1, nRowDeltaRow, RowDeltaRow);
    } else {
        if( num1 > 0 ) {
            *nRowDeltaRow += 3;
            iNumBytes += 3;
			iNumDots += num1 * 8;
            if (*RowDeltaRow)
            {
                N = num1;
                *((*RowDeltaRow)++) = 0x00;
				*((*RowDeltaRow)++) = 0x00;
				*((*RowDeltaRow)++) = N;
            }
        }
    }

	EPS_RETURN_VOID
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      DeltaRowEmitReduce                                 */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*                                                         */
/*                                                         */
/*=========================================================*/
void DeltaRowEmitReduce(EPS_UINT8 *Row, EPS_INT32 *nDelta0, EPS_INT32 *nSkip, EPS_INT32 *nDelta1, EPS_INT32 *nRowDeltaRow, EPS_UINT8 **RowDeltaRow)
{
    EPS_INT32 nDelta0TIFF, nDelta1TIFF, nDeltaTIFF, SizeofDSD, SizeofD;

	EPS_LOG_FUNCIN;
	
	if (*nDelta0) {
        /* (DSD) state. */
        nDelta0TIFF = DoTIFF(Row - *nDelta1 - *nSkip - *nDelta0, *nDelta0, 0);
        nDelta1TIFF = DoTIFF(Row - *nDelta1,                     *nDelta1, 0);
        nDeltaTIFF  = DoTIFF(Row - *nDelta1 - *nSkip - *nDelta0, *nDelta0 + *nSkip + *nDelta1, 0);

        SizeofDSD = nDelta0TIFF + nDelta1TIFF + (nDelta0TIFF>15?1:0)
            + (nDelta0TIFF>255?1:0) + (nDelta1TIFF>15?1:0)
            + (nDelta1TIFF>255?1:0) + (*nSkip>7?1:0) + (*nSkip>127?1:0) + 3;
        SizeofD = nDeltaTIFF + (nDeltaTIFF>15?1:0) + (nDeltaTIFF>255?1:0) + 1;

    } else {
        /* (-SD) state. */
        nDelta0TIFF = 0;
        nDelta1TIFF = DoTIFF(Row - *nDelta1, *nDelta1, 0);
        nDeltaTIFF  = DoTIFF(Row - *nDelta1 - *nSkip, *nSkip + *nDelta1, 0);

        SizeofDSD = nDelta1TIFF + (nDelta1TIFF>15?1:0)
            + (nDelta1TIFF>255?1:0) + (*nSkip>7?1:0) + (*nSkip>127?1:0) + 2;
        SizeofD = nDeltaTIFF + (nDeltaTIFF>15?1:0) + (nDeltaTIFF>255?1:0) + 1;
    }

    if (SizeofD <= SizeofDSD) {
        /* Turn the delta/skip/delta into a single delta. */
        *nDelta0 += *nSkip + *nDelta1;
        *nSkip = 0;
        *nDelta1 = 0;

    } else {
        /* Emit a delta, then a skip, and the shift the leftover delta. */
        if (*nDelta0) {
            DeltaRowEmitXFER(Row- *nDelta0 - *nSkip - *nDelta1, *nDelta0, nDelta0TIFF, nRowDeltaRow, RowDeltaRow);
        }
        DeltaRowEmitMOVX(*nSkip, nRowDeltaRow, RowDeltaRow);

        /* Shift remaining delta */
        *nDelta0 = *nDelta1;
        *nSkip = 0;
        *nDelta1 = 0;
    }

	EPS_RETURN_VOID
}


/*=========================================================*/
/*                                                         */
/* Function:                                               */
/*                                                         */
/*      DoDeltaRow                                         */
/*                                                         */
/* Description:                                            */
/*                                                         */
/*      Do DeltaRow compression                            */
/*                                                         */
/*=========================================================*/
static EPS_INT32 DoDeltaRow(EPS_UINT8 *Row, EPS_INT32 nRow, EPS_UINT8 *RowDeltaRow, EPS_UINT8 *Seed)
{
	EPS_INT32 nRowDeltaRow = 0;
    EPS_INT32 nSkip        = 0;
    EPS_INT32 nDelta1      = 0;
    EPS_INT32 nDelta0      = 0;
	EPS_INT32 nDelta0TIFF;

    for ( ; nRow--; Row++, Seed++) {
		if (*Row != *Seed) {
			nDelta1++;
		}
        else
        {
            if (nDelta1)
            {
                if (nDelta0 == 0 && nSkip ==0)
                {
                    nDelta0 = nDelta1;
                    nDelta1 = 0;
                }
                else
                {
                    if (nSkip <= 3)
                    {
                        nDelta0 += nSkip + nDelta1;
                        nSkip = 0;
                        nDelta1 = 0;
                    }
                    else
                    {
                       DeltaRowEmitReduce(Row, &nDelta0, &nSkip, &nDelta1,
                            &nRowDeltaRow, &RowDeltaRow);
                    }
                }
            }
            nSkip++;
        }
    }

    /* Have reached the end of the line. */
    if (nDelta0)
    {
        if (nDelta1)
        {
            DeltaRowEmitReduce(Row, &nDelta0, &nSkip, &nDelta1, &nRowDeltaRow, &RowDeltaRow);
            nDelta0TIFF = DoTIFF(Row-nDelta0, nDelta0, 0);
            DeltaRowEmitXFER(Row-nDelta0, nDelta0, nDelta0TIFF, &nRowDeltaRow, &RowDeltaRow);
        }
        else
        {
            nDelta0TIFF = DoTIFF(Row-nSkip-nDelta0, nDelta0, 0);
            DeltaRowEmitXFER(Row-nDelta0-nSkip, nDelta0, nDelta0TIFF, &nRowDeltaRow, &RowDeltaRow);
        }
    }
    else if (nDelta1)
    {
        if (nSkip)
        {
            DeltaRowEmitReduce(Row, &nDelta0, &nSkip, &nDelta1, &nRowDeltaRow, &RowDeltaRow);
            nDelta0TIFF = DoTIFF(Row-nDelta0, nDelta0, 0);
            DeltaRowEmitXFER(Row-nDelta0, nDelta0, nDelta0TIFF, &nRowDeltaRow, &RowDeltaRow);
        }
        else
        {
            nDelta0TIFF = DoTIFF(Row-nDelta1, nDelta1, 0);
            DeltaRowEmitXFER(Row-nDelta1, nDelta1, nDelta0TIFF, &nRowDeltaRow, &RowDeltaRow);
        }
    }

    return nRowDeltaRow;
}

/*_______________________________  epson-escpage-mono.c   ______________________________*/

/*34567890123456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*       1         2         3         4         5         6         7         8        */
/*******************************************|********************************************/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/***** End of File *** End of File *** End of File *** End of File *** End of File ******/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
