#include "NodeSearch.h"
#include "NodeTrack.h"
#include "NodeAlbum.h"
#include "NodeArtist.h"
#include "NodePlaylist.h"
#include "../../Application.h"

extern Application* application;

NodeSearch::NodeSearch(const char* _searchQuery) : searchQuery(_searchQuery), trackOffset(0), albumOffset(0), artistOffset(0), playlistOffset(0),
  trackLimit(10), albumLimit(10), artistLimit(10), playlistLimit(10) {

}

NodeSearch::NodeSearch(const char* _searchQuery, int offset) : searchQuery(_searchQuery), trackOffset(offset), albumOffset(offset), artistOffset(offset), playlistOffset(offset),
  trackLimit(10), albumLimit(10), artistLimit(10), playlistLimit(10) {

}

NodeSearch::NodeSearch(const char* _searchQuery, int offset, int limit) : searchQuery(_searchQuery), trackOffset(offset), albumOffset(offset), artistOffset(offset), playlistOffset(offset),
  trackLimit(limit), albumLimit(limit), artistLimit(limit), playlistLimit(limit) {

}

NAN_METHOD(NodeSearch::execute) {
  Nan::HandleScope scope;
  if(info.Length() < 1) {//TODO: how to check if it is a function? ->IsFunction() does not work, it does not recoginze functions.
    return Nan::ThrowError("execute needs a callback function as its argument.");
  }
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->makePersistent();
  nodeSearch->browseCompleteCallback = std::unique_ptr<Nan::Callback>(new Nan::Callback(info[0].As<Function>()));
  nodeSearch->search = std::unique_ptr<Search>(new Search());
  nodeSearch->search->nodeObject = nodeSearch;
  nodeSearch->search->execute(nodeSearch->searchQuery, nodeSearch->trackOffset, nodeSearch->trackLimit,
    nodeSearch->albumOffset, nodeSearch->albumLimit,
    nodeSearch->artistOffset, nodeSearch->artistLimit,
    nodeSearch->playlistLimit, nodeSearch->playlistLimit);
  nodeSearch->setupAdditionalMethods();
  return;
}

/**
 * Adds adiitional properties to the V8 object.
 * These will call libspotify functions and should first be available when the search has been executed.
 **/
void NodeSearch::setupAdditionalMethods() {
  Handle<Object> nodeObject = this->handle();
  Nan::SetAccessor(nodeObject, Nan::New<String>("didYouMean").ToLocalChecked(), didYouMean);
  Nan::SetAccessor(nodeObject, Nan::New<String>("link").ToLocalChecked(), getLink);
  nodeObject->Set(Nan::New<String>("getTrack").ToLocalChecked(), Nan::New<FunctionTemplate>(getTrack)->GetFunction());
  nodeObject->Set(Nan::New<String>("getAlbum").ToLocalChecked(), Nan::New<FunctionTemplate>(getAlbum)->GetFunction());
  nodeObject->Set(Nan::New<String>("getArtist").ToLocalChecked(), Nan::New<FunctionTemplate>(getArtist)->GetFunction());
  nodeObject->Set(Nan::New<String>("getPlaylist").ToLocalChecked(), Nan::New<FunctionTemplate>(getPlaylist)->GetFunction());
  Nan::SetAccessor(nodeObject, Nan::New<String>("totalTracks").ToLocalChecked(), getTotalTracks);
  Nan::SetAccessor(nodeObject, Nan::New<String>("numTracks").ToLocalChecked(), getNumTracks);
  Nan::SetAccessor(nodeObject, Nan::New<String>("totalAlbums").ToLocalChecked(), getTotalAlbums);
  Nan::SetAccessor(nodeObject, Nan::New<String>("numAlbums").ToLocalChecked(), getNumAlbums);
  Nan::SetAccessor(nodeObject, Nan::New<String>("totalArtists").ToLocalChecked(), getTotalArtists);
  Nan::SetAccessor(nodeObject, Nan::New<String>("numArtists").ToLocalChecked(), getNumArtists);
  Nan::SetAccessor(nodeObject, Nan::New<String>("totalPlaylists").ToLocalChecked(), getTotalPlaylists);
  Nan::SetAccessor(nodeObject, Nan::New<String>("numPlaylists").ToLocalChecked(), getNumPlaylists);
}

NAN_GETTER(NodeSearch::getTrackOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->trackOffset));
}

NAN_SETTER(NodeSearch::setTrackOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->trackOffset = value->ToInteger()->Value();
}

NAN_GETTER(NodeSearch::getAlbumOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->albumOffset));
}

NAN_SETTER(NodeSearch::setAlbumOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->albumOffset = value->ToInteger()->Value();
}

NAN_GETTER(NodeSearch::getArtistOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->artistOffset));
}

NAN_SETTER(NodeSearch::setArtistOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->artistOffset = value->ToInteger()->Value();
}

NAN_GETTER(NodeSearch::getPlaylistOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->playlistOffset));
}

NAN_SETTER(NodeSearch::setPlaylistOffset) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->playlistOffset = value->ToInteger()->Value();
}

NAN_GETTER(NodeSearch::getTrackLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->trackLimit));
}

NAN_SETTER(NodeSearch::setTrackLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->trackLimit = value->ToInteger()->Value();
}

NAN_GETTER(NodeSearch::getAlbumLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->albumLimit));
}

NAN_SETTER(NodeSearch::setAlbumLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->albumLimit = value->ToInteger()->Value();
}

