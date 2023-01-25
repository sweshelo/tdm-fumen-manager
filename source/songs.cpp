#include "songs.hpp"
#include "util.hpp"

// release.json をロードする
void Remote_songlist::file_open()
{
  Remote_songlist::json = nlohmann::json::parse(readFileIntoString("sdmc:/3ds/tdm/release.json"));
};

void Remote_songlist::load_availsonglist()
{
  if( Remote_songlist::json == NULL )
    Remote_songlist::file_open();

  size_t array_size = Remote_songlist::json.size();
  for( int i = 0 ; i < array_size ; i ++ ){
    std::string id = Remote_songlist::json[i]["dir"];
    std::string title = Remote_songlist::json[i]["title"];

    Song song;
    song.title = title;
    song.id = id;

    Remote_songlist::songs.push_back(song);
  }

};
