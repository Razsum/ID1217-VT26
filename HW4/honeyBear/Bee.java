package HW4.honeyBear;

public class Bee extends Thread {
    private HoneyPot pot;
    private int id;

    public Bee(HoneyPot pot, int id){
        this.pot = pot;
        this.id = id;
    }

    @Override
    public void run(){
        try{
            while (true) {
                Thread.sleep(500); // simulate gathering honey
                pot.putHoney(id);
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } 
    }
}
