# Pedometer
This high accuracy and extremely low power (<50uA) pedometer utilizes the XLP sleep feature of the PIC18F26K20 and Kionix KX-126 triad accelerometer to monitor and record # of steps taken by the user. The wearable goes to sleep after 30 second of inactivity however the KX126 wakes up after just 50ms of motion detected and continues to record data using internal FIFO. The firmware then allows the user to see # of steps in the 1000’s by pressing a tactile switch and counting LED flashes (saves substantial power over LCD or 7 seg led. Another tactile switch is used to connect to putty and reveal the exact number of steps. Again one of the tactile switches can be pressed for > 5 seconds to reset the pedometer cnt registers.
# The KX126 
Is a very low cost MEMs accelerometer with over  50 registers the can be used to customize the behavior of the pedometer. Some of the features include stride length, step sensitivity impact, # steps before record starts. The device is configured by the microcontroller via I2C in this example to detect movement and capture # of steps taken. Multiple interrupts can be setup, currently INT1 is latches high when the pedometer reaches 5000 cnts.
