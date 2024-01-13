#!/bin/bash

# Function to display a pop-up with information from environmental_report.txt
show_popup() {
    local Temperature=$(awk -F': ' '/Temperature/ {print $2}' environmental_report.txt)
    if [ -n "$Temperature" ]; then
        if [ "$(echo "$Temperature < 25" | bc)" -eq 1 ]; then
            zenity --info --title="Environmental Alert" --text="Temperature is lower than 25: $Temperature" --timeout=3600
        elif [ "$(echo "$Temperature > 35" | bc)" -eq 1 ]; then
            zenity --info --title="Environmental Alert" --text="Temperature is higher than 35: $Temperature" --timeout=3600
        fi
    fi
}

# Run environment_program every 3 hours
while true; do
    ./my_program
    sleep 2m
done &

# Display alerts using show_popup continuously
while true; do
    show_popup
done

# Keep the main script running
read -r -d '' _ </dev/null

