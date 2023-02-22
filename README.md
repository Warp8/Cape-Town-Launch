# Stockbridge Cape Town Launch
Stockbridge Special Projects Lab sensor logging code designed for the Cape Town Launch in March of 2023.
This code is made for the xChips extended core module along with these xChips sensors:
- SGP30 (Gas, CO2, VOCT)
- LIS2DH12 (IMU, 3 Axis)
- SPL06-007 (Pressure, Altimeter)
NOTE: This code will only work consistently if the zero resistors labeled INT1 and INT2 on the SPLO6-007 (IMU) are removed (via desoldering or otherwise). This code will also only run if all three sensors are installed along with a *FAT32* formatted SD Card.

This code is open source and you are free to modify it according to the liscence. 
