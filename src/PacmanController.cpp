#include "PacmanController.h"
#include <SDL2/SDL.h>

#define MAX 10000000

using namespace std;

PacmanController::PacmanController(std::shared_ptr<Character> character):
	Controller(character){
}

PacmanController::~PacmanController() {

}

Move PacmanController::AcercarsePoint(const GameState& game, std::pair<int,int> target)const{
	int minDist=MAX;
	Move minMove=character->getDirection();
	std::vector<Move> moves=game.getMaze().getPossibleMoves(character->getPos());
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

Move PacmanController::AlejarsePoint(const GameState& game, std::pair<int,int> target)const{
	int maxDist=-1;
	Move maxMove=character->getDirection();
	std::vector<Move> moves=game.getMaze().getPossibleMoves(character->getPos());
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

float PacmanController::CalcularDistancia(pair<int,int> PacmanCords, pair<int,int>PointCords)const{

	return sqrt(euclid2(PacmanCords,PointCords)); // Calcula la distancia eucladiana entre pacman y el punto indicado

}


Move
PacmanController::getMove(const GameState& game){

	//para cerrar la ventana
	SDL_Event e;
	if( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT || 
			(e.type == SDL_KEYDOWN && 
				(e.key.keysym.sym==SDLK_ESCAPE || 
				e.key.keysym.sym==SDLK_q) ))
		{
			SDL_Quit();
			exit(0);
		}
	}

	//Recolectar Información del mapa

	int pacmanNode = character->getPos();
	auto pacmanCoords = game.getMaze().getNodePos(pacmanNode); // Cordenadas de Pacman en el mapa
	
	std::vector<std::pair<int,int>> ghostPositions;
	for(int i=0;i<4;i++){
		ghostPositions.push_back(game.getMaze().getNodePos(game.getGhostsPos(i)));
	}

	std::vector<bool> ghostsEdible;
	for(int i=0;i<4;i++){
		ghostsEdible.push_back(game.isGhostEdible(i));
	}

	auto powerPillPositions=game.getMaze().getPowerPillPositions();	
	auto pillPositions=game.getMaze().getPillPositions();
//Calculo de Distancias

	//Calcular Distancia del Fantasma Mas cercano
	float DistanciaFantasmaMasCercano = MAX;

	for(int i=0;i<4;i++){
		if(!ghostsEdible[i]){
			float DistanciaFantasma = CalcularDistancia(pacmanCoords, ghostPositions[i]);
			if(DistanciaFantasmaMasCercano>DistanciaFantasma){
				DistanciaFantasmaMasCercano=DistanciaFantasma;
			}
		}
	}

	// Calcular Distacia de la SuperPastilla Mas Cercana

	float DistanciaSuperpastillaMasCercana = MAX;
	pair<int,int> CordenadaPildoraCercana;
	for(long unsigned int i=0;i<powerPillPositions.size();i++){
		float DistanciaPildora = CalcularDistancia(pacmanCoords,powerPillPositions[i]);
		if(DistanciaSuperpastillaMasCercana>DistanciaPildora){
			DistanciaSuperpastillaMasCercana=DistanciaPildora;
			CordenadaPildoraCercana= powerPillPositions[i];
		}
	}
	//Calcular Distancia de la Pildora Mas Cercana
	float DistanciaPildoraMasCercana = MAX;
	pair<int,int> CordenadaPildoraMasCercana;
	for(long unsigned int i=0;i<pillPositions.size();i++){
		float DistanciaPildora = CalcularDistancia(pacmanCoords,pillPositions[i]);
		if(DistanciaPildoraMasCercana>DistanciaPildora){
			DistanciaPildoraMasCercana=DistanciaPildora;
			CordenadaPildoraMasCercana= pillPositions[i];
		}
	}
	
// Calculo de Utilidades

	// Calcular Utilidad de Ir A SuperPastilla
	float UtilidadSuperPastilla= 0.0f; 
	Move MovimientoSuperPastilla=PASS;
	if(DistanciaFantasmaMasCercano<MAX){
		UtilidadSuperPastilla = (DistanciaFantasmaMasCercano-DistanciaSuperpastillaMasCercana)/DistanciaFantasmaMasCercano;
		if(UtilidadSuperPastilla<0) UtilidadSuperPastilla= 0.0f;
		MovimientoSuperPastilla = AcercarsePoint(game, CordenadaPildoraCercana);
	}
	
	//Calcular Utilidad de Ir a Pildora Normal
	float UtilidadPildora= 0.0f;
	Move MovimientoPildora=PASS;
	if(DistanciaFantasmaMasCercano<MAX){
		float Seguridad = 1.0f/(1.0f + pow(2.718f *0.45f, -DistanciaFantasmaMasCercano+20.0f)); //logistica Inversa
		float AtractivoPildora = 1.0f/(1.0f + DistanciaPildoraMasCercana); // Atractivo de la pildora, entre mas cerca este mas atractivo para evitar que por la distancia del fantasma se dirija a una pildora lejana
		UtilidadPildora = Seguridad * AtractivoPildora;
		if(UtilidadPildora<0) UtilidadPildora=0.0f;
		MovimientoPildora = AcercarsePoint(game, CordenadaPildoraMasCercana);
	}

	// Calcular Utilidad de Huir
	float UtilidadHuir = 0.0f;
	Move MovimientoHuir=PASS;

	for (int i=0;i<4;i++){
		if(!ghostsEdible[i]){
			float DistanciaFantasma = CalcularDistancia(pacmanCoords, ghostPositions[i]);
			float UtilidadTemporal = 1.0f-1.0f/(1.0f+pow(2.718f * 0.45f,-DistanciaFantasma+20.0f));//logistica
			if(UtilidadTemporal>UtilidadHuir){
				UtilidadHuir=UtilidadTemporal;
				MovimientoHuir = AlejarsePoint(game, ghostPositions[i]);
			}
		}
	}

	// Calcular Utilidad Perseguir Fantasma Azul

	float UtilidadPerseguir = 0.0f;
	Move MovimientoPerseguir =  PASS;
	for (int i=0;i<4;i++){
		if(ghostsEdible[i]){
			float DistanciaFantasma = CalcularDistancia(pacmanCoords, ghostPositions[i]);
			float UtilidadTemporal=pow(80.0f-DistanciaFantasma,2)/pow(80.0,2);//cuadratica
			if(UtilidadTemporal>UtilidadPerseguir){
				UtilidadPerseguir = UtilidadTemporal;
				MovimientoPerseguir = AcercarsePoint(game,ghostPositions[i]);
			}
		}
	}

	// Seleccionar Mejor Movimiento 
	/*std::cerr << "Huir=" << UtilidadHuir << " Pastilla=" << UtilidadSuperPastilla 
    << " Perseguir=" << UtilidadPerseguir << "\n" << "ComerPildora=" << UtilidadPildora << "\n";*/
	
	float MayorUtilidad=0.0f;
	Move MejorMovimiento=PASS;
	if(MayorUtilidad < UtilidadHuir) {
		MayorUtilidad = UtilidadHuir;
		MejorMovimiento= MovimientoHuir;
	}
	if(MayorUtilidad < UtilidadPerseguir){
		MayorUtilidad = UtilidadPerseguir;
		MejorMovimiento = MovimientoPerseguir;
	}
	if(MayorUtilidad < UtilidadSuperPastilla){
		MayorUtilidad = UtilidadSuperPastilla;
		MejorMovimiento = MovimientoSuperPastilla;
	}
	if(MayorUtilidad < UtilidadPildora){
		MayorUtilidad = UtilidadPildora;
		MejorMovimiento = MovimientoPildora;
	}

	return MejorMovimiento;
}
