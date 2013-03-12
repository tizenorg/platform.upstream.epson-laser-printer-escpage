/*__________________________________  epson-escpage.c   ________________________________*/

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
#include "epson-escpr-mem.h"
#include "epson-escpage-color.h"
#include "epson-escpage-mono.h"
#ifdef GCOMSW_CMD_ESCPAGE_S
#include "epson-escpage-s.h"
#endif
#include "epson-escpage.h"

/*-----------------------------  Local Macro Definitions -------------------------------*/
/*******************************************|********************************************/
#ifdef EPS_LOG_MODULE_PAGE
#define EPS_LOG_MODULE  EPS_LOG_MODULE_PAGE
#else
#define EPS_LOG_MODULE  0
#endif

/*-----------------------------------  EJL Commands ------------------------------------*/
/*******************************************|********************************************/
const static EPS_UINT8 sbEJL_ESC_SOH[] = "\x1B\x01";          /* Escape and start of heading <ESC><SOH> */
const static EPS_UINT8 sbEJL[]    = "@EJL";             /* EJL command */
const static EPS_UINT8 sbEJL_SP[] = "\x20";             /* Space <SP> */
const static EPS_UINT8 sbEJL_LF[] = "\x0A";             /* Line Feed <LF> */
const static EPS_UINT8 sbEJL_EN[] = "@EJL""\x20""EN""\x20""LA=ESC/PAGE";    /* EJL ENTER command */
const static EPS_UINT8 sbEJL_EN_C[] = "@EJL""\x20""EN""\x20""LA=ESC/PAGE-COLOR";/* EJL ENTER command */
const static EPS_UINT8 sbEJL_SE[] = "@EJL""\x20""SE""\x20""LA=ESC/PAGE";    /* EJL SELECT command */
const static EPS_UINT8 sbEJL_SET[]  = "SET";              /* SET command */

/*---------------------------------  ESC/Page Commands ---------------------------------*/
/*******************************************|********************************************/
const static EPS_INT8 sbLOS[]   = "\x1D%d;%dloE";         /* Logical Origin setting for 300 dpi(0.2 inch) */
const static EPS_UINT8 sbCHPAS[]    = {0x1D, '0', 'X'};         /* Reset the current absolute X position */
const static EPS_UINT8 sbCVPAS[]  = {0x1D, '0', 'Y'};         /* Reset the current absolute Y position */
const static EPS_INT8 sbCA[]    = "\x1D""0;0;%d;%dcaE";       /* Set Clip Area */


/*---------------------------  Data Structure Declarations   ---------------------------*/
/*******************************************|********************************************/
typedef struct _tagEPS_PAGE_MEDIA_NAME_ {
    EPS_INT32 id;
    const EPS_INT8 *name;
} EPS_PAGE_MEDIA_NAME;

/*---------------------------  ESC/Page Media Declarations   ---------------------------*/
/*******************************************|********************************************/
/* Type */
const EPS_PAGE_MEDIA_NAME pageMediaType[]  = {
  { EPS_MTID_PLAIN,     "NM" },
  { EPS_MTID_SEMITHICK,     "ST"},
  { EPS_MTID_PREPRINTED,    "PP"},
  { EPS_MTID_LETTERHEAD,    "LH"},
  { EPS_MTID_RECYCLED,    "RC"},
  { EPS_MTID_COLOR,       "CL"},
  { EPS_MTID_LABEL,     "LB"},
  { EPS_MTID_THICK,     "TK"},
  { EPS_MTID_COATED,      "CT"},
  { EPS_MTID_SPECIAL,     "SP"},
  { EPS_MTID_TRANSPARENT,   "TP"},
  { EPS_MTID_UNSPECIFIED,   "UN"}
};

