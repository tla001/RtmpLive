#include "vlcplayer.h"
#include <QDebug>

VLCPlayer::VLCPlayer(){

}
VLCPlayer::~VLCPlayer(){

}

void VLCPlayer::init(uint32_t winId){
    instance =libvlc_new(0,NULL);
    this->winId=winId;
//    qDebug()<<winId;
    player =libvlc_media_player_new(instance);
    libvlc_media_player_set_hwnd(player,(void *)winId);
}

void VLCPlayer::playFile(string file){
    libvlc_media_t *media=libvlc_media_new_path(instance,file.c_str());
//    totalTime=libvlc_media_get_duration(media);
//    qDebug()<<file.c_str();
    if(!media)
        qDebug()<<"media null";
    else{
        libvlc_media_player_set_media(player,media);
    //     player = libvlc_media_player_new_from_media (media);
        libvlc_media_release(media);
    //    libvlc_media_player_set_xwindow(player,winId);
        libvlc_media_player_play(player);
    }
}
void VLCPlayer::playUrl(string url){
    libvlc_media_t *media=libvlc_media_new_location(instance,url.c_str());
    if(!media)
        qDebug()<<"net media null";
    else{
        qDebug()<<"net media not null";
        libvlc_media_player_set_media(player,media);
        libvlc_media_release(media);
        libvlc_media_player_play(player);
        _sleep(2000);
    }
}
void VLCPlayer::pause(bool stat){
    if(stat){
        libvlc_media_player_set_pause(player,1);
    }else{
        libvlc_media_player_set_pause(player,0);
    }
}
void VLCPlayer::stop(){
    if(player){
        libvlc_media_player_stop(player);
//        libvlc_media_player_release(player);
    }
}
libvlc_time_t VLCPlayer::getTotalTime(){
    return libvlc_media_player_get_length(player);
//    return totalTime;
}
libvlc_time_t VLCPlayer::getCurrentTime(){
    return libvlc_media_player_get_time(player);
}
libvlc_state_t VLCPlayer::getPlayerStat(){
    return libvlc_media_player_get_state(player);
}
void VLCPlayer::setPlayerPostion(float pos){
    libvlc_media_player_set_position(player,pos);
}
void VLCPlayer::setVolume(int value){
    libvlc_audio_set_volume(player,value);
}
void VLCPlayer::setSpeed(float speed){
    libvlc_media_player_set_rate(player,speed);
}
