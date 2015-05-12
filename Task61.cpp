//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task61.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"


#include <iostream>
using namespace std;

IMPLEMENT_GEOX_CLASS( Task61, 0)
{
    BEGIN_CLASS_INIT( Task61 );

	ADD_NOARGS_METHOD(Task61::LoadFiles)

    ADD_SEPARATOR("Vectorfield")
    ADD_FLOAT32_PROP(ArrowScale, 0)
    ADD_NOARGS_METHOD(Task61::DrawVectorField)

    ADD_SEPARATOR("Texture")
    ADD_STRING_PROP(ImageFilename, 0)
    ADD_BOOLEAN_PROP(bColoredTexture, 0)
    ADD_NOARGS_METHOD(Task61::DrawTexture)

	ADD_SEPARATOR("LIC")
	ADD_STRING_PROP(VectorfieldFilename, 0)
	ADD_INT32_PROP(SampleX, 0)
	ADD_INT32_PROP(SampleY, 0)
	ADD_FLOAT32_PROP(KernelSize, 0)
	ADD_INT32_PROP(minRes, 0)
	ADD_INT32_PROP(Seed, 0)
	ADD_BOOLEAN_PROP(RandomTexture, 0)
	ADD_BOOLEAN_PROP(FastLIC, 0)
	ADD_BOOLEAN_PROP(AutoContrast, 0)
	ADD_BOOLEAN_PROP(BWTexture, 0)
	ADD_BOOLEAN_PROP(ScalarColor, 0)

	ADD_NOARGS_METHOD(Task61::LIC)
	ADD_NOARGS_METHOD(Task61::EnhanceContrast)
	

}

QWidget* Task61::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task61::Task61()
{
    viewer = NULL;

    VectorfieldFilename = "Cylinderclose2CT10.am";

    ArrowScale = 0.1;
    ImageFilename = "";
    bColoredTexture = false;

	SampleX = 64;
	SampleY = 64;
	KernelSize = 0.1;
	Seed = 1;
	minRes = 8;

	RandomTexture = true;
	FastLIC = true;
	AutoContrast = false;
	BWTexture = false;
	ScalarColor=true;

	normal = true;
	printComments = false;
	MinSpeed = 0.1;
}

Task61::~Task61() {}

//Class resposible for representing the values of the stream line points
class Cell{
	public:
	float val;
	float rval;
	float gval;
	float bval;
	int nVis;
};

void Task61::DrawVectorField()
{
    //viewer->clear();

    //Load the vector field
    //if (!field.load(VectorfieldFilename))
    //{
    //    output << "Error loading field file " << VectorfieldFilename << "\n";
    //    return;
    //}

    //Draw vector directions (constant length)
    for(float32 x=field.boundMin()[0]; x<=field.boundMax()[0]; x+=0.02)
    {
        for(float32 y=field.boundMin()[1]; y<=field.boundMax()[1]; y+=0.02)
        {
            Vector2f vec = field.sample(x,y);
            vec.normalize();

            viewer->addLine(x, y, x + ArrowScale*vec[0], y + ArrowScale*vec[1]);
        }
    }

    viewer->refresh();
}

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

