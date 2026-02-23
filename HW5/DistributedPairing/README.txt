Algorithm Explanation:

The teacher is implemented as a server process using Java sockets, and students are implemented as client processes.
Each student sends a partner request containing their ID to the teacher.
The teacher stores incoming requests in a FIFO queue.
When two students are available, the teacher pairs them and sends each student their partner’s ID using message passing.
The teacher is started with the class size n and only accepts n students. 
If more students attempt to connect, they are rejected. 
If n is odd, the last remaining student in the queue is paired with themself. 
The system is fully distributed since each student runs as a separate process and communicates with the teacher via TCP sockets.


How to run:
1.    Compile the java files:
        - javac DistributedPairing/*.java

2.    Run the server (teacher) and decide the classroom size (n):
        - java DistributedPairing.Teacher number n 

3.    Run the clients (students) in seperate terminals and give them an id:
        - java DistributedPairing.Student 1
        - java DistributedPairing.Student 2
        - java DistributedPairing.Student 3
        - java DistributedPairing.Student 4
        - java DistributedPairing.Student 5
