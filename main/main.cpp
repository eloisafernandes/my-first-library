#include "vector.h"
#include <iostream>

int main(){

    sc::vector<int> vec{10, 2, 3, 2, 5};
    sc::vector<int> vec2{ vec };
    
    //vec.clear();

    
    auto it = vec.end();
    auto it2 = vec.cend();
    vec.push_back(10);
    vec.push_back(10); 
    //vec.insert(vec.begin(), 80); 
    //vec.insert(vec.begin(), 20); 
    //vec.erase(vec.begin()+2);

    vec.erase(vec.begin(), vec.begin()+2);
    for (int i = 0; i < vec.size(); ++i){
        std::cout<<vec[i]<<"\n";
    }
    
    
    
    
    return 0;
}