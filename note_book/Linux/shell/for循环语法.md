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
