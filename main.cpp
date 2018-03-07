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
std::cout << "FINISHED1!";
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
std::cout << "FINISHED2!";
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
std::cout << "FINISHED3!";
}

int main(int, char *[])
{
    auto result1 = std::async(std::launch::async, run1);
    auto result2 = std::async(std::launch::async, run2);
    auto result3 = std::async(std::launch::async, run3);
std::cout << "main1";
    std::size_t bulk = 5;
    h4 = async::connect(bulk);
std::cout << "main2";
    h5 = async::connect(bulk);
std::cout << "main3";
    async::receive(h1, "1", 1);
std::cout << "main4";
    async::receive(h2, "1\n", 2);
std::cout << "main5";
    async::receive(h4, "\n2\n3\n4\n5\n6\n{\na\n", 15);
std::cout << "main6";
    async::receive(h5, "b\nc\nd\n}\n89\n", 11);
std::cout << "main7";
    async::disconnect(h4);
std::cout << "main8";
    async::disconnect(h5);
std::cout << "main9";
    result1.get();
std::cout << "FINISH1 GOT!";
    result2.get();
std::cout << "FINISH2 GOT!";
    result3.get();
std::cout << "FINISHED ALL!";
    return 0;
}
