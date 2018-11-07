#include <Windows.h>
#include "resource.h"
#include <fstream>
#include <minwinbase.h>
#include <winuser.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/opencv.hpp>
#include "Picture.h"
using namespace std;
//using namespace cv;

HINSTANCE hInst;
Picture *picture;
Mat hist;
bool gbStopEvent = false;
uint currentChannel = CHANNEL_RGB;

BOOL CALLBACK DialogProcedure(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI readCamera(LPVOID);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdParam, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdParam);

	hInst = hInstance;	
	//picture = NULL;

	HWND hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)DialogProcedure);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) == TRUE)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

BOOL CALLBACK DialogProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	OPENFILENAME ofn;
	HANDLE hCamera = NULL;
	WCHAR path[260];

	switch (msg)
	{
	case WM_INITDIALOG:
		Picture::adaptControl(GetDlgItem(hwnd, PIC_IMAGE), "pic_image");
		Picture::adaptControl(GetDlgItem(hwnd, PIC_HISTOGRAM), "pic_histogram");
		SendMessage(GetDlgItem(hwnd, RD_RGBCHANNEL), BM_SETCHECK, BST_CHECKED, 1);
		
		waitKey(1);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_IMAGEN_ABRIR:
			gbStopEvent = true;
			WaitForSingleObject(hCamera, INFINITE);
			CloseHandle(hCamera);
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = path;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(path);
			ofn.lpstrFilter = L"Image files (*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0"
				"All files\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;	
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn) == TRUE)
			{
				char fileName[260];
				char fileBuffer = ' ';
				WideCharToMultiByte(CP_ACP, 0, path, -1, fileName, 260, &fileBuffer, NULL);
				picture = new Picture(string(fileName));

				wchar_t dimensions[100];
				wsprintf(dimensions, L"%dpx*%dpx", picture->image.cols, picture->image.rows);

				ifstream img(string(fileName).c_str(), ifstream::in | ifstream::binary);
				img.seekg(0, ios::end);
				int fileSize = img.tellg();
				img.close();
				wchar_t size[100];
				wsprintf(size, L"%dKB", fileSize / 1000);

				SetDlgItemText(hwnd, LBL_DIMENSIONS, dimensions);
				SetDlgItemText(hwnd, LBL_SIZE, size);
				SetDlgItemText(hwnd, LBL_PATH, path);

				imshow("pic_image", picture->image);
				//SendMessage(GetDlgItem(hwnd, PIC_IMAGE), LB_GETITEMRECT, 0, (LPARAM)&lb_pic_histogram);
				hist = picture->getHistogram(160, 132, CHANNEL_RGB);
				imshow("pic_histogram", hist);

				EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
			}
			break;
		case ID_VIDEO_TOMAR:
			gbStopEvent = false;
			DWORD cameraThread;
			hCamera = CreateThread(0, 0, readCamera, NULL , 0, &cameraThread);
			EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
			EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
			EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
			EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
			break;
		case RD_RGBCHANNEL:
			currentChannel = CHANNEL_RGB;
			hist = picture->getHistogram(160, 132, CHANNEL_RGB);
			imshow("pic_histogram", hist);
			break;
		case RD_RCHANNEL:
			currentChannel = CHANNEL_R;
			hist = picture->getHistogram(160, 132, CHANNEL_R);
			imshow("pic_histogram", hist);
			break;
		case RD_GCHANNEL:
			currentChannel = CHANNEL_G;
			hist = picture->getHistogram(160, 132, CHANNEL_G);
			imshow("pic_histogram", hist);
			break;
		case RD_BCHANNEL:
			currentChannel = CHANNEL_B;
			hist = picture->getHistogram(160, 132, CHANNEL_B);
			imshow("pic_histogram", hist);
			break;
		}
		break;
	case WM_CLOSE:
		if (MessageBox(NULL, L"¿Seguro que deseas cerrar la ventana?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES)
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

DWORD WINAPI readCamera(LPVOID lpParameter)
{
	VideoCapture camara(0);
	while(!gbStopEvent)
	{
		Mat frame;
		bool exito = camara.read(frame);
		if(exito)
		{
			picture = new Picture(frame);
			hist = picture->getHistogram(160, 132, currentChannel);
			imshow("pic_histogram", hist);
			imshow("pic_image", frame);
		}
	}
	return 0;
}
