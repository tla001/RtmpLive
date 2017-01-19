#ifndef VLCPLAYER_H
#define VLCPLAYER_H
#include "vlc/vlc.h"
#include <string>

using namespace std;
class VLCPlayer{
public:
    explicit VLCPlayer();
    virtual ~VLCPlayer();

    void init(uint32_t winId);
    void playFile(string file);
    void playUrl(string url);
    void stop();
    void pause(bool stat);
    libvlc_time_t getTotalTime();
    libvlc_time_t getCurrentTime();
    libvlc_state_t getPlayerStat();
    void setPlayerPostion(float pos);
    void setVolume(int value);
    void setSpeed(float speed);


private:
    libvlc_instance_t *instance;
    libvlc_media_player_t *player;
    uint32_t winId;
    int64_t totalTime;
};

#endif // VLCPLAYER_H
