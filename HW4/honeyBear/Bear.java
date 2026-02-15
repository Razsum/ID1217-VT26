package HW4.honeyBear;

public class Bear extends Thread {
    private HoneyPot pot;

    public Bear(HoneyPot pot){
        this.pot = pot;
    }

    @Override
    public void run(){
        try{
            while (true) 
                pot.eatAllHoney();
        } catch(InterruptedException e) {
            e.printStackTrace();
        }
    }
}
