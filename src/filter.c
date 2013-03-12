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

#define HAVE_PPM (0)

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <cups/cups.h>
#include <cups/raster.h>

#include "err.h"
#include "mem.h"
#include "optBase.h"
#include "epson-escpr-def.h"
#include "epson-escpr-media.h"
#include "epson-escpr-pvt.h"
#include "epson-escpr-api.h"
#include "epson-escpr-services.h"
#include "epson-escpr-mem.h"
#include "epson-escpage.h" 
#include "linux_cmn.h"

#define PATH_MAX 256
#define NAME_MAX 41

#define EPS_PARAM_MP_TRAY       0x00
#define EPS_PARAM_CASSETTE1       0x01
#define EPS_PARAM_CASSETTE2       0x02
#define EPS_PARAM_CASSETTE3       0x03
#define EPS_PARAM_CASSETTE4       0x04
#define EPS_PARAM_MANUALFEED        0x0A
#define EPS_PARAM_AUTO_TRAY       0xFF

#define WIDTH_BYTES(bits) (((bits) + 31) / 32 * 4)

#define PIPSLITE_FILTER_VERSION "* epson-escpr is a part of " PACKAGE_STRING

#define PIPSLITE_FILTER_USAGE "Usage: $ epson-escpr model width_pixel height_pixel Ink PageSize Quality"


#ifndef EPS_END_PAGE
#define EPS_END_PAGE                    0       /* There is no next page                */
#endif

#ifndef EPS_NEXT_PAGE
#define EPS_NEXT_PAGE                   1       /* There is a next page                 */
#endif

#define PIPSLITE_WRAPPER_VERSION "* epson-escpr-wrapepr is a part of " PACKAGE_STRING
#define CUPS_FILTER_PATH "/opt/epson-abcdef/"
#define CUPS_FILTER_NAME "epson-escpage"

typedef struct rtp_filter_option {
  char model[NAME_MAX];
  char model_low[NAME_MAX];
  char ink[NAME_MAX];
  char media[NAME_MAX];
  char quality[NAME_MAX];
  char duplex[NAME_MAX];
} filter_option_t;

int cancel_flg = 0;

/* static functions */
static int getMediaTypeID(char *);
static int getMediaSizeID(char *);
const int band_line = 1;

extern EPS_INT32    libStatus;                  /*  Library (epsInitDriver) status      */
extern EPS_PRINT_JOB   printJob;
extern EPS_UINT32   sendDataBufSize;
extern EPS_UINT8*   sendDataBuf;    /* buffer of SendCommand(save) input                */
extern EPS_UINT32   tmpLineBufSize;
extern EPS_UINT8*   tmpLineBuf;

extern EPS_CMN_FUNC epsCmnFnc;

extern EPS_INT32 back_type;
extern EPS_INT32 lWidth;
extern EPS_INT32 lHeight;
extern EPS_INT32 areaWidth;
extern EPS_INT32 areaHeight;

#ifndef ESCPR_HEADER_LENGTH 
#define ESCPR_HEADER_LENGTH            10    /* ESC + CLASS + ParamLen + CmdName */ 
#endif

#ifndef ESCPR_SEND_DATA_LENGTH
#define ESCPR_SEND_DATA_LENGTH          7
#endif


EPS_ERR_CODE epsInitLib(){
  EPS_CMN_FUNC cmnFuncPtrs;
  memset(&cmnFuncPtrs, 0, sizeof(EPS_CMN_FUNC));
  
  cmnFuncPtrs.version = EPS_CMNFUNC_VER_CUR;
  cmnFuncPtrs.findCallback = NULL;
  cmnFuncPtrs.memAlloc = &epsmpMemAlloc;
  cmnFuncPtrs.memFree = &epsmpMemFree;
  cmnFuncPtrs.sleep = &epsmpSleep;
  cmnFuncPtrs.getTime = &epsmpGetTime;
  cmnFuncPtrs.getLocalTime = &epsmpGetLocalTime;
  cmnFuncPtrs.lockSync = &epsmpLockSync;
  cmnFuncPtrs.unlockSync = &epsmpUnlockSync;
  cmnFuncPtrs.stateCallback = NULL; /* current version unused */

  memcpy((void*)(&epsCmnFnc), (void*)&cmnFuncPtrs, sizeof(EPS_CMN_FUNC));
}

