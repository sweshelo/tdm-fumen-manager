#include <citro2d.h>
#include <vector>
#include <string>
#include "songs.hpp"

class Gui
{
  private:
    C2D_TextBuf gBuffer;
    std::vector<C2D_Text> gSongtitleText;
    std::vector<C2D_Text> guideText;
    std::vector<Song> _songlist;
    std::vector<Song>& songlist = _songlist;
    C3D_RenderTarget* top;
    int cursor = 0;
    int status = 0;
    int page = 0;
    int max_page = 0;
    std::string songid_download = "";
    std::string songid_overwrite = "";
    void select_download_song(void);
    void select_overwrite_song(void);
    std::string navigate[5] = {
      "ダウンロードする楽曲を選択します",
      "上書きする楽曲を選択します",
      "ダウンロードしています",
      "ダウンロードに成功しました",
      "ダウンロードに失敗しました",
    };

  public:
    u32 blue = C2D_Color32f(0.0f,0.0f,1.0f,1.0f);
    u32 black = C2D_Color32f(0.0f,0.0f,0.0f,1.0f);
    Gui(void);
    void draw(void);
    void set_songlist(std::vector<Song>&);
    void key_handle(u32);
};
