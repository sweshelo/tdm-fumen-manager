#include "songs.hpp"

// release.json をロードする
void Remote_songlist::file_open()
{
  Remote_songlist::json = json_load_file("sdmc:/3ds/tdm/release.json", 0, &(Remote_songlist::error_json));
};

void Remote_songlist::load_availsonglist()
{
  if( Remote_songlist::json == NULL )
    Remote_songlist::file_open();

  size_t array_size = json_array_size(Remote_songlist::json);
  for( int i = 0 ; i < array_size ; i ++ ){
    std::string id = json_string_value(json_object_get(json_array_get(Remote_songlist::json, i), "dir"));
    std::string title = json_string_value(json_object_get(json_array_get(Remote_songlist::json, i), "title"));

    Song song;
    song.title = title;
    song.id = id;

    Remote_songlist::songs.push_back(song);
  }

};
