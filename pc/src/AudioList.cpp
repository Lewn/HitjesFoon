#include "AudioList.h"

AudioList::AudioList(GUI &gui, Config &config) : gui(gui), retriever(gui) {
    DIR *dir;
    // save the hitjeslist string in own memory for future reference
    FILE *listFile = NULL;
    do {
        listFilePath = config.getHitjesList();
        listFile = fopen(listFilePath.c_str(), "r");
        if (listFile == NULL) {
            gui.printlevel(LWARNING, "Invalid list file specified, '%s', %d\n", config.getHitjesList().c_str(), errno);
        } else {
            // get the last backslash from hitjesPath for making relative paths
            hitjesPath = config.getHitjesPath();
            dir = opendir(hitjesPath.c_str());
            if (hitjesPath.empty() || dir == NULL) {
                SAFE_CLOSE(listFile);
                gui.printlevel(LWARNING, "Invalid hitjespath specified, '%s', %d", hitjesPath.c_str(), errno);
            }
            closedir(dir);
            // TODO use boost filesystem
#ifdef _WIN32   // Stupid windows paths
            if (hitjesPath.back() != '\\') {
                hitjesPath += '\\';
            }
#else
            if (hitjesPath.back() != '/') {
                hitjesPath += '/';
            }
#endif
        }
        if (listFile == NULL) {
            gui.printlevel(LINFO, "\n");
            if (!config.nextHitjesConfig()) {
                throw "No valid hitjes config found";
            }
            gui.printlevel(LINFO, "Trying next hitjes config...\n");
        }
    } while (listFile == NULL);
    SAFE_CLOSE(listFile);

    // Seed our RNG with the current time
    generator.seed(chrono::high_resolution_clock::now().time_since_epoch().count());

    gui.printlevel(LINFO, "Creating the hitjeslist\n");
    for (int i = 1; i < 1000; i++) {
        hitjes.push_back(Hitje(i, hitjesPath));
    }
    gui.events().hitjeChange(boost::bind(&AudioList::hitjeUpdate, this, _1));
    update();
}

AudioList::~AudioList() {
}

const Hitje &AudioList::getHitje(int hitIndex) {
    if (hitIndex <= 0 || hitIndex > 999) {
        throw "Hitindex out of range";
    }
    return hitjes[hitIndex - 1];
}

const Hitje &AudioList::getRandom() {
    gui.printlevel(LDEBUG, "Randomising one of %d hitjes\n", validHitjes.size());
    uniform_int_distribution<int> distribution(0, validHitjes.size() - 1);
    int randIndex = distribution(generator);
    gui.printlevel(LDEBUG, "Randomised %d\n", randIndex);
    return getHitje(validHitjes[randIndex]);
}

void AudioList::hitjeUpdate(const Hitje &hitje) {
    // Propagate external hitje changes back to list
    if (hitjes[hitje.hitIndex - 1] != hitje) {
        Hitje oldHitje(hitjes[hitje.hitIndex - 1]);
        hitjes[hitje.hitIndex - 1] = hitje;
        // Write the changes to the file
        if (oldHitje.hitIndex != hitje.hitIndex || oldHitje.title != hitje.title || oldHitje.artist != hitje.artist) {
            // Only write when actually something interesting happened to the file
            writeUpdate();
            // Add hitje to valid hitjes
            auto itr = find(validHitjes.begin(), validHitjes.end(), hitje.hitIndex);
            if (hitje && itr == validHitjes.end()) {
                // Valid, however not yet in the list, add it
                validHitjes.push_back(hitje.hitIndex);
            } else if (!hitje && itr != validHitjes.end()) {
                // Invalid, however is in the list, remove it
                validHitjes.erase(itr);
            }
        }
    }
}

