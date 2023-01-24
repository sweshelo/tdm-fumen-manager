#include <iostream>
#include <string>
#include <vector>
#include <jansson.h>

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
    json_t* json;
    json_error_t error_json;
    std::vector<Song> songs;
    void file_open(void);
    void load_availsonglist(void);
};
