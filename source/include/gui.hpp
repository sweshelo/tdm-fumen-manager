#include <citro2d.h>
#include <vector>
#include "songs.hpp"

class gui
{
  private:
    C2D_TextBuf gBuffer;
    std::vector<C2D_Text> gSongtitleText;
    std::vector<song> _songlist;
    std::vector<song>& songlist = _songlist;
    C3D_RenderTarget* top;
    int cursor = 0;
    int status = 0;
    void select_download_song(void);
    void select_overwrite_song(void);

  public:
    gui(void);
    void draw(void);
    void set_songlist(std::vector<song>&);
    void key_handle(u32);
};
