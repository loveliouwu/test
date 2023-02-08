### if结构的语法格式  
if结构用于在shell脚本中进行判定。  
`if TEST_COMMANDS; then CONSEQUENT-COMMANDS; fi`
或
```shell
if TEST-COMMANDS; then
	CONSEQUENT-COMMANDS
fi
```
或 
```shell
if TEST-COMMANDS
then
	
```

### 测试命令
-e：测试文件是否存在
-f：测试文件是否为常规文件
-d：测试文件是否为目录
-r：测试文件是否可读
-w：测试文件是否可写
-x：测试文件是否可执行
-s：测试文件是否非空
-z：测试字符串是否为空
-n：测试字符串是否非空
str1 = str2：测试两个字符串是否相等
str1 != str2：测试两个字符串是否不相等
num1 -eq num2：测试两个数字是否相等
num1 -ne num2：测试两个数字是否不相等
num1 -gt num2：测试第一个数字是否大于第二个数字
num1 -lt num2：测试第一个数字是否小于第二个数字


### 例子

```shell
read a
read b
if test $a > $b
then
	echo "a>b"
else
	echo "a<b"
fi


if test $"a" == "123"
then
	echo "a is ${a}
fi

#if...elif...else...fi

declare -i num
read num
if test ${num} -eq 100
then
	echo "num is eq 100"
elif test ${num} > 100
then
	echo "num is larger than 100"
else
	echo "num is less than 100"
fi

```

