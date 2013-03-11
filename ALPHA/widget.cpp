#include "widget.h"
#include "ui_widget.h"
#include <fstream>
#include <QKeyEvent>
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB18030"));
    init();
    phase=-1;
    themep=1000;
}

void Widget::init(){
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(16);
    mytime = new QTime();
    for(int i=65;i<=90;i++) imgnote[i].load(QString("./images/")+char(i)+".png");
    for(int i=0;i<3;i++) imgstar[i].load(QString("./images/star")+char(i+48)+".png");
    for(int i=0;i<4;i++) imgr[i].load(QString("./images/r")+char(i+48)+".png");
    theme.load(QString("./images/theme.png"));
    game_back.load(QString("./images/game_back.png"));
    sei_back.load(QString("./images/sei_back.png"));
    imgsb1.load(QString("./images/selbox1.png"));
    imgsb2.load(QString("./images/selbox2.png"));
    imgkey.load(QString("./images/key.png"));
    imgmark[1].load(QString("./images/mark1.png"));
    imgmark[2].load(QString("./images/mark2.png"));
    imgchng.load(QString("./images/change.png"));
    imgbottom.load(QString("./images/bottom.png"));
    imghilight.load(QString("./images/hilight.png"));
    ckeyb="QWERTYUIOP,ASDFGHJKL,ZXCVBNM,";
    ui->listWidget_file->hide();
    int x=0,y=0;
    klen[0]=10;klen[1]=9;klen[2]=7;
    for(int i=0;i<ckeyb.length();i++){
        if(ckeyb[i]==','){
            x=0;
            y++;
        }else{
            kx[int(ckeyb[i].toAscii())]=x;
            ky[int(ckeyb[i].toAscii())]=y;
            x++;
        }
    }
    for(int i=0;i<15;i++){
        anssin[i]=sin(3.14159*2*i/15);
        anscos[i]=cos(3.14159*2*i/15);
    }
    mka = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource(QString("./se/ka.mp3")));
    cursel=0;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadalp(){
    QByteArray ba = filename.toLatin1();
    const char *c_str2 = ba.data();
    ifstream fin(c_str2);
    qDebug()<<c_str2;
    fin>>bpm>>offset;
    char temp[100];
    int now=0,ls,lc,x;
    notesum=0;
    while(fin>>temp){
        ls=0;lc=0;x=0;
        for(int i=0;i<int(strlen(temp));i++){
            if(temp[i]==',')ls++;
        }
        for(int i=0;i<int(strlen(temp));i++){
            if(temp[i]==','){
                lc++;
                x=0;
                continue;
            }
            note[notesum].asc=temp[i];
            note[notesum].t=now+double(lc)/double(ls);
            note[notesum].pos=x;
            note[notesum].live=1;
            note[notesum].curscore=-1;
            notesum++;
            x++;
        }
        now++;
    }
    linesum=now;
    note[notesum].pos=0;
    note[0].curscore=0;
    for(int i=0;i<notesum;){
        int j=i+1;
        while(note[j].pos!=0)j++;
        for(int k=i;k<j;k++) note[k].sum=note[j-1].pos+1;
        i=j;
    }
    seqs=0;
    seqe=0;
}


void Widget::gameinit(){
    score=0;
    timebgm=2000;
    bgmplaying=0;
    combo=0;
    memset(star,0,sizeof(star));
    stars=0;stare=-1;
    notec[0]=notec[1]=notec[2]=notec[3]=0;
    music = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource(QString("./songs/")+file_music[cursel]));
    mytime->start();
}

