#include <Windows.h>
#include "resource.h"
#include <fstream>
#include <minwinbase.h>
#include <winuser.h>
#include <CommCtrl.h>
#include <string>
#include <tchar.h>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/opencv.hpp>
#include "Picture.h"
#include "FilterFactory.h"
#include "MedianFilter.h"
#include "WeightedMedianFilter.h"
#include "MinusMedianFilter.h"
#include "AverageFilter.h"
#include "LaplacianFilter.h"
#include "MinusLaplacianFilter.h"
#include "DirectionalNorthFilter.h"
#include "DirectionalEastFilter.h"
#include "GrayscaleLuminanceFilter.h"
#include "SobelFilter.h"
#include "GaussianFilter.h"
#include "ExponentialEqualizationHistogramFilter.h"
#include "HighlightFilter.h"
#include "DisplacementHistogramFilter.h"
#include "UmbralFilter.h"
using namespace std;
using namespace cv;

#define CORRECT_FROM_START 0
#define CORRECT_FROM_CENTER 1
#define CORRECT_FROM_END 2
#define COMES_FROM_VIDEO 100
#define COMES_FROM_DIRECT 200
#define COMES_FROM_HOG 300

HINSTANCE hInst;
Picture *picture;
Mat hist;
bool gbStopEvent = true;
uint currentChannel = CHANNEL_RGB;
FilterFactory ff = FilterFactory::get_instance();
HWND procHwnd;
HMENU procHwndMenu;
int filter_count = 0;
int id_helper = -1;
int how_to_correct = -1;
bool imageIsSaved = false;
bool videoIsSaved = false;
struct Record
{
	FilterList type;
	Filter *filter;
};
vector <Record> list;
bool is_video = false;
vector<Mat> clip;
String videoPath;
int from_where_comes = COMES_FROM_DIRECT;

