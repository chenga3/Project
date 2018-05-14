
public class BasicEnemy extends Enemy {
	
	public static final String IMAGE_PATH = "res/basic-enemy.png";
	public static final int SCORE = 50;

	public BasicEnemy(float x, float y) {
		super(IMAGE_PATH, x, y, SCORE);
	}
}