/* Paper Source */
const EPS_PAGE_MEDIA_NAME pagePaperSource[]  = {
  { EPS_MPID_AUTO,     "AU" },
  /*{ EPS_MPID_MPTRAY,    "1" },
  { EPS_MPID_FRONT1,    "2" },
  { EPS_MPID_FRONT2,    "3" },
  { EPS_MPID_FRONT3,    "4" },
  { EPS_MPID_FRONT4,    "5" },*/
  {  IPS_MPTID_TRAY1,    "1" },
  {  IPS_MPTID_TRAY2,    "2" },
  {  IPS_MPTID_TRAY3,    "3" },
  {  IPS_MPTID_TRAY4,    "4" },
  {  IPS_MPTID_TRAY5,    "5" },
  {  IPS_MPTID_TRAY6,    "6" },
  {  IPS_MPTID_TRAY7,    "7" },
  {  IPS_MPTID_TRAY8,    "8" },
  {  IPS_MPTID_TRAY9,    "9" },
  {  IPS_MPTID_TRAY10,   "10" },
  {  IPS_MPTID_TRAY11,   "11" },
  {  IPS_MPTID_TRAY12,   "12" },
  {  IPS_MPTID_TRAY13,   "13" },
  {  IPS_MPTID_TRAY14,   "14" },
  {  IPS_MPTID_TRAY15,   "15" },
};


/*----------------------------  ESC/P-R Lib Global Variables  --------------------------*/
/*******************************************|********************************************/
extern EPS_CMN_FUNC   epsCmnFnc;
extern EPS_PRINT_JOB  printJob;

