#include "drawwidget.h"
#include "ui_drawwidget.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QMessageBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QLineEdit>
#include <QBrush>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include "cJSON.h"
DrawWidget::DrawWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawWidget)
{

    ui->setupUi(this);
    image=QImage(this->size().width()-320,this->size().height(),QImage::Format_RGB32);  //设定一张采用32位图（最常用的）的规模为900*600的画布
    setting_color=qRgb(255,255,255);    //选定背景色为白色
    image.fill(setting_color);  //将背景色填充在画布上
    tempImage = image;  //将临时画布设置成与主画布相同的状态
    drawing = false;    //默认未绘图
    status = false; //默认未加入会议
    shape = 0;  //默认涂鸦

    //绘画数据初始化
    ui->shape->setChecked(true);
    width = 0;
    heigh = 0;
    for(int i=0;i<3;i++)
    {
        pointPolygon[i].setX(0);
        pointPolygon[i].setY(0);
    }
    lineEdit.setParent(this);
    lineEdit.resize(70,20);
    lineEdit.setText(" ");
    lineEdit.setVisible(false);
   // ui->lineEdit->show()



}

DrawWidget::~DrawWidget()
{
    delete ui;
}
void DrawWidget::paintEvent(QPaintEvent *event) //重写窗口重绘事件
{
    /*
     *采用双缓冲绘图，以矩形为例，当绘制矩形时，
     * 鼠标左键未松开（移动），将图形绘制在临时画布（即预览）
     * 在鼠标松开后，将临时画布上的图形copy到主画布上，
     * 显示主画布
     *
     * */

    QPainter painter(this);
    if(drawing == true)
    {
        painter.drawImage(0,0,tempImage);   //鼠标按住但在拖动时在临时画布上画
    }
    else {
        painter.drawImage(0,0,image);//在image上绘画
    }
    //lineEdit.setVisible(false);

}

void DrawWidget::paint(QImage &theImage)        //绘图
{
    QPainter thePainter(&theImage);
    QPen pen;

    //设置画笔属性
    pen.setWidth(ui->penWidth->value());
    pen.setColor(penColor);
    thePainter.setPen(pen);

    //绘图
    switch (shape) {
    case 0:thePainter.drawLine(change,point);
            change = point;
            break;
    case 1:thePainter.drawLine(from,point);
            break;   //thePainter.drawLine(from,to);break;
    case 2:thePainter.drawRect(from.x(),from.y(),width,heigh);
            break;
    case 3:thePainter.eraseRect(point.x(),point.y(),ui->penWidth->value()+5,ui->penWidth->value()+5);
            break;
    case 4:thePainter.drawPolygon(pointPolygon,3);
            break;
    case 5:
                lineEdit.move(point.x(),point.y());
                lineEdit.setVisible(true);
                thePainter.drawText(change,lineEdit.text());
                lineEdit.clear();
                if(lineEdit.text()!="")
                {
                    lineEdit.setVisible(false);
                }
            break;
    default:break;
    }
    thePainter.end(); //结束绘图
    update();   //窗口重绘
}

void DrawWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        drawing = true;
        point = event->pos();
        from = event->pos();
        change = event->pos();
        width=0;heigh=0;
        pointPolygon[0]=point;
        pointPolygon[1].setX(point.x());
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event)
{
        point = event->pos();
        width = point.x()-from.x();
        heigh = point.y()-from.y();
        pointPolygon[1].setY(point.y());
        pointPolygon[2]=point;
        tempImage = image;

        if(shape == 0||shape==3)
        {
            if(status)  //如果处于会议状态将数据发送给服务器，反之直接在本地绘制
            {
                drawSend(true,point,from,to,change,pointPolygon,width,heigh);
            }
            else {
                paint(image);
            }

        }
        else {

            if(status)
            {
                drawSend(false,point,from,to,change,pointPolygon,width,heigh);
            }
            else {
                paint(tempImage);
            }

        }


}

void DrawWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        to = event->pos();
        point = event->pos();
        width = to.x()-from.x();
        heigh = to.y()-from.y();
        pointPolygon[2]=point;

        drawing = false;

        if(status)
        {
            drawSend(true,point,from,to,change,pointPolygon,width,heigh);
        }
        else {
            paint(image);
        }

    }


}

void DrawWidget::on_radioButton_clicked()
{
    shape = 1;
}

void DrawWidget::on_radioButton_2_clicked()
{
    shape = 2;
}



void DrawWidget::on_pushButton_clicked()
{
    QColorDialog color;//调出颜色选择器对话框
    penColor = color.getRgba(); //设置画笔颜色

}

void DrawWidget::on_shape_clicked()
{
    shape = 0;
}

void DrawWidget::on_radioButton_3_clicked()
{
    shape = 3;
}

void DrawWidget::on_pushButton_2_clicked()  //将画布内容保存
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save Image"),
        "",
        tr("*.bmp;; *.png;; *.jpg;; *.tif;; *.GIF")); //选择路径
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        if(! (image.save(filename) ) ) //保存图像
        {
            QMessageBox::information(this,
                tr("Failed to save the image"),
                tr("Failed to save the image!"));
            return;
        }
    }
}

void DrawWidget::on_radioButton_4_clicked()
{
    shape = 4;
}

void DrawWidget::on_pushButton_3_clicked()
{
    image.fill(setting_color);//将背景色填充在画布上
    update();
}

