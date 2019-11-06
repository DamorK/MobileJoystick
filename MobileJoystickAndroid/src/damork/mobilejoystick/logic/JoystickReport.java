package damork.mobilejoystick.logic;

public class JoystickReport
{
	private JoystickPosition joyPosition;
	private JoystickButtons joyButtons;
	private float xmin, xmax, xcenter;
	private float ymin, ymax, ycenter;
	
	public JoystickReport(
			JoystickPosition pos, JoystickButtons btn,
			float xmin, float xmax, float xcenter,
			float ymin, float ymax, float ycenter)
	{
		this.joyPosition = pos;
		this.joyButtons = btn;
		this.xmin = xmin;
		this.xmax = xmax;
		this.xcenter = xcenter;
		this.ymin = ymin;
		this.ymax = ymax;
		this.ycenter = ycenter;
	}
	
	public byte[] prepareReport()
	{
		float x;
		float y;
		
		synchronized (joyPosition) 
		{
			x = joyPosition.x();
			y = joyPosition.y();
		}
		
		x = normalize(x, xmin, xmax, xcenter);
		y = normalize(y, ymin, ymax, ycenter);
		
		byte[] ret = new byte[3];
		ret[0] = (byte) Utils.clamp((int) (x * 127), -127, 127);
		ret[1] = (byte) Utils.clamp((int) (y * 127), -127, 127);		
		ret[2] = joyButtons.buttonsState();
		return ret;
	}
	
	private float normalize(float val, float min, float max, float center)
	{
		if (val >= center)	
			return (val - center) / (max - center);
		else
			return (val - center) / (center - min);
	}
}
