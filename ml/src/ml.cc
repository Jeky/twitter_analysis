#include "ml.h"

Instance::Instance(){}


void Instance::setClassValue(double value){
    classValue = value;
}


double Instance::getClassValue(double value){
    return classValue;
}


bool Instance::hasAttribute(String &name){
    return values.find(name) != values.end();
}


double& Instance::operator[](String &name){
    return values[name];
}


double& Instance::operator[](const char *name){
    String key = name;
    return values[key];
}


Dataset::Dataset(){}


void Dataset::addInstance(Instance &instance){
    instances.push_back(instance);
}


int Dataset::size(){
    return instances.size();
}


void Dataset::shuffle(){
}


Instance& Dataset::operator[](int index){
    return instances[index];
}