BOOL CALLBACK DialogProcedure(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK FiltersProcedure(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ModifyProcedure(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI loadedVideo(LPVOID);
DWORD WINAPI cameraFilter(LPVOID);
DWORD WINAPI cameraHOG(LPVOID);
void add_filter(FilterList);
void correct_all_filters();
void save_image(OPENFILENAME&);
void save_video(OPENFILENAME&);
Picture* apply_all_filters();
float rand_range(float, float);

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
	bool wantToSave = false;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetTimer(hwnd, 100, 10, NULL);
		Picture::adaptControl(GetDlgItem(hwnd, PIC_IMAGE), "pic_image");
		Picture::adaptControl(GetDlgItem(hwnd, PIC_FILTER), "pic_filter");
		Picture::adaptControl(GetDlgItem(hwnd, PIC_HISTOGRAM), "pic_histogram");
		SendMessage(GetDlgItem(hwnd, RD_RGBCHANNEL), BM_SETCHECK, BST_CHECKED, 1);
		procHwnd = hwnd;
		procHwndMenu = GetMenu(hwnd);
		waitKey(1);
		return TRUE;
	case WM_TIMER:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_IMAGEN_ABRIR:
			if(list.size() != 0 && !imageIsSaved)
				wantToSave = (MessageBox(NULL, L"¿Seguro que deseas continuar sin guardar?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES) ? false : true;

			if(imageIsSaved || !wantToSave)
			{
				is_video = false;
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

					Mat res = picture->GetSquareImage(GetDlgItem(hwnd, PIC_IMAGE));
					imshow("pic_image", res);
					//SendMessage(GetDlgItem(hwnd, PIC_IMAGE), LB_GETITEMRECT, 0, (LPARAM)&lb_pic_histogram);
					hist = picture->getHistogram(160, 132, currentChannel);
					imshow("pic_histogram", hist);

					EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, BTN_ADD), true);
					EnableWindow(GetDlgItem(hwnd, BTN_EDIT), false);
					EnableWindow(GetDlgItem(hwnd, BTN_DELETE), false);
					EnableMenuItem(procHwndMenu, ID_IMAGEN_GUARDAR, MF_DISABLED);
					EnableMenuItem(procHwndMenu, ID_VIDEO_GUARDAR, MF_DISABLED);
					imageIsSaved = false;

					filter_count = 0;
					list.erase(list.begin(), list.end());
					SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_RESETCONTENT, 0, 0);
				}
			}
			else
				save_image(ofn);
			break;
		case ID_IMAGEN_GUARDAR:
			save_image(ofn);
			break;
		case ID_VIDEO_ABRIR:
			if (list.size() != 0 && !imageIsSaved)
				wantToSave = (MessageBox(NULL, L"¿Seguro que deseas continuar sin guardar?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES) ? false : true;

			if (imageIsSaved || !wantToSave)
			{
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = path;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(path);
				ofn.lpstrFilter = L"MP4 Videos\0*.mp4\0"
					"AVI Videos\0*.avi\0";
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
					videoPath = String(fileName);
					SetDlgItemText(hwnd, LBL_PATH, path);

					if (!gbStopEvent)
					{
						gbStopEvent = true;
						WaitForSingleObject(hCamera, INFINITE);
						CloseHandle(hCamera);
						clip.erase(clip.begin());
						Sleep(3000);
					}
					is_video = true;
					gbStopEvent = false;
					if (filter_count > 0)
					{
						filter_count = 0;
						list.erase(list.begin(), list.end());
					}
					SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_RESETCONTENT, 0, 0);

					DWORD cameraThread;
					hCamera = CreateThread(0, 0, loadedVideo, NULL, 0, &cameraThread);
					EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
					EnableWindow(GetDlgItem(hwnd, BTN_ADD), true);
					EnableWindow(GetDlgItem(hwnd, BTN_EDIT), false);
					EnableWindow(GetDlgItem(hwnd, BTN_DELETE), false);
					EnableMenuItem(procHwndMenu, ID_IMAGEN_GUARDAR, MF_DISABLED);
					EnableMenuItem(procHwndMenu, ID_VIDEO_GUARDAR, MF_ENABLED);
				}
			}
			break;
		case ID_VIDEO_TOMAR:
			if (list.size() != 0 && !imageIsSaved)
				wantToSave = (MessageBox(NULL, L"¿Seguro que deseas continuar sin guardar?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES) ? false : true;

			if (imageIsSaved || !wantToSave)
			{
				if (!gbStopEvent)
				{
					gbStopEvent = true;
					WaitForSingleObject(hCamera, INFINITE);
					CloseHandle(hCamera);
					clip.erase(clip.begin());
					Sleep(3000);
				}
				is_video = true;
				gbStopEvent = false;
				if(filter_count > 0)
				{
					filter_count = 0;
					list.erase(list.begin(), list.end());
				}
				SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_RESETCONTENT, 0, 0);

				DWORD cameraThread;
				hCamera = CreateThread(0, 0, cameraFilter, NULL, 0, &cameraThread);
				EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, BTN_ADD), true);
				EnableWindow(GetDlgItem(hwnd, BTN_EDIT), false);
				EnableWindow(GetDlgItem(hwnd, BTN_DELETE), false);
				EnableMenuItem(procHwndMenu, ID_IMAGEN_GUARDAR, MF_DISABLED);
				EnableMenuItem(procHwndMenu, ID_VIDEO_GUARDAR, MF_ENABLED);
			}
			break;
		case ID_VIDEO_HOG:
			if (list.size() != 0 && !imageIsSaved)
				wantToSave = (MessageBox(NULL, L"¿Seguro que deseas continuar sin guardar?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES) ? false : true;

			if (imageIsSaved || !wantToSave)
			{
				if(!gbStopEvent)
				{
					gbStopEvent = true;
					WaitForSingleObject(hCamera, INFINITE);
					CloseHandle(hCamera);
					clip.erase(clip.begin());
					Sleep(3000);
				}
				is_video = true;
				gbStopEvent = false;
				if (filter_count > 0)
				{
					filter_count = 0;
					list.erase(list.begin(), list.end());
				}
				SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_RESETCONTENT, 0, 0);

				DWORD cameraThread;
				hCamera = CreateThread(0, 0, cameraHOG, NULL, 0, &cameraThread);
				EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, BTN_ADD), false);
				EnableWindow(GetDlgItem(hwnd, BTN_EDIT), false);
				EnableWindow(GetDlgItem(hwnd, BTN_DELETE), false);
				EnableMenuItem(procHwndMenu, ID_IMAGEN_GUARDAR, MF_DISABLED);
				EnableMenuItem(procHwndMenu, ID_VIDEO_GUARDAR, MF_ENABLED);
			}
			break;
		case ID_VIDEO_GUARDAR:
			is_video = false;
			gbStopEvent = true;
			WaitForSingleObject(hCamera, INFINITE);
			CloseHandle(hCamera);
			save_video(ofn);
			if(from_where_comes != COMES_FROM_VIDEO)
				clip.erase(clip.begin());
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
		case BTN_ADD:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FILTERS), hwnd, (DLGPROC)FiltersProcedure);
			break;
		case BTN_EDIT:
			{
				if (id_helper == 0)
					how_to_correct = CORRECT_FROM_START;
				else if (id_helper == list.size() - 1)
					how_to_correct = CORRECT_FROM_END;
				else
					how_to_correct = CORRECT_FROM_CENTER;

				DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MODIFY), hwnd, (DLGPROC)ModifyProcedure);
			}
			break;
		case BTN_DELETE:
			{
				if(id_helper == 0)
					how_to_correct = CORRECT_FROM_START;
				else if(id_helper == list.size() - 1 )
					how_to_correct = CORRECT_FROM_END;
				else
					how_to_correct = CORRECT_FROM_CENTER;

				SendMessage(GetDlgItem(hwnd, LB_RECORD), LB_DELETESTRING, id_helper, 0);
				filter_count--;
				list.erase(list.begin() + id_helper);
				if(!is_video)
					correct_all_filters();
				EnableWindow(GetDlgItem(hwnd, BTN_DELETE), false);
			}
			break;
		case LB_RECORD:
			switch(HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				{
					const int count = SendMessage(GetDlgItem(hwnd, LB_RECORD), LB_GETCOUNT, 0, 0);
					for (int i = 0; i < count; i++)
					{
						if (SendMessage(GetDlgItem(hwnd, LB_RECORD), LB_GETSEL, i, 0) > 0)
						{
							id_helper = i;
							break;
						}
					}
					if(!is_video)
						EnableWindow(GetDlgItem(hwnd, BTN_EDIT), true);
					EnableWindow(GetDlgItem(hwnd, BTN_DELETE), true);
				}
				break;
			}
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

