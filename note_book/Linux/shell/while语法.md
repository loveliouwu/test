### while  
基本语法：  
while condition
do
    statements
done 



### 使用break跳出循环   
break是可以带参数的，比如`break 2`表示跳出两层循环  
```shell
while true
do 
	while i < 10
	do 
		echo $i 
		break 2  跳出两层循环  
	done
dnoe

```


```shell
!/usr/bin/bash
i=1

func1(){
while [ $i -le 5 ]
do
	echo "func $i"

	let i++
done
}

func2(){
	while (( i < 5))
	do
		echo "func2 $i"
		let i++
	done
}

func3(){
	while read arg
	do
		echo "$arg"
		if test "$arg" == "EOF"
		then
			break	
		else
			echo "input sth , EOF to end"
		fi
	done
}

while true;
do
	case $1 in 
		--func1)
			func1
			shift
			;;
		--func2)
			func2
			shift
			;;
		--func3)
			func3
			shift
			;;
		*)
			exit 1
			;;
	esac
done

```