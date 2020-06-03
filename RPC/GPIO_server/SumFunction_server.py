
import grpc
import pigpio

#contatin the request and response classes
import GPIO_write_pb2_grpc
#contains the generated client and server classes
import GPIO_write_pb2_grpc_grpc

#class that subclasses the generated class
class PI_GPIOServicer(GPIO_write_pb2_grpc_grpc.PI_GPIOServicer):

    pi1 = pigpio.pi()
    
    def set_mode(self, request, context):
        pi1.set_mode( request.gpio_pin,request.gpio_mode )
    
    def write(self, request, context):
        pi1.write( request.gpio_pin,request.gpio_level )
    
    print ("Parameters received\n")


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    GPIO_write_pb2_grpc.add_PI_GPIOServicer_to_server(PI_GPIOServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()


if __name__ == '__main__':
    serve()
