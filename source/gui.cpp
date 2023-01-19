#include "gui.hpp"

Gui::Gui()
{
  Gui::gBuffer = C2D_TextBufNew(4096);

  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();

  // Create screen
  Gui::top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
};

void Gui::set_songlist(std::vector<Song>& songlist)
{
  Gui::songlist = songlist;
}

void Gui::draw()
{
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(top, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
  C2D_SceneBegin(top);

  switch(Gui::status){
    case 0:
      select_download_song();
    case 1:
      select_overwrite_song();
  }

  C3D_FrameEnd(0);
};

void Gui::select_download_song()
{
  u32 blue = C2D_Color32f(0.0f,0.0f,1.0f,1.0f);
  u32 black = C2D_Color32f(0.0f,0.0f,0.0f,1.0f);

  size_t array_size = Gui::songlist.size();
  C2D_Text empty_string;

  for(int i = 0; i<array_size; i++){

    char buf[100];

    // string copy
    if(Gui::gSongtitleText.size() <= i){
      Gui::gSongtitleText.push_back(empty_string);
      snprintf(buf, sizeof(buf), "%s", Gui::songlist[i].title.c_str());
      C2D_TextParse(&(Gui::gSongtitleText[i]), Gui::gBuffer, buf);
      C2D_TextOptimize(&(Gui::gSongtitleText[i]));
    }

    // draw
    C2D_DrawText(&(Gui::gSongtitleText[i]), C2D_WithColor, 8.0f, float(i * 16.0f), 0.5f, 0.5f, 0.5f, (i == Gui::cursor) ? blue : black);
  }
};

void Gui::select_overwrite_song()
{
  return;
}

void Gui::key_handle(u32 key)
{
  if ( key & KEY_DDOWN )
    Gui::cursor = ( Gui::cursor+1 >= Gui::gSongtitleText.size()) ? Gui::gSongtitleText.size() : Gui::cursor+1;
  if ( key & KEY_DUP )
    Gui::cursor = ( Gui::cursor-1 < 0 ) ? 0 : Gui::cursor-1;
  return;
};
