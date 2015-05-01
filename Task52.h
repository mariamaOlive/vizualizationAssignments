//---------------------------------------------------------------------------
#ifndef Task52H
#define Task52H
//---------------------------------------------------------------------------
#include "Experiment.h"
#include "LinearAlgebra.h"
#include "GLGeometryViewer.h"
//---------------------------------------------------------------------------
#include <limits>
#include "Field2.hpp"


/// This is an example experiment.
///
/// The code is meant to demonstrate how
///  to use the GeoX framework
///

class Task52 : public Experiment
{
    GEOX_CLASS(Task52)

//Constructor / Destructor
public:
    Task52();
    virtual ~Task52();

//Methods
public:
    void DrawVectorField();	//drawVectorField to be removed later
    virtual QWidget* createViewer();
	
	//Euler
	void EulerStreamlines();

	//RungeKutta
	void RungeKuttaStreamlines();

	void RandomSeeding();
	void RungeKuttaStreamlines(VectorField2 field, float startX, float startY);

//Attributes
public:

    ///File name of the vector field
    string VectorfieldFilename;

	//start values
	float XStart;
	float YStart;
	float MaxLength;
	float MinSpeed;
	bool backwards;

    ///Length of the arrows		TO BE REMOVED
    float ArrowScale;

	//Euler values
	float EulerStepSize;
	int EulerSteps;

	//Runge-Kutta values
	float RKStepSize;
	int RKSteps;

	//number of seeding points
	int n;


protected:
    GLGeometryViewer* viewer;
};

#endif
