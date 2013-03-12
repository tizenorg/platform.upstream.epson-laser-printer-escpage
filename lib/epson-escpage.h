/*__________________________________  epson-escpage.h   ________________________________*/

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
/*******************************************|********************************************/
/*                                                                                      */
/*                            Epson ESC/Page command Functions                          */
/*                                                                                      */
/*******************************************|********************************************/

#ifndef __EPSON_ESCPAGE_H__
#define __EPSON_ESCPAGE_H__
#ifdef __cplusplus
extern "C" {
#endif
/*------------------------------------  Includes   -------------------------------------*/
/*******************************************|********************************************/
#include "epson-escpr-pvt.h"
#include "epson-escpr-media.h"

/*------------------------------- Global Compiler Switch -------------------------------*/
/*******************************************|********************************************/
#define EPS_PAGE_RIT				(1)
#define EPS_PAGE_TONER_SAVE			(0)
#define EPS_PAGE_OUTUNIT_FACEDOWN	(1)
#define EPS_PAGE_LOWRES_MODE		(1)		/* OFF=0 / ON=1 */
#define EPS_PAGE_HT_ERR_DIFFUSION	(1)		/* Halftoning mode */

/*-----------------------------------  Definitions  ------------------------------------*/
/*******************************************|********************************************/
#define	dim(x)	(sizeof(x) / sizeof(x[0]))
#define EPS_EJL_LINEMAX				(256)

/*---------------------------  ESC/Page Media Declarations   ---------------------------*/
/*******************************************|********************************************/
typedef struct _tagEPS_PAGE_MEDIASIZE_ {
    EPS_INT32	id;
    EPS_INT32	paper_x;
    EPS_INT32	paper_y;
	  EPS_INT32	print_area_x_border;
	  EPS_INT32	print_area_y_border;
    const EPS_INT8 *name;
} EPS_PAGE_MEDIASIZE;

/* Size 600dpi */
static const EPS_PAGE_MEDIASIZE pageMediaSize[] = {
	{ EPS_MSID_A4,				4960, 7016, 4820, 6776, "A4" },
  { EPS_MSID_A3,		    7014, 9918, 6774, 9778, "A3" },
  { EPS_MSID_USB,       6600, 10200, 6460, 10060, "USB"},
	{ EPS_MSID_A5, 			  3496, 4960, 3356, 4820, "A5" },
  //{ EPS_MSID_A6, 	2480, 3496, 2340, 3356, "A6" },
	//{ EPS_MSID_B4,		6072, 8600, 5832, 8360, "B4" },
	{ EPS_MSID_GLG, 			5100, 7800, 4960, 7660, "GLG" },
	{ EPS_MSID_GLT, 			4800, 6300, 4660, 6160, "GLT" },
	{ EPS_MSID_F4, 			  4960, 7796, 4820, 7656, "F4"  },
	{ EPS_MSID_B5,				4300, 6072, 4060, 5832, "B5"  },
	{ EPS_MSID_LETTER,		5100, 6600, 4860, 6360, "LT"  },
	{ EPS_MSID_LEGAL,			5100, 8400, 4960, 8260, "LGL" },
	{ EPS_MSID_EXECUTIVE, 4350, 6300, 4210, 6160, "EXE" },
	{ EPS_MSID_HLT,			  3300, 5100, 3160, 4960, "HLT" },
	{ EPS_MSID_MON,			  2326, 4500, 2186, 4360, "MON" },
	{ EPS_MSID_C10,			  2476, 5700, 2336, 5560, "C10" },
	{ EPS_MSID_DL,				2580, 5220, 2440, 5080, "DL"  },
	{ EPS_MSID_C5,				3840, 5400, 3700, 5260, "C5"  },
	{ EPS_MSID_C6, 			  2700, 3840, 2560, 3700, "C6"  },
	{ EPS_MSID_IB5,			  4140, 5880, 4000, 5740, "IB5" }
	
	//{ EPS_MSID_POSTCARD,2362, 3496, 2122, 3256, "PC" }
};

/*---------------------------  Data Structure Declarations   ---------------------------*/
/*******************************************|********************************************/
/* command data buffer                */
typedef struct tagEPS_COMMAND_BUFFER 
{
	EPS_UINT32	size;		/* allocated buffer size */
	EPS_UINT32	len;		/* data size */
	EPS_INT8*	p;          
	void*		pExtent;
} EPS_COMMAND_BUFFER;

/*--------------------------  Public Function Declarations   ---------------------------*/
/*******************************************|********************************************/
extern EPS_ERR_CODE pageInitJob				(const EPS_JOB_ATTRIB *pJobAttr);
extern EPS_ERR_CODE pageAllocBuffer			(void);
extern void			pageRelaseBuffer		(void);
extern EPS_ERR_CODE pageStartJob			(void);
extern EPS_ERR_CODE pageEndJob				(void);
extern EPS_ERR_CODE pageStartPage			(void);
extern EPS_ERR_CODE pageEndPage				(void);
extern EPS_ERR_CODE pageColorRow			(const EPS_BANDBMP*, EPS_RECT*);
extern EPS_ERR_CODE pageSendLeftovers		(void);

    /*** Get Supported Media Function                                                   */
    /*** -------------------------------------------------------------------------------*/
extern EPS_ERR_CODE pageCreateMediaInfo  	(EPS_PRINTER_INN* printer, EPS_UINT8* pmString,
											 EPS_INT32 pmSize							);
extern void			pageClearSupportedMedia	(EPS_PRINTER_INN* printer					);

    /*** Get Printable Area                                                             */
    /*** -------------------------------------------------------------------------------*/
extern EPS_ERR_CODE pageGetPrintableArea  (EPS_JOB_ATTRIB*, EPS_UINT32*, EPS_UINT32*    );


/*-----------------------  ESC/Page Local Function Declarations   ----------------------*/
/*******************************************|********************************************/
typedef EPS_ERR_CODE	(*PAGE_CmdBuffGrow	)(EPS_COMMAND_BUFFER *pCmdBuff, EPS_INT32 addSize);

extern EPS_ERR_CODE ejlStart	(EPS_COMMAND_BUFFER *pCmdBuff, PAGE_CmdBuffGrow pfncGrow);
extern EPS_ERR_CODE ejlEnd		(EPS_COMMAND_BUFFER *pCmdBuff, PAGE_CmdBuffGrow pfncGrow,
								 EPS_INT32 pageCount									);
extern EPS_ERR_CODE ejlPageEsc	(EPS_COMMAND_BUFFER *pCmdBuff, PAGE_CmdBuffGrow pfncGrow);

#ifdef __cplusplus
}
#endif

#endif  /* def __EPSON_ESCPAGE_H__ */

/*__________________________________  epson-escpage.h   ________________________________*/
  
/*34567890123456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*       1         2         3         4         5         6         7         8        */
/*******************************************|********************************************/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/***** End of File *** End of File *** End of File *** End of File *** End of File ******/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
