#ifndef _PLAYLIST_CONTAINER_CALLBACKS_H
#define _PLAYLIST_CONTAINER_CALLBACKS_H

#include <node.h>

#include <initializer_list>
#include <libspotify/api.h>
#include <nan.h>
#include <memory>

class PlaylistContainerCallbacksHolder {
private:
  sp_playlistcontainer* playlistContainer;
  sp_playlistcontainer_callbacks* playlistContainerCallbacks;
  Nan::ObjectWrap* userdata;
  void call(std::unique_ptr<Nan::Callback>& callback, std::initializer_list<v8::Handle<v8::Value>> info);
public:
  PlaylistContainerCallbacksHolder(sp_playlistcontainer* pc, Nan::ObjectWrap* userdata);
  ~PlaylistContainerCallbacksHolder();

  //libspotify callback functions
  static void playlistAdded(sp_playlistcontainer* pc, sp_playlist* spPlaylist, int position, void* userdata);
  static void playlistRemoved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata);
  static void playlistMoved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, int new_position, void *userdata);

  std::unique_ptr<Nan::Callback> playlistAddedCallback;
  std::unique_ptr<Nan::Callback> playlistRemovedCallback;
  std::unique_ptr<Nan::Callback> playlistMovedCallback;

  void setCallbacks();
  void unsetCallbacks();
};

#endif