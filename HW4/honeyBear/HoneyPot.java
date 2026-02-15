package HW4.honeyBear;

public class HoneyPot {
    private int portions = 0;  // shared state
    private final int capacity;

    public HoneyPot(int capacity){
        this.capacity = capacity;
    }

    public synchronized void putHoney(int beeId) throws InterruptedException{
        while(portions == capacity){ //vi kör med while istället för if för tråden kan väckas och vi vill checka ifall conditionen är rätt eller inte igen innan den går visdare.
            wait(); //vänta till björnen äter upp all honung
        }

        portions++;
        System.out.println("Bee " + beeId + " added honey to the pot. Portions = (" + portions + "/" + capacity + ")");

        if(portions == capacity) {
            System.out.println("Bee " + beeId + " filled the pot! Waking the bear.");
            notifyAll(); //We can use notify all because the only thread locked now is the bear.
        }


    }

    public synchronized void eatAllHoney() throws InterruptedException {
        while(portions < capacity) {
            wait(); // wait for bees to fill pot
        }

        // Critical section: bear eats everything
        System.out.println("Bear is eating all the honey!");
        portions = 0; // pot becomes empty

        // Wake all bees so they can start filling again
        notifyAll(); 
    }

}
