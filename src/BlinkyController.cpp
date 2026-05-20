#include "BlinkyController.h"
#include <iostream>

BlinkyController::BlinkyController(std::shared_ptr<Character> character):
	Controller(character),
	fsm(std::make_shared<BlinkyFMStateMachine>(character)) {
}

BlinkyController::~BlinkyController() {
	
}

Move BlinkyController::getMove(const GameState& game){
	return fsm->update(game);
}


//Estados

	//Chase
BlinkyChaseState::BlinkyChaseState(std::shared_ptr<Character> _character):FSMState(_character){

}
void BlinkyChaseState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}
Move BlinkyChaseState::onUpdate(const GameState& game){
	
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

BlinkyChaseState::~BlinkyChaseState() {
	
}	
	// Scatter
BlinkyScatterState::BlinkyScatterState(std::shared_ptr<Character> _character):FSMState(_character){

}
void BlinkyScatterState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}
Move BlinkyScatterState::onUpdate(const GameState& game){
	
	std::vector<Move> moves;
	const auto pacmanCoord=game.getMaze().getPowerPillPositions()[0];
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

BlinkyScatterState::~BlinkyScatterState() {
	
}
	//Frightened
BlinkyFrightenedState::BlinkyFrightenedState(std::shared_ptr<Character> _character):FSMState(_character){

}
void BlinkyFrightenedState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}
Move BlinkyFrightenedState::onUpdate(const GameState& game){
	std::vector<Move> moves;
	const auto myPos=character->getPos();
	//const auto myCoord=game.getMaze().getNodePos(myPos);

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	return moves[rand()%moves.size()];
}

BlinkyFrightenedState::~BlinkyFrightenedState() {
	
}

//Trancisiones

	// Pildora Transition
BlinkyPillTransition::BlinkyPillTransition(std::shared_ptr<FSMState> next):_next(next){

}
bool BlinkyPillTransition::isValid(const GameState& gs){
	static int last = 0;
	int quedan=gs.getMaze().getPillPositions().size();
	if(last == 0) { last = quedan; return false; } 
	if(last!=quedan && quedan%20==0 && quedan>0){
		last =quedan;
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> BlinkyPillTransition::getNextState(){

	return _next;
}
	// PowerPill Transition
BlinkyPowerPillTransition::BlinkyPowerPillTransition(std::shared_ptr<FSMState> next):_next(next){

}
bool BlinkyPowerPillTransition::isValid(const GameState& gs){
	return gs.isGhostEdible(0);
}
std::shared_ptr<FSMState> BlinkyPowerPillTransition::getNextState(){
	return _next;
}
	// EndPowerPill Transition
BlinkyEndPowerPillTransition::BlinkyEndPowerPillTransition(std::shared_ptr<FSMState> next): _next(next){

}
bool BlinkyEndPowerPillTransition::isValid(const GameState& gs){
	return !gs.isGhostEdible(0);
}
std::shared_ptr<FSMState> BlinkyEndPowerPillTransition::getNextState(){
	return _next;
}

// State Machine

BlinkyFMStateMachine::BlinkyFMStateMachine(std::shared_ptr<Character> _character):
	FiniteStateMachine(_character){

	initialState = std::make_shared<BlinkyChaseState>(_character);
	auto dispersar = std::make_shared<BlinkyScatterState>(_character);
	auto asustado = std::make_shared<BlinkyFrightenedState>(_character);
	activeState = initialState;
	states.push_back(initialState);
	states.push_back(dispersar);
	states.push_back(asustado);
	initialState->addTransition(std::make_shared<BlinkyPillTransition>(dispersar));
	initialState->addTransition(std::make_shared<BlinkyPowerPillTransition>(asustado));
	dispersar->addTransition(std::make_shared<BlinkyPillTransition>(initialState));
	dispersar->addTransition(std::make_shared<BlinkyPowerPillTransition>(asustado));
	asustado->addTransition(std::make_shared<BlinkyEndPowerPillTransition>(initialState));
}
Move BlinkyFMStateMachine::update(const GameState& gs){
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}
BlinkyFMStateMachine::~BlinkyFMStateMachine() {
	
}