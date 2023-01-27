#include "spotify.h"
#include "utils.h"
#include <fstream>
#include <thread>

Spotify::Spotify() = default;

void Spotify::initWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas){
    this->access_token = fetchAccessToken();
    //load the font
    if(!loadedFont.LoadFont(fetchFont(5).c_str())){
    }
    if(!loadedFontSmall.LoadFont(fetchFont(4).c_str())){
    }
}

void Spotify::fetchWidgetData(atomic_bool& request_end){
    while(!request_end){
        usleep(this->delay_speed_usec);
        cout << "Fetching song data" << endl;
        vector<string> songData = fetchCurrentSong();
        if(songData[1] != this->song){
             if(songData[0] == "200") {
                cout << "New song data" << endl;
                fetchAlbumArt(songData[4]);
                this->song = songData[1];
                this->artist = songData[2];
                this->album = songData[3];
                this->new_data = true;
                this->album_x = 0;
                this->artist_x = 0;
                this->song_x = 0;
            } else if(songData[0] == "204") {
                this->song = songData[1];
                this->artist = "";
                this->album = "";
                this->album_art = {};
            } //else {
            //     this->song = songData[1];
            //     this->artist = "";
            //     this->album = "";
            // }
        }
    }
    cout << "Exiting" << endl;
}

void Spotify::layoutWidgetFields(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas){
    cout << "Layout" << endl;
    canvas->Clear();
    int song_loc = 0;
    int album_loc = 22;
    int artist_loc = 22;
    //check if a font has been loaded
    int album_length = rgb_matrix::DrawText(canvas, loadedFontSmall, album_loc, 7, rgb_matrix::Color(30, 215, 96), NULL, album.c_str(), 0, album_x, 40, 0, true);
    int artist_length = rgb_matrix::DrawText(canvas, loadedFontSmall, artist_loc, 17, rgb_matrix::Color(30, 215, 96), NULL, artist.c_str(), 0, artist_x, 40, 0, true);
    int song_length = rgb_matrix::DrawText(canvas, loadedFont, song_loc, 30, rgb_matrix::Color(30, 215, 96), NULL, song.c_str(), 0, song_x, 62, 1, true);

    drawAlbumArt(matrix, canvas);

    //Scrolls the song, album, and artist names
    int extra_offset = song_length - (64 - song_loc) + 1;
    bool song_direction_change = false;
    if(extra_offset > 0){
        if(song_x < extra_offset && direction_left){
            song_x++;
        } else if(song_x > 0 && !direction_left){
            song_x--;
        } else {
            song_direction_change = true;
        }
    }
    bool album_direction_change = false;
    int extra_offset_album = album_length - 40;
    if(extra_offset_album > 0){
        if(album_x < extra_offset_album && direction_left){
            album_x++;
        } else if(album_x > 0 && !direction_left){
            album_x--;
        } else {
            album_direction_change = true;
        }
    }
    bool artist_direction_change = false;
    int extra_offset_artist = artist_length - 40;
    if(extra_offset_artist > 0){
        if(artist_x < extra_offset_artist && direction_left){
            artist_x++;
        } else if(artist_x > 0 && !direction_left){
            artist_x--;
        } else {
            artist_direction_change = true;
        }
    }
    if((song_direction_change || extra_offset <= 0) && (album_direction_change || extra_offset_album <= 0) && (artist_direction_change || extra_offset_artist <= 0)){
        direction_left = !direction_left;
        usleep(3000000);
    }
    canvas = matrix->SwapOnVSync(canvas);
    usleep(50000);
}

void Spotify::drawWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, atomic_bool& end){
    thread t1(&Spotify::fetchWidgetData, this, ref(end));
    // fetchWidgetData(ref(end));
    while(!end){
        do {
            this->layoutWidgetFields(matrix, canvas);
        } while(((this->album_x != 0) || (this->artist_x != 0) || (this->song_x != 0)));
    }
    t1.join();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

//create a write callback function for curl where the data is of type unsigned char
static size_t WriteCallbackChar(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::vector<unsigned char>*)userp)->insert(((std::vector<unsigned char>*)userp)->end(), (unsigned char*)contents, (unsigned char*)contents + size * nmemb);
    return size * nmemb;
}

//Function for reading string from file
string readFromFile(string fileLocation){
    string fileContent;
    ifstream fileToRead(fileLocation);
    fileToRead >> fileContent;
    fileToRead.close();
    return fileContent;
}

