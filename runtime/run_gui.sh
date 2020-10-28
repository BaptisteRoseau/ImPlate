#!/bin/bash
set -e # Exit on error

# CONFIGURATION 
INPUT=                 # The input file for the blur programm
OUTPUT=                # The output file used for backup, that will be copied on the server with the same name.
SERV_ROOT=      # The ssh adress
SSHFS_SERV_IMG_DIR=  # The sshfs "img" local directory (used to read the buffer only)
IMG_DIR= # The path to the "img" on the server (used to crop buffer files path)
DIST_SERV_IMG_DIR=$SERV_ROOT:$IMG_DIR             # The remote "img" directory
BUFFER_GUI=buffer_gui                             # Created during the listing process

SUCCESS_FILE=blur_success.txt  # Blur output success files
FAILURE_FILE=blur_failures.txt # Blur output failure files
LOG_FILE=log.txt               # Blur logs

# VARIABLES
NOW=$(date +"%Y-%m-%d_%k-%M-%S") # Used for renaming logs, success and failures

# ===============================

# CLEANING
#rm -rf $INPUT $OUTPUT $SUCCESS_FILE $FAILURE_FILE $LOG_FILE
#mkdir -p $INPUT $OUTPUT

###### NEW AREA BEGIN

######## NOT DONE BEGIN

rm -f $BUFFER_GUI

# LISTING FILES
STR_CMP="success_2020-01-07_15-59-55.txt" # On va donner ça en entrée du script bash sur le serveur: 2020-01-07_15-59-55
FILE_LIST=""
FILE_LIST_ALL=$(cat ../ls_example.txt | sort -r)
for file in $FILE_LIST_ALL; do
    if [[ $file < $STR_CMP ]]; then \
        break;
    fi
    FILE_LIST=$(printf "$FILE_LIST\n$file")
done

exit
######## NOT DONE END


# CREATING BUFFER_GUI
BUFFER_GUI=""
for file in $FILE_LIST; do \
    while IFS="" read -r p || [ -n "$p" ]; do
        BUFFER_GUI=$(printf "$BUFFER_GUI\n$p")
    done < $file
done

# Et on va afficher $BUFFER_GUI en sortie, donc cherche à le mettre en variable plutôt
exit
###### NEW AREA END


# RETRIEVING FILES
if [ -f $BUFFER_GUI ]; then
    # Copying buffer data to input directory
    while IFS="" read -r p || [ -n "$p" ]
    do
        path=${p#"./$IMG_DIR"}
        mkdir -p $INPUT/$(dirname $path)
        echo "Copying $path.."
        scp $SERV_ROOT:$p $INPUT/$path
    done < $BUFFER_GUI
    rm $BUFFER_GUI
else 
    echo "ERROR: Buffer file not found."
    exit
fi

# EXECUTING BLUR
echo "Executing blur..."
blur_gui -i $INPUT -o $OUTPUT -l $LOG_FILE -r -v -s -p -b 150

# Asking for user before sending pictures, if an error occured during blur
while true; do
    read -p "Do you wish to send the pictures back to the server (y/n) ?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) echo "Exiting program."; exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

exit # Remove me when everything's fine


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

# SENDING LOGS
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
