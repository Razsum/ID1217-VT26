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
            boolean consumingWorms = true;
            while (consumingWorms) {
                Thread.sleep(500); // Ensures semi-fairness
                consumingWorms = worms.eatWorms(id);
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        } 
    }
}
