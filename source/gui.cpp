#include "gui.hpp"

void gui::init()
{
  gui::gBuffer = C2D_TextBufNew(4096);

  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();

  // Create screen
  gui::top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
};

void gui::draw(std::vector<song>& songs)
{
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(top, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
  C2D_SceneBegin(top);

  size_t array_size = songs.size();
  C2D_Text empty_string;

  std::cout << songs.size() << std::endl;
  std::cout << gui::gSongtitleText.size() << std::endl;

  for(int i; i<array_size; i++){

    char buf[100];

    // string copy
    if(gui::gSongtitleText.size() <= i){
      gui::gSongtitleText.push_back(empty_string);
    }

    // draw
    snprintf(buf, sizeof(buf), "%s", songs[i].title.c_str());
    C2D_TextParse(&(gui::gSongtitleText[i]), gui::gBuffer, buf);
    C2D_TextOptimize(&(gui::gSongtitleText[i]));
    C2D_DrawText(&(gui::gSongtitleText[i]), 0, 8.0f, float(i * 16.0f), 0.5f, 0.5f, 0.5f);
  }

  C3D_FrameEnd(0);
};