EPS_ERR_CODE epsInitJob(){
  
  memset(&printJob, 0, sizeof(EPS_PRINT_JOB));
  printJob.printer = (EPS_PRINTER_INN*) malloc(sizeof(EPS_PRINTER_INN));
  memset(printJob.printer, 0, sizeof(EPS_PRINTER_INN));
  
  printJob.jobStatus  = EPS_STATUS_NOT_INITIALIZED;
  printJob.pageStatus = EPS_STATUS_NOT_INITIALIZED;
  printJob.findStatus = EPS_STATUS_NOT_INITIALIZED;
  printJob.bComm    = TRUE; 
  printJob.platform   = 0x04; /* '0x04 = linux' is default */ 

  libStatus   = EPS_STATUS_NOT_INITIALIZED;
  sendDataBufSize = 0;
  sendDataBuf = NULL;
  tmpLineBufSize = 0;
  tmpLineBuf  = NULL;

/*** Initialize continue buffer                                                         */
  printJob.contData.sendData = NULL;
  printJob.contData.sendDataSize = 0;

  /* DEL printJob.additional = EPS_ADDDATA_NONE;
  printJob.qrcode.bits = NULL;
  printJob.qrcode.cellNum = 0;
  printJob.qrcode.dpc = 0; */
  obsClear();

/*** Set "Endian-ness" for the current cpu                                              */
  memInspectEndian();
  
/*** Set Communication Mode                                                             */
  //printJob.commMode = commMode;
  
/*** Change ESC/P-R Lib Status                                                          */
  libStatus = EPS_STATUS_INITIALIZED;
  EPS_PRINTER_INN*    printer = printJob.printer;
  printer->pmData.state = EPS_PM_STATE_NOT_FILTERED;
  printer->language = EPS_LANG_ESCPAGE;
  return EPS_ERR_NONE;
}


EPS_ERR_CODE epsInitVariable(){
  sendDataBufSize = (EPS_INT32)(ESCPR_HEADER_LENGTH    +
                  ESCPR_SEND_DATA_LENGTH +
                  (printJob.printableAreaWidth * printJob.bpp));
  sendDataBuf = (EPS_UINT8*)EPS_ALLOC(sendDataBufSize);
  if(sendDataBuf == NULL){
    sendDataBufSize = 0;
    EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION );
  }
  
  memset(sendDataBuf, 0xFF, (EPS_UINT32)sendDataBufSize);
  /*** Allocate buffer for RLE complession                                        */
  tmpLineBufSize = (EPS_INT32)(printJob.printableAreaWidth * printJob.bpp) + 256; /* 256 is temp buffer */
  tmpLineBuf = (EPS_UINT8*)EPS_ALLOC(tmpLineBufSize);
  if(tmpLineBuf == NULL){
    tmpLineBufSize = 0;
    EPS_RETURN( EPS_ERR_MEMORY_ALLOCATION );
  }
  memset(tmpLineBuf, 0xFF, (EPS_UINT32)tmpLineBufSize);
  return EPS_ERR_NONE;
}


EPS_ERR_CODE epsFilterEndPage(EPS_BOOL bNextPage){
  EPS_ERR_CODE    retStatus= EPS_ERR_NONE;
  
#ifdef GCOMSW_CMD_ESCPAGE_S
    retStatus = pageEndPage();
#else
    retStatus = EPS_ERR_LANGUAGE_NOT_SUPPORTED;
#endif

  return retStatus;

}

void eps_toupper(char *str){
  int i = 0;
  int len;
  if(str == NULL || strlen(str) == 0){
    return;
  }
  len = strlen(str);
  for(i = 0; i < len; i++){
    str[i] = toupper(str[i]);
  }
  return;
}

/* Get value for PPD */
static char *get_default_choice (ppd_file_t *ppd_p, const char *key)
{
  ppd_option_t *option;
  ppd_choice_t *choice;

  option = ppdFindOption (ppd_p, key);
  
  if( option == NULL && strcmp(key, "FaceUp") == 0 ) {
     debug_msg("__ Face Down __\n");
     return "False";
  }
  
  if (!option)
  {
    debug_msg("option null\n");
    return NULL;
  }
  
  choice = ppdFindChoice (option, option->defchoice);
  if (!choice)
  {
    debug_msg("choice null\n");
    return NULL;
  }
  debug_msg("choice ok = [%s]\n", choice->choice);
  return choice->choice;
}


