#pragma once

#include "Controller.h"
#include "FSM.h"

class SueStateMachine;

class SueController: public Controller {

public:
	SueController(std::shared_ptr<Character> character);
	virtual ~SueController();
	virtual Move getMove(const GameState& game)override;
};

// Trancisiones

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