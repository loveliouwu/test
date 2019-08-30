#shell学习笔记
- - -
###***1.数值运算***
- 对某个数加１
```
i=0
((i++))
echo $i

let i++
echo $i

expr $i + 1
```
- 从１加到某个数
```
i=0;
while [ $i -lt 100]
do
	((i++))
done
echo $i
```

- 求模、求幂
```
let i=5%2
echo "5%2" | bc

let i=%**2
((i=5**2))
echo "5^2" | bc
```

- 进制转换
```
echo "obase=10;ibase=8;11" | bc -l
9
echo $((8#11)) #把8进制的11转换成10进制
9

#要把某些字符串以特定的进制表示，用od命令
echo -n "$IFS" | od -c
echo -n "$IFS" | od -b
```

- 浮点运算
```
echo "scale=3;1/13" | bc
#１除13,保留3位有效数字
```

- 从文件中读取并计算
```
[ $# -lt 1 ] && echo "please input the income file" && exit -1
[ ! -f $1 ] && echo "$1 is not a file" && exit -1
income=$1
awk `{
	printf("%d %0.2f\n",$1,$3/$2);
}` $income | sort -k 2 -n -r
```
