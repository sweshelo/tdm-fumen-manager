#include "gui.hpp"

gui::gui()
{
  gui::gBuffer = C2D_TextBufNew(4096);

  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();

  // Create screen
  gui::top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
};

void gui::set_songlist(std::vector<song>& songlist)
{
  gui::songlist = songlist;
}

void gui::draw()
{
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(top, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
  C2D_SceneBegin(top);

  switch(gui::status){
    case 0:
      select_download_song();
    case 1:
      select_overwrite_song();
  }

  C3D_FrameEnd(0);
};

void gui::select_download_song()
{
  u32 blue = C2D_Color32f(0.0f,0.0f,1.0f,1.0f);
  u32 black = C2D_Color32f(0.0f,0.0f,0.0f,1.0f);

  size_t array_size = gui::songlist.size();
  C2D_Text empty_string;

  for(int i = 0; i<array_size; i++){

    char buf[100];

    // string copy
    if(gui::gSongtitleText.size() <= i){
      gui::gSongtitleText.push_back(empty_string);
      snprintf(buf, sizeof(buf), "%s", gui::songlist[i].title.c_str());
      C2D_TextParse(&(gui::gSongtitleText[i]), gui::gBuffer, buf);
      C2D_TextOptimize(&(gui::gSongtitleText[i]));
    }

    // draw
    C2D_DrawText(&(gui::gSongtitleText[i]), C2D_WithColor, 8.0f, float(i * 16.0f), 0.5f, 0.5f, 0.5f, (i == gui::cursor) ? blue : black);
  }
};

void gui::select_overwrite_song()
{
  return;
}

void gui::key_handle(u32 key)
{
  if ( key & KEY_DDOWN )
    gui::cursor = ( gui::cursor+1 >= gui::gSongtitleText.size()) ? gui::gSongtitleText.size() : gui::cursor+1;
  if ( key & KEY_DUP )
    gui::cursor = ( gui::cursor-1 < 0 ) ? 0 : gui::cursor-1;
  return;
};
