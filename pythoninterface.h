#ifndef PYTHONINTERFACE_H
#define PYTHONINTERFACE_H

#include <Python.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <cstdio>

class PythonInterface
{

public:

    //! Constructor
    PythonInterface();

    //! Destructor
    virtual ~PythonInterface();

    //! Init
    int init();

    //! De init
    int deinit();

    //! Add path which containing python modules
    int addModulePath(std::string path);

    //! Parameter countingn
    template <typename T>
    constexpr int countParams (const T t) const {
        return 1;
    }

    //! Parameter countingn
    template <typename T, typename... Args>
    constexpr int countParams (const T t, Args... args) const {
        return countParams(args...)+1;
    }

    //! Add parameter to tuple
    template <typename T>
    int addParam(PyObject* tuple, Py_ssize_t &paramCount, T param);

    //! Add parameter to tuple
    template <typename T, typename... Args>
    int addParam(PyObject* tuple, Py_ssize_t &paramCount, T param, Args... args);

    //! Import a module
    PyObject* importModule(const std::string name);

    //! Get instance of class
    PyObject* classInstance(const std::string name);

    //! Call a method without arguments in python class
    inline PyObject* callInClass (PyObject *classInstance, const std::string methodName);

    //! Call a method in python class
    template <typename... Args>
    PyObject* callInClass (PyObject *classInstance, const std::string methodName, const Args... args);

    //! Call a method without arguments in python class
    inline PyObject* callInClass (const std::string className, const std::string methodName);

    //! Call a method in python class
    template <typename... Args>
    PyObject* callInClass (const std::string className, const std::string methodName, const Args... args);

    //! Convert a value to a PyObject
    template <typename T>
    inline PyObject* convertToPyObject(T v);

    PyObject* convertToPyObject(std::string v);


private:

    //! Clean up our references
    int cleanUp();

    std::vector<PyObject*> _refsToCleanUp;
    std::map<std::string, PyObject*> _pyModules;
    std::map<std::string, PyObject*> _pyDicts;

    /*! Prohibited */
    PythonInterface(const PythonInterface& other);
    PythonInterface& operator=(const PythonInterface& other);
    bool operator==(const PythonInterface& other) const;
};

template<typename T>
int PythonInterface::addParam(PyObject* tuple, Py_ssize_t &paramCount, T param)
{
    PyTuple_SetItem(tuple, paramCount, convertToPyObject(param));
    paramCount++;
    return paramCount;
}

template<typename T, typename... Args>
int PythonInterface::addParam(PyObject* tuple, Py_ssize_t &paramCount, T param, Args... args)
{
    PyTuple_SetItem(tuple, paramCount, convertToPyObject(param));
    paramCount++;
    addParam(tuple, paramCount, args...);
    return paramCount;
}


PyObject* PythonInterface::callInClass(PyObject* classInstance, const std::string methodName)
{
    PyObject *res = PyObject_CallMethodObjArgs(classInstance, PyString_FromString(const_cast<char*>(methodName.c_str())), nullptr);
    return res;
}

template<typename... Args>
PyObject* PythonInterface::callInClass(PyObject* classInstance, const std::string methodName, const Args... args)
{
    const int nArgs = countParams(args...);

    Py_ssize_t paramCount = 0;

    PyObject *pyArgs = PyTuple_New(nArgs);

    addParam(pyArgs, paramCount, args...);

    PyObject *res = PyObject_CallMethodObjArgs(classInstance,PyString_FromString(const_cast<char*>(methodName.c_str())), pyArgs, nullptr);
    
    Py_DecRef(pyArgs);

    return res;

}

PyObject* PythonInterface::callInClass(const std::string className, const std::string methodName)
{
    PyObject *classInst = classInstance(className);
    if(classInst)
    {
        return callInClass(classInst, methodName);
    }
    return 0;
}

template<typename... Args>
PyObject* PythonInterface::callInClass(const std::string className, const std::string methodName, const Args... args)
{
    PyObject *classInst = classInstance(className);
    if(classInst)
    {
        PyObject *res = callInClass(classInst, methodName, args...);
        return res;
    }

    return 0;
}

template<typename T>
PyObject* PythonInterface::convertToPyObject(T v)
{
    std::cout << "Not converting: " << v << std::endl;
    return 0;
}



#endif // PYTHONINTERFACE_H

