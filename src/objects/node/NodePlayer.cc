#include "NodePlayer.h"
#include "NodeTrack.h"
#include "../../callbacks/SessionCallbacks.h"
#include "../../exceptions.h"
#include "../../utils/V8Utils.h"

NodePlayer::NodePlayer(std::shared_ptr<Player> _player) : player(_player) {}

NodePlayer::~NodePlayer() {

}

NodePlayer::NodePlayer(const NodePlayer& other) {

}

NAN_METHOD(NodePlayer::pause) {
  Nan::HandleScope scope;
  NodePlayer* nodePlayer = Nan::ObjectWrap::Unwrap<NodePlayer>(info.This());
  nodePlayer->player->pause();
  return;
}

NAN_METHOD(NodePlayer::stop) {
  Nan::HandleScope scope;
  NodePlayer* nodePlayer = Nan::ObjectWrap::Unwrap<NodePlayer>(info.This());
  nodePlayer->player->stop();
  return;
}

NAN_METHOD(NodePlayer::resume) {
  Nan::HandleScope scope;
  NodePlayer* nodePlayer = Nan::ObjectWrap::Unwrap<NodePlayer>(info.This());
  nodePlayer->player->resume();
  return;
}

NAN_METHOD(NodePlayer::play) {
  Nan::HandleScope scope;
  if(info.Length() < 1) {
    return Nan::ThrowError("play needs a track as its first argument.");
  }
  NodePlayer* nodePlayer = Nan::ObjectWrap::Unwrap<NodePlayer>(info.This());
  NodeTrack* nodeTrack = Nan::ObjectWrap::Unwrap<NodeTrack>(info[0]->ToObject());
  try {
    nodePlayer->player->play(nodeTrack->track);
  } catch (const TrackNotPlayableException& e) {
    return Nan::ThrowError("Track not playable");
  }
#ifndef NODE_SPOTIFY_NATIVE_SOUND
  catch (const NoAudioHandlerException& e) {
    return Nan::ThrowError("No audio handler registered. Use spotify.useNodejsAudio().");
  }
#endif

  return;
}

NAN_METHOD(NodePlayer::seek) {
  Nan::HandleScope scope;
  if(info.Length() < 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("seek needs an integer as its first argument.");
  }
  NodePlayer* nodePlayer = Nan::ObjectWrap::Unwrap<NodePlayer>(info.This());
  int second = info[0]->ToInteger()->Value();
  nodePlayer->player->seek(second);
  return;
}

NAN_GETTER(NodePlayer::getCurrentSecond) {
  Nan::HandleScope scope;
  NodePlayer* nodePlayer = Nan::ObjectWrap::Unwrap<NodePlayer>(info.This());
  info.GetReturnValue().Set(Nan::New<Integer>(nodePlayer->player->currentSecond));
}

NAN_METHOD(NodePlayer::on) {
  Nan::HandleScope scope;
  if(info.Length() < 1 || !info[0]->IsObject()) {
    return Nan::ThrowError("on needs an object as its first argument.");
  }
  Handle<Object> callbacks = info[0]->ToObject();
  Handle<String> endOfTrackKey = Nan::New<String>("endOfTrack").ToLocalChecked();
  SessionCallbacks::endOfTrackCallback = V8Utils::getFunctionFromObject(callbacks, endOfTrackKey);
  return;
}

NAN_METHOD(NodePlayer::off) {
  Nan::HandleScope scope;
  SessionCallbacks::endOfTrackCallback = std::unique_ptr<Nan::Callback>(new Nan::Callback());
  return;
}

void NodePlayer::init() {
  Nan::HandleScope scope;
  Local<FunctionTemplate> constructorTemplate = Nan::New<FunctionTemplate>();
  constructorTemplate->SetClassName(Nan::New<String>("Player").ToLocalChecked());
  constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(constructorTemplate, "on", on);
  Nan::SetPrototypeMethod(constructorTemplate, "off", off);

  Nan::SetPrototypeMethod(constructorTemplate, "play", play);
  Nan::SetPrototypeMethod(constructorTemplate, "pause", pause);
  Nan::SetPrototypeMethod(constructorTemplate, "resume", resume);
  Nan::SetPrototypeMethod(constructorTemplate, "stop", stop);
  Nan::SetPrototypeMethod(constructorTemplate, "seek", seek);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("currentSecond").ToLocalChecked(), &getCurrentSecond);
  NodePlayer::constructorTemplate.Reset(constructorTemplate);
}
