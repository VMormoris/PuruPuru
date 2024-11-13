#ifdef PLATFORM_WINDOWS
#include <FileDialog.h>
#include <filesystem>

#include <shlobj_core.h>


std::string CreateFileDialog(FileDialogType dialogType, const char* filters)
{
	const auto dir = (std::filesystem::current_path()).string();
	OPENFILENAMEA ofn;
	char szFile[256] = { 0 };
	memset(&ofn, 0, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	//Window* window = GetContext()->GlobalWindow;
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filters;
	ofn.lpstrInitialDir = dir.c_str();
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	switch (dialogType)
	{
	case FileDialogType::Open:
		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		break;
	case FileDialogType::Save:
		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		break;
	default:
		break;
	}

	return std::string();
}

std::string PeekDirectory(void)
{
	BROWSEINFOA odn = { 0 };
	char szFile[256] = { 0 };

	ITEMIDLIST* pidl = static_cast<ITEMIDLIST*>(SHBrowseForFolderA(&odn));
	if (pidl)
	{
		char folder[_MAX_PATH];
		SHGetPathFromIDListA(pidl, folder);
		SHFree(pidl);
		return std::string(folder);
	}
	return std::string();
}
#endif