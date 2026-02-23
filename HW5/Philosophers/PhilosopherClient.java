package HW5.Philosophers;

import java.io.*;
import java.net.Socket;
import java.util.Random;

public class PhilosopherClient {
    public static void main(String[] args) {
        if (args.length < 1)
            return;
        int id = Integer.parseInt(args[0]);
        Random rand = new Random();

        try (Socket socket = new Socket("localhost", 8888);
                // OutputStream converts data into bytes
                // InputStream converts bytes into data
                ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
                ObjectInputStream in = new ObjectInputStream(socket.getInputStream())) {

            System.out.println("Philosopher " + id + " joined.");

            while (true) {
                System.out.println("Philosopher " + id + " is thinking.");
                Thread.sleep(rand.nextInt(3000) + 2000);

                // Sends a request to the server with client ID
                // Using writeObject(Message) to send message to InputPutStream
                System.out.println("Philosopher " + id + " wants to eat.");
                out.writeObject(new Message(Message.Type.REQUEST, id));

                // Stops and waits for a reply
                // Atomic - Client is unaware if forks are available and 
                // which clients are currently using them
                if (((Message) in.readObject()).type == Message.Type.GRANTED) {
                    System.out.println("Philosopher " + id + " is eating.");
                    Thread.sleep(rand.nextInt(3000) + 2000);
                    out.writeObject(new Message(Message.Type.RELEASE, id));
                    System.out.println("Philosopher " + id + " has finished eating.");
                }
            }
        } catch (Exception e) {
            System.out.println("Lost connection to table");
        }
    }
}