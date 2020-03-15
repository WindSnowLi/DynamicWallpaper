#include "pch.h"
#include "VedioPlayer.h"

bool VedioPlayer::setVolume(int volume){
	Volume = volume;
	if (m != NULL && mp != NULL && ml != NULL && mlp != NULL) {
		libvlc_audio_set_volume(mp, Volume);
	}
	return true;
}


bool VedioPlayer::releasePlayer(){
	if (m != NULL && mp != NULL && ml != NULL && mlp != NULL) {
		libvlc_media_list_release(ml);
		libvlc_media_list_player_release(mlp);
		libvlc_media_player_release(mp);
		libvlc_release(inst);
	}
	return true;
}



bool VedioPlayer::loadPlayer(char* videoPath){
	if (m != NULL && mp != NULL && ml != NULL && mlp != NULL) {
		releasePlayer();
	}
	inst = libvlc_new(0, NULL);
	m = libvlc_media_new_path(inst, videoPath);

	ml = libvlc_media_list_new(inst);
	mp = libvlc_media_player_new_from_media(m);
	mlp = libvlc_media_list_player_new(inst);
	libvlc_media_list_add_media(ml, m);
	libvlc_media_release(m);
	libvlc_media_list_player_set_media_list(mlp, ml);
	libvlc_media_list_player_set_media_player(mlp, mp);
	//libvlc_media_list_player_set_playback_mode(mlp, libvlc_playback_mode_loop);
	libvlc_media_player_set_hwnd(mp, workerw);
	libvlc_audio_set_volume(mp, Volume);
	libvlc_media_list_player_play(mlp);
	return true;
}

float VedioPlayer::get_position()
{
	float temp = 0;
	if (mp != NULL) {
		temp = libvlc_media_player_get_position(mp);
	}
	
	return temp;
}

float VedioPlayer::get_length()
{
	float temp = 0;
	if (mp != NULL) {
		temp = libvlc_media_player_get_length(mp);
	}
	return temp; 
}

bool VedioPlayer::set_position(float position)
{
	if (mp != NULL) {
		libvlc_media_player_set_position(mp, position);
	}
	return true;
}

bool VedioPlayer::set_stop() {
	if (mp != NULL) {
		libvlc_media_player_pause(mp);
	}
	return true;
}

bool VedioPlayer::set_play() {
	if (mp != NULL) {
		libvlc_media_player_play(mp);
	}
	return true;
}