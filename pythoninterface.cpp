#include "pythoninterface.h"

PythonInterface::PythonInterface()
{

}

PythonInterface::~PythonInterface()
{
  cleanUp();
  Py_Finalize();
}
int PythonInterface::init()
{
  Py_Initialize();
  return 0;
}

int PythonInterface::deinit()
{
  cleanUp();
  Py_Finalize();
  
  return 0;
}


int PythonInterface::addModulePath(std::string path)
{
  PyObject *obj = PySys_GetObject("path");
  PyList_Append(obj,PyString_FromString(const_cast<char*>(path.c_str())));
  return 0;
}


PyObject* PythonInterface::importModule(const std::string name)
{
  PyObject *moduleName = PyString_FromString(name.c_str());
  
  PyObject *module = PyImport_Import(moduleName);
  
  PyObject *dict = PyModule_GetDict(module);
  
  _pyModules.insert(std::pair<std::string, PyObject*>(name,module));
  _pyDicts.insert(std::pair<std::string, PyObject*>(name,dict));

  Py_DecRef(moduleName);
  
  return module;
}

int PythonInterface::cleanUp()
{
  
  for(const auto & r : _refsToCleanUp)
  {
    Py_DecRef(r);
  }
  
  for(const auto & kv : _pyModules)
  {
    Py_DecRef(kv.second);
  }
  
  _pyDicts.clear();
  _pyModules.clear();
  
  return 0;
}

PyObject* PythonInterface::classInstance(const std::string name)
{
    PyObject *classInstance = 0;
    for(const auto &kv : _pyDicts)
    {
        PyObject *pyClass = PyDict_GetItemString(kv.second, name.c_str());
        if(PyCallable_Check(pyClass))
        {
            classInstance = PyObject_CallObject(pyClass, NULL);
	    return classInstance;
        }
    }
    return 0;
}


template<>
PyObject* PythonInterface::convertToPyObject(int v)
{
    return PyInt_FromLong(v);
}

template<>
PyObject* PythonInterface::convertToPyObject(float v)
{
    return PyFloat_FromDouble(v);
}

template<>
PyObject* PythonInterface::convertToPyObject(double v)
{
    return PyFloat_FromDouble(v);
}

template<>
PyObject* PythonInterface::convertToPyObject(bool v)
{
    return PyBool_FromLong(v);
}

PyObject* PythonInterface::convertToPyObject(std::string v)
{
    return PyString_FromString(v.c_str());
}


template<>
PyObject* PythonInterface::convertToPyObject(const char* v)
{
    return PyString_FromString(v);
}
