#include <windows.h>
#include <tchar.h>

#include <iostream>
#include <fstream>

#include "resource.h"
#include "resource1.h"

#include "SystemDefine.h"
#include "MaterialArchiveFile.h"
#include "MyDirectXDraw.h"
#include "MyDirectXFont.h"
#include "MyDirectXSystem.h"
#include "MyDirectXImage.h"
#include "GameManager.h"
#include "Key.h"
#include "Fps.h"
#include "My_Random.h"
#include "My_Input.h"
#include "CRC.h"
#include "XOR.h"
#include "Mutex.h"
#include "Main.h"

using namespace std;

bool Main::ready_read_ROM_flag = false;
TCHAR Main::ROMFilePath[] = {0};

bool Main::ROM_loaded_flag = false;
uint8_t Main::Cartridge_Type = 0;
uint32_t Main::PGM_size = 0;
uint32_t Main::SRAM_size = 0;

bool Main::Show_FPS_Flag = true;



//int frame_process_type = 0;
//bool fullscreen_result = true;

/*
�v���g�^�C�v�錾
*/
LRESULT CALLBACK FirstWinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);


void __app_safe_exit_error__() {
	Mutex::get_instance_ptr()->delete_app_mutex();
	exit(EXIT_FAILURE);
}

void __app_safe_exit_normal__() {
	Mutex::get_instance_ptr()->delete_app_mutex();
	exit(0);
}

