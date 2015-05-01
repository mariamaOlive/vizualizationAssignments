//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task52.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
#include "math.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"

IMPLEMENT_GEOX_CLASS( Task52, 0)
{
    BEGIN_CLASS_INIT( Task52 );

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

	ADD_BOOLEAN_PROP(backwards,0)


    ADD_NOARGS_METHOD(Task52::DrawVectorField)
	ADD_NOARGS_METHOD(Task52::EulerStreamlines)
	ADD_NOARGS_METHOD(Task52::RungeKuttaStreamlines)
}

QWidget* Task52::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task52::Task52()
{
    viewer = NULL;
    VectorfieldFilename = "Cylinderclose2CT10.am";
    ArrowScale = 0.1;

	//start values
	XStart = 1;
	YStart = 0;
	MaxDistance = 5.3;
	backwards = false;

	//Euler values
	EulerStepSize = 0.1;
	EulerSteps = 100;

	//Runge-Kutta values
	RKStepSize = 0.3;
	RKSteps = 30;
}

Task52::~Task52() {}

//From ExpampleExperimentField (remove later)
void Task52::DrawVectorField()
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
            //vec.normalize();

            viewer->addLine(x, y, x + ArrowScale*vec[0], y + ArrowScale*vec[1]);
        }
    }

    viewer->refresh();
}

//Euler
void Task52::EulerStreamlines(){
	
	//Determining the start point
	Vector2f startPoint= makeVector2f(XStart, YStart);

	Vector4f RKcolor = backwards ? makeVector4f(0,1,0,1) : makeVector4f(0,0,1,1);
	Vector2f currentPoint = startPoint;
	//Calculating the line between steps
	for(int i=0; i<EulerSteps; i++){
		
		float nextPointX;
		float nextPointY;

		Vector2f nextPoint;

		int sign = backwards ? -1 : 1;

		nextPointX= currentPoint[0] +EulerStepSize*(-sign)*currentPoint[1];
		nextPointY= currentPoint[1] +EulerStepSize*(sign)*currentPoint[0]/2;
		nextPoint= makeVector2f(nextPointX, nextPointY);

		viewer->addLine(currentPoint, nextPoint, RKcolor);
		viewer->addPoint(currentPoint);

		currentPoint=nextPoint;
	}

	//Render the Euler curve
	viewer->refresh();

}


//Runge-Kutta
void Task52::RungeKuttaStreamlines(){

	Vector4f RKcolor = backwards ? makeVector4f(0,1,1,1) : makeVector4f(1,0,0,1);
	Vector2f startPoint = makeVector2f(XStart,YStart);
	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	int sign = backwards ? -1 : 1;
	float a, b, length;
	float accLength = 0;


	for(int i = 0; i < RKSteps; i++){

	//The 4 vectors of th RK method
		Vector2f v1 = makeVector2f((-1)*x[1], x[0]/2);
		
		Vector2f v2p = makeVector2f((x[0]+sign*RKStepSize*v1[0]/2),(x[1]+sign*RKStepSize*v1[1]/2));
		Vector2f v2 = makeVector2f((-1)*v2p[1], v2p[0]/2);
		
		Vector2f v3p = makeVector2f((x[0]+sign*RKStepSize*v2[0]/2),(x[1]+sign*RKStepSize*v2[1]/2));
		Vector2f v3 = makeVector2f((-1)*v3p[1], v3p[0]/2);
		
		Vector2f v4p = makeVector2f((x[0]+sign*RKStepSize*v3[0]),(x[1]+sign*RKStepSize*v3[1]));
		Vector2f v4 = makeVector2f((-1)*v4p[1], v4p[0]/2);

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+sign*RKStepSize*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+sign*RKStepSize*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

		viewer->addPoint(x);
        viewer->addLine(x, x1, RKcolor, 2);
		a = x1[0]-x[0];
		b = x1[1]-x[1];
		length = sqrt(pow(a,2)+pow(b,2));
		accLength += length;
		output << "length: " << length << " , total: " << accLength << "\n";


		x = x1;
	}
	viewer->refresh();
}