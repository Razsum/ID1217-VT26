package HW4.Birds;

public class ParentBird extends Thread {
    private WormDish worms;

    public ParentBird(WormDish worms) {
        this.worms = worms;
    }

    @Override
    public void run() {
        try {
            while (true) {
                worms.gatherWorms();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
