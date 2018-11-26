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
#include "FilterFactory.h"
using namespace std;
using namespace cv;

#define CORRECT_FROM_START 0
#define CORRECT_FROM_CENTER 1
#define CORRECT_FROM_END 2

HINSTANCE hInst;
Picture *picture;
Mat hist;
bool gbStopEvent = false;
uint currentChannel = CHANNEL_RGB;
FilterFactory ff = FilterFactory::get_instance();
HWND procHwnd;
HMENU procHwndMenu;
int filter_count = 0;
int id_helper = -1;
int how_to_correct = -1;
bool imageIsSaved = false;
vector <Mat> clip;

struct Record
{
	Record *before, *after;
	FilterList type;
	Filter *filter;
}*index = nullptr, *aux = nullptr, *helper = nullptr;

BOOL CALLBACK DialogProcedure(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK FiltersProcedure(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI cameraFilter(LPVOID);
DWORD WINAPI cameraHOG(LPVOID);
void add_filter(FilterList);
void correct_all_filters(Record*&);
void save_image(OPENFILENAME&);
void clear_linked_list();
Picture* apply_all_filters();
float randRange(float, float);

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
			if(helper != nullptr && !imageIsSaved)
				wantToSave = (MessageBox(NULL, L"¿Seguro que deseas continuar sin guardar?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES) ? false : true;

			if(imageIsSaved || !wantToSave)
			{
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
					imageIsSaved = false;

					filter_count = -1;
					clear_linked_list();
					SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_RESETCONTENT, 0, 0);
				}
			}
			else
				save_image(ofn);
			break;
		case ID_IMAGEN_GUARDAR:
			save_image(ofn);
			break;
		case ID_VIDEO_TOMAR:
			if (helper != nullptr && !imageIsSaved)
				wantToSave = (MessageBox(NULL, L"¿Seguro que deseas continuar sin guardar?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES) ? false : true;

			if (imageIsSaved || !wantToSave)
			{
				gbStopEvent = false;
				filter_count = -1;
				clear_linked_list();
				SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_RESETCONTENT, 0, 0);

				DWORD cameraThread;
				hCamera = CreateThread(0, 0, cameraFilter, NULL, 0, &cameraThread);
				EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, BTN_ADD), true);
			}
			break;
		case ID_VIDEO_HOG:
			if (helper != nullptr && !imageIsSaved)
				wantToSave = (MessageBox(NULL, L"¿Seguro que deseas continuar sin guardar?", L"Alerta", MB_YESNO | MB_ICONWARNING) == IDYES) ? false : true;

			if (imageIsSaved || !wantToSave)
			{
				gbStopEvent = false;
				filter_count = -1;
				clear_linked_list();
				SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_RESETCONTENT, 0, 0);

				DWORD cameraThread;
				hCamera = CreateThread(0, 0, cameraHOG, NULL, 0, &cameraThread);
				EnableWindow(GetDlgItem(hwnd, RD_RGBCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_RCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_GCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, RD_BCHANNEL), true);
				EnableWindow(GetDlgItem(hwnd, BTN_ADD), false);
			}
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
		case BTN_DELETE:
			{
				Record *second_aux;
				Record *to_delete = second_aux = aux;
				if(aux == index)
				{
					aux = aux->after->after;
					index = second_aux = aux->before;
					index->before = nullptr;
					to_delete->after = nullptr;
					how_to_correct = CORRECT_FROM_START;
				}else if(aux == helper)
				{
					helper = aux = aux->before;
					second_aux = aux->before;
					helper->after = nullptr;
					to_delete->before = nullptr;
					how_to_correct = CORRECT_FROM_END;
				}else
				{
					second_aux = to_delete->before;
					aux = to_delete->after;
					to_delete->before = nullptr;
					to_delete->after = nullptr;
					second_aux->after = aux;
					aux->before = second_aux;
					how_to_correct = CORRECT_FROM_CENTER;
				}
				delete to_delete;
				correct_all_filters(second_aux);
				SendMessage(GetDlgItem(hwnd, LB_RECORD), LB_DELETESTRING, id_helper, 0);
				filter_count--;
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
					aux = index;
					for (int i = 0; i < id_helper; i++)
						aux = aux->after;
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
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, NULL);
		break;
	}
	return FALSE;
}

DWORD WINAPI cameraFilter(LPVOID lpParameter)
{
	VideoCapture camara(0);
	while(!gbStopEvent)
	{
		Mat frame;
		bool exito = camara.read(frame);
		if(exito)
		{
			picture = new Picture(frame);
			imshow("pic_image", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE)));
			Picture *filter_frame = apply_all_filters();
			if (filter_frame != nullptr)
				imshow("pic_filter", filter_frame->GetSquareImage(GetDlgItem(procHwnd, PIC_FILTER)));

			/*hist = picture->getHistogram(160, 132, currentChannel);
			imshow("pic_histogram", hist);*/


		}
	}
	return 0;
}

