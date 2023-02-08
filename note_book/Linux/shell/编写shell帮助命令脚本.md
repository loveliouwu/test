## 一般在shell中帮助信息定义为一个函数  如：  
```shell
#!/usr/bin/bash
usage(){
	cat <<EOF
Usage: help.sh [ Options ] 
--help		show help info
--exit		exit shell
--info		test func
EOF
	exit 0
}

while true; 
do
	case $1 in
	--help)
		usage;
		shift
		;;
	--exit)
		echo "exit !"
		exit 0
		;;
	--me)
		echo "$0"
		shift
		;;
	--show)
		echo "$1"
		shift
		;;
	*)
		exit 1
		;;
	esac
done
}

>> sh ./help.sh --help --me --show  

```