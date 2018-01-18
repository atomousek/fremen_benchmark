#include "CPythonHyperTime.h"

using namespace std; // nejsem si jist, myslim, ze to nepotrebuju


// tyto knihovny jsou potreba
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "Python.h"
#include "numpy/arrayobject.h"
#include <iostream>


// tyto promenne by mely byt public
long measurements = 0;
PyObject *pModel;
const long numberOfDimensions = 2;
const long maxMeasurements = 10000;
double tableOfMeasurements[numberOfDimensions][maxMeasurements];


// stolen from https://github.com/davisking/dlib/blob/master/tools/python/src/numpy_returns.cpp
// we need this wonky stuff because different versions of numpy's import_array macro
// contain differently typed return statements inside import_array().
#if PY_VERSION_HEX >= 0x03000000
#define DLIB_NUMPY_IMPORT_ARRAY_RETURN_TYPE void* 
#define DLIB_NUMPY_IMPORT_RETURN return 0
#else
#define DLIB_NUMPY_IMPORT_ARRAY_RETURN_TYPE void
#define DLIB_NUMPY_IMPORT_RETURN return
#endif
DLIB_NUMPY_IMPORT_ARRAY_RETURN_TYPE import_numpy_stuff()
{
    import_array();
    DLIB_NUMPY_IMPORT_RETURN;
}


CPythonHyperTime::CPythonHyperTime(const char* name)
{
	strcpy(id,name); //? nevim, zda to budu pouzivat
    // instead of export PYTHONPATH=`pwd` in terminal
    // stolen from https://stackoverflow.com/questions/46493307/embed-python-numpy-in-c
    setenv("PYTHONPATH", ".", 1);

    /* Py_SetProgramName(argv[0]); //default 'python', I will not call that */

    //initialize the python interpreter
    Py_Initialize();
    //importing of the python script
    PyObject *pModuleName = PyUnicode_FromString("python_module");//name must be changed
    PyObject *pModule = PyImport_Import(pModuleName);
    Py_DECREF(pModuleName); // free memory
    //checking the existence
    if (!pModule){
        std::cout << "python module can not be imported" << std::endl;
        // some kill command ??? :)
    }
/*
    int measurements = 0;
    PyObject *pModel;
    const int numberOfDimensions = 2;
    const int maxMeasurements = 10000;
    double tableOfMeasurements [maxMeasurements][numberOfDimensions];
*/
}

void CPythonHyperTime::init(int iMaxPeriod,int elements,int numActivities)
{
//nevim, co tu napsat :)

}

CPythonHyperTime::~CPythonHyperTime()
{
    // tady pravdepodobne
    Py_DECREF(pModel); 
    Py_Finalize(); // shuts the interpreter down
    // clear the memory
    //free(tableOfMeasurements);//hazi chybu, nevim, jak zlikvidovat
}

// adds new state observations at given times
int CPythonHyperTime::add(uint32_t time,float state)
{
    tableOfMeasurements[measurements][0] = (double)time;
    tableOfMeasurements[measurements][1] = (double)state;


    measurements++;

    return 0; 
}

