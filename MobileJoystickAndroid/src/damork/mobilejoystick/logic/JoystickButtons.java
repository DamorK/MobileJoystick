package damork.mobilejoystick.logic;

public class JoystickButtons
{
	private byte buttonsState;

	public synchronized void setButton(int button, boolean state)
	{
		if (state)
			buttonsState |= (1 << button);
		else
			buttonsState &= ~(1 << button);
	}
	
	public synchronized void clear()
	{
		buttonsState = 0;
	}
	
	public synchronized void setButtons(byte state)
	{
		buttonsState = state;
	}
	
	public synchronized byte buttonsState()
	{
		return buttonsState;
	}
}
