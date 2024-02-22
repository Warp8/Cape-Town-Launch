# Stockbridge Cape Town Launch
<img align="left" src="https://user-images.githubusercontent.com/22381811/221216441-0308e739-31b6-4197-8540-0f042f9e8010.png" width="250" height="250" />

Stockbridge Special Projects Lab sensor logging code designed for the Cape Town Launch in March of 2023.
This code is made for the xChips extended core module along with these xChips sensors:
- SGP30 (Gas, CO2, VOCT)
- LIS2DH12 (IMU, 3 Axis)
- SPL06-007 (Pressure, Altimeter)

<br clear="left"/>

---

NOTE: This code will only work consistently if the zero resistors labeled INT1 and INT2 on the SPLO6-007 (IMU) are removed (via desoldering or otherwise). This code will also only run if all three sensors are installed along with a *FAT32* formatted SD Card.

This is what your hardware should look like if you are replicating this configuration.
<img src="https://user-images.githubusercontent.com/22381811/220488130-2704702e-7b64-4b8f-b166-e6caea7a9caa.jpg" width="500" height="500" />

This code is open source and you are free to modify it according to the license. 
