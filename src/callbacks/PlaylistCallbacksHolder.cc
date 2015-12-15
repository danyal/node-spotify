#include "PlaylistCallbacksHolder.h"
#include "../objects/node/NodeTrackExtended.h"
#include "../objects/node/NodeUser.h"
#include "../objects/spotify/TrackExtended.h"
#include "../objects/spotify/Playlist.h"
#include "../objects/spotify/User.h"

#include <memory>

PlaylistCallbacksHolder::PlaylistCallbacksHolder(Nan::ObjectWrap* _userdata, sp_playlist* _playlist) : userdata(_userdata), playlist(_playlist) {
  playlistCallbacks = new sp_playlist_callbacks();
}

PlaylistCallbacksHolder::~PlaylistCallbacksHolder() {
  sp_playlist_remove_callbacks(playlist, playlistCallbacks, this);
  delete playlistCallbacks;
}

void PlaylistCallbacksHolder::call(std::unique_ptr<Nan::Callback>& callback, std::initializer_list<Handle<Value>> info) {
  unsigned int argc = info.size();
  Handle<Value>* argv = const_cast<Handle<Value>*>(info.begin());
  callback->Call(argc, argv);
}

void PlaylistCallbacksHolder::playlistRenamed(sp_playlist* spPlaylist, void* userdata) {
  auto holder = static_cast<PlaylistCallbacksHolder*>(userdata);
  holder->call(holder->playlistRenamedCallback, { Nan::Undefined(), holder->userdata->handle() });
}

void PlaylistCallbacksHolder::tracksAdded(sp_playlist* spPlaylist, sp_track *const *tracks, int num_tracks, int position, void *userdata) {
  auto holder = static_cast<PlaylistCallbacksHolder*>(userdata);
  Handle<Array> nodeTracks = Nan::New<Array>(num_tracks);
  for(int i = 0; i < num_tracks; i++) {
    NodeTrack* nodeTrackExtended = new NodeTrackExtended(std::make_shared<TrackExtended>(tracks[i], spPlaylist, position + i));
    nodeTracks->Set(Nan::New<Number>(i), nodeTrackExtended->createInstance());
  }
  holder->call(holder->tracksAddedCallback, { Nan::Undefined(), holder->userdata->handle(), nodeTracks, Nan::New<Number>(position) });
}

void PlaylistCallbacksHolder::tracksMoved(sp_playlist* spPlaylist, const int* tracks, int num_tracks, int new_position, void *userdata) {
  auto holder = static_cast<PlaylistCallbacksHolder*>(userdata);
  Handle<Array> movedTrackIndices = Nan::New<Array>(num_tracks);
  for(int i = 0; i < num_tracks; i++) {
    movedTrackIndices->Set(Nan::New<Number>(i), Nan::New<Number>(tracks[i]));
  }
  holder->call(holder->tracksMovedCallback, { Nan::Undefined(), holder->userdata->handle(), movedTrackIndices, Nan::New<Number>(new_position) });
}

void PlaylistCallbacksHolder::tracksRemoved(sp_playlist* spPlaylist, const int *tracks, int num_tracks, void *userdata) {
  auto holder = static_cast<PlaylistCallbacksHolder*>(userdata);
  Handle<Array> removedTrackIndexes = Nan::New<Array>(num_tracks);
  for(int i = 0; i < num_tracks; i++) {
    removedTrackIndexes->Set(Nan::New<Number>(i), Nan::New<Number>(tracks[i]));
  }
  holder->call(holder->tracksRemovedCallback, { Nan::Undefined(), holder->userdata->handle(), removedTrackIndexes });
}

void PlaylistCallbacksHolder::trackCreatedChanged(sp_playlist* spPlaylist, int position, sp_user* spUser, int when, void* userdata) {
  auto holder = static_cast<PlaylistCallbacksHolder*>(userdata);
  double date = (double)when * 1000;
  NodeUser* nodeUser = new NodeUser(std::unique_ptr<User>(new User(spUser)));
  holder->call(holder->trackCreatedChangedCallback, { Nan::Undefined(), holder->userdata->handle(), Nan::New<Integer>(position), nodeUser->handle(), Nan::New<Date>(date).ToLocalChecked() });
}

void PlaylistCallbacksHolder::trackSeenChanged(sp_playlist* spPlaylist, int position, bool seen, void* userdata) {
  auto holder = static_cast<PlaylistCallbacksHolder*>(userdata);
  holder->call(holder->trackSeenChangedCallback, { Nan::Undefined(), holder->userdata->handle(), Nan::New<Integer>(position), Nan::New<Boolean>(seen) });
}

void PlaylistCallbacksHolder::trackMessageChanged(sp_playlist* spPlaylist, int position, const char* message, void* userdata) {
  auto holder = static_cast<PlaylistCallbacksHolder*>(userdata);
  holder->call(holder->trackMessageChangedCallback, { Nan::Undefined(), holder->userdata->handle(), Nan::New<Integer>(position), Nan::New<String>(message).ToLocalChecked() });
}

void PlaylistCallbacksHolder::setCallbacks() {
  sp_playlist_remove_callbacks(playlist, playlistCallbacks, this);

  if(playlistRenamedCallback && !playlistRenamedCallback->IsEmpty()) {
    playlistCallbacks->playlist_renamed = &PlaylistCallbacksHolder::playlistRenamed;
  }
  if(tracksAddedCallback && !tracksAddedCallback->IsEmpty()) {
    playlistCallbacks->tracks_added = &PlaylistCallbacksHolder::tracksAdded;
  }
  if(tracksMovedCallback && !tracksMovedCallback->IsEmpty()) {
    playlistCallbacks->tracks_moved = &PlaylistCallbacksHolder::tracksMoved;
  }
  if(tracksRemovedCallback && !tracksRemovedCallback->IsEmpty()) {
    playlistCallbacks->tracks_removed = &PlaylistCallbacksHolder::tracksRemoved;
  }
  if(trackCreatedChangedCallback && !trackCreatedChangedCallback->IsEmpty()) {
    playlistCallbacks->track_created_changed = &PlaylistCallbacksHolder::trackCreatedChanged;
  }
  if(trackSeenChangedCallback && !trackSeenChangedCallback->IsEmpty()) {
    playlistCallbacks->track_seen_changed = &PlaylistCallbacksHolder::trackSeenChanged;
  }
  if(trackMessageChangedCallback && !trackMessageChangedCallback->IsEmpty()) {
    playlistCallbacks->track_message_changed = &PlaylistCallbacksHolder::trackMessageChanged;
  }
  sp_playlist_add_callbacks(playlist, playlistCallbacks, this);
}

void PlaylistCallbacksHolder::unsetCallbacks() {
  sp_playlist_remove_callbacks(playlist, playlistCallbacks, this);
}
