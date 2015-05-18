//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task7.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"

IMPLEMENT_GEOX_CLASS( Task7, 0)
{
    BEGIN_CLASS_INIT( Task7 );

    ADD_SEPARATOR("Scalarfield")
    ADD_STRING_PROP(ScalarfieldFilename, 0)
    ADD_NOARGS_METHOD(Task7::DrawScalarField)

    ADD_SEPARATOR("Vectorfield")
    ADD_STRING_PROP(VectorfieldFilename, 0)
    ADD_FLOAT32_PROP(ArrowScale, 0)
    ADD_NOARGS_METHOD(Task7::DrawVectorField)

    ADD_SEPARATOR("Texture")
    ADD_STRING_PROP(ImageFilename, 0)
    ADD_BOOLEAN_PROP(bColoredTexture, 0)
	
	ADD_SEPARATOR("LIC")
	ADD_STRING_PROP(VectorfieldFilename, 0)
	ADD_INT32_PROP(SampleX, 0)
	ADD_INT32_PROP(SampleY, 0)
	ADD_FLOAT32_PROP(KernelSize, 0)
	ADD_INT32_PROP(Seed, 0)
	ADD_BOOLEAN_PROP(RandomTexture, 0)
	ADD_BOOLEAN_PROP(FastLIC, 0)
	ADD_BOOLEAN_PROP(AutoContrast, 0)
	ADD_BOOLEAN_PROP(BWTexture, 0)
	ADD_BOOLEAN_PROP(ScalarColor, 0)

	ADD_SEPARATOR("Texture")
    ADD_STRING_PROP(ImageFilename, 0)
    ADD_BOOLEAN_PROP(bColoredTexture, 0)
    
	ADD_NOARGS_METHOD(Task7::LoadGradientVectorField)
	ADD_NOARGS_METHOD(Task7::DrawTexture)
	ADD_NOARGS_METHOD(Task7::LIC)
	ADD_NOARGS_METHOD(Task7::EnhanceContrast)
	ADD_NOARGS_METHOD(Task7::RunFindingZero)
	ADD_NOARGS_METHOD(Task7::RunFindingZeroScalar)
	ADD_NOARGS_METHOD(Task7::ClassifyPoints)
	
}

QWidget* Task7::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task7::Task7()
{
    viewer = NULL;
    ScalarfieldFilename = "NoisyHill.am";
    VectorfieldFilename = "ANoise2CT4.am";
    ArrowScale = 0.1;
    ImageFilename = "";
    bColoredTexture = true;
	SFCheck=false;

	normal = true;
	printComments = true;
	MinSpeed = 0.001;

	//LIC and texture
	isLoaded = false;

    bColoredTexture = false;

	SampleX = 64;
	SampleY = 64;
	KernelSize = 10;
	Seed = 1;

	RandomTexture = true;
	FastLIC = false;
	AutoContrast = false;
	BWTexture = false;
	ScalarColor=true;

	r=0;
	w=0;

}

Task7::~Task7() {}

void Task7::RunFindingZero(){
	viewer->clear();
	//Load the vector field
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

	Vector2ui dimention=field.dims();

	float gapX= (abs(field.boundMax()[0]-field.boundMin()[0])/(field.dims()[0]-1));
	float gapY= (abs(field.boundMax()[1]-field.boundMin()[1])/(field.dims()[1]-1));

	Vector2f maxV= field.boundMax();
	float borderDiscount=.05;
	maxV[0]-=borderDiscount;
	maxV[1]-=borderDiscount;

	//Draw vector directions (constant length)
    for(float32 x=field.boundMin()[0]; x<=maxV[0]; x+=gapX)
    {
        for(float32 y=field.boundMin()[1]; y<=maxV[1]; y+=gapY)
        {
			Vector2f p1= makeVector2f(x, y+gapY);
			Vector2f p2= makeVector2f(x+gapX,y+gapY);
			Vector2f p3= makeVector2f(x+gapX, y);
			Vector2f p4= makeVector2f(x, y);
			
			FindingZeros(p1,p2,p3,p4);
			
		}
	}

	
	
	viewer->refresh();
}

void Task7::LoadGradientVectorField(){

	//Load scalar field
	if (!scalarField.load(ScalarfieldFilename))
    {
        output << "Error loading field file " << ScalarfieldFilename << "\n";
        return;
	}

	//Initializing gradient field of vectors
	field.init(makeVector2f(scalarField.boundMin()[0],scalarField.boundMin()[1]),makeVector2f(scalarField.boundMax()[0],scalarField.boundMax()[1]), makeVector2ui(scalarField.dims()[0], scalarField.dims()[1]));

	float gapX= (abs(field.boundMax()[0]-field.boundMin()[0])/(field.dims()[0]-1));
	float gapY= (abs(field.boundMax()[1]-field.boundMin()[1])/(field.dims()[1]-1));

	
	//Set the values at the vertices
	float y=field.boundMin()[1];
	for(size_t j=0; j<field.dims()[1]; j++, y+=gapY)
    {
		float x=field.boundMin()[0];
		for(size_t i=0; i<field.dims()[0]; i++, x+=gapX)
        {
			field.setNode(i, j, scalarField.sampleGradient(x,y));
        }
    }

}

void Task7::RunFindingZeroScalar(){
	viewer->clear();
	//Creating the vector field of gradients 
	LoadGradientVectorField();
	
	Vector2ui dimention=field.dims();

	float gapX= (abs(field.boundMax()[0]-field.boundMin()[0])/(field.dims()[0]-1));
	float gapY= (abs(field.boundMax()[1]-field.boundMin()[1])/(field.dims()[1]-1));

	Vector2f maxV= field.boundMax();
	float borderDiscount=.05;
	maxV[0]-=borderDiscount;
	maxV[1]-=borderDiscount;

	//Draw vector directions (constant length)
	for(float32 x=field.boundMin()[0]; x<=maxV[0]; x+=gapX)
    {
		for(float32 y=field.boundMin()[1]; y<=maxV[1]; y+=gapY)
        {
			Vector2f p1= makeVector2f(x, y+gapY);
			Vector2f p2= makeVector2f(x+gapX,y+gapY);
			Vector2f p3= makeVector2f(x+gapX, y);
			Vector2f p4= makeVector2f(x, y);
			
			FindingZeros(p1,p2,p3,p4);
			
		}
	}
	
	viewer->refresh();
}

void Task7::DrawScalarField()
{
    viewer->clear();

    //Load scalar field
	if (!scalarField.load(ScalarfieldFilename))
    {
        output << "Error loading field file " << ScalarfieldFilename << "\n";
        return;
    }

    //Get the minimum/maximum value in that scalarField
    float32 min = std::numeric_limits<float32>::max();
    float32 max = -std::numeric_limits<float32>::max();
	for(size_t j=0; j<scalarField.dims()[1]; j++)
    {
        for(size_t i=0; i< scalarField.dims()[0]; i++)
        {
            const float32 val = scalarField.nodeScalar(i,j);
            min = val < min ? val : min;
            max = val > max ? val : max;
        }
    }


    //Draw a point for each grid vertex.
    for(size_t j=0; j<scalarField.dims()[1]; j++)
    {
        for(size_t i=0; i<scalarField.dims()[0]; i++)
        {
            const float32 val = scalarField.nodeScalar(i, j);
            const float32 c = (val - min) / (max - min);

            Point2D p;
            p.position  = scalarField.nodePosition(i, j);
            p.size = 5;
            //Use a grayscale depending on the actual value
            p.color[0] = c; p.color[1] = c; p.color[2] = c;
            viewer->addPoint(p);
        }
    }

    viewer->refresh();
}

