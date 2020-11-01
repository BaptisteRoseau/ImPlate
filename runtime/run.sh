#!/bin/bash
cd $(dirname $0)
source ./config.sh

# VARIABLES
#NOW=$(date +"%Y-%m-%d_%k-%M-%S") # Used for renaming logs, success and failures (SORTABLE MAIS IL FAUT MODIFIER LE FORMAT DES FICHIERS DE LOG SUR LE SERVEUR AVANT)
NOW=$(date +"%d-%m-%Y_%k-%M-%S") # Used for renaming logs, success and failures

# ===============================

# CLEANING
rm -rf $INPUT $OUTPUT $SUCCESS_FILE $FAILURE_FILE $LOG_FILE
mkdir -p $INPUT $OUTPUT

# RETRIEVING AND CLEANING BUFFER
scp $SERV_ROOT:$BUFFER_SERV $BUFFER
ssh $SERV_ROOT rm $BUFFER_SERV

# RETRIEVING FILES
if [ -f $BUFFER ]; then
    # Copying buffer data to input directory
    while IFS="" read -r p || [ -n "$p" ]
    do
        path=${p#"./$IMG_DIR"}
        mkdir -p $INPUT/$(dirname $path)
        echo "Copying $path.."
        scp $SERV_ROOT:$p $INPUT/$path
    done < $BUFFER
    rm $BUFFER
else 
    echo "ERROR: Buffer file not found."
    exit
fi

# EXECUTING BLUR
echo "Executing blur..."
$BLUR -i $INPUT -o $OUTPUT -l $LOG_FILE -r -v -s -p -b 150

# SENDING BLUR
if [ -f $SUCCESS_FILE ]; then
    # Sending backup pictures first
    echo "Copying original pictures.."
    for dir in `ls $OUTPUT`; do \
        scp -r $dir $DIST_SERV_IMG_DIR/backup/
    done

    echo "Replacing original pictures with blurred ones.."
    while IFS="" read -r p || [ -n "$p" ]
    # Sending blurred pictures
    do
        echo "Replacing ${p#"$INPUT"}.."
        scp $p $DIST_SERV_IMG_DIR/${p#"$INPUT"}
    done < $SUCCESS_FILE
else
    echo "No picture were blurred this time."
fi

# Sending logs
if [ -f $LOG_FILE ]; then
    scp $LOG_FILE $DIST_SERV_IMG_DIR/util/blur_log/log/log_$NOW.txt
    rm $LOG_FILE
fi
if [ -f $SUCCESS_FILE ]; then
    scp $SUCCESS_FILE $DIST_SERV_IMG_DIR/util/blur_log/success/success_$NOW.txt
    rm $SUCCESS_FILE
fi
if [ -f $FAILURE_FILE ]; then
    scp $FAILURE_FILE $DIST_SERV_IMG_DIR/util/blur_log/failure/failure_$NOW.txt
    rm $FAILURE_FILE
fi

rm -rf $INPUT $OUTPUT $BUFFER $LOG_FILE $SUCCESS_FILE $FAILURE_FILE