#if 0
#include "epsmp-make-img.h" 
static FILE *fp = NULL;
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%                                             %%%%%%%%%%%%%%%%%%%%%*/
/*--------------------              Public Functions               ---------------------*/
/*%%%%%%%%%%%%%%%%%%%%                                             %%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageInitJob()                                               */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* jobAttr      const EPS_JOB_ATTRIB*   I: Print Job Attribute                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      << Normal >>                                                                    */
/*      EPS_ERR_NONE                            - Success                               */
/*      << Error >>                                                                     */
/*      EPS_ERR_INVALID_VERSION                 - Invalid structure version             */
/*      EPS_ERR_INV_COLOR_PLANE                 - Invalid Color Plane                   */
/*      EPS_ERR_INV_PALETTE_SIZE                - Invalid Palette Size                  */
/*      EPS_ERR_INV_PALETTE_DATA                - Invalid Palette Data                  */
/*      EPS_ERR_INV_MEDIA_SIZE                  - Invalid Media Size                    */
/*      EPS_ERR_INV_MEDIA_TYPE                  - Invalid Media Type                    */
/*      EPS_ERR_INV_BORDER_MODE                 - Invalid Border Mode                   */
/*      EPS_ERR_INV_PAPER_SOURCE                - Invalid Paper source                  */
/*      EPS_ERR_INV_COLOR_MODE                  - Invalid Color Mode                    */
/*      EPS_ERR_INV_INPUT_RESOLUTION            - Invalid Input Resolution              */
/*      EPS_ERR_INV_DUPLEX                      - Invalid duplex                        */
/*      EPS_ERR_INV_FEED_DIRECTION              - Invalid feed direction                */
/*                                                                                      */
/* Description:                                                                         */
/*      Called form epsStartJob. Confirm ESC/Page Job Attribute.                        */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageInitJob(

    const EPS_JOB_ATTRIB *jobAttr
    
){
  EPS_INT32 i;
  const EPS_PAGE_MEDIASIZE *mediaSizeEnt = NULL;

  EPS_LOG_FUNCIN

/*** Validate input parameters                                                          */
  /*** Structure version                                                              */
  if(EPS_JOB_ATTRIB_VER_3 > jobAttr->version){
    EPS_RETURN( EPS_ERR_INVALID_VERSION )
  }

  /*** Color Plane                                                                    */
  if (! (   (jobAttr->colorPlane         == EPS_CP_FULLCOLOR         ) ||
        (jobAttr->colorPlane         == EPS_CP_256COLOR          ) ) ){
    EPS_RETURN( EPS_ERR_INV_COLOR_PLANE )
  }

  /*** Media Type                                                                     */
  for( i=0; i < dim(pageMediaType); i++ ) {
    if( jobAttr->mediaTypeIdx == pageMediaType[i].id ) {
      break;
    }
  }
  if(i >= dim(pageMediaType)){
    EPS_RETURN( EPS_ERR_INV_MEDIA_TYPE )
  }

#if 0
  /*** Print Quality                                                                  */
  if (! (   (jobAttr->printQuality       == EPS_MQID_NORMAL          ) ||
        (jobAttr->printQuality       == EPS_MQID_HIGH            )    ) ){
    EPS_RETURN( EPS_ERR_INV_PRINT_QUALITY )
  }
#endif
  /*** Input Image Resolution                                                         */
    if (! ( (jobAttr->inputResolution    == EPS_IR_150X150 ) ||
      (jobAttr->inputResolution    == EPS_IR_300X300 ) || 
      (jobAttr->inputResolution    == EPS_IR_1200X1200 )
#if !EPS_PAGE_LOWRES_MODE
      || (jobAttr->inputResolution    == EPS_IR_600X600 )
#endif
  ) ){
        EPS_RETURN( EPS_ERR_INV_INPUT_RESOLUTION )
  }

  /*** Paper Source                                                                   */
  debug_msg("__ Number = %d __\n", dim(pagePaperSource) );
  for( i=0; i < dim(pagePaperSource); i++ ) {
    if( jobAttr->paperSource == pagePaperSource[i].id ) {
      break;
    }
  }
  if(i >= dim(pagePaperSource)){
    debug_msg(" Invalid paper source: i = %d\n", i);
    EPS_RETURN( EPS_ERR_INV_PAPER_SOURCE )
  }

  /*** Border Mode                                                                    */
  if( jobAttr->printLayout != EPS_MLID_BORDERS ){
        EPS_RETURN( EPS_ERR_INV_BORDER_MODE )
  }

  /*** Color Mode                                                                     */
    if( !( (jobAttr->colorMode          == EPS_CM_COLOR             ) ||
           (jobAttr->colorMode          == EPS_CM_MONOCHROME        ) ) ){
    EPS_RETURN( EPS_ERR_INV_COLOR_MODE )
  }
  if( jobAttr->colorMode == EPS_CM_COLOR && 
    printJob.printer->language == EPS_LANG_ESCPAGE){
    EPS_RETURN( EPS_ERR_INV_COLOR_MODE )
  }

  /*** Pallette Data                                  */
  if (jobAttr->colorPlane == EPS_CP_256COLOR) {
    if (! ((jobAttr->paletteSize       >= 3                        ) &&
         (jobAttr->paletteSize       <= 768/*765*/               )    ) ){
      EPS_RETURN( EPS_ERR_INV_PALETTE_SIZE )
    }

    if (    jobAttr->paletteData       == NULL                     ){
      EPS_RETURN( EPS_ERR_INV_PALETTE_DATA )
    }
  }

  /*** duplex                                                                         */
  if( !( (jobAttr->duplex             == EPS_DUPLEX_NONE             ) ||
       (jobAttr->duplex             == EPS_DUPLEX_LONG             ) ||
           (jobAttr->duplex           == EPS_DUPLEX_SHORT            ) ) ){
    EPS_RETURN( EPS_ERR_INV_DUPLEX )
  }

  /*** Color Mode                                                                     */
  if( !( (jobAttr->feedDirection     == EPS_FEEDDIR_PORTRAIT         ) ||
           (jobAttr->feedDirection     == EPS_FEEDDIR_LANDSCAPE        ) ) ){
    EPS_RETURN( EPS_ERR_INV_FEED_DIRECTION )
  }
  

  memcpy(&printJob.attr, jobAttr, sizeof(printJob.attr));
  if(jobAttr->copies < 0){
    printJob.attr.copies = 1;
  }

  /*** Find the Media by ID                                                           */
  for(i = 0; i < dim(pageMediaSize); i++){
    if( jobAttr->mediaSizeIdx == pageMediaSize[i].id ){
      mediaSizeEnt = &pageMediaSize[i];
      break;
    }
  }
  if( NULL != mediaSizeEnt){
    printJob.printableAreaWidth   = mediaSizeEnt->print_area_x_border;
    printJob.printableAreaHeight  = mediaSizeEnt->print_area_y_border;
    printJob.topMargin        = 120;
        printJob.leftMargin       = 120;
  } else{
    EPS_RETURN( EPS_ERR_INV_MEDIA_SIZE )
  }

  if(EPS_IR_300X300 == jobAttr->inputResolution){
    printJob.printableAreaWidth   /= 2;
    printJob.printableAreaHeight  /= 2;
    printJob.topMargin        /= 2;
    printJob.leftMargin       /= 2;
  } else if(EPS_IR_150X150 == jobAttr->inputResolution){
    printJob.printableAreaWidth   /= 4;
    printJob.printableAreaHeight  /= 4;
    printJob.topMargin        /= 4;
    printJob.leftMargin       /= 4;
  }

  printJob.bpp = (printJob.attr.colorPlane == EPS_CP_256COLOR)?1:3;
  debug_msg("printJob.bpp = %d\n", printJob.bpp);

  printJob.whiteColorValue = memSearchWhiteColorVal(printJob.attr.colorPlane, 
                          printJob.attr.paletteData,
                          printJob.attr.paletteSize);

  EPS_RETURN( EPS_ERR_NONE )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageAllocBuffer()                                             */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success                   */
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*                                                                                      */
/* Description:                                                                         */
/*      Allocate buffer for ESC/Page Job.                       */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageAllocBuffer(void)
{
  EPS_ERR_CODE  ret = EPS_ERR_NONE;

  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    ret = pageS_AllocBuffer();
  } else
#endif
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    ret = pageAllocBuffer_C();
  } else{
    /** ESC/Page **/
    ret = pageAllocBuffer_M();
  }

  EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageRelaseBuffer()                                              */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      N/A                                                                             */