int epsGetInputOption(char** in_argv,int in_argc, char *opt, char *dest)
{
  int i = 0;
  char *str = NULL;
  char *str_tmp = NULL;
  char dest_t[256];
  if(opt == NULL || strlen(opt) == 0 || dest == NULL ){
    return -1;
  }
  for(i = 0; i < in_argc; i++){
    if(strcmp(opt, "RiTech") == 0)
    {
      str_tmp = strstr(in_argv[i], "noRiTech");
      if(str_tmp != NULL)
      {
        strcpy(dest, "False");
        return 0;
      } 
      else{
        str_tmp = strstr(in_argv[i], "RiTech");
        if(str_tmp != NULL)
        {
          strcpy(dest, "True");
          return 0;
        }
      }
    }
    if(strcmp(opt, "TonerSave") == 0)
    {
      str_tmp = strstr(in_argv[i], "noTonerSaving");
      if(str_tmp != NULL)
      {
        strcpy(dest, "False");
        return 0;
      } else{
        str_tmp = strstr(in_argv[i], "TonerSaving");
        if(str_tmp != NULL)
        {
          strcpy(dest, "True");
          return 0;
        }
      }
    }
   if(strcmp(opt, "Duplex") == 0)
    {
      str_tmp = strstr(in_argv[i], "Duplex=DuplexNoTumble");
      if(str_tmp != NULL)
      {
        strcpy(dest, "DuplexNoTumble");
        return 0;
      } else if(str_tmp = strstr(in_argv[i], "Duplex=DuplexTumble")){
        if(str_tmp != NULL)
        {
          strcpy(dest, "DuplexTumble");
          return 0;
        }
      } else {
      strcpy(dest, "None");
         return 0;
    }
    }
  if(strcmp(opt, "Orientation") == 0)
    {
      str_tmp = strstr(in_argv[i], "Orientation=Portrait");
      if(str_tmp != NULL)
      {
        strcpy(dest, "Portrait");
        return 0;
      } else if(str_tmp = strstr(in_argv[i], "Orientation=Landscape")){
        if(str_tmp != NULL)
        {
          strcpy(dest, "Landscape");
          return 0;
        }
      }
    }
    str = strstr(in_argv[i], opt);
    if(str != NULL && (str[strlen(opt)] == '=') && strncmp(str, opt, strlen(opt)) == 0){
      strcpy(dest_t, str + strlen(opt) + 1);
      int j = 0;
      while((dest_t[j] != ' ') && (j < 20))
      {
        dest[j] = dest_t[j];
        j++;
      }
      dest[j] = '\0';
     
      return 0;
    }
  }

  debug_msg("return -1 \n");
  return -1;
}
/*tuanvd start add*/

static char *getOptionForJob(ppd_file_t *ppd_p, const char** argv, const int argc, char *key)
{
  char *opt_tmp = NULL;
  int error;
  //char opt_t[64];
  char *opt_t;
  char opt_tem[256];

  error = epsGetInputOption (argv, argc, key, opt_t);
  if(error == 0){
    opt_tmp = opt_t;
  }

  if(opt_tmp == NULL){
    opt_tmp = get_default_choice (ppd_p, key);
  }
  if(strcmp(key, "Duplex") == 0)
  {
    debug_msg("Duplex = %s \n", opt_tmp);
  }
  free(opt_t);
  return opt_tmp;
}

