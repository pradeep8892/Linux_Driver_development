cmd_/home/pi/pradeep/Linux_Driver_Development/Linux_Driver_development/4.GPIO_driver/Module.symvers :=  sed 's/ko$$/o/'  /home/pi/pradeep/Linux_Driver_Development/Linux_Driver_development/4.GPIO_driver/modules.order | scripts/mod/modpost -m -a    -o /home/pi/pradeep/Linux_Driver_Development/Linux_Driver_development/4.GPIO_driver/Module.symvers -e -i Module.symvers -T - 