/*                                                                                      */
/* Description:                                                                         */
/*      Free buffer for ESC/Page Job.                                             */
/*                                                                                      */
/*******************************************|********************************************/
void pageRelaseBuffer(void)
{
  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    pageS_RelaseBuffer();
  } else
#endif
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    pageRelaseBuffer_C();
  } else{
    /** ESC/Page **/
    pageRelaseBuffer_M();
  }

  EPS_RETURN_VOID
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageStartJob()                            */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success                   */
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page start job commands.                                       */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageStartJob(void)
{
  EPS_ERR_CODE  ret = EPS_ERR_NONE;

  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    ret = pageS_StartJob();
  } else
#endif
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    debug_msg("call Start Job Color mode\n");
    ret = pageStartJob_C();
    debug_msg("call Start Job Color mode 1.0 \n");
  } else{
    /** ESC/Page **/
    debug_msg("startjob Mono\n");
    ret = pageStartJob_M();
  }

  EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageEndJob()                            */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success                   */
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page end job commands.                                         */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageEndJob()
{
  EPS_ERR_CODE  ret = EPS_ERR_NONE;           /* Return status of internal calls  */

  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    ret = pageS_EndJob();
  } else
#endif  
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    ret = pageEndJob_C();
  } else{
    /** ESC/Page **/
    ret = pageEndJob_M();
  }

  EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageStartPage()                           */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success                   */
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page start page commands.                                      */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageStartPage()
{
  debug_msg("check StartPage call function \n");
  EPS_ERR_CODE ret = EPS_ERR_NONE;

  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    ret = pageS_StartPage();
  } else
#endif
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    ret = pageStartPage_C();
  } else{
    /** ESC/Page **/
    ret = pageStartPage_M();
  }

  EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageEndPage()                           */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success                   */
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page end page commands.                                      */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageEndPage()
{
  EPS_ERR_CODE ret = EPS_ERR_NONE;

  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    ret = pageS_EndPage();
  } else
#endif
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    ret = pageEndPage_C();
  }

  EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageColorRow()                            */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success                   */
/*      EPS_ERR_MEMORY_ALLOCATION       - Fail to memory allocation                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      Send ESC/Page raster commands.                                        */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageColorRow(        
              
    const EPS_BANDBMP*  pInBmp,
        EPS_RECT*           pBandRec

){
  EPS_ERR_CODE ret = EPS_ERR_NONE;

  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    ret = pageS_ColorRow(pInBmp, pBandRec);
  } else
#endif
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    ret = pageColorRow_C(pInBmp, pBandRec);
  } else{
    /** ESC/Page **/
    ret = pageColorRow_M(pInBmp, pBandRec);
  }

  EPS_RETURN( ret )
}


