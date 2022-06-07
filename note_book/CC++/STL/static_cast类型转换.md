## 可以用于基本的数据类型转换  
```C++
int a = 10;
char b = static_cast<char>(a);
int c = static_cast<int>(b);
```

## 可以转换具有继承关系的指针、引用
```C++
int a = 10;
char *b = static_cast<char *>(&a);//错误！！ 不能转换不同类型的指针
int c = static_cast<int *>(&a);

```

## 继承关系的类，类型转换  
父类不能转换成子类的类型，即小不能转大，大能装小  
```C++
class ani{
    string name;
};
class cat : public ani{

};

void test(){
    ani ani1;
    cat cat1 = static_cast<cat>(ani1);//父类对象ani1转换成子类对象，编译器可能爆错，不能这么用 

    cat cat2;
    ani ani2 = static_cast<ani>(cat2);//子类对象cat2转换成父类对象ani2，允许使用
    ani2.name = "cat2";
}
```