void Widget::timerUpdate(){
    repaint();
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter pain(this);
    pain.setFont(QFont(QString::fromLocal8Bit("Courier New"),15,-1,false));
    if(phase==-1){
        pain.setOpacity(1.0-double(abs(themep-500))/500.0);
        pain.drawPixmap(0,0,800,600,theme);
        themep-=100;
        if(themep<=0) themedone();
    }else if(phase==1){
        if(selp!=-130)selp+=10;
        if(selp>0){
            selp=-130;
            tempmusic->stop();
            tempmusic = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource(QString("./songs/")+file_music[cursel]));
            tempmusic->play();
        }
        pain.drawPixmap(0, 0, 800, 600, album);
        double l=min(400,filesum*60);
        double y=100.0+l/filesum*cursel;
        pain.setOpacity(abs((op+700)%1200-500)/1300.0+.8);
        pain.drawPixmap(610+prep/2,150,200,350+double(selp)/130*350*(selp!=-130),imgsb2);
        op+=5;
        for(int i=0;i<filesum;i++){
            if(i!=cursel){
                pain.setPen(QPen(Qt::black));
                if(i!=lassel) pain.drawPixmap(70-5*abs(i-cursel)-prep,(i-cursel)*60+y,200,60,imgsb1);
                else pain.drawPixmap(70-5*abs(i-cursel)-prep,(i-cursel)*60+y,200-selp*(selp!=-130),60,imgsb1);
                pain.drawText(80-5*abs(i-cursel)-prep,int((i-cursel)*60+y+30),180,200,1,realname[i],0);
            }
            else{
                pain.setOpacity(abs(op%1000-500)/1300.0+.7);
                pain.setPen(QPen(Qt::red));
                pain.drawPixmap(70-prep,(i-cursel)*100+y,330+selp*(selp!=-130),60,imgsb1);
                pain.drawText(80-prep,int((i-cursel)*60+y+30),310,200,1,realname[i],0);
                pain.setOpacity(1);
            }
        }
        prep-=20;
        if(prep<0) prep=0;
        if(prep==0){
            char ranktext[10][10]={"1st","2nd","3rd","4th","5th","6th"};
            pain.setPen(QPen(QColor(0,0,0)));
            if(selp==-130){
                for(int i=0;i<6;i++){
                    char tempchar[10],tc2[10];
                    itoa(filehiscore[cursel][i],tempchar,10);
                    itoa(i+1,tc2,10);
                    pain.drawText(630,190+i*50,QString("Ranking ")+ranktext[i]+" :");
                    pain.drawText(650,210+i*50,QString(tempchar));
                }
            }
        }
        pain.setOpacity(abs((op+300)%1000-500)/1300.0+.8);
        pain.drawPixmap(0,500+prep/8,800,100,imgbottom);
        if(prep==0){
            pain.drawText(403,574,"Level = ");
            for(int i=0;i<level[cursel];i++){
                pain.setOpacity(double(abs((op-i*50)%1000-500))/1000+.5);
                pain.drawPixmap(500+i*30,560,15,15,imgstar[0]);
            }
            pain.setOpacity(double(op%200)/400+.5);
            pain.drawPixmap(60-prep+op/3%320,y,20,8,imghilight);
            pain.drawPixmap(380-prep-op/3%320,y+54,20,8,imghilight);
        }
        if(chngp>0){
            pain.drawPixmap(0,600-chngp,800,1200,imgchng);
            chngp-=20;
            if(chngp<=900){
                phase=1000;
                filename=filelist[cursel];
                qDebug()<<filename;
                loadalp();
                gameinit();
                this->setWindowTitle("ALPHA  -  "+sel_name[cursel]);
            }
        }
    }else if(phase==1000){
        if(bgmplaying==0 && mytime->elapsed()>=2000){
            music->play();
            bgmplaying=1;
        }
        pain.drawPixmap(0, 0, 800, 600, album);
        pain.drawPixmap(0, 0, 800, 600, game_back);
        for(int i=0;i<3;i++){
            for(int j=0;j<klen[i];j++){
                pain.drawPixmap(320+j*45+i*15,460+i*45,40,40,imgkey);
            }
        }
        while(500-(note[seqs].t*4*60000/bpm-music->currentTime()+offset)*80/bpm>600 && seqs<=seqe){
            if(note[seqs].live==1) combo=0;
            seqs++;
        }
        while(500-(note[seqe+1].t*4*60000/bpm-music->currentTime()+offset)*80/bpm>-50 && seqe<notesum) seqe++;
        int ttimes[128]={0};
        for(int i=seqe;i>=seqs;i--){
            int x=215.0-(double(note[i].sum)-.5)*30.0+note[i].pos*60;
            int y=500-(note[i].t*4*60000/bpm-music->currentTime()+offset)*80/bpm;
            if(note[i].live==1)pain.drawPixmap(x,y,50,50,imgnote[note[i].asc]);
            pain.setOpacity(1);
        }
        for(int i=seqs;i<=seqe;i++){
            int y=500-(note[i].t*4*60000/bpm-music->currentTime()+offset)*80/bpm;
            if(y<500&&y>0){
                ttimes[note[i].asc]++;
                if(ttimes[note[i].asc]>2) ttimes[note[i].asc]=2;
                double x1=320+kx[note[i].asc]*45+ky[note[i].asc]*15-double((y-500))*20/500;
                double y1=460+ky[note[i].asc]*45-double((y-500))*20/500;
                double a=40-double((500-y))*40/500;
                pain.drawPixmap(x1+1,y1,a,a,imgmark[ttimes[note[i].asc]]);
            }
        }
        for(int i=stars;i<=stare;i++){
            if(star[i]%1000>500||star[i]==0)continue;
            double x=235-(double(note[i].sum)-.5)*30.0+note[i].pos*60;
            double y=520;
            double x1,y1,r=(star[i]%1000)/15;
            for(int j=0;j<15;j++){
                x1=x+r*anscos[j];
                y1=y+r*anssin[j];
                pain.drawPixmap(int(x1),int(y1),10,10,imgstar[star[i]/1000]);
            }
            star[i]+=30;
            while ((star[stars]%1000>500||star[stars]==0)&&stars<=stare) stars++;
        }
        if(chngp>0){
            if(chngp<=30) tempmusic->stop();
            pain.drawPixmap(0,600-chngp,800,1200,imgchng);
            chngp-=30;
            if(chngp==900) seiseki();
        }
        char tempchar[20],tc2[10];
        itoa(int(score+.1),tempchar,10);
        pain.drawText(450,20,"  "+realname[cursel]);
        pain.setFont(QFont(QString::fromLocal8Bit("Courier New"),50,-1,false));
        pain.drawText(760-strlen(tempchar)*40,70,tempchar);
        if(combo>=10){
            itoa(combo,tc2,10);
            pain.setPen(QPen(QColor(min(combo,200),min(combo/2,100),0)));
            pain.drawText(220-strlen(tc2)*20,180,tc2);
        }
        if(music->currentTime()==music->totalTime() && music->currentTime()>100 && chngp==0){
            pre_seiseki();
        }
    }else if(phase==2000){
        pain.drawPixmap(0, 0, 800, 600, sei_back);
        pain.drawPixmap(50, 187, 300, 225, album);
        notec[3]=notesum-notec[0]-notec[1]-notec[2];
        music->stop();
        char tempchar[10],tc[5][10];
        for(int i=0;i<4;i++) itoa(notec[i],tc[i],10);
        itoa(int(score+.1),tempchar,10);
        pain.setFont(QFont(QString::fromLocal8Bit("Courier"),35,-1,false));
        pain.drawText(20,80,realname[cursel]);
        pain.setFont(QFont(QString::fromLocal8Bit("Courier New"),25,-1,false));
        for(int i=0;i<4;i++){
            pain.drawPixmap(420,200+i*50,150,50,imgr[i]);
        }
        pain.drawText(20,130,QString("Your Score = ")+tempchar);
        pain.drawText(600,237,tc[0]);
        pain.drawText(600,287,tc[1]);
        pain.drawText(600,337,tc[2]);
        pain.drawText(600,387,tc[3]);
        if(chngp>0){
            pain.drawPixmap(0,600-chngp,800,1200,imgchng);
            chngp-=30;
            if(chngp<=0){
                seip=2000;
            }
        }
        if(seip>0){
            seip-=20;
        }
        pain.setOpacity(1-double(chngp)/900.0);
        pain.setPen(QPen(QColor(100,100,100)));
        pain.drawText(10,490,"Performance");
        pain.setFont(QFont(QString::fromLocal8Bit("Courier"),100,-1,false));
        if(rank=="EXC") pain.setPen(QPen(QColor(250,200,0)));
        else if(rank=="SSS") pain.setPen(QPen(QColor(200,150,0)));
        else if(rank=="SS") pain.setPen(QPen(QColor(200,150,0)));
        else if(rank=="S") pain.setPen(QPen(QColor(200,150,0)));
        else if(rank=="A") pain.setPen(QPen(QColor(200,0,0)));
        else if(rank=="B") pain.setPen(QPen(QColor(0,200,0)));
        else if(rank=="C") pain.setPen(QPen(QColor(0,0,200)));
        else if(rank=="D") pain.setPen(QPen(QColor(0,0,0)));
        else pain.setPen(QPen(QColor(100,100,100)));
        pain.drawText(550-30*rank.length(),130,rank);
        if(chngp==0){
            for(int i=0;i<double(notesum)*(2000.0-double(seip))/2000.0-1;i++){
                pain.setPen( QPen( Qt::blue, 5, Qt::SolidLine));
                pain.drawLine(800.0/note[notesum-1].t*note[i].t,600.0-147.0*double(i+1)/double(notesum),
                              800.0/note[notesum-1].t*note[i+1].t,600.0-147.0*double(i+2)/double(notesum)
                              );
                if(note[i].curscore < note[i+1].curscore||i+1==notesum){
                    pain.setPen( QPen( Qt::green, 5, Qt::SolidLine));
                    pain.drawLine(800.0/note[notesum-1].t*note[i].t,600.0-147.0*note[i].curscore/1000000,
                                  800.0/note[notesum-1].t*note[i+1].t,600.0-147.0*note[i+1].curscore/1000000);
                }
            }
            for(int i=0;i<double(notesum)*(2000.0-double(seip))/2000.0-1;i++){
                if(note[i].curscore == note[i+1].curscore||i+1==notesum){
                    pain.setPen( QPen( Qt::red, 5, Qt::SolidLine));
                    pain.drawLine(800.0/note[notesum-1].t*note[i].t,600.0-147.0*note[i].curscore/1000000,
                                  800.0/note[notesum-1].t*note[i+1].t,600.0-147.0*note[i+1].curscore/1000000);
                }
            }
        }
    }
}

