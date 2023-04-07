#!/bin/bash  

root_path=$(cd $(dirname $0);pwd)
BackUpPath="${root_path}/test"

CHSM_INTERFACE_NAME="eno2"
CHSM_INTERFACE_NAME_TEST="eno1"  #测试时使用的测试网卡，真实网卡为eno2，方便远程连接测试


function  backup(){
    if [ $# -eq 0 ]; then 
        BackUpName=$(date "+%Y-%m-%d_%H-%M-%S")
    else
        BackUpName=$1
    fi
    # echo "BackUpName = $BackUpName"
    BackUpDir=${BackUpPath}/${BackUpName}
    if [ -d ${BackUpDir} ]; then
        rm -rf ${BackUpDir}
        mkdir ${BackUpDir}
    else
        mkdir ${BackUpDir}
    fi
    # echo  "BackUpDir: ${BackUpDir}"
    # echo  "BackUpPath: ${BackUpPath}"
    # echo  "BackUpName: ${BackUpName}"

# step 2: copy file
# TODO
    touch ${BackUpDir}/test.c  #FIXME #LYP

# step 3: tar file
    before_path=`pwd`
    cd ${BackUpPath}
    tar -czvf "${BackUpName}.tar.gz"  "${BackUpName}"   #��Ҫ�þ���·�����д��
    cd -

    echo "${BackUpName}.tar.gz"
    return $?
}

usage(){
	cat <<EOF
Usage: ${0} [ Options ] 
--help		                :show help info

--getversion                :get chsm's version

--backup [file_name/NULL]   :backup chsm
--clean                     :clean all packeg

--time | -t                 :close the default ntp service and set system time
                            "--time xxxx-xx-xx xx:xx:xx"

--getip                     :get chsm's ip
--getnetmask                :get chsm's netmask
--getgw                     :get chsm's gateway

--setip                     :set chsm's ip
                            "--setip 192.168.10.10"

--vsmlist                   :list all vsm's uuid and name
--vsmstate                  :list vsm's info
                            "--vsmstate vsm_name"

--clone                     :clone a vsm, need shutdown before
                            "--clone -h" help
                            "--clone -s source_name -n new_name"
EOF
	exit 0
}


while true; 
do
	case $1 in
	--help | -h)
		usage;
		shift
		;;
    --getversion)
        echo "0.0.1"
        shift
        ;;
	--backup | -b)
        echo "backup:"
        if [[ $2 =~ ^- ]] ; then
            backup ;
        else
            backup $2;
            shift
        fi
        shift
		;;
	--time | -t)
        echo "set time:"
        if [ -n "$2" ] ; then
            sudo timedatectl set-ntp false  #关闭系统默认NTP服务
            sudo date -s "$2"
        else
            echo "--time xxxx-xx-xx xx:xx:xx"
        fi
        shift
		;;
	--getip)
        echo "get ip:"
	    ifconfig ${CHSM_INTERFACE_NAME} | grep inet | awk -F' ' 'NR==1  {print $2}'
        shift
		;;
	--getnetmask)
        echo "get netmask:"
	    ifconfig ${CHSM_INTERFACE_NAME} | grep inet | awk -F' ' 'NR==1  {print $4}'
        shift
		;;
	--getgw)
        echo "get gateway:"
        cat /etc/resolv.conf | grep nameserver | awk '{print $2}'
        shift
		;;
	--clean | -c)
        echo "files clean:"
		cd  ${BackUpPath}
        # ls . | grep -E "202.-..-.._..:..:..*" 
		# rm -rf `ls . | grep -E "202.-..-.._..-..-..*"`
        ls .
        rm -rf `ls`
        cd ${root_path}
		shift
		;;
    
	--setip | -ip)
		shift
        while getopts "n:i:m:g:" opt; do
            echo "opt = ${opt}"
            case $opt in 
            n)
                NAME=${OPTARG}
                echo "name:${NAME}"
                ;;
            i)
                IP=${OPTARG}
                echo "ip:${IP}"
                ;;
            m)
                NETMASK=${OPTARG}
                echo "netmask:${NETMASK}"
                ;;
            g)
                GATE=${OPTARG}
                echo "gateway:${GATE}"
                ;;
            :)
                ;;
            \?)
                echo "invalid option -${OPTARG}"
                ;;
            esac
        done
        if [ -z "${IP}" ] && [ -z "${NETMASK}"] && [ -z "${GATE}" ] && [ -z "${NETMASK}" ]; then
            echo "error!  -n: name  -i: ip_address  -m: netmask  -g: gateway"
            exit 1
        fi

        net_uuid=`nmcli connection show | grep ${NAME} | awk  -F' '  '{print $2}'`
        echo "uuid:${net_uuid}"
        sudo ifconfig ${NAME} netmask ${NETMASK}
        nmcli connection modify uuid ${net_uuid} ipv4.addresses ${IP} ipv4.gateway ${GATE}
        echo "up ${net_uuid}"
        nmcli connection up uuid ${net_uuid}
        ifconfig | grep -w inet | awk -F' ' 'NR==1  {print $2}'
        ;;
    --vsmlist)
        sudo virsh list --all --uuid --name   #显示所有虚拟机的UUID和名字
        shift
        ;;
    --vsmstate)
        sudo virsh dominfo  --domain $2 | grep State | awk -F' ' '{$1=""; print}'
        shift
        ;;
	--clone) 
		shift
		##TODO
        while getopts ":n:s:h" opt; do
            case $opt in 
            s)
                source_name=${OPTARG}
                ;;
            n)
                new_name=${OPTARG}
                ;;
            h)
                echo "help: -s: source_name  -n: new_name"
                exit 0;
                ;;
            :)
                echo ":"
                ;;
            \?)
                echo "invalid option -${OPTARG}"
                ;;
            esac
        done
        if [ -z "${source_name}" ] && [ -z "${new_name}"]; then
            echo "error!   -s: source_name  -n: new_name"
        else
            #克隆虚拟机
            echo "source_name=${source_name} new_name=${new_name}"
            sudo virt-clone --original ${source_name} --name ${new_name} --auto-clone 
            #完成后获取虚拟机的uuid
            domain_uuid=`sudo virsh domuuid --domain ${new_name}`   
            echo "uuid: ${domain_uuid}"
            #设置为自动启动
            sudo virsh autostart --domain ${new_name}   
            #获取122网卡的mac地址  
            domain_default_mac=`sudo virsh domiflist  --domain ubu3 | grep default | awk -F' ' '{print $5}'`
            echo "default network mac address: ${domain_default_mac}"
        fi
		;;
	--test)
        shift
        while getopts ":n:s:" opt; do
            case $opt in 
            s)
                echo "s : ${OPTARG}"
                ;;
            n)
                echo "n : ${OPTARG}"
                ;;
            :)
                echo ":"
                ;;
            \?)
                echo "invalid option -${OPTARG}"
                ;;
            esac
        done
        shift
        ;;
	*)
		exit 1
		;;
	esac
done
}





