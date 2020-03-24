# Microcontrollers 2 Final Assignment

## Final
The "Final" directory contains the Atmel Studio 7 Project. This project contains the code which reads the BH1750 sensor, outputs a value on the MCP23008 chip and sends the read value from the sensor via USART.
</br>
### Dependencies
* No dependencies given Atmel Studio 7 is installed
</br></br>
## Python_Code
The Python_Code directory contains the python script running on the Raspberry Pi. Every 10 minutes this code reads the value sent via USART and inputs the value in a database
</br>
### Dependencies
* pySerial
* mysql-connector-python
</br></br>
## C#_Code
The C#_Code Directory contains a Visual Studio .Net Framework Windows Form project. This project contains the GUI of the final assignment
</br>
### Dependencies
 _Best to install these dependencies via Nuget_
* Oxyplot
* MySql.Data