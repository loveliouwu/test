### 定义   
`const char *c_str();`   
c_str函数返回一个指向正规C字符串的指针常量，内容与本string串相同。   
这是为了与C语言兼容，在C语言中没有string类型，故必须通过string类对象成员函数`c_str`把string转换成c中的字符串样式。