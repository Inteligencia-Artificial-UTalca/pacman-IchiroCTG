#include "SueController.h"
#include <iostream>

SueController::SueController(std::shared_ptr<Character> character):
	Controller(character),
	fsm(std::make_shared<SueStateMachine>(character)) {
}

SueController::~SueController() {}

Move SueController::getMove(const GameState& game){
	return fsm->update(game);
}

// Estados
	//Chase
SueChaseState::SueChaseState(std::shared_ptr<Character> _character):FSMState(_character){}

void SueChaseState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move SueChaseState::onUpdate(const GameState& game){
	std::vector<Move> moves;
	const auto pacmanCoord=game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos=character->getPos();
	//const auto myCoord=game.getMaze().getNodePos(myPos);

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			pacmanCoord);
	int minI=0;
	for(unsigned int i=1;i<moves.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[i])),
			pacmanCoord);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return moves[minI];
}
SueChaseState::~SueChaseState() {}

 //Feel
SueFeelState::SueFeelState(std::shared_ptr<Character> _character):FSMState(_character){}

void SueFeelState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move SueFeelState::onUpdate(const GameState& game){
	std::vector<Move> movimientos;
	const auto PowerPillCoord = game.getMaze().getPowerPillPositions()[3];
	const auto myPos = character->getPos();

	if(character->getDirection()==PASS){
		movimientos=game.getMaze().getPossibleMoves(myPos);
	}else{
		movimientos=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,movimientos[0])),
			PowerPillCoord);
	int minI=0;
	for(unsigned int i=1;i<movimientos.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,movimientos[i])),
			PowerPillCoord);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return movimientos[minI];
}
SueFeelState::~SueFeelState() {}
	// Frightened
SueFrightenedState::SueFrightenedState(std::shared_ptr<Character> _character):FSMState(_character){}

void SueFrightenedState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move SueFrightenedState::onUpdate(const GameState& game){
	
	std::vector<Move> movimientos;
	const auto PowerPillCoord = game.getMaze().getPowerPillPositions()[0];
	const auto myPos = character->getPos();

	if(character->getDirection()==PASS){
		movimientos=game.getMaze().getPossibleMoves(myPos);
	}else{
		movimientos=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,movimientos[0])),
			PowerPillCoord);
	int minI=0;
	for(unsigned int i=1;i<movimientos.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,movimientos[i])),
			PowerPillCoord);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return movimientos[minI];
}
SueFrightenedState::~SueFrightenedState() {}

// Trancisiones

	//Distancia entre Sue y Pacman
SuePacmanDistanceTransition::SuePacmanDistanceTransition(std::shared_ptr<FSMState> next):_next(next){}

bool SuePacmanDistanceTransition::isValid(const GameState& gs){
	auto pacmanCoord=gs.getMaze().getNodePos(gs.getPacmanPos());
	auto myCoord=gs.getMaze().getNodePos(gs.getGhostsPos(3));
	return euclid2(pacmanCoord,myCoord)<5;
}

std::shared_ptr<FSMState> SuePacmanDistanceTransition::getNextState(){
	return _next;
}
	//Distancia entre Sue y los SuperPills
SuePowerPillDistanceTransition::SuePowerPillDistanceTransition(std::shared_ptr<FSMState> next):_next(next){}

bool SuePowerPillDistanceTransition::isValid(const GameState& gs){
	auto PowerPillCoord=gs.getMaze().getPowerPillPositions()[3];
	auto myCoord=gs.getMaze().getNodePos(gs.getGhostsPos(3));
	return euclid2(PowerPillCoord,myCoord)<3;
} 

std::shared_ptr<FSMState> SuePowerPillDistanceTransition::getNextState(){
	return _next;
}
	//PowerPill Consumida

SuePowerPillTransition::SuePowerPillTransition(std::shared_ptr<FSMState> next):_next(next){}

bool SuePowerPillTransition::isValid(const GameState& gs){
	return gs.isGhostEdible(3);
}
std::shared_ptr<FSMState> SuePowerPillTransition::getNextState(){
	return _next;
}

	//PowerPill Terminada

SueEndPowerPillTransition::SueEndPowerPillTransition(std::shared_ptr<FSMState> next):_next(next){}

bool SueEndPowerPillTransition::isValid(const GameState& gs){
	return !gs.isGhostEdible(3);	
}

std::shared_ptr<FSMState> SueEndPowerPillTransition::getNextState(){
	return _next;
}

// Maquina de Estados

SueStateMachine::SueStateMachine(std::shared_ptr<Character> _character):
	FiniteStateMachine(_character){
	
	initialState=std::make_shared<SueChaseState>(_character);
	auto alejarse = std::make_shared<SueFeelState>(_character);
	auto asustado = std::make_shared<SueFrightenedState>(_character);

	activeState = initialState;
	states.push_back(initialState);
	states.push_back(alejarse);
	states.push_back(asustado);

	initialState->addTransition(std::make_shared<SuePacmanDistanceTransition>(alejarse));
	initialState->addTransition(std::make_shared<SuePowerPillTransition>(asustado));
	alejarse->addTransition(std::make_shared<SuePowerPillDistanceTransition>(initialState));
	alejarse->addTransition(std::make_shared<SuePowerPillTransition>(asustado));
	asustado->addTransition(std::make_shared<SueEndPowerPillTransition>(initialState));
}

Move SueStateMachine::update(const GameState& gs){
	auto transition = activeState->getActiveTransition(gs);
	if(transition!=nullptr){
		activeState->onExit(gs);
		transition->onTransition(gs);
		activeState=transition->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}
SueStateMachine::~SueStateMachine() {
	
}