### for循环  
```shell
for VAR in ITEM1 ITEM2 ... ITEMN
do
	command1
	command2
done

```

```shell
files=`ls`
for file in $files
do 
	echo "$file"
done

for linux in Debian Redhat Suse Fedora
do
	echo "the os is $linux"
done

# ubuntu 默认使用的dash 因此要正确执行需要 bash 
for (( i = 0; i < 10; i++ ))
do
	echo "i=$i"
done

echo "input num:"; read num 
for i in `seq $num`
do
	echo "$RANDOM"
done

```  


```shell 
echo 当前传入的参数个数  $#  
echo 分别为: 
let index=0
for arg in $*
do
        echo "$index:$arg"
        let index++
done

for i
do
        echo $1
        shift
done

set aa bb cc dd ee ff
echo $*
echo $@
echo $#


```
