#ifndef USGHOSTCONTROLLER_H_
#define USGHOSTCONTROLLER_H_

#include "Controller.h"
using namespace std;

class USGhostController: public Controller {
	Move AcercarsePoint(const GameState& juego, pair<int,int> objetivo)const;
	Move AlejarsePoint(const GameState& juego, pair<int,int> objetivo)const;
	float CalcularDistancia(pair<int,int> PersonajeCordenadas, pair<int,int> ObjetivoCordenadas)const;
public:
	USGhostController(shared_ptr<Character> character);
	virtual ~USGhostController();
	virtual Move getMove(const GameState& game)override;
};

#endif /* USGHOSTCONTROLLER_H_ */
