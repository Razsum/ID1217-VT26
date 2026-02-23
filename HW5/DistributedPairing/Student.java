package HW5.DistributedPairing;

import java.io.*;
import java.net.*;

public class Student {
    private static final String SERVER_HOST = "localhost"; //Servern (Läraren) ligger på samma dator som clienten(eleven)
    private static final int SERVER_PORT = 12345; //Clienten (eleven) når Servern (Lärarn) genom Serverns port nummer SERVER_PORT 

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Skicka ett id för studenten");
            return;
        }

        //Studentens id är den första linjen den får i sin buffer när den initialeras
        String studentId = args[0];
        String partnerId = null;
        
        try (
            Socket socket = new Socket(SERVER_HOST, SERVER_PORT); //Studenten öppnar en TCP anslutning och kontaktar Servern (läraren) för att skapa en kommunikations kanal
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream())); //Input Buffer
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true) //Output Buffer
            ) { 

            out.println(studentId);     // Skicka en request till lärarn (med sitt id)
            try {
                partnerId = in.readLine();   // Vänta tills en linje kommer in i buffern och läs den (det ska vara studentens partner)
            } catch (IOException e) {
                //Det här händer om lärare stänger socketen innan eleven får en partnerid == null eller == CLASS_FULL
                System.out.println("Student " + studentId + " was rejected: class is full.");
                return;
            }

            if (partnerId == null || partnerId.equals("CLASS_FULL")) { //Om partner id är null eller == CLASS_FULL så är klassen full och eleven ska sluta köras
                System.out.println("Student " + studentId + " was rejected: class is full.");
                return;
            } System.out.println("Student " + studentId + " is paired with Student " + partnerId);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}