BOOL CALLBACK FiltersProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_MEDIAN:
			add_filter(_Median);
			break;
		case BTN_WEIGHTED_MEDIAN:
			add_filter(_WeightedMedian);
			break;
		case BTN_MINUS_MEDIAN:
			add_filter(_MinusMedian);
			break;
		case BTN_AVERAGE:
			add_filter(_Average);
			break;
		case BTN_LAPLACIAN:
			add_filter(_Laplacian);
			break;
		case BTN_MINUS_LAPLACIAN:
			add_filter(_MinusLaplacian);
			break;
		case BTN_DIRECTIONAL_NORTH:
			add_filter(_DirectionalNorth);
			break;
		case BTN_DIRECTIONAL_EAST:
			add_filter(_DirectionalEast);
			break;
		case BTN_GRAYSCALE_AVERAGE:
			add_filter(_GrayscaleAverage);
			break;
		case BTN_GRAYSCALE_LUMINOSITY:
			add_filter(_GrayscaleLuminosity);
			break;
		case BTN_GRAYSCALE_LUMINANCE:
			add_filter(_GrayscaleLuminance);
			break;
		case BTN_SEPIA:
			add_filter(_Sepia);
			break;
		case BTN_SOBEL:
			add_filter(_Sobel);
			break;
		case BTN_GAUSSIAN:
			add_filter(_Gaussian);
			break;
		case BTN_NORMALIZE_HISTOGRAM:
			add_filter(_NormalizeHistogram);
			break;
		case BTN_EQUALIZATION_HISTOGRAM:
			add_filter(_EqualizationHistogram);
		case BTN_SIMPLE_EQUALIZATION_HISTOGRAM:
			add_filter(_SimpleEqualizationHistogram);
			break;
		case BTN_UNIFORM_EQUALIZATION_HISTOGRAM:
			add_filter(_UniformEqualizationHistogram);
			break;
		case BTN_EXPONENTIAL_EQUALIZATION_HISTOGRAM:
			add_filter(_ExponentialEqualizationHistogram);
			break;
		case BTN_DISPLACEMENT_HISTOGRAM:
			add_filter(_DisplacementHistogram);
			break;
		case BTN_UMBRAL:
			add_filter(_Umbral);
			break;
		case BTN_HIGHLIGHT:
			add_filter(_Highlight);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, NULL);
		break;
	}
	return FALSE;
}

