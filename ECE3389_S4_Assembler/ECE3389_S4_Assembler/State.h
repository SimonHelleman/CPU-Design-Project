#ifndef INC_STATE_H
#define INC_STATE_H
template<class T>
class State
{
public:
	State(State<T>(T::* stateFunc)(void) = 0, T* machine = 0)
		: _stateFunction(stateFunc), _machine(machine)
	{
	}

	State<T> operator()(void)
	{
		return (_machine->*_stateFunction)();
	}

	State<T>& operator=(const State& rhs)
	{
		_stateFunction = rhs._stateFunction;
		return *this;
	}

	int operator==(State<T> rhs)
	{
		return _stateFunction == rhs._stateFunction;
	}

	int operator!=(State<T> rhs)
	{
		return !(*this == rhs);
	}

private:
	State<T>(T::* _stateFunction)(void);
	T* _machine;
};
#endif
