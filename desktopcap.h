#ifndef DESKTOPCAP_H
#define DESKTOPCAP_H
#include <QThread>
#include <QTimer>
#include <QDesktopWidget>
#include <QScreen>
#include <QPixmap>
#include <QApplication>
#include <QImage>

class DesktopCap: public QThread{
    Q_OBJECT
public:
    DesktopCap();
    ~DesktopCap();

    int init(int width, int height);
    void getFrame(unsigned char *&framebuff);
    void doCap();
    void doStop();
private:
    QTimer *timer;
    QDesktopWidget *desk;
    QScreen *screen;
    QPixmap  pix;
    QImage img;
    int width;
    int height;
    int size;
    unsigned char *buf;
public slots:
    void worker();
signals:
    void capFinish();
};

#endif // DESKTOPCAP_H
