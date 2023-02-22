# Stockbridge Cape Town Launch
Stockbridge Special Projects Lab sensor logging code designed for the Cape Town Launch in March of 2023.
This code is made for the xChips extended core module along with these xChips sensors:
- SGP30 (Gas, CO2, VOCT)
- LIS2DH12 (IMU, 3 Axis)
- SPL06-007 (Pressure, Altimeter)

NOTE: This code will only work consistently if the zero resistors labeled INT1 and INT2 on the SPLO6-007 (IMU) are removed (via desoldering or otherwise). This code will also only run if all three sensors are installed along with a *FAT32* formatted SD Card.

ALSO: If you are trying to compile and run this code yourself (via PlatformIO) you must either download and use the included PlatformIO workspace OR modify your SPL06-007 library to modify the built in I2C address as it doesn't let you do this any other way on this version of the library. The address you need to change it to is in the comments of the code.

This code is open source and you are free to modify it according to the liscence. 
