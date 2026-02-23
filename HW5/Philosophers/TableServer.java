package HW5.Philosophers;

import java.io.*;
import java.net.*;

public class TableServer {
    private final boolean[] forks = { true, true, true, true, true };

    public static void main(String[] args) throws IOException {
        TableServer server = new TableServer();

        try (ServerSocket serverSocket = new ServerSocket(8888)) {
            System.out.println("Table is open. Waiting for philosophers..");

            while (true) {
                // Blocking call, waits for a philosopher (client) to connect
                Socket socket = serverSocket.accept();
                // Each philosopher gets a thread to handle their requests
                new Thread(() -> server.handleClient(socket)).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void handleClient(Socket socket) {
        // ObjectOutPutStream sends and receives the Message Object
        try (ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
                ObjectInputStream in = new ObjectInputStream(socket.getInputStream())) {
            // Infinite loop
            while (true) {
                Message msg = (Message) in.readObject();
                // Server calls acquireForks. Blocks thread if forks are busy
                // Responds with a granted if client can access
                if (msg.type == Message.Type.REQUEST) {
                    acquireForks(msg.id);
                    out.writeObject(new Message(Message.Type.GRANTED, msg.id));
                    // Releases forks from client, opening them for others
                } else if (msg.type == Message.Type.RELEASE) {
                    releaseForks(msg.id);
                }
            }
        } catch (Exception e) {
            System.out.println("Server: A philosopher has left the table");
        }
    }

    // Critical section
    // synchronized - ensures that only one thread can enter this method at a time
    private synchronized void acquireForks(int id) throws InterruptedException {
        int left = id, right = (id + 1) % 5;
        int leftPhil = (id + 4) % 5, rightPhil = (id + 1) % 5;

        if (!forks[left])
            System.out.println(
                    "Server: Philosopher " + id + " is waiting for philosopher " + leftPhil + " to finish eating");
        if (!forks[right])
            System.out.println(
                    "Server: Philosopher " + id + " is waiting for philosopher " + rightPhil + " to finish eating");
        while (!forks[left] || !forks[right])
            wait();
        forks[left] = forks[right] = false;
        System.out.println("Server: Philosopher " + id + " is eating.");
    }

    private synchronized void releaseForks(int id) {
        forks[id] = forks[(id + 1) % 5] = true;
        System.out.println("Server: Philosopher " + id + " finished.");
        notifyAll();
    }
}