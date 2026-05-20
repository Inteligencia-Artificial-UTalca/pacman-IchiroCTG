#pragma once

#include "Controller.h"
#include "FSM.h"

class SueStateMachine;

class SueController: public Controller {
	std::shared_ptr<SueStateMachine> fsm;
public:
	SueController(std::shared_ptr<Character> character);
	virtual ~SueController();
	virtual Move getMove(const GameState& game)override;
};

// Transiciones

	//Distancia entre pacman y Sue

class SuePacmanDistanceTransition: public FSMTransition{ 
	std::shared_ptr<FSMState> _next;

	public:
	SuePacmanDistanceTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;

};
	
	//Distancia entre Sue y las SuperPills

class SuePowerPillDistanceTransition: public FSMTransition{
	std::shared_ptr<FSMState> _next;

	public:
	SuePowerPillDistanceTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

	//PowerPill Consumida

class SuePowerPillTransition: public FSMTransition{
	std::shared_ptr<FSMState> _next;
	
	public:
	SuePowerPillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;

};

	//PowerPill Terminada
class SueEndPowerPillTransition: public FSMTransition{
	std::shared_ptr<FSMState> _next;
	
	public:
	SueEndPowerPillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};
//Estados
	//Chase
class SueChaseState : public FSMState{

	public: 
	
	SueChaseState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~SueChaseState();
	
};
	//Feel
class SueFeelState : public FSMState{

	public:

	SueFeelState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~SueFeelState();
	
};
	// Frightened
class SueFrightenedState : public FSMState{

	public: 

	SueFrightenedState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~SueFrightenedState();
	
};
//Maquina de Estados

class SueStateMachine : public FiniteStateMachine{

	public:
	SueStateMachine(std::shared_ptr<Character> _character);
	Move update(const GameState& gs) override;
	~SueStateMachine();
};