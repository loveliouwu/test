#!/bin/bash  

while true; 
do
    case $1 in
    --start)
        echo "start" > /home/lyp/Desktop/log
        exec sudo /home/lyp/Desktop/qt_http/build-qt_http-Desktop_Qt_6_6_1_GCC_64bit-Debug/xdpcard_server &
        exit 0
        ;;
    --stop)
        SERVICE_PID=`pgrep xdpcard_server`
        echo "Get PID:\n${SERVICE_PID}"
        if [ -z "$SERVICE_PID" ]; then
            exit 1
        fi
        SERVICE_NUM=`pgrep xdpcard_server | wc -l`
        INDEX=0
        while [[ $INDEX -lt $SERVICE_NUM ]]; do
            INDEX=$(expr "$INDEX" + 1)
            SERVICE_PID=`pgrep xdpcard_server | awk 'NR==1{print}'`
            echo "Kill ${INDEX} PID:${SERVICE_PID}"
            sudo kill -9 ${SERVICE_PID}
        done
        exit 0
        ;;
	esac
done
}