void Task61::DrawTexture()
{
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

	//Adjustments for the resolution
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
	
	//Integer dimensions for RandomTexture (desired resolution)
	//int iWidth = 256;
	//int iHeight = 512;

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
    }
    else
    {
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
void Task61::LoadFiles(){

	//viewer->clear();

	//Load the vector field
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

	//vector<pStream> test=PositionStream(field, 0, 0,.05);
}


void Task61::LIC(){
	//output << "in LIC" << "\n";

	ScalarField2 drawnGreyField;
	ScalarField2 drawnRedField;
	ScalarField2 drawnGreenField;
	ScalarField2 drawnBlueField;

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
					//Get scalar value of that point
					//In our case the norm of the vector
		
						Vector2f sampleVector = field.sample(x, y);
						float speed = sqrt(pow(sampleVector[0],2)+pow(sampleVector[1],2));
						float middleSpeed= speed/2;

						if(speed!=0){
						//Defining the color of the pixel
						float dif= abs(max-min);
						float middle= dif/2;

						float R=0;
						float G=0;
						float B=0;

						if(speed<(min+middle)){
						//Blue to Green
							G = ((speed-min)/middle)*mean; 
							B = (1-G)*mean;
						
						}else{
						//Green to Red
							R	= ((speed-min-middle)/middle)*mean;
							G = (1 - R)*mean;
						}

						//Drawing the colored pixel on the screen
						drawnRedField.setNodeScalar(i, j, R);
						drawnGreenField.setNodeScalar(i, j, G);
						drawnBlueField.setNodeScalar(i, j, B);
						}
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

		//Array of the cells
		//Cell pixelArray[ff][rr];
		Cell **pixelArray;

		// Allocate memory
		pixelArray = new Cell*[arX];
			for (int i = 0; i < arX; ++i)
				pixelArray[i] = new Cell[arY];

		//Assigning values for each cell (otherwise undefinied)
		for(int cx=0; cx < arX; cx++){
			for(int cy=0; cy < arY; cy++){
				pixelArray[cx][cy].bval = 0;
				pixelArray[cx][cy].gval = 0;
				pixelArray[cx][cy].rval = 0;
				pixelArray[cx][cy].nVis = 0;
				pixelArray[cx][cy].val = 0;
			}
		}

		float pixelSizeX = (field.boundMax()[0] - field.boundMin()[0]) /arX;
		float pixelSizeY = (field.boundMax()[1] - field.boundMin()[1]) /arY;

		//Iterate over the pixels in the vector field to store the data in the cells
		for(y=field.boundMin()[1], j=0; j<drawnGreyField.dims()[1]; y=y+pixelSizeY, j++){
			float x;
			int i;
			for(x=field.boundMin()[0], i=0; i<drawnGreyField.dims()[0] && pixelArray[i][j].nVis == 0; x=x+pixelSizeX, i++){
				//output << "in inner Fast: x= " << i << ", y=" << j << ", numVis: " << pixelArray[i][j].nVis << "\n";
				//get the streamline data
				vector<pStream> data = PositionStream(field, x, y, pixelSize);
				
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
					for(int backw=k; backw > 0 && backwKernelSize < KernelSize; backw--){
						mean += data[backw].val;
						npoints++;
						backwKernelSize += pixelSize;
					}										
					if(npoints!= 0){
						mean = mean/npoints;
					}

					//get the cell that the point belongs to
					vector<int> cellVec = GetCellValues(data[k].x, data[k].y, pixelSizeX, pixelSizeY);

					//adding values to the cell, adding to number of times visited
					pixelArray[cellVec[0]][cellVec[1]].val+= mean;
					pixelArray[cellVec[0]][cellVec[1]].nVis++;
					
				}
			}
		}
		//Iterate over the cells in order to draw the surface
		for(y=field.boundMin()[1], j=0; j<drawnGreyField.dims()[1]; y=y+pixelSizeY, j++){
			float x;
			int i;
			for(x=field.boundMin()[0], i=0; i<drawnGreyField.dims()[0]; x=x+pixelSizeX, i++){
				//Draw
				float mean = 0;
				if(pixelArray[i][j].nVis != 0){
					mean = pixelArray[i][j].val/pixelArray[i][j].nVis;
				}
				drawnGreyField.setNodeScalar(i,j,mean);
			}
		}

		// De-Allocate memory to prevent memory leak
		for (int i = 0; i < arX; ++i)
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
vector<int> Task61::GetCellValues(float posX, float posY, float pixelSizeX, float pixelSizeY){
	vector<int> cellvector;

	float tx = posX-field.boundMin()[0];
	float ty = posY-field.boundMin()[1];

	int xPosGrid = tx / pixelSizeX;
	int yPosGrid = ty / pixelSizeY;

	cellvector.push_back(xPosGrid);
	cellvector.push_back(yPosGrid);

	return cellvector;
}


//Calculating position of the stream line
vector<float> Task61::SumStream(VectorField2 field, float startX, float startY, float pixelSize, float L){
	
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
 vector<float> Task61::RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, float maxLength, bool backwards){

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
			/*pStream point;
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

void Task61::EnhanceContrast(){
	viewer->clear();
	
	//Contrast definitions (slide 45, Lecture 07)
	newStdDev = 0.1;
	newMean	  = 0.5;	
	
	contrastGrayField.init(makeVector2f(field.boundMin()[0], field.boundMin()[1]), makeVector2f(field.boundMax()[0], field.boundMax()[1]), makeVector2ui(iWidth, iHeight));

	//Iterate over the pixels in the vector field in order to redraw the surface
	float pixelSizeX=abs(field.boundMax()[0] - field.boundMin()[0])/iWidth;
	float pixelSizeY= abs(field.boundMax()[1] - field.boundMin()[1])/iHeight;
	
	float y;
	int j;

	for(y=field.boundMin()[1], j=0; j<drawnGreyField.dims()[1]; y=y+pixelSizeY, j++){
		float x;
		int i;
		for(x=field.boundMin()[0], i=0; i<drawnGreyField.dims()[0]; x=x+pixelSizeX, i++){
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
					contrastGrayField.setNodeScalar(i,j,newValue); // draws the output pixel
					//output << "oldValue= " << oldValue << "; newValue= " << newValue << "\n";
				}
		}

	}
	
	viewer->setTextureGray(contrastGrayField.getData());
	viewer->refresh();

}


//Calculating position of the stream line
vector<pStream> Task61::PositionStream(VectorField2 field, float startX, float startY, float pixelSize){
	
	//Final vector to be returned
	vector<pStream> finalVector;

	//Vectors that will contain the positions of the stream line (-L to L)
	vector<pStream> forward=RungeKuttaStreamlines(field, startX, startY, pixelSize, false);
	vector<pStream> backward=RungeKuttaStreamlines(field, startX, startY, pixelSize, true);
	
	//Reserving memory for the two sets of position
	finalVector.reserve(forward.size()+backward.size()+1);
	
	//Adding backward
	finalVector.insert(finalVector.end(), backward.begin(), backward.end());

	//Adding a final point (the point byitself)
	pStream p;
	p.x=startX;
	p.y=startY;
	p.val = Gray.sampleScalar(startX,startY);
	finalVector.push_back(p);

	//Adding forward
	finalVector.insert(finalVector.end(), forward.begin(), forward.end());

	return finalVector;
}


//Runge-Kutta
 vector<pStream> Task61::RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, bool backwards){

	 //The vector to be returned with the positions
	 vector<pStream> positions;

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
	for(int i = 0; ((!outOfBounds) && (!tooSlow) && (!zeroSpeed) && (!fix)); i++){

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
			  //output << "Out of bounds! \n";
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
			//Adding the point in the vector of positions
			pStream point;
			point.x=x1[0];
			point.y=x1[1];
			point.val = Gray.sampleScalar(x1[0],x1[1]);			
			 
			if(!backwards){
				//forward
				positions.push_back(point);
			}else{
				//backwards
				positions.insert(positions.begin(),point);
			}
			x = x1;
		}
	}
	
	return positions;
}





