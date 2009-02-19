
#include "Header.h"
#include <commctrl.h>
#include "ConEmu.h"

const u8 chSetsNums[19] = {0, 178, 186, 136, 1, 238, 134, 161, 177, 129, 130, 77, 255, 204, 128, 2, 222, 162, 163};
int upToFontHeight=0;
HWND ghOpWnd=NULL;

CSettings::CSettings()
{
	InitSettings();
}

CSettings::~CSettings()
{
}

void CSettings::InitSettings()
{
    //memset(&gSet, 0, sizeof(gSet)); // -- Class!!! ����� ������ ��� �������!

//------------------------------------------------------------------------
///| Moved from VirtualConsole |//////////////////////////////////////////
//------------------------------------------------------------------------
	_tcscpy(Config, _T("Software\\ConEmu"));

	BufferHeight = 0;
	LogFont.lfHeight = 16;
	LogFont.lfWidth = 0;
	LogFont.lfEscapement = LogFont.lfOrientation = 0;
	LogFont.lfWeight = FW_NORMAL;
	LogFont.lfItalic = LogFont.lfUnderline = LogFont.lfStrikeOut = FALSE;
	LogFont.lfCharSet = DEFAULT_CHARSET;
	LogFont.lfOutPrecision = OUT_TT_PRECIS;
	LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	LogFont.lfQuality = ANTIALIASED_QUALITY;
	LogFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
    _tcscpy(LogFont.lfFaceName, _T("Lucida Console"));
    _tcscpy(LogFont2.lfFaceName, _T("Lucida Console"));
    isTryToCenter = false;
	isTabFrame = true;
	
	Registry RegConColors, RegConDef;
	if (RegConColors.OpenKey(_T("Console"), KEY_READ))
	{
		RegConDef.OpenKey(HKEY_USERS, _T(".DEFAULT\\Console"), KEY_READ);

		TCHAR ColorName[] = _T("ColorTable00");
		for (uint i = 0x10; i--;)
		{
			ColorName[10] = i/10 + '0';
			ColorName[11] = i%10 + '0';
			if (!RegConColors.Load(ColorName, (DWORD *)&Colors[i]))
				RegConDef.Load(ColorName, (DWORD *)&Colors[i]);
		}

		RegConDef.CloseKey();
		RegConColors.CloseKey();
	}

//------------------------------------------------------------------------
///| Default settings |///////////////////////////////////////////////////
//------------------------------------------------------------------------
    _tcscpy(pBgImage, _T("c:\\back.bmp"));
    isFixFarBorders = true;
    bgImageDarker = 0;
    wndHeight = 25; // NightRoman
    wndWidth = 80;  // NightRoman
    isConVisible = false;
    isDefCopy = 2;
    nSlideShowElapse = 2500;
    nIconID = IDI_ICON1;
    isRClickSendKey = 2;
	_tcscpy(szTabPanels, _T("Panels"));
	_tcscpy(szTabEditor, _T("[%s]"));
	_tcscpy(szTabEditorModified, _T("[%s] *"));
	_tcscpy(szTabViewer, _T("{%s}"));
	nTabLenMax = 20;
	
	isAllowDetach = 0;
	isCreateAppWindow = false;
    isScrollTitle = true;
    ScrollTitleLen = 22;
}

void CSettings::LoadSettings()
{
    DWORD inSize = LogFont.lfHeight;
    TCHAR inFont[MAX_PATH], inFont2[MAX_PATH];
    _tcscpy(inFont, LogFont.lfFaceName);
    _tcscpy(inFont2, LogFont2.lfFaceName);
    DWORD Quality = LogFont.lfQuality;
    gConEmu.WindowMode = rMaximized;
    DWORD FontCharSet = LogFont.lfCharSet;
    bool isBold = (LogFont.lfWeight>=FW_BOLD), isItalic = (LogFont.lfItalic!=FALSE);
    
//------------------------------------------------------------------------
///| Loading from registry |//////////////////////////////////////////////
//------------------------------------------------------------------------
    Registry reg;
    if (reg.OpenKey(Config, KEY_READ)) // NightRoman
    {
        reg.Load(_T("FontName"), inFont);
        reg.Load(_T("FontName2"), inFont2);
        reg.Load(_T("CmdLine"), Cmd);
        reg.Load(_T("BackGround Image"), pBgImage);
        reg.Load(_T("bgImageDarker"), &bgImageDarker);
        reg.Load(_T("FontSize"), &inSize);
        reg.Load(_T("FontSizeX"), &FontSizeX);
		reg.Load(_T("FontSizeX3"), &FontSizeX3);
        reg.Load(_T("FontSizeX2"), &FontSizeX2);
        reg.Load(_T("FontCharSet"), &FontCharSet);
        reg.Load(_T("Anti-aliasing"), &Quality);
        reg.Load(_T("WindowMode"), &gConEmu.WindowMode);
        reg.Load(_T("ConWnd X"), &wndX); if (wndX<-10) wndX = 0;
        reg.Load(_T("ConWnd Y"), &wndY); if (wndY<-10) wndY = 0;
        reg.Load(_T("ConWnd Width"), &wndWidth);
        reg.Load(_T("ConWnd Height"), &wndHeight);
        reg.Load(_T("CursorType"), &isCursorV);
        reg.Load(_T("CursorColor"), &isCursorColor);
        reg.Load(_T("Experimental"), &isFixFarBorders);
        reg.Load(_T("RightClick opens context menu"), &isRClickSendKey);
        reg.Load(_T("AltEnter"), &isSentAltEnter);
        reg.Load(_T("BackGround Image show"), &isShowBgImage);
        reg.Load(_T("FontBold"), &isBold);
        reg.Load(_T("FontItalic"), &isItalic);
        reg.Load(_T("ForceMonospace"), &isForceMonospace);
        reg.Load(_T("Update Console handle"), &isConMan);
        reg.Load(_T("Dnd"), &isDnD);
        reg.Load(_T("DefCopy"), &isDefCopy);
        reg.Load(_T("GUIpb"), &isGUIpb);
        reg.Load(_T("Tabs"), &isTabs);
		reg.Load(_T("TabFrame"), &isTabFrame);
		reg.Load(_T("TabMargins"), &rcTabMargins);
        reg.Load(_T("ConVisible"), &isConVisible);
        reg.Load(_T("SlideShowElapse"), &nSlideShowElapse);
        reg.Load(_T("IconID"), &nIconID);
		reg.Load(_T("TabPanels"), &szTabPanels);
		reg.Load(_T("TabEditor"), &szTabEditor);
		reg.Load(_T("TabEditorModified"), &szTabEditorModified);
		reg.Load(_T("TabViewer"), &szTabViewer);
		reg.Load(_T("TabLenMax"), &nTabLenMax);
		reg.Load(_T("ScrollTitle"), &isScrollTitle);
		reg.Load(_T("ScrollTitleLen"), &ScrollTitleLen);
		reg.Load(_T("TryToCenter"), &isTryToCenter);
		reg.Load(_T("CreateAppWindow"), &isCreateAppWindow);
		reg.Load(_T("AllowDetach"), &isAllowDetach);
        reg.CloseKey();
    }

    /*if (wndWidth)
        pVCon->TextWidth = wndWidth;
    if (wndHeight)
        pVCon->TextHeight = wndHeight;*/

    /*if (wndHeight && wndWidth)
    {
        COORD b = {wndWidth, wndHeight};
      SetConsoleWindowSize(b,false); // Maximus5 - �� �������� � NightRoman
      //MoveWindow(hConWnd, 0, 0, 1, 1, 0);
      //SetConsoleScreenBufferSize(pVCon->hConOut(), b);
      //MoveWindow(hConWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
    }*/

    LogFont.lfHeight = inSize;
    LogFont.lfWidth = FontSizeX;
    _tcscpy(LogFont.lfFaceName, inFont);
    _tcscpy(LogFont2.lfFaceName, inFont2);
    LogFont.lfQuality = Quality;
    if (isBold)
        LogFont.lfWeight = FW_BOLD;
    LogFont.lfCharSet = (BYTE) FontCharSet;
    if (isItalic)
        LogFont.lfItalic = true;

	// pVCon ��� �� �������!
    if (isShowBgImage && pVCon)
        LoadImageFrom(pBgImage);
}

