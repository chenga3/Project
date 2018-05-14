import org.newdawn.slick.Image;
import org.newdawn.slick.Input;
import org.newdawn.slick.SlickException;

public class Background {

	private static final float SCROLL_SPEED = 0.2f;
	
	// y coordinate of the middle images
	private float backgroundOffset = 0;
	private Image backgroundImage;
	
	public Background(String imageLocation) throws SlickException{
		backgroundImage = new Image(imageLocation);
	}
	
	public void update(Input input, int delta) {
		backgroundOffset += SCROLL_SPEED * delta;
		backgroundOffset = backgroundOffset % backgroundImage.getHeight();
	}
	
	public void render() {
		// Tile the background image
		for (int i = 0; i < App.SCREEN_WIDTH; i += backgroundImage.getWidth()) {
			for (int j = -backgroundImage.getHeight() + (int)backgroundOffset; j < App.SCREEN_HEIGHT; j += backgroundImage.getHeight()) {
				backgroundImage.draw(i, j);
			}
		}
	}
}
