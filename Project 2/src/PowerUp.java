import org.newdawn.slick.Input;

public class PowerUp {
	
	private String type;
	private int duration;
	private boolean active = true;
	
	// Create a powerup without a defined duration yet
	public PowerUp(String type) {
		this.type = type;
	}
	
	public PowerUp(String type, int duration) {
		this.type = type;
		this.duration = duration;
	}
	
	public void update(Input input, int delta) {
		if (duration > 0) {
			duration -= delta;
			
		} else {
			active = false;
		}
	}
	
	public String getType() { return type; }
	public boolean getActive() { return active; }
	
}