/*******************************************|********************************************/
/*                                                                                      */
/* Function name:   pageSendLeftovers()                         */
/*                                                                                      */
/* Arguments                                                                            */
/* ---------                                                                            */
/* Name:    Type:         Description:                                    */
/* N/A                                          */
/*                                                                                      */
/* Return value:                                                                        */
/*      EPS_ERR_NONE                    - Success                   */
/*      EPS_ERR_INVALID_CALL            - This call was unnecessary                     */
/*      EPS_ERR_OPR_FAIL                - Internal Error                                */
/*      EPS_ERR_COMM_ERROR              - Communication Error                           */
/*      EPS_ERR_PRINTER_ERR_OCCUR       - Printer Error happened                        */
/*                                                                                      */
/* Description:                                                                         */
/*      send leftovers data.                                                            */
/*                                                                                      */
/*******************************************|********************************************/
EPS_ERR_CODE pageSendLeftovers(
                 
       void
                 
){
  EPS_ERR_CODE  ret = EPS_ERR_NONE;

  EPS_LOG_FUNCIN

#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    ret = pageS_SendLeftovers();
  } else
#endif
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    ret = pageSendLeftovers_C();
  } else{
    /** ESC/Page **/
    ret = pageSendLeftovers_M();
  }
  
  EPS_RETURN( ret )
}


EPS_ERR_CODE pageCreateMediaInfo (

    EPS_PRINTER_INN*  printer,
    EPS_UINT8*      pmString,
    EPS_INT32     pmSize

){
  EPS_INT32   num_mSize = dim(pageMediaSize);
    EPS_INT32       num_mType = dim(pageMediaType);
  EPS_INT32   i, j, n;
    EPS_MEDIA_SIZE* sizeList = NULL;
  EPS_MEDIA_TYPE* typeList = NULL;
  EPS_UINT32 paperSource = EPS_MPID_ALL_ESCPAGE;

  EPS_LOG_FUNCIN

  memset(&printer->supportedMedia, 0, sizeof(EPS_SUPPORTED_MEDIA));

    /* Allocate memory for the media size list. */
    sizeList = (EPS_MEDIA_SIZE*)EPS_ALLOC( sizeof(EPS_MEDIA_SIZE) * num_mSize );   
    if( sizeList == NULL ){
        EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
    }

    /* Allocate memory for the media type array. */
    typeList = (EPS_MEDIA_TYPE*)EPS_ALLOC( sizeof(EPS_MEDIA_TYPE) * num_mSize * num_mType );
    if (typeList == NULL) {
        EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION )
    }

  if( sizeof(EPS_UINT32) == pmSize ){
    memcpy(&paperSource, pmString, sizeof(EPS_UINT32));
  }

    /* set media size property. */
  for(i = 0; i < num_mSize; i++){
    /* set media type property. */
    for(j = 0; j < num_mType; j++){
      n = i * num_mType + j;
      typeList[n].mediaTypeID = pageMediaType[j].id;
      typeList[n].paperSource = paperSource;
      typeList[n].layout    = EPS_MLID_BORDERS;
#if EPS_PAGE_LOWRES_MODE
      typeList[n].quality   = EPS_MQID_DRAFT | EPS_MQID_NORMAL;
#else
      typeList[n].quality   = EPS_MQID_DRAFT | EPS_MQID_NORMAL | EPS_MQID_HIGH;
#endif

      if(EPS_MSID_POSTCARD == pageMediaSize[i].id){
        typeList[n].duplex = EPS_DUPLEX_DISABLE;
      } else{
        typeList[n].duplex = EPS_DUPLEX_ENABLE;
      }
    }
    
    sizeList[i].mediaSizeID = pageMediaSize[i].id;
    sizeList[i].numTypes  = num_mType;
    sizeList[i].typeList  = &typeList[i];
  }

  printer->supportedMedia.sizeList = sizeList;
    printer->supportedMedia.numSizes = num_mSize;

#if EPS_PAGE_LOWRES_MODE
  printer->supportedMedia.resolution = EPS_IR_150X150 | EPS_IR_300X300;
#else
  printer->supportedMedia.resolution = EPS_IR_150X150 | EPS_IR_300X300 | EPS_IR_600X600 | EPS_IR_1200X1200;
#endif

#if GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    pageS_UpdateSupportedMedia( &printer->supportedMedia );
  }
#endif

  EPS_RETURN( EPS_ERR_NONE )
}


void pageClearSupportedMedia  (
                 
    EPS_PRINTER_INN* printer        
    
){
  EPS_LOG_FUNCIN

  if( NULL != printer->supportedMedia.sizeList ){
    EPS_SAFE_RELEASE(printer->supportedMedia.sizeList[0].typeList);
    EPS_SAFE_RELEASE(printer->supportedMedia.sizeList);
  }

  EPS_RETURN_VOID
}


