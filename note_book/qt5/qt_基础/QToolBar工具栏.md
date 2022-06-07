- 属性
    - allowedAreas:函数制定停靠区域
    如：Qt::LeftToolBarArea
    - moveable:是否可以移动
    注：只有当前工具栏在QMainWindow中有效
    - floatable:是否可以拖拽浮动
    - Orientation:方向
    Qt::Horizontal
    - floating:
    - toolButtonStyle:按钮的风格
    默认是 Qt::ToolButtonIconOnly 
    - iconSize:icon的大小

- signal:
    acitonTriggered()
    allowedAreasChanged()//

- 常用方法
    setMovable()//设置工具栏区域是否可以移动
    setAllowedAreas();//设置工具栏可移动的范围
    setToolButtonStyle();//设置工具栏按钮的风格