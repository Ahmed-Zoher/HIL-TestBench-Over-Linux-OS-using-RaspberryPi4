
import grpc

#contatin the request and response classes
import GPIO_write_pb2_grpc
#contains the generated client and server classes
import GPIO_write_pb2_grpc_grpc

def run():
    #open a gRPC channel
    channel = grpc.insecure_channel('localhost:50051')

    #create a stub (client)
    stub = GPIO_write_pb2_grpc_grpc.PI_GPIOStub(channel)

    #create a valid request message
    Mode_params = GPIO_write_pb2_grpc.ModeInputParams(gpio_pin=18,gpio_mode=1)
    Write_params = GPIO_write_pb2_grpc.SetInputParams(gpio_pin=18,gpio_level=1)
    
    #make the call
    stub.set_mode(Mode_params)
    stub.write(Write_params)
    
    print("Pin set :D")

if __name__ == '__main__':
    #logging.basicConfig()
    run()
