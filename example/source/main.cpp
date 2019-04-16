// SimpleIniParser
// Copyright (C) 2019 Steven Mattera
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <iostream>
#include <switch.h>
#include <Swirl.hpp>

using namespace swirl;
using namespace std;

void swirlProgressUpdate(WebRequest * request, double progress);
void swirlOnCompleted(WebRequest * request);
void swirlOnError(WebRequest * request, string error);

int main(int argc, char **argv) {
    SessionManager::initialize();
    consoleInit(NULL);

    // Global Settings
    SessionManager::userAgent = "SwirlExample/1.0.0";
    SessionManager::requestHeaders.insert(
        pair<string, string>(
            "Cache-Control",
            "no-cache"
        )
    );

    // Callbacks
    SessionManager::onProgressChanged = swirlProgressUpdate;
    SessionManager::onCompleted = swirlOnCompleted;
    SessionManager::onError = swirlOnError;

    // Draw the progress.
    cout << "Downloading:   0%\n";
    consoleUpdate(NULL);

    // Make the request.
    WebRequest * kosmosReleases = new WebRequest("https://api.github.com/repos/AtlasNX/Kosmos/releases");
    SessionManager::makeRequest(kosmosReleases);

    int state = 0;
    while(appletMainLoop())
    {
        hidScanInput();
        if (hidKeysDown(CONTROLLER_P1_AUTO)) {
            consoleClear();
            cout << "\x1b[0;0H";

            if (state == 0) {
                map<string, string>::iterator reqIt;
                for (auto it = kosmosReleases->response.headers.begin(); it != kosmosReleases->response.headers.end(); it++) {
                    cout << it->first << ": " << it->second << "\n";
                }
                cout << "\n" << "Press any key to view a preview of the response body.\n";
            }
            else if (state == 1) {
                consoleClear();
                cout << "\x1b[0;0HFirst 256 characters of the response body:\n\n" << kosmosReleases->response.rawResponseBody.substr(0, 256) << "\n";
                cout << "\n" << "Press any key to close.\n";
            }
            else {
                break;
            }

            state++;
        }

        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    SessionManager::dealloc();
    delete kosmosReleases;
    return 0;
}

void swirlProgressUpdate(WebRequest * request, double progress) {
    int progressPercentage = (progress < 1) ? progress * 100 : 100;
    if (progressPercentage < 10) {
        cout << "\x1b[1;14H  " << progressPercentage;
    } else if (progressPercentage < 100) {
        cout << "\x1b[1;14H " << progressPercentage;
    } else {
        cout << "\x1b[1;14H" << progressPercentage;
    }
    consoleUpdate(NULL);
}

void swirlOnCompleted(WebRequest * request) {
    cout << "\x1b[2;0HDownload Completed with status code " << request->response.statusCode << "!\n\n";
    cout << "Press any key to view response headers.\n";
    consoleUpdate(NULL);
}

void swirlOnError(WebRequest * request, string error) {
    cout << "\x1b[2;0HError: " << error << "\n\n";
    cout << "Press any key to close.\n";
    consoleUpdate(NULL);
}