NAN_GETTER(NodeSearch::getArtistLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->artistLimit));
}

NAN_SETTER(NodeSearch::setArtistLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->artistLimit = value->ToInteger()->Value();
}

NAN_GETTER(NodeSearch::getPlaylistLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->playlistLimit));
}

NAN_SETTER(NodeSearch::setPlaylistLimit) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  nodeSearch->playlistLimit = value->ToInteger()->Value();
}

NAN_METHOD(NodeSearch::New) {
  Nan::EscapableHandleScope scope;
  NodeSearch* search;
  Nan::Utf8String searchQuery(info[0]->ToString());
  if(info.Length() == 1) {
    search = new NodeSearch(*searchQuery);
  } else if(info.Length() == 2) {
    int offset = info[1]->ToInteger()->Value();
    search = new NodeSearch(*searchQuery, offset);
  } else if(info.Length() == 3) {
    int offset = info[1]->ToInteger()->Value();
    int limit = info[2]->ToInteger()->Value();
    search = new NodeSearch(*searchQuery, offset, limit);
  } else {
    return Nan::ThrowError("Please provide an object to the node-spotify initializer function");
  }
  search->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_GETTER(NodeSearch::didYouMean) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeSearch->search->didYouMeanText().c_str()).ToLocalChecked());
}

NAN_GETTER(NodeSearch::getLink) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeSearch->search->link().c_str()).ToLocalChecked());
}

NAN_METHOD(NodeSearch::getTrack) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  if(info.Length() < 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("getTrack needs a number as its first argument.");
  }

  int position = info[0]->ToNumber()->IntegerValue();
  if(position >= nodeSearch->search->numTracks() || position < 0) {
    return Nan::ThrowError("Track index out of bounds");
  }

  NodeTrack* nodeTrack = new NodeTrack(nodeSearch->search->getTrack(position));
  info.GetReturnValue().Set(nodeTrack->createInstance());
}

NAN_METHOD(NodeSearch::getAlbum) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  if(info.Length() < 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("getAlbum needs a number as its first argument.");
  }

  int position = info[0]->ToNumber()->IntegerValue();
  if(position >= nodeSearch->search->numAlbums() || position < 0) {
    return Nan::ThrowError("Album index out of bounds");
  }

  NodeAlbum* nodeAlbum = new NodeAlbum(nodeSearch->search->getAlbum(position));
  info.GetReturnValue().Set(nodeAlbum->createInstance());
}

NAN_METHOD(NodeSearch::getArtist) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  if(info.Length() < 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("getArtist needs a number as its first argument.");
  }

  int position = info[0]->ToNumber()->IntegerValue();
  if(position >= nodeSearch->search->numArtists() || position < 0) {
    return Nan::ThrowError("Artist index out of bounds");
  }

  NodeArtist* nodeArtist = new NodeArtist(nodeSearch->search->getArtist(position));
  info.GetReturnValue().Set(nodeArtist->createInstance());
}

NAN_METHOD(NodeSearch::getPlaylist) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  if(info.Length() < 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("getPlaylist needs a number as its first argument.");
  }

  int position = info[0]->ToNumber()->IntegerValue();
  if(position >= nodeSearch->search->numPlaylists() || position < 0) {
    return Nan::ThrowError("Playlist index out of bounds");
  }

  NodePlaylist* nodePlaylist = new NodePlaylist(nodeSearch->search->getPlaylist(position));
  info.GetReturnValue().Set(nodePlaylist->createInstance());
}

NAN_GETTER(NodeSearch::getTotalTracks) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->totalTracks()));
}

NAN_GETTER(NodeSearch::getNumTracks) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->numTracks()));
}

NAN_GETTER(NodeSearch::getTotalAlbums) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->totalAlbums()));
}

NAN_GETTER(NodeSearch::getNumAlbums) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->numAlbums()));
}

NAN_GETTER(NodeSearch::getTotalArtists) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->totalArtists()));
}

NAN_GETTER(NodeSearch::getNumArtists) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->numArtists()));
}

NAN_GETTER(NodeSearch::getTotalPlaylists) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->totalPlaylists()));
}

NAN_GETTER(NodeSearch::getNumPlaylists) {
  Nan::HandleScope scope;
  NodeSearch* nodeSearch = Nan::ObjectWrap::Unwrap<NodeSearch>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeSearch->search->numPlaylists()));
}

void NodeSearch::init() {
  Nan::HandleScope scope;
  Local<FunctionTemplate> constructorTemplate = Nan::New<FunctionTemplate>(New);
  constructorTemplate->SetClassName(Nan::New<String>("Search").ToLocalChecked());
  constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(constructorTemplate, "execute", execute);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("trackOffset").ToLocalChecked(), getTrackOffset, setTrackOffset);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("trackLimit").ToLocalChecked(), getTrackLimit, setTrackLimit);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("albumOffset").ToLocalChecked(), getAlbumOffset, setAlbumOffset);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("albumLimit").ToLocalChecked(), getAlbumLimit, setAlbumLimit);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("artistOffset").ToLocalChecked(), getArtistOffset, setArtistOffset);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("artistLimit").ToLocalChecked(), getArtistLimit, setArtistLimit);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("playlistOffset").ToLocalChecked(), getPlaylistOffset, setPlaylistOffset);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("playlistLimit").ToLocalChecked(), getPlaylistLimit, setPlaylistLimit);
  NodeSearch::constructorTemplate.Reset(constructorTemplate);
}
