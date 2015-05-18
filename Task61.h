//---------------------------------------------------------------------------
#ifndef Task61H
#define Task61H
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


//Class resposible for representing the values of the stream line points
class pStream{
	public:
	float x;
	float y;
	float val;
};


class Task61 : public Experiment
{
    GEOX_CLASS(Task61)

//Constructor / Destructor
public:
    Task61();
    virtual ~Task61();

//Methods
public:

    void DrawVectorField();
    void DrawTexture();
    virtual QWidget* createViewer();

	void LoadFiles();
	void LIC();
	void EnhanceContrast();
	vector<float> SumStream(VectorField2 field, float startX, float startY, float pixelSize, float L);
	vector<float> RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, float length, bool backwards);
	vector<pStream> RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, bool backwards);
	vector<pStream> PositionStream(VectorField2 field, float startX, float startY, float pixelSize);

	vector<int> Task61::GetCellValues(float posX, float posY, float pixelSizeX, float pixelSizeY, int celldimX, int celldimY);

	vector<float> Task61::RGBValues(VectorField2 field,float x, float y, float pixelSizeX, float pixelSizeY);
	void Task61::FindingZeros(Vector2f p1,Vector2f p2,Vector2f p3,Vector2f p4);
//Attributes
public:

    ///File name of the vector field
    string VectorfieldFilename;
	bool isLoaded;

	//Data
	ScalarField2 Gray;
	ScalarField2 Red;
	ScalarField2 Green;
	ScalarField2 Blue;
	VectorField2 field;
	ScalarField2 drawnGreyField;
	ScalarField2 drawnRedField;
	ScalarField2 drawnGreenField;
	ScalarField2 drawnBlueField;
	ScalarField2 contrastGrayField;
	ScalarField2 contrastRedField;
	ScalarField2 contrastGreenField;
	ScalarField2 contrastBlueField;

	//Values fo
	int SampleX;
	int SampleY;
	float KernelSize;
	int Seed;
	bool RandomTexture;
	bool FastLIC;
	bool AutoContrast;
	bool BWTexture;
	bool ScalarColor;

	//Variabels RK
	float MaxLength;
	float MinSpeed;
	bool backwards;
	bool printComments;
	bool normal;
	float RKStepSize;
	int RKSteps;

	//Resolution of texture
	int minRes;
	int iWidth;
	int iHeight;

	//Variables Enhancing Contrast
	int n;
	float accMeans;		// u*n
	float accStdDev;	// P
	float oldMean;		// u
	float oldStdDev;	// stdDev (sigma)
	float newMean;		// u'
	float newStdDev;	// stdDev'

    ///Length of the arrows
    float ArrowScale;

    ///File name of the image for the texture
    string ImageFilename;

    ///Whether to draw the texture in RGB or grayscale
    bool bColoredTexture;

	//Color paramenters
	float max;
	float min;

protected:
    GLGeometryViewer* viewer;
};


#endif
