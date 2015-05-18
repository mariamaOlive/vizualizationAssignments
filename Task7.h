//---------------------------------------------------------------------------
#ifndef Task7H
#define Task7H
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
class pStream7{
	public:
	float x;
	float y;
	float val;
};

class Task7 : public Experiment
{
    GEOX_CLASS(Task7)

//Constructor / Destructor
public:
    Task7();
    virtual ~Task7();

//Methods
public:
    void DrawScalarField();
    void DrawVectorField();
	void FindingZeros(Vector2f p1,Vector2f p2,Vector2f p3,Vector2f p4);
	void RunFindingZero();
	void ClassifyPoints();
	void RungeKuttaStreamlinesVoid(VectorField2 field, float startX, float startY, float RKStepSize, bool backwards);
	bool RKSingleStep(VectorField2 field, float startX, float startY, float RKStepSize, bool backwards);
	void DrawSeparatrices(Point2D P, Matrix2f eigenVecs);
	bool Sign(float num);
	Vector2f GetSampleField(bool scalarField, float X, float Y);
	void RunFindingZeroScalar();
	void LoadGradientVectorField();
    virtual QWidget* createViewer();

	//LIC and texture
	void DrawTexture();
	void LoadFiles();
	void LIC();
	void EnhanceContrast();
	vector<float> SumStream(VectorField2 field, float startX, float startY, float pixelSize, float L);
	vector<float> RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, float length, bool backwards);
	vector<pStream7> RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, bool backwards);
	vector<pStream7> PositionStream(VectorField2 field, float startX, float startY, float pixelSize);
	vector<int> Task7::GetCellValues(float posX, float posY, float pixelSizeX, float pixelSizeY, int celldimX, int celldimY);
	vector<float> Task7::RGBValues(VectorField2 field,float x, float y, float pixelSizeX, float pixelSizeY);



//Attributes
public:
    ///File name of the scalar field
    string ScalarfieldFilename;

    ///File name of the vector field
    string VectorfieldFilename;

    ///Length of the arrows
    float ArrowScale;
	
	//Vector field to be loaded
	VectorField2 field;
	//Scalar field to be loaded
	ScalarField2 scalarField;
	VectorField2 gradientField;
	//Boolean choosen if scalar field selected
	bool SFCheck;

	//Vector of critical points
	Vector2f lastCriticalPoint;
	vector<Point2D> critPts;

	//RK variables
	bool printComments;
	bool normal;
	float MinSpeed;
	bool fix;
	int r,w;

	// **** LIC and Texture variables (from Task7) ****
	
	///File name of the vector field
	bool isLoaded;

	//Data
	ScalarField2 Gray;
	ScalarField2 Red;
	ScalarField2 Green;
	ScalarField2 Blue;
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

	///File name of the image for the texture
    string ImageFilename;

	///Whether to draw the texture in RGB or grayscale
    bool bColoredTexture;

	//Color paramenters
	float max;
	float min;

	// **** end of LIC and Texture variables (from Task7) ****

protected:
    GLGeometryViewer* viewer;
};


#endif
