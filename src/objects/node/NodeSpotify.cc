#include "NodeSpotify.h"
#include "../../audio/AudioHandler.h"
#include "../../audio/NativeAudioHandler.h"
#include "../../audio/NodeAudioHandler.h"
#include "../../Application.h"
#include "../../exceptions.h"
#include "../../callbacks/SessionCallbacks.h"
#include "../spotify/SpotifyOptions.h"
#include "NodePlaylist.h"
#include "NodePlaylistFolder.h"
#include "NodePlaylistContainer.h"
#include "NodePlayer.h"
#include "NodeArtist.h"
#include "NodeAlbum.h"
#include "NodeTrack.h"
#include "NodeUser.h"
#include "../../utils/V8Utils.h"

extern Application* application;

NodeSpotify::NodeSpotify(Handle<Object> options) {
  /*
   * Important note: The session callbacks must be initialized before
   * the sp_session is started. The uv timer and async handle must be set up
   * as they will be used as soon as the sp_session is created, which happens in the
   * NodeSpotify ctor.
   */
  SessionCallbacks::init();

  SpotifyOptions _options;
  Nan::HandleScope scope;
  Handle<String> settingsFolderKey = Nan::New<String>("settingsFolder").ToLocalChecked();
  Handle<String> cacheFolderKey = Nan::New<String>("cacheFolder").ToLocalChecked();
  Handle<String> traceFileKey = Nan::New<String>("traceFile").ToLocalChecked();
  Handle<String> appkeyFileKey = Nan::New<String>("appkeyFile").ToLocalChecked();
  if(options->Has(settingsFolderKey)) {
    String::Utf8Value settingsFolderValue(options->Get(settingsFolderKey)->ToString());
    _options.settingsFolder = *settingsFolderValue;
  } else {
    _options.settingsFolder = "settings";
  }
  if(options->Has(cacheFolderKey)) {
    String::Utf8Value cacheFolderValue(options->Get(cacheFolderKey)->ToString());
    _options.cacheFolder = *cacheFolderValue;
  } else {
    _options.cacheFolder = "cache";
  }
  if(options->Has(traceFileKey)) {
    String::Utf8Value traceFileValue(options->Get(traceFileKey)->ToString());
    _options.traceFile = *traceFileValue;
  }
  if(options->Has(appkeyFileKey)) {
    String::Utf8Value appkeyFileValue(options->Get(appkeyFileKey)->ToString());
    _options.appkeyFile = *appkeyFileValue;
  }
  spotify = std::unique_ptr<Spotify>(new Spotify(_options));
}

NodeSpotify::~NodeSpotify() {

}

NAN_METHOD(NodeSpotify::createFromLink) {
  Nan::HandleScope scope;
  Handle<Value> out;
  String::Utf8Value linkToParse(info[0]->ToString());
  sp_link* parsedLink = sp_link_create_from_string(*linkToParse);
  if(parsedLink != nullptr) {
    sp_linktype linkType = sp_link_type(parsedLink);
    switch(linkType) {
      case SP_LINKTYPE_TRACK:
      {
        sp_track* track = sp_link_as_track(parsedLink);
        NodeTrack* nodeTrack = new NodeTrack(std::make_shared<Track>(track));
        out = nodeTrack->createInstance();
        break;
      }
      case SP_LINKTYPE_ALBUM:
      {
        sp_album* album = sp_link_as_album(parsedLink);
        NodeAlbum* nodeAlbum = new NodeAlbum(std::unique_ptr<Album>(new Album(album)));
        out = nodeAlbum->createInstance();
        break;
      }
      case SP_LINKTYPE_ARTIST:
      {
        sp_artist* artist = sp_link_as_artist(parsedLink);
        NodeArtist* nodeArtist = new NodeArtist(std::unique_ptr<Artist>(new Artist(artist)));
        out = nodeArtist->createInstance();
        break;
      }
      case SP_LINKTYPE_PROFILE:
      {
        sp_user* user = sp_link_as_user(parsedLink);
        NodeUser* nodeUser = new NodeUser(std::unique_ptr<User>(new User(user)));
        out = nodeUser->createInstance();
        break;
      }
      case SP_LINKTYPE_PLAYLIST:
      {
        sp_playlist* spPlaylist = sp_playlist_create(application->session, parsedLink);
        auto playlist = Playlist::fromCache(spPlaylist);
        NodePlaylist* nodePlaylist = new NodePlaylist(playlist);
        out = nodePlaylist->createInstance();
        break;
      }
      case SP_LINKTYPE_LOCALTRACK:
      {
        sp_track* track = sp_link_as_track(parsedLink);
        NodeTrack* nodeTrack = new NodeTrack(std::make_shared<Track>(track));
        out = nodeTrack->createInstance();
        break;
      }
      default:
        out = Nan::Undefined();
    }
    sp_link_release(parsedLink);
  } else {
    out = Nan::Undefined();
  }
  info.GetReturnValue().Set(out);
}

