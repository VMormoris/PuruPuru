#pragma once

#include <string>

/**
* @brief Enumaration for every kind of File Dialog
*/
enum class FileDialogType : int32_t {
	None = 0x00,
	Open = 0x01,
	Save = 0x02
};

/**
* @brief Creates a File Dialog Window
* @param dialogType Type for the kind of FileDialog you are intresting for
* @param filters Filters for what file you are intresting in
* @returns A string containing the filepath of the choosen file
* @warnings An empty string is returned if the Dialog window was canceled
*/
std::string CreateFileDialog(FileDialogType dialogType, const char* filters = "Puru Puru File (*.puru)\0*.puru\0");

std::string PeekDirectory(void);