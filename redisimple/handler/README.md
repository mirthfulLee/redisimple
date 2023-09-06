# handler

Handlers are the working units in Reactor Redis framework, they handle events like connection request, socket Input/Output, timed tasks.

## feature of handler

They have flowing features:

- Synchronized IO. The IO multiplexer only tells specific handler that the event is 'ready', and the handler have to read or write data it self.
- Blocked. The IO multiplexer could assign only one task to one handler, and then blocked until the handler finished (return)
- Single process(thread). Redis does not use multiple process or thread, so handler is much simpler since there is no concern about synchronization.

## handler types

There are different handler designed for different tasks.

### connection handler

When the listen socket get new connection, this handler will be used. It only needs to accept the new connection, and add the socket to IO multiplexer.

### IO handler

When there is input in connection socket, this IO handler will be used to read, operate and return result to client. Its process includes:

1. read request.
2. deserialize and resolve request.
3. execute request.
    - validate the request format.
    - check the permission of user.
    - call the specific processor to execute the request.
4. get result of execution.
5. serialize the result and write to socket.

### timer handler

When the timer ends, the timer handler will be used to finish the periodic tasks like cleaning up expired data, updating DB status.
