import org.newdawn.slick.Input;

public abstract class PowerUpSprite extends Sprite {
	
	public static final float SPEED = 0.1f;
	
	// Each powerup sprite is associated with an actual powerup object that can be used
	private PowerUp powerUp;
	private String type;
	
	// Create a PowerUp sprite onto the screen
	public PowerUpSprite(String imageSrc, float x, float y, String type) {
		super(imageSrc, x, y);
		this.type = type;
		powerUp = new PowerUp(type);

	}
	
	@Override
	public void update(Input input, int delta) {
		move(0, SPEED);
		if (!onScreen()) {
			deactivate();
		}
	}
	
	public String getType() { return type; }
	public PowerUp getPowerUp(int duration) {
		return new PowerUp(powerUp.getType(), duration);
	}
	
	
}
