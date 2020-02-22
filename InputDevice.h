#ifndef __INPUT_DEVICE_H__
#define __INPUT_DEVICE_H__


enum KeyState {
	KEY_IDLE,
	KEY_PRESSED,
	KEY_RELEASED,
	KEY_MANTAINED
};

struct InputKey {
	KeyState state = KEY_IDLE;
};

struct Mouse {
	KeyState state;
};


class InputDevice
{
public:
	InputDevice();
	virtual ~InputDevice();

private:
	InputKey * keys;
};

#endif