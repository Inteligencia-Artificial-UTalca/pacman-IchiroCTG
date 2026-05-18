#ifndef INKYCONTROLLER_H_
#define INKYCONTROLLER_H_

#pragma once

#include "Controller.h"
#include "BehaviorTree.h"
#include <chrono>

class InkyController: public Controller {
private:
	std::shared_ptr<Composite> root;
public:
	InkyController(std::shared_ptr<Character> character);
	virtual ~InkyController();
	virtual Move getMove(const GameState& game)override;
};
 // Perseguir
class InkyChase : public Behavior{
public:
    virtual Status update() override;

};
//Huir
class InkyFrightened : public Behavior{
private:
    std::uniform_int_distribution<int> uniform_dist;
public:
    virtual Status update() override;
    InkyFrightened ();

};
//Dispersar
class InkyScatter : public Behavior{
private:
    std::pair<int,int> target;

public:
    virtual Status update() override;
    InkyScatter();

};
// 	Condicion para activar el estado de Huida
class InkyPowerpill : public Behavior{
public:
    virtual Status update() override;
};


// Ciclo de Acciones de Inky cuando NO esta asustad@
class InkyTimeOut : public Behavior{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
    virtual Status update() override;
    InkyTimeOut ();
};

#endif /* INKYCONTROLLER_H_ */