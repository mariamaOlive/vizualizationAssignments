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
	ADD_FLOAT32_PROP(XStart, 0)
	ADD_FLOAT32_PROP(YStart, 0)

	ADD_SEPARATOR("Euler")	
	ADD_FLOAT32_PROP(EulerStepSize,0)
	ADD_INT32_PROP(EulerSteps,0)

	ADD_SEPARATOR("RungeKutta")
	ADD_FLOAT32_PROP(RKStepSize,0)
	ADD_INT32_PROP(RKSteps,0)  

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

	//start values
	XStart = 1;
	YStart = 0;

	//Euler values
	EulerStepSize = 0.1;
	EulerSteps = 100;

	//Runge-Kutta values
	RKStepSize = 0.3;
	RKSteps = 30;
}

Task51::~Task51() {}

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

	Vector4f RKcolor = makeVector4f(1,0,0,1);
	Vector2f startPoint = makeVector2f(XStart,YStart);
	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	for(int i = 0; i < RKSteps; i++){
	
	//The 4 vectors of th RK method
		Vector2f v1 = makeVector2f((-1)*x[1], x[0]/2);
		
		Vector2f v2p = makeVector2f((x[0]+RKStepSize*v1[0]/2),(x[1]+RKStepSize*v1[1]/2));
		Vector2f v2 = makeVector2f((-1)*v2p[1], v2p[0]/2);
		
		Vector2f v3p = makeVector2f((x[0]+RKStepSize*v2[0]/2),(x[1]+RKStepSize*v2[1]/2));
		Vector2f v3 = makeVector2f((-1)*v3p[1], v3p[0]/2);
		
		Vector2f v4p = makeVector2f((x[0]+RKStepSize*v3[0]),(x[1]+RKStepSize*v3[1]));
		Vector2f v4 = makeVector2f((-1)*v4p[1], v4p[0]/2);

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+RKStepSize*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+RKStepSize*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

		viewer->addPoint(x);
		viewer->addLine(x, x1, RKcolor);

		x = x1;
	}
	viewer->refresh();
}