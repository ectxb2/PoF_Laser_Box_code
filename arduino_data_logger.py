##############
## Script listens to serial port and writes contents into a file
##############
## requires pySerial to be installed 
#Example Comand Line
#	python3 arduio_data_logger.py exampe_file_name.txt /dev/ttyACM0
#
	
import serial
import datetime
import sys

output_name=sys.argv[1] 
#output_name += ".txt"
com_port = sys.argv[2]
#name=x[1]

serial_port = com_port;
baud_rate = 115200; #In arduino, Serial.begin(baud_rate)
write_to_file_path = output_name     #"output.txt";

output_file = open(write_to_file_path, "w+");
ser = serial.Serial(serial_port, baud_rate, timeout = 2)
while True:
    line = ser.readline();
    line = line.decode("utf-8") #ser.readline returns a binary, convert to string
    line = str(datetime.datetime.now()) + "," + line
    line = line.strip("\n")
    print(line);
    output_file.write(line);
