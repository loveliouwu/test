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

```