void DrawWidget::on_radioButton_5_clicked()
{
    shape = 5;
}

void DrawWidget::on_button_join_clicked()   //加入会议
{

    if(!status) //如果当前未连接到服务器，连接服务器，否则与服务器断开连接
    {
        if(ui->ip->text()=="")
        {
            QMessageBox::information(this,tr("error"),tr("User name error!"));
            return;
        }
        else {
            ip = ui->ip->text();
        }
        if(ui->name->text()=="")
        {
            QMessageBox::information(this,tr("error"),tr("User name error!"));
            return;
        }
        else {
           userName = ui->name->text();
        }
        if(ui->port->text()=="")
        {
            QMessageBox::information(this,tr("error"),tr("Port name error!"));
            return;
        }
        else {
            port = ui->port->text().toInt();
        }

        tcpSocket = new QTcpSocket(this);
        //检测链接信号
        connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
        //检测如果断开
        connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
        //检测如果有新可以读信号
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

        tcpSocket->connectToHost(QHostAddress(ip),port);

        status=true;
    }
    else
    {
        int length=0;
        QString msg=userName+tr(":Leave Chat Room");
        if((length=tcpSocket->write(msg.toLatin1(),msg.length()))!=msg. length())
        {
            return;
        }

        tcpSocket->disconnectFromHost();

        status=false;
    }
}

//链接后
void DrawWidget::slotConnected()
{
    ui->button_send->setEnabled(true);
    ui->button_join->setText(tr("离开"));

    int length=0;
    QString msg=userName+tr(":Enter Chat Room");
    if((length=tcpSocket->write(msg.toLatin1(),msg.length()))!=msg.length())
    {
        return;
    }
}



void DrawWidget::slotDisconnected()
{
    ui->button_send->setEnabled(false);
    ui->button_join->setText(tr("加入会议"));
}

int DrawWidget::readJson(cJSON * json)  //解析服务器发来的数据
{

    cJSON * node = NULL;
    node = cJSON_GetObjectItem(json,"point_x");
    if(node!=NULL)
    {
        point.setX(node->valueint);
    }
    else {
        qDebug()<<"json reaad error";
    }

    node = cJSON_GetObjectItem(json,"point_y");
    if(node!=NULL)
    {
        point.setY(node->valueint);
    }
    else {
        qDebug()<<"json reaad error";
    }

/*
    node = cJSON_GetObjectItem(json,"from_x");
    if(node!=NULL)
    {from.setX(node->valueint);
    }

    node = cJSON_GetObjectItem(json,"from_y");
    if(node!=NULL)
    {
        from.setY(node->valueint);
    }



    node = cJSON_GetObjectItem(json,"to_x");
    if(node!=NULL)
    to.setX(node->valueint);

    node = cJSON_GetObjectItem(json,"to_y");
    if(node!=NULL)
    to.setY(node->valueint);
    node = cJSON_GetObjectItem(json,"change_x");
    if(node!=NULL)
    change.setX(node->valueint);

    node = cJSON_GetObjectItem(json,"change_y");
    if(node!=NULL)
    change.setY(node->valueint);

    node = cJSON_GetObjectItem(json,"width");
    if(node!=NULL)width = node->valueint;

    node = cJSON_GetObjectItem(json,"heigh");
    if(node!=NULL)heigh = node->valueint;
    */

    if(node!=NULL)node = cJSON_GetObjectItem(json,"image");
    if(node!=NULL)
    {
        return node->valueint;
    }
    else {
        return 3;
    }


}

void DrawWidget::dataReceived() //接收消息
{

    while(tcpSocket->bytesAvailable()>0)
    {
        int typ = 0;//画图or消息
        QByteArray datagram;
        datagram.resize(tcpSocket->bytesAvailable());
        tcpSocket->read(datagram.data(),datagram.size());
        QString msg = datagram.data();
        cJSON *json = cJSON_Parse(datagram.data());
        typ = readJson(json);   //调用readjson解析服务器发来的数据，返回1在将图形绘制到主画布，0，绘制到临时画布，3，数据内容为信息，显示在列表中
        if(typ==1)
        {
            //charToInt(i);
            paint(image);
        }
        else {
            if(typ==0)
            {
               // charToInt(i);
                paint(tempImage);
            }
            else {
                ui->listWidget->addItem(msg.left(datagram.size()));
            }

        }

    }
}


void DrawWidget::on_button_send_clicked()
{
    if(ui->message->text()=="")
    {
        return ;
    }

    QString msg=userName+":"+ui->message->text();

    tcpSocket->write(msg.toLatin1(),msg.length());
    ui->message->clear();
}

void DrawWidget::drawSend(bool image,QPoint poi,QPoint fr,QPoint t,QPoint cha,QPoint pointP[3],int wid,int hei)
{
    //会议状态下将本地画笔数据打包为json格式发送给服务器

   // QString msg = QString("%1:%2:%3:").arg(image).arg(poi.x()).arg(poi.y());
  //  tcpSocket->write(msg.toLatin1(),msg.length());
 //  qDebug()<<"fasong";


    //创建一个空的文档对象{}
    QJsonObject json_data;
    json_data.insert("image",image);
    json_data.insert("point_x",point.x());
    json_data.insert("point_y",point.y());



    QByteArray temp = QString(QJsonDocument(json_data).toJson()).toUtf8();
    char* buf = temp.data();
    tcpSocket->write(buf);
    }




