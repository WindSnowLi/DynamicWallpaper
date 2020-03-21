#include <windows.h>
#include <WinUser.h>
#define ssize_t SSIZE_T
#include <vlc/vlc.h>
#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")
using namespace std;
#pragma once
class VideoPlayer
{
private:
	libvlc_instance_t* inst = NULL;
	libvlc_media_t* m = NULL;
	libvlc_media_list_t* ml = NULL;
	libvlc_media_player_t* mp = NULL;
	libvlc_media_list_player_t* mlp = NULL;
public:
	bool setVolume(int volume);
	bool releasePlayer();
	bool loadPlayer(char* videoPath);
	float get_position();
	float get_length();
	bool set_position(float position);
	bool set_stop();
	bool set_play();
	HWND workerw;
private:
	int Volume=30;
	
};

