#include <citro2d.h>
#include <vector>
#include "songs.hpp"

class Gui
{
  private:
    C2D_TextBuf gBuffer;
    std::vector<C2D_Text> gSongtitleText;
    std::vector<Song> _songlist;
    std::vector<Song>& songlist = _songlist;
    C3D_RenderTarget* top;
    int cursor = 0;
    int status = 1;
    void select_download_song(void);
    void select_overwrite_song(void);

  public:
    Gui(void);
    void draw(void);
    void set_songlist(std::vector<Song>&);
    void key_handle(u32);
};
