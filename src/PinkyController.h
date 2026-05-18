/*
 * PinkyController.h
 */

#ifndef PINKYCONTROLLER_H_
#define PINKYCONTROLLER_H_

#include "Controller.h"
#include "BehaviorTree.h"
#include <chrono>
#include <random>



class PinkyController : public Controller {
private:
    std::shared_ptr<Composite> root;
public:
    PinkyController(std::shared_ptr<Character> character);
    virtual ~PinkyController();
    virtual Move getMove(const GameState& gs) override;
};


// Condicion para cambiar a Huir
class PinkyPowerpill : public Behavior {
public:
    virtual Status update() override;
};

// Ciclo de Acciones (Tiempo para cada una) cuando Pinky no esta asustad@
class PinkyTimeOut : public Behavior {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
    PinkyTimeOut();
    virtual Status update() override;
};


//  Huir 
class PinkyFrightened : public Behavior {
private:
    std::mt19937 e;
    std::uniform_int_distribution<int> uniform_dist;

public:
    virtual Status update() override;
    PinkyFrightened();
};

// Dispersion
class PinkyScatter : public Behavior {
private:
    std::pair<int,int> target;
public:
    PinkyScatter();
    virtual Status update() override;
};


// Persecucion
class PinkyChase : public Behavior {
public:
    virtual Status update() override;
};

#endif /* PINKYCONTROLLER_H_ */