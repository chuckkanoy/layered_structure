# Layered Structure
This is another project from my CSCI 459: Foundations of Computer Networks class. **Unfortunately, the following commands won't work on your computer**, but the code and commands are available for those curious. To compile each source code file for this project, use -pthread for compiling threading codes

## Compiling (enter the command following the first colon):
physical wire: 
```
gcc -pthread physical_wire.c -o physical_wire.exe
```
data link layer: 
```
gcc -pthread data_link_layer.c -o data_link_layer.exe
```
network layer:
```
gcc -pthread network_layer.c -o network_layer.exe
```
## Running:
physical wire: 
```
./physical_wire.exe wire_port
```
(wire_port is some integer (4000 in grading demo) representing the port for data link layer to connect to)<br/>
data link layer: 
```
./data_link_layer.exe wire_addr wire_port data_port
```
(wire_addr is the ip address of the lab computer used (i.e. lab00.cs.ndsu.nodak.edu),<br/>
wire_port is same as used in physical wire execution,<br/>
data_port is some integer (3000 in grading demo) representing the port for network layer to connect to)<br/>
network layer: 
```
./network_layer.exe data_addr data_port nickname
```
(data_addr is the ip address of the lab computer used for data link layer (i.e. lab00.cs.ndsu.nodak.edu),<br/>
data_port is same as used in data link layer execution (different for different machines),<br/>
nickname is a string representing the name of the user of the system)<br/>