NAN_METHOD(NodeSpotify::login) {
  Nan::HandleScope scope;
  NodeSpotify* nodeSpotify = Nan::ObjectWrap::Unwrap<NodeSpotify>(info.This());
  String::Utf8Value v8User(info[0]->ToString());
  String::Utf8Value v8Password(info[1]->ToString());
  bool rememberMe = info[2]->ToBoolean()->Value();
  bool withRemembered = info[3]->ToBoolean()->Value();
  std::string user(*v8User);
  std::string password(*v8Password);
  nodeSpotify->spotify->login(user, password, rememberMe, withRemembered);
  return;
}

NAN_METHOD(NodeSpotify::logout) {
  Nan::HandleScope scope;
  if(info.Length() > 0) {
    SessionCallbacks::logoutCallback = std::unique_ptr<Nan::Callback>(new Nan::Callback(info[0].As<Function>()));
  }
  NodeSpotify* nodeSpotify = Nan::ObjectWrap::Unwrap<NodeSpotify>(info.This());
  nodeSpotify->spotify->logout();
  return;
}

NAN_GETTER(NodeSpotify::getPlaylistContainer) {
  Nan::HandleScope scope;
  NodePlaylistContainer* nodePlaylistContainer = new NodePlaylistContainer(application->playlistContainer);
  info.GetReturnValue().Set(nodePlaylistContainer->createInstance());
}

NAN_GETTER(NodeSpotify::getRememberedUser) {
  Nan::HandleScope scope;
  NodeSpotify* nodeSpotify = Nan::ObjectWrap::Unwrap<NodeSpotify>(info.This());
  info.GetReturnValue().Set(Nan::New<String>(nodeSpotify->spotify->rememberedUser().c_str()).ToLocalChecked());
}

NAN_GETTER(NodeSpotify::getSessionUser) {
  Nan::HandleScope scope;
  NodeSpotify* nodeSpotify = Nan::ObjectWrap::Unwrap<NodeSpotify>(info.This());
  NodeUser* nodeUser = new NodeUser(std::move(nodeSpotify->spotify->sessionUser()));
  info.GetReturnValue().Set(nodeUser->createInstance());
}

NAN_GETTER(NodeSpotify::getConstants) {
  Nan::HandleScope scope;
  Local<Object> constants = Nan::New<Object>();
  constants->Set(Nan::New<String>("ARTISTBROWSE_FULL").ToLocalChecked(), Nan::New<Number>(SP_ARTISTBROWSE_FULL));
  constants->Set(Nan::New<String>("ARTISTBROWSE_NO_TRACKS").ToLocalChecked(), Nan::New<Number>(SP_ARTISTBROWSE_NO_TRACKS));
  constants->Set(Nan::New<String>("ARTISTBROWSE_NO_ALBUMS").ToLocalChecked(), Nan::New<Number>(SP_ARTISTBROWSE_NO_ALBUMS));

  constants->Set(Nan::New<String>("PLAYLIST_TYPE_PLAYLIST").ToLocalChecked(), Nan::New<Number>(SP_PLAYLIST_TYPE_PLAYLIST));
  constants->Set(Nan::New<String>("PLAYLIST_TYPE_START_FOLDER").ToLocalChecked(), Nan::New<Number>(SP_PLAYLIST_TYPE_START_FOLDER));
  constants->Set(Nan::New<String>("PLAYLIST_TYPE_END_FOLDER").ToLocalChecked(), Nan::New<Number>(SP_PLAYLIST_TYPE_END_FOLDER));
  constants->Set(Nan::New<String>("PLAYLIST_TYPE_PLACEHOLDER").ToLocalChecked(), Nan::New<Number>(SP_PLAYLIST_TYPE_PLACEHOLDER));


  constants->Set(Nan::New<String>("SP_TRACK_AVAILABILITY_UNAVAILABLE").ToLocalChecked(), Nan::New<Number>(SP_TRACK_AVAILABILITY_UNAVAILABLE));
  constants->Set(Nan::New<String>("SP_TRACK_AVAILABILITY_AVAILABLE").ToLocalChecked(), Nan::New<Number>(SP_TRACK_AVAILABILITY_AVAILABLE));
  constants->Set(Nan::New<String>("SP_TRACK_AVAILABILITY_NOT_STREAMABLE").ToLocalChecked(), Nan::New<Number>(SP_TRACK_AVAILABILITY_NOT_STREAMABLE));
  constants->Set(Nan::New<String>("SP_TRACK_AVAILABILITY_BANNED_BY_ARTIST").ToLocalChecked(), Nan::New<Number>(SP_TRACK_AVAILABILITY_BANNED_BY_ARTIST));

  info.GetReturnValue().Set(constants);
}