EPS_ERR_CODE    pageGetPrintableArea (

        EPS_JOB_ATTRIB*     jobAttr,
        EPS_UINT32*         printableWidth,
        EPS_UINT32*         printableHeight

){
  EPS_ERR_CODE  ret = EPS_ERR_NONE;
  EPS_INT32   num_mSize = dim(pageMediaSize);
  EPS_INT32   sizeIdx = 0;

  EPS_LOG_FUNCIN

/*** Validate/Confirm Page Attribute Data                                               */
    /*** Media Size                                                                     */
  for(sizeIdx = 0; sizeIdx < num_mSize; sizeIdx++){
    if(pageMediaSize[sizeIdx].id == jobAttr->mediaSizeIdx){
      break;
    }
  }
  if(sizeIdx >= num_mSize){
        EPS_RETURN( EPS_ERR_INV_MEDIA_SIZE )
  }

    /*** Border Mode                                                                    */
  if( jobAttr->printLayout != EPS_MLID_BORDERS ){
        EPS_RETURN( EPS_ERR_INV_BORDER_MODE )
  }

    /*** Input Image Resolution                                                         */
    if (! ( (jobAttr->inputResolution    == EPS_IR_150X150 ) ||
      (jobAttr->inputResolution    == EPS_IR_300X300 ) ||
      (jobAttr->inputResolution    == EPS_IR_600X600 ) ||
      (jobAttr->inputResolution    == EPS_IR_1200X1200 )) ){
        EPS_RETURN( EPS_ERR_INV_INPUT_RESOLUTION )
  }

  *printableWidth   = pageMediaSize[sizeIdx].print_area_x_border;
  *printableHeight  = pageMediaSize[sizeIdx].print_area_y_border;
#ifdef GCOMSW_CMD_ESCPAGE_S
  if(printJob.printer && EPS_LANG_ESCPAGE_S == printJob.printer->language){
    pageS_FeedExchange(jobAttr, printableWidth, printableHeight);
  }
#endif

  if(EPS_IR_300X300 == jobAttr->inputResolution){
    *printableWidth  /= 2;
    *printableHeight /= 2;
  } else if(EPS_IR_150X150 == jobAttr->inputResolution){
    *printableWidth  /= 4;
    *printableHeight /= 4;
    *printableHeight -= 2;
  }

  EPS_RETURN( ret )
}


