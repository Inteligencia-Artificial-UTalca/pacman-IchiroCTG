/*
 * PinkyController.h
 */

#ifndef PINKYCONTROLLER_H_
#define PINKYCONTROLLER_H_

#include "Controller.h"
#include "BehaviorTree.h"
#include <chrono>



class PinkyController : public Controller {
private:
    std::shared_ptr<Composite> root;
public:
    PinkyController(std::shared_ptr<Character> character);
    virtual ~PinkyController();
    virtual Move getMove(const GameState& gs) override;
};


// condicion: ¿el fantasma es comestible?
class PinkyPowerpill : public Behavior {
public:
    virtual Status update() override;
};

// Asignacion de ciclo de 27s, primeros 7 → Scatter
class PinkyTimeOut : public Behavior {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
    PinkyTimeOut();
    virtual Status update() override;
};


//  movimiento aleatorio al huir
class PinkyFrightened : public Behavior {
public:
    virtual Status update() override;
};

// Ir a la esquina de Pinky
class PinkyScatter : public Behavior {
private:
    std::pair<int,int> target;
public:
    PinkyScatter();
    virtual Status update() override;
};


// Moverse a 4 casillas ADELANTE de Pacman
class PinkyChase : public Behavior {
public:
    virtual Status update() override;
};

#endif /* PINKYCONTROLLER_H_ */