void CSettings::UpdateMargins(RECT arcMargins)
{
	if (memcmp(&arcMargins, &rcTabMargins, sizeof(rcTabMargins))==0)
		return;

	rcTabMargins = arcMargins;

	Registry reg;
	if (reg.OpenKey(Config, KEY_WRITE))
	{
		reg.Save(_T("TabMargins"), rcTabMargins);
		reg.CloseKey();
	}
}

BOOL CSettings::SaveSettings()
{
    Registry reg;
    if (reg.OpenKey(_T("Console"), KEY_WRITE))
    {
        TCHAR ColorName[] = _T("ColorTable00");
        for (uint i = 0x10; i--;)
        {
            ColorName[10] = i/10 + '0';
            ColorName[11] = i%10 + '0';
            reg.Save(ColorName, (DWORD)Colors[i]);
        }
        reg.CloseKey();

      if (reg.OpenKey(Config, KEY_WRITE)) // NightRoman
        {
            GetDlgItemText(ghOpWnd, tCmdLine, Cmd, MAX_PATH);
			WINDOWPLACEMENT wpl; wpl.length = sizeof(wpl);
            GetWindowPlacement(ghWnd, &wpl);
			if (wpl.showCmd == SW_SHOWNORMAL)
			{
				wndX = wpl.rcNormalPosition.left;
				wndY = wpl.rcNormalPosition.top;
			}

            reg.Save(_T("CmdLine"), Cmd);
            reg.Save(_T("FontName"), LogFont.lfFaceName);
            reg.Save(_T("FontName2"), LogFont2.lfFaceName);
            reg.Save(_T("BackGround Image"), pBgImage);
            reg.Save(_T("bgImageDarker"), bgImageDarker);
            reg.Save(_T("FontSize"), LogFont.lfHeight);
            reg.Save(_T("FontSizeX"), FontSizeX);
            reg.Save(_T("FontSizeX2"), FontSizeX2);
			reg.Save(_T("FontSizeX3"), FontSizeX3);
            reg.Save(_T("FontCharSet"), LogFont.lfCharSet);
            reg.Save(_T("Anti-aliasing"), LogFont.lfQuality);
            reg.Save(_T("WindowMode"), isFullScreen ? rFullScreen : IsZoomed(ghWnd) ? rMaximized : rNormal);
            reg.Save(_T("CursorType"), isCursorV);
            reg.Save(_T("CursorColor"), isCursorColor);
            reg.Save(_T("Experimental"), isFixFarBorders);
            reg.Save(_T("RightClick opens context menu"), isRClickSendKey);
            reg.Save(_T("AltEnter"), isSentAltEnter);
            reg.Save(_T("Dnd"), isDnD);
            reg.Save(_T("DefCopy"), isDefCopy);
            reg.Save(_T("GUIpb"), isGUIpb);
            reg.Save(_T("Tabs"), isTabs);
            reg.Save(_T("BackGround Image show"), isShowBgImage);
            reg.Save(_T("FontBold"), LogFont.lfWeight == FW_BOLD);
            reg.Save(_T("FontItalic"), LogFont.lfItalic);
            reg.Save(_T("ForceMonospace"), isForceMonospace);
            reg.Save(_T("Update Console handle"), isConMan);

            reg.Save(_T("ConWnd Width"), wndWidth);
            reg.Save(_T("ConWnd Height"), wndHeight);
            reg.Save(_T("ConWnd X"), wndX);
            reg.Save(_T("ConWnd Y"), wndY);

			reg.Save(_T("ScrollTitle"), isScrollTitle);
			reg.Save(_T("ScrollTitleLen"), ScrollTitleLen);
            
            reg.CloseKey();
            
            //if (isTabs==1) ForceShowTabs();
            
            //MessageBoxA(ghOpWnd, "Saved.", "Information", MB_ICONINFORMATION);
            return TRUE;
        }
    }

    MessageBoxA(ghOpWnd, "Failed", "Information", MB_ICONERROR);
	return FALSE;
}


