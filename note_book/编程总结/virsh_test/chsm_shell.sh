#!/bin/bash  

root_path=$(cd $(dirname $0);pwd)
BackUpPath="${root_path}/test"

CHSM_INTERFACE_NAME="eno2"
#test interface
CHSM_INTERFACE_NAME_TEST="eno1"  

VSM_INTERFACE_TYPE="direct"

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
    tar -czvf "${BackUpName}.tar.gz"  "${BackUpName}" 
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
                                "--setip -n eno1 -i 192.168.10.10 -m 255.255.255.0 -g 192.168.10.1"

    --getvsmnetflow				:get vsm's interface netflow
                                "--getvsmnetflow ubu1"
                                
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
            sudo timedatectl set-ntp false  #close the system's ntp service
            sudo date -s "$2"
        else
            echo "--time xxxx-xx-xx xx:xx:xx"
        fi
        shift
		;;
	--getip)
        echo "get ip:"
	    ifconfig ${CHSM_INTERFACE_NAME} | grep -w inet | awk -F' ' 'NR==1  {print $2}'
        shift
		;;
	--getnetmask)
        echo "get netmask:"
	    ifconfig ${CHSM_INTERFACE_NAME} | grep -w inet | awk -F' ' 'NR==1  {print $4}'
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
        while getopts "n:i:g:m:" opt; do
            case $opt in 
            n)
                NAME=${OPTARG}
                echo "name:     ${NAME}"
                ;;
            i)
                IP=${OPTARG}
                echo "ip:       ${IP}"
                ;;
            m)
                MASK=${OPTARG}
                echo "mask:     ${MASK}"
                ;;
            g)
                GW=${OPTARG}
                echo "gateway:  ${GW}"
                ;;
            :)
                ;;
            \?)
                echo "invalid option -${OPTARG}"
                ;;
            esac
        done
        if [ -z "${IP}" ] && [ -z "${GW}" ] && [ -z "${NAME}" ] && [ -z "${MASK}" ]; then
            echo "error!  -n: name  -i: ip_address  -g: gateway -m netmask"
            exit 1
        fi


        # method 1: use nmcli tool to change the network . but not work after reboot
            # net_uuid=`nmcli connection show | grep ${NAME} | awk  -F' '  '{print $2}'`
            # echo "uuid:${net_uuid}"
            # nmcli connection modify uuid ${net_uuid} ipv4.addresses ${IP} ipv4.gateway ${GW}
            # echo "up ${net_uuid}"
            # nmcli connection up uuid ${net_uuid}
            # ifconfig | grep -w inet | awk -F' ' 'NR==1  {print $2}'

        # method 2 change the file:  /etc/network/interfaces    
        # such as： 
            # auto eno1
            # iface eno1 inet static  
            # address 192.168.16.1
            # netmask 255.255.255.0
            # gateway 192.168.16.16
            # dns-nameservers 8.8.8.8 
        # sudo cat /etc/network/interfaces | grep -A 4 'eno1'  #-A print the following 4 lines
        # get the file content and parse it
        CUR_INFO=`sudo cat /etc/network/interfaces | grep -A 4 ${NAME}`
        # echo "${CUR_INFO}"
        CUR_IP=`echo ${CUR_INFO} | grep -o "address.*" | awk -F' ' '{print $2}'`
        CUR_MASK=`echo ${CUR_INFO} | grep -o "netmask.*" | awk -F' ' '{print $2}'`
        CUR_GW=`echo ${CUR_INFO} | grep -o "gateway.*" | awk -F' ' '{print $2}'`
        CUR_DNS=`echo ${CUR_INFO} | grep -o "dns-nameservers.*" | awk -F' ' '{print $2}'`
        echo "current ip:   ${CUR_IP}"
        echo "current mask: ${CUR_MASK}"
        echo "current gw:   ${CUR_GW}"
        echo "current dns:  ${CUR_DNS}"
        # use sed to change file
        sudo sed -i "s/address ${CUR_IP}/address ${IP}/g" /etc/network/interfaces
        sudo sed -i "s/netmask ${CUR_MASK}/netmask ${MASK}/g" /etc/network/interfaces
        sudo sed -i "s/gateway ${CUR_GW}/gateway ${GW}/g" /etc/network/interfaces
        sudo service networking restart  #restart the network service
        
        shift
        ;;
    --setdns)
        
        shift
        ;;
	--getvsmnetflow)
		INTERFACE_MAC=`sudo virsh  domiflist --domain $2 | grep ${VSM_INTERFACE_TYPE} | awk -F' ' '{print $5}'`
		CUR_RX_BYTES=`sudo virsh domifstat --domain $2 --interface ${INTERFACE_MAC} | grep -w rx_bytes | awk -F' ' '{print $3}'`
        echo current rx_bytes=${CUR_RX_BYTES}
		shift
        ;;
    --vsmlist)
        sudo virsh list --all --uuid --name   #display all vsm's uuid and name
        shift
        ;;
    --vsmstate)
        sudo virsh dominfo  --domain $2 | grep State | awk -F' ' '{$1=""; print}'
        shift
        ;;
	--clone) 
		shift
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
            #clone vsm
            echo "source_name=${source_name} new_name=${new_name}"
            sudo virt-clone --original ${source_name} --name ${new_name} --auto-clone 
            #get vsm's uuid
            domain_uuid=`sudo virsh domuuid --domain ${new_name}`   
            echo "uuid: ${domain_uuid}"
            #set vsm autostart
            sudo virsh autostart --domain ${new_name}   
            #get the mac (122)  
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





