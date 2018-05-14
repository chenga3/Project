import java.util.ArrayList;

import org.newdawn.slick.Image;
import org.newdawn.slick.Input;
import org.newdawn.slick.SlickException;

public class Player extends Sprite {
	
	public static final String SHIELD_PATH = "res/shield.png";
	public static final String IMAGE_PATH = "res/spaceship.png";
	
	public static final int INITIAL_X = 512;
	public static final int INITIAL_Y = 688;
	public static final int INITIAL_LIVES = 3;
	public static final int SHIELD_DURATION = 3000;
	public static final int BASE_SHOT_DELAY = 350;
	public static final int POWERUP_DURATION = 5000;
	public static final int ENHANCED_SHOT_DELAY = 150;
	
	public static float SPEED = 0.5f;
	
	private int shotDelay = BASE_SHOT_DELAY;
	private int shotCooldown = 0;
	private int lives = INITIAL_LIVES;
	private int score = 0;
	private Image shield;
	private ArrayList<PowerUp> powerUps = new ArrayList<>();
	
	public Player() {
		super(IMAGE_PATH, INITIAL_X, INITIAL_Y);
		try {
			shield = new Image(SHIELD_PATH);
		} catch (SlickException e) {
			e.printStackTrace();
		}
		powerUps.add(new PowerUp("shield", SHIELD_DURATION));
	}
	
	@Override
	public void update(Input input, int delta) {
		doMovement(input, delta);
		// do shooting
		if (input.isKeyDown(Input.KEY_SPACE) && shotCooldown <= 0) {
			World.getInstance().addSprite(new LaserShot(getX(), getY(), this));
			shotCooldown = shotDelay;
		}
		if (shotCooldown > 0) {
			shotCooldown -= delta;
		}
		
		// Powerup management
		for (int i = 0; i < powerUps.size(); i++) {
			powerUps.get(i).update(input, delta);
		}
		// Check if any powerups have run out
		for (int i = 0; i < powerUps.size(); i++) {
			if (!powerUps.get(i).getActive()) {
				powerUps.remove(i);
				i--;
			}
		}
		// Check if we have a shotspeed powerup
		if (checkPowerUp("shotspeed")) {
			shotDelay = ENHANCED_SHOT_DELAY;
		} else {
			shotDelay = BASE_SHOT_DELAY;
		}
		
		
	}
	
	@Override
	public void render() {
		super.render();
		if (hasShield()) {
			// Draw shield centred around Player's position
			shield.drawCentered(getX(), getY());
		}
	}
		
	private void doMovement(Input input, int delta) {
		// handle horizontal movement
		float dx = 0;
		if (input.isKeyDown(Input.KEY_LEFT)) {
			dx -= SPEED;
		}
		if (input.isKeyDown(Input.KEY_RIGHT)) {
			dx += SPEED;
		}

		// handle vertical movement
		float dy = 0;
		if (input.isKeyDown(Input.KEY_UP)) {
			dy -= SPEED;
		}
		if (input.isKeyDown(Input.KEY_DOWN)) {
			dy += SPEED;
		}
		
		move(dx * delta, dy * delta);
		clampToScreen();

	}
	
	public void contactSprite(Sprite other) {
		// Player loses life upon contact with enemy
		if (other instanceof Enemy && !hasShield()) {
			loseLife();
		}
		
		if (other instanceof PowerUpSprite) {
			// Typecast to PowerUpSprite class
			PowerUpSprite powerUpSprite = (PowerUpSprite) other;
			// Create a powerup variable
			PowerUp powerUp = powerUpSprite.getPowerUp(POWERUP_DURATION);
			// Add it to the powerup arraylist
			activatePowerUp(powerUp);
			powerUpSprite.deactivate();
		}
				
	}
	
	public void loseLife() {
		if (!hasShield()) {
			if (lives > 0) {
				lives--;
				// Activate shield
				activatePowerUp(new PowerUp("shield", SHIELD_DURATION));
			} else {
				System.exit(0);
			}
		}
	}
	
	private void activatePowerUp(PowerUp powerUp) {
		// If the power up is already activated, remove it and refresh
		for (int i = 0; i < powerUps.size(); i++) {
			if (powerUps.get(i).getType().equals(powerUp.getType())) {
				powerUps.remove(i);
				i--;
			}
		}
		powerUps.add(powerUp);
	}
	
	public boolean hasShield() {
		return checkPowerUp("shield");
	}
	
	public boolean checkPowerUp(String type) {
		for (PowerUp powerUp: powerUps) {
			if (powerUp.getType().equals(type)) {
				return true;
			}
		}
		return false;
	}
	
	public int getLives() { return lives; }
	public int getScore() { return score; }
	
	public void addScore(int delta) {
		score += delta;
	}
	
}
