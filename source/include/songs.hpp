#include <iostream>
#include <string>
#include <vector>
#include <jansson.h>

class song
{
  public:
    std::string title;
    std::string id;
};

class songlist
{
  public:
    json_t* json;
    json_error_t error_json;
    std::vector<song> songs;
    void file_open(void);
    void load_availsonglist(void);
};
