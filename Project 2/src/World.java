
import java.util.ArrayList;

import org.newdawn.slick.Graphics;
import org.newdawn.slick.Image;
import org.newdawn.slick.Input;
import org.newdawn.slick.SlickException;

public class World {
	
	public static final String LIVES_PATH = "res/lives.png";
	public static final float LIVES_X_OFFSET = 20;
	public static final float LIVES_Y_OFFSET = 696;
	public static final float LIVES_X_SEP = 40;
	
	public static final float SCORE_X_OFFSET = 20;
	public static final float SCORE_Y_OFFSET = 738;
	
	public static final float ENEMY_X_OFFSET = 64;
	public static final float ENEMY_Y_OFFSET = 32;
	public static final float ENEMY_X_SEP = 128;
	
	private Background background;
	private Image livesImage;
	
	private static World world;
	public static World getInstance() {
		if (world == null) {
			world = new World();
		}
		return world;
	}
	
	private ArrayList<Sprite> sprites = new ArrayList<>();
	private Player player;
	
	public void addSprite(Sprite sprite) {
		sprites.add(sprite);
	}
	
	public World() {
		// Create background and other necessary images
		try {
			background = new Background("res/space.png");
			livesImage = new Image(LIVES_PATH);
		} catch (SlickException e) {
			e.printStackTrace();
		}
		
		// Create player
		player = new Player();
		
		// Create sprites
		for (int i = 0; i < App.SCREEN_WIDTH; i += ENEMY_X_SEP) {
			sprites.add(new BasicEnemy(ENEMY_X_OFFSET + i, ENEMY_Y_OFFSET));
		}
		sprites.add(player);
		
		world = this;
	}
	
	public void update(Input input, int delta) {
		
		// Update the background
		background.update(input, delta);
		
		// Update all sprites
		for (int i = 0; i < sprites.size(); ++i) {
			sprites.get(i).update(input, delta);
		}
		// Handle collisions
		for (int i = 0; i < sprites.size(); i++) {
			for (int j = 0; j < sprites.size(); j++) {
				Sprite sprite = sprites.get(i);
				Sprite other = sprites.get(j);
				if (sprite != other && sprite.getBoundingBox().intersects(other.getBoundingBox())) {
					sprite.contactSprite(other);
				}
			}
		}
		// Clean up inactive sprites
		for (int i = 0; i < sprites.size(); ++i) {
			if (sprites.get(i).getActive() == false) {
				sprites.remove(i);
				// decrement counter to make sure we don't miss any
				--i;
			}
		}
		
	}
	
	public void render(Graphics g) {
		
		// Draw the background
		background.render();
		
		// Draw all sprites
		for (Sprite sprite : sprites) {
			sprite.render();
		}
		
		// Display player information
		for (int i = 0; i < player.getLives(); i++) {
			livesImage.draw(LIVES_X_OFFSET + i*LIVES_X_SEP, LIVES_Y_OFFSET);
		}
		g.drawString("Score:" + player.getScore(), SCORE_X_OFFSET, SCORE_Y_OFFSET);
	}
	
}
