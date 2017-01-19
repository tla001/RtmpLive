#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QDialog>
#include "vlcplayer.h"
#include <QTimer>
#include <QTime>

namespace Ui {
class videoPlayer;
}

class videoPlayer : public QDialog
{
    Q_OBJECT

public:
    explicit videoPlayer(QWidget *parent = 0);
    ~videoPlayer();

private:
    Ui::videoPlayer *ui;
    QString fileName;
    QString url;
    int64_t totalTime;
    int64_t currentTime;
    int volume;
    int runFlag;
    bool streamFlag;
    QString stateInfo[5];

    VLCPlayer *player;
    QTimer *timer;


public slots:
    void doStartButton();
    void doStopButton();
    void doVolumeButton();
    void doOpenFileButton();
    void doSelectInput();
    void doSpeedButton();

    void doUpdateStateInfo(int index);
    void doUpdateFileInfo(QString info);

    void doTimerSlot();
    void doVolumeChanged();
    void doProcessChanged();
    void doPlayEnd();

};

#endif // VIDEOPLAYER_H