BOOL CALLBACK ModifyProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (msg)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_ADDSTRING, 0, (LPARAM)L"3 x 3");
		SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_ADDSTRING, 0, (LPARAM)L"5 x 5");
		SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_ADDSTRING, 0, (LPARAM)L"7 x 7");
		SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_ADDSTRING, 0, (LPARAM)L"9 x 9");
		SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_ADDSTRING, 0, (LPARAM)L"11 x 11");
		SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		switch (list[id_helper].type)
		{
		case _Median:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			break;
		case _WeightedMedian:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			EnableWindow(GetDlgItem(hwnd, TXT_WEIGHT), true);
			break;
		case _MinusMedian:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			break;
		case _Average:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			break;
		case _Laplacian:
			EnableWindow(GetDlgItem(hwnd, TXT_WEIGHT), true);
			break;
		case _MinusLaplacian:
			EnableWindow(GetDlgItem(hwnd, TXT_WEIGHT), true);
			break;
		case _DirectionalNorth:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			EnableWindow(GetDlgItem(hwnd, TXT_WEIGHT), true);
			break;
		case _DirectionalEast:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			EnableWindow(GetDlgItem(hwnd, TXT_WEIGHT), true);
			break;
		case _GrayscaleLuminance:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			break;
		case _Sobel:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			break;
		case _Gaussian:
			EnableWindow(GetDlgItem(hwnd, CMB_SIZE), true);
			EnableWindow(GetDlgItem(hwnd, TXT_SIGMA), true);
			break;
		case _ExponentialEqualizationHistogram:
			EnableWindow(GetDlgItem(hwnd, TXT_SIGMA), true);
			break;
		case _DisplacementHistogram:
			EnableWindow(GetDlgItem(hwnd, SLD_DISPLACEMENT), true);
			break;
		case _Umbral:
			EnableWindow(GetDlgItem(hwnd, TXT_FIRST_UMBRAL), true);
			EnableWindow(GetDlgItem(hwnd, TXT_LAST_UMBRAL), true);
			break;
		case _Highlight:
			EnableWindow(GetDlgItem(hwnd, TXT_HIGHLIGHT), true);
			break;
		default:
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_OK:
			switch (list[id_helper].type)
			{
			case _Median:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<MedianFilter*>(list[id_helper].filter)->modify(3);
						break;
					case 1:
						dynamic_cast<MedianFilter*>(list[id_helper].filter)->modify(5);
						break;
					case 2:
						dynamic_cast<MedianFilter*>(list[id_helper].filter)->modify(7);
						break;
					case 3:
						dynamic_cast<MedianFilter*>(list[id_helper].filter)->modify(9);
						break;
					case 4:
						dynamic_cast<MedianFilter*>(list[id_helper].filter)->modify(11);
						break;
					}
				}
				break;
			case _WeightedMedian:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_WEIGHT), buff, 1024);
					int number_text = _ttoi(buff);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<WeightedMedianFilter*>(list[id_helper].filter)->modify(3, number_text);
						break;
					case 1:
						dynamic_cast<WeightedMedianFilter*>(list[id_helper].filter)->modify(5, number_text);
						break;
					case 2:
						dynamic_cast<WeightedMedianFilter*>(list[id_helper].filter)->modify(7, number_text);
						break;
					case 3:
						dynamic_cast<WeightedMedianFilter*>(list[id_helper].filter)->modify(9, number_text);
						break;
					case 4:
						dynamic_cast<WeightedMedianFilter*>(list[id_helper].filter)->modify(11, number_text);
						break;
					}
				}
				break;
			case _MinusMedian:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<MinusMedianFilter*>(list[id_helper].filter)->modify(3);
						break;
					case 1:
						dynamic_cast<MinusMedianFilter*>(list[id_helper].filter)->modify(5);
						break;
					case 2:
						dynamic_cast<MinusMedianFilter*>(list[id_helper].filter)->modify(7);
						break;
					case 3:
						dynamic_cast<MinusMedianFilter*>(list[id_helper].filter)->modify(9);
						break;
					case 4:
						dynamic_cast<MinusMedianFilter*>(list[id_helper].filter)->modify(11);
						break;
					}
				}
				break;
			case _Average:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<AverageFilter*>(list[id_helper].filter)->modify(3);
						break;
					case 1:
						dynamic_cast<AverageFilter*>(list[id_helper].filter)->modify(5);
						break;
					case 2:
						dynamic_cast<AverageFilter*>(list[id_helper].filter)->modify(7);
						break;
					case 3:
						dynamic_cast<AverageFilter*>(list[id_helper].filter)->modify(9);
						break;
					case 4:
						dynamic_cast<AverageFilter*>(list[id_helper].filter)->modify(11);
						break;
					}
				}
				break;
			case _Laplacian:
				{
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_WEIGHT), buff, 1024);
					int number_text = _ttoi(buff);
					dynamic_cast<LaplacianFilter*>(list[id_helper].filter)->modify(number_text);
				}
				break;
			case _MinusLaplacian:
				{
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_WEIGHT), buff, 1024);
					int number_text = _ttoi(buff);
					dynamic_cast<MinusLaplacianFilter*>(list[id_helper].filter)->modify(number_text);
				}
				break;
			case _DirectionalNorth:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_WEIGHT), buff, 1024);
					int number_text = _ttoi(buff);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<DirectionalNorthFilter*>(list[id_helper].filter)->modify(3, number_text);
						break;
					case 1:
						dynamic_cast<DirectionalNorthFilter*>(list[id_helper].filter)->modify(5, number_text);
						break;
					case 2:
						dynamic_cast<DirectionalNorthFilter*>(list[id_helper].filter)->modify(7, number_text);
						break;
					case 3:
						dynamic_cast<DirectionalNorthFilter*>(list[id_helper].filter)->modify(9, number_text);
						break;
					case 4:
						dynamic_cast<DirectionalNorthFilter*>(list[id_helper].filter)->modify(11, number_text);
						break;
					}
				}
				break;
			case _DirectionalEast:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_WEIGHT), buff, 1024);
					int number_text = _ttoi(buff);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<DirectionalEastFilter*>(list[id_helper].filter)->modify(3, number_text);
						break;
					case 1:
						dynamic_cast<DirectionalEastFilter*>(list[id_helper].filter)->modify(5, number_text);
						break;
					case 2:
						dynamic_cast<DirectionalEastFilter*>(list[id_helper].filter)->modify(7, number_text);
						break;
					case 3:
						dynamic_cast<DirectionalEastFilter*>(list[id_helper].filter)->modify(9, number_text);
						break;
					case 4:
						dynamic_cast<DirectionalEastFilter*>(list[id_helper].filter)->modify(11, number_text);
						break;
					}
				}
				break;
			case _GrayscaleLuminance:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<GrayscaleLuminanceFilter*>(list[id_helper].filter)->modify(3);
						break;
					case 1:
						dynamic_cast<GrayscaleLuminanceFilter*>(list[id_helper].filter)->modify(5);
						break;
					case 2:
						dynamic_cast<GrayscaleLuminanceFilter*>(list[id_helper].filter)->modify(7);
						break;
					case 3:
						dynamic_cast<GrayscaleLuminanceFilter*>(list[id_helper].filter)->modify(9);
						break;
					case 4:
						dynamic_cast<GrayscaleLuminanceFilter*>(list[id_helper].filter)->modify(11);
						break;
					}
				}
				break;
			case _Sobel:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<SobelFilter*>(list[id_helper].filter)->modify(3);
						break;
					case 1:
						dynamic_cast<SobelFilter*>(list[id_helper].filter)->modify(5);
						break;
					case 2:
						dynamic_cast<SobelFilter*>(list[id_helper].filter)->modify(7);
						break;
					case 3:
						dynamic_cast<SobelFilter*>(list[id_helper].filter)->modify(9);
						break;
					case 4:
						dynamic_cast<SobelFilter*>(list[id_helper].filter)->modify(11);
						break;
					}
				}
				break;
			case _Gaussian:
				{
					const int index_selected = SendMessage(GetDlgItem(hwnd, CMB_SIZE), CB_GETCURSEL, NULL, NULL);
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_SIGMA), buff, 1024);
					int number_text = _ttoi(buff);
					switch (index_selected)
					{
					case 0:
						dynamic_cast<GaussianFilter*>(list[id_helper].filter)->modify(3, number_text);
						break;
					case 1:
						dynamic_cast<GaussianFilter*>(list[id_helper].filter)->modify(5, number_text);
						break;
					case 2:
						dynamic_cast<GaussianFilter*>(list[id_helper].filter)->modify(7, number_text);
						break;
					case 3:
						dynamic_cast<GaussianFilter*>(list[id_helper].filter)->modify(9, number_text);
						break;
					case 4:
						dynamic_cast<GaussianFilter*>(list[id_helper].filter)->modify(11, number_text);
						break;
					}
				}
				break;
			case _ExponentialEqualizationHistogram:
				{
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_SIGMA), buff, 1024);
					int number_text = _ttoi(buff);
					dynamic_cast<ExponentialEqualizationHistogramFilter*>(list[id_helper].filter)->modify(number_text);
				}
				break;
			case _DisplacementHistogram:
				{
					int slider_percent = SendMessage(GetDlgItem(hwnd, SLD_DISPLACEMENT), TBM_GETPOS, 0, 0);
					slider_percent = slider_percent * 255 / 100;
					dynamic_cast<DisplacementHistogramFilter*>(list[id_helper].filter)->modify(slider_percent);
				}
				break;
			case _Umbral:
				{
					TCHAR buff_first[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_HIGHLIGHT), buff_first, 1024);
					int first_umbral = _ttoi(buff_first);
					TCHAR buff_last[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_HIGHLIGHT), buff_last, 1024);
					int last_umbral = _ttoi(buff_last);
					dynamic_cast<UmbralFilter*>(list[id_helper].filter)->modify(first_umbral, last_umbral);
				}
				break;
			case _Highlight:
				{
					TCHAR buff[1024];
					GetWindowText(GetDlgItem(hwnd, TXT_HIGHLIGHT), buff, 1024);
					int number_text = _ttoi(buff);
					dynamic_cast<HighlightFilter*>(list[id_helper].filter)->modify(number_text);
				}
				break;
			default:
				break;
			}
			if (!is_video)
				correct_all_filters();
			EndDialog(hwnd, NULL);
			break;
		case BTN_RESET:
			switch (list[id_helper].type)
			{
			case _Median:
				dynamic_cast<MedianFilter*>(list[id_helper].filter)->reset();
				break;
			case _WeightedMedian:
				dynamic_cast<WeightedMedianFilter*>(list[id_helper].filter)->reset();
				break;
			case _MinusMedian:
				dynamic_cast<MinusMedianFilter*>(list[id_helper].filter)->reset();
				break;
			case _Average:
				dynamic_cast<AverageFilter*>(list[id_helper].filter)->reset();
				break;
			case _Laplacian:
				dynamic_cast<LaplacianFilter*>(list[id_helper].filter)->reset();
				break;
			case _MinusLaplacian:
				dynamic_cast<MinusLaplacianFilter*>(list[id_helper].filter)->reset();
				break;
			case _DirectionalNorth:
				dynamic_cast<DirectionalNorthFilter*>(list[id_helper].filter)->reset();
				break;
			case _DirectionalEast:
				dynamic_cast<DirectionalEastFilter*>(list[id_helper].filter)->reset();
				break;
			case _GrayscaleLuminance:
				dynamic_cast<GrayscaleLuminanceFilter*>(list[id_helper].filter)->reset();
				break;
			case _Sobel:
				dynamic_cast<SobelFilter*>(list[id_helper].filter)->reset();
				break;
			case _Gaussian:
				dynamic_cast<GaussianFilter*>(list[id_helper].filter)->reset();
				break;
			case _ExponentialEqualizationHistogram:
				dynamic_cast<ExponentialEqualizationHistogramFilter*>(list[id_helper].filter)->reset();
				break;
			case _DisplacementHistogram:
				dynamic_cast<DisplacementHistogramFilter*>(list[id_helper].filter)->reset();
				break;
			case _Umbral:
				dynamic_cast<UmbralFilter*>(list[id_helper].filter)->reset();
				break;
			case _Highlight:
				dynamic_cast<HighlightFilter*>(list[id_helper].filter)->reset();
				break;
			default:
				break;
			}
			if (!is_video)
				correct_all_filters();
			EndDialog(hwnd, NULL);
			break;
		case BTN_CANCEL:
			EndDialog(hwnd, NULL);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, NULL);
		break;
	}
	return FALSE;
}

