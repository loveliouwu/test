### while  
基本语法：  
while condition
do
    statements
done 

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