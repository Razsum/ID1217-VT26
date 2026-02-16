package HW4.honeyBear;

public class BearsAndBees {
    public static void main(String[] args) {
        int potCapacity = 8;
        int numberOfBees = 3;

        HoneyPot pot = new HoneyPot(potCapacity);

        Bear bear = new Bear(pot);
        bear.start();

        Bee[] bees = new Bee[numberOfBees];

        //Create and start bee threads
        for(int i = 0; i < numberOfBees; i++){
            bees[i] = new Bee(pot, i);
            bees[i].start();
        }
    }
}