DWORD WINAPI loadedVideo(LPVOID lpParameter)
{
	from_where_comes = COMES_FROM_VIDEO;
	VideoCapture video(videoPath);
	while (1)
	{
		Mat frame;
		video >> frame;
		Mat aux = Mat(frame.rows / 3, frame.cols / 3, frame.type());
		if (!frame.empty())
		{
			resize(frame, aux, aux.size(), 0, 0, INTER_LINEAR);
			clip.push_back(aux);
		}else
			break;
	}
	while (!gbStopEvent)
	{
		Mat frame;
		video >> frame;
		Mat aux = Mat(frame.rows / 3, frame.cols / 3, frame.type());
		if (!frame.empty())
		{
			resize(frame, aux, aux.size(), 0, 0, INTER_LINEAR);
			picture = new Picture(aux);
			imshow("pic_image", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE)));
			Picture *filter_frame = apply_all_filters();
			if (filter_frame != nullptr)
			{
				imshow("pic_filter", filter_frame->GetSquareImage(GetDlgItem(procHwnd, PIC_FILTER)));
				hist = filter_frame->getHistogram(160, 132, currentChannel);
				imshow("pic_histogram", hist);
			}
			waitKey(20);
		}else
		{
			video.set(CV_CAP_PROP_POS_FRAMES, 0);
		}
	}
	return 0;
}

