#ifndef _BRAIN_H_
#define _BRAIN_H_

enum OutputNeurons // TODO: decide if this is necessary or just use an int of numOutputNuerons
{
	MOVE_SPEED = 0,
	TURN_SPEED,

	//----------------

	NUM_OUTPUT_NUERONS
};

class Brain
{
public:
	Brain();
	~Brain();

private:
};


#endif // _BRAIN_H_