void Task7::DrawVectorField()
{
    viewer->clear();

    //Load the vector field
    VectorField2 field;
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

	//Draw boundrys
	viewer->addLine(gradientField.boundMin()[0], gradientField.boundMin()[1], gradientField.boundMin()[0], gradientField.boundMax()[1], makeVector4f(1,1,0.5,1), 2);
	viewer->addLine(gradientField.boundMin()[0], gradientField.boundMin()[1], gradientField.boundMax()[0], gradientField.boundMin()[1], makeVector4f(1,1,0.5,1), 2);
	viewer->addLine(gradientField.boundMin()[0], gradientField.boundMax()[1], gradientField.boundMax()[0], gradientField.boundMax()[1], makeVector4f(1,1,0.5,1), 2);
	viewer->addLine(gradientField.boundMax()[0], gradientField.boundMin()[1], gradientField.boundMax()[0], gradientField.boundMax()[1], makeVector4f(1,1,0.5,1), 2);

    //Draw vector directions (constant length)
    for(float32 x=gradientField.boundMin()[0]; x<=gradientField.boundMax()[0]; x+=0.08)
    {
        for(float32 y=gradientField.boundMin()[1]; y<=gradientField.boundMax()[1]; y+=0.08)
        {
            Vector2f vec = gradientField.sample(x,y);
           // vec.normalize();

			viewer->addLine(x, y, x + ArrowScale*vec[0], y + ArrowScale*vec[1]);

			/*//Adds points with colors depending on the orientation (may be useful for the algorithm, if not implemented yet)
			Point2D P(x,y);
			P.size  = 10;
			if (vec[0] > 0) {
				if (vec[1] > 0) {	// right-top
					P.color = makeVector4f(0,1,0,1);
				}
				else {				// right-bottom
					P.color = makeVector4f(0,1,1,1);
				}
			}
			else {
				if (vec[1] > 0) {	// left-top
					P.color = makeVector4f(1,1,0,1);
				}
				else {				// left-bottom
					P.color = makeVector4f(1,0,0,1);
				}
			}
			
			viewer->addPoint(P);*/
        }
    }

    viewer->refresh();
}

bool Task7::Sign(float num){
	if(num>0){
		return true;
	}else{
		return false;
	}
}

//Vector2f Task7::GetSampleField(bool scalarFieldCheck, float x, float y){
//	
//	Vector2f returnVector;
//	
//	if(scalarFieldCheck){
//	//Get sample from the gradient
//		returnVector=gradientField.sample(x,y);
//	
//	}else{
//	//Get the sample from the vector field
//		returnVector=field.sample(x,y);
//	}
//
//	return returnVector;
//};

void Task7::FindingZeros(Vector2f p1,Vector2f p2,Vector2f p3,Vector2f p4){

	//Points position
	//  p1___p2
	//  |    |
	//  p4___p3

	/*output<<p1<<" "<<p3<<"\n";*/
	
	float tolerance=.01;
	//Test the size of the "square" and see if should be divided
	Vector2f v12=makeVector2f(p1[0]-p2[0], p1[1]-p2[1]);
	float v12Size= sqrt(v12.getSqrNorm());
	Vector2f v14=makeVector2f(p1[0]-p4[0], p1[1]-p4[1]);
	float v14Size= sqrt(v14.getSqrNorm());
	
	if(v12Size<.000001|| v14Size<.000001){
		//Time to test if the central value is ZERO	or close to it	
		float cX=p1[0]+v12Size/2;
		float cY=p1[1]-v14Size/2;

		Vector2f centerValueVector=field.sample(cX,cY);
		float centerValueNorm=sqrt(centerValueVector.getSqrNorm());
		/*output<<"final: "<<p1<<" "<<p3<<"\n";
		output<<"center norm:"<<centerValueNorm<<"\n";*/

		if(centerValueNorm<.0001){
			//Add a point in the place since is zero 
			Vector2f centerPoint= makeVector2f(cX,cY);
			//output<<"center norm:"<<centerValueNorm<<"\n";
			Point2D P(centerPoint[0], centerPoint[1]);
			P.size=10;
			P.color = makeVector4f(1,0.5,0.5,1);
			viewer->addPoint(P);

			//Trick to avoid multiple points in the same critical point
			lastCriticalPoint=makeVector2f(centerPoint[0], centerPoint[1]);

			critPts.push_back(P); //Adds the point to global vector
		}	
		return;
	}

	//Collecting the sign of the points
	Vector2f p1V=field.sample(p1[0], p1[1]);
	Vector2f p2V=field.sample(p2[0], p2[1]);
	Vector2f p3V=field.sample(p3[0], p3[1]);
	Vector2f p4V=field.sample(p4[0], p4[1]);
	bool p1X=Sign(p1V[0]);
	bool p1Y=Sign(p1V[1]);
	bool p2X=Sign(p2V[0]);
	bool p2Y=Sign(p2V[1]);
	bool p3X=Sign(p3V[0]);
	bool p3Y=Sign(p3V[1]);
	bool p4X=Sign(p4V[0]);
	bool p4Y=Sign(p4V[1]);

	if(!(p1X==p2X && p1X==p3X && p1X==p4X && p2X==p3X && p2X==p4X && p3X==p4X) && 
		!(p1Y==p2Y && p1Y==p3Y && p1Y==p4Y && p2Y==p3Y && p2Y==p4Y && p3Y==p4Y)){
		//We should divide the area in four more
			
			Vector2f p1p2=makeVector2f(p1[0]+v12Size/2, p1[1]);
			Vector2f p1p4=makeVector2f(p1[0], p1[1]-v14Size/2);
			Vector2f p2p3=makeVector2f(p2[0], p2[1]-v14Size/2);
			Vector2f p3p4=makeVector2f(p4[0]+v12Size/2, p4[1]);
			Vector2f pCenter= makeVector2f(p1[0]+v12Size/2, p1[1]-v14Size/2);

			Vector2f d1V= makeVector2f(pCenter[0]-lastCriticalPoint[0],pCenter[1]-lastCriticalPoint[1]);
			float d1=sqrt(d1V.getSqrNorm());
			if(d1>tolerance)
				FindingZeros(p1,p1p2, pCenter,p1p4);
			
			d1V= makeVector2f(pCenter[0]-lastCriticalPoint[0],pCenter[1]-lastCriticalPoint[1]);
			d1=sqrt(d1V.getSqrNorm());
			if(d1>tolerance)
				FindingZeros(p1p2, p2,p2p3,pCenter);

			d1V= makeVector2f(pCenter[0]-lastCriticalPoint[0],pCenter[1]-lastCriticalPoint[1]);
			d1=sqrt(d1V.getSqrNorm());
			if(d1>tolerance)
				FindingZeros(pCenter, p2p3, p3, p3p4);

			d1V= makeVector2f(pCenter[0]-lastCriticalPoint[0],pCenter[1]-lastCriticalPoint[1]);
			d1=sqrt(d1V.getSqrNorm());
			if(d1>tolerance)
				FindingZeros(p1p4, pCenter,p3p4,p4);	
	}

}