EPS_ERR_CODE ejlStart (
               
    EPS_COMMAND_BUFFER *pCmdBuff, 
    PAGE_CmdBuffGrow pfncGrow
    
){
  EPS_ERR_CODE ret = EPS_ERR_NONE;
  EPS_INT32 i;
  EPS_INT8  *p;

  EPS_LOG_FUNCIN

#define EJLStart_ADDSTR(b, p, v)  \
  {               \
    sprintf(b, " %s=%s", p, v); \
    b += strlen(b);       \
  }
#define EJLStart_ADDINT(b, p, v)  \
  {               \
    sprintf(b, " %s=%d", p, v); \
    b += strlen(b);       \
  }

#ifdef GCOMSW_DEBUG
  if(pCmdBuff->len+EPS_EJL_LINEMAX > pCmdBuff->size){EPS_DBGPRINT(("Command buffer not enough!\r\n"))}
#endif

  p = pCmdBuff->p + pCmdBuff->len;

  /*** EJL commands ***/
  if( EPS_CM_COLOR == printJob.attr.colorMode ){
      /** ESC/Page-Color **/
      sprintf(p, "%s%s \x0A%s\x0A%s %s",
      sbEJL_ESC_SOH,  /* <ESC><SOH>     */
      sbEJL,      /* @EJL         */
      sbEJL_SE,   /* EJL SELECT command */
      sbEJL,      /* @EJL         */
      sbEJL_SET);   /* SET command      */
  } else{
    /** ESC/Page **/
    sprintf(p, "%s%s \x0A%s\x0A%s%s \x0A%s\x0A%s %s",
      sbEJL_ESC_SOH,  /* <ESC><SOH>     */
      sbEJL,      /* @EJL         */
      sbEJL_EN,   /* EJL ENTER command  */
      sbEJL_ESC_SOH,  /* <ESC><SOH>     */
      sbEJL,      /* @EJL         */
      sbEJL_SE,   /* EJL SELECT command */
      sbEJL,      /* @EJL         */
      sbEJL_SET);   /* SET command      */
  }
  p += strlen(p);

  /* RS */
  if(printJob.attr.inputResolution == EPS_IR_1200X1200){
    EJLStart_ADDSTR(p, "RS", "1200");
  } else if(EPS_IR_600X600 == printJob.attr.inputResolution){
    EJLStart_ADDSTR(p, "RS", "FN");
  } else{
    EJLStart_ADDSTR(p, "RS", "QK");
  }
  /* PS */
  for( i=0; i < dim(pageMediaSize); i++ ) {
    
    if( printJob.attr.mediaSizeIdx == pageMediaSize[i].id ) {
      break;
    }

  }

  EJLStart_ADDSTR(p, "PS", pageMediaSize[i].name);
  /* PK */
  for( i=0; i < dim(pageMediaType); i++ ) {
    if( printJob.attr.mediaTypeIdx == pageMediaType[i].id ) {
      break;
    }
  }
  debug_msg("print Job paperType = %d\n", printJob.attr.mediaTypeIdx);
  EJLStart_ADDSTR(p, "PK", pageMediaType[i].name);

  /* PU */
  for( i=0; i < dim(pagePaperSource); i++ ) {
    if( printJob.attr.paperSource == pagePaperSource[i].id ) {
      debug_msg("print Job paperSource = %d\n", i);
      break;
    }
  }
  debug_msg("Check manual feed\n");
  if(printJob.attr.manualFeed == 1)
  {
    debug_msg("MANUAL FEED\n"); 
    EJLStart_ADDSTR(p, "PU", "AU"); 
  } else
  {    
    debug_msg("pagePaperSource.name = %s\n", pagePaperSource[i].name);
    EJLStart_ADDSTR(p, "PU", pagePaperSource[i].name);
  }
  
  /* OU */
  if(printJob.attr.FacingPage == 0){
    EJLStart_ADDSTR(p, "OU", "FD");
  }else{
    EJLStart_ADDSTR(p, "OU", "FU");
  }

  /* DX */
  EJLStart_ADDSTR(p, "DX", ( printJob.attr.duplex != EPS_DUPLEX_NONE )?"ON":"OFF");

  if( printJob.attr.duplex != EPS_DUPLEX_NONE ) {
    /* BD */
    EJLStart_ADDSTR(p, "BD", ( printJob.attr.duplex == EPS_DUPLEX_LONG )?"LE":"SE");

    /* TB/LB */
    EJLStart_ADDSTR(p, "TB", "0");
    EJLStart_ADDSTR(p, "LB", "0");
  } 

  /* ZO */
  EJLStart_ADDSTR(p, "ZO", "OFF");

  /* EC */
  EJLStart_ADDSTR(p, "EC", "ON" );

  /* RI */
  if(printJob.attr.RiTech == TRUE){
    EJLStart_ADDSTR(p, "RI", "ON");
  } else{
    EJLStart_ADDSTR(p, "RI", "OFF");
  }

  /* SN */
  if(printJob.attr.tonerSave == 1){
    EJLStart_ADDSTR(p, "SN", "ON");
  } else{
    EJLStart_ADDSTR(p, "SN", "OFF");
  }

  /* Collate */
#if 0
  if( printJob.attr.collate ) {
    /* CO */
    EJLStart_ADDINT(p, "CO", 1);
    /* QT */
    EJLStart_ADDINT(p, "QT", printJob.attr.copies);
  } else {
#endif
    /* CO */
    /*EJLStart_ADDINT(p, "CO", printJob.attr.copies);
    
    EJLStart_ADDINT(p, "QT", 1);*/

    if(printJob.attr.colLate)
    {
      EJLStart_ADDINT(p, "CO", 1);
    /* QT */
      EJLStart_ADDINT(p, "QT", printJob.attr.copies);
    } else{
      EJLStart_ADDINT(p, "CO", printJob.attr.copies);
    /* QT */
      EJLStart_ADDINT(p, "QT", 1);
    }
/*  }*/

  /* Feed direction */
  if( EPS_FEEDDIR_LANDSCAPE == printJob.attr.feedDirection ){
    EJLStart_ADDSTR(p, "FE", "SE");
  } 

  if( EPS_CM_COLOR == printJob.attr.colorMode ){
    /** ESC/Page-Color **/
    /* SZ */
    EJLStart_ADDSTR(p, "SZ", "OFF");

    /* GAMMAMODE */
    EJLStart_ADDSTR(p, "GAMMAMODE", "NORMAL");

    /* SL */
    EJLStart_ADDSTR(p, "SL", "YES");

    /* TO */
    EJLStart_ADDSTR(p, "TO", "0");

    /* LO */
    EJLStart_ADDSTR(p, "LO", "0");

    /* OR  */
    EJLStart_ADDSTR(p, "OR", "PO");

    /* EJL ENTER command */
    sprintf(p, " %s%s%s", sbEJL_LF, sbEJL_EN_C, sbEJL_LF);
  } else{
    /* FO */
    EJLStart_ADDSTR(p, "FO", "OFF");

    /* EJL ENTER command */
    sprintf(p, " %s%s%s", sbEJL_LF, sbEJL_EN, sbEJL_LF);
  }

  p += strlen(p);
  pCmdBuff->len += (EPS_INT32)(p - pCmdBuff->p);

  EPS_RETURN( ret )
}


EPS_ERR_CODE ejlEnd (
               
    EPS_COMMAND_BUFFER *pCmdBuff, 
    PAGE_CmdBuffGrow  pfncGrow,
    EPS_INT32     pageCount
    
){
  EPS_ERR_CODE ret = EPS_ERR_NONE;
  EPS_UINT32  dwPaperNum = pageCount;
  EPS_INT8  *p;

  EPS_LOG_FUNCIN

#ifdef GCOMSW_DEBUG
  if(pCmdBuff->len+EPS_EJL_LINEMAX > pCmdBuff->size){EPS_DBGPRINT(("Command buffer not enough!\r\n"))}
#endif

  p = pCmdBuff->p + pCmdBuff->len;

  /* EJL commands -------- */
  sprintf(p, "%s%s ",
      sbEJL_ESC_SOH,  /* <ESC><SOH>  */
      sbEJL);     /* @EJL      */
  p += strlen(p);

  /* Page */
  if( printJob.attr.duplex != EPS_DUPLEX_NONE ){
    dwPaperNum = (pageCount+1)/2;
  } 
  sprintf(p, "JI PAGES=\"%d\"", dwPaperNum);
  p += strlen(p);

  /* @EJL */
  sprintf(p, " %s%s %s", sbEJL_LF, sbEJL, sbEJL_LF);
  p += strlen(p);

  pCmdBuff->len = (EPS_INT32)(p - pCmdBuff->p);

#ifdef GCOMSW_DEBUG
  if(pCmdBuff->len > pCmdBuff->size){EPS_DBGPRINT(("Command buffer over flow!!\r\n"))}
#endif

  EPS_RETURN( ret )
}


EPS_ERR_CODE ejlPageEsc (

    EPS_COMMAND_BUFFER *pCmdBuff, 
    PAGE_CmdBuffGrow  pfncGrow
                 
){
  EPS_ERR_CODE ret = EPS_ERR_NONE;
  EPS_INT8  *p;

  EPS_LOG_FUNCIN

#define PageEscCommand_ADDCMD(b, CMD) {   \
    memcpy(b, CMD, sizeof(CMD));    \
    b += sizeof(CMD);         \
  }

  p = pCmdBuff->p + pCmdBuff->len;

  /* Set Logical Origin Setting */
  sprintf(p, sbLOS, printJob.leftMargin, printJob.topMargin);
  p += strlen(p);

  /* Reset the current absolute position */
  PageEscCommand_ADDCMD(p, sbCHPAS);
  PageEscCommand_ADDCMD(p, sbCVPAS);

  /* Set Clip Area */
  sprintf(p, sbCA, printJob.printableAreaWidth, printJob.printableAreaHeight);
  p += strlen(p);

  pCmdBuff->len = (EPS_INT32)(p - pCmdBuff->p);

  EPS_RETURN( ret )
}

/*__________________________________  epson-escpage.c   ________________________________*/

/*34567890123456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*       1         2         3         4         5         6         7         8        */
/*******************************************|********************************************/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/***** End of File *** End of File *** End of File *** End of File *** End of File ******/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
