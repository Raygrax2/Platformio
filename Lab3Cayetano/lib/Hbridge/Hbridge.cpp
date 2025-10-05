#include "Hbridge.h"
#include <math.h>

HBridge::HBridge()
{
	_threshold = 1.0f;
}

void HBridge::setup(uint8_t pinA, uint8_t chA, uint8_t pinB, uint8_t chB, TimerConfig *timer_config, float threshold)
{
	_pwmA.setup(pinA, chA, timer_config, false);
	_pwmB.setup(pinB, chB, timer_config, false);
	_threshold = fabsf(threshold);
}

void HBridge::setSpeed(float pwm_signed_percentage)
{
	// Clamp input to -100..100
	if (pwm_signed_percentage > 100.0f)
		pwm_signed_percentage = 100.0f;
	if (pwm_signed_percentage < -100.0f)
		pwm_signed_percentage = -100.0f;

	float abs_val = fabsf(pwm_signed_percentage);

	if (abs_val <= _threshold)
	{
		setMode(STOP);
		return;
	}

	if (pwm_signed_percentage > 0)
	{
		setMode(FORWARD, abs_val);
	}
	else
	{
		setMode(BACKWARD, abs_val);
	}
}

void HBridge::setMode(Mode mode, float duty)
{
	switch (mode)
	{
	case FORWARD:
		_pwmA.setDuty(duty);
		_pwmB.setDuty(0.0f);
		break;
	case BACKWARD:
		_pwmB.setDuty(duty);
		_pwmA.setDuty(0.0f);
		break;
	case STOP:
		// Let motor coast: both PWM 0
		_pwmA.setDuty(0.0f);
		_pwmB.setDuty(0.0f);
		break;
	case BRAKE:
		// Hard brake by forcing both sides active at same duty (or full duty)
		// Using 100% duty for braking unless a specific duty provided
		if (duty <= 0.0f)
			duty = 100.0f;
		_pwmA.setDuty(duty);
		_pwmB.setDuty(duty);
		break;
	}
}