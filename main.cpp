#include <iostream>

#include <future>
#include "async.h"

void run1()
{
    std::size_t bulkSize = 2;
    auto h1 = async::connect(bulkSize);
    async::receive(h1, "1", 1);
    async::receive(h1, "2", 1);
    async::receive(h1, "3", 1);
    async::receive(h1, "\n", 1);
    async::receive(h1, "4", 1);
    async::receive(h1, "5", 1);
    async::receive(h1, "6", 1);
    async::receive(h1, "7", 1);
    async::receive(h1, "\n", 1);
    async::receive(h1, "8", 1);
    async::receive(h1, "9", 1);
    async::receive(h1, "10", 2);
    async::receive(h1, "11", 2);
    async::receive(h1, "\n", 1);
    async::receive(h1, "12", 2); 
    async::receive(h1, "13", 2); 
    async::receive(h1, "14", 2); 
    async::disconnect(h1);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void run2()
{
    std::size_t bulkSize = 1;
    auto h2 = async::connect(bulkSize);
    async::receive(h2, "14", 2);
    async::receive(h2, "15", 2);
    async::receive(h2, "16", 2);
    async::receive(h2, "\n", 1);
    async::receive(h2, "17", 2);
    async::receive(h2, "18", 2);
    async::receive(h2, "19", 2);
    async::receive(h2, "20", 2);
    async::receive(h2, "\n", 1);
    async::receive(h2, "21", 2);
    async::disconnect(h2);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void run3()
{
    std::size_t bulkSize = 3;
    auto h3 = async::connect(bulkSize);
    async::receive(h3, "22", 2);
    async::receive(h3, "23", 2);
    async::receive(h3, "24", 2);
    async::receive(h3, "\n", 1);
    async::receive(h3, "25", 2);
    async::receive(h3, "26", 2);
    async::receive(h3, "27", 2);
    async::receive(h3, "28", 2);
    async::receive(h3, "\n", 1);
    async::receive(h3, "29", 2);
    async::receive(h3, "30", 2);
    async::receive(h3, "31", 2);
    async::receive(h3, "32", 2);
    async::receive(h3, "\n", 1);
    async::receive(h3, "33", 2);
    async::disconnect(h3);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main(int, char *[])
{
    auto result1 = std::async(std::launch::async, run1);
    auto result2 = std::async(std::launch::async, run2);
    auto result3 = std::async(std::launch::async, run3);
    std::size_t bulk = 5;
    auto h4 = async::connect(bulk);
    auto h5 = async::connect(bulk);
    async::receive(h4, "a", 1);
    async::receive(h4, "b", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "c", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "d", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "\n", 1);
    async::receive(h5, "e", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "f", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "g", 1);
    async::receive(h5, "\n", 1);
    async::receive(h4, "{", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "h", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "i", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "j", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "k", 1);
    async::receive(h4, "\n", 1);
    async::receive(h5, "}", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "l", 1);
    async::receive(h5, "m", 1);
    async::receive(h4, "\n", 1);
    async::disconnect(h4);
    async::disconnect(h5);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    result1.wait();
    result2.wait();
    result3.wait();
    return 0;
}
