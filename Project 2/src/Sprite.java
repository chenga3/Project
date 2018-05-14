import org.newdawn.slick.Image;
import org.newdawn.slick.Input;
import org.newdawn.slick.SlickException;

import utilities.BoundingBox;

public abstract class Sprite {
	
	private Image image = null;
	private float x;
	private float y;
	private BoundingBox bb;
	private boolean active = true;
	
	public Sprite(String imageSrc, float x, float y) {
		try {
			image = new Image(imageSrc);
		} catch (SlickException e) {
			e.printStackTrace();
		}
		
		this.x = x;
		this.y = y;
		
		bb = new BoundingBox(image, x, y);
	}
	
	public void update(Input input, int delta) {
		
	}
	
	public void render() {
		image.drawCentered(x, y);
	}
	
	/*
	 * Called whenever this Sprite makes contact with another.
	 */
	public void contactSprite(Sprite other) {
		
	}
	

	/*
	 * Returns true whenever the sprite is on screen.
	 */
	public boolean onScreen() {
		return x >= 0 && x <= App.SCREEN_WIDTH - bb.getWidth()
			&& y >= 0 && y <= App.SCREEN_HEIGHT - bb.getHeight();
	}
	
	/*
	 *  Forces the sprite to remain on the screen
	 */
	public void clampToScreen() {
		x = Math.max(x, 0);
		x = Math.min(x, App.SCREEN_WIDTH);
		y = Math.max(y, 0);
		y = Math.min(y, App.SCREEN_HEIGHT);
	}
	
	public void move(float dx, float dy) {
		x += dx;
		y += dy;
		bb.setX(x);
		bb.setY(y);
	}
	

	public float getX() { return x; }
	public float getY() { return y; }
	public boolean getActive() { return active; }
	public void deactivate() { active = false; }
	
	public BoundingBox getBoundingBox() {
		return new BoundingBox(bb);
	}
}
