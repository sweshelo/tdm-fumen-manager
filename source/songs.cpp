#include "songs.hpp"

// release.json をロードする
void songlist::file_open()
{
  songlist::json = json_load_file("sdmc:/3ds/tdm/release.json", 0, &(songlist::error_json));
};

void songlist::load_availsonglist()
{
  if( songlist::json == NULL )
    songlist::file_open();


  size_t array_size = json_array_size(songlist::json);
  for( int i = 0 ; i < array_size ; i ++ ){
    std::string name = json_string_value(json_object_get(json_array_get(songlist::json, i), "dir"));
    std::cout << name << std::endl;
  }

};
