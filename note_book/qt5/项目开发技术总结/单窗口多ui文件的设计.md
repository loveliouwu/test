## 需求
要实现一个窗口包含多个ui文件的设计，这样主窗窗口可以包含不同的副窗口实现类似于tabWidget但又不同的形式。
这种设计适合在主窗口功能简单，子窗口功能相对复杂的情况。 

## 实现方式  
- 创建  
首先创建一个父窗口，创建Qt类下的ui/.h类，基本类型可以是MainWidget也可以是widget, 标记为main_wid,
main_wid的ui文件中添加一个layout，水平或者垂直或者宫格，然后创建子窗口wid1、wid2，和父窗口一样的ui/.h类，
将头文件包含在main_wid中 如wid1.h  wid2.h。
- 切换 
通过主窗口的ui->mylayout->addWidget(wid1);
注意addwidget之前如果需要关闭上一个wid,则需要调用ui->removeWidget(wid2);
并且wid2要隐藏，wid2->hide(); 隐藏之后下次要再显示需要 wid2->show();

要重新设置Layout的大小或者窗口的大小可以使用以下方法：   
ui->main_layout->setGeometry(QRect(0,0,400,300));
ui->main_layout->setGeometry(wid1->geometry().adjust(0,0,100,100));  //调整大小，x,y,width,height
this->setGeometry(QRect(this->geometry().x(),this->geometry().y(),wid1->width()+100,wid1->height()+300));
 