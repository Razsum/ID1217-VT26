package HW4.Birds;

public class BabyBird extends Thread {
    private WormDish worms;
    private int id;

    public BabyBird( WormDish worms, int id){
        this.worms = worms;
        this.id = id;
    }

    @Override
    public void run(){
        try{
            while (true) {
                Thread.sleep(500); // Ensures semi-fairness
                worms.eatWorms(id);
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } 
    }
}
