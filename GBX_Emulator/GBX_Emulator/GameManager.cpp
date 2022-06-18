#include "GameManager.h"

GameManager::GameManager(MyDirectXSystem *myDirectXSystem, Key *key/*, LPD3DXSPRITE sprite*/) :
	myDirectXSystem(myDirectXSystem), key(key)//, sprite(sprite)
{
}

GameManager::~GameManager()
{
	delete gbx;
}

/*
�Q�[���̏���������
*/
void GameManager::execute_game_process()
{
	if (Main::ready_read_ROM_flag == true) {
		delete gbx;

		//M_debug_printf("Main::ROMFilePath = %s\n", Main::ROMFilePath);
		gbx = new GBX(Main::ROMFilePath, key);

		Main::ready_read_ROM_flag = false;
	}

	if (gbx != nullptr) {
		if (gbx->get_FATAL_ERROR_FLAG() == true) {
			delete gbx;
			gbx = nullptr;

			Main::ROM_loaded_flag = false;
		}
	}

	if (gbx != nullptr) {
		gbx->execute_all(myDirectXSystem);
	}
	else {
		MyDirectXDraw::draw_box_leftup(myDirectXSystem, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0xFF000000);
	}
}