DWORD WINAPI cameraFilter(LPVOID lpParameter)
{
	from_where_comes = COMES_FROM_DIRECT;
	VideoCapture camara(0);
	while(!gbStopEvent)
	{
		Mat frame;
		bool exito = camara.read(frame);
		Mat aux = Mat(frame.rows / 3, frame.cols / 3, CV_8U);
		if(exito)
		{
			resize(frame, aux, aux.size(), 0, 0, INTER_LINEAR);
			picture = new Picture(aux);
			clip.push_back(picture->image);
			imshow("pic_image", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE)));
			Picture *filter_frame = apply_all_filters();
			if (filter_frame != nullptr)
			{
				imshow("pic_filter", filter_frame->GetSquareImage(GetDlgItem(procHwnd, PIC_FILTER)));
				hist = filter_frame->getHistogram(160, 132, currentChannel);
				imshow("pic_histogram", hist);
			}
		}
	}
	return 0;
}

DWORD WINAPI cameraHOG(LPVOID lpParameter)
{
	from_where_comes = COMES_FROM_HOG;
	VideoCapture camara(0);
	const int maxColorCount = 10;
	vector<cv::Scalar> colors;

	srand(GetTickCount());
	for (int i = 0; i < maxColorCount; i++) {
		const Scalar color = Scalar(rand_range(0, 255), rand_range(0, 255), rand_range(0, 255));
		colors.push_back(color);
	}

	int escala = 3;
	Mat tempSize;
	camara.read(tempSize);

	Mat aux(tempSize.rows / escala, tempSize.cols / escala, CV_8U);
	Mat aux2(tempSize.rows / escala, tempSize.cols / escala, CV_8U);
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	while (!gbStopEvent)
	{
		Mat frame;
		bool exito = camara.read(frame);
		if (exito)
		{
			resize(frame, aux, aux.size(), 0, 0, INTER_LINEAR);

			picture = new Picture(aux);
			imshow("pic_image", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE)));
			
			cvtColor(aux, aux2, CV_BGR2GRAY);
			vector<Rect> found, found_filtered;
			
			double t = (double)getTickCount();
			hog.detectMultiScale(aux2, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
			t = (double)getTickCount() - t;
			
			size_t i, j;
			for (i = 0; i < found.size(); i++)
			{
				Rect r = found[i];
				for (j = 0; j < found.size(); j++)
					if (j != i && (r & found[j]) == r)
						break;
				if (j == found.size())
					found_filtered.push_back(r);
			}
			string number = to_string(found_filtered.size());

			putText(picture->image, number.c_str(), cvPoint(30, 50),
				FONT_ITALIC, 1, cvScalar(0, 255, 0), 2, CV_AA);

			int colorsSize = colors.size();
			for (i = 0; i < found_filtered.size(); i++)
			{
				Rect r = found_filtered[i];
				r.x *= escala;
				r.x += cvRound(r.width*0.1);
				r.width = cvRound(r.width*0.8*escala);
				r.y = r.y*escala;
				r.y += cvRound(r.height*0.07);
				r.height = cvRound(r.height*0.8*escala);
				int index = i % colorsSize;
				rectangle(picture->image, r.tl(), r.br(), colors[index], 3);
			}
			if (waitKey(10) >= 40)
				break;
			clip.push_back(picture->image);
			imshow("pic_histogram", picture->getHistogram(160, 132, currentChannel));
			imshow("pic_filter", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_FILTER)));

		}
	}
	return 0;
}

