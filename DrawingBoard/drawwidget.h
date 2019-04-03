#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPoint>
#include <QLineEdit>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include "cJSON.h"

namespace Ui {
class DrawWidget;
}

class DrawWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawWidget(QWidget *parent = nullptr);
    ~DrawWidget();
    void paint(QImage &theImage);
    void drawSend(bool image,QPoint poi,QPoint fr,QPoint t,QPoint cha,QPoint pointP[3],int wid,int hei);    //发送绘画数据到服务器
    int readJson(cJSON *);
private:
    Ui::DrawWidget *ui;

    //绘画变量
    QImage image;   //画布
    QImage tempImage;   //临时画布（双缓冲绘图时应用）
    QRgb setting_color;//背景色
    QColor penColor;    //画笔颜色
    bool drawing;   //绘图状态
    int shape;  //绘制类型
    bool status;    //服务器链接状态

    //绘图需要的鼠标数据
    QPoint point;
    QPoint from;
    QPoint to;
    QPoint change;
    QPoint pointPolygon[3];
    int width,heigh;
    QLineEdit lineEdit;

    //网络变量
    QString ip;
    quint16 port;
    QString userName;
    QTcpSocket *tcpSocket;



protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private slots:
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_pushButton_clicked();
    void on_shape_clicked();
    void on_radioButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_radioButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_radioButton_5_clicked();
    void on_button_join_clicked();
    void on_button_send_clicked();

public slots:
    void slotConnected();
    void slotDisconnected();
    void dataReceived();





};

#endif // DRAWWIDGET_H
