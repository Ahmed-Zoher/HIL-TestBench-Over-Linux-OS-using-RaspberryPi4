import GPIO_write_client
from GPIO_macros import *

## GPIO pins ##
# 25 o/p -> 2 i/p
# 24 o/p -> 3 i/p
# 16 o/p -> 5 i/p

## PWM pins ##
# 18 PWM1
# 13 PWM2


def RunTestCases ():
    
    TestBench = GPIO_write_client.TestBench()
    ## Write Test cases starting from here ##
    TestBench.GPIO_SetMode(25, OUTPUT)
    TestBench.GPIO_SetMode(2, INPUT)
    TestBench.GPIO_Write(25, HIGH)
    value = TestBench.GPIO_Read(2)
    print ("Pin 2 reading : " + str(value) )
    
    TestBench.GPIO_SetMode(24, OUTPUT)
    TestBench.GPIO_SetMode(3, INPUT)
    TestBench.GPIO_Write(24, LOW)
    value = TestBench.GPIO_Read(3)
    print ("Pin 3 reading : " + str(value) )
    
    TestBench.GPIO_SetMode(16, OUTPUT)
    TestBench.GPIO_SetMode(5, INPUT)
    TestBench.GPIO_Write(16, HIGH)
    value = TestBench.GPIO_Read(5)
    print ("Pin 5 reading : " + str(value) )
    
    TestBench.GPIO_SetMode(18, ALT5)
    TestBench.PWM_Configure(18,1,500000)

if __name__ == '__main__':
    
    RunTestCases()