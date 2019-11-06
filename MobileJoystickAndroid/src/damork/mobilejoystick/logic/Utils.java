package damork.mobilejoystick.logic;

public class Utils 
{
	public static float clamp(float val, float min, float max)
	{
		return Math.min(max, Math.max(min, val));
	}
	
	public static int clamp(int val, int min, int max)
	{
		return Math.min(max, Math.max(min, val));
	}
	
	public static float lerp(float a, float b, float t)
	{
		return a * (1.0f - t) + b * t;
	}
	
	public static float length(float[] vec)
	{
		float len = 0.0f;
		for (int i = 0; i < vec.length; i++)
			len += vec[i] * vec[i];
		return (float) Math.sqrt(len);
	}
	
	public static void normalize(float[] vec)
	{
		float len = length(vec);
		if (len > 0.0f)
		{
			len = 1.0f / len;
			for (int i = 0; i < vec.length; i++)
				vec[i] *= len;
		}				
	}
}