void Task7::ClassifyPoints()
{
	for(int i=0; i < critPts.size(); i++) {
		//Sample Jacobian
		Matrix2f Jac = field.sampleJacobian(critPts[i].position[0], critPts[i].position[1]);
		if(Jac.getDeterminant() != 0) {
			// Compute Eigen values/vectors
			Vector2f eigenValsReal, eigenValsImg;
			Matrix2f eigenVectors;
			Jac.solveEigenProblem(eigenValsReal, eigenValsImg, eigenVectors);
			
			// Analyse and classify (colours)
			if(eigenValsImg[0] == 0 && eigenValsImg[1] == 0) {
				if(eigenValsReal[0] > 0 && eigenValsReal[1] > 0) { //Repelling node (source)
					critPts[i].color = makeVector4f(1,0,0,1);
				}
				else if(eigenValsReal[0] < 0 && eigenValsReal[1] < 0) { //Attracting node (sink)
					critPts[i].color = makeVector4f(0,0,1,1);
				}
				else {	//Saddle point
					critPts[i].color = makeVector4f(1,1,0,1);
					DrawSeparatrices(critPts[i], eigenVectors);
				}
			}
			else {
				if(eigenValsReal[0] == 0 && eigenValsReal[1] == 0) { //Center
					critPts[i].color = makeVector4f(0,1,0,1);
				}
				else if(eigenValsReal[0] == eigenValsReal[1] && eigenValsReal[0] > 0) { //Repelling focus
					critPts[i].color = makeVector4f(1,0.5,0,1);
				}
				else if(eigenValsReal[0] == eigenValsReal[1] && eigenValsReal[0] < 0) { //Attracting focus
					critPts[i].color = makeVector4f(0.5,0,1,1);
				}
			}
			viewer->addPoint(critPts[i]);
			
			//output << "Aloha!\n";

		}

		output<<r<<"\n";
		output<<w<<"\n";
	}
	
	viewer->refresh();

	//clear the values of the critPts
	if(!critPts.empty()){
		critPts.clear();
	}
	
}

//Runge-Kutta
void Task7::RungeKuttaStreamlinesVoid(VectorField2 field, float startX, float startY, float RKStepSize, bool backwards){

	// Different colors for forwards (green) and backwards (red)
	Vector4f RKcolor = backwards ? makeVector4f(1,0,0,1) : makeVector4f(0,1,0,1);
	
	//Defining the start point
	Vector2f startPoint = makeVector2f(startX,startY);
	//Vector2f startPoint = makeVector2f((field.boundMin()[0] + field.boundMax()[0])/2,(field.boundMin()[1] + field.boundMax()[1])/2); // starting in the middle
	//Vector2f startPoint = makeVector2f(field.boundMin()[0], field.boundMin()[1]);

	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	float RKStepSize2 = backwards ? -RKStepSize : RKStepSize; // if "backwards" is checked, inverts sign of integration step
	float a, b, length, speed;
	float arcLength = 0;
	bool outOfBounds = false;
	bool tooSlow = false;

	//Checks if initial point is out of boundaries
	if ((x[0] < field.boundMin()[0])||(x[0] > field.boundMax()[0])||(x[1] < field.boundMin()[1])||(x[1] > field.boundMax()[1])) {
		outOfBounds = true;
		if(printComments)
		  output << "Out of bounds! \n";
	}

	//for(int i = 0; ((i < RKSteps) && (arcLength < MaxLength) && (!outOfBounds) && (!tooSlow)); i++){
	for(int i = 0; ((i < 15000) && (!outOfBounds) && (!tooSlow)); i++){

	//The 4 vectors of th RK method
		Vector2f v1 = field.sample(x[0],x[1]);
		//v1.normalize();
		if(normal){
		v1.normalize();
		}

		Vector2f v2p = makeVector2f((x[0]+RKStepSize2*v1[0]/2),(x[1]+RKStepSize2*v1[1]/2));
		Vector2f v2 = field.sample(v2p[0],v2p[1]);
		if(normal){
		v2.normalize();
		}

		Vector2f v3p = makeVector2f((x[0]+RKStepSize2*v2[0]/2),(x[1]+RKStepSize2*v2[1]/2));
		Vector2f v3 = field.sample(v3p[0],v3p[1]);
		if(normal){
		v3.normalize();
		}

		Vector2f v4p = makeVector2f((x[0]+RKStepSize2*v3[0]),(x[1]+RKStepSize2*v3[1]));
		Vector2f v4 = field.sample(v4p[0],v4p[1]);
		if(normal){
		v4.normalize();
		}

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+RKStepSize2*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+RKStepSize2*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

	//Calculates arc length
		a = x1[0]-x[0];
		b = x1[1]-x[1];
		length = sqrt(pow(a,2)+pow(b,2));
		arcLength += length;


	//Calculates speed (actual vector size) by sampling
		/*Vector2f sampleVector = field.sample(x1[0], x1[1]);
		a = sampleVector[0]-x1[0];
		b = sampleVector[1]-x1[1];*/
			
		speed = sqrt(x1.getSqrNorm());
		//output << "speed: " << speed << "\n";
		
		//Checks boundary limits
		if ((x1[0] < field.boundMin()[0])||(x1[0] > field.boundMax()[0])||(x1[1] < field.boundMin()[1])||(x1[1] > field.boundMax()[1])) {
			outOfBounds = true;
			if(printComments)
			  output << "Out of bounds! \n";
		}
		else if (speed < MinSpeed) {
			tooSlow = true;
			if(printComments)
			  output << "Vector speed too slow... \n";
		}
		else {
			//viewer->addPoint(x);
			viewer->addLine(x, x1, RKcolor, 2);	
			x = x1;
		}
	}
	
}

