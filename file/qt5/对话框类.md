- Qt中使用`QDialog`实现对话框，就像一个主窗口一样，我们通常会设计一个类继承`QDialog`,

QDialog *dialog = new QDialog(this);//指定新对话框的parent，指定了之后将不会在状态栏显示新的对话框选项。
dialog->setWindowTitle();//设置对话框标题


- 属性
    - modal:
        Qt::NonModal    //非模态对话框
        Qt::WindowModal //窗口级别的模态对话框
        Qt::ApplicationModal    //应用程序级别的模态对话框
        对应的方法：
            isModal()
            setModal(bool modal)
    - sizeGripEnable:  是否显示改变大小的箭头，默认是false

- 模态对话框和非模态对话框  
>
    模态对话框就是会阻塞同一应用程序中其他窗口的输入，模态对话框很常见，比如"打开文件"功能，当打开文件对话框出现时，我们是不能对除此之外的窗口部分进行操作的，与此相反的是非模态对话框，例如查找对话框，我们可以在显示着查找对话框的同时，继续对记事本的内容进行编辑

- qt里的两种级别的模态对话框
>
    应用程序级别的模态和窗口级别的模态，默认是应用程序级别的模态
    应用程序模态是指当该种模态的对话框出现时，用户必须首先对对话框进行交互，知道关闭对话框，然后才能访问程序中的其他窗口
    窗口级别的模态是指，该模态仅仅阻塞与对话框关联的窗口，但是依然允许用户与程序中其他窗口进行交互，窗口级别 的模态尤其适用于多窗口模式
    Qt使用`QDialog::exec()`实现应用程序级别的模态对话框
    使用`QDialog::open()`实现窗口级别的模态对话框
    使用`QDialog::show()`实现非模态对话框
    注：可以使用QWidget实现新的窗口以测试窗口级别的模态对话框

- 测试demo

```
 QWidget *new_widget = new QWidget(nullptr);

    //QDialog *dialog = new QDialog(this,nullptr);//指定了parent之后，该对话框将不会在状态栏显示新对话框
    //QDialog *dialog = new QDialog(nullptr);//没有指定parent，电脑下方的状态栏会多出一个对话框
    QDialog *dialog = new QDialog(new_widget);
    dialog->setWindowTitle(tr("new dialog"));
    dialog->setSizeGripEnabled(true);//设置右下角是否可以拖拽改变大小的箭头，默认为false
    new_widget->show();
    dialog->open();//窗口级别的模态对话框，仅仅阻塞与对话框关联的窗口，其他窗口还能继续操作
    //dialog->show();//非模态对话框，独立与其他对话框

    //dialog->exec();//应用程序级别的模态对话框，必须先操作当前对话框

```

