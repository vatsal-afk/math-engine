#include <iostream>
#include <string>

class Entity
{
    public:
    void Assign()const {int x=5;std::cout<<x<<std::endl;}
    void Print() const {std::cout<<"Hello!"<<std::endl;}
};

int main()
{
    Entity e;
    std::cout<<&e<<std::endl;
    e.Print();

    Entity* ptr = &e; // stores the address of e
    Entity& entity = *ptr;
    (*ptr).Print(); // dereferencing, also precedence 
    ptr->Print(); // use the arrow "operator"
    ptr->Assign();
}