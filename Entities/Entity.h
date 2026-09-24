#pragma once
#include <string>
#include <vector>
#include "Utils/Validator.h"
using namespace std;

class Entity
{
protected:
    string id="";
public:
    Entity()
    {
    }
    Entity(string id)
    {
        this->id = id;
    }
    virtual ~Entity() {}

    string getID() const // promise to compiler that this func will not change variables, because when passing a constant address like const Entity& e to a function then call e.getID() cpp feared that the func might change the value of a constant variable.
    {
        return id;
    }
    virtual void parseFromString(const string &data) = 0;

    virtual string exportToString() const=0;
    virtual vector<string> exportToVector() const = 0;
};