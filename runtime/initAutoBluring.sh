# CONFIGURATION 
SCRIPT=/home/tl/TransferLeasing/ProgFloutage/Runtime/run.sh  # Path to run.sh
INTERVAL_SEC=3600    #seconds Note: you need to put the exact value, not a multiplication

# Running periodically
$SCRIPT
echo "Execution finished on $(date +"%d-%m-%Y at %k-%M-%S")"
while sleep $INTERVAL_SEC; do
    $SCRIPT
    echo "Execution finished on $(date +"%d-%m-%Y at %k-%M-%S")"
done