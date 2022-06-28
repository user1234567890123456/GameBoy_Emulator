#pragma once

#include "MyDirectXSystem.h"
#include "MyDirectXImage.h"
#include "Key.h"
#include "GameBoy.h"
#include "Main.h"
#include "MyDirectXDraw.h"

class GameManager
{
private:
	GameBoy* gbx = nullptr;

	MyDirectXSystem *myDirectXSystem;

	//LPD3DXSPRITE sprite;

	Key *key;
public:
	GameManager(MyDirectXSystem *myDirectXSystem, Key *key/*, LPD3DXSPRITE sprite*/);
	~GameManager();

	void execute_game_process();
};
