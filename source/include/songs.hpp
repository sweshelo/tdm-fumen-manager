#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"

class Song
{
  public:
    std::string title;
    std::string id;
    std::string song = "";
};

class Remote_songlist
{
  public:
    nlohmann::json json;
    std::vector<Song> songs;
    void file_open(void);
    void load_availsonglist(void);
};
