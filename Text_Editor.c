#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_SIZE 50
#define MAX_TEXT_SIZE 1000

// Creating a structure
typedef struct 
{
    char filename[MAX_FILENAME_SIZE];
    char text[MAX_TEXT_SIZE + 1]; // Increased the size by 1 to accommodate the null terminator
    int length;
    char undo_text[MAX_TEXT_SIZE + 1]; // Increased the size by 1 to accommodate the null terminator
    int undo_length;
    char redo_text[MAX_TEXT_SIZE + 1]; // Increased the size by 1 to accommodate the null terminator
    int redo_length;
    char deleted_word[MAX_TEXT_SIZE + 1]; // Increased the size by 1 to accommodate the null terminator
    int deleted_position;
} TextEditor;

// Function to initialize the text editor
void initialize(TextEditor* editor) 
{
    editor->filename[0] = '\0';  // Set the filename to an empty string
    editor->length = 0;         // Set the length of the text to 0
    editor->undo_length = 0;    // Set the undo length to 0
    editor->redo_length = 0;    // Set the redo length to 0
}

// Function to create a file
void createFile(TextEditor* editor, const char* filename)
{
    strncpy(editor->filename, filename, MAX_FILENAME_SIZE);       // Copy the filename to the editor's filename field
    editor->filename[MAX_FILENAME_SIZE - 1] = '\0';               // Ensure the filename is null-terminated
    printf("File '%s' created.\n", editor->filename);
}

// Function to append text to the editor
void append(TextEditor* editor, const char* text)
{
    strncpy(editor->undo_text, editor->text, MAX_TEXT_SIZE);      // Copy the current text to the undo_text field for undo operation
    editor->undo_length = editor->length;                          // Save the current length for undo operation

    // Calculate the available space for appending text
    int availableSpace = MAX_TEXT_SIZE - editor->length;

    if (availableSpace <= 0)
    {
        printf("No space available for appending text.\n");
        return;
    }

    // Append the input text to the editor's text field, limited by the available space
    strncat(editor->text, text, availableSpace - 1); // Subtract 1 to leave space for the null terminator

    // Find the last word in the appended text
    char* lastSpace = strrchr(editor->text, ' ');
    if (lastSpace != NULL)
    {
        // Calculate the position of the last word in the text
        int wordPosition = lastSpace - editor->text + 1;

        // Save the last word and its position
        strncpy(editor->deleted_word, lastSpace + 1, MAX_TEXT_SIZE);
        editor->deleted_position = editor->length + 1 - wordPosition;
    }

    // Update the length of the text
    editor->length = strlen(editor->text);

    printf("Text appended.\n");
}

// Function to delete a word from the editor
void Delete(TextEditor* editor, const char* word)
{
    int wordLength = strlen(word);
    int position = 0;
    char* found = strstr(editor->text, word);

    if (found == NULL)
    {
        printf("Word not found.\n");
        return;
    }

    // Calculate the position of the found word in the text
    position = found - editor->text;

    // Save the current state for undo operation
    strncpy(editor->undo_text, editor->text, MAX_TEXT_SIZE);
    editor->undo_length = editor->length;
    strncpy(editor->redo_text, editor->text, MAX_TEXT_SIZE);
    editor->redo_length = editor->length;

    // Shift the remaining text after the word to the left
    memmove(found, found + wordLength, MAX_TEXT_SIZE - position - wordLength);

    // Update the length of the text
    editor->length = strlen(editor->text);

    printf("Word '%s' deleted.\n", word);
}

// Function to perform undo operation
void undo(TextEditor* editor)
{
    if (editor->undo_length == 0)
    {
        printf("Undo not available.\n");
        return;
    }

    // Save the current state for redo operation
    strncpy(editor->redo_text, editor->text, MAX_TEXT_SIZE);
    editor->redo_length = editor->length;

    // Restore the previously saved text from the undo_text field
    strncpy(editor->text, editor->undo_text, MAX_TEXT_SIZE);
    editor->length = editor->undo_length;

    // Clear the undo length for subsequent undo/redo operations
    editor->undo_length = 0;

    printf("Undo completed.\n");
}

// Function to perform redo operation
void redo(TextEditor* editor)
{
    if (editor->redo_length == 0)
    {
        printf("Redo not available.\n");
        return;
    }

    // Save the current state for undo operation
    strncpy(editor->undo_text, editor->text, MAX_TEXT_SIZE);
    editor->undo_length = editor->length;

    // Restore the previously saved text from the redo_text field
    strncpy(editor->text, editor->redo_text, MAX_TEXT_SIZE);
    editor->length = editor->redo_length;

    // Clear the redo length for subsequent undo/redo operations
    editor->redo_length = 0;

    printf("Redo completed.\n");
}

// Function to rename the file
void renameFile(TextEditor* editor, const char* newFilename) 
{
    strncpy(editor->filename, newFilename, MAX_FILENAME_SIZE);       // Copy the new filename to the editor's filename field
    editor->filename[MAX_FILENAME_SIZE - 1] = '\0';                   // Ensure the filename is null-terminated
    printf("File renamed to '%s'.\n", editor->filename);
}

// Function to save the file
void saveFile(const TextEditor* editor)
{
    FILE* file = fopen(editor->filename, "w");           // Open the file in write mode
    if (file == NULL)
    {
        printf("Error saving file.\n");
        return;
    }
    fwrite(editor->text, sizeof(char), editor->length, file);   // Write the text to the file
    fclose(file);                                             // Close the file
    printf("File '%s' saved.\n", editor->filename);
}

// Function to display the editor's text
void display(const TextEditor* editor)
{
    if (editor->length > 0)
        printf("Text: %s\n", editor->text);  // Print the entire text without specifying the length
    else
        printf("Text is empty.\n");
}

int main()
{
    TextEditor editor;
    initialize(&editor);

    char choice;
    char filename[MAX_FILENAME_SIZE];
    char text[MAX_TEXT_SIZE];
    char word[MAX_TEXT_SIZE];

    do {
        printf("\n--------Text Editor Menu---------\n");
        printf("-> Create File(c)\n");
        printf("-> Append Text(a)\n");
        printf("-> Display Text(d)\n");
        printf("-> Rename(e)\n");
        printf("-> Delete(t)\n");
        printf("-> Undo(u)\n");
        printf("-> Redo(r)\n");
        printf("-> Save File(s)\n");
        printf("-> Exit(q)\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (choice) 
        {
            case 'c':
                printf("Enter filename: ");
                scanf("%s", filename);
                createFile(&editor, filename);
                break;
            case 'a':
                printf("Enter text to append: ");
                scanf(" %[^\n]", text);
                append(&editor, text);
                break;
            case 'd':
                display(&editor);
                break;
            case 'e':
                printf("Enter new filename: ");
                scanf("%s", filename);
                renameFile(&editor, filename);
                break;
            case 't':
                printf("Enter the word: ");
                scanf("%s", word);
                Delete(&editor, word);
                break;
            case 'u':
                undo(&editor);
                break;
            case 'r':
                redo(&editor);
                break;
            case 's':
                saveFile(&editor);
                break;
            case 'q':
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 'q');

    return 0;
}
