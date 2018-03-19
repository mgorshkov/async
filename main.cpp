#include <iostream>

#include <future>
#include "async.h"

async::handle_t h1 = 0;
async::handle_t h2 = 0;
async::handle_t h3 = 0;
async::handle_t h4 = 0;
async::handle_t h5 = 0;

void run1()
{
   std::size_t bulkSize = 2;
   h1 = async::connect(bulkSize);
   async::receive(h1, "1", 1);
   async::receive(h1, "2", 1);
   async::receive(h1, "3", 1);
   async::receive(h1, "\n", 1);
   async::receive(h1, "4", 1);
   async::receive(h1, "1", 1);
   async::receive(h1, "2", 1);
   async::receive(h2, "3", 1);
   async::receive(h1, "\n", 1);
   async::receive(h1, "4", 1);
   async::receive(h1, "1", 1);
   async::receive(h3, "2", 1);
   async::receive(h1, "3", 1);
   async::receive(h1, "\n", 1);
   async::receive(h1, "4", 1); 
   async::disconnect(h1);
}

void run2()
{
   std::size_t bulkSize = 1;
   h2 = async::connect(bulkSize);
   async::receive(h2, "1", 1);
   async::receive(h2, "2", 1);
   async::receive(h2, "3", 1);
   async::receive(h2, "\n", 1);
   async::receive(h2, "4", 1);
   async::receive(h1, "1", 1);
   async::receive(h1, "2", 1);
   async::receive(h1, "3", 1);
   async::receive(h3, "\n", 1);
   async::receive(h1, "4", 1);
   async::disconnect(h2);
}

void run3()
{
   std::size_t bulkSize = 3;
   h3 = async::connect(bulkSize);
   async::receive(h3, "1", 1);
   async::receive(h3, "2", 1);
   async::receive(h3, "3", 1);
   async::receive(h3, "\n", 1);
   async::receive(h3, "4", 1);
   async::receive(h1, "1", 1);
   async::receive(h1, "2", 1);
   async::receive(h1, "3", 1);
   async::receive(h1, "\n", 1);
   async::receive(h1, "4", 1);
   async::receive(h2, "1", 1);
   async::receive(h1, "2", 1);
   async::receive(h1, "3", 1);
   async::receive(h1, "\n", 1);
   async::receive(h1, "4", 1);
   async::disconnect(h3);
}

int main(int, char *[])
{
//    auto result1 = std::async(std::launch::async, run1);
//    auto result2 = std::async(std::launch::async, run2);
//    auto result3 = std::async(std::launch::async, run3);
    std::size_t bulk = 5;
    h4 = async::connect(bulk);
//    h5 = async::connect(bulk);
    async::receive(h4, "1", 1);
    async::receive(h4, "1", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "2", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "3", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "\n", 1);
    async::receive(h5, "4", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "5", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "6", 1);
    async::receive(h5, "\n", 1);
    async::receive(h4, "{", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "a", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "b", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "c", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "d", 1);
    async::receive(h4, "\n", 1);
    async::receive(h5, "}", 1);
    async::receive(h4, "\n", 1);
    async::receive(h4, "8", 1);
    async::receive(h5, "9", 1);
    async::receive(h4, "\n", 1);
    async::disconnect(h4);
//    async::disconnect(h5);
//    result1.wait();
//    result2.wait();
//    result3.wait();
    return 0;
}
