Algorithm Explanation:

The teacher is implemented as a server process using Java sockets, and students are implemented as client processes. Each student sends a partner request containing their ID to the teacher. The teacher stores incoming requests in a FIFO queue. When two students are available, the teacher pairs them and sends each student their partner’s ID using message passing. The teacher is started with the class size n and only accepts n students. If more students attempt to connect, they are rejected. If n is odd, the last remaining student in the queue is paired with themself. The system is fully distributed since each student runs as a separate process and communicates with the teacher via TCP sockets.

Teacher.java

* Acts as the **server**.
* Opens a **ServerSocket** on a port and listens for student connections.
* Maintains a **FIFO queue** of waiting students.
* When two students are in the queue, it pairs them and sends each the partners ID.
* Knows the **class size** (`n`) from a command-line argument.
* Rejects any extra students beyond `n` with a `"CLASS_FULL"` message.
* If `n` is **odd**, the last student in the queue is paired with **themselves**.
* Uses **threads** (`StudentHandler`) so multiple students can connect simultaneously.

Student.java

* Acts as a **client** process.
* Connects to the teacher via **TCP socket** (`ServerSocket`) using `localhost` and the teaches port.
* Sends its **student ID** as a request for a partner.
* Waits for the teacher to respond with the partner’s ID.
* If the class is full, prints a message and exits.
* Prints: `"Student X is paired with Student Y"` when pairing is successful.
* Completely **independent of other students**, all coordination is handled by the teacher.

How to run:

1. Compile the java files:

   - javac HW5/DistributedPairing/*.java
2. Run the server (teacher) and decide the classroom size (n):

   - java HW5/DistributedPairing.Teacher number n
3. Run the clients (students) in seperate terminals and give them an id:

   - java HW5/DistributedPairing.Student 1
   - java HW5/DistributedPairing.Student 2
   - java HW5/DistributedPairing.Student 3
   - java HW5/DistributedPairing.Student 4
   - java HW5/DistributedPairing.Student 5
