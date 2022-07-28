# 使用QSettings进行参数设置 
```C
class ini {
    QSettings *ini;
    void ini_init(QString file_path);
    void ini_read(QString key,QString *value);
    void ini_write(QString key,QString &value);
}

void ini::ini_init(QString file_path)
{
    if(file_path.isEmpty())
    {
        return;
    }
    ini = new QSettings(file_path,QSettings::IniFormat);
}

void ini::ini_write(QString key,QString value)
{
    ini->setValue(key,value);
}

void ini::ini_read(QString key,QString *value)
{
    *value = ini->value(key).toString();
}

```