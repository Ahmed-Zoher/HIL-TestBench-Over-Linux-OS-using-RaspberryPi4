import time 
import GPIO_write_client 
from GPIO_macros import * 



def Test_main (): 
	## Blinky example ##
	TestBench.GPIO_SetMode(25, OUTPUT) 
	while 1 :
		TestBench.GPIO_Write(25, HIGH) 
		Time.sleep(1)
		TestBench.GPIO_Write(25, LOW) 
		Time.sleep(1)




if __name__ == '__main__': 
	global TestBench
	TestBench = GPIO_write_client.TestBench()
	Test_main()
	TestBench.PIGPIO_Stop()
