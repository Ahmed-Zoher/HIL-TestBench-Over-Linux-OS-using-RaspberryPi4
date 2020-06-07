
from concurrent import futures
import grpc
import pigpio

#contatin the request and response classes
import GPIO_write_pb2
#contains the generated client and server classes
import GPIO_write_pb2_grpc

pi1 = pigpio.pi()

#class that subclasses the generated class
class PI_GPIOServicer(GPIO_write_pb2_grpc.PI_GPIOServicer):

    def set_mode(self, request, context):
        response = GPIO_write_pb2.Empty()
        pi1.set_mode( request.gpio_pin,request.gpio_mode )
        return response
    
    def write(self, request, context):
        response = GPIO_write_pb2.Empty()
        pi1.write( request.gpio_pin,request.gpio_level )
        return response
        
    def read(self, request, context):
        response = GPIO_write_pb2.GPIO_Level()
        response.gpio_pin_level = pi1.read( request.gpio_pin )
        return response
        
    def set_pull_up_down(self, request, context):
        response = GPIO_write_pb2.Empty()
        pi1.set_pull_up_down( request.gpio_pin,request.gpio_pud )
        return response
        
    def hardware_PWM (self, request, context):
        response = GPIO_write_pb2.PWM_Status()
        response.PWM_return = pi1.hardware_PWM( request.gpio_pin , request.PWMfreq ,request.PWMduty )
        return response


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    GPIO_write_pb2_grpc.add_PI_GPIOServicer_to_server(PI_GPIOServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()


if __name__ == '__main__':
    serve()
