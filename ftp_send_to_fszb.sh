#!/bin/sh
if [ $# -ne 4 ];then
    echo "Use parameters correctly"
    echo "[usage] ftp_.sh  source_dir  source_file  target_dir target_ip"
    exit 1
fi

SOURCE_DIR=$1
SOURCCE_FILE=$2
TARGET_DIR=$3
TARGET_IP=$4
#升级包上传到控制器
ftp -v -n $TARGET_IP <<EOF
user root 111111
binary
#hash
cd $TARGET_DIR
#rm -rf bak
lcd $SOURCE_DIR
prompt
mput $SOURCCE_FILE
close
bye
EOF

echo "ftp-send update $SOURCCE_FILE successfully"
