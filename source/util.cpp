#include "util.hpp"

Result http_download(const char *url, string location)
{
  Result ret=0;
  httpcContext context;
  char *newurl=NULL;
  u32 statuscode=0;
  u32 contentsize=0, readsize=0, size=0;
  u8 *buf, *lastbuf;
  fs out;

  do {
    ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);

    // This disables SSL cert verification, so https:// will be usable
    ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

    // Enable Keep-Alive connections
    ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);

    // Set a User-Agent header so websites can identify your application
    ret = httpcAddRequestHeaderField(&context, "User-Agent", "TDMManager/0.0.0");

    // Tell the server we can support Keep-Alive connections.
    // This will delay connection teardown momentarily (typically 5s)
    // in case there is another request made to the same server.
    ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

    ret = httpcBeginRequest(&context);
    if(ret!=0){
      httpcCloseContext(&context);
      if(newurl!=NULL) free(newurl);
      return ret;
    }

    ret = httpcGetResponseStatusCode(&context, &statuscode);
    if(ret!=0){
      httpcCloseContext(&context);
      if(newurl!=NULL) free(newurl);
      return ret;
    }

    if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
      if(newurl==NULL) newurl = (char*)malloc(0x1000); // One 4K page for new URL
      if (newurl==NULL){
        httpcCloseContext(&context);
        return -1;
      }
      ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
      url = newurl; // Change pointer to the url that we just learned
      httpcCloseContext(&context); // Close this context before we try the next
    }
  } while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

  if(statuscode!=200){
    httpcCloseContext(&context);
    if(newurl!=NULL) free(newurl);
    return -2;
  }

  // This relies on an optional Content-Length header and may be 0
  ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
  if(ret!=0){
    httpcCloseContext(&context);
    if(newurl!=NULL) free(newurl);
    return ret;
  }


  // Start with a single page buffer
  buf = (u8*)malloc(0x1000);
  if(buf==NULL){
    httpcCloseContext(&context);
    if(newurl!=NULL) free(newurl);
    return -1;
  }

  out.openfile(location);

  do {
    // This download loop resizes the buffer as data is read.
    ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
    size += readsize;
    if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
      lastbuf = buf; // Save the old pointer, in case realloc() fails.
      buf = (u8*)realloc(buf, size + 0x1000);
      if(buf==NULL){
        httpcCloseContext(&context);
        free(lastbuf);
        if(newurl!=NULL) free(newurl);
        return -1;
      }
    }
    out.writefile((const char*)buf, (size_t)readsize);
  } while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);	

  out.closefile();

  if(ret!=0){
    httpcCloseContext(&context);
    if(newurl!=NULL) free(newurl);
    free(buf);
    return -1;
  }

  // Resize the buffer back down to our actual final size
  lastbuf = buf;
  buf = (u8*)realloc(buf, size);
  if(buf==NULL){ // realloc() failed.
    httpcCloseContext(&context);
    free(lastbuf);
    if(newurl!=NULL) free(newurl);
    return -1;
  }

  httpcCloseContext(&context);
  free(buf);
  if (newurl!=NULL) free(newurl);

  return 0;
}

void load_songlist() {

  json_t *json;
  json_error_t error_json;

  json = json_load_file("romfs:/list.json", 0, &error_json);
  printf("address: %x\n", json);
  printf("Song ID: %s\n", json_string_value(json_object_get(json_array_get(json_object_get(json_array_get(json_object_get(json, "list"), 0), "songs"), 0), "id")));

  json_decref(json);
}

void quitwait(){
  while(true){
    gspWaitForVBlank();
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
      break;
  }
  httpcExit();
  gfxExit();
}
