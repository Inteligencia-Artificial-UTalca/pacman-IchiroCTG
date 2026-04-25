#ifndef DTGHOSTCONTROLLER_H_
#define DTGHOSTCONTROLLER_H_

#include "Controller.h"

class DTGhostController : public Controller {
public:
    DTGhostController(std::shared_ptr<Character> character);
    virtual ~DTGhostController();
    virtual Move getMove(const GameState& game) override;
};

#endif /* DTGHOSTCONTROLLER_H_ */