void Widget::keyPressEvent(QKeyEvent *event){
    if(phase==1){
        lassel=cursel;
        if(event->key()==Qt::Key_Up||event->key()==Qt::Key_Down){
            if(event->key()==Qt::Key_Up){
                cursel=(cursel-1+filesum)%filesum;
            }else if(event->key()==Qt::Key_Down){
                cursel=(cursel+1+filesum)%filesum;
            }
            selp=-129;
            mka->stop();
            mka->play();
            album.load(QString("./songs/")+file_img[cursel]);
        }else if(event->key()==Qt::Key_Return){
            mka->stop();
            tempmusic->stop();
            musicselect();
        }
    }
    if(phase==1000){
        if(event->key()==Qt::Key_Escape){
            music->stop();
            phase=0;
            getAlps();
        }
        for(int i=seqs;i<=seqe;i++){
            int gap=note[i].t*4*60000/bpm-music->currentTime()+offset;
            if(gap>100) break;
            if(note[i].live==0||gap<-50) continue;
            if(note[i].asc!=event->key()) continue;
            note[i].live=0;
            if(gap<50){
                score+=1000000.0/notesum;
                star[i]=20;
                notec[0]++;
            }else if(gap<75){
                score+=500000.0/notesum;
                star[i]=1020;
                notec[1]++;
            }else{
                score+=200000.0/notesum;
                star[i]=2020;
                notec[2]++;
            }
            note[i].curscore=score;
            combo++;
            if(i>stare) stare=i;
            if(i<stars) stars=i;
            break;
        }
    }else if(phase==2000){
        phase=0;
        getAlps();
    }
}

