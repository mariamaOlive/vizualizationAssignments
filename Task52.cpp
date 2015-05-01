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

	ADD_SEPARATOR("RungeKutta")
	ADD_FLOAT32_PROP(RKStepSize,0)
	ADD_INT32_PROP(RKSteps,0)  

	ADD_BOOLEAN_PROP(backwards,0)

    ADD_NOARGS_METHOD(Task52::DrawVectorField)
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

	//Runge-Kutta values
	RKStepSize = 0.3;
	RKSteps = 30;
}

Task52::~Task52() {}

//From ExpampleExperimentField (remove later)
void Task52::DrawVectorField()
{
    viewer->clear();

	VectorField2 field;

    //Load the vector field
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

    //Draw vector directions (constant length)
    for(float32 x=field.boundMin()[0]; x<=field.boundMax()[0]; x+=0.1)
    {
        for(float32 y=field.boundMin()[1]; y<=field.boundMax()[1]; y+=0.1)
        {
            Vector2f vec = field.sample(x,y);
            //vec.normalize();

            viewer->addLine(x, y, x + ArrowScale*vec[0], y + ArrowScale*vec[1]);
        }
    }

    viewer->refresh();
}

//Runge-Kutta
void Task52::RungeKuttaStreamlines(){

	VectorField2 field;

    //Load the vector field
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

	// Different colors for forwards (green) and backwards (red)
	Vector4f RKcolor = backwards ? makeVector4f(1,0,0,1) : makeVector4f(0,1,0,1);
	
	//Defining the start point
	Vector2f startPoint = makeVector2f((field.boundMin()[0] + field.boundMax()[0])/2,(field.boundMin()[1] + field.boundMax()[1])/2);

	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	RKStepSize = backwards ? -RKStepSize : RKStepSize; // if "backwards" is checked, inverts sign of integration step
	float a, b, length;
	float accLength = 0;

	for(int i = 0; i < RKSteps; i++){

	//The 4 vectors of th RK method
		Vector2f v1 = field.sample(x[0],x[1]);
		v1.normalize();
		
		Vector2f v2p = makeVector2f((x[0]+RKStepSize*v1[0]/2),(x[1]+RKStepSize*v1[1]/2));
		Vector2f v2 = field.sample(v2p[0],v2p[1]);
		v2.normalize();

		Vector2f v3p = makeVector2f((x[0]+RKStepSize*v2[0]/2),(x[1]+RKStepSize*v2[1]/2));
		Vector2f v3 = field.sample(v3p[0],v3p[1]);
		v3.normalize();

		Vector2f v4p = makeVector2f((x[0]+RKStepSize*v3[0]),(x[1]+RKStepSize*v3[1]));
		Vector2f v4 = field.sample(v4p[0],v4p[1]);
		v4.normalize();

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+RKStepSize*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+RKStepSize*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

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