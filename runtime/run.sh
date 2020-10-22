#!/bin/bash

# CONFIGURATION 
INPUT=/                  # The input file for the blur programm
OUTPUT=/                # The output file used for backup, that will be copied on the server with the same name.
SERV_ROOT=      # The ssh adress
SSHFS_SERV_IMG_DIR=  # The sshfs "img" local directory (used to read the buffer only)
IMG_DIR= # The path to the "img" on the server (used to crop buffer files path)
DIST_SERV_IMG_DIR=$SERV_ROOT:$IMG_DIR             # The remote "img" directory
BUFFER=$SSHFS_SERV_IMG_DIR/        # The path to the buffer

SUCCESS_FILE=blur_success.txt  # Blur output success files
FAILURE_FILE=blur_failures.txt # Blur output failure files
LOG_FILE=log.txt               # Blur logs

# VARIABLES
NOW=$(date +"%d-%m-%Y_%k-%M-%S") # Used for renaming logs, success and failures

# ===============================

# CLEANING
rm -rf $INPUT $OUTPUT $SUCCESS_FILE $FAILURE_FILE $LOG_FILE
mkdir -p $INPUT $OUTPUT

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
blur -i $INPUT -o $OUTPUT -l $LOG_FILE -r -v -s -p -b 150

# SENDING BLUR
if [ -f $SUCCESS_FILE ]; then
    # Sending backup pictures first
    echo "Copying original pictures.."
    scp -r $OUTPUT/ $DIST_SERV_IMG_DIR/

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

rm -rf $INPUT
rm -rf $OUTPUT
