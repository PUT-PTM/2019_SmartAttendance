# Smart Attendance

## Smart Attendance is a system that help lecturers checking attendance at classes.

One day I thought that there is a better way to check attendance that to read every name from a list and wait for 
students response.  

Smart Attendance is based on STM32F407VG/STM32F4DISCOVERY (main operation), RC522 RFID cards reader (reading cards' ID), 
ESP8266 (network connection) and SD card reader (reading network and classroom configuration).

It is written in C/C++ and built via OpenOCD. It also uses a Flask-based HTTP server (getting ID from STM32 and sending 
them to the database) and database (power by Microsoft SQL Server)

The main purpose was to build that would be easiest to use. Students just have to put their ELS close to the reader and 
that is it. Users are also given 4 leds to inform about its state:

     - green -> ready to use
     - orange -> waiting for cards
     - blue -> card is read and send to the database
     - red -> network configuration failed (need to restart)
     