int Spotify::shouldSwitchWidget() {
    vector<string> songData = fetchCurrentSong();
    return (songData[0] == "200") ? 9 : 0;
}

//Fetches the album art from the url (also parses so that ui thread isnt overloaded)
void Spotify::fetchAlbumArt(string url){
    string adafruitKey = readFromFile("adafruit.key");
    string adafruitUsername = readFromFile("adafruit_username.key");
    string requestUrl = "https://io.adafruit.com/api/v2/" + adafruitUsername +"/integrations/image-formatter?x-aio-key=" + adafruitKey + "&width=20&height=20&output=BMP24&url=" + url;
    CURL *curl;
    vector<unsigned char> readBuffer;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackChar);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    cout << "fetching album art" << endl;
    curl_easy_perform(curl);
    cout << "fetched album art" << endl;
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    cout << http_code << endl;
    if(http_code == 200){
        //const char* arr = readBuffer.c_str();
        //set album art equal to byte 54 and onward of readbuffer
        //write the read buffer to a file
        vector<unsigned char> temp(readBuffer.begin() + 54, readBuffer.end());
        this->album_art = temp;
    }
}

//member function for drawing the album art
void Spotify::drawAlbumArt(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas){
   // print the size of the album art array
    if(!(this->album_art.empty())){
        for(int i = 1; i < 21; i++){
            for(int k = 1; k < 21; k++){
                int index = ((i-1) * 20 + (k-1)) * 3;
                int b = this->album_art[index];
                int g = this->album_art[index + 1];
                int r = this->album_art[index + 2];
                canvas->SetPixel(k, 21 - i, r, g, b);
                // rgb_matrix::DrawLine(canvas, k, 21 - i, k, 21 - i, rgb_matrix::Color(255, 255, 255));
            }
        }
        //rgb_matrix::SetImage(canvas, 10, 10, this->album_art.data(), 1200, 20, 20, true);
    } else {
        cout << "Load Failed" << endl;
    }
    //set the pixels surrounding the album art to black
//     for(int i = 0; i < 21; i++){
//         canvas->SetPixel(i, 0, 0, 0, 0);
//         canvas->SetPixel(i, 21, 0, 0, 0);
//         canvas->SetPixel(0, i, 0, 0, 0);
//         canvas->SetPixel(21, i, 0, 0, 0);
//     }
// }
}

//Fetches the current song
vector<string> Spotify::fetchCurrentSong(){
    cout << "fetching song" << endl;
    CURL *curl;
    string readBuffer;
    string header = "Authorization: Bearer " + this->access_token;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, header.c_str());
    string url = "https://api.spotify.com/v1/me/player/currently-playing";
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    cout << "fetching song" << endl;
    curl_easy_perform(curl);
    cout << "fetched song" << endl;
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    cout << http_code << endl;
    if(http_code == 401) {
        this->access_token = fetchAccessToken();
        return fetchCurrentSong();
    } else if(http_code == 204) {
        return {"204", "No song playing"};
    } else if(http_code == 200) {
        json j = json::parse(readBuffer);
        
        string song = j["item"]["name"];
        string artist = j["item"]["artists"][0]["name"];
        string album = j["item"]["album"]["name"];
        string art_url = j["item"]["album"]["images"][0]["url"];
        return {"200", song, artist, album, art_url};
    } else {
        return {"n200", "Error"};
    }
}

//fetch the access token from the spotify api
string Spotify::fetchAccessToken(){
    cout << "Fetching Access Token" << endl;
    string refreshToken = readFromFile("./refresh_token.key");
    string clientId = readFromFile("./spotify_client_id.key");
    string clientSecret = readFromFile("./spotify_secret.key");
    //create the curl object
    CURL *curl;
    //create the curl response object
    CURLcode res;
    //create the curl response string
    string readBuffer;
    //create the curl response string stream
    string url = "https://accounts.spotify.com/api/token";
    //create the curl request
    curl = curl_easy_init();
    //set the curl request url
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    string post_fields = "grant_type=refresh_token&refresh_token="+refreshToken+"&client_id=" + clientId + "&client_secret="+clientSecret;
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
    //set the curl request write function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    //set the curl request write data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    //set the curl request post
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    //perform the curl request
    res = curl_easy_perform(curl);
    //check for errors
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    cout << "Access Token Fetched" << endl;
    if(http_code != 200) {
        return("Error fetching access token");
    } else {
        //parse the json response
        json j = json::parse(readBuffer);
        return j["access_token"];
    }
}