# Linux_Driver_development
Linux Driver Development 

I used a Raspberry Pi 3 to develop and test my modules and drivers. To compile them, you need to install the Kernel headers on your Pi. On Raspbian you can do this with the following command:

sudo apt update
sudo apt install raspberrypi-kernel-headers
Raspberry Pi OS is only installs the latest kernel headers. So, make sure, you are running the latest kernel. You can do this by running:

sudo apt upgrade
You also need the build utils (make, gcc, ...) but they come preinstalled on Raspbian.