DWORD WINAPI cameraHOG(LPVOID lpParameter)
{
	VideoCapture camara(0);
	const int maxColorCount = 10;
	vector<cv::Scalar> colors;

	srand(GetTickCount());
	for (int i = 0; i < maxColorCount; i++) {
		const Scalar color = Scalar(randRange(0, 255), randRange(0, 255), randRange(0, 255));
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
			picture = new Picture(frame);
			imshow("pic_image", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE)));
			Mat hist = picture->getHistogram(255, 255, currentChannel);
			resize(picture->image, aux, aux.size(), 0, 0, INTER_LINEAR);
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
			string number = std::to_string(found_filtered.size());

			putText(picture->image, number.c_str(), cvPoint(30, 50),
				FONT_ITALIC, 3, cvScalar(255, 255, 255), 1, CV_AA);

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
			imshow("pic_histogram", picture->getHistogram(160, 132, currentChannel));
			imshow("pic_filter", picture->GetSquareImage(GetDlgItem(procHwnd, PIC_FILTER)));

		}
	}
	return 0;
}

void add_filter(FilterList _filter)
{
	ff.change_choise(_filter);
	if (index == nullptr)
	{
		index = aux = helper = new Record();
		index->before = nullptr;
		index->after = nullptr;
		index->type = _filter;
		index->filter = ff.createFilter();
		index->filter->set_image(picture->image);
		index->filter->apply();
	}
	else
	{
		aux = new Record();
		aux->before = helper;
		aux->after = nullptr;
		helper->after = aux;
		aux->type = _filter;
		aux->filter = ff.createFilter();
		Picture *prev = helper->filter->get_result_image();
		aux->filter->set_image(prev->image);
		aux->filter->apply();
		helper = aux;
	}
	Picture *last = helper->filter->get_result_image();
	Mat res = last->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE));
	imshow("pic_filter", res);

	hist = last->getHistogram(160, 132, currentChannel);
	imshow("pic_histogram", hist);

	filter_count++;
	const int index = static_cast<int>(SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(helper->filter->get_name())));
	SendMessage(GetDlgItem(procHwnd, LB_RECORD), LB_SETITEMDATA, index, static_cast<LPARAM>(filter_count));

	EnableMenuItem(procHwndMenu, ID_IMAGEN_GUARDAR, MF_ENABLED);
}

void correct_all_filters(Record*& _second_aux)
{
	while(1)
	{
		Picture *prev = _second_aux->filter->get_result_image();
		if(how_to_correct == CORRECT_FROM_START)
		{
			aux->filter->set_image(picture->image);
			aux->filter->apply();
			how_to_correct = CORRECT_FROM_CENTER;
		}
		else if(how_to_correct == CORRECT_FROM_CENTER)
		{
			aux->filter->set_image(prev->image);
			aux->filter->apply();
		}
		if (aux == helper)
		{
			_second_aux = nullptr;
			delete _second_aux;
			break;
		}
		else
		{
			aux = aux->after;
			_second_aux = _second_aux->after;
		}
	}
	Picture *last = helper->filter->get_result_image();
	Mat res = last->GetSquareImage(GetDlgItem(procHwnd, PIC_IMAGE));
	imshow("pic_filter", res);

	hist = last->getHistogram(160, 132, currentChannel);
	imshow("pic_histogram", hist);
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
		Picture *toSave = helper->filter->get_result_image();
		imwrite(string(fileName), toSave->image);
		imageIsSaved = true;
	}
}

void clear_linked_list(){
	if(index != nullptr)
	{
		bool stop = false;
		aux = index;
		index = nullptr;
		helper = nullptr;
		while (!stop)
		{
			Record* to_delete = aux;
			aux = aux->after;
			if (aux == nullptr)
				stop = true;
			else
				aux->before = nullptr;
			to_delete->after = nullptr;
			delete to_delete;
		}
	}
};

Picture* apply_all_filters()
{
	if(index != nullptr)
	{
		aux = index;
		while(1)
		{
			try
			{
				if (aux == nullptr)
					throw "no jalo";

				if (aux == index)
				{
					aux->filter->set_image(picture->image);
					aux->filter->apply();
				}
				else
				{
					aux->filter->set_image(aux->before->filter->get_result_image()->image);
					aux->filter->apply();
				}
				if (aux == helper)
					break;
				aux = aux->after;
			}
			catch (char* error)
			{
				break;
			}
		}
		return helper->filter->get_result_image();
	}
	return nullptr;
}

float randRange(float min, float max) {
	return ((float)rand() / (float)RAND_MAX) * max + (min);
}