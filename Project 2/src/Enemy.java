public class Enemy extends Sprite {
	
	private int scoreValue;
	
	public Enemy(String imageSrc, float x, float y, int scoreValue) {
		super(imageSrc, x, y);
		this.scoreValue = scoreValue;
	}

	@Override
	public void contactSprite(Sprite other) {
		// Enemy dies upon being shot
		if (other instanceof LaserShot) {
			LaserShot laser = (LaserShot) other;
			// Increment player score
			laser.getShooter().addScore(scoreValue);
			
			// 2.5% chance of dropping each powerup
			Double random = Math.random();
			if (random < 0.025) {
				World.getInstance().addSprite(new ShieldSprite(getX(), getY()));
			} else if (random >= 0.025 && random < 1) {
				World.getInstance().addSprite(new ShotSpeedSprite(getX(), getY()));
			}
			
			deactivate();
			other.deactivate();

		}
	}
}
