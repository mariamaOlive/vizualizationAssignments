//---------------------------------------------------------------------------
#ifndef Task51H
#define Task51H
//---------------------------------------------------------------------------
#include "Experiment.h"
#include "LinearAlgebra.h"
#include "GLGeometryViewer.h"
//---------------------------------------------------------------------------

/// This is an example experiment.
///
/// The code is meant to demonstrate how
///  to use the GeoX framework
///

class Task51 : public Experiment
{
    GEOX_CLASS(Task51)

//Constructor / Destructor
public:
    Task51();
    virtual ~Task51();

//Methods
public:
    void DrawVectorField();	//drawVectorField to be removed later
    virtual QWidget* createViewer();
	
	//Euler
	void EulerStreamlines();

	//RungeKutta
	void RungeKuttaStreamlines();

//Attributes
public:

    ///File name of the vector field
    string VectorfieldFilename;

	//start values
	
	float XStart;
	float YStart;
	float MaxDistance;
	

    ///Length of the arrows		TO BE REMOVED
    float ArrowScale;

	//Euler values
	float EulerStepSize;
	int EulerSteps;

	//Runge-Kutta values
	float RKStepSize;
	int RKSteps;


protected:
    GLGeometryViewer* viewer;
};

#endif