static int setupOptionforJob(const char** argv, const int argc, EPS_JOB_ATTRIB *jobAttr, const char *printer)
{
  char *ppd_path;   /* Path of PPD */
  ppd_file_t *ppd_p;  /* Struct of PPD */
  char *opt = NULL;    /* Temporary buffer (PPD option) */
  int i;      /* loop */
  char *err = NULL;

  jobAttr->colorPlane = EPS_CP_FULLCOLOR;
  jobAttr->printDirection = EPS_PD_BIDIREC;
  jobAttr->brightness = 0;
  jobAttr->mediaSizeIdx = -1;
  jobAttr->mediaTypeIdx = -1;
  jobAttr->paperSource  = 10000;
  jobAttr->duplex       = -1;
  jobAttr->tonerSave    = -1;
  jobAttr->RiTech       = -1;
  jobAttr->colLate      = -1;
  jobAttr->contrast     = 0;
  jobAttr->orienTation  = -1;
  jobAttr->manualFeed   = -1;
  jobAttr->printQuality = 10000;

  /* Get option from PPD. */
  ppd_path = (char *) cupsGetPPD (printer);
  ppd_p = ppdOpenFile (ppd_path);
  if(NULL == ppd_p){
    return 0;
  }
  /* get Color option */
  opt = getOptionForJob(ppd_p, argv, argc, "Color");
  if(opt != NULL){
    debug_msg("opt check = %s \n", opt);
    if(strcmp(opt, "MONO") == 0){
      jobAttr->colorMode =  EPS_CM_MONOCHROME;
    }else{
      jobAttr->colorMode =  EPS_CM_COLOR;
    }
  } else{
    jobAttr->colorMode =  EPS_CM_MONOCHROME;
  }
  debug_msg("debug 1\n");
  /* get PageSize */
  opt = getOptionForJob(ppd_p, argv, argc, "PageSize");
  debug_msg("opt = %s\n", opt);
  if(opt != NULL){
    debug_msg("opt check = %s \n", opt);
    jobAttr->mediaSizeIdx = getMediaSizeID(opt);
    debug_msg("jobAttr->mediaSizeIdx = %d\n", jobAttr->mediaSizeIdx);
  } else return 0;
  debug_msg("debug 2\n");
  /* get MediaType */
  opt = getOptionForJob(ppd_p, argv, argc, "MediaType");
  if(opt != NULL){
    jobAttr->mediaTypeIdx = getMediaTypeID(opt);
  } else return 0;
  debug_msg("debug 3\n");
  /* get InputSlot */

  opt = getOptionForJob(ppd_p, argv, argc, "InputSlot");
  if(opt != NULL){
    eps_toupper(opt);
    debug_msg("get option InputSlot \n");


    if(strcmp(opt, "Auto") == 0){
      jobAttr->paperSource = IPS_MPTID_AUTO;
    }else if(strcmp(opt, "TRAY1") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY1;
    }else if(strcmp(opt, "TRAY2") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY2;
    }else if(strcmp(opt, "TRAY3") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY3;
    }else if(strcmp(opt, "TRAY4") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY4;
    }else if(strcmp(opt, "TRAY5") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY5;
    }else if(strcmp(opt, "TRAY6") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY6;
    }else if(strcmp(opt, "TRAY7") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY7;
    }else if(strcmp(opt, "TRAY8") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY8;
    }else if(strcmp(opt, "TRAY9") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY9;
    }else if(strcmp(opt, "TRAY10") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY10;
    }else if(strcmp(opt, "TRAY11") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY11;
    }else if(strcmp(opt, "TRAY12") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY12;
    }else if(strcmp(opt, "TRAY13") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY13;
    }else if(strcmp(opt, "TRAY14") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY14;
    }else if(strcmp(opt, "TRAY15") == 0){
      jobAttr->paperSource = IPS_MPTID_TRAY15;
    } else jobAttr->paperSource = IPS_MPTID_AUTO;


  } else{
    jobAttr->paperSource = IPS_MPTID_AUTO;
  }


  /* get Duplex */
  opt = getOptionForJob(ppd_p, argv, argc, "Duplex");
  if(opt != NULL){
    if(strcmp(opt, "DuplexTumble") == 0){
      jobAttr->duplex = EPS_DUPLEX_SHORT;
    }else if(strcmp(opt, "DuplexNoTumble") == 0){
      jobAttr->duplex = EPS_DUPLEX_LONG;
    }else{
      jobAttr->duplex = EPS_DUPLEX_NONE;
    }
    debug_msg("jobAttr->duplex = %d\n", jobAttr->duplex);
  } else return 0;
  debug_msg("debug 4\n");
  /* get Tonersave */
  opt = getOptionForJob(ppd_p, argv, argc, "Tonersave");
  if(opt != NULL){
    eps_toupper(opt);
    if(strcmp(opt, "TRUE") == 0){
      jobAttr->tonerSave = 1; //ON
    }
    if(strcmp(opt, "FALSE") == 0){
      jobAttr->tonerSave = 0; //OFF
    }
  } else return 0;

  /* get option RiTech */
  opt = getOptionForJob(ppd_p, argv, argc, "RiTech");
  if(opt != NULL){
    eps_toupper(opt);
    if(strcmp(opt, "TRUE") == 0){
      jobAttr->RiTech = 1; //ON
    }
    if(strcmp(opt, "FALSE") == 0){
      jobAttr->RiTech = 0; //OFF
    }
  } else return 0;
  
  /*get option Quality*/
  opt = getOptionForJob(ppd_p, argv, argc, "Quality");
  debug_msg("Quality: = %s\n", opt);
  if(opt != NULL){
    if(strcmp(opt, "300x300dpi") == 0){
      debug_msg("__ Nam 0 __\n");
      jobAttr->printQuality = EPS_MQID_DRAFT;
      jobAttr->inputResolution = EPS_IR_300X300;
      jobAttr->topMargin = 300/6;
    }
    if(strcmp(opt, "600x600dpi") == 0){
      debug_msg("__ Nam 1 __\n");
      jobAttr->printQuality = EPS_MQID_NORMAL;
      jobAttr->inputResolution = EPS_IR_600X600;
      jobAttr->topMargin = 600/6;
    }
    if (strcmp(opt, "1200x1200dpi") == 0) {
      debug_msg("__ Nam 2 __\n");
      jobAttr->printQuality = EPS_MQID_HIGH;
      jobAttr->inputResolution = EPS_IR_1200X1200;
      jobAttr->topMargin = 1200/6;
    }
  } else return 0;

  /*get option Collate*/
  opt = getOptionForJob(ppd_p, argv, argc, "Collate");
  if(opt != NULL){
    eps_toupper(opt);
    if((strcmp(opt, "TRUE") == 0) || (strcmp(opt, "ON") == 0)){
      jobAttr->colLate = 1; //ON
    } else jobAttr->colLate = 0; //OFF
  } else return 0;

  /*get option Orientation*/
  debug_msg("debug orientation \n");
  opt = getOptionForJob(ppd_p, argv, argc, "Orientation");
  if(opt != NULL){
    eps_toupper(opt);
    if(strcmp(opt, "LANDSCAPE") == 0){
      jobAttr->orienTation = 1; //Landscape
    } else jobAttr->orienTation = 0; //Portrai
  } else return 0;
  
  /*get option ManualFeed*/
  debug_msg("debug manual feed \n");
  opt = getOptionForJob(ppd_p, argv, argc, "ManualFeed");
  if(opt != NULL){
    eps_toupper(opt);
    if( strcmp(opt, "MANUAL") == 0 ){
      debug_msg("check manual feed \n");
      jobAttr->manualFeed = 1; //Manual
    } else {
      jobAttr->manualFeed = 0; //Automatic
    }
  } else return 0;

  opt = getOptionForJob(ppd_p, argv, argc, "FaceUp");
  if(opt != NULL){
    //eps_toupper(opt);
    if(strcmp(opt, "True") == 0){
      jobAttr->FacingPage = 1;
    } else{
      jobAttr->FacingPage = 0;
    }
    debug_msg("jobAttr->FacingPage = %d\n", jobAttr->FacingPage);
  }


  /*get option copies*/
  opt = getOptionForJob(ppd_p, argv, argc, "nCopies");
  if(opt != NULL){
    jobAttr->copies = atoi(opt);
    debug_msg("jobAttr->copies = %d", jobAttr->copies);
    if(jobAttr->copies < 1 || jobAttr->copies > 999){
      jobAttr->copies = 1;
    }
  } else return 0;  

  jobAttr->printLayout = EPS_MLID_BORDERS;
  jobAttr->feedDirection = EPS_FEEDDIR_PORTRAIT;      /* paper feed direction  hardcode */
  ppdClose (ppd_p);
  return 1;
}
/*tuanvd end add*/


