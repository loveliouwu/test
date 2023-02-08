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