/*not required in incremental version*/
void CPythonHyperTime::update(int modelOrder)
{
    //initializing numpy array api
    //instead of import_array();
    import_numpy_stuff();

    // Convert it to a NumPy array
    //npy_intp dims[numberOfDimensions]{numberOfDimensions,maxMeasurements};
    npy_intp dims[numberOfDimensions]{numberOfDimensions,measurements};
    PyObject *pArray = PyArray_SimpleNewFromData(
        numberOfDimensions, dims, NPY_FLOAT, reinterpret_cast<void*>(tableOfMeasurements));
    if (!pArray)
        std::cout << "numpy array was not created" << std::endl;


    // call python function
    PyObject *pFunc = PyObject_GetAttrString(pModule,"python_function"); // name must be changed
    if (!pFunc)
        std::cout << "python function was not created" << std::endl; //?
    if (!PyCallable_Check(pFunc))
        std::cout << "python function is not callable." << std::endl;


/*
// v kazdem pripade bych nemel delat to numpy array z celeho toho arraye, ale jen z vyuzite casti. Pak se pouzije tato cas kodu, nebot pArray nebude obsahovat nesmyslne (nenaplnene) radky
    // np_ret = mymodule.array_tutorial(np_arr)
    PyObject *pModel = PyObject_CallFunctionObjArgs(pFunc, pArray, NULL);
    if (!pModel)
        std::cout << "python function did not respond" << std::endl;
//zde predpokladame, ze pModel je pythoni objekt obsahujici libovolny pythoni bordel, ktery definuje model
// nicmene, ja tady budu muset ukoncit tu funkci a smazat bordel z ramky

*/

//### tady zacina docasna cast

    // np_ret = mymodule.array_tutorial(np_arr)

    PyObject *pArgs0 = PyTuple_New(2);
    PyTuple_SetItem(pArgs0, 0, pArray);

    PyObject *pValue0 = PyInt_FromLong(measurements);
    if (!pValue0)
        std::cout << "unable to convert  value" << std::endl;

    PyTuple_SetItem(pArgs0, 1, pValue0);
    // np_ret = mymodule.array_tutorial(np_arr)
    PyObject *pModel = PyObject_CallObject(pFunc, pArgs0);
    Py_DECREF(pArgs0);
    if (!pModel)
        std::cout << "python function did not respond" << std::endl;
//zde predpokladame, ze pModel je pythoni objekt obsahujici libovolny pythoni bordel, ktery definuje model
// nicmene, ja tady budu muset ukoncit tu funkci a smazat bordel z ramky

    Py_DECREF(pValue0);

//### tady konci docasna cast


    Py_DECREF(pArray);
    Py_XDECREF(pFunc); //XDECREF?



}

/*text representation of the fremen model*/
void CPythonHyperTime::print(bool verbose)
{
	std::cout << "Model: " << id << " Prior: " << predictGain << " Size: " << measurements << " ";
	if (verbose){
		for (int i = 0;i<order;i++){
			std::cout << "Frelement " << i << " " << predictFrelements[i].amplitude << " " << predictFrelements[i].phase << " " << predictFrelements[i].period << " ";
		}
	}
	std::cout << endl; 
}

float CPythonHyperTime::estimate(uint32_t time)
{

    double test = (double)time;

    // call python function
    PyObject *pFunc2 = PyObject_GetAttrString(pModule,"python_function_estimate"); // name must be changed
    if (!pFunc2)
        std::cout << "python function does not exist" << std::endl; //?
    if (!PyCallable_Check(pFunc2))
        std::cout << "python function is not callable." << std::endl;


    PyObject *pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, pModel);

    PyObject *pValue = PyFloat_FromDouble(test);
    if (!pValue)
        std::cout << "unable to convert  value" << std::endl;

    PyTuple_SetItem(pArgs, 1, pValue);
    // np_ret = mymodule.array_tutorial(np_arr)
    //PyObject *pModel = PyObject_CallFunctionObjArgs(pFunc, pArray, NULL);
    PyObject *pEstimate = PyObject_CallObject(pFunc2, pArgs);
    Py_DECREF(pArgs);
    if (!pEstimate)
        std::cout << "python function did not respond" << std::endl;

    estimate =  PyFloat_AsDouble(pEstimate);

    Py_DECREF(pValue);
    Py_DECREF(pEstimate);
    Py_XDECREF(pFunc2);

    return estimate;
}

float CPythonHyperTime::predict(uint32_t time)
{
	float saturation = 0.01;
	float estimate =  predictGain;
	for (int i = 0;i<order;i++) estimate+=2*predictFrelements[i].amplitude*cos(time/predictFrelements[i].period*2*M_PI-predictFrelements[i].phase);
	if (estimate > 1.0-saturation) estimate =  1.0-saturation;
	if (estimate < 0.0+saturation) estimate =  0.0+saturation;
	return estimate;
}

int CPythonHyperTime::save(char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	save(file);
	fclose(file);
	return 0;
}

int CPythonHyperTime::load(char* name)
{
	FILE* file = fopen(name,"r");
	load(file);
	fclose(file);
	return 0;
}


int CPythonHyperTime::save(FILE* file,bool lossy)
{
	int frk = numElements;
	fwrite(&frk,sizeof(uint32_t),1,file);
	fwrite(&storedGain,sizeof(float),1,file);
	fwrite(storedFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}

int CPythonHyperTime::load(FILE* file)
{
	int frk = numElements;
	fwrite(&frk,sizeof(uint32_t),1,file);
	fwrite(&storedGain,sizeof(float),1,file);
	fwrite(storedFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}
