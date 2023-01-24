#include "util.hpp"

#define USER_AGENT "TDMManager/0.0.0"

static char *result_buf = nullptr;
static size_t result_sz = 0;
static size_t result_written = 0;

#define TIME_IN_US 1
#define TIMETYPE curl_off_t
#define TIMEOPT CURLINFO_TOTAL_TIME_T
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL	3000000

curl_off_t downloadTotal = 1; // Dont initialize with 0 to avoid division by zero later.
curl_off_t downloadNow = 0;
curl_off_t downloadSpeed = 0;

static FILE *downfile = nullptr;
static size_t file_buffer_pos = 0;
static size_t file_toCommit_size = 0;
static char *g_buffers[2] = { nullptr };
static u8 g_index = 0;
static Thread fsCommitThread;
static LightEvent readyToCommit;
static LightEvent waitCommit;
static bool killThread = false;
static bool writeError = false;
#define FILE_ALLOC_SIZE 0x60000
CURL *CurlHandle = nullptr;


static int curlProgress(CURL *hnd,
    curl_off_t dltotal, curl_off_t dlnow,
    curl_off_t ultotal, curl_off_t ulnow)
{
  downloadTotal = dltotal;
  downloadNow = dlnow;

  return 0;
}

bool filecommit() {
  if (!downfile) return false;
  fseek(downfile, 0, SEEK_END);
  u32 byteswritten = fwrite(g_buffers[!g_index], 1, file_toCommit_size, downfile);
  if (byteswritten != file_toCommit_size) return false;
  file_toCommit_size = 0;
  return true;
}

static void commitToFileThreadFunc(void *args) {
  LightEvent_Signal(&waitCommit);

  while (true) {
    LightEvent_Wait(&readyToCommit);
    LightEvent_Clear(&readyToCommit);
    if (killThread) threadExit(0);
    writeError = !filecommit();
    LightEvent_Signal(&waitCommit);
  }
}

static size_t file_handle_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
  //if (getAvailableSpace() < (u64)downloadTotal) return 0; // Out of space.
  if (writeError) return 0;
  //if (QueueSystem::CancelCallback) return 0;

  (void)userdata;
  const size_t bsz = size * nmemb;
  size_t tofill = 0;


  if (!g_buffers[g_index]) {
    LightEvent_Init(&waitCommit, RESET_STICKY);
    LightEvent_Init(&readyToCommit, RESET_STICKY);

    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    fsCommitThread = threadCreate(commitToFileThreadFunc, NULL, 0x1000, prio - 1, -2, true);

    g_buffers[0] = (char*)memalign(0x1000, FILE_ALLOC_SIZE);
    g_buffers[1] = (char*)memalign(0x1000, FILE_ALLOC_SIZE);

    if (!fsCommitThread || !g_buffers[0] || !g_buffers[1]) return 0;
  }

  if (file_buffer_pos + bsz >= FILE_ALLOC_SIZE) {
    tofill = FILE_ALLOC_SIZE - file_buffer_pos;
    memcpy(g_buffers[g_index] + file_buffer_pos, ptr, tofill);

    LightEvent_Wait(&waitCommit);
    LightEvent_Clear(&waitCommit);
    file_toCommit_size = file_buffer_pos + tofill;
    file_buffer_pos = 0;
    svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
    g_index = !g_index;
    LightEvent_Signal(&readyToCommit);
  }

  memcpy(g_buffers[g_index] + file_buffer_pos, ptr + tofill, bsz - tofill);
  file_buffer_pos += bsz - tofill;
  return bsz;
}


/*
   Download a file.
   const std::string &url: The download URL.
   const std::string &path: Where to place the file.
   */
Result downloadToFile(const std::string &url, const std::string &path) {
  bool needToDelete = false;
  downloadTotal = 1;
  downloadNow = 0;
  downloadSpeed = 0;

  CURLcode curlResult;
  Result retcode = 0;
  int res;

  printf("Downloading from:\n%s\nto:\n%s\n", url.c_str(), path.c_str());

  void *socubuf = memalign(0x1000, 0x100000);
  if (!socubuf) {
    retcode = -1;
    goto exit;
  }

  res = socInit((u32 *)socubuf, 0x100000);
  if (R_FAILED(res)) {
    retcode = res;
    goto exit;
  }

  /* make directories. */
  for (char *slashpos = strchr(path.c_str() + 1, '/'); slashpos != NULL; slashpos = strchr(slashpos + 1, '/')) {
    char bak = *(slashpos);
    *(slashpos) = '\0';

    mkdir(path.c_str(), 0777);

    *(slashpos) = bak;
  }

  downfile = fopen(path.c_str(), "wb");
  if (!downfile) {
    retcode = -2;
    goto exit;
  }

  CurlHandle = curl_easy_init();
  curl_easy_setopt(CurlHandle, CURLOPT_BUFFERSIZE, FILE_ALLOC_SIZE);
  curl_easy_setopt(CurlHandle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(CurlHandle, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(CurlHandle, CURLOPT_USERAGENT, USER_AGENT);
  curl_easy_setopt(CurlHandle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(CurlHandle, CURLOPT_FAILONERROR, 1L);
  curl_easy_setopt(CurlHandle, CURLOPT_ACCEPT_ENCODING, "gzip");
  curl_easy_setopt(CurlHandle, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(CurlHandle, CURLOPT_XFERINFOFUNCTION, curlProgress);
  curl_easy_setopt(CurlHandle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(CurlHandle, CURLOPT_WRITEFUNCTION, file_handle_data);
  curl_easy_setopt(CurlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(CurlHandle, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(CurlHandle, CURLOPT_STDERR, stdout);

  curlResult = curl_easy_perform(CurlHandle);
  curl_easy_cleanup(CurlHandle);
  CurlHandle = nullptr;

  if (curlResult != CURLE_OK) {
    retcode = -curlResult;
    needToDelete = true;
    goto exit;
  }

  LightEvent_Wait(&waitCommit);
  LightEvent_Clear(&waitCommit);

  file_toCommit_size = file_buffer_pos;
  svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
  g_index = !g_index;

  if (!filecommit()) {
    retcode = -3;
    needToDelete = true;
    goto exit;
  }

  fflush(downfile);

exit:
  if (fsCommitThread) {
    killThread = true;
    LightEvent_Signal(&readyToCommit);
    threadJoin(fsCommitThread, U64_MAX);
    killThread = false;
    fsCommitThread = nullptr;
  }

  socExit();

  if (socubuf) free(socubuf);

  if (downfile) {
    fclose(downfile);
    downfile = nullptr;
  }

  if (g_buffers[0]) {
    free(g_buffers[0]);
    g_buffers[0] = nullptr;
  }

  if (g_buffers[1]) {
    free(g_buffers[1]);
    g_buffers[1] = nullptr;
  }

  g_index = 0;
  file_buffer_pos = 0;
  file_toCommit_size = 0;
  writeError = false;

  /*
     if (needToDelete) {
     if (access(path.c_str(), F_OK) == 0) deleteFile(path.c_str()); // Delete file, cause not fully downloaded.
     }
     */

  //if (QueueSystem::CancelCallback) return 0;
  return retcode;
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
