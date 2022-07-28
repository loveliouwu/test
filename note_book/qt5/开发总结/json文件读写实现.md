### Json类介绍   

- QJsonDoucument   
 它封装了一个完整的 JSON 文档，并且可以从 UTF-8 编码的基于文本的表示以及 Qt 自己的二进制格式读取和写入该文档。
- QJsonArray  
JSON 数组是一个值列表。可以通过从数组中插入和删除 QJsonValue 来操作该列表。
- QJsonObject  
JSON 对象是键值对的列表，其中键是唯一的字符串，值由 QJsonValue 表示。
- QJsonValue   
该类封装了 JSON 支持的数据类型。   



### QJsonValue
在 Qt 中 QJsonValue 可以封装的基础数据类型有六种（和 Json 支持的类型一致），分别为：
>布尔类型：QJsonValue::Bool

>浮点类型（包括整形）： QJsonValue::Double

>字符串类型： QJsonValue::String

>Json 数组类型： QJsonValue::Array

>Json 对象类型：QJsonValue::Object

>空值类型： QJsonValue::Null
  
    


###  QJsonObject
QJsonObject 封装了 Json 中的对象，在里边可以存储多个键值对，为了方便操作，键值为字符串类型，值为 QJsonValue 类型。


### QJsonArray

QJsonArray 封装了 Json 中的数组，在里边可以存储多个元素，为了方便操作，所有的元素类统一为 QJsonValue 类型。


### QJsonDocument

它封装了一个完整的 JSON 文档，并且可以从 UTF-8 编码的基于文本的表示以及 Qt 自己的二进制格式读取和写入该文档。QJsonObject 和 QJsonArray 这两个对象中的数据是不能直接转换为字符串类型的，如果要进行数据传输或者数据的持久化，操作的都是字符串类型而不是 QJsonObject 或者 QJsonArray 类型，我们需要通过一个 Json 文档类进行二者之间的转换。



## 使用范例  

- 创建json多层结构  
```c++
    QJsonObject *root_obj = new QJsonObject();
    QJsonObject *row_obj = new QJsonObject();
    QJsonObject *child_obj = new QJsonObject();

    child_obj->insert("name","dike");
    child_obj->insert("age",12);
    row_obj->insert("child_1",child_obj);
    root_obj->insert("Row", row_obj);//三级json
```

- 获取json数据  
```c++
    QJsonObject *root_obj = new QJsonObject();

    root_obj->insert("string","123456");
    root_obj->insert("int",1080);
    root_obj->insert("double",0.01);

    QJsonArray array;//json数组结构,索引从1开始
    array.insert(0,"num 1");
    array.insert(1,"num 2");
    root_obj->insert("array",array);


//解析获取数据  
    QJsonObject *obj = root_obj->value("string").toObject();
    QString obj_data = obj->toString();
    array = root_obj->value("array").toArray();

    QString num_1 = array.at(0).toString();
    QString num_2 = array.at(1).toString();

//获取所有keys  
    QStringList keys = root_obj->keys();

//删除keys  
    root_obj->remove("int");
```


- 读取和解析 
```c++
    QFile file("test.json");
    QJsonDocument json(*root_obj);
    QByteArray ba = json.toJson();
    file.open(QIODecive::ReadWrite);
    file.write(ba.data);

    QByteArray read = file.readAll();
    file.close();
    json = QJsonDocument::fromJson(read);
    *root_obj = json.object();
```