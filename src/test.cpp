#include "json.hpp"
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
    ifstream fJson("cd2");
    stringstream buffer;
    buffer << fJson.rdbuf();
    auto json = nlohmann::json::parse(buffer.str());

    cout << "Size = " << json.size() << endl;
    cout << "Inner Size = " << json[0][0].size() << endl;
    int a[json.size()][json.size()][json[0][0].size()];
    for (int r = 0;r < json.size(); r++)
      {
        for (int c = 0;c < json.size(); c++)
          {
            for (int e = 0;e < json[0][0].size(); e++)
              {
                a[r][c][e] = json[r][c][e];
              }
          }
        
      }
    for (int r = 0;r < json.size(); r++)
      {
        cout << "[";
        for (int c = 0;c < json.size(); c++)
          {
            cout << "[";
            for (int e = 0;e < json[0][0].size(); e++)
              {
                cout << a[r][c][e];
              }
            cout << "]";
          }
        
        cout << "]";
      }
    cout << endl;


    nlohmann::json data;
    for (int r = 0;r < json.size(); r++)
      {
        for (int c = 0;c < json.size(); c++)
          {
            for (int e = 0;e < json[0][0].size(); e++)
              {
                data[r][c][e] = a[r][c][e];
              }
          }
        
      }

    std::ofstream output_file("foo");
    if (!output_file.is_open())  {
        std::cout << "\n Failed to open output file";
    } else {
        output_file << data;
        output_file.close();
    }


    
    return 0;
}