/*
##############################################
#��������v���O�������n�܂�
##############################################
*/
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR command, int commandShow)
{
	//���������N���ł��Ȃ��悤�ɂ���
	Mutex::Init();
	Mutex::get_instance_ptr()->create_app_mutex();

//################################################

	CRC::Init();


#ifdef GBX_EMU_DEBUG
	//if (full_result == IDNO) {//NO
//	if (fullscreen_result == false) {//NO
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONIN$", "r", stdin);

		M_debug_printf("\n");
		M_debug_printf("=======================================\n");
		M_debug_printf("GBX_Emu Debug Console\n");
		M_debug_printf("=======================================\n");
		M_debug_printf("\n");
//	}
#endif

//	M_debug_printf("###################################\n");
//	M_debug_printf("###################################\n");
//	M_debug_printf("frame_process_type = %d\n", frame_process_type);
//	M_debug_printf("fullscreen_result = %s\n", (fullscreen_result ? "true" : "false"));
//	M_debug_printf("app_exit_flag = %s\n", (app_exit_flag ? "true" : "false"));
//	M_debug_printf("###################################\n");
//	M_debug_printf("###################################\n");


	//�J�[�\����ύX����
	HCURSOR hCursor;
	hCursor = LoadCursor(NULL, IDC_ARROW);//�W�����J�[�\��
	SetCursor(hCursor);
///	//if (full_result == IDNO) {//NO
///	if (fullscreen_result == false) {//NO
///		//�J�[�\����ύX����
///		HCURSOR hCursor;
///		hCursor = LoadCursor(NULL, IDC_ARROW);//�W�����J�[�\��
///		SetCursor(hCursor);
///	}
///	else {//YES
///		ShowCursor(FALSE);//�J�[�\����\�����Ȃ�
///	}


	const TCHAR* WINDOW_DEF_STR = _T("WINDOW");
	const TCHAR* WINDOW_TITLE_STR = _T(TITLE);

	/*
	WNDCLASSEX�\���̂̐ݒ�
	*/
	WNDCLASSEX wnd_ex;
	wnd_ex.cbSize = sizeof(WNDCLASSEX);
	wnd_ex.style = CS_HREDRAW | CS_VREDRAW;
	wnd_ex.lpfnWndProc = WinProc;
	wnd_ex.cbClsExtra = 0;
	wnd_ex.cbWndExtra = 0;
	wnd_ex.hInstance = hInstance;
	wnd_ex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wnd_ex.hCursor = NULL;
	wnd_ex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd_ex.lpszMenuName = "IDR_MENU1";
	wnd_ex.lpszClassName = WINDOW_DEF_STR;
	wnd_ex.hIconSm = NULL;

	/*
	�E�C���h�E�̓o�^������
	*/
	if (RegisterClassEx(&wnd_ex) == FALSE) {
		//�G���[�����������ꍇ�v���O�������I������
		MessageBox(NULL, _T("Window�̏������̍ۂɃG���[���������܂���"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return EXIT_FAILURE;
	}

	HWND h_wnd;
	/*
	�E�C���h�E�̕`��͈͂�WINDOW_WIDTH��WINDOW_HEIGHT�̑傫���ɂ���
	*/
	RECT window_rect;
	SetRect(&window_rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRectEx(&window_rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, false, 0);

	h_wnd = CreateWindowEx(
		0,
		WINDOW_DEF_STR,
		WINDOW_TITLE_STR,
		WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	SetWindowPos(h_wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

///	//if (full_result == IDNO) {//�t���X�N���[���łȂ���
///	if (fullscreen_result == false) {//�t���X�N���[���łȂ���
///
///		/*
///		�E�C���h�E�̕`��͈͂�WINDOW_WIDTH��WINDOW_HEIGHT�̑傫���ɂ���
///		*/
///		RECT window_rect;
///		SetRect(&window_rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
///		AdjustWindowRectEx(&window_rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, false, 0);
///
///		h_wnd = CreateWindowEx(
///			0,
///			WINDOW_DEF_STR,
///			WINDOW_TITLE_STR,
///			WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
///			CW_USEDEFAULT,
///			CW_USEDEFAULT,
///			window_rect.right - window_rect.left,
///			window_rect.bottom - window_rect.top,
///			NULL,
///			NULL,
///			hInstance,
///			NULL);
///
///		SetWindowPos(h_wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
///	}
///	else {//�t���X�N���[���̎�
///
///		//�𑜓x�̕ύX
///		DEVMODE mode = {};
///		BOOL result = EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);
///		mode.dmPelsWidth = WINDOW_WIDTH;
///		mode.dmPelsHeight = WINDOW_HEIGHT;
///		ChangeDisplaySettings(&mode, CDS_FULLSCREEN);
///
///		int display_width = GetSystemMetrics(SM_CXSCREEN);
///		int display_height = GetSystemMetrics(SM_CYSCREEN);
///
///		h_wnd = CreateWindowEx(
///			0,
///			WINDOW_DEF_STR,
///			WINDOW_TITLE_STR,
///			WS_VISIBLE | WS_POPUP,
///			0,
///			0,
///			display_width,
///			display_height,
///			NULL,
///			NULL,
///			hInstance,
///			NULL);
///
///		SetWindowPos(h_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
///	}

	if (h_wnd == NULL) {
		MessageBox(NULL, _T("Window�̏������̍ۂɃG���[���������܂���"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return EXIT_FAILURE;
	}


	MaterialArchiveFile::Init();


	D3DPRESENT_PARAMETERS g_D3DParam;
	/*
	DirectX������
	*/
	MyDirectXSystem myDirectXSystem(h_wnd, WINDOW_WIDTH, WINDOW_HEIGHT, 0, &g_D3DParam, TRUE/*((fullscreen_result == false) ? TRUE : FALSE)*/);

	//LPD3DXSPRITE sprite;
	//if (FAILED(D3DXCreateSprite(myDirectXSystem.get_pDevice3D(), &sprite))) {
	//	MessageBox(NULL, _T("DirectX�̏������̍ۂɃG���[���������܂���"), _T("ERROR"), MB_OK | MB_ICONERROR);
	//
	//	__app_safe_exit_error__();
	//
	//	return EXIT_FAILURE;
	//}



	My_Random::_make_singleton();

	Key key;
	GameManager* game_manager = new GameManager(&myDirectXSystem, &key/*, sprite*/);

	Fps fps(&myDirectXSystem/*, sprite*/);

	/*
	���낢�돉����
	*/

	My_Input::_make_singleton(h_wnd, hInstance);


	//���t���b�V�����[�g���v�Z����
	HDC hdc = GetDC(h_wnd);
	int refresh_rate = GetDeviceCaps(hdc, VREFRESH);
	M_debug_printf("################################\n");
	M_debug_printf("refresh_rate = %d\n", refresh_rate);
	M_debug_printf("################################\n");

	bool wait_flag = false;
//	if (frame_process_type == 0) {
//		if (refresh_rate <= 60) {
//			wait_flag = false;
//		}
//		else {
//			wait_flag = true;
//		}
//	}
//	else if (frame_process_type == 1) {
//		wait_flag = true;
//	}
//	else {
//		wait_flag = true;
//	}
	if (refresh_rate <= 60) {
		wait_flag = false;
	}
	else {
		wait_flag = true;
	}


	M_debug_printf("################################\n");
	M_debug_printf("wait_flag = %s\n", (wait_flag ? "true" : "false"));
	M_debug_printf("################################\n");


	/*
	���b�Z�[�W���[�v����
	*/
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		//fps.update(); //�������Ɛ��m�ɑ���Ȃ�

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (SUCCEEDED(myDirectXSystem.get_pDevice3D()->BeginScene()) == TRUE) {
				My_Input::get_instance_ptr()->update_process();

				//�w�i�F
				DWORD ClearColor = 0xFFFFFF;
				//��ʂ��N���A�[����
				myDirectXSystem.get_pDevice3D()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, ClearColor, 1.0f, 0);

				//__SPRITE__
				//sprite->Begin(D3DXSPRITE_ALPHABLEND);

				fps.update();

				MyDirectXDraw::set_default_state(&myDirectXSystem);
				MyDirectXDraw::enable_alpha(&myDirectXSystem);
				MyDirectXDraw::set_sampler_state__2d(&myDirectXSystem);

				//######################################################################################################
				//#
				//#�����ŃQ�[���̕`��⏈��������
				//#
				//######################################################################################################

				key.key_state_update();

				game_manager->execute_game_process();

				//######################################################################################################
				//#
				//#�����܂ŃQ�[���̕`��⏈��������
				//#
				//######################################################################################################


				if (Main::Show_FPS_Flag == true) {
					fps.draw_fps();
				}

				//__SPRITE__
				//sprite->End();

				myDirectXSystem.get_pDevice3D()->EndScene();

				//��ʂɔ��f����
				HRESULT present_result_1 = myDirectXSystem.get_pDevice3D()->Present(NULL, NULL, NULL, NULL);
				if (present_result_1 == D3DERR_DEVICELOST) {
					HRESULT present_result_2 = myDirectXSystem.get_pDevice3D()->TestCooperativeLevel();

					if (present_result_2 != D3D_OK) {
						if (present_result_2 == D3DERR_DEVICENOTRESET) {
							if (myDirectXSystem.get_pDevice3D()->Reset(&g_D3DParam) != D3D_OK) {
								__app_safe_exit_error__();
							}
						}
					}

				}

				if (present_result_1 != D3D_OK) {//Present�����s�����Ƃ�
					if (wait_flag == false) {
						fps.wait();//���������h��
					}
				}
				
				
				if (wait_flag == true) {
					fps.wait();
				}

			}
		}
	}

	/*
	���낢��������
	*/

	delete game_manager;

	My_Input::_delete_singleton();

	My_Random::_delete_singleton();

	CRC::End();

	MaterialArchiveFile::End();

#ifdef GBX_EMU_DEBUG
	//if (full_result == IDNO) {
//	if (fullscreen_result == false) {
		FreeConsole();
//	}
#endif

	Mutex::get_instance_ptr()->delete_app_mutex();
	Mutex::End();

	return 0;

}


/*
�R�[���o�b�N�֐�(�Q�[�����)
*/
LRESULT CALLBACK WinProc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	static HMENU hmenu;
	static MENUITEMINFO menuInfo;

	static OPENFILENAME ofn = { 0 };
	static TCHAR strCustom[256] = TEXT("All files\0*.*\0\0");

	switch (u_msg) {
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	/*case WM_KEYDOWN:
		if (w_param == VK_ESCAPE) {//Esc�L�[���������Ƃ��I������
			PostMessage(h_wnd, WM_CLOSE, 0, 0);
		}

		return 0;*/
	case WM_CREATE:
		hmenu = GetMenu(h_wnd);
		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fState = MFS_UNCHECKED;

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = h_wnd;
		ofn.lpstrFilter = //TEXT("�Q�[���{�[�C(�J���[)��ROM�̂�� (*.gb)\0*.gb\0")
						  TEXT("�Q�[���{�[�C��ROM�̂�� (*.gb)\0*.gb\0")
						  TEXT("���ׂẴt�@�C�� (*.*)\0*.*\0\0");
		ofn.lpstrCustomFilter = strCustom;
		ofn.nMaxCustFilter = 256;
		ofn.nFilterIndex = 0;
		ofn.lpstrFile = Main::ROMFilePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;

		menuInfo.fMask = MIIM_STATE;
		menuInfo.fState = MFS_CHECKED;
		SetMenuItemInfo(hmenu, ID_40008, FALSE, &menuInfo);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(w_param)) {
		case ID_40003:
			if (Main::ready_read_ROM_flag == false) {
				if (GetOpenFileName(&ofn) == TRUE) {//�L�����Z���Ȃǂ����Ă��Ȃ��Ƃ�
					Main::ready_read_ROM_flag = true;
				}
			}
			else {
				MessageBox(NULL, _T("ROM�ǂݍ��݂̍ۂɃG���[���������܂���\n�ēx��������������"), _T("���"), MB_ICONWARNING);
			}

			return 0;
		case ID_40006:
			if (Main::ROM_loaded_flag == true) {
				char cart_type_name[256];
				if (Main::Cartridge_Type == 0x00) {
					strcpy_s(cart_type_name, 256, "ROM ONLY");
				}
				else if (Main::Cartridge_Type == 0x01) {
					strcpy_s(cart_type_name, 256, "MBC1");
				}
				else if (Main::Cartridge_Type == 0x02) {
					strcpy_s(cart_type_name, 256, "MBC1+RAM");
				}
				else if (Main::Cartridge_Type == 0x03) {
					strcpy_s(cart_type_name, 256, "MBC1+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x05) {
					strcpy_s(cart_type_name, 256, "MBC2");
				}
				else if (Main::Cartridge_Type == 0x06) {
					strcpy_s(cart_type_name, 256, "MBC2+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x08) {
					strcpy_s(cart_type_name, 256, "ROM+RAM 1");
				}
				else if (Main::Cartridge_Type == 0x09) {
					strcpy_s(cart_type_name, 256, "ROM+RAM+BATTERY 1");
				}
				else if (Main::Cartridge_Type == 0x0B) {
					strcpy_s(cart_type_name, 256, "MMM01");
				}
				else if (Main::Cartridge_Type == 0x0C) {
					strcpy_s(cart_type_name, 256, "MMM01+RAM");
				}
				else if (Main::Cartridge_Type == 0x0D) {
					strcpy_s(cart_type_name, 256, "MMM01+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x0F) {
					strcpy_s(cart_type_name, 256, "MBC3+TIMER+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x10) {
					strcpy_s(cart_type_name, 256, "MBC3+TIMER+RAM+BATTERY 2");
				}
				else if (Main::Cartridge_Type == 0x11) {
					strcpy_s(cart_type_name, 256, "MBC3");
				}
				else if (Main::Cartridge_Type == 0x12) {
					strcpy_s(cart_type_name, 256, "MBC3+RAM 2");
				}
				else if (Main::Cartridge_Type == 0x13) {
					strcpy_s(cart_type_name, 256, "MBC3+RAM+BATTERY 2");
				}
				else if (Main::Cartridge_Type == 0x19) {
					strcpy_s(cart_type_name, 256, "MBC5");
				}
				else if (Main::Cartridge_Type == 0x1A) {
					strcpy_s(cart_type_name, 256, "MBC5+RAM");
				}
				else if (Main::Cartridge_Type == 0x1B) {
					strcpy_s(cart_type_name, 256, "MBC5+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x1C) {
					strcpy_s(cart_type_name, 256, "MBC5+RUMBLE");
				}
				else if (Main::Cartridge_Type == 0x1D) {
					strcpy_s(cart_type_name, 256, "MBC5+RUMBLE+RAM");
				}
				else if (Main::Cartridge_Type == 0x1E) {
					strcpy_s(cart_type_name, 256, "MBC5+RUMBLE+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x20) {
					strcpy_s(cart_type_name, 256, "MBC6");
				}
				else if (Main::Cartridge_Type == 0x22) {
					strcpy_s(cart_type_name, 256, "MBC7+SENSOR+RUMBLE+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0xFC) {
					strcpy_s(cart_type_name, 256, "POCKET CAMERA");
				}
				else if (Main::Cartridge_Type == 0xFD) {
					strcpy_s(cart_type_name, 256, "BANDAI TAMA5");
				}
				else if (Main::Cartridge_Type == 0xFE) {
					strcpy_s(cart_type_name, 256, "HuC3");
				}
				else if (Main::Cartridge_Type == 0xFF) {
					strcpy_s(cart_type_name, 256, "HuC1+RAM+BATTERY");
				}
				else {
					strcpy_s(cart_type_name, 256, "UNKNOWN");
				}
				char tmp_str_buffer[512];
				sprintf_s(tmp_str_buffer, "�J�[�h���b�W�̃^�C�v : [0x%02x] %s\n�v���O����ROM�T�C�Y : %dKB\nSRAM�̃T�C�Y : %dKB", Main::Cartridge_Type, cart_type_name, Main::PGM_size, Main::SRAM_size);
				MessageBox(NULL, _T(tmp_str_buffer), _T("ROM���"), MB_ICONINFORMATION);
			}
			else {
				MessageBox(NULL, _T("�܂�ROM���ǂݍ��܂�Ă��܂���"), _T("ROM���"), MB_ICONINFORMATION);
			}

			return 0;
		case ID_40008:
			Main::Show_FPS_Flag = !Main::Show_FPS_Flag;

			if (Main::Show_FPS_Flag == false) {
				menuInfo.fState = MFS_UNCHECKED;
			}
			else {
				menuInfo.fState = MFS_CHECKED;
			}
			SetMenuItemInfo(hmenu, ID_40008, FALSE, &menuInfo);

			return 0;
		case ID_40007:
			if (My_Input::get_instance_ptr() != nullptr){
				My_Input::get_instance_ptr()->find_and_update_pad();

				MessageBox(NULL, _T("�Q�[���p�b�h�̍X�V���������܂���"), _T("���"), MB_ICONINFORMATION);
			}
			else {
				MessageBox(NULL, _T("�Q�[���p�b�h�̍X�V�̍ۂɃG���[���������܂���\n�ēx��������������"), _T("���"), MB_ICONWARNING);
			}

			return 0;
		}
	}

	return DefWindowProc(h_wnd, u_msg, w_param, l_param);
}
