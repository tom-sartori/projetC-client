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
        if (isDebugMode) { printf("file -send filename\n"); }
        pthread_create(&fileThread, NULL, sendFileThreaded, param);
    }
    else if (strcmp("-get", regexGroupList[1]) == 0) {
        // User get file from the server.
        if (isDebugMode) { printf("file -get filename\n"); }
        pthread_create(&fileThread, NULL, receiveFileThreaded, param);
    }
    else {
        // User send file to other user.
        if (isDebugMode) { printf("file username filename\n"); }

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

        window_print_white(FILE_LIST_HEADER_CLIENT);

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
//                printf("%s\n", file->d_name);
                window_print_white("\t");
                window_print_white(file->d_name);
                window_print_white("\n");
            }
        }
        window_print_white("\n");
        closedir(directory);
    }
    else if (strcmp(regexGroupList[1], "s") == 0) {
        // Server files listing.
        window_print_white(FILE_LIST_HEADER_SERVER);
        sendMessage(message);
    }

//    free(regexGroupList[0]);
//    free(regexGroupList[1]);
//    free(regexGroupList[2]);
}

/**
 * Action used to join another channel.
 *
 * @param message
 */
void joinAction (char *message) {
    sendMessage(message);
    int newPort = receiveNewPort();

    if (newPort == 404) {
        // Channel not found.
        window_print_red(CHANNEL_NOT_FOUND);
    }
    else if (newPort == 400) {
        // Bad request.
        window_print_red(ALREADY_IN_CHANNEL);
    }
    else if (newPort == 409) {
        // Bad request.
        window_print_red(CHANNEL_FULL);
    }
    else {
        // Port received.
        int newServerSocket = connectToServer(IP, newPort);
        int oldSocket = acceptedSocketDescriptor;
        acceptedSocketDescriptor = newServerSocket;
        window_print_cyan(CHANNEL_WELCOME);

        // Set the new channel name.
        char *newChannelName = receiveMessageFrom(oldSocket);
        set_title(newChannelName);
        free(newChannelName);
    }
}

void discoAction(){
    if(isDiscoMode==true){
        isDiscoMode = false;
        window_print_green("Disco mode disabled\n");
    }
    else {
        isDiscoMode = true;
        window_print_green("LETS DISCOOO !\n");
        pthread_create(&discoThread,NULL, &disco,NULL);
    }
}

void pewPewAction(){
    raimbowOnce(250000);
}