int main (int argc, char *argv[])
{
  int fd;     /* file descriptor */
  FILE *pfp;
  int i;      /* loop */
  int resul = 0;
  cups_raster_t *ras; /* raster stream for printing */
  cups_page_header_t header; /* page device dictionary header */
  EPS_JOB_ATTRIB fopt;
  
  debug_msg("%s:%d \t<<%s>>: \t\tStart()\n", __FILE__, __LINE__, __FUNCTION__);

/* attach point */
#ifdef USE_DEBUGGER
  int flag = 1;
  while (flag) sleep (3);
#endif /* USE_DEBUGGER */

  for (i = 0; i < argc; i++)
    debug_msg("%s:%d \t<<%s>>: \t\t argv[%d] = [%s]\n", __FILE__, __LINE__, __FUNCTION__, i, argv[i]);

  debug_msg("%s:%d \t<<%s>>: \t\tStart get cups option\n", __FILE__, __LINE__, __FUNCTION__);
  /*get_option_for_arg(argv, argc, &fopt);
  get_cups_option(argv[0], &fopt);  */
  resul = setupOptionforJob(argv, argc, &fopt, argv[0]);
  if(resul == 0){
    debug_msg("not setup option for job \n");
  }

  /* Print start */
  ras = cupsRasterOpen (fd, CUPS_RASTER_READ);
  if (ras == NULL)
  {
    fprintf (stderr, "Can't open CUPS raster file.");
    debug_msg("%s:%d \t<<%s>>: \t\tCan't open CUPS raster file\n", __FILE__, __LINE__, __FUNCTION__);
    return 1;
  }
  if(ras){
    do_printJob(pfp, ras, fopt);
  }
  debug_msg("Check start call function close rsater file \n");
  if(ras){
    cupsRasterClose (ras);
  }
  debug_msg("Check end call function close rsater file");
  return 0;
}