//Runge-Kutta Single Step (test orientation)
bool Task7::RKSingleStep(VectorField2 field, float startX, float startY, float RKStepSize, bool backwards){

	// Different colors for forwards (green) and backwards (red)
	Vector4f RKcolor = backwards ? makeVector4f(1,0,0,1) : makeVector4f(0,1,0,1);
	
	//Defining the start point
	Vector2f startPoint = makeVector2f(startX,startY);
	//Vector2f startPoint = makeVector2f((field.boundMin()[0] + field.boundMax()[0])/2,(field.boundMin()[1] + field.boundMax()[1])/2); // starting in the middle
	//Vector2f startPoint = makeVector2f(field.boundMin()[0], field.boundMin()[1]);

	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	float RKStepSize2 = backwards ? -RKStepSize : RKStepSize; // if "backwards" is checked, inverts sign of integration step
	float a, b, length, speed;
	float arcLength = 0;
	bool outOfBounds = false;
	bool tooSlow = false;

	//Checks if initial point is out of boundaries
	if ((x[0] < field.boundMin()[0])||(x[0] > field.boundMax()[0])||(x[1] < field.boundMin()[1])||(x[1] > field.boundMax()[1])) {
		outOfBounds = true;
		if(printComments)
		  output << "Out of bounds! \n";
	}

	//for(int i = 0; ((i < RKSteps) && (arcLength < MaxLength) && (!outOfBounds) && (!tooSlow)); i++){
	for(int i = 0; ((i < 1) && (!outOfBounds) && (!tooSlow)); i++){

	//The 4 vectors of th RK method
		Vector2f v1 = field.sample(x[0],x[1]);
		//v1.normalize();
		if(normal){
		v1.normalize();
		}

		Vector2f v2p = makeVector2f((x[0]+RKStepSize2*v1[0]/2),(x[1]+RKStepSize2*v1[1]/2));
		Vector2f v2 = field.sample(v2p[0],v2p[1]);
		if(normal){
		v2.normalize();
		}

		Vector2f v3p = makeVector2f((x[0]+RKStepSize2*v2[0]/2),(x[1]+RKStepSize2*v2[1]/2));
		Vector2f v3 = field.sample(v3p[0],v3p[1]);
		if(normal){
		v3.normalize();
		}

		Vector2f v4p = makeVector2f((x[0]+RKStepSize2*v3[0]),(x[1]+RKStepSize2*v3[1]));
		Vector2f v4 = field.sample(v4p[0],v4p[1]);
		if(normal){
		v4.normalize();
		}

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+RKStepSize2*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+RKStepSize2*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

	//Calculates arc length
		a = x1[0]-x[0];
		b = x1[1]-x[1];
		length = sqrt(pow(a,2)+pow(b,2));
		arcLength += length;

	//Calculates speed (actual vector size) by sampling
		Vector2f sampleVector = field.sample(x1[0], x1[1]);
		a = sampleVector[0]-x1[0];
		b = sampleVector[1]-x1[1];
		speed = sqrt(pow(a,2)+pow(b,2));
		//output << "speed: " << speed << "\n";
		
		//Checks boundary limits
		if ((x1[0] < field.boundMin()[0])||(x1[0] > field.boundMax()[0])||(x1[1] < field.boundMin()[1])||(x1[1] > field.boundMax()[1])) {
			outOfBounds = true;
			if(printComments)
			  output << "Out of bounds! \n";
		}
		else if (speed < MinSpeed) {
			tooSlow = true;
			if(printComments)
			  output << "Vector speed too slow... \n";
		}
		else {
			//viewer->addPoint(x);
			viewer->addLine(x, x1, RKcolor, 2);	
			//x = x1;
			if (x1.getSqrNorm() > x.getSqrNorm()) 
				return true;
			else
				return false;
		}
	}
	
}

void Task7::DrawSeparatrices(Point2D P, Matrix2f eigenVecs)
{
	// Tests with single step RK to get orientation of the stream line
	
	//if (RKSingleStep(field, P.position[0]+0.000001*eigenVecs[0][0], P.position[1]+0.000001*eigenVecs[0][1], 0.01, false)) {
	//	//1st eigen vector direction
	//	RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[0][0], P.position[1]+0.01*eigenVecs[0][1], 0.01, false);	
	//	RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[0][0], P.position[1]-0.01*eigenVecs[0][1], 0.01, false);	
	//	//2nd eigen vector direction (backwards)
	//	RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[1][0], P.position[1]+0.01*eigenVecs[1][1], 0.01, true);	
	//	RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[1][0], P.position[1]-0.01*eigenVecs[1][1], 0.01, true);	
	//	r++;
	//}
	//else {
	//	//1st eigen vector direction (backwards)
	//	RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[0][0], P.position[1]+0.01*eigenVecs[0][1], 0.01, true);	
	//	RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[0][0], P.position[1]-0.01*eigenVecs[0][1], 0.01, true);	
	//	//2nd eigen vector direction
	//	RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[1][0], P.position[1]+0.01*eigenVecs[1][1], 0.01, false);	
	//	RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[1][0], P.position[1]-0.01*eigenVecs[1][1], 0.01, false);	
	//	w++;
	//}

		//1st eigen vector direction (backwards)
		RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[0][0], P.position[1]+0.01*eigenVecs[0][1], 0.01, true);	
		RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[0][0], P.position[1]+0.01*eigenVecs[0][1], 0.01, false);	
		//2nd eigen vector direction
		RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[1][0], P.position[1]-0.01*eigenVecs[1][1], 0.01, true);	
		RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[1][0], P.position[1]-0.01*eigenVecs[1][1], 0.01, false);

		//1st eigen vector direction (backwards)
		RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[0][0], P.position[1]-0.01*eigenVecs[0][1], 0.01, true);	
		RungeKuttaStreamlinesVoid(field, P.position[0]-0.01*eigenVecs[0][0], P.position[1]-0.01*eigenVecs[0][1], 0.01, false);	
		////2nd eigen vector direction
		RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[1][0], P.position[1]+0.01*eigenVecs[1][1], 0.01, true);	
		RungeKuttaStreamlinesVoid(field, P.position[0]+0.01*eigenVecs[1][0], P.position[1]+0.01*eigenVecs[1][1], 0.01, false);	

}

// LIC and Texture function declarations
//Class resposible for representing the values of the stream line points
class Cell{
	public:
	float val;
	float rval;
	float gval;
	float bval;
	int nVis;
};

namespace
{
    ///Returns the next power-of-two
    int32 NextPOT(int32 n)
    {
        n--;
        n |= n >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
        n |= n >> 2;   // and then or the results.
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n++;           // The result is a number of 1 bits equal to the number
                       // of bits in the original number, plus 1. That's the
                       // next highest power of 2.
        return n;
    }
}

void Task7::DrawTexture()
{
	LoadFiles();

	viewer->clear();

	////Load Vectorfield
 //   VectorField2 field;
 //   if (!field.load(VectorfieldFilename))
 //   {
 //       output << "Error loading field file " << VectorfieldFilename << "\n";
 //       return;
 //   }

	QImage image(ImageFilename.c_str());

    //Float dimensions for RandomTexture
	float fWidth = abs(field.boundMax()[0] - field.boundMin()[0]);
	float fHeight = abs(field.boundMax()[1] - field.boundMin()[1]);

	iWidth = SampleX;
	iHeight= SampleY;
	
/*	//Adjustments for the resolution
	if(fWidth > fHeight) {
		int factor = ceil(fWidth/fHeight);
		iHeight = minRes;
		iWidth  = (NextPOT(minRes*factor)>=1024) ? 1024 : NextPOT(minRes*factor); //Max is 1024
	}
	else {
		int factor = ceil(fHeight/fWidth);
		iWidth   = minRes;
		iHeight  = (NextPOT(minRes*factor)>=1024) ? 1024 : NextPOT(minRes*factor);
	}
*/	
	if(!RandomTexture) {
		//Load the texture using Qt
		
		//Get its (original) dimensions. Used as bounds later.
		//fWidth = (float)image.width();
		//fHeight = (float)image.height();

		//Resize to power-of-two and mirror.
		image = image.mirrored().scaled(NextPOT(image.width()), NextPOT(image.height()));

		//Get its new integer dimensions.
		iWidth = image.width();
		iHeight = image.height();
	}
	else {
		//Initialize random seed
		srand (Seed);
	}


    if (bColoredTexture)
    {
        //Create three color channels for the texture
        //Each of them is represented using a scalar field
        Red.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
        Green.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
        Blue.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));

        //Fill the scalar fields
        for(size_t j=0; j<Red.dims()[1]; j++)
        {
            for(size_t i=0; i<Red.dims()[0]; i++)
            {
                Red.setNodeScalar(i, j, (float)(qRed(image.pixel(i, j))) / 255.0 );
                Green.setNodeScalar(i, j, (float)(qGreen(image.pixel(i, j))) / 255.0 );
                Blue.setNodeScalar(i, j, (float)(qBlue(image.pixel(i, j))) / 255.0 );
            }
        }

        //Set the texture in the viewer
        viewer->setTextureRGB(Red.getData(), Green.getData(), Blue.getData());

	}else if(!RandomTexture && !bColoredTexture){
		//It generates grayish image

		Gray.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));

        //Set the values at the vertices
        for(size_t j=0; j<Gray.dims()[1]; j++)
        {
            for(size_t i=0; i<Gray.dims()[0]; i++)
            {
                Gray.setNodeScalar(i, j, (float)(qGray(image.pixel(i, j))) / 255.0 );
            }
        }

        //Set the texture in the viewer
        viewer->setTextureGray(Gray.getData());

	}else{
        //Create one gray color channel represented as a scalar field
		float randGray;
		int randBW=0;

        Gray.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));

        //Set the values at the vertices (i and j have to be integers)
        for(int j=0; j<Gray.dims()[1]; j++)
        {
            for(int i=0; i<Gray.dims()[0]; i++)
            {
                //Gray.setNodeScalar(i, j, (float)(qGray(image.pixel(i, j))) / 255.0 );
				
				// Generate random intensities

				if(!BWTexture){
					randGray = (rand() % 256) / 255.0;
					Gray.setNodeScalar(i, j, randGray);
				}
				else{
					randBW = rand() % 2;
					//output << randBW << "\n";
					Gray.setNodeScalar(i, j, (float)(randBW));
				}
            }
        }

        //Set the texture in the viewer
        viewer->setTextureGray(Gray.getData());
    }

    viewer->refresh();
}

