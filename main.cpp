#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <chrono>
#include <clocale>
#include <string>
#include <sstream>
#include <exception>

using namespace std;

static auto start = chrono::high_resolution_clock::now();

class ThListIsEmpty : public exception
{
public:
    ThListIsEmpty()
    {
    }
    virtual ~ThListIsEmpty() override {}

    // exception interface
public:
    virtual const char *what() const noexcept override;
};

const char *ThListIsEmpty::what() const noexcept
{
    return "ThListIsEmpty";
}


class ThList
{
public:
    explicit ThList()
    {

    }
    void push_(int i)
    {
        unique_lock<mutex> ul(mut_1, defer_lock);
        mut_1.lock();
        ll.push_back(i);
        mut_1.unlock();
    }
    int pop_()
    {
        unique_lock<mutex> ul(mut_1, defer_lock);

        mut_1.lock();
        std::string("asdf");
        if (ll.size() == 0) throw ThListIsEmpty();
        int i = ll.front();
        ll.pop_front();
        mut_1.unlock();
        return i;
    }

    bool isEmpty_()
    {
        return ll.size() == 0 ? true : false;
    }

    size_t len_()
    {
        return ll.size();
    }

    friend ostream& operator<<(ostream &os, ThList &right)
    {
        string str;
        std::cout << "thList::print() ";
        if(right.isEmpty_()) return os << "is empty" << std::endl;
        for(auto &value : right.ll)  {

           str += std::to_string(value) + " ";
        }
        return os << str << std::endl;
    }
protected:
    mutex mut_1;
    list<int> ll;
};


void print(string &&ss);

void write_tl(ThList &tl)
{
    print("=============Write thread started============\n");
    for(int i = 0; i < 1000; i++)
    {
        auto now_ = chrono::high_resolution_clock::now();
        chrono::duration<float> dur = now_ - start;
        std::stringstream ss;
        ss << "Write_Thread_ID:"<<this_thread::get_id() << ", "
                  << "Write " << i << ", "
                  << "Len: " << tl.len_() << ", "
                  << "Count: " << dur.count() << std::endl;
        print(ss.str());
        tl.push_(i);
    }
    print("=============Write thread stoped============\n");

}

std::list<int> read_tl(ThList &tl)
{
    std::list<int> buff;

    print("=============Read thread started============\n");

    while(!tl.isEmpty_())
    {
        auto now_ = chrono::high_resolution_clock::now();
        chrono::duration<float> dur = now_ - start;
        int i = tl.pop_();
        buff.push_back(i);
        std::stringstream ss;
        ss << "Read_Thread_ID:"<<this_thread::get_id() << ", "
                  << "Value is " << i << ", "
                  << "Len: " << tl.len_() << ", "
                  << "Count: " << dur.count() << std::endl;
        //this_thread::sleep_for(std::chrono::milliseconds(300));
        print(ss.str());
        this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    print("=============Read thread stoped============\n");
    return buff;
}

static mutex mut_2;
void print(string &&ss)
{
    unique_lock<mutex> ul(mut_2, defer_lock);
    mut_2.lock();
    std::cout << ss << std::endl;
    mut_2.unlock();
}

int main()
{
    std::setlocale(LC_CTYPE,"rus");

    ThList tl;

    try {
        tl.pop_();
    } catch (std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    std::cout << "Threads " << thread::hardware_concurrency() << ", "
              << "Main thread ID: " << this_thread::get_id() << std::endl;

    std::cout << tl << std::endl;

    std::list<int> ll;
    thread th_write(write_tl, std::ref(tl));
    this_thread::sleep_for(std::chrono::milliseconds(1));
    thread th_read([&](){ ll = read_tl(tl);});

    th_write.join();
    th_read.join();

    auto it = ll.begin();
    while(true)
    {
        if( it == ll.end() ) {
            std::cout << "( A Leng of the Data is "<< ll.size() << " )." << std::endl;
            break;
        }
        else
        {
            std::cout << *it << ", ";
            it++;
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<float> dur = stop - start;
    std::cout << "time: " << dur.count() << std::endl;

    return 0;
}