static int  getMediaTypeID(char *rsc_name)
{
  debug_msg("%s:%d \t<<%s>>: \t\tTrace in()\n", __FILE__, __LINE__, __FUNCTION__);
  int j;
  for(j = 0; mediaTypeData[j].value != END_ARRAY; j++){
    if(strncmp(mediaTypeData[j].rsc_name,rsc_name, strlen(mediaTypeData[j].rsc_name)) == 0){
      debug_msg("%s:%d \t<<%s>>: \t\tmediaTypeData[%d].rsc_name = %s, value = %d\n", __FILE__, __LINE__, __FUNCTION__, j, mediaTypeData[j].rsc_name, mediaTypeData[j].value);
      debug_msg("%s:%d \t<<%s>>: \t\tTrace out OK [%d]\n", __FILE__, __LINE__, __FUNCTION__, mediaTypeData[j].value);
      return mediaTypeData[j].value;
    }}
  debug_msg("%s:%d \t<<%s>>: \t\tTrace out Fail()\n", __FILE__, __LINE__, __FUNCTION__);
  return 0;
}

static int  getMediaSizeID(char *media_name)
{
  debug_msg("check media size \n");
  debug_msg("%s:%d \t<<%s>>: \t\tTrace in()\n", __FILE__, __LINE__, __FUNCTION__);
  int j;

  for(j = 0; mediaSizeData[j].value != END_ARRAY; j++){
    if(strncmp(mediaSizeData[j].rsc_name,media_name, strlen(mediaSizeData[j].rsc_name)) == 0){
    eps_toupper(media_name);
    debug_msg("media_name = %s \n", media_name);
    if(strcmp(media_name, "USER")==0){
      debug_msg("%s:%d \t<<%s>>: \t\tTrace out USER mode\n", __FILE__, __LINE__, __FUNCTION__);
      return 0;
    }
      debug_msg("%s:%d \t<<%s>>: \t\tTrace out OK [%d]()\n", __FILE__, __LINE__, __FUNCTION__, mediaSizeData[j].value);
      return mediaSizeData[j].value;
    }
  }
  return 0;
}

