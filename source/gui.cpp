#include "gui.hpp"

Gui::Gui()
{
  Gui::gBuffer = C2D_TextBufNew(4096);

  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();

  // Create screen
  Gui::top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

  // GuideString
  for(int i=0; i<4; i++){
    C2D_Text guide_str_c2d;
    char buf[100];
    snprintf(buf, sizeof(buf), "%s", Gui::navigate[i].c_str());
    C2D_TextParse(&guide_str_c2d, Gui::gBuffer, buf);
    C2D_TextOptimize(&guide_str_c2d);
    Gui::guideText.push_back(guide_str_c2d);
  }
};

void Gui::set_songlist(std::vector<Song>& songlist)
{
  Gui::songlist = songlist;
  Gui::max_page = Gui::songlist.size() / 12;
  for (int i = 0; i < Gui::songlist.size(); i++){
    char buf[100];
    C2D_Text empty_string;

    Gui::gSongtitleText.push_back(empty_string);
    snprintf(buf, sizeof(buf), "%s", Gui::songlist[i].title.c_str());
    C2D_TextParse(&(Gui::gSongtitleText[i]), Gui::gBuffer, buf);
    C2D_TextOptimize(&(Gui::gSongtitleText[i]));
  }
}

void Gui::draw()
{
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(top, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
  C2D_SceneBegin(top);

  C2D_DrawText(&(Gui::guideText[Gui::status]), C2D_WithColor | C2D_AlignJustified, 8.0f, 0.0f, 0.5f, 0.5f, 0.5f, Gui::black);
  switch(Gui::status){
    case 0:
      select_download_song();
      break;
    case 1:
      select_overwrite_song();
      break;
    case 2:
      download();
      break;
  }

  C3D_FrameEnd(0);
};

void Gui::select_download_song()
{
  size_t array_size = Gui::songlist.size();
  for(int i = 0; i < ((Gui::max_page != Gui::page) ? 12 : array_size % 12); i++){
    C2D_DrawText(&(Gui::gSongtitleText[i + Gui::page * 12]), C2D_WithColor, 8.0f, float(i * 16.0f) + 32.0f, 0.5f, 0.5f, 0.5f, (i == Gui::cursor) ? Gui::blue : Gui::black);
  }
};

void Gui::select_overwrite_song()
{
  size_t array_size = Gui::songlist.size();
  for(int i = 0; i < ((Gui::max_page != Gui::page) ? 12 : array_size % 12); i++){
    C2D_DrawText(&(Gui::gSongtitleText[i + Gui::page * 12]), C2D_WithColor, 8.0f, float(i * 16.0f) + 32.0f, 0.5f, 0.5f, 0.5f, (i == Gui::cursor) ? Gui::blue : Gui::black);
  }
}

void Gui::key_handle(u32 key)
{
  if ( key & KEY_DDOWN ){
    switch(Gui::page == Gui::max_page){
      case true:
        Gui::cursor = (Gui::cursor+1 < Gui::songlist.size() % 12) ? Gui::cursor+1 : (Gui::songlist.size() % 12 - 1);
        break;
      case false:
        Gui::cursor = Gui::cursor+1 <= 11 ? Gui::cursor+1 : 11;
        break;
    }
  }
  if ( key & KEY_DUP )
    Gui::cursor = ( Gui::cursor-1 < 0 ) ? 0 : Gui::cursor-1;
  if ( key & KEY_DLEFT ){
    Gui::page = ( Gui::page-1 < 0 ) ? 0 : Gui::page-1;
    Gui::cursor = 0;
  }
  if ( key & KEY_DRIGHT ){
    Gui::page = ( Gui::page+1 > Gui::max_page ) ? Gui::max_page : Gui::page+1;
    Gui::cursor = 0;
  }

  if ( key & KEY_A ){
    switch( Gui::status ){
      case 0:
        {
          /*
           * 置換先楽曲データをロードし描画用のデータに変換
           */
          Gui::songid_download = Gui::songlist[ Gui::cursor + Gui::page * 12 ].id;
          Gui::gSongtitleText.clear();

          json_t *json, *genre, *songs, *song;
          json_error_t error_json;

          json = json_load_file("romfs:/list.json", 0, &error_json);

          //今は未使用だがジャンル分けしているので全ジャンルを統合する
          genre = json_object_get(json, "list");
          int genre_count = json_array_size(genre);
          for( int i=0; i < genre_count; i++){
            songs = json_object_get(json_array_get(genre, i), "songs");
            int songs_count = json_array_size(songs);
            for( int j=0; j < songs_count; j++ ){
              // title 取り出し
              song = json_array_get(songs, j);
              std::string song_title = json_string_value(json_object_get(song, "title"));

              Song _song;
              _song.title = song_title;
              _song.id = json_string_value(json_object_get(song, "id"));
              _song.song = json_string_value(json_object_get(song, "song"));

              Gui::stocksonglist.push_back(_song);

              // c2dtext push
              char buf[100];
              C2D_Text song_title_c2d;
              snprintf(buf, sizeof(buf), "%s", song_title.c_str());
              C2D_TextParse(&song_title_c2d, Gui::gBuffer, buf);
              C2D_TextOptimize(&song_title_c2d);
              Gui::gSongtitleText.push_back(song_title_c2d);
            }
          }

          json_decref(json);
          json_decref(genre);
          json_decref(songs);
          json_decref(song);

          Gui::status = 1;
          Gui::page = 0;
          Gui::max_page = Gui::gSongtitleText.size() / 12;
          Gui::cursor = 0;

          std::cout << Gui::songid_download << std::endl;
        }
        break;

      case 1:
        {
          Gui::status = 2;
        }
        break;
    }
  }
  return;
};

void Gui::download(){
  C2D_DrawText(&(Gui::guideText[Gui::status]), C2D_WithColor | C2D_AlignJustified, 8.0f, 0.0f, 0.5f, 0.5f, 0.5f, Gui::black);
  C3D_FrameEnd(0);

  std::string ROMFS_PATH="sdmc:/luma/titles/0004000000190E00/romfs/_data/";
  Song owsong = Gui::stocksonglist[ Gui::cursor + Gui::page * 12 ];
  std::cout << Gui::songid_download << " -> " << owsong.id << std::endl;

  // Downloads
  std::string path = ROMFS_PATH + "fumen/" + owsong.id + "/";
  mkdir(path.c_str(), 0777);
  mkdir((path + "solo/").c_str(), 0777);
  std::cout << ("https://sweshelo.jp/tdm/" + owsong.id + "/fumen.bin").c_str() << std::endl;
  std::cout << downloadToFile(("https://sweshelo.jp/tdm/" + Gui::songid_download + "/!!fumen.bin").c_str() , (path + "solo/" + owsong.id + "_m.bin").c_str()) << std::endl;
  std::cout << downloadToFile(("https://sweshelo.jp/tdm/" + Gui::songid_download + "/!!audio.naac").c_str() , (ROMFS_PATH + "sound/song/" + owsong.song + ".naac").c_str()) << std::endl;
  std::cout << downloadToFile(("https://sweshelo.jp/tdm/" + Gui::songid_download + "/!!audio_s.naac").c_str() , (ROMFS_PATH + "sound/song/" + owsong.song + "_s.naac").c_str()) << std::endl;

  Gui::page = 0;
  Gui::cursor = 0;
  Gui::status = 0;
  Gui::set_songlist(Gui::songlist);
}
