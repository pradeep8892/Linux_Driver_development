cmd_/home/pi/pradeep/Linux_Driver_Development/Linux_Driver_development/2.simple_kernel_module/dev_nr.mod := printf '%s\n'   dev_nr.o | awk '!x[$$0]++ { print("/home/pi/pradeep/Linux_Driver_Development/Linux_Driver_development/2.simple_kernel_module/"$$0) }' > /home/pi/pradeep/Linux_Driver_Development/Linux_Driver_development/2.simple_kernel_module/dev_nr.mod
