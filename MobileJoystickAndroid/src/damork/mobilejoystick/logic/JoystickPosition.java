package damork.mobilejoystick.logic;

public class JoystickPosition
{
	private float x;
	private float y;
	
	public JoystickPosition(float x, float y)
	{
		this.x = x;
		this.y = y;
	}
	
	public synchronized void set(float x, float y)
	{
		this.x = x;
		this.y = y;
	}
	
	public float x()	{ return x; }
	public float y()	{ return y; }
	
	public void min(JoystickPosition p)
	{
		x = Math.min(x, p.x());
		y = Math.min(y, p.y());
	}
	
	public void max(JoystickPosition p)
	{
		x = Math.max(x, p.x());
		y = Math.max(y, p.y());
	}
	
	public synchronized JoystickPosition clone()
	{
		return new JoystickPosition(x, y);
	}
}
