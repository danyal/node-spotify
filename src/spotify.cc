#include <node.h>
#include <v8.h>

#include <mach/clock.h>
#include <mach/mach.h>

#include "SpotifyService.h"

using namespace v8;

SpotifyService* spotifyService;

Handle<Value> login(const Arguments& args) {
	HandleScope scope;
	std::string user("USER");
	std::string password("PASSWORD");
	spotifyService->login(user, password);
	return scope.Close(Undefined());
}

Handle<Value> logout(const Arguments& args) {
	HandleScope scope;
	Callback<SpotifyService>* logoutCallback = new Callback<SpotifyService>(spotifyService, &SpotifyService::logout);
	spotifyService->executeSpotifyAPIcall(logoutCallback);
	return Undefined();
}

Handle<Value> userName(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> printPlaylists(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}
void init(Handle<Object> target) {
	  target->Set(String::NewSymbol("login"),
			        FunctionTemplate::New(login)->GetFunction());
	  target->Set(String::NewSymbol("logout"),
			        FunctionTemplate::New(logout)->GetFunction());
	  target->Set(String::NewSymbol("userName"),
			        FunctionTemplate::New(userName)->GetFunction());
	  target->Set(String::NewSymbol("printPlaylists"),
			        FunctionTemplate::New(printPlaylists)->GetFunction());
	  spotifyService = new SpotifyService();
}
NODE_MODULE(spotify, init)