int do_printJob(FILE* pfp,cups_raster_t *ras, EPS_JOB_ATTRIB jobAttr)
{
  debug_msg("%s:%d \t<<%s>>: \tTrace in \n", __FILE__, __LINE__, __FUNCTION__);
  cups_page_header_t header;
  int width_pixel, height_pixel;
  char cBuff[256];
  int bytes_per_line;
  int byte_par_pixel;
  
  double print_area_x, print_area_y;
  char *image_raw;
  int leftMargin, topMargin;

  int read_size = 0;

  int err = 0;
  int i, j;
  EPS_INT32 heightPixels = 0; 

   int page_count = 0; 
#if (HAVE_PPM)
  char ppmfilename[30];
  FILE *fp;
  
#endif

  if (jobAttr.colorMode == EPS_CM_COLOR) {
      byte_par_pixel = 3;
  }else{ 
      byte_par_pixel = 1;
  }

  while (cupsRasterReadHeader (ras, &header) > 0 && !cancel_flg)
  {
    debug_msg("%s:%d \t<<%s>>: \tStart Cupsraster read header \n", __FILE__, __LINE__, __FUNCTION__);
    int image_bytes;
    char *image_raw;
    int write_size = 0;
    char tmpbuf[256];
    heightPixels = 0;
    if (page_count == 0)
    {
      width_pixel = header.cupsWidth;
      height_pixel = header.cupsHeight;
      epsInitLib();
      epsInitJob();
      err = SetupJobAttrib(&jobAttr);
      if (err)
      {
        err_fatal ("Error occurred in \"SetupJobAttrib\".");  /* exit */
      }
      printJob.paperWidth = width_pixel;
      printJob.paperHeight = height_pixel;

      epsInitVariable();
      switch(printJob.attr.inputResolution)
      {
      case EPS_IR_300X300:
        printJob.topMargin      = 50;
          printJob.leftMargin     = 50;
        break;
        case EPS_IR_600X600:
          printJob.topMargin      = 100;
          printJob.leftMargin     = 100;
        break;
        default:
          printJob.topMargin      = 200;
          printJob.leftMargin     = 200;
        break;
      }

      leftMargin = printJob.leftMargin;
      topMargin = printJob.topMargin;
  
      printJob.printableAreaHeight  = (EPS_UINT32)height_pixel;// - (printJob.topMargin + printJob.leftMargin);
        printJob.printableAreaWidth = (EPS_UINT32)width_pixel;// - (printJob.topMargin + printJob.leftMargin);
      err = pageAllocBuffer();
      if(err)
      {
        debug_msg("%s:%d \t<<%s>>: \t\t Error occurred in \"pageAllocBuffer\".", __FILE__, __LINE__, __FUNCTION__); /* exit */
        err_fatal ("Error occurred in \"pageAllocBuffer\"."); /* exit */
      }else
      {
        debug_msg("%s:%d \t<<%s>>: \t\t pageAllocBuffer() success\n", __FILE__, __LINE__, __FUNCTION__);
      }

      EPS_PRINTER_INN curPrinter;
      memset(&curPrinter, 0, sizeof(curPrinter));
      if(jobAttr.colorMode == EPS_CM_COLOR){
        curPrinter.language = EPS_LANG_ESCPAGE_COLOR;
      }else{
        curPrinter.language = EPS_LANG_ESCPAGE;
      }
  
      printJob.printer = &curPrinter;
      err = pageStartJob();
      if (err)
      {
        err_fatal ("Error occurred in \"pageStartJob\".");  /* exit */
      }else
      {
        debug_msg("%s:%d \t<<%s>>: \t\t pageStartJob() success",  __FILE__, __LINE__, __FUNCTION__);
      }
    }
    err = pageStartPage();
    if (err)
    {
      debug_msg("%s:%d \t<<%s>>: \t\t Error occurred in \"pageStartPage\": %d\n", __FILE__, __LINE__, __FUNCTION__, err);
      err_fatal ("Error occurred in \"pageStartPage\".");  /* exit */
    }else
    {
      debug_msg("%s:%d \t<<%s>>: \t\t pageStartPage() success",  __FILE__, __LINE__, __FUNCTION__);
    }
    printJob.verticalOffset = 0;
    print_area_x = printJob.printableAreaWidth;
    print_area_y = printJob.printableAreaHeight;
  
    bytes_per_line = (width_pixel * byte_par_pixel);
  
    int band_out_len = printJob.printableAreaWidth*byte_par_pixel;
    //band_out_len += band_out_len % 2;
    band_out_len = ((band_out_len +7)/8)*8;
    char *bandPrint = mem_new0(char, band_out_len*band_line);
  
    image_raw = mem_new0 (char, bytes_per_line);
  
    char *grayBuff;
    if(byte_par_pixel == 1)
    {
      grayBuff = mem_new0 (char, band_out_len*band_line*3);
    }
  
    char *pPrint;

    long int total_bytes = 0;
    read_size = 0;
    image_bytes = WIDTH_BYTES(header.cupsBytesPerLine)*8;
    #if (HAVE_PPM)
      
    sprintf(ppmfilename, "/tmp/al-m2010-page%d.ppm", page_count);
    fp = fopen(ppmfilename, "w");
    if(jobAttr.colorMode == EPS_CM_COLOR){
      fprintf(fp, "P3\n");
    }else{
      fprintf(fp, "P2\n");
    }
    fprintf(fp, "%u\n",(int)printJob.printableAreaWidth);
    fprintf(fp, "%u\n",(int)header.cupsHeight);
    fprintf(fp, "255\n");
    fclose(fp);
    
    #endif  
    page_count++;
    debug_msg("%s:%d \t<<%s>>: \t\timage_bytes = %d\n", __FILE__, __LINE__, __FUNCTION__, image_bytes);
    image_raw = (char *)calloc (sizeof (char), image_bytes);
    int i = 0;
    int y_count = 0;
    debug_msg("duplex = %d, page_count = %d\n", jobAttr.duplex, page_count);
    
    for (i = 0; i < header.cupsHeight && !cancel_flg; i ++)
      {
        debug_msg("%s:%d \t<<%s>>: \tStart print line [%d]\n", __FILE__, __LINE__, __FUNCTION__, i);
        int numread = cupsRasterReadPixels (ras, (unsigned char*)image_raw, header.cupsBytesPerLine);
        if (!numread)
        {
          fprintf (stderr, "cupsRasterReadPixels");
          debug_msg("%s:%d \t<<%s>>: \t\tcupsRasterReadPixels error\n", __FILE__, __LINE__, __FUNCTION__);
          return 1;
        }
        debug_msg("%s:%d \t<<%s>>: \t numread = %d\n", __FILE__, __LINE__, __FUNCTION__, numread);
        memcpy (bandPrint + (heightPixels%band_line) * band_out_len, image_raw, band_out_len );
        heightPixels ++;
        //y_count++;
        debug_msg("header.cupsBytesPerLine = %d\n", header.cupsBytesPerLine);
        if (i < printJob.paperHeight)
        {
          if(byte_par_pixel == 1)
          {
            pPrint = grayBuff;
            int j = 0;
            for(j = 0; j < band_out_len*band_line; j++)
            {
              grayBuff[j*3] = bandPrint[j];
              grayBuff[j*3+1] = bandPrint[j];
              grayBuff[j*3+2] = bandPrint[j];
            }
            debug_msg("byte_par_pixel = %d\n", byte_par_pixel);
            err = PrintBand (grayBuff, band_out_len*band_line*3, &heightPixels);
            #if (HAVE_PPM)
            int k = 0;
            fp = fopen(ppmfilename, "a+");
            for(k=0; k<(int)header.cupsBytesPerLine; k++)
            {   
              fprintf(fp, "%u ", (unsigned char)bandPrint[k]);
            }
            fprintf(fp, "\n");
            fclose(fp);
            #endif
          }else
          {
            debug_msg("byte_par_pixel = %d\n", byte_par_pixel);
            err = PrintBand (bandPrint, band_out_len, &heightPixels);
            #if (HAVE_PPM)
            int k = 0;
            fp = fopen(ppmfilename, "a+");
            for(k=0; k<(int)header.cupsBytesPerLine; k++)
            {   
              fprintf(fp, "%u ", (unsigned char)bandPrint[k]);
            }
            fprintf(fp, "\n");
            fclose(fp);
            #endif
          }
          heightPixels = 0;
          //err = PrintBand (image_raw, band_out_len, &heightPixels);
          if (err)
          {
            if(err == EPS_OUT_OF_BOUNDS)
            {
              debug_msg("EPS_OUT_OF_BOUNDS\n");
              //break;
            }
            debug_msg("PrintBand error: %d\n", err);
            err_fatal ("Error occurred in \"OUT_FUNC\"."); /* exit */
          }else
          {
            debug_msg("PrintBand success\n");
          }
          debug_msg("%s:%d \t<<%s>>: \tCompleted print line [%d]\n", __FILE__, __LINE__, __FUNCTION__, i);
        }
      }
    debug_msg("pageEndPage(%d) call\n", page_count);
    err = pageEndPage();
    if (err)
    {
      debug_msg("pageEndPage() error: %d\n", err);
      err_fatal ("Error occurred in \"PEND_FUNC\"."); /* exit */
    }else
    {
      debug_msg("pageEndPage() success\n");
    }
    free (image_raw);
    free(bandPrint);
    bandPrint = NULL;
  }
  
  err = pageEndJob();
  if(err){
    err_fatal ("Error occurred in \"pageEndJob\"."); /* exit */
  }else{
    debug_msg("pageEndJob() success\n");
  }
  return 0;
}