void AudioList::update() {
    int hitIndex;
    ifstream listFileStream;
    // Construct a new list of valid hitjes
    validHitjes.clear();

    gui.printlevel(LBGINFO, "Updating hitjeslist\n");

    // Enable exceptions on fail to open or read
    listFileStream.exceptions(ifstream::failbit | ifstream::badbit);
    listFileStream.open(listFilePath.c_str(), ifstream::binary | ifstream::in);
    hitIndex = skipInvalidLines(listFileStream);
    while (!listFileStream.eof()) {
        if (hitIndex > 0 && hitIndex <= 999) {
            // Got hitje successfully
            gui.printlevel(LDEBUG, "Found hitje %d\n", hitIndex);
        }
        do {
            hitIndex = readLine(listFileStream);
        } while (!listFileStream.eof() && hitIndex == 0);
    }
    listFileStream.close();

    // All hitjes must start with a three digit number,
    // so delete all files that don't have this or whose
    // number is not in the hitjes list
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(hitjesPath.c_str())) != NULL) {
        char intbuf[4];
        intbuf[3] = '\0';
        // Loop over all files in hitjes directory
        while ((ent = readdir(dir)) != NULL) {
            string entryName(ent->d_name);
            // TODO also use boost filesystem for reading directories
            // Don't try and alter level up entries....
            if (entryName != "." && entryName != "..") {
                // Get first three digits and parse int.
                strncpy(intbuf, ent->d_name, 3);
                hitIndex = atoi(intbuf);
                if (hitjes[hitIndex - 1]) {
                    string hitjeFilename = hitjes[hitIndex - 1].createFilename();
                    // Check for incorrect or duplicate filenames
                    if (hitjeFilename == entryName) {
                        // Don't delete correct files
                        continue;
                    }
                }
                string absPath = hitjesPath + entryName;
                gui.printlevel(LWARNING, "Deleting unused file '%s'\n", entryName.c_str());
                remove(absPath.c_str());
            }
        }
        closedir(dir);
    } else {
        gui.printlevel(LERROR, "Could not clean hitjes, directory not accessible\n");
    }

    for (const Hitje &hitje : hitjes) {
        // And copy all hitjes to the gui, to apply the changes
        // Only changed hitjes are actually forwarded
        gui.setHitje(hitje);
        // Keep the list of valid hitjes
        if (hitje) {
            validHitjes.push_back(hitje.hitIndex);
        }
    }

    gui.printlevel(LBGINFO, "Successfully updated hitjeslist\n");
}


void AudioList::writeUpdate() {
    ifstream inputStream;
    ofstream outputStream;
    // Enable exceptions on fail to open or read
    inputStream.exceptions(ifstream::failbit | ifstream::badbit);
    outputStream.exceptions(ifstream::failbit | ifstream::badbit);

    gui.printlevel(LBGINFO, "Writing update file\n");

    // Generate a random bu file (just an easy safety measure for possibly restoring lost file)
    // Only 100 backups are kept at max, keeping usage not too much
    // TODO generate using boost filesystem, extension is fucked up now...
    string buFile = listFilePath + "_bu";
    buFile += to_string(rand() % 100);
    rename(listFilePath.c_str(), buFile.c_str());
    try {
        // Open justly created bu stream as input file, to read it's contents
        inputStream.open(buFile.c_str(), ifstream::binary | ifstream::in);
        // Write previous contents and newly updated hitjes to the regular file
        outputStream.open(listFilePath.c_str(), ofstream::trunc | ofstream::binary | ofstream::out);
        // Read and write all lines
        while (!inputStream.eof()) {
            readLine(inputStream, &outputStream);
        }
        gui.printlevel(LBGINFO, "Read and written whole file successfully\n");
        inputStream.close();
        outputStream.close();
        return;
    } catch (const std::exception &e) {
        gui.printlevel(LERROR, "While writing update, exception caught: %s\n", e.what());
    } catch (const char *e) {
        gui.printlevel(LERROR, "While writing update, error thrown: %s\n", e);
    } catch (...) {
        // If anything goes wrong, nothing bad happened, we just can't detail to user
        gui.printlevel(LERROR, "While writing update, something bad happened...\n");
    }
    gui.printlevel(LBGINFO, "Some error occured, unrolling changes\n");
    // Something bad happened, unroll changes
    // Meaning we delete the newly created (corrupt) file
    remove(listFilePath.c_str());
    // And moving the backup back as regular file
    rename(buFile.c_str(), listFilePath.c_str());
    gui.printlevel(LBGINFO, "Done unrolling changes\n");
}

