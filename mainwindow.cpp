#include<QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "box.h"
#include "utils.h"
#include <cstdlib>
#include <ctime>
#define SIZE 10
#define BOMBLEVEL 5
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    boxSize=37;
    isGameOngoing=true;
    srand(time(NULL));
    ui->setupUi(this);
    //=======================
    setupGrid();
    setupMenu();
}
void MainWindow::setupMenu(){
    connect(ui->actionNew_Game, SIGNAL(triggered()),this, SIGNAL(newGame()));
    connect(ui->actionExit_2, SIGNAL(triggered()),qApp,SLOT(quit()));
}
void MainWindow::setupGrid(){
    //mapper definitions
    QSignalMapper *mouseLeftMapper;
    QSignalMapper *mouseRightMapper;
    mouseLeftMapper = new QSignalMapper(this);
    mouseRightMapper = new QSignalMapper(this);
    connect(mouseLeftMapper, SIGNAL(mapped(int)),this, SLOT(boxClicked(int)));
    connect(mouseRightMapper, SIGNAL(mapped(int)),this, SLOT(boxRightClicked(int)));

    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            bool bomb=(utils::randbool(BOMBLEVEL));
            Box *b=new Box("Yeah sure",bomb);
            grid[i][j]=b;
            b->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
            b->setFocusPolicy(Qt::NoFocus);
            b->setMinimumSize(boxSize,boxSize);
            b->setMaximumSize(boxSize,boxSize);
            QFont font = b->font();
            font.setPointSize(18);
            b->setFont(font);
            ui->mineField->addWidget(b,i,j);
            //event binding
            mouseLeftMapper->setMapping(b,(i*1000)+j);//function member is setmapping (sender,id)
            mouseRightMapper->setMapping(b,(i*1000)+j);
            connect(b,SIGNAL(clicked()),mouseLeftMapper,SLOT(map()));
            connect(b,SIGNAL(rightClicked()),mouseRightMapper,SLOT(map()));
        }
    }
}


int MainWindow::findBombCount(int x,int y){
    int count=0;
    for(int i=-1;i<=1;i++){
        if(x+i<0 || x+i>SIZE-1)continue;
        for(int j=-1;j<=1;j++){
            if(y+j<0 || y+j>SIZE-1 || (i==0 && j==0))continue;
            printf("x=%d y=%d\n",x+i,y+j);
            Box* box=grid[x+i][y+j];
            if(box->isBomb())count++; 
        }
    }
    return count;
}
bool MainWindow::winCheck(){
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            Box *b=grid[i][j];
            if(b->isBomb() && !b->isFlagged())return false;       
            if(!b->isBomb() && !b->isClicked())return false;
            if(!b->isBomb() && b->isFlagged())return false;       
        }
    }
    printf("You won\n");
    isGameOngoing=false;
    showMsg("Congratulations, You Won!");
    return true;
}
void MainWindow::showMsg(QString s){

    QMessageBox msgbox;
    msgbox.setWindowTitle("Minesweeper");
    msgbox.setText(s);
    msgbox.exec();
}
void MainWindow::explodeAll(){
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            Box *b=grid[i][j];
            if(b->isBomb()){
                b->boxClicked();
            }

        }
    }
    isGameOngoing=false;
    showMsg("Oops! You lose.");
}
void MainWindow::boxClicked(int id){
    if(!isGameOngoing)return;
    int xy[2];
    xy[0]=id/1000;
    xy[1]=id%1000;
    //printf("clicked - (%d,%d)\n",
    //xy[0],xy[1]);
    Box* b=grid[xy[0]][xy[1]];
    if(b->isBomb()){
        explodeAll();
        b->boxClicked(); 
        return;
    }
    b->boxClicked(); 
    recursiveClean(xy[0],xy[1]);
    winCheck();
}
void MainWindow::boxRightClicked(int id){
    if(!isGameOngoing)return;
    int xy[2];
    xy[0]=id/1000;
    xy[1]=id%1000;
    printf("R-clicked - (%d,%d)\n",
            xy[0],xy[1]);
    Box* b=grid[xy[0]][xy[1]];
    b->boxRightClicked(); 
    winCheck();
}
void MainWindow::recursiveClean(int x,int y){
    grid[x][y]->boxClicked();
    grid[x][y]->setBombCount(findBombCount(x,y));
    if(findBombCount(x,y)==0){
        for(int i=-1;i<=1;i++){
            if(x+i<0 || x+i>SIZE-1)continue;
            for(int j=-1;j<=1;j++){
                if(y+j<0 || y+j>SIZE-1 || (i==0 && j==0))continue;
                if(!grid[x+i][y+j]->isClicked()){
                    recursiveClean(x+i,y+j);
                }
            }
        }

    }else{
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

