#ifndef PLATFORM_WINDOWS
#include <FileDialog.h>
#include <filesystem>

#include <gtk/gtk.h>

std::string CreateFileDialog(FileDialogType dialogType, const char* filters)
{
    // Initialize GTK if it hasn't been initialized yet
    if (!g_test_initialized()) {
        int argc = 0;
        char **argv = nullptr;
        gtk_init(&argc, &argv);
    }

    GtkWidget *dialog;
    std::string result;

    // Create a file dialog based on the dialogType
    if (dialogType == FileDialogType::Open) {
        dialog = gtk_file_chooser_dialog_new(
            "Open File", nullptr, GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Open", GTK_RESPONSE_ACCEPT,
            nullptr);
    } else { // Save dialog
        dialog = gtk_file_chooser_dialog_new(
            "Save File", nullptr, GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Save", GTK_RESPONSE_ACCEPT,
            nullptr);
    }

    // Set filters if provided
    if (filters != nullptr) {
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_add_pattern(filter, filters);
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    }

    // Set the initial directory to the current directory
    const char* currentDir = std::filesystem::current_path().c_str();
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), currentDir);

    // Show the dialog and wait for the user's response
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    // Handle the response
    if (response == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        result = std::string(filename);
        g_free(filename); // Free the filename after use
    }

    // Clean up the dialog after use
    gtk_widget_destroy(dialog);

    return result;
}

std::string PeekDirectory(void)
{
    // Initialize GTK if it hasn't been initialized yet
    if (!g_test_initialized()) {
        int argc = 0;
        char **argv = nullptr;
        gtk_init(&argc, &argv);
    }

    GtkWidget *dialog;
    std::string result;

    // Create a folder chooser dialog
    dialog = gtk_file_chooser_dialog_new(
        "Select Folder", nullptr, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Select", GTK_RESPONSE_ACCEPT,
        nullptr);

    // Show the dialog and wait for the user's response
    int response = gtk_dialog_run(GTK_DIALOG(dialog));

    // Handle the response
    if (response == GTK_RESPONSE_ACCEPT) {
        char* folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        result = std::string(folder);
        g_free(folder); // Free the folder string after use
    }

    // Clean up the dialog after use
    gtk_widget_destroy(dialog);

    return result;
}

#endif