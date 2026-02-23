1. Distributed Pairing 1 (a client-server application)

3. Dining philosophers
Message.java
Defines our Type of messages being REQUEST-, RELEASE-, GRANTED access to forks. As well as defining client IDs. The crucial part of this object definition is “Serializable” which dictates that we’re able to convert our type objects into data streams, to send through our sockets from client to server. Where it then will be unpacked into readable objects on the server side. 
TableServer.java
We define the forks as a Boolean array to keep track of which are available, if they’re unavailable, they’ll be false. This defines and initializes a new ServerSocket connection in the localhost with port number 8888. It will check for new connections constantly and assign each new client a new thread. 
Method handleClient() will accept new incoming messages from the client side, these messages are in the form of bytes will be translated using the ObjectInputStream into usable data. The server will constantly check for new incoming messages, if the message is REQUEST, it will attempt to acquire forks for the ID, if the forks are granted it will return a GRANTED response using the out.writeObject() method. If the message is a RELEASE object it will simply release those forks making them available. 
PhilosopherClient.java
Attempts to connect to the localhost server. And will give confirmation output that the given philosopher was able to join. The client will constantly be running its loop. It begins by thinking between 3-5 seconds. Then it attempts to eat by sending a REQUEST to the server. Until it has received a GRANTED response it will be on standby. 
The client is atomic, meaning that it has no knowledge of the other clients attempting or currently busy eating, all logic is handled on the server side. When the client has received a GRANTED response it will begin eating for 3-5 seconds, before sending the RELEASE message back to the server.
