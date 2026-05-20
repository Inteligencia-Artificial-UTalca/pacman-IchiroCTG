#pragma once

#include "Controller.h"
#include "FSM.h"

class BlinkyFMStateMachine;

class BlinkyController:public Controller{
	std::shared_ptr<BlinkyFMStateMachine> fsm;
public:
	BlinkyController(std::shared_ptr<Character> character);
	virtual ~BlinkyController();
	virtual Move getMove(const GameState& game)override;
};


// Trancisiones

	//Pildoras
class BlinkyPillTransition: public FSMTransition{
	std::shared_ptr<FSMState> _next;

	public:
	BlinkyPillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

	// PowerPill
class BlinkyPowerPillTransition: public FSMTransition{
	std::shared_ptr<FSMState> _next;

	public:
	BlinkyPowerPillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

	//EndPowerPill
class BlinkyEndPowerPillTransition: public FSMTransition{
	std::shared_ptr<FSMState> _next;

	public:
	BlinkyEndPowerPillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};	

// Estados

	//Chase
class BlinkyChaseState : public FSMState{
	
	public:
		BlinkyChaseState(std::shared_ptr<Character> _character);
		Move onUpdate(const GameState& gs) override;
		void onEnter(const GameState& gs) override;
		~BlinkyChaseState();
};
	// Scatter
class BlinkyScatterState : public FSMState{
	
	public:
		BlinkyScatterState(std::shared_ptr<Character> _character);
		Move onUpdate(const GameState& gs) override;
		void onEnter(const GameState& gs) override;
		~BlinkyScatterState();
};
	//Frightened
class BlinkyFrightenedState : public FSMState{
	
	public:
		BlinkyFrightenedState(std::shared_ptr<Character> _character);
		Move onUpdate(const GameState& gs) override;
		void onEnter(const GameState& gs) override;
		~BlinkyFrightenedState();
};
// Maquina de Estados

class BlinkyFMStateMachine: public FiniteStateMachine{

public:
	BlinkyFMStateMachine(std::shared_ptr<Character> _character);
	Move update(const GameState& gs) override;
	~BlinkyFMStateMachine();
};

