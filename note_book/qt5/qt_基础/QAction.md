- QAction 包含了图标、菜单文字、快捷键、状态栏文字、浮动帮助等信息

- 添加到菜单或者工具栏
>
    QMenu *file = menuBar()->addMenu(tr(&File));
    file->addAction(openAction);

    QToolBar *toolBar = addToolBar(tr("&File"));
    toolBar->addAction(openAction);
    //注意这里用`&File`意味着这将成为一个快捷键，此时File的F会有一个下划线
- 方法
>

    QAction *openAction;

    setShortcuts(QKeySequence::Open);//设置快捷键

    setStatusTip(tr("Open an existing file"));//设置状态栏

    connect(openAction, &QAction::triggered, this, &MainWindow::open);//信号槽


