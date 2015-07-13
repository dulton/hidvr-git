#include <stdio.h>
#include <strings.h>
#include <string.h>

#include "httpproc.h"

int avlib_httpclientproc(char *rheader,char *requestcmd)
{
  char requesttype[5]="";
  char request[1024]="";

  sscanf(rheader,"%s %s",requesttype,request);
  if(strcasecmp(requesttype,"GET")!=0&&strcasecmp(requesttype,"POST")!=0)
  {
    return -1; //Bad request;
  }
  strcpy(requestcmd,request);
  return 0;
}

int avlib_writehttpheader(int code,char * contenttype,int contentsize,char *rheader)
{
    char codestr[100];
    if(code==200)
      strcpy(codestr,"200 OK");
    if(code==404)
      strcpy(codestr,"404 Not Found");
    if(code==400)
      strcpy(codestr,"400 Bad Request");
    if(code==401)
      strcpy(codestr,"401 Unauthorized\nConnection: close\nWWW-Authenticate: Basic realm=\"Media Server\"");
    sprintf(rheader,"HTTP/1.1 %s\nContent-type: %s\nContent-Length: %d\nMini Media Server 1.0\n\n",codestr,contenttype,contentsize);
    return 0;
}



