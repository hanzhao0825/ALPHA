#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QPainter>
#include <phonon/MediaObject>
#include <phonon/BackendCapabilities>
#include <QSound>
#include <QDirIterator>
#include <math.h>
#include <QLabel>
#include <QTextCodec>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

protected:
        void paintEvent(QPaintEvent *);
        void keyPressEvent(QKeyEvent *);
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QTextCodec *codec;
    int phase;
    QString filename,realname[1000];
    char file_music[1000][100],file_img[1000][100];
    int level[1000];
    double bpm, offset;
    QTime *mytime;
    struct Note{
        int asc;
        double t;
        int pos, sum;
        int live;
        double curscore;
    };
    Note note[10000];
    int notesum,linesum;
    int notec[4];
    int star[1000],stars,stare;
    double anssin[20],anscos[20];
    int seqs,seqe;
    QString sel_name[1000];
    int cursel,lassel,selp,chngp,seip,prep,themep;
    QPixmap imgnote[128],imgstar[3],game_back,sei_back,imgchng,album;
    QPixmap theme,imgsb1,imgsb2,imgkey,imgmark[3],imgr[4];
    QPixmap imgbottom,imghilight;
    QString ckeyb;
    Phonon::MediaObject *music,*mka,*tempmusic;
    int op;
    int bgmplaying;
    double score;
    int combo;
    QString rank;
    int timebgm;
    QString filelist[1000];
    int filehiscore[1000][10];
    int filesum;
    void init();
    int kx[128],ky[128],klen[5];
    void loadalp();
    void gameinit();
    void findalp();
    void getAlps();
    void seiseki();
    void themedone();
    
private:
    Ui::Widget *ui;
    QTimer *timer;

public slots:
    void timerUpdate();
    void musicselect();
    void pre_seiseki();
};



#endif // WIDGET_H
