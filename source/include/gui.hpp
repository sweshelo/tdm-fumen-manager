#include <citro2d.h>
#include <vector>
#include "songs.hpp"

class gui
{
  public:
    void init(void);
    void draw(std::vector<song>&);
  private:
    C2D_TextBuf gBuffer;
    std::vector<C2D_Text> gSongtitleText;
    C3D_RenderTarget* top;
};
