
from concurrent import futures
import grpc
import pigpio

#contatin the request and response classes
import GPIO_write_pb2
#contains the generated client and server classes
import GPIO_write_pb2_grpc

global pi1

#class that subclasses the generated class
class PI_GPIOServicer(GPIO_write_pb2_grpc.PI_GPIOServicer):

    def set_mode(self, request, context):
        global pi1
        pi1 = pigpio.pi()
        response = GPIO_write_pb2.returnMsg()
        response.fstek = 1
        pi1.set_mode( request.gpio_pin,request.gpio_mode )
        return response
    
    #def write(self, request, context):
        #response = GPIO_write_pb2.returnMsg()
        #response.fstek = 1
        #pi1.write( request.gpio_pin,request.gpio_level )
        #return response


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    GPIO_write_pb2_grpc.add_PI_GPIOServicer_to_server(PI_GPIOServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()


if __name__ == '__main__':
    serve()
