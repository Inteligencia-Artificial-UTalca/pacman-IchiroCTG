#include "USGhostController.h"
#include "Ghost.h"
#include <SDL2/SDL.h>


// Personalidad Acosador

using namespace std;

USGhostController::USGhostController(shared_ptr<Character> character):
	Controller(character){
}

USGhostController::~USGhostController(){}

Move USGhostController::AcercarsePoint(const GameState& game, pair<int,int> target)const{
	int minDist=100000;
	Move minMove=character->getDirection();
	vector<Move> moves=game.getMaze().getGhostLegalMoves(character->getPos(),character->getDirection());

    bool Valido=false;
    for(auto movimiento: moves){
        if(movimiento != PASS){
            Valido=true;
            break;
        }
    }

    if(!Valido){
        moves = game.getMaze().getPossibleMoves(character->getPos());
    }

	for(Move m:moves){
		int vecino = game.getMaze().getNeighbour(character->getPos(),m);
		if(vecino<0)continue;
		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int sqDist=euclid2(vecinoCoords,target);
		if(sqDist<minDist){
			minDist=sqDist;
			minMove=m;
		}
	}
	return minMove;	
}

Move USGhostController::AlejarsePoint(const GameState& game, pair<int,int> target)const{
	int maxDist=-1;
	Move maxMove=character->getDirection();
	
    vector<Move> moves=game.getMaze().getGhostLegalMoves(character->getPos(),character->getDirection());

    bool Valido=false;
    for(auto movimiento: moves){
        if(movimiento != PASS){
            Valido=true;
            break;
        }
    }

    if(!Valido){
        moves = game.getMaze().getPossibleMoves(character->getPos());
    }

	for(Move m:moves){
		int vecino = game.getMaze().getNeighbour(character->getPos(),m);
		if(vecino<0)continue;
		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int sqDist=euclid2(vecinoCoords,target);
		if(sqDist>maxDist){
			maxDist=sqDist;
			maxMove=m;
		}
	}
	return maxMove;
}

float USGhostController::CalcularDistancia(pair<int,int> PersonajeCordenadas, pair<int,int>ObjetivoCordenadas)const{

	return sqrt(euclid2(PersonajeCordenadas,ObjetivoCordenadas)); // Calcula la distancia eucladiana entre pacman y el punto indicado

}

Move USGhostController::getMove(const GameState& game){


    auto PacmanCoord = game.getMaze().getNodePos(game.getPacmanPos());
    auto FantasmaCoord = game.getMaze().getNodePos(character->getPos());
    auto Fantasma = dynamic_cast<Ghost*>(character.get());
    auto InicioFantasma = game.getMaze().getNodePos(game.getMaze().getGhostStart()[0]);


    bool isEdible=false;
    if(Fantasma!=nullptr){
        
        isEdible = Fantasma -> isEdible(); 
    }

    float DistaciaPacman = CalcularDistancia(FantasmaCoord,PacmanCoord);  

    //Perseguir
    float UtilidadChasear = 0.0f;
    Move MovimientoChasear = PASS;
    
    if(!isEdible){
       
        UtilidadChasear = pow(120.0f - DistaciaPacman, 2) / pow(120.0f, 2);
        if(DistaciaPacman>120.0f){UtilidadChasear=0.0f;}
        MovimientoChasear = AcercarsePoint(game,PacmanCoord);
    }
    // Huir
    float UtilidadCorrer = 0.0f;
    Move MovimientoCorrer = PASS;

    if(isEdible){
        UtilidadCorrer = 1.0f - 1.0f / (1.0f + pow(2.718f * 0.45f, - DistaciaPacman + 40.0f));
        MovimientoCorrer = AlejarsePoint(game,PacmanCoord);
    }

    // Reagruparse
    float UtilidadReagrupacion = 0.0f;
    Move MovimientoReagrupacion = PASS;

    if(!isEdible){

        UtilidadReagrupacion = DistaciaPacman/150;
        MovimientoReagrupacion = AcercarsePoint(game, InicioFantasma);
    }

    std::cerr << "Chasear =" << UtilidadChasear 
          << " Correr =" << UtilidadCorrer 
          << " Reagrupar =" << UtilidadReagrupacion << "\n";

    float MayorUtilidad=0.0f;
	Move MejorMovimiento=PASS;
	if(MayorUtilidad < UtilidadChasear) {
		MayorUtilidad = UtilidadChasear;
		MejorMovimiento= MovimientoChasear;
	}
	if(MayorUtilidad < UtilidadCorrer){
		MayorUtilidad = UtilidadCorrer;
		MejorMovimiento = MovimientoCorrer;
	}
	if(MayorUtilidad < UtilidadReagrupacion){
		MayorUtilidad = UtilidadReagrupacion;
		MejorMovimiento = MovimientoReagrupacion;
	}

	return MejorMovimiento;

}