#ifdef NODE_SPOTIFY_NATIVE_SOUND
NAN_METHOD(NodeSpotify::useNativeAudio) {
  Nan::HandleScope scope;
  //Since the old audio handler has to be deleted first, do an empty reset.
  application->audioHandler.reset();
  application->audioHandler = std::unique_ptr<AudioHandler>(new NativeAudioHandler());
  return;
}
#endif

NAN_METHOD(NodeSpotify::useNodejsAudio) {
  Nan::HandleScope scope;
  if(info.Length() < 1) {
    return Nan::ThrowError("useNodjsAudio needs a function as its first argument.");
  }
  //Since the old audio handler has to be deleted first, do an empty reset.
  application->audioHandler.reset();
  auto callback = std::unique_ptr<Nan::Callback>(new Nan::Callback(info[0].As<Function>()));
  application->audioHandler = std::unique_ptr<AudioHandler>(new NodeAudioHandler(std::move(callback)));

  Handle<Function> needMoreDataSetter = Nan::New<FunctionTemplate>(NodeAudioHandler::setNeedMoreData)->GetFunction();
  info.GetReturnValue().Set(needMoreDataSetter);
}

NAN_METHOD(NodeSpotify::on) {
  Nan::HandleScope scope;
  if(info.Length() < 1 || !info[0]->IsObject()) {
    return Nan::ThrowError("on needs an object as its first argument.");
  }
  Handle<Object> callbacks = info[0]->ToObject();
  Handle<String> metadataUpdatedKey = Nan::New<String>("metadataUpdated").ToLocalChecked();
  Handle<String> readyKey = Nan::New<String>("ready").ToLocalChecked();
  Handle<String> logoutKey = Nan::New<String>("logout").ToLocalChecked();
  Handle<String> playTokenLostKey = Nan::New<String>("playTokenLost").ToLocalChecked();
  SessionCallbacks::metadataUpdatedCallback = V8Utils::getFunctionFromObject(callbacks, metadataUpdatedKey);
  SessionCallbacks::loginCallback = V8Utils::getFunctionFromObject(callbacks, readyKey);
  SessionCallbacks::logoutCallback = V8Utils::getFunctionFromObject(callbacks, logoutKey);
  SessionCallbacks::playTokenLostCallback = V8Utils::getFunctionFromObject(callbacks, playTokenLostKey);
  return;
}

void NodeSpotify::init() {
  Nan::HandleScope scope;
  Handle<FunctionTemplate> constructorTemplate = NodeWrapped::init("Spotify");
  Nan::SetPrototypeMethod(constructorTemplate, "login", login);
  Nan::SetPrototypeMethod(constructorTemplate, "logout", logout);
  Nan::SetPrototypeMethod(constructorTemplate, "createFromLink", createFromLink);
  Nan::SetPrototypeMethod(constructorTemplate, "on", on);
#ifdef NODE_SPOTIFY_NATIVE_SOUND
  Nan::SetPrototypeMethod(constructorTemplate, "useNativeAudio", useNativeAudio);
#endif
  Nan::SetPrototypeMethod(constructorTemplate, "useNodejsAudio", useNodejsAudio);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("rememberedUser").ToLocalChecked(), getRememberedUser);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("sessionUser").ToLocalChecked(), getSessionUser);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("playlistContainer").ToLocalChecked(), getPlaylistContainer);
  Nan::SetAccessor(constructorTemplate->InstanceTemplate(), Nan::New<String>("constants").ToLocalChecked(), getConstants);

  NodeSpotify::constructorTemplate.Reset(constructorTemplate);
}
