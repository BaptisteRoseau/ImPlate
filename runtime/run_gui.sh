#!/bin/bash
cd $(dirname $0)
source ./config.sh

# VARIABLES
#NOW=$(date +"%Y-%m-%d_%k-%M-%S") # Used for renaming logs, success and failures (SORTABLE MAIS IL FAUT MODIFIER LE FORMAT DES FICHIERS DE LOG SUR LE SERVEUR AVANT)
NOW=$(date +"%d-%m-%Y_%k-%M-%S") # Used for renaming logs, success and failures

# ===============================

# CLEANING
rm -rf $INPUT_GUI $OUTPUT_GUI $SUCCESS_FILE_GUI $FAILURE_FILE_GUI $LOG_FILE_GUI
mkdir -p $INPUT_GUI $OUTPUT_GUI

# If buffer was cleaned, lat execution was successful
if [ ! -f $BUFFER_GUI ]; then
    # RETRIEVING AND CLEANING BUFFER
    scp $SERV_ROOT:$BUFFER_GUI_SERV $BUFFER_GUI
    ssh $SERV_ROOT rm $BUFFER_GUI_SERV
fi

# RETRIEVING FILES
if [ -f $BUFFER_GUI ]; then
    # Copying buffer data to INPUT_GUI directory
    while IFS="" read -r p || [ -n "$p" ]
    do
        path=${p#"./$IMG_DIR"}
        mkdir -p $INPUT_GUI/$(dirname $path)
        echo "Copying $path.."
        scp $SERV_ROOT:$p $INPUT_GUI/$path
    done < $BUFFER_GUI
else 
    echo "ERROR: Buffer file not found."
    exit
fi

# EXECUTING BLUR
echo "Executing blur..."
$BLUR_GUI -i $INPUT_GUI -o $OUTPUT_GUI -l $LOG_FILE_GUI -r -v -s -p -b 150

# Asking user to send back images just in case of crash
while true; do
    read -p "Do you want to send back images to the server ?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

# SENDING BLUR
if [ -f $SUCCESS_FILE_GUI ]; then
    # Sending backup pictures first
    echo "Copying original pictures.."
    for dir in `ls $OUTPUT_GUI`; do \
        scp -r $OUTPUT_GUI/$dir $DIST_SERV_IMG_DIR/backup/
    done

    echo "Replacing original pictures with blured ones.."
    while IFS="" read -r p || [ -n "$p" ]
    # Sending blured pictures
    do
        echo "Replacing ${p#"$INPUT_GUI"}.."
        scp $p $DIST_SERV_IMG_DIR/${p#"$INPUT_GUI"}
    done < $SUCCESS_FILE_GUI
else
    echo "No picture were blured this time."
fi

# Sending logs
if [ -f $LOG_FILE_GUI ]; then
    scp $LOG_FILE_GUI $DIST_SERV_IMG_DIR/util/blur_log/log/log_$NOW.txt
    rm $LOG_FILE_GUI
fi
if [ -f $SUCCESS_FILE_GUI ]; then
    scp $SUCCESS_FILE_GUI $DIST_SERV_IMG_DIR/util/blur_log/success/success_$NOW.txt
    rm $SUCCESS_FILE_GUI
fi
if [ -f $FAILURE_FILE_GUI ]; then
    scp $FAILURE_FILE_GUI $DIST_SERV_IMG_DIR/util/blur_log/failure/failure_$NOW.txt
    rm $FAILURE_FILE_GUI
fi

# CLEANING
rm -rf $INPUT_GUI $OUTPUT_GUI $BUFFER_GUI $LOG_FILE_GUI $SUCCESS_FILE_GUI $FAILURE_FILE_GUI