void Widget::getAlps(){
    filesum=0;
    ui->listWidget_file->hide();
    ui->listWidget_file->clear();
    QDirIterator it("./songs", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while(it.hasNext()){
        QString alpFile = it.next();
        QStringList list = alpFile.split('.');
        QString last = list.takeLast();
        if ("alp"  == last){
            filelist[filesum]=alpFile;
            QByteArray ba = (alpFile+".dat").toLatin1();
            const char *c_str2 = ba.data();
            ifstream fin(c_str2);
            for(int i=0;i<6;i++){
                fin>>filehiscore[filesum][i];
            }
            sel_name[filesum]=alpFile.right(alpFile.length()-8).left(alpFile.length()-12);
            ui->listWidget_file->addItem(alpFile.right(alpFile.length()-8).left(alpFile.length()-12));
            ba = (filelist[filesum]+".info").toLatin1();
            const char *c_str3 = ba.data();
            ifstream fin3(c_str3);
            qDebug()<<c_str3;
            fin3>>file_music[filesum];
            fin3>>file_img[filesum];
            fin3>>level[filesum];
            char tcc[100];
            fin3>>tcc;
            realname[filesum]=QString(tcc);
            filesum++;
        }
    }
    album.load(QString("./songs/")+file_img[cursel]);
    tempmusic = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource(QString("./songs/")+file_music[cursel]));
    tempmusic->play();
    selp=-130;
    chngp=0;
    seip=0;
    prep=400;
    op=0;
    this->setWindowTitle("ALPHA  -  Music Select");
    phase=1;
}

void Widget::musicselect(){
    chngp=1800;
    setFocus();
}

void Widget::pre_seiseki(){
    chngp=1800;
}

void Widget::seiseki(){
    phase=2000;
    if(score>999999)rank="EXC";
    else if(score>979999)rank="SSS";
    else if(score>949999)rank="SS";
    else if(score>899999)rank="S";
    else if(score>849999)rank="A";
    else if(score>799999)rank="B";
    else if(score>699999)rank="C";
    else if(score>499999)rank="D";
    else rank="E";
    note[notesum].curscore=score;
    note[notesum].t=linesum;
    for(int i=1;i<notesum;i++){
        if(note[i].curscore==-1) note[i].curscore=note[i-1].curscore;
    }
    notec[3]=notesum-notec[0]-notec[1]-notec[2];
    music->stop();
    char tempchar[10],tc[5][10];
    for(int i=0;i<4;i++) itoa(notec[i],tc[i],10);
    itoa(int(score+.1),tempchar,10);
    for(int i=0;i<6;i++){
        if(filehiscore[cursel][i]<score){
            qDebug()<<i;
            for(int j=5;j>i;j--){
                filehiscore[cursel][j]=filehiscore[cursel][j-1];
            }
            filehiscore[cursel][i]=int(score+.1);
            QByteArray ba = (filename+".dat").toLatin1();
            const char *c_str2 = ba.data();
            ofstream fout(c_str2);
            for(int j=0;j<6;j++){
                fout<<filehiscore[cursel][j]<<"\n";
            }
            break;
        }
    }
}

void Widget::themedone(){
    phase=0;
    getAlps();
}