//Load file and create data
void Task7::LoadFiles(){

	//viewer->clear();

	//Load the vector field
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

	isLoaded = true;
	
	//vector<pStream7> test=PositionStream(field, 0, 0,.05);
}

void Task7::LIC(){


	//output << "in LIC" << "\n";

	/* These fields are declared as global in order to be used in Contrast function
	ScalarField2 drawnGreyField;
	ScalarField2 drawnRedField;
	ScalarField2 drawnGreenField;
	ScalarField2 drawnBlueField;
	*/

	//Initializing scalar fields
	drawnGreyField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
	drawnRedField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
	drawnGreenField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
	drawnBlueField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
	
	float pixelSizeX= abs(field.boundMax()[0] - field.boundMin()[0])/iWidth;
	float pixelSizeY= abs(field.boundMax()[1] - field.boundMin()[1])/iHeight;

	//Get the minimum/maximum value in that field
    min = std::numeric_limits<float32>::max();
    max = -std::numeric_limits<float32>::max();
	for(float k=field.boundMin()[1]; k<field.boundMax()[1]; k=k+pixelSizeY)
    {
        for(float w=field.boundMin()[0]; w<field.boundMax()[0]; w=w+pixelSizeX)
        {
			Vector2f val = field.sample(k,w);
			float speed = sqrt(pow(val[0],2)+pow(val[1],2));
            min = speed < min ? speed : min;
            max = speed > max ? speed : max;
        }
    }
	/* Algorithm
	
	Loop through all points in vector field (pixels)
	{
		Integrate streamline from each pixel
			- Get a vector of positions for each streamline (Mariama)
			- Towards and backwards (according to defined kernel length)
			- Fixed step size: min(height, width) of pixel (?)
			- Store each step point in a vector stepPtsVector
			Loop through each point in stepPtsVector
			{
				Compute integral (LIC) over the stepPtsVector
			}

		Set pixel intensity with result from LIC
	}
	*/

	//Use LIC
	if(!FastLIC){
		viewer->clear();

		//Contrast-related variables
		accMeans = 0;
		accStdDev = 0;
		n = 0;

		float y;
		int j;
		//Iterate over the pixels in the vector field in order to draw the surface
		for(y=field.boundMin()[1], j=0; j<drawnGreyField.dims()[1]; y=y+pixelSizeY, j++){
			float x;
			int i;
			for(x=field.boundMin()[0], i=0; i<drawnGreyField.dims()[0]; x=x+pixelSizeX, i++){
		
				float pixelSize= (pixelSizeX+pixelSizeY)/2;
				float rmean = 0;
				float gmean = 0;
				float bmean = 0;
				float mean = 0;

				//Calling the function responsible to get the sum of -L to L in the coord
				vector<float> TextSum = SumStream(field, x, y, pixelSize, KernelSize);

				if(bColoredTexture){
					//for color
					rmean = TextSum[2]/TextSum[0];
					gmean = TextSum[3]/TextSum[0];
					bmean = TextSum[4]/TextSum[0];

					//Drawing the colored pixel on the screen
					drawnRedField.setNodeScalar(i, j, rmean);
					drawnGreenField.setNodeScalar(i, j, gmean);
					drawnBlueField.setNodeScalar(i, j, bmean);
				}
				else{
					mean = TextSum[1]/TextSum[0];
					drawnGreyField.setNodeScalar(i,j,mean);
				}


				if(!ScalarColor){
					//for grayscale and black&white
					drawnGreyField.setNodeScalar(i,j,mean);

					//Contrast-related variables
					if (mean > 0.01) {		//Non-black pixels = those > 0.01 (0 is total black) 
						n++;
						accMeans += mean;
						accStdDev += pow(mean, 2);
						//output << "mean= " << mean << "\n";
					}

				}
				else{
					//Necessary to able to apply the contrast later
					drawnGreyField.setNodeScalar(i,j,mean);
					//Contrast-related variables
					if (mean > 0.01) {		//Non-black pixels = those > 0.01 (0 is total black) 
						n++;
						accMeans += mean;
						accStdDev += pow(mean, 2);
						//output << "mean= " << mean << "\n";
					}

					//Get the color map and apply intensity of the mean
					vector<float> rgb=RGBValues(field, x, y, pixelSizeX, pixelSizeY);
					float R=rgb.at(0)*mean;
					float G=rgb.at(1)*mean;
					float B=rgb.at(2)*mean;

					//Drawing the colored pixel on the screen
					drawnRedField.setNodeScalar(i, j, R);
					drawnGreenField.setNodeScalar(i, j, G);
					drawnBlueField.setNodeScalar(i, j, B);
				}	
			}
		}
	}

	//Use FastLIC
	else{
		viewer->clear();
		float y;
		int j;
		float pixelSize= (pixelSizeX+pixelSizeY)/2;

		int arX = drawnGreyField.dims()[0];
		int arY = drawnGreyField.dims()[1];

		//output << "arX: " << arX << " , arY: " << arY <<"\n";

		//Array of the cells
		//Cell pixelArray[ff][rr];
		Cell **pixelArray;

		// Allocate memory
		pixelArray = new Cell*[arY];
			for (int i = 0; i < arY; ++i)
				pixelArray[i] = new Cell[arX];

		//Assigning values for each cell (otherwise undefinied)
		for(int cy=0; cy < arY; cy++){
			for(int cx=0; cx < arX; cx++){
				pixelArray[cy][cx].bval = 0;
				pixelArray[cy][cx].gval = 0;
				pixelArray[cy][cx].rval = 0;
				pixelArray[cy][cx].nVis = 0;
				pixelArray[cy][cx].val = 0;
			}
		}

		//output << "arX:" << arX << ", arY: " << arY  <<"\n";

		float cellSizeX = (field.boundMax()[0] - field.boundMin()[0]) /arX;
		float cellSizeY = (field.boundMax()[1] - field.boundMin()[1]) /arY;

		//Iterate over the pixels in the vector field to store the data in the cells
		for(y=field.boundMin()[1], j=0; j<arY; y=y+pixelSizeY, j++){
			//output << "in y \n";
			float x;
			int i;

			for(x=field.boundMin()[0], i=0;  i<arX; x=x+pixelSizeX, i++){
				//output << "in x \n";
				output << "j:" << j << " , i: " << i << "\n";
			//	output << " nvis:" << pixelArray[j][i].nVis << "\n";
			/*	output << "\n";
				output << "j " << j << ", i " << i << "\n";
				output << "y " << y - field.boundMin()[1] << ", x " << x - field.boundMin()[0]<< "\n";*/
				if(pixelArray[j][i].nVis == 0){

					//output <<"vis check ok \n";

					if(j == 47 && i ==8){
						printComments = true;
					}

					//get the streamline data
					vector<pStream7> data = PositionStream(field, x, y, pixelSize);
				
					//output <<"after data";
					//store data
					for(int k = 0; k < data.size(); k++){

						//calculating mean for the point
						float mean = 0;
						float npoints = 0;
						float forwKernelSize = 0;
						for(int forw=k; forwKernelSize < KernelSize && forw < data.size(); forw++){
							mean += data[forw].val;
							npoints++;
							forwKernelSize += pixelSize;
						}
						float backwKernelSize = 0;
						for(int backw=k-1; backw > 0 && backwKernelSize < KernelSize; backw--){
							mean += data[backw].val;
							npoints++;
							backwKernelSize += pixelSize;
						}									
						if(npoints!= 0){
							mean = mean/npoints;
						}
						else{
							output << "npoints0" << "\n";
						}


						//get the cell that the point belongs to
						vector<int> cellVec = GetCellValues(data[k].x, data[k].y, cellSizeX, cellSizeY, arX, arY);

						if(data[k].x == x){
							cellVec[0] = i;
						}

						if(data[k].y == y){
							cellVec[1] = j;
						}

						//output << "before cell aloc \n cell:" << cellVec[1] << ","<< cellVec[0] << "\n";
						//output << pixelArray[cellVec[1]][cellVec[0]].nVis << "\n";

						//adding values to the cell, adding to number of times visited
						pixelArray[cellVec[1]][cellVec[0]].val+= mean;
						pixelArray[cellVec[1]][cellVec[0]].nVis++;

						//output << "k " << k << "processed: \n";
					}
					//output << "cell processed:" << "\n";
				}
				//output << "\n";
				//output << "kol " << j << ", rad " << i << "\n";
				/*for(int m=0; m < arY; m++){
					for(int n=0; n < arX; n++){
						output << pixelArray[m][n].nVis << "   ;   ";
					}
					output << "\n";
				}
				output << "\n";*/
			}
			//output << "\n";
		}

		//output << "before draw \n";

		//Iterate over the cells in order to draw the surface
		for(j=0; j<arY; j++){
			int i;
			for(i=0; i<arX; i++){
				//Draw
				float finalMean = pixelArray[j][i].val;
				if(pixelArray[j][i].nVis != 0){
					finalMean = pixelArray[j][i].val/pixelArray[j][i].nVis;
				}
				else{
					output << "not been visited:  x: " << i << " , y: " << j << "\n";
				}
				drawnGreyField.setNodeScalar(i,j,finalMean);

				//Contrast-related variables
				if (finalMean > 0.01) {		//Non-black pixels = those > 0.01 (0 is total black) 
					n++;
					accMeans += finalMean;
					accStdDev += pow(finalMean, 2);
					//output << "mean= " << mean << "\n";
				}
			}
		}

		// De-Allocate memory to prevent memory leak
		for (int i = 0; i < arY; ++i)
			delete [] pixelArray[i];
		delete [] pixelArray;
	}
	
	//viewer->setTextureGray(drawnGreyField.getData());


	//Contrast final calculations
	oldMean = accMeans / n;
	oldStdDev = sqrt((accStdDev - n*pow(oldMean,2))/(n-1));
	
	//Set the texture
	if(!ScalarColor && !bColoredTexture){
		viewer->setTextureGray(drawnGreyField.getData());
	}else{
        viewer->setTextureRGB(drawnRedField.getData(), drawnGreenField.getData(), drawnBlueField.getData());
	}

	viewer->refresh();
}

