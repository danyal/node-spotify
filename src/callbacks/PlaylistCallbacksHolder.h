#ifndef _SPOTIFY_SERVICE_PLAYLIST_CALLBACKS_HOLDER_H
#define _SPOTIFY_SERVICE_PLAYLIST_CALLBACKS_HOLDER_H

#include <node.h>

#include <libspotify/api.h>
#include <nan.h>
#include <initializer_list>
#include <memory>

using namespace v8;

class PlaylistCallbacksHolder {
private:
  Nan::ObjectWrap* userdata;
  sp_playlist* playlist;
  sp_playlist_callbacks* playlistCallbacks;
  void call(std::unique_ptr<Nan::Callback>& callback, std::initializer_list<Handle<Value>> info);
public:
  PlaylistCallbacksHolder(Nan::ObjectWrap* userdata, sp_playlist* playlist);
  ~PlaylistCallbacksHolder();

  //libspotify callback functions.
  static void playlistRenamed(sp_playlist* spPlaylist, void* userdata);
  static void tracksAdded(sp_playlist* playlist, sp_track *const *tracks, int num_tracks, int position, void *userdata);
  static void tracksMoved(sp_playlist* playlist, const int* tracks, int num_tracks, int new_position, void *userdata);
  static void tracksRemoved(sp_playlist* spPlaylist, const int *tracks, int num_tracks, void *userdata);
  static void trackCreatedChanged(sp_playlist* spPlaylist, int position, sp_user* spUser, int when, void* userdata);
  static void trackSeenChanged(sp_playlist* spPlaylist, int position, bool seen, void* userdata);
  static void trackMessageChanged(sp_playlist* spPlaylist, int position, const char* message, void* userdata);
  
  std::unique_ptr<Nan::Callback> playlistRenamedCallback;
  std::unique_ptr<Nan::Callback> tracksAddedCallback;
  std::unique_ptr<Nan::Callback> tracksMovedCallback;
  std::unique_ptr<Nan::Callback> tracksRemovedCallback;
  std::unique_ptr<Nan::Callback> trackCreatedChangedCallback;
  std::unique_ptr<Nan::Callback> trackSeenChangedCallback;
  std::unique_ptr<Nan::Callback> trackMessageChangedCallback;
  /**
    Register the callbacks with libspotify. Will first remove old registered callbacks.
  **/
  void setCallbacks();
  /**
    Unregister all callbacks.
  **/
  void unsetCallbacks();
};

#endif
