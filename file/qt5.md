#QT5
-- -
####QMessageBox
```
#include <QMessageBox>
#第一个参数为parent，说明他的父组件，第二个参数为对话框标题，第三个对话框显示的内容，第四个参数为选项，第五个为默认的选项
#这是无序实例的几个static接口,static函数都返回一个StandardButton,如果要获取用户的选择，
QMessageBos::StandardButton rb = QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
if(rb == QMessagtBox::Yes)
{
	qDebug()<<"yes";
}

QMessageBox qmess(QMessageBox::NoIcon,"show icon","gou zao messagebox",QMessageBox::Yes|QMessageBox::No,NULL);
QString qtext = qmess.buttonText(QMessageBox::No);
qDebug()<<qtext;
qmess.setButtonText(QMessageBox::No,"nooo!");
qmess.exec();
    
QMessageBox::about(NULL,"about","send about message");
QMessageBox::question(NULL,"question","send a question messagebox",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
QMessageBox::warning(NULL,"warning","send a warning messagebox",QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
#如果需要自己实现messagebox
QMessageBox message(QMessageBox::NoIcon, "Title", "Content with icon."); 
message.setIconPixmap(QPixmap("icon.png")); 
message.exec();
#用exec而不用show
```
-- -
####QDebug
```
#include <QDebug>

qDebug()<<"this is a qdebug mess";
QList<QString> list;
list<<"abc"<<"123"<<"hhh";
qDebug()<<list[0]<<list;
qDebug("print a debug string only");
```


-- -
####QT容器类

-- -
####QString类
>append() 在字符串的后面添加字符串，prepend() 在字符串的前面添加字符串
oUpper() 将字符串内的字母全部转换为大写形式，toLower() 将字母全部转换为小写形式
count()、size() 和 length() 都返回字符串的字符个数，这 3 个函数是相同的，但是要注意，字符串中如果有汉字，一个汉字算一个字符。
trimmed() 去掉字符串首尾的空格，simplified() 不仅去掉首尾的空格，中间连续的空格也用一个空格替换。
indexOf () 和 lastIndexOf ()其功能是在自身字符串内查找参数字符串 str 出现的位置，参数 from 是幵始查找的位置，Qt::CaseSensitivity cs 参数指定是否区分大小写。lastIndexOf() 函数则是查找某个字符串最后出现的位置。
isNull() 和 isEmpty()
两个函数都判读字符串是否为空，但是稍有差别。如果一个空字符串，只有“\0”，isNull() 返回 false，而 isEmpty() 返回 true；只有未赋值的字符串，isNull() 才返回 true。
contains()
判断字符串内是否包含某个字符串，可指定是否区分大小写。
endsWith() 和 startsWith()
startsWith() 判断是否以某个字符串幵头，endsWith() 判断是否以某个字符串结束。
left() 和 right()
left 表示从字符串中取左边多少个字符，right 表示从字符串中取右边多少个字符。注意，一个汉字被当作一个字符。

```
QString str1="卖"，str2="拐";
QString str3=str1;
str1.append (str2) ; //str1="卖拐"
str3.prepend (str2) ; //str3="拐卖"
toUpper() 和 toLower()
QString str1="NI 好"
N=str1.count()  //N=3
N=str1.size() //N=3
N=str1.length() //N=3
QString str1="Hello, World", str2;
str2=str1.toUpper(); //str1="HELLO,WORLD"
str2=str1.toLower(); //str1="hello, world"

QString str1=" Are you OK? ", str2;
str2=str1.trimmed () ; //str1="Are you OK? "
str2=str1.simplified(); //str1="Are you OK?"

QString str1="G:\Qt5Book\QT5.9Study\qw.cpp";
N=str1.indexOf("5.9");  // N=13
N=str1.lastIndexOf("\\"); //N=21

QString str1, str2="";
N=str1.isNull () ; // N=true 未赋值字符串变量
N=str2.isNull () ; // N=false 只有"\0"的字符串，也不是 Null
N=str1.isEmpty(); // N=true
N=str2.isEmpty(); // N=true

#字符串类型转换
QString str;
str = "123";
int num = str.toInt();

int num1 = 100;
str = QString::number(num1);
```
-- - 

####QSpinBox
>QSpinBox 用于整数的显示和输入，一般显示十进制数，也可以显示二进制、十六进制的数，而且可以在显示框中增加前缀或后缀。
QDoubleSpinBox 用于浮点数的显示和输入，可以设置显示小数位数，也可以设置显示的前缀和后缀。


