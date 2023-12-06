from pynq import Overlay
from pynq import allocate
import numpy as np
import math
import time


# ### Program the FPGA with the bit file

ol = Overlay("/home/xilinx/jupyter_notebooks/bit_files/design_1.bit")
# ol = Overlay("/home/xilinx/jupyter_notebooks/bit_files/design_2.bit")
# ol = Overlay("/home/xilinx/jupyter_notebooks/bit_files/design_3.bit")



# ### Check the IPs in the overaly (configuration provided by the bit file)

ol.ip_dict


# ### Create an instance of the DMA and define functions for sending and receiving the data

dma = ol.axi_dma_0

dma_send = ol.axi_dma_0.sendchannel
dma_recv = ol.axi_dma_0.recvchannel


# ### Define three arrays in the PS memory to store the data transfer
data_size = 16
input_buffer1 = allocate(shape=(data_size,), dtype=float)
input_buffer2 = allocate(shape=(data_size,), dtype=float)
output_buffer = allocate(shape=(data_size,), dtype=float)

# ### Generate some test data

for i in range(data_size):
    input_buffer1[i] = i + 1
for i in range(data_size):
    input_buffer2[i] = i + 1

# ### Send the data

start = time.time()

dma_send.transfer(input_buffer1)


dma_send.idle

dma_send.transfer(input_buffer2)
dma_send.idle


# ### Receive the data from the Streaming FIFO
dma_recv.transfer(output_buffer)
dma.recvchannel.wait()

end = time.time()
fpga_run_time = end - start


A_matrix = input_buffer1.reshape((int(math.sqrt(data_size)),int(math.sqrt(data_size))))
B_matrix = input_buffer2.reshape((int(math.sqrt(data_size)),int(math.sqrt(data_size))))
Output_matrix = output_buffer.reshape((int(math.sqrt(data_size)),int(math.sqrt(data_size))))

print('A Matrix :')
print(A_matrix)
print("\n")

print('B matrix :')
print(B_matrix)
print("\n")


print('Out matrix :')
print(Output_matrix)
print("\n")


output_matrix = A_matrix @ B_matrix

print("Results match: {}".format(np.array_equal(output_matrix.reshape((data_size,)), output_buffer)))


dma_recv.idle
print('FPGA run time: ', fpga_run_time)
