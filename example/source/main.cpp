/*
 * Swurl
 * Copyright (c) 2019 Steven Mattera
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above 
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <iostream>
#include <switch.h>
#include <Swurl.hpp>

using namespace swurl;
using namespace std;

void swurlProgressUpdate(WebRequest * request, double progress);
void swurlOnCompleted(WebRequest * request);
void swurlOnError(WebRequest * request, string error);

int main(int argc, char **argv) {
    SessionManager::initialize();
    consoleInit(NULL);

    // Global Settings
    SessionManager::userAgent = "SwurlExample/1.0.0";
    SessionManager::requestHeaders.insert(
        pair<string, string>(
            "Cache-Control",
            "no-cache"
        )
    );

    // Callbacks
    SessionManager::onProgressChanged = swurlProgressUpdate;
    SessionManager::onCompleted = swurlOnCompleted;
    SessionManager::onError = swurlOnError;

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

void swurlProgressUpdate(WebRequest * request, double progress) {
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

void swurlOnCompleted(WebRequest * request) {
    cout << "\x1b[2;0HDownload Completed with status code " << request->response.statusCode << "!\n\n";
    cout << "Press any key to view response headers.\n";
    consoleUpdate(NULL);
}

void swurlOnError(WebRequest * request, string error) {
    cout << "\x1b[2;0HError: " << error << "\n\n";
    cout << "Press any key to close.\n";
    consoleUpdate(NULL);
}