void add_filter(FilterList _filter)
{
	ff.change_choise(_filter);
	list.push_back(Record());
	if(filter_count == 0)
	{
		list[filter_count].type = _filter;
		list[filter_count].filter = ff.createFilter();
		list[filter_count].filter->set_image(picture->image);
		list[filter_count].filter->apply();
	}
	else
	{
		Picture *prev = list[filter_count - 1].filter->get_result_image();
		list[filter_count].type = _filter;
		list[filter_count].filter = ff.createFilter();
		list[filter_count].filter->set_image(prev->image);
		list[filter_count].filter->apply();
	}

	Picture *last = list.back().filter->get_result_image();
	const auto res = last->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE));
	imshow("pic_filter", res);

	hist = last->getHistogram(160, 132, currentChannel);
	imshow("pic_histogram", hist);

	filter_count++;
	const auto index = static_cast<int>(SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(list.back().filter->get_name())));
	SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_SETITEMDATA, index, static_cast<LPARAM>(filter_count));

	if(is_video)
		EnableMenuItem(procHwndMenu, ID_VIDEO_GUARDAR, MF_ENABLED);
	else
		EnableMenuItem(procHwndMenu, ID_IMAGEN_GUARDAR, MF_ENABLED);
}

void correct_all_filters()
{
	if(filter_count > 0)
	{
		while (1)
		{
			if (how_to_correct == CORRECT_FROM_START)
			{
				list[id_helper].filter->set_image(picture->image);
				list[id_helper].filter->apply();
				how_to_correct = CORRECT_FROM_CENTER;
			}
			else if (how_to_correct == CORRECT_FROM_CENTER)
			{
				Picture *prev = list[id_helper - 1].filter->get_result_image();
				list[id_helper].filter->set_image(prev->image);
				list[id_helper].filter->apply();
			}
			id_helper++;
			if (id_helper == list.size() || how_to_correct == CORRECT_FROM_END)
				break;
		}
		Picture *last = list[filter_count - 1].filter->get_result_image();
		Mat res = last->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE));
		imshow("pic_filter", res);

		hist = last->getHistogram(160, 132, currentChannel);
		imshow("pic_histogram", hist);
	}else
	{
		imshow("pic_filter", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_FILTER)));
		hist = picture->getHistogram(160, 132, currentChannel);
		imshow("pic_histogram", hist);
	}
}

