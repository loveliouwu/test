### JNI的注意事项  
1、调试困难  
2、仅有应用程序与签名的applet可以调用JNI  
3、依赖于JNI的应用失去了Java的平台移植性(一种解决办法是为每个平台编写专门的JNI代码，然后在JAVA中根据操作系统加载正确的JNI代码)  
4、JNI框架没有对non-JVM内存提供自动垃圾回收机制，Native代码(如汇编语言)分配的内存和资源


### 基本步骤    
1、编写Java测试类  
2、利用Java测试类生成C头文件  
3、利用C头文件编写C代码   
4、生成C库   
5、Java调用测试  

### 命名规则   
将Java的native方法转换成C函声明的规则：  
`Java_{packet_and_classname}_{function_name}(JNI arguments)`  
其中需要特别说明的有两个参数：
1、JNIEnv *:这是一个指向JNI运行环境的指针，JAVA通过这个指针访问JNI函数。  
2、jobject:这里代指java中的this对象    
同时为了保证函数名按照C语言的签名协议规则去编译，在头文件中一般会有 
```C
#ifdef __cplusplus 
extern "C"{
#endif  
...
#ifdef __cplusplus 
}
#endif
```

### 编译库  
`gcc -c *.cpp -o3 -Wall -Wextra -fPIC -shared -o jnitest.so ` 

### 链接库   
`java -Djava.library.path=. jnitest`  

### 数据类型   
JAVA原始类型：`jint`,`jbyte`,`jshort`,`jlong`,`jfloat`,`jdouble`,`jchar`,`jboolean`。这些分别对应着java的`int`,`byte`,`short`,`long`,`float`,`double`,`char`,`boolean` 

### 对数据类型参数的使用   
1、对于JNI类型参数，需要将这些数据转换或者拷贝成本地数据类型，比如将`jstring`转成`char *`，将`jintArray`转成C的`int[]`，但有些如jint，jdouble之类的不用进行转换可以直接使用。  
2、进行数据操作，以本地的方式   
3、创建一个JNI的返回类型，然后将结果数据拷贝到这个JNI数据中。  
5、return JNI类型数据。  

### 传递字符串   
```JAVA
public class testjnistring{
    static {
        System.loadLibrary("myjni");
    }
    private native String sayHello(String msg);
}
```  
```C
JNIEXPORT jstring JNICALL Java_TestJNIString_sayHello(JNIEnv *, jobject, jstring);  
```
该C函数接收一个jstring类型的数据，并返回一个jstring类型的数据。  
传递一个字符串比传递基本类型要复杂的多。因为要从Java中的对象转换为C中的char数组。   
JNI环境指针`JNIEnv *`已经为我们定义了非常丰富的接口函数用来处理数据的转换。   
1、调用`const char *GetStringUTFChars(JNIEnv *, jstring, jboolean *)`来将JNI的jstring转换成C的`char *`；  
2、调用`jstring NewStringUTF(JNIEnv *, char *);`来将C的char *转换成JNI的jstring。   
因此我们的C程序基本过程如下：  
```C
JNIEXPORT jstring JNICALL Java_TestJNIString_sayHello(JNIEnv *env, jobjcet thisobj, jstring inJNIStr)
{
    const char *inCStr = (*env)->GetStringUTFChars(env, inJINStr, NULL);  
    if(NULL == inCSt) return NULL;

    (*env)->ReleaseStringUTFChars(env, inJNIStr, inCStr);//release resource.

    char outCStr[128];
    scanf("%s", outCStr);

    return (*env)->NewStringUTF(env, outCStr);
}
```  
**注意**：当不再需要`GetStringUTFChars`返回的字符串的时候，一定记得调用`ReleaseStringUTFChars`函数来将内存资源释放，否则会内存邪路。并且上层java中的GC也不能进行！  

### 传递数组  
`JNIEXPORT jdoubleArray JNICALL Java_TestJNIPrimitiveArray_arverage(JNIEnv *,jobject)`  
在Java中，array是指一种类型，类似于类，一共有9中Java的array，8个基本类型的array和一个object的array：`jintArray`,`jbyteArray`,`jlongArray`,`jfloatArray`,`jdoubleArray`,`jcharArray`,`jbooleanArray`,并且也有面向object的`jobjectArray`