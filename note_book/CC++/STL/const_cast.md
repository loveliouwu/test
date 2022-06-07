## 增加或去除const性，取消的是新变量，而不是之前的原始变量

const int g = 20;
int *h = const_cast<int*>(&g);//去掉const常量const属性

const int g = 20;
int &h = const_cast<int &>(g);//去掉const引用const属性

 const char *g = "hello";
char *h = const_cast<char *>(g);//去掉const指针const属性