void save_image(OPENFILENAME& _ofn)
{
	WCHAR path[260];
	ZeroMemory(&_ofn, sizeof(_ofn));
	_ofn.lStructSize = sizeof(_ofn);
	_ofn.hwndOwner = procHwnd;
	_ofn.lpstrFile = path;
	_ofn.lpstrFile[0] = '\0';
	_ofn.nMaxFile = sizeof(path);
	_ofn.lpstrFilter = L"Image files (*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0"
		"All files\0*.*\0";
	_ofn.lpstrDefExt = L"jpg";
	_ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetSaveFileName(&_ofn) == TRUE)
	{
		char fileName[260];
		char fileBuffer = ' ';
		WideCharToMultiByte(CP_ACP, 0, path, -1, fileName, 260, &fileBuffer, NULL);
		Picture *toSave = list.back().filter->get_result_image();
		imwrite(string(fileName), toSave->image);
		imageIsSaved = true;
	}
}

void save_video(OPENFILENAME& _ofn)
{
	WCHAR path[260];
	ZeroMemory(&_ofn, sizeof(_ofn));
	_ofn.lStructSize = sizeof(_ofn);
	_ofn.hwndOwner = procHwnd;
	_ofn.lpstrFile = path;
	_ofn.lpstrFile[0] = '\0';
	_ofn.nMaxFile = sizeof(path);
	_ofn.lpstrFilter = L"MP4 Videos\0*.mp4\0"
		"AVI Videos\0*.avi\0";
	_ofn.lpstrDefExt = L"avi";
	_ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetSaveFileName(&_ofn) == TRUE)
	{
		EnableWindow(GetDlgItem(procHwnd, BTN_ADD), false);
		char fileName[260];
		char fileBuffer = ' ';
		WideCharToMultiByte(CP_ACP, 0, path, -1, fileName, 260, &fileBuffer, NULL);
		
		VideoWriter to_save;
		if (from_where_comes == COMES_FROM_VIDEO)
		{
			VideoCapture video(videoPath);
			to_save.open(string(fileName), 1145656920, video.get(CAP_PROP_FPS), Size(clip[0].cols, clip[0].rows), true);
		}
		else
			to_save.open(string(fileName), 1145656920, 16, Size(clip[0].cols, clip[0].rows), true);

		if (to_save.isOpened())
		{
			for (int i = 0; i < clip.size(); i++)
			{
				picture = new Picture(clip[i]);
				if (from_where_comes == COMES_FROM_HOG)
					to_save << picture->image;
				if (from_where_comes == COMES_FROM_DIRECT || from_where_comes == COMES_FROM_VIDEO)
				{
					Picture *filter_frame = apply_all_filters();
					to_save << filter_frame->image;
				}
			}
		}
		to_save.release();
		MessageBox(NULL, L"Video guardado correctamente", L"Información", MB_OK | MB_ICONINFORMATION);
	}
}

Picture* apply_all_filters()
{
	if(filter_count != 0)
	{
		int index = 0;
		while(1)
		{
			if (index == 0)
			{
				list[index].filter->set_image(picture->image);
				list[index].filter->apply();
			}
			else
			{
				list[index].filter->set_image(list[index - 1].filter->get_result_image()->image);
				list[index].filter->apply();
			}
			index++;
			if (index == list.size())
				break;
		}
		return list.back().filter->get_result_image();
	}
	return picture;
}

float rand_range(float _min, float _max) {
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * _max + (_min);
}