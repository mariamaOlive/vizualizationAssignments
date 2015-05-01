//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task51.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"

IMPLEMENT_GEOX_CLASS( Task51, 0)
{
    BEGIN_CLASS_INIT( Task51 );

    ADD_SEPARATOR("Start Values")
    ADD_STRING_PROP(VectorfieldFilename, 0)
	ADD_FLOAT32_PROP(XStart, 0)
	ADD_FLOAT32_PROP(YStart, 0)
	ADD_FLOAT32_PROP(MaxDistance, 0)
    ADD_FLOAT32_PROP(ArrowScale, 0)

	ADD_SEPARATOR("Euler")	
	ADD_FLOAT32_PROP(EulerStepSize,0)
	ADD_INT32_PROP(EulerSteps,0)

	ADD_SEPARATOR("RungeKutta")
	ADD_FLOAT32_PROP(RKStepSize,0)
	ADD_INT32_PROP(RKSteps,0)  


    ADD_NOARGS_METHOD(Task51::DrawVectorField)
	ADD_NOARGS_METHOD(Task51::EulerStreamlines)
	ADD_NOARGS_METHOD(Task51::RungeKuttaStreamlines)
}

QWidget* Task51::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task51::Task51()
{
    viewer = NULL;
    VectorfieldFilename = "Cylinderclose2CT10.am";
    ArrowScale = 0.1;

	//start values
	XStart = 1;
	YStart = 0;
	MaxDistance = 5.3;

	//Euler values
	EulerStepSize = 0.1;
	EulerSteps = 100;

	//Runge-Kutta values
	RKStepSize = 0.3;
	RKSteps = 30;
}

Task51::~Task51() {}

//From ExpampleExperimentField (remove later)
void Task51::DrawVectorField()
{
    viewer->clear();

    //Load the vector field
    VectorField2 field;
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

    //Draw vector directions (constant length)
    for(float32 x=field.boundMin()[0]; x<=field.boundMax()[0]; x+=0.2)
    {
        for(float32 y=field.boundMin()[1]; y<=field.boundMax()[1]; y+=0.2)
        {
            Vector2f vec = field.sample(x,y);
            vec.normalize();

            viewer->addLine(x, y, x + ArrowScale*vec[0], y + ArrowScale*vec[1]);
        }
    }

    viewer->refresh();
}

//Euler
void Task51::EulerStreamlines(){
	
	//Determining the start point
	Vector2f startPoint= makeVector2f(XStart, YStart);

	Vector2f currentPoint = startPoint;
	//Calculating the line between steps
	for(int i=0; i<EulerSteps; i++){
		
		float nextPointX;
		float nextPointY;

		Vector2f nextPoint;

		nextPointX= currentPoint[0] +EulerStepSize*(-1)*currentPoint[1];
		nextPointY= currentPoint[1] +EulerStepSize*currentPoint[0]/2;
		nextPoint= makeVector2f(nextPointX, nextPointY);

		viewer->addLine(currentPoint, nextPoint);
		viewer->addPoint(currentPoint);

		currentPoint=nextPoint;
	}

	//Render the Euler curve
	viewer->refresh();

}


//Runge-Kutta
void Task51::RungeKuttaStreamlines(){

}