int AudioList::skipInvalidLines(ifstream &listFileStream) {
    int hitIndex;
    // loop through all lines without hitjes
    do {
        hitIndex = readLine(listFileStream);
    } while (!listFileStream.eof() && hitIndex == 0);
    gui.printlevel(LDEBUG, "Skipped all non hitjes lines\n");
    return hitIndex;
}

int AudioList::readLine(ifstream &listFileStream, ofstream *fileOutput) {
    int hitIndex;
    // Use a buffer to ensure line by line output
    // on errors the next line can be extracted without problem
    string line;
    gui.printlevel(LDEBUG, "Retrieving next line\n");
    // If no more characters are available, stop prematurely
    // prevents exceptions on get()
    if (listFileStream.peek() == EOF) { return 0; }
    // Read a line in buffer
    getline(listFileStream, line);

    if (fileOutput != NULL) {
        // If we want to write to file, we only need to parse the index
        hitIndex = parseHitIndex(line);
    } else {
        // Try to parse the whole line
        hitIndex = parseLine(line);
    }
    if (hitIndex == 0) {
        // Some of the information was missing or wrong
        if (fileOutput != NULL) {
            // Just keep the contents
            (*fileOutput) << line << "\n";
        }
        // Signal an invalid line
        return 0;
    }

    // Depending on whether we have an output file we want to read or write
    // Either parse the media file, or write the updated hitjes
    if (fileOutput == NULL) {
        // Try to create the media file for this hitje
        gui.printlevel(LDEBUG, "Creating media file\n");
        retriever.createMediaFile(hitjes[hitIndex - 1]);
    } else {
        // Some valid data, copy all the data that was valid
        (*fileOutput) << to_string(hitIndex) << ';'
                      << hitjes[hitIndex - 1].title << ';'
                      << hitjes[hitIndex - 1].artist << '\n';
    }
    gui.printlevel(LDEBUG, "Done retrieving line\n");
    // If the file existed already, we have a valid index
    return hitIndex;
}


int AudioList::parseHitIndex(string &line) {
    int hitIndex;
    istringstream lineStream(line);

    // read hit index
    lineStream >> hitIndex;
    if (lineStream.fail() || hitIndex <= 0 || hitIndex > 999) {
        gui.printlevel(LDEBUG, "Incorrect hitindex, skipping\n");
        return 0;
    }
    return hitIndex;
}

int AudioList::parseLine(string &line) {
    int hitIndex;
    string title, artist;
    istringstream lineStream(line);

    gui.printlevel(LDEBUG, "Parsing line: \n%s\n", line.c_str());

    // read hit index
    lineStream >> hitIndex;
    if (lineStream.fail() || hitIndex <= 0 || hitIndex > 999) {
        gui.printlevel(LDEBUG, "Incorrect hitindex, skipping\n");
        return 0;
    }
    // skip until ;
    lineStream.ignore(numeric_limits<streamsize>::max(), ';');
    // read until ; as title
    getline(lineStream, title, ';');
    if (lineStream.fail()) {
        gui.printlevel(LERROR, "Error parsing title for hitindex %d\n", hitIndex);
        return 0;
    }
    // title is correct, store it
    hitjes[hitIndex - 1].title = title;
    // read until ; as artist
    getline(lineStream, artist, ';');
    if (lineStream.fail()) {
        gui.printlevel(LDEBUG, "Error parsing artist for %d\n", hitIndex);
        return 0;
    }
    // artist is correct, store it
    hitjes[hitIndex - 1].artist = artist;

    if (hitjes[hitIndex - 1].title.empty() || hitjes[hitIndex - 1].artist.empty()) {
        gui.printlevel(LDEBUG, "Either title or artist not set for hitje %d, skipping\n", hitIndex);
        return 0;
    }

    gui.printlevel(LDEBUG, "Got %d: '%s' '%s'\n", hitIndex, title.c_str(), artist.c_str());
    return hitIndex;
}



