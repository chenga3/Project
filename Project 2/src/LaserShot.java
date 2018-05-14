import org.newdawn.slick.Input;

public class LaserShot extends Sprite {
	
	public static final float SPEED = -3f;
	// Record the player who shot a particular instance of a lasershot
	private Player shooter;
	
	public LaserShot(float x, float y, Player shooter) {
		super("res/shot.png", x, y);
		this.shooter = shooter;
	}
	
	@Override
	public void update(Input input, int delta) {
		move(0, SPEED);
		if (!onScreen()) {
			deactivate();
		}
	}
	
	public Player getShooter() { return shooter; }
	
}
