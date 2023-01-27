#include "stock.h"
#include <fstream>
#include "utils.h"
#include <cfloat>
#include <cmath>

Stock::Stock(string ticker){
    this->ticker = ticker;
}

void Stock::initWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas){
    if(!loadedFont.LoadFont(fetchFont(5).c_str())){
    }
    if(!loadedFontSmall.LoadFont(fetchFont(4).c_str())){
    }
    fetchApiKey();
}

void Stock::fetchApiKey(){
    ifstream file("stocks_api_key.key");
    file >> this->api_key;
    file.close();
}

void Stock::drawWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, atomic_bool& end){
    thread t1(&Stock::fetchStockData, this);
    while(!end){
        layoutWidget(matrix, canvas);
    }
    t1.join();
}

void Stock::layoutWidget(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas){
    canvas->Clear();
    int text_size = rgb_matrix::DrawText(canvas, loadedFont, 1, 9, rgb_matrix::Color(255, 255, 255), NULL, ticker.c_str(), 0, 0, 62, 0, false);
    drawChart(matrix, canvas, text_size + 1);
    canvas = matrix->SwapOnVSync(canvas);
    usleep(delay_speed_usec);
}

void Stock::drawChart(RGBMatrix *matrix, rgb_matrix::FrameCanvas *canvas, int text_end) {
    double max = DBL_MAX;
    double min = DBL_MIN;
    //reduce stock_data to 64 elements that span the entire day
    vector<double> reduced_stock_data;
    
    if(!stock_data.empty()){
        if (stock_data.size() >= 64){
            double interval = stock_data.size() / 64;
            for(long unsigned int i = 1; i <= 64; i++){
                reduced_stock_data.push_back(stock_data[(i * interval) - 1]);
            }
        } else {
            reduced_stock_data = stock_data;
        }
        
        for(long unsigned int i = 0; i < reduced_stock_data.size(); i++){
            if(i == 0){
                max = reduced_stock_data[i];
                min = reduced_stock_data[i];
            }
            if(reduced_stock_data[i] > max){
                max = reduced_stock_data[i];
            }
            if(reduced_stock_data[i] < min){
                min = reduced_stock_data[i];
            }
        }

        double range = max - min;
        bool isGreen = reduced_stock_data[reduced_stock_data.size() - 1] >= reduced_stock_data[0];
        //determine the percent change from the first to the last element
        double percent_change = (reduced_stock_data[reduced_stock_data.size() - 1] - reduced_stock_data[0]) / reduced_stock_data[0];
        if (percent_change < 0){
            //canvas->SetPixel(text_end + 1, 2, 168, 0, 0);
        } else {
            //canvas->SetPixel(text_end + 1, 8, 11, 87, 31);
            /*canvas->SetPixel(text_end + 2, 7, 11, 87, 31);
            canvas->SetPixel(text_end + 3, 6, 11, 87, 31);
            canvas->SetPixel(text_end + 4, 5, 11, 87, 31);
            canvas->SetPixel(text_end + 5, 4, 11, 87, 31);
            canvas->SetPixel(text_end + 6, 3, 11, 87, 31);
            canvas->SetPixel(text_end + 6, 4, 11, 87, 31);
            canvas->SetPixel(text_end + 6, 5, 11, 87, 31);
            canvas->SetPixel(text_end + 6, 6, 11, 87, 31);
            canvas->SetPixel(text_end + 5, 3, 11, 87, 31);
            canvas->SetPixel(text_end + 4, 3, 11, 87, 31);
            canvas->SetPixel(text_end + 3, 3, 11, 87, 31);*/
            //canvas->SetPixel(text_end + 7, 2, 11, 87, 31);
        }
        for(long unsigned int i = 0; i < reduced_stock_data.size(); i++){
            int relative_y = 42 - (((reduced_stock_data[i] - min) / range * 20) + 11);
            if(isGreen){
                //canvas->SetPixel(i, relative_y, 11, 212, 0);
                for(int j = relative_y; j < 32; j++){
                    canvas->SetPixel(i, j, 11, 87, 31);
                }
            } else {
                //canvas->SetPixel(i, relative_y, 212, 0, 0);
                for(int j = relative_y; j < 32; j++){  
                    canvas->SetPixel(i, j, 168, 0, 0);
                }
            }
        }
    }
    
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void Stock::fetchStockData(){
    string requestUrl = "https://finnhub.io/api/v1/stock/candle?symbol=" + this->ticker + "&resolution=5&from=1674227376&to=1674313821&token=" + this->api_key;
    CURL *curl;
    string readBuffer;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    //cout << http_code << endl;
    if(http_code == 200){
        json j = json::parse(readBuffer);
        if(j["s"] == "ok"){
            stock_data = {};
            for(long unsigned int i = 0; i < j["c"].size(); i++){
                // day.push_back(j["o"][i]);
                // day.push_back(j["h"][i]);
                // day.push_back(j["l"][i]);
                //day.push_back(j["t"][i]);
                stock_data.push_back(j["c"][i]);
            }
        }
    }
    usleep(10000000);
}

int Stock::shouldSwitchWidget(){
    return 0;
}