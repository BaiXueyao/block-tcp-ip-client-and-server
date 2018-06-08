# block-tcp-ip-client-and-server
An simple exmple for tcp/ip client and server.

str_echo():
In the server, the str_echo() function means the client send message 
to the server, the server get and resend this message to the client. 
We can change this method like make it be a chat room. 
It very simple that just change the the standar input and output.

For the client, the second argument is the server's ip address.
Both of them have port number 7899, of course you can change this 
port number, but make sure they must be same.

In here, we fork() a child process to handle the task in the server
for each client. It's will increase the cpu load. Maybe 10 or 20 clients
will not cause some big problem, but if we have hundreds and thousands
clients, the server will crush because the cpu. 

So, next we will try the I/O reused method like select and poll to 
solve this problem.
