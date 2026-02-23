package HW5.DistributedPairing;

import java.io.*;
import java.net.*;
import java.util.*;

public class Teacher {
    private static final int PORT = 12345;
    private static Queue<StudentHandler> waitingQueue = new LinkedList<>();
    private static int classSize;
    private static int studentsInClass = 0;

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Give the teacher a classroom size");
            return;
        }

        classSize = Integer.parseInt(args[0]);
        System.out.println("Teacher (Server) is running...");
        System.out.println("Expected class size: " + classSize);

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {  //Här så kör vi en try ifall vi lyckas öppna en TCP port på port nummer PORT som lyssnar efter TCP anslutnings requests tills programmet stängs
            while (true) {
                Socket socket = serverSocket.accept();  //Vi väntar här tills vi får TCP respons, och när vi får det så hamnar TCP kanalen i en socket 
                
                synchronized (Teacher.class) { //Critical section where threads wait to acquire Teacher.class lock
                    if (studentsInClass >= classSize) { //If the amount of students in the class are the max capacity we will reject the student and print that class is full
                        PrintWriter tempOut = new PrintWriter(socket.getOutputStream(), true); //skapa en temporär outpuut buffer
                        tempOut.println("CLASS_FULL"); //Skicka CLASS_FULL till eleven
                        socket.close(); //Stäng socketen
                        System.out.println("Rejected a student: class is already full."); //Printa att klassen är full 
                        continue; //Jump to next iteration (check if other students are trying to enter full classroom)
                    }
                    studentsInClass++; //If class is not full, we increment the students in class variable then we move to the code under
                }
            
                StudentHandler handler = new StudentHandler(socket); //Vi ger denna socket till en studen handler objekt
                handler.start(); //Ge socketen som studenten är kopplad till, till en thread som hanterar den 
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static class StudentHandler extends Thread {
        private Socket socket;
        private BufferedReader in;
        private PrintWriter out;
        private String studentId;

        public StudentHandler(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try {
                in = new BufferedReader(new InputStreamReader(socket.getInputStream())); //Definerar vart input buffern ska strömma in info 
                out = new PrintWriter(socket.getOutputStream(), true); //Definerar vart output buffern ska strömma ut info 

                studentId = in.readLine(); //Här väntar vi på att en student ska kopplas och skriva sitt ID, sen så lägger vi det i student id
                System.out.println("Student " + studentId + " requested a partner.");

                pairStudent(this); //Ny så parar vi ihop studenten med en annan student

            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        public void sendPartner(String partnerId) {
            out.println(partnerId);
        }
    }

    // Synchronized pairing method (important!)
    private static synchronized void pairStudent(StudentHandler student) { //Critical section för waitingQueue är en delad varible
        if (waitingQueue.isEmpty()) { 
            waitingQueue.add(student); //Om det inte finns några andra elever som vill para sig så lägger vi den väntade eleven i kön
            
            // Om det är sista eleven i kön (som får platts i klassen) och klass anatalet är udda så parar vi eleven med sig själv
            if (studentsInClass == classSize && classSize % 2 == 1) {
                StudentHandler last = waitingQueue.poll();
                last.sendPartner(last.studentId);
                System.out.println("Odd number of students. " + last.studentId + " is paired with themselves.");
            }

        } else {
            StudentHandler partner = waitingQueue.poll(); //Om det dinns elever i kön så tar vi den första eleven som väntar i kön (FIFO)

            // Pair both students
            student.sendPartner(partner.studentId); //Skicka parade elevens id genom den etablerade socketen som vi skapade i Student handler
            partner.sendPartner(student.studentId); //Skicka parade elevens id genom den etablerade socketen som vi skapade i Student handler

            System.out.println("Paired " + student.studentId +
                               " with " + partner.studentId);
        }
    }
}