//Calculate the cell from the point position
vector<int> Task7::GetCellValues(float posX, float posY, float maxX, float maxY, int celldimX, int celldimY){
	vector<int> cellvector;

	float tx = posX-field.boundMin()[0];
	float ty = posY-field.boundMin()[1];

	int xPosGrid = floor ((tx / maxX));			//hotfix for int converting
	int yPosGrid = floor ((ty / maxY));

	if(!(xPosGrid < celldimX)){

		xPosGrid = xPosGrid -1;
	}

	if(!(yPosGrid < celldimY)){
		//output << "\n y to large \n";
		yPosGrid = yPosGrid -1;
	}

	cellvector.push_back(xPosGrid);
	cellvector.push_back(yPosGrid);
	
	//output << "cellx Value: " << xPosGrid << "  , tx: " << tx << "  , xmax: " << maxX << "\n";
	//output << "celly Value: " << yPosGrid << "  , ty: " << ty << "  , ymax: " << maxY << "\n" << "\n";

	return cellvector;
}


//Calculating position of the stream line
vector<float> Task7::SumStream(VectorField2 field, float startX, float startY, float pixelSize, float L){
	
	//Final vector to be returned
	vector<float> sumVector;

	//Vectors that will contain the positions of the stream line (-L to L)
	vector<float> forward = RungeKuttaStreamlines(field, startX, startY, pixelSize, L, false);
	vector<float> backward = RungeKuttaStreamlines(field, startX, startY, pixelSize, L, true);
	
	////Get the Texture value of the point itself
	//			float redVal=0;
	//			float greenVal=0;
	//			float blueVal=0;
	//			float textVal=0;

	//		if(bColoredTexture){
	//			//get color texture value for each channel (rgb)
	//			redVal = (float) Red.sampleScalar(startX,startY);
	//			greenVal = (float) Green.sampleScalar(startX,startY);
	//			blueVal = (float) Blue.sampleScalar(startX,startY);
	//		}else{
	//			//Get the value for the Gray texture
	//			textVal = (float) Gray.sampleScalar(startX,startY);
	//		}

	sumVector.push_back((forward[0] + backward[0]));			//number of points
	sumVector.push_back((forward[1] + backward[1]));		//gray sum value
	sumVector.push_back((forward[2] + backward[2]));				//red sum value
	sumVector.push_back((forward[3] + backward[3]));				//green sum value
	sumVector.push_back((forward[4] + backward[4]));				//blue sum value

	return sumVector;
}


//Runge-Kutta
 vector<float> Task7::RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, float maxLength, bool backwards){

	//The vector to be returned with the number of points processed and subtotal
	vector<float> partSum;
	float nPoint = 0;
	float subtotal = 0;		//gray or black/white
	float redsubtotal = 0;
	float greensubtotal = 0;
	float bluesubtotal = 0;

	//Defining the start point
	Vector2f startPoint = makeVector2f(startX,startY);
	
	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	float RKStepSize2 = backwards ? -stepSize : stepSize; // if "backwards" is checked, inverts sign of integration step
	float a, b, length, speed;
	float arcLength = 0;
	bool outOfBounds = false;
	bool tooSlow = false;
	bool zeroSpeed = false;
	float MaxLength=maxLength;
	bool fix=false;

	//Checks if initial point is out of boundaries
	if ((x[0] < field.boundMin()[0])||(x[0] > field.boundMax()[0])||(x[1] < field.boundMin()[1])||(x[1] > field.boundMax()[1])) {
		outOfBounds = true;
	//	if(printComments)
	//	  output << "Out of bounds! \n";
	}

	//Verifing if starts at zeroSpeed
	Vector2f sampleVector = field.sample(x[0], x[1]);
	speed = sqrt(pow(sampleVector[0],2)+pow(sampleVector[1],2));

	if(speed==0){
		zeroSpeed=true;
	}

	//bugFix
	float past=0;
	for(int i = 0; ((arcLength < MaxLength) && (!outOfBounds) && (!tooSlow) && (!zeroSpeed) && (!fix)); i++){

	//The 4 vectors of th RK method
		Vector2f v1 = field.sample(x[0],x[1]);
		//float test=v1.getSqrNorm();
		v1.normalize();
		

		Vector2f v2p = makeVector2f((x[0]+RKStepSize2*v1[0]/2),(x[1]+RKStepSize2*v1[1]/2));
		Vector2f v2 = field.sample(v2p[0],v2p[1]);
		//test=v2.getSqrNorm();
		v2.normalize();
		

		Vector2f v3p = makeVector2f((x[0]+RKStepSize2*v2[0]/2),(x[1]+RKStepSize2*v2[1]/2));
		Vector2f v3 = field.sample(v3p[0],v3p[1]);
		//test=v3.getSqrNorm();
		v3.normalize();


		Vector2f v4p = makeVector2f((x[0]+RKStepSize2*v3[0]),(x[1]+RKStepSize2*v3[1]));
		Vector2f v4 = field.sample(v4p[0],v4p[1]);
		//test=v4.getSqrNorm();
		v4.normalize();

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+RKStepSize2*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+RKStepSize2*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

	//Calculates arc length
		a = x1[0]-x[0];
		b = x1[1]-x[1];
		length = sqrt(pow(a,2)+pow(b,2));
		arcLength += length;
	
		//Trick to fix crazy bug
		if(past>length && i!=0){
			if((past-length)>.001){
				fix=true;
			}
		}else{
			if((past-length)<-.001 && i!=0){
				fix=true;
			}
		}
		past=length;

	//Calculates speed (actual vector size) by sampling
		Vector2f sampleVector = field.sample(x1[0], x1[1]);
		a = sampleVector[0]-x1[0];
		b = sampleVector[1]-x1[1];
		speed = sqrt(pow(a,2)+pow(b,2));
		//output << "speed: " << speed << "\n";
		
		//Checks boundary limits
		if ((x1[0] < field.boundMin()[0])||(x1[0] > field.boundMax()[0])||(x1[1] < field.boundMin()[1])||(x1[1] > field.boundMax()[1])) {
			outOfBounds = true;
			//if(printComments)
			//  output << "Out of bounds! \n";
		}else if(speed==0){
			zeroSpeed=true;
			output<<"zero speed"<< "\n";
		}
		//else if (speed < MinSpeed) {
		//	tooSlow = true;
		////	//if(printComments)
		////	  //output << "Vector speed too slow... \n";
		//}
		else {
			//Adding the point in the vector of positions
			/*pStream7 point;
			point.x=x1[0];
			point.y=x1[1];
			
			positions.push_back(point);*/
			if(i==0 && backwards) {
				//Get the Texture value at the point	
				if(bColoredTexture){
					//get color texture value for each channel (rgb)
					float redVal = (float) Red.sampleScalar(x[0],x[1]);
					redsubtotal += redVal;
					float greenVal = (float) Green.sampleScalar(x[0],x[1]);
					greensubtotal += greenVal;
					float blueVal = (float) Blue.sampleScalar(x[0],x[1]);
					bluesubtotal += blueVal;
					nPoint++;
				}
				else{
					//Get the value for the Gray texture
					float textVal = (float) Gray.sampleScalar(x[0],x[1]);
					subtotal += textVal;
					nPoint++;
				}
			}


		//Get the Texture value at the point	
			if(bColoredTexture){
				//get color texture value for each channel (rgb)
				float redVal = (float) Red.sampleScalar(x1[0],x1[1]);
				redsubtotal += redVal;
				float greenVal = (float) Green.sampleScalar(x1[0],x1[1]);
				greensubtotal += greenVal;
				float blueVal = (float) Blue.sampleScalar(x1[0],x1[1]);
				bluesubtotal += blueVal;
				nPoint++;
			}
			else{
				//Get the value for the Gray texture
				float textVal = (float) Gray.sampleScalar(x1[0],x1[1]);
				subtotal += textVal;
				nPoint++;
			}
			x = x1;
		}
		
	}
	partSum.push_back(nPoint);
	partSum.push_back(subtotal);
	partSum.push_back(redsubtotal);
	partSum.push_back(greensubtotal);
	partSum.push_back(bluesubtotal);
	return partSum;
}

void Task7::EnhanceContrast(){
	viewer->clear();
	
	//Contrast definitions (slide 45, Lecture 07)
	newStdDev = 0.1;
	newMean	  = 0.5;	
	
	//New field for contrast
	contrastGrayField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
	contrastRedField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
	contrastGreenField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));
	contrastBlueField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));


	//Iterate over the pixels in the vector field in order to redraw the surface
	float pSizeX= abs(field.boundMax()[0] - field.boundMin()[0])/iWidth;
	float pSizeY= abs(field.boundMax()[1] - field.boundMin()[1])/iHeight;
	
	float y;
	int j;

	for(y=field.boundMin()[1], j=0; j<drawnGreyField.dims()[1]; y=y+pSizeY, j++){
		float x;
		int i;
		for(x=field.boundMin()[0], i=0; i<drawnGreyField.dims()[0]; x=x+pSizeX, i++){
			if(bColoredTexture){
				//for color (TODO)
					/*float rmean = ;
					float gmean = ;
					float bmean = ;*/
				}
			else{

				//for grayscale and black&white
				float oldValue = (float) drawnGreyField.sampleScalar(x,y);
				float newValue = newMean + (newStdDev / oldStdDev)*(oldValue - oldMean); // slide 44, Lecture 07

				if(!ScalarColor){
				contrastGrayField.setNodeScalar(i,j,newValue); // draws the output pixel
				//output << "oldValue= " << oldValue << "; newValue= " << newValue << "\n";
				}else{

				vector<float> rgb=RGBValues(field, x, y, pSizeX, pSizeY);

				float R=rgb.at(0)*newValue;
				float G=rgb.at(1)*newValue;
				float B=rgb.at(2)*newValue;

				contrastRedField.setNodeScalar(i,j,R);
				contrastGreenField.setNodeScalar(i,j,G);
				contrastBlueField.setNodeScalar(i,j,B);
				}
			}
		}

	}
	
	if(bColoredTexture || ScalarColor){
		viewer->setTextureRGB(contrastRedField.getData(), contrastGreenField.getData(), contrastBlueField.getData());
	}
	else{
		viewer->setTextureGray(contrastGrayField.getData());
	}

	viewer->refresh();

}


//Calculating position of the stream line
vector<pStream7> Task7::PositionStream(VectorField2 field, float startX, float startY, float pixelSize){
	
	//Final vector to be returned
	vector<pStream7> finalVector;

	//output <<"i PosStream \n";

	//Vectors that will contain the positions of the stream line (-L to L)
	vector<pStream7> forward = RungeKuttaStreamlines(field, startX, startY, pixelSize, false);

	//output <<"forw ok \n";

	vector<pStream7> backward = RungeKuttaStreamlines(field, startX, startY, pixelSize, true);


	//output <<"back ok \n";
	
	//Reserving memory for the two sets of position
	finalVector.reserve(forward.size()+backward.size()+1);
	
	//Adding backward
	finalVector.insert(finalVector.end(), backward.begin(), backward.end());

	//output <<"forw ins ok \n";

	//Adding a final point (the point byitself)
	pStream7 p;
	p.x=startX;
	p.y=startY;
	p.val = Gray.sampleScalar(startX,startY);
	finalVector.push_back(p);

	//output <<"point ok\n";

	//Adding forward
	finalVector.insert(finalVector.end(), forward.begin(), forward.end());

	//output <<"end PosStream \n";

	return finalVector;
}


//Runge-Kutta
 vector<pStream7> Task7::RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, bool backwards){

	 if(printComments){
		 output <<"i RK \n";
	 }

	 //The vector to be returned with the positions
	 vector<pStream7> positions;

	//Defining the start point
	Vector2f startPoint = makeVector2f(startX,startY);
	
	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	float RKStepSize2 = backwards ? -stepSize : stepSize; // if "backwards" is checked, inverts sign of integration step
	float a, b, length, speed;
	float arcLength = 0;
	bool outOfBounds = false;
	bool tooSlow = false;
	bool zeroSpeed = false;
	bool fix=false;

	//Checks if initial point is out of boundaries
	if ((x[0] < field.boundMin()[0])||(x[0] > field.boundMax()[0])||(x[1] < field.boundMin()[1])||(x[1] > field.boundMax()[1])) {
		outOfBounds = true;
		if(printComments)
		  output << "Out of bounds! \n";
		 printComments = false;
	}

	/*if(printComments){
		output << "after first bound check, before verifiing startspeed  \n";
	 }*/

	//Verifing if starts at zeroSpeed
	Vector2f sampleVector = field.sample(x[0], x[1]);
	speed = sqrt(pow(sampleVector[0],2)+pow(sampleVector[1],2));

	if(speed==0){
		zeroSpeed=true;
	}

	//bugFix
	float past=0;
	for(int i = 0; ((!outOfBounds) && (!tooSlow) && (!zeroSpeed) && (!fix)); i++){
		if(printComments){
			output << "i början på foor loop \n";
		}

	//The 4 vectors of th RK method
		Vector2f v1 = field.sample(x[0],x[1]);
		float test=v1.getSqrNorm();
		v1.normalize();
		

		Vector2f v2p = makeVector2f((x[0]+RKStepSize2*v1[0]/2),(x[1]+RKStepSize2*v1[1]/2));
		Vector2f v2 = field.sample(v2p[0],v2p[1]);
		v2.normalize();
		

		Vector2f v3p = makeVector2f((x[0]+RKStepSize2*v2[0]/2),(x[1]+RKStepSize2*v2[1]/2));
		Vector2f v3 = field.sample(v3p[0],v3p[1]);
		v3.normalize();


		Vector2f v4p = makeVector2f((x[0]+RKStepSize2*v3[0]),(x[1]+RKStepSize2*v3[1]));
		Vector2f v4 = field.sample(v4p[0],v4p[1]);
		v4.normalize();

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+RKStepSize2*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+RKStepSize2*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

	//Calculates arc length
		a = x1[0]-x[0];
		b = x1[1]-x[1];
		length = sqrt(pow(a,2)+pow(b,2));
		arcLength += length;
	
		//Trick to fix crazy bug
		if(past>length && i!=0){
			if((past-length)>.001){
				fix=true;
			}
		}else{
			if((past-length)<-.001 && i!=0){
				fix=true;
			}
		}
		past=length;

	/*	if(printComments){
			output << "before for-loop speed check  \n";
			output << "x1:" << x1[0] << ", y1: " << x1[1] << "  \n";
			//output << "sampleval: " << field.sample(x1[0], x1[1]) << "  \n";
		 }*/

	//Calculates speed (actual vector size) by sampling
		Vector2f sampleVector = field.sample(x1[0], x1[1]);
		a = sampleVector[0]-x1[0];
		b = sampleVector[1]-x1[1];
		speed = sqrt(pow(a,2)+pow(b,2));
		//output << "speed: " << speed << "\n";

		//Checks boundary limits
		if ((x1[0] < field.boundMin()[0])||(x1[0] > field.boundMax()[0])||(x1[1] < field.boundMin()[1])||(x1[1] > field.boundMax()[1])) {
			outOfBounds = true;
			/*if(printComments)
			  output << "Out of bounds! \n";*/
		}else if(speed==0){
			zeroSpeed=true;
			//output<<"zero speed"<< "\n";
		}
		//else if (speed < MinSpeed) {
		//	tooSlow = true;
		////	//if(printComments)
		////	  //output << "Vector speed too slow... \n";
		//}
		else {
			if(printComments){
				output << "al rk calc correct know add the point \n";
				output << "x1:" << x1[0] << ", y1: " << x1[1] << "  \n";
			}

			//Adding the point in the vector of positions
			pStream7 point;
			point.x=x1[0];
			point.y=x1[1];
			point.val = Gray.sampleScalar(x1[0],x1[1]);	

			if(printComments){
				output << "after val \n";
			}
			 
			if(!backwards){
				//forward
				if(printComments){
					output << "i forw alloc \n";
				}
				positions.push_back(point);
			}else{
				//backwards
				if(printComments){
					output << "i backw alloc \n";
				}
				positions.insert(positions.begin(),point);
			}
			x = x1;
		}
	}
	
	 if(printComments){
		output << "ben of rk  \n";
		printComments = false;
	 }

	return positions;
}

vector<float> Task7::RGBValues(VectorField2 field,float x, float y, float pixelSizeX, float pixelSizeY){


	Vector2f sampleVector = field.sample(x, y);
	float speed = sqrt(pow(sampleVector[0],2)+pow(sampleVector[1],2));
	float middleSpeed= speed/2;

	float R=0;
	float G=0;
	float B=0;

	if(speed!=0){
		//Defining the color of the pixel
		float dif= abs(max-min);
		float middle= dif/2;

		if(speed<(min+middle)){
		//Blue to Green
			G = ((speed-min)/middle); 
			B = (1-G);
						
		}else{
		//Green to Red
			R	= ((speed-min-middle)/middle);
			G = (1 - R);
		}
	}
	vector<float> returnVector;
	returnVector.push_back(R);
	returnVector.push_back(G);
	returnVector.push_back(B);

	return returnVector;
}
