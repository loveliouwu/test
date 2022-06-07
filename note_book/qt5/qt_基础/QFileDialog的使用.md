####QFileDialog文件对话框的使用：
- 最简单的方法是调用静态函数来实现
```
QString file = QFileDialog::getOpenFileName(
    "/home/foxman",
    "Images(*.png *.xpm *.jpg)",
    this
)
//建立一个取文件对话框，选择后OK将文件路径返回给file
```
- 一般的文件对话框的使用
```
QFileDialog *fd = new QFileDialog(this,"file dlg",TRUE);
if(fd->exec() == QFileDialog::Accepted)
{
    QString file = fd->selectedFile();
    qDebug()<<file;
}
```

- 常用API
1.设定对话框返回什么
```
fd->setMode(QFileDialog::ExitingFile);
AnyFile:一般用于save as对话框
ExitingFile存在的一个文件
ExitingFiles存在的0个或多个文件
Directory:返回目录
DirectoryOnly:返回目录（选中文件的时候只选中目录）
```
返回选择中的一个文件（夹）名字
QString s = fd->selectedFile();
返回选择的多个文件
QStringList slist = fd->selectedFiles();


- 既选则文件又选择文件夹的实现  
```c++
QFileDialog* _f_dlg = new QFileDialog(this);
_f_dlg->setFileMode(QFileDialog::Directory);
_f_dlg->setOption(QFileDialog::DontUseNativeDialog, true);

QListView *l = _f_dlg->findChild<QListView*>("listView");
if (l) {
    l->setSelectionMode(QAbstractItemView::MultiSelection);
}
QTreeView *t = _f_dlg->findChild<QTreeView*>();
if (t) {
    t->setSelectionMode(QAbstractItemView::MultiSelection);
}
_f_dlg->setFilter(QDir::AllEntries);
int nMode = _f_dlg->exec();
QStringList _fnames = _f_dlg->selectedFiles();
```
