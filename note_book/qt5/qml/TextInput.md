## 文本编辑元素 

### 文本输入（TextInput） 
文本输入允许用户输入一行文本。这个元素支持使用正则表达式验证器来限制输入和输出掩码的模式设置  

```qml
TextInput{
    id:input1
    x:8;y:8
    width:100;height:30
    focus:true
    text:"text input 1"
    KeyNavigation.tab:input2  //预先设计一个元素id绑定切换焦点的按键
}

TextInput{
    id:input2
    x:8;y:8
    width:100;height:30
    focus:true
    text:"text input 1"
    
}
```

- `selectionColor`    设置选中时的颜色  

- `selectByMouse`   默认为false，设置是否支持鼠标滑动选择   

- `horzontalAlignment`  水平对齐方式 值： TextInput.AlignHCenter

- `verticalAlignment`   垂直对齐方式  

- `contentWidth` `contentHeight`  返回文本的宽、高

- `length`  返回编辑框内的字符个数  相应的信号函数`onLengthChanged`

- `maxinumiLength` 设置编辑框允许输入的最大字符串长度，超过就会被截断 

- `echoMode`  `TextInput.Password`、`TextInput.PasswordEchoOnEdit`、`TextInput.NoEcho` 注：如果设置为password，则属性`displayText`中的值为一串“*”  

- `inputMask` 字符串值，用来限制你的输入。 如果想输入类似“2020-02-20”，则设置为“0000-00-00”  

- 当用户按了`回车`或者`确认键`或者编辑框`失去焦点`时，会发出`accepted`和`editingFinished`信号，开发者可以实现`onAccepted`和`onEditingFinished`信号处理器来处理。     

- `clip` 属性是从Item那里继承下来的，让绘制限定在它自己的矩形范围内。

- `mouseSelectionMode` 设置选择模式，如单词选择`TextInput.SelectWords`  

- tab切换焦点， `focus: true; KeyNavigation.tab: txIn_ID` 

- 光标  `cursorVisible: false` 光标显隐。 `cursorPosition:200`光标位置  



