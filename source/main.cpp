#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <dirent.h>
#include <3ds.h>
#include "gui.hpp"
#include "util.hpp"

int main()
{
  Result ret=0;
  gfxInitDefault();
  httpcInit(0); // Buffer size when POST/PUT.
  romfsInit();

  //consoleInit(GFX_BOTTOM,NULL);

  //作業用ディレクトリチェック
  mkdir("sdmc:/3ds/tdm", 0777);
  mkdir("sdmc:/luma/", 0777);
  mkdir("sdmc:/luma/titles", 0777);
  mkdir("sdmc:/luma/titles/0004000000190E00", 0777);
  mkdir("sdmc:/luma/titles/0004000000190E00/romfs", 0777);
  mkdir("sdmc:/luma/titles/0004000000190E00/romfs/_data", 0777);
  mkdir("sdmc:/luma/titles/0004000000190E00/romfs/_data/fumen", 0777);
  mkdir("sdmc:/luma/titles/0004000000190E00/romfs/_data/sound", 0777);
  mkdir("sdmc:/luma/titles/0004000000190E00/romfs/_data/sound/song", 0777);

  ret=downloadToFile("https://sweshelo.jp/tdm/release.json", "sdmc:/3ds/tdm/release.json");
  printf("return from http_download: %" PRId32 "\n",ret);

  Remote_songlist songs;
  songs.file_open();
  songs.load_availsonglist();

  Gui gui;
  gui.set_songlist(songs.songs);

  // Main loop
  while (aptMainLoop())
  {
    gspWaitForVBlank();
    hidScanInput();

    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
      break; // break in order to return to hbmenu

    gui.draw();
    if (kDown) gui.key_handle(kDown);
  }

  // Exit services
  httpcExit();
  gfxExit();
  return 0;
}

