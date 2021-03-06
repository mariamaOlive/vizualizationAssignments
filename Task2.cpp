//---------------------------------------------// 
#include "stdafx.h"                             
//---------------------------------------------//
#include "Task2.h"                
//---------------------------------------------// 
#include "Properties.h"      
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"                         
//---------------------------------------------//
                                             

///        Task2: Part1               //
///		Group members: Terese Nothnagel, Mariama Oliveira, Lui Pillmann de Barros	//


IMPLEMENT_GEOX_CLASS( Task2, 0 )    
{                                               
	BEGIN_CLASS_INIT( Task2 );       

	//input number of points
	ADD_INT32_PROP(NumSamples, 0)

	//input values slope
	ADD_SEPARATOR("Negative slope")
	ADD_FLOAT32_PROP(slope, 0)
	ADD_FLOAT32_PROP(Yorigo,0)

	//input circle values
	ADD_SEPARATOR("Circle")
	ADD_FLOAT32_PROP(Radius, 0)
	ADD_VECTOR2F_PROP(Center, 0)

	//input hyperbola
	ADD_SEPARATOR("Hyperbola")
	ADD_FLOAT32_PROP(a, 0)
	ADD_FLOAT32_PROP(b, 0)
	ADD_FLOAT32_PROP(minu, 0)
	ADD_FLOAT32_PROP(maxu, 0)


	ADD_NOARGS_METHOD(Task2::NegativeSlope)   
	ADD_NOARGS_METHOD(Task2::DrawCircle)
	ADD_NOARGS_METHOD(Task2::DrawHyperbola)
}                                               

QWidget* Task2::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task2::Task2()          
{          
	viewer = NULL;

	//scatterplot axis
	scatterOrigin = makeVector2f(-1.5,0.0);
	endAxisX = makeVector2f(12,0.0);
	endAxisY = makeVector2f(-1.5,13.5);
	endAxisYDown = makeVector2f(-1.5,-13.5);
	endAxisXLeft= makeVector2f(-13.5,0.0);

	//parallel cord axis
	startX1 = makeVector2f(17.5,-13.5);
	endX1 = makeVector2f(17.5,13.5);
	startX2 = makeVector2f(20,-13.5);
	endX2 = makeVector2f(20,13.5);

	NumSamples = 10;

	//slope values
	slope = -1;
	Yorigo = 13.5;

	//circle values
	Radius = 1;
	Center = makeVector2f(0,0);

	//hyperbola values
	a = 1;
	b = 1;
	minu = -3;
	maxu = 3;
}                                               
                                              
Task2::~Task2()        
{             
}                                               
                                               
void Task2::CreateAxis()        
{                        
	Vector4f color = makeVector4f(1,1,1,1);
	//scatter plot axes
	viewer->addLine(scatterOrigin, endAxisX, color, 2);
	viewer->addLine(scatterOrigin, endAxisY, color, 2);
	viewer->addLine(scatterOrigin, endAxisYDown, color, 2);
	viewer->addLine(scatterOrigin, endAxisXLeft, color, 2);

	//parallel coordinates plot "axes"
	viewer->addLine(startX1, endX1, color, 2);
	viewer->addLine(startX2, endX2, color, 2);
}    

void Task2::CreateParaline(float xvalue, float yvalue){
	//make parallel line between points
	Vector2f X1 = makeVector2f(startX1[0],(xvalue-scatterOrigin[0]));
	Vector2f Y1 = makeVector2f(startX2[0],(yvalue-scatterOrigin[1]));

	viewer->addLine( X1, Y1);
}

void Task2::NegativeSlope()
{
	viewer->clear();
	CreateAxis();

	float pointX= scatterOrigin[0];
	float pointY= Yorigo;

	Vector4f color=makeVector4f(1,0,0,1);
	int size= 5;
	
	float pace=(endAxisX[0]-scatterOrigin[0])/NumSamples;

	for(int i=0; i<=NumSamples; i++){ 
		Vector2f scatterPoint= makeVector2f(pointX,pointY);
		viewer->addPoint(scatterPoint);

		CreateParaline(pointX, pointY);

		pointX+= pace;
		pointY= slope*(pointX-scatterOrigin[0])+Yorigo;
	}

	// display changes
    viewer->refresh();
}

void Task2::DrawCircle(){
	viewer->clear();
	CreateAxis();
	
	//Traslating the point of center to the right coordinate
	float xCenter = Center[0]+scatterOrigin[0];
	float yCenter = Center[1]+scatterOrigin[1];

	//Create a circle by iterating over 360 degrees.
	for(int i=1; i < NumSamples; i++){

		float pointX = xCenter + Radius * cos(2 * 3.14159265 * float(i)/float(NumSamples-1));
		float pointY = yCenter + Radius * sin(2 * 3.14159265 * float(i)/float(NumSamples-1));
		//radiens not degrees
		const Vector2f A = makeVector2f(pointX,pointY);
		
		CreateParaline(pointX,pointY);	

		viewer->addPoint(A);  
	}

	// display changes
    viewer->refresh();

}

void Task2::DrawHyperbola(){
	viewer->clear();
	CreateAxis();
	
	float u= minu;
	//Create a hyperbola
	for(int i=0; i < NumSamples; i++){
		u = u + (maxu-minu)/NumSamples;

		//Relative to the axis X
		float pointX = (a * cosh(u));
		float pointX2 = -(a * cosh(u));
		float pointY =(b * sinh(u));

		double rad = 45*3.1415926/180;

		float newX = pointX * cos(rad) - pointY * sin(rad) +scatterOrigin[0];
		float newX2 = pointX2 * cos(rad) - pointY * sin(rad)+scatterOrigin[0];
		float newY = pointY * cos(rad) + pointX * sin(rad)+scatterOrigin[1];
		float newY2 = pointY * cos(rad) + pointX2 * sin(rad)+scatterOrigin[1];

		const Vector2f A = makeVector2f(newX,newY);
		const Vector2f B= makeVector2f(newX2,newY2);
			
		CreateParaline(newX,newY);
		CreateParaline(newX2,newY2);

		//Drawing the points on the screen
		viewer->addPoint(A); 
		viewer->addPoint(B);
	}

	// display changes
    viewer->refresh();
}