bool CSettings::ShowColorDialog(HWND HWndOwner, COLORREF *inColor)
{
    CHOOSECOLOR cc;                 // common dialog box structure
    //static COLORREF acrCustClr[16]; // array of custom colors

    // Initialize CHOOSECOLOR
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = HWndOwner;
    cc.lpCustColors = (LPDWORD) Colors;
    cc.rgbResult = *inColor;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc))
    {
        *inColor = cc.rgbResult;
        return true;
    }
    return false;
}

BOOL CALLBACK CSettings::EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID aFontCount)
{
    int far * aiFontCount = (int far *) aFontCount;

    // Record the number of raster, TrueType, and vector
    // fonts in the font-count array.

    if (FontType & RASTER_FONTTYPE)
        aiFontCount[0]++;
    else if (FontType & TRUETYPE_FONTTYPE)
        aiFontCount[2]++;
    else
        aiFontCount[1]++;

    SendDlgItemMessage(ghOpWnd, tFontFace, CB_ADDSTRING, 0, (LPARAM) lplf->lfFaceName);
    SendDlgItemMessage(ghOpWnd, tFontFace2, CB_ADDSTRING, 0, (LPARAM) lplf->lfFaceName);

    if (aiFontCount[0] || aiFontCount[1] || aiFontCount[2])
        return TRUE;
    else
        return FALSE;

    UNREFERENCED_PARAMETER( lplf );
    UNREFERENCED_PARAMETER( lpntm );
}

LRESULT CSettings::OnInitDialog()
{
	{
		HDC hdc = GetDC(ghOpWnd);
		int aFontCount[] = { 0, 0, 0 };
		EnumFontFamilies(hdc, (LPCTSTR) NULL, (FONTENUMPROC) EnumFamCallBack, (LPARAM) aFontCount);
		DeleteDC(hdc);
	    
		TCHAR szTitle[MAX_PATH]; szTitle[0]=0;
		int nConfLen = _tcslen(Config);
		int nStdLen = strlen("Software\\ConEmu");
		if (nConfLen>(nStdLen+1))
			wsprintf(szTitle, _T("Settings (%s)..."), (Config+nStdLen+1));
		else
			_tcscpy(szTitle, _T("Settings..."));
		SetWindowText ( ghOpWnd, szTitle );
	}

	SendDlgItemMessage(ghOpWnd, tFontFace, CB_SELECTSTRING, -1, (LPARAM)LogFont.lfFaceName);
	SendDlgItemMessage(ghOpWnd, tFontFace2, CB_SELECTSTRING, -1, (LPARAM)LogFont2.lfFaceName);

	{
		const BYTE FSizes[] = {0, 8, 10, 12, 14, 16, 18, 20, 24, 26, 28, 30, 32, 34, 36, 40, 46, 50, 52, 72};
		for (uint i=0; i < sizeofarray(FSizes); i++)
		{
			wsprintf(temp, _T("%i"), FSizes[i]);
			if (i > 0)
				SendDlgItemMessage(ghOpWnd, tFontSizeY, CB_ADDSTRING, 0, (LPARAM) temp);
			SendDlgItemMessage(ghOpWnd, tFontSizeX, CB_ADDSTRING, 0, (LPARAM) temp);
			SendDlgItemMessage(ghOpWnd, tFontSizeX2, CB_ADDSTRING, 0, (LPARAM) temp);
			SendDlgItemMessage(ghOpWnd, tFontSizeX3, CB_ADDSTRING, 0, (LPARAM) temp);
		}

		wsprintf(temp, _T("%i"), LogFont.lfHeight);
		upToFontHeight = LogFont.lfHeight;
		if( SendDlgItemMessage(ghOpWnd, tFontSizeY, CB_SELECTSTRING, -1, (LPARAM)temp) == CB_ERR )
			SetDlgItemText(ghOpWnd, tFontSizeY, temp);

		wsprintf(temp, _T("%i"), FontSizeX);
		if( SendDlgItemMessage(ghOpWnd, tFontSizeX, CB_SELECTSTRING, -1, (LPARAM)temp) == CB_ERR )
			SetDlgItemText(ghOpWnd, tFontSizeX, temp);

		wsprintf(temp, _T("%i"), FontSizeX2);
		if( SendDlgItemMessage(ghOpWnd, tFontSizeX2, CB_SELECTSTRING, -1, (LPARAM)temp) == CB_ERR )
			SetDlgItemText(ghOpWnd, tFontSizeX2, temp);

		wsprintf(temp, _T("%i"), FontSizeX3);
		if( SendDlgItemMessage(ghOpWnd, tFontSizeX3, CB_SELECTSTRING, -1, (LPARAM)temp) == CB_ERR )
			SetDlgItemText(ghOpWnd, tFontSizeX3, temp);
	}

	{
		const char *ChSets[] = {"ANSI", "Arabic", "Baltic", "Chinese Big 5", "Default", "East Europe",
			"GB 2312", "Greek", "Hebrew", "Hangul", "Johab", "Mac", "OEM", "Russian", "Shiftjis",
			"Symbol", "Thai", "Turkish", "Vietnamese"};

		u8 num = 4;
		for (uint i=0; i < 19; i++)
		{
			SendDlgItemMessageA(ghOpWnd, tFontCharset, CB_ADDSTRING, 0, (LPARAM) ChSets[i]);
			if (chSetsNums[i] == LogFont.lfCharSet) num = i;
		}
		SendDlgItemMessage(ghOpWnd, tFontCharset, CB_SETCURSEL, num, 0);
	}

	SetDlgItemText(ghOpWnd, tCmdLine, Cmd);
	SetDlgItemText(ghOpWnd, tBgImage, pBgImage);
	CheckDlgButton(ghOpWnd, rBgSimple, BST_CHECKED);

	TCHAR tmp[10];
	wsprintf(tmp, _T("%i"), bgImageDarker);
	SendDlgItemMessage(ghOpWnd, tDarker, EM_SETLIMITTEXT, 3, 0);
	SetDlgItemText(ghOpWnd, tDarker, tmp);

	SendDlgItemMessage(ghOpWnd, slDarker, TBM_SETRANGE, (WPARAM) true, (LPARAM) MAKELONG(0, 255));
	SendDlgItemMessage(ghOpWnd, slDarker, TBM_SETPOS  , (WPARAM) true, (LPARAM) bgImageDarker);

	if (isShowBgImage)
		CheckDlgButton(ghOpWnd, cbBgImage, BST_CHECKED);
	else
	{
		EnableWindow(GetDlgItem(ghOpWnd, tBgImage), false);
		EnableWindow(GetDlgItem(ghOpWnd, tDarker), false);
		EnableWindow(GetDlgItem(ghOpWnd, slDarker), false);
		EnableWindow(GetDlgItem(ghOpWnd, bBgImage), false);
	}

	switch(LogFont.lfQuality)
	{
	case NONANTIALIASED_QUALITY:
		CheckDlgButton(ghOpWnd, rNoneAA, BST_CHECKED);
		break;
	case ANTIALIASED_QUALITY:
		CheckDlgButton(ghOpWnd, rStandardAA, BST_CHECKED);
		break;
	case CLEARTYPE_NATURAL_QUALITY:
		CheckDlgButton(ghOpWnd, rCTAA, BST_CHECKED);
		break;
	}
	if (isFixFarBorders)   CheckDlgButton(ghOpWnd, cbFixFarBorders, BST_CHECKED);
	if (isCursorColor) CheckDlgButton(ghOpWnd, cbCursorColor, BST_CHECKED);
	if (isRClickSendKey) CheckDlgButton(ghOpWnd, cbRClick, (isRClickSendKey==1) ? BST_CHECKED : BST_INDETERMINATE);
	if (isSentAltEnter) CheckDlgButton(ghOpWnd, cbSendAE, BST_CHECKED);

	if (isDnD)
	{
		CheckDlgButton(ghOpWnd, cbDnD, BST_CHECKED);
		EnableWindow(GetDlgItem(ghOpWnd, cbDnDCopy), true);
	}
	else
		EnableWindow(GetDlgItem(ghOpWnd, cbDnDCopy), false);
	if (isDefCopy) CheckDlgButton(ghOpWnd, cbDnDCopy, (isDefCopy==1) ? BST_CHECKED : BST_INDETERMINATE);

	if (isGUIpb) CheckDlgButton(ghOpWnd, cbGUIpb, BST_CHECKED);
	if (isTabs) CheckDlgButton(ghOpWnd, cbTabs, (isTabs==1) ? BST_CHECKED : BST_INDETERMINATE);
	if (isCursorV)
		CheckDlgButton(ghOpWnd, rCursorV, BST_CHECKED);
	else
		CheckDlgButton(ghOpWnd, rCursorH, BST_CHECKED);
	if (isForceMonospace)
		CheckDlgButton(ghOpWnd, cbMonospace, BST_CHECKED);
	if (isConMan)
		CheckDlgButton(ghOpWnd, cbIsConMan, BST_CHECKED);

	if (LogFont.lfWeight == FW_BOLD) CheckDlgButton(ghOpWnd, cbBold, BST_CHECKED);
	if (LogFont.lfItalic)            CheckDlgButton(ghOpWnd, cbItalic, BST_CHECKED);

	if (isFullScreen)
		CheckRadioButton(ghOpWnd, rNormal, rFullScreen, rFullScreen);
	else if (IsZoomed(ghWnd))
		CheckRadioButton(ghOpWnd, rNormal, rFullScreen, rMaximized);
	else
		CheckRadioButton(ghOpWnd, rNormal, rFullScreen, rNormal);

	wsprintf(temp, _T("%i"), wndWidth);
	SetDlgItemText(ghOpWnd, tWndWidth, temp);
	SendDlgItemMessage(ghOpWnd, tWndWidth, EM_SETLIMITTEXT, 3, 0);

	wsprintf(temp, _T("%i"), wndHeight);
	SetDlgItemText(ghOpWnd, tWndHeight, temp);
	SendDlgItemMessage(ghOpWnd, tWndHeight, EM_SETLIMITTEXT, 3, 0);

	if (!isFullScreen && !IsZoomed(ghWnd))
	{
		EnableWindow(GetDlgItem(ghOpWnd, tWndWidth), true);
		EnableWindow(GetDlgItem(ghOpWnd, tWndHeight), true);

	}
	else
	{
		EnableWindow(GetDlgItem(ghOpWnd, tWndWidth), false);
		EnableWindow(GetDlgItem(ghOpWnd, tWndHeight), false);

	}

	#define getR(inColorref) (byte)inColorref
	#define getG(inColorref) (byte)(inColorref >> 8)
	#define getB(inColorref) (byte)(inColorref >> 16)

	for (uint i = 0; i < 16; i++)
	{
		SendDlgItemMessage(ghOpWnd, 1100 + i, EM_SETLIMITTEXT, 11, 0);
		wsprintf(temp, _T("%i %i %i"), getR(Colors[i]), getG(Colors[i]), getB(Colors[i]));
		SetDlgItemText(ghOpWnd, 1100 + i, temp);
	}

	{
		RECT rect;
		GetWindowRect(ghOpWnd, &rect);
		MoveWindow(ghOpWnd, GetSystemMetrics(SM_CXSCREEN)/2 - (rect.right - rect.left)/2, GetSystemMetrics(SM_CYSCREEN)/2 - (rect.bottom - rect.top)/2, rect.right - rect.left, rect.bottom - rect.top, false);
	}

	return 0;
}

LRESULT CSettings::OnButtonClicked(WPARAM wParam, LPARAM lParam)
{
    WORD CB = LOWORD(wParam);
    switch(wParam)
    {
    case IDOK:
    case IDCANCEL:
    case IDCLOSE:
		if (isTabs==1) gConEmu.ForceShowTabs(TRUE); else
		if (isTabs==0) gConEmu.ForceShowTabs(FALSE); // ��� ��� ���� '==2', �� ��� ����� �� ������������
        SendMessage(ghOpWnd, WM_CLOSE, 0, 0);
        break;

    case rNoneAA:
    case rStandardAA:
    case rCTAA:
        LogFont.lfQuality = wParam == rNoneAA ? NONANTIALIASED_QUALITY : wParam == rStandardAA ? ANTIALIASED_QUALITY : CLEARTYPE_NATURAL_QUALITY;
        DeleteObject(pVCon->hFont);
        pVCon->hFont = 0;
        LogFont.lfWidth = FontSizeX;
        pVCon->Update(true);
        InvalidateRect(ghWnd, NULL, FALSE);
        break;

    case bSaveSettings:
        if (SaveSettings())
			SendMessage(ghOpWnd,WM_COMMAND,IDOK,0);
        break;

    case rNormal:
    case rFullScreen:
    case rMaximized:
        gConEmu.SetWindowMode(wParam);
        break;

    case cbFixFarBorders:
        isFixFarBorders = !isFixFarBorders;

        pVCon->Update(true);
        InvalidateRect(ghWnd, NULL, FALSE);
        break;

    case cbCursorColor:
        isCursorColor = !isCursorColor;

        pVCon->Update(true);
        InvalidateRect(ghWnd, NULL, FALSE);
        break;

    case cbBold:
    case cbItalic:
        {
            if (wParam == cbBold)
                LogFont.lfWeight = SendDlgItemMessage(ghOpWnd, cbBold, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED ? FW_BOLD : FW_NORMAL;
            else if (wParam == cbItalic)
                LogFont.lfItalic = SendDlgItemMessage(ghOpWnd, cbItalic, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED ? true : false;

            LogFont.lfWidth = FontSizeX;
            HFONT hFont = pVCon->CreateFontIndirectMy(&LogFont);
            if (hFont)
            {
                DeleteObject(pVCon->hFont);
                pVCon->hFont = hFont;

                pVCon->Update(true);
				if (!isFullScreen && !IsZoomed(ghWnd))
                    gConEmu.SyncWindowToConsole();
				else
                    gConEmu.SyncConsoleToWindow();
				gConEmu.ReSize();
                InvalidateRect(ghWnd, 0, 0);
            }
        }
        break;

    case cbBgImage:
        isShowBgImage = SendDlgItemMessage(ghOpWnd, cbBgImage, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED ? true : false;
        EnableWindow(GetDlgItem(ghOpWnd, tBgImage), isShowBgImage);
        EnableWindow(GetDlgItem(ghOpWnd, tDarker), isShowBgImage);
        EnableWindow(GetDlgItem(ghOpWnd, slDarker), isShowBgImage);
		EnableWindow(GetDlgItem(ghOpWnd, bBgImage), isShowBgImage);

        if (isShowBgImage && isBackgroundImageValid)
            SetBkMode(pVCon->hDC, TRANSPARENT);
        else
            SetBkMode(pVCon->hDC, OPAQUE);

        pVCon->Update(true);
        InvalidateRect(ghWnd, NULL, FALSE);
        break;

    case cbRClick:
        //isRClickSendKey = !isRClickSendKey;
        switch(IsDlgButtonChecked(ghOpWnd, cbRClick)) {
            case BST_UNCHECKED:
                isRClickSendKey=0; break;
            case BST_CHECKED:
                isRClickSendKey=1; break;
            case BST_INDETERMINATE:
                isRClickSendKey=2; break;
        }
        break;

    case cbSendAE:
        isSentAltEnter = !isSentAltEnter;
        break;

    case cbDnD:
        isDnD = !isDnD;
        EnableWindow(GetDlgItem(ghOpWnd, cbDnDCopy), isDnD);
        break;
    
    case cbDnDCopy:
        switch(IsDlgButtonChecked(ghOpWnd, cbDnDCopy)) {
            case BST_UNCHECKED:
                isDefCopy=0; break;
            case BST_CHECKED:
                isDefCopy=1; break;
            case BST_INDETERMINATE:
                isDefCopy=2; break;
        }
        //isDefCopy = !isDefCopy;
        break;
    
    case cbGUIpb:
        isGUIpb = !isGUIpb;
        break;
    
    case cbTabs:
        switch(IsDlgButtonChecked(ghOpWnd, cbTabs)) {
            case BST_UNCHECKED:
                isTabs=0; break;
            case BST_CHECKED:
                isTabs=1; break;
            case BST_INDETERMINATE:
                isTabs=2; break;
        }
        //isTabs = !isTabs;
        break;

    case cbMonospace:
        isForceMonospace = !isForceMonospace;

        pVCon->Update(true);
        InvalidateRect(ghWnd, NULL, FALSE);
        break;

    case cbIsConMan:
        isConMan = !isConMan;

        pVCon->Update(true);
        InvalidateRect(ghWnd, NULL, FALSE);
        break;

    case rCursorH:
    case rCursorV:
        if (wParam == rCursorV)
            isCursorV = true;
        else
            isCursorV = false;

        pVCon->Update(true);
        InvalidateRect(ghWnd, NULL, FALSE);
        break;

	case bBgImage:
		{
			GetDlgItemText(ghOpWnd, tBgImage, temp, MAX_PATH);
			OPENFILENAME ofn; memset(&ofn,0,sizeof(ofn));
			ofn.lStructSize=sizeof(ofn);
			ofn.hwndOwner = ghOpWnd;
			ofn.lpstrFilter = _T("Bitmap images (*.bmp)\0*.bmp\0\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = temp;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = _T("Choose background image");
			ofn.Flags = OFN_ENABLESIZING|OFN_NOCHANGEDIR
					| OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn))
				SetDlgItemText(ghOpWnd, tBgImage, temp);
		}
		break;

    default:
        if (CB >= 1000 && CB <= 1015)
        {
            COLORREF color = Colors[CB - 1000];
            if( ShowColorDialog(ghOpWnd, &color) )
            {
                Colors[CB - 1000] = color;
                wsprintf(temp, _T("%i %i %i"), getR(color), getG(color), getB(color));
                SetDlgItemText(ghOpWnd, CB + 100, temp);
                InvalidateRect(GetDlgItem(ghOpWnd, CB), 0, 1);

				gConEmu.m_Back.Refresh();

                pVCon->Update(true);
                InvalidateRect(ghWnd, NULL, FALSE);
            }
        }
    }

	return 0;
}

LRESULT CSettings::OnEditChanged(WPARAM wParam, LPARAM lParam)
{
    WORD TB = LOWORD(wParam);
    if (TB >= 1100 && TB <= 1115)
    {
        int r, g, b;
        GetDlgItemText(ghOpWnd, TB, temp, MAX_PATH);
        TCHAR *sp1 = _tcschr(temp, ' '), *sp2;
        if (sp1 && *(sp1+1) && *(sp1+1) != ' ')
        {
            sp2 = _tcschr(sp1+1, ' ');
            if (sp2 && *(sp2+1) && *(sp2+1) != ' ')
            {
                *sp1 = 0;
                sp1++;
                *sp2 = 0;
                sp2++;
                r = klatoi(temp); g = klatoi(sp1), b = klatoi(sp2);
                if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255 && Colors[TB - 1100] != RGB(r, g, b))
                {
                    Colors[TB - 1100] = RGB(r, g, b);
                    if (pVCon) pVCon->Update(true);
                    if (ghWnd) InvalidateRect(ghWnd, 0, 1);
                    InvalidateRect(GetDlgItem(ghOpWnd, TB - 100), 0, 1);
                }
            }
        }
    }
    else if (TB == tBgImage)
    {
        GetDlgItemText(ghOpWnd, tBgImage, temp, MAX_PATH);
        if( LoadImageFrom(temp, true) )
        {
            if (isShowBgImage && isBackgroundImageValid)
                SetBkMode(pVCon->hDC, TRANSPARENT);
            else
                SetBkMode(pVCon->hDC, OPAQUE);

            pVCon->Update(true);
            InvalidateRect(ghWnd, NULL, FALSE);
        }
    }
    else if ( (TB == tWndWidth || TB == tWndHeight) && IsDlgButtonChecked(ghOpWnd, rNormal) == BST_CHECKED )
    {
        DWORD newX, newY;
        GetDlgItemText(ghOpWnd, tWndWidth, temp, MAX_PATH);
        newX = klatoi(temp);
        GetDlgItemText(ghOpWnd, tWndHeight, temp, MAX_PATH);
        newY = klatoi(temp);

        if (newX > 24 && newY > 7)
        {
            wndWidth = newX;
            wndHeight = newY;

            COORD b = {wndWidth, wndHeight};
      gConEmu.SetConsoleWindowSize(b, false);  // NightRoman
      //MoveWindow(hConWnd, 0, 0, 1, 1, 0);
      //SetConsoleScreenBufferSize(pVCon->hConOut(), b);
      //MoveWindow(hConWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
        }

    }
    else if (TB == tDarker)
    {
        DWORD newV;
        TCHAR tmp[10];
        GetDlgItemText(ghOpWnd, tDarker, tmp, 10);
        newV = klatoi(tmp);
        if (newV < 256 && newV != bgImageDarker)
        {
            bgImageDarker = newV;
            SendDlgItemMessage(ghOpWnd, slDarker, TBM_SETPOS, (WPARAM) true, (LPARAM) bgImageDarker);
            LoadImageFrom(pBgImage);
            pVCon->Update(true);
            InvalidateRect(ghWnd, NULL, FALSE);
        }
    }

	return 0;
}

LRESULT CSettings::OnComboBox(WPARAM wParam, LPARAM lParam)
{
    if (LOWORD(wParam) == tFontFace || LOWORD(wParam) == tFontFace2)
    {
        LOGFONT* pLogFont = (LOWORD(wParam) == tFontFace) ? &LogFont : &LogFont2;
        int nID = (LOWORD(wParam) == tFontFace) ? tFontFace : tFontFace2;
        _tcscpy(temp, pLogFont->lfFaceName);
        if (HIWORD(wParam) == CBN_EDITCHANGE)
            GetDlgItemText(ghOpWnd, nID, pLogFont->lfFaceName, LF_FACESIZE);
        else
            SendDlgItemMessage(ghOpWnd, nID, CB_GETLBTEXT, SendDlgItemMessage(ghOpWnd, nID, CB_GETCURSEL, 0, 0), (LPARAM)pLogFont->lfFaceName);

        if (HIWORD(wParam) == CBN_EDITCHANGE)
        {
            LRESULT a = SendDlgItemMessage(ghOpWnd, nID, CB_FINDSTRINGEXACT, -1, (LPARAM)pLogFont->lfFaceName);
            if(a == CB_ERR)
            {
                _tcscpy(pLogFont->lfFaceName, temp);
                return -1;
            }
        }

        BYTE qWas = pLogFont->lfQuality;
        pLogFont->lfHeight = upToFontHeight;
        pLogFont->lfWidth = FontSizeX;
        HFONT hFont = pVCon->CreateFontIndirectMy(&LogFont);
        if (hFont)
        {
            if (LOWORD(wParam) == tFontFace) {
                DeleteObject(pVCon->hFont);
                pVCon->hFont = hFont;
            } else {
                DeleteObject(pVCon->hFont2);
                pVCon->hFont2 = hFont;
            }

            pVCon->Update(true);
            if (!isFullScreen && !IsZoomed(ghWnd))
                gConEmu.SyncWindowToConsole();
            else
                gConEmu.SyncConsoleToWindow();
			gConEmu.ReSize();
            InvalidateRect(ghWnd, 0, 0);

            if (LOWORD(wParam) == tFontFace) {
                wsprintf(temp, _T("%i"), pLogFont->lfHeight);
                SetDlgItemText(ghOpWnd, tFontSizeY, temp);
            }
        }
    }
    else if (LOWORD(wParam) == tFontSizeY || LOWORD(wParam) == tFontSizeX || 
		LOWORD(wParam) == tFontSizeX2 || LOWORD(wParam) == tFontSizeX3 || LOWORD(wParam) == tFontCharset)
    {
        int newSize = 0;
        if (LOWORD(wParam) == tFontSizeY || LOWORD(wParam) == tFontSizeX || 
			LOWORD(wParam) == tFontSizeX2 || LOWORD(wParam) == tFontSizeX3)
        {
            if (HIWORD(wParam) == CBN_EDITCHANGE)
                GetDlgItemText(ghOpWnd, LOWORD(wParam), temp, MAX_PATH);
            else
                SendDlgItemMessage(ghOpWnd, LOWORD(wParam), CB_GETLBTEXT, SendDlgItemMessage(ghOpWnd, LOWORD(wParam), CB_GETCURSEL, 0, 0), (LPARAM)temp);

            newSize = klatoi(temp);
        }

        if (newSize > 4 && newSize < 200 || (newSize == 0 && *temp == '0') || LOWORD(wParam) == tFontCharset)
        {
            if (LOWORD(wParam) == tFontSizeY)
                LogFont.lfHeight = upToFontHeight = newSize;
            else if (LOWORD(wParam) == tFontSizeX)
                FontSizeX = newSize;
            else if (LOWORD(wParam) == tFontSizeX2)
                FontSizeX2 = newSize;
            else if (LOWORD(wParam) == tFontSizeX3)
                FontSizeX3 = newSize;
            else if (LOWORD(wParam) == tFontCharset)
            {
                int newCharSet = SendDlgItemMessage(ghOpWnd, tFontCharset, CB_GETCURSEL, 0, 0);
                if (newCharSet != CB_ERR && newCharSet >= 0 && newCharSet < 19)
                    LogFont.lfCharSet = chSetsNums[newCharSet];
                LogFont.lfHeight = upToFontHeight;
            }
            LogFont.lfWidth = FontSizeX;

            HFONT hFont = pVCon->CreateFontIndirectMy(&LogFont);
            if (hFont)
            {
                DeleteObject(pVCon->hFont);
                pVCon->hFont = hFont;

                pVCon->Update(true);
                if (!isFullScreen && !IsZoomed(ghWnd))
                    gConEmu.SyncWindowToConsole();
                else
                    gConEmu.SyncConsoleToWindow();
				gConEmu.ReSize();
                InvalidateRect(ghWnd, 0, 0);
            }
        }
	}
	return 0;
}


bool CSettings::LoadImageFrom(TCHAR *inPath, bool abShowErrors)
{
	if (!inPath || _tcslen(inPath)>=MAX_PATH) {
		if (abShowErrors)
			MBoxA(_T("Invalid 'inPath' in CSettings::LoadImageFrom"));
		return false;
	}

    TCHAR exPath[MAX_PATH + 2];
	if (!ExpandEnvironmentStrings(inPath, exPath, MAX_PATH)) {
		if (abShowErrors) {
			TCHAR szError[MAX_PATH*2];
			DWORD dwErr = GetLastError();
			swprintf(szError, _T("Can't expand environment strings:\r\n%s\r\nError code=0x%08X\r\nImage loading failed"),
				inPath, dwErr);
			MBoxA(szError);
		}
        return false;
	}

    bool lRes = false;
    klFile file;
    if (file.Open(exPath))
    {
        char File[101];
        file.Read(File, 100);
        char *pBuf = File;
        if (*(u16*)pBuf == 'MB' && *(u32*)(pBuf + 0x0A) >= 0x36 && *(u32*)(pBuf + 0x0A) <= 0x436 && *(u32*)(pBuf + 0x0E) == 0x28 && !pBuf[0x1D] && !*(u32*)(pBuf + 0x1E))
            //if (*(u16*)pBuf == 'MB' && *(u32*)(pBuf + 0x0A) >= 0x36)
        {
            const HDC hScreenDC = GetDC(0);
            HDC hNewBgDc = CreateCompatibleDC(hScreenDC);
            HBITMAP hNewBgBitmap;
            if (hNewBgDc)
            {
                if(hNewBgBitmap = (HBITMAP)LoadImage(NULL, exPath, IMAGE_BITMAP,0,0,LR_LOADFROMFILE))
                {
					if (pVCon->hBgBitmap) { DeleteObject(pVCon->hBgBitmap); pVCon->hBgBitmap=NULL; }
					if (pVCon->hBgDc) { DeleteDC(pVCon->hBgDc); pVCon->hBgDc=NULL; }

                    pVCon->hBgDc = hNewBgDc;
                    pVCon->hBgBitmap = hNewBgBitmap;

                    if(SelectObject(pVCon->hBgDc, pVCon->hBgBitmap))
                    {
                        isBackgroundImageValid = true;
                        pVCon->bgBmp.cx = *(u32*)(pBuf + 0x12);
                        pVCon->bgBmp.cy = *(i32*)(pBuf + 0x16);
						// ������� �� ������� 4-� �������� (WinXP SP2)
						int nCxFixed = ((pVCon->bgBmp.cx+3)>>2)<<2;
                        if (klstricmp(pBgImage, inPath))
                        {
                            lRes = true;
                            _tcscpy(pBgImage, inPath);
                        }

                        struct bColor
                        {
                            u8 b;
                            u8 g;
                            u8 r;
                        };

						MCHKHEAP
							//GetDIBits ������ �� ������� 
                        bColor *bArray = new bColor[(nCxFixed+10) * pVCon->bgBmp.cy];
						MCHKHEAP

                        BITMAPINFO bInfo; memset(&bInfo, 0, sizeof(bInfo));
                        bInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
                        bInfo.bmiHeader.biWidth = nCxFixed/*pVCon->bgBmp.cx*/;
                        bInfo.bmiHeader.biHeight = pVCon->bgBmp.cy;
                        bInfo.bmiHeader.biPlanes = 1;
                        bInfo.bmiHeader.biBitCount = 24;
                        bInfo.bmiHeader.biCompression = BI_RGB;

						MCHKHEAP
                        if (!GetDIBits(pVCon->hBgDc, pVCon->hBgBitmap, 0, pVCon->bgBmp.cy, bArray, &bInfo, DIB_RGB_COLORS))
                            //MBoxA(_T("!")); //Maximus5 - ��, ��� ����� ������������
                            MBoxA(_T("!GetDIBits"));


						MCHKHEAP
						for (int y=0; y<pVCon->bgBmp.cy; y++)
						{
							int i = y*nCxFixed;
							for (int x=0; x<pVCon->bgBmp.cx; x++, i++)
							//for (int i = 0; i < pVCon->bgBmp.cx * pVCon->bgBmp.cy; i++)
							{
								bArray[i].r = klMulDivU32(bArray[i].r, bgImageDarker, 255);
								bArray[i].g = klMulDivU32(bArray[i].g, bgImageDarker, 255);
								bArray[i].b = klMulDivU32(bArray[i].b, bgImageDarker, 255);
							}
						}

						MCHKHEAP
                        if (!SetDIBits(pVCon->hBgDc, pVCon->hBgBitmap, 0, pVCon->bgBmp.cy, bArray, &bInfo, DIB_RGB_COLORS))
                            MBoxA(_T("!SetDIBits"));

						MCHKHEAP
                        delete[] bArray;
						MCHKHEAP
                    }
                }
                else
                    DeleteDC(hNewBgDc);
            }

            ReleaseDC(0, hScreenDC);
		} else {
			if (abShowErrors)
				MBoxA(_T("Only BMP files supported as background!"));
        }
        file.Close();
    }

    return lRes;
}

BOOL CALLBACK CSettings::wndOpProc(HWND hWnd2, UINT messg, WPARAM wParam, LPARAM lParam)
{
    switch (messg)
    {
    case WM_INITDIALOG:
        ghOpWnd = hWnd2;
		#ifdef _DEBUG
		if (IsDebuggerPresent())
			SetWindowPos(ghOpWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
		#endif
		gSet.OnInitDialog();
        break;

	case WM_GETICON:
		if (wParam==ICON_BIG) {
			SetWindowLong(hWnd2, DWL_MSGRESULT, (LRESULT)hClassIcon);
		} else {
			SetWindowLong(hWnd2, DWL_MSGRESULT, (LRESULT)hClassIconSm);
		}
		return 1;

    case WM_CTLCOLORSTATIC:
        for (uint i = 1000; i < 1016; i++)
            if (GetDlgItem(hWnd2, i) == (HWND)lParam)
            {
                static HBRUSH KillBrush;
                DeleteObject(KillBrush);
                KillBrush = CreateSolidBrush(gSet.Colors[i-1000]);
                return (BOOL)KillBrush;
            }
            break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            SendMessage(hWnd2, WM_CLOSE, 0, 0);
        break;

    case WM_HSCROLL:
        {
            int newV = SendDlgItemMessage(hWnd2, slDarker, TBM_GETPOS, 0, 0);
            if (newV != gSet.bgImageDarker)
            {
                gSet.bgImageDarker = newV;
                TCHAR tmp[10];
                wsprintf(tmp, _T("%i"), gSet.bgImageDarker);
                SetDlgItemText(hWnd2, tDarker, tmp);
                gSet.LoadImageFrom(gSet.pBgImage);
                pVCon->Update(true);
                InvalidateRect(ghWnd, NULL, FALSE);
            }
        }
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
			gSet.OnButtonClicked(wParam, lParam);
        }
        else if (HIWORD(wParam) == EN_CHANGE)
        {
			gSet.OnEditChanged(wParam, lParam);
        }
        else if (HIWORD(wParam) == CBN_EDITCHANGE || HIWORD(wParam) == CBN_SELCHANGE)
        {
			gSet.OnComboBox(wParam, lParam);
        }
        break;
    case WM_CLOSE:
    case WM_DESTROY:
        EndDialog(hWnd2, TRUE);
        ghOpWnd = NULL;
        break;
    default:
        return 0;
    }
    return 0;
}
