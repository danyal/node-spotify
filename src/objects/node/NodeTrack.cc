#include "NodeTrack.h"
#include "NodeArtist.h"
#include "NodeAlbum.h"

NodeTrack::NodeTrack(std::shared_ptr<Track> _track) : track(_track) {

};

NodeTrack::~NodeTrack() {

}

NAN_GETTER(NodeTrack::getName) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeTrack->track->name().c_str()).ToLocalChecked());
}

NAN_GETTER(NodeTrack::getLink) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeTrack->track->link().c_str()).ToLocalChecked());
}

NAN_GETTER(NodeTrack::getDuration) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeTrack->track->duration()/1000));
}

NAN_GETTER(NodeTrack::getAvailability) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeTrack->track->getAvailability()));
}

NAN_GETTER(NodeTrack::getPopularity) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodeTrack->track->popularity()));
}

NAN_GETTER(NodeTrack::getArtists) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  Local<Array> jsArtists = Nan::New<Array>(nodeTrack->track->artists().size());
  for(int i = 0; i < (int)nodeTrack->track->artists().size(); i++) {
    if(nodeTrack->track->artists()[i]) {
      NodeArtist* nodeArtist = new NodeArtist(std::move(nodeTrack->track->artists()[i]));
      jsArtists->Set(Nan::New<Number>(i), nodeArtist->createInstance());
    } else {
      jsArtists->Set(Nan::New<Number>(i), Nan::Undefined());
    }
  }
  info.GetReturnValue().Set(jsArtists);
}

NAN_GETTER(NodeTrack::getAlbum) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  if(nodeTrack->track->album()) {
    NodeAlbum* nodeAlbum = new NodeAlbum(nodeTrack->track->album());
    info.GetReturnValue().Set(nodeAlbum->createInstance());
  } else {
    return;
  }
}

NAN_GETTER(NodeTrack::getStarred) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  info.GetReturnValue().Set(Nan::New<Boolean>(nodeTrack->track->starred()));
}

NAN_SETTER(NodeTrack::setStarred) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  nodeTrack->track->setStarred(value->ToBoolean()->Value());
}

NAN_GETTER(NodeTrack::isLoaded) {
  Nan::HandleScope scope;
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info.This());
  info.GetReturnValue().Set(Nan::New<Boolean>(nodeTrack->track->isLoaded()));
}

Handle<FunctionTemplate> NodeTrack::init() {
  Nan::EscapableHandleScope scope;
  Handle<FunctionTemplate> constructorTemplate = NodeWrapped::init("Track");
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("name").ToLocalChecked(), getName);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("name").ToLocalChecked(), getName);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("link").ToLocalChecked(), getLink);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("link").ToLocalChecked(), getLink);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("duration").ToLocalChecked(), getDuration);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("duration").ToLocalChecked(), getDuration);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("artists").ToLocalChecked(), getArtists);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("artists").ToLocalChecked(), getArtists);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("album").ToLocalChecked(), getAlbum);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("album").ToLocalChecked(), getAlbum);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("starred").ToLocalChecked(), getStarred, setStarred);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("starred").ToLocalChecked(), getStarred, setStarred);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("popularity").ToLocalChecked(), getPopularity);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("popularity").ToLocalChecked(), getPopularity);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("isLoaded").ToLocalChecked(), isLoaded);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("isLoaded").ToLocalChecked(), isLoaded);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("availability").ToLocalChecked(), getAvailability);
  // constructorTemplate->InstanceTemplate()->SetAccessor(Nan::New<String>("availability").ToLocalChecked(), getAvailability);
  NodeTrack::constructorTemplate.Reset(constructorTemplate);
  return scope.Escape(constructorTemplate);
}
