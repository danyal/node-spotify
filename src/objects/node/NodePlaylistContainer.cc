#include "NodePlaylistContainer.h"
#include "NodePlaylist.h"
#include "NodePlaylistFolder.h"
#include "NodeUser.h"
#include "../../exceptions.h"
#include "../../utils/V8Utils.h"

NodePlaylistContainer::NodePlaylistContainer(std::shared_ptr<PlaylistContainer> _playlistContainer) : playlistContainer(_playlistContainer),
  playlistContainerCallbacksHolder(playlistContainer->playlistContainer, this) {
}

NodePlaylistContainer::~NodePlaylistContainer() {
}

NAN_GETTER(NodePlaylistContainer::getOwner) {
  Nan::HandleScope scope;
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  NodeUser* nodeUser = new NodeUser(nodePlaylistContainer->playlistContainer->owner());
  info.GetReturnValue().Set(nodeUser->createInstance());
}

NAN_GETTER(NodePlaylistContainer::getNumPlaylists) {
  Nan::HandleScope scope;
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodePlaylistContainer->playlistContainer->numPlaylists()));
}

NAN_METHOD(NodePlaylistContainer::getPlaylist) {
  Nan::HandleScope scope;
  if(info.Length() < 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("getPlaylist needs an interger as its first argument.");
  }
  int index = info[0]->ToNumber()->IntegerValue();
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  if(index < 0 || index >= nodePlaylistContainer->playlistContainer->numPlaylists()) {
    return Nan::ThrowError("Index out of range.");
  }
  std::shared_ptr<PlaylistBase> playlist = nodePlaylistContainer->playlistContainer->getPlaylist(index);

  Handle<Value> outNodePlaylist;
  if(!playlist->isFolder) {
    NodePlaylist* nodePlaylist = new NodePlaylist(std::static_pointer_cast<Playlist>(playlist));
    outNodePlaylist = nodePlaylist->createInstance();
  } else {
    NodePlaylistFolder* nodePlaylistFolder = new NodePlaylistFolder(std::static_pointer_cast<PlaylistFolder>(playlist));
    outNodePlaylist = nodePlaylistFolder->createInstance();
  }

  info.GetReturnValue().Set(outNodePlaylist);
}

NAN_METHOD(NodePlaylistContainer::addPlaylist) {
  Nan::HandleScope scope;
  if(info.Length() < 1 || !info[0]->IsString()) {
    return Nan::ThrowError("addPlaylist needs a string as its argument");
  }
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  String::Utf8Value playlistName(info[0]->ToString());
  try {
    nodePlaylistContainer->playlistContainer->addPlaylist(std::string(*playlistName));
  } catch(const PlaylistCreationException& e) {
    return Nan::ThrowError("Playlist creation failed");
  }
  return;
}

NAN_METHOD(NodePlaylistContainer::addFolder) {
  Nan::HandleScope scope;
  if(info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsString()) {
    return Nan::ThrowError("addFolder needs a number and a string as arguments.");
  }
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  int index = info[0]->ToNumber()->IntegerValue();
  String::Utf8Value folderName(info[1]->ToString());
  try {
    nodePlaylistContainer->playlistContainer->addFolder(index, std::string(*folderName));
  } catch(const PlaylistCreationException& e) {
    return Nan::ThrowError("Folder creation failed");
  }
  return;
}

NAN_METHOD(NodePlaylistContainer::deletePlaylist) {
  Nan::HandleScope scope;
  if(info.Length() < 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("deletePlaylist needs an integer as its first argument.");
  }
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  int position = info[0]->ToNumber()->IntegerValue();
  nodePlaylistContainer->playlistContainer->removePlaylist(position);
  return;
}

NAN_METHOD(NodePlaylistContainer::movePlaylist) {
  Nan::HandleScope scope;
  if(info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return Nan::ThrowError("Move playlist needs 2 numbers as its first arguments.");
  }
  int index = info[0]->ToNumber()->IntegerValue();
  int newPosition = info[1]->ToNumber()->IntegerValue();
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  try {
    nodePlaylistContainer->playlistContainer->movePlaylist(index, newPosition);
  } catch(const PlaylistNotMoveableException& e) {
    return Nan::ThrowError(e.message.c_str());
  }
  return;
}

NAN_GETTER(NodePlaylistContainer::isLoaded) {
  Nan::HandleScope scope;
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  info.GetReturnValue().Set(Nan::New<Boolean>(nodePlaylistContainer->playlistContainer->isLoaded()));
}

NAN_METHOD(NodePlaylistContainer::on) {
  Nan::HandleScope scope;
  if(info.Length() < 1 || !info[0]->IsObject()) {
    return Nan::ThrowError("on needs an object as its first argument.");
  }
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  Handle<Object> callbacks = info[0]->ToObject();
  Handle<String> playlistAddedKey = Nan::New<String>("playlistAdded").ToLocalChecked();
  Handle<String> playlistMovedKey = Nan::New<String>("playlistMoved").ToLocalChecked();
  Handle<String> playlistRemovedKey = Nan::New<String>("playlistRemoved").ToLocalChecked();
  nodePlaylistContainer->playlistContainerCallbacksHolder.playlistAddedCallback = V8Utils::getFunctionFromObject(callbacks, playlistAddedKey);
  nodePlaylistContainer->playlistContainerCallbacksHolder.playlistMovedCallback = V8Utils::getFunctionFromObject(callbacks, playlistMovedKey);
  nodePlaylistContainer->playlistContainerCallbacksHolder.playlistRemovedCallback = V8Utils::getFunctionFromObject(callbacks, playlistRemovedKey);
  nodePlaylistContainer->playlistContainerCallbacksHolder.setCallbacks();
  return;
}

NAN_METHOD(NodePlaylistContainer::off) {
  Nan::HandleScope scope;
  NodePlaylistContainer* nodePlaylistContainer = Nan::ObjectWrap::Unwrap<NodePlaylistContainer>(info.This());
  nodePlaylistContainer->playlistContainerCallbacksHolder.unsetCallbacks();
  return;
}

void NodePlaylistContainer::init() {
  Nan::HandleScope scope;
  Local<FunctionTemplate> constructorTemplate = Nan::New<FunctionTemplate>();
  constructorTemplate->SetClassName(Nan::New<String>("PlaylistContainer").ToLocalChecked());
  constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(constructorTemplate, "on", on);
  Nan::SetPrototypeMethod(constructorTemplate, "off", off);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("owner").ToLocalChecked(), getOwner);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("numPlaylists").ToLocalChecked(), getNumPlaylists);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("isLoaded").ToLocalChecked(), isLoaded);
  Nan::SetPrototypeMethod(constructorTemplate, "getPlaylist", getPlaylist);
  Nan::SetPrototypeMethod(constructorTemplate, "addPlaylist", addPlaylist);
  Nan::SetPrototypeMethod(constructorTemplate, "addFolder", addFolder);
  Nan::SetPrototypeMethod(constructorTemplate, "deletePlaylist", deletePlaylist);
  Nan::SetPrototypeMethod(constructorTemplate, "movePlaylist", movePlaylist);

  NodePlaylistContainer::constructorTemplate.Reset(constructorTemplate);
}
