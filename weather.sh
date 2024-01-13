#!/bin/bash
#export DISPLAY=:0  # Use a virtual display, change the number if needed
#export XAUTHORITY=/home/ali8098/.Xauthority

# Path to the report file
DATA_FILE=/home/aonymous/Desktop/cew_project/environmental_report.txt
complex_EXEC=/home/anonymous/Desktop/cew_project/my_program

# Run complex to update data
$complex_EXEC

# Function to read data from processed_data.txt
read_data() {
    if [ -f "$DATA_FILE" ]; then
        cat "$DATA_FILE"
  
    fi
}

# Read data from processed_data.txt
data=$(read_data)

# Use Xvfb to run Zenity
echo "$data" 

#zenity --info --text="$data" --width 500 --height 250 --timeout=10 --title="Environmental Data" >> /var/log/weather_cron1.log 2>&1

# Exit the script
exit 0

