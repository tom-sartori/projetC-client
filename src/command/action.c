#include <dirent.h>
#include "actionThreaded.c"

/**
 * Shutdown the client.
 */
void disconnectAction () {
    shutdownClient();
}

/**
 * Action used to send a file to the server.
 * Create a thread which use fileActionThreaded(...).
 *
 * @param command
 * @param message
 */
void fileAction (Command *command, char *message) {
    // Get filename and add it to the file path.
    char *regexGroupList[3];
    getRegexGroup(regexGroupList, message, command->regex);
    // regexGroupList[0] = message
    // regexGroupList[1] = -send || -get || username
    // regexGroupList[2] = filename

    struct paramFileThreaded *param;
    param = malloc(sizeof(struct paramFileThreaded));
    param->message = message;
    param->username = regexGroupList[1];    // -send || -get || username
    param->filename = regexGroupList[2];

    pthread_t fileThread;

    if (strcmp("-send", regexGroupList[1]) == 0) {
        // User send file to the server.
        printf("file -send filename\n");
        pthread_create(&fileThread, NULL, sendFileThreaded, param);
    }
    else if (strcmp("-get", regexGroupList[1]) == 0) {
        // User get file from the server.
        printf("file -get filename\n");
        pthread_create(&fileThread, NULL, receiveFileThreaded, param);
    }
    else {
        // User send file to other user.
        printf("file username filename\n");

        pthread_create(&fileThread, NULL, mpSendFileThreaded, param);
    }

    // Free within threaded functions.
}

/**
 * Print directory's names within uploads/.
 * Depending on the option in message, print the client or server directory.
 *
 * @param command
 * @param message
 */
void filesAction (Command *command, char *message) {
    // Get the option. -c || -s.
    char *regexGroupList[3];
    getRegexGroup(regexGroupList, message, command->regex);

    if (strcmp(regexGroupList[1], "c") == 0) {
        // Client files listing.

        printf("Liste des fichiers disponibles dans le dossier uploads : \n");

        // Get uploads path.
        char uploadDirectoryPath[200];
        getUploadDirectoryPath(uploadDirectoryPath);

        // Get directory.
        DIR *directory;
        struct dirent *file;
        directory = opendir(uploadDirectoryPath);
        if (directory == NULL) {
            throwError("Unable to open the directory. \n", 0);
        }

        while ((file = readdir(directory)) != NULL) {
            if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {  // Don't take files : "." et "..".
//                printf("File %d : %s\n", n, file->d_name);
                printf("%s\n", file->d_name);
            }
        }
        printf("\n");
        closedir(directory);
    }
    else if (strcmp(regexGroupList[1], "s") == 0) {
        // Server files listing.
        printf("Liste des fichiers disponibles sur le serveur : \n");
        sendMessage(message);
    }

    free(regexGroupList[0]);
    free(regexGroupList[1]);
    free(regexGroupList[2]);
}
