QMessigeBox::information()
QMessigeBox::question()
QMessigeBox::critical()//显示严重错误对话框，这个对话框将显示一个红色的错误符号，我们可以通过buttons参数知名其显示的按钮



- 文件对话框
<QFileDialog>
```
QFileDialog::getOpenFileName()
QFileDialog::getSaveFileName()


//函数原型
QString getOpenFileName(QWidget * parent = 0,
                        const QString & caption = QString(),
                        const QString & dir = QString(),
                        const QString & filter = QString(),
                        QString * selectedFilter = 0,
                        Options options = 0)
/*
parent：父窗口
caption:对话框标题
dir:对话框打开时默认目录，"."当前目录  "/"当前盘符根目录
filter:过滤器，"image Files(*.jpg*.png)",多个过滤器用`;;`分隔
selectedFilter:默认选择的过滤器
options:对话框的一些参数设定，比如只显示文件夹等等 enum QFileDialog::Option
*/

```