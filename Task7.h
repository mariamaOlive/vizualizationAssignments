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
    void DrawTexture();
	void FindingZeros(Vector2f p1,Vector2f p2,Vector2f p3,Vector2f p4);
	void RunFindingZero();
	void ClassifyPoints();
	bool Sign(float num);
    virtual QWidget* createViewer();

//Attributes
public:
    ///File name of the scalar field
    string ScalarfieldFilename;

    ///File name of the vector field
    string VectorfieldFilename;

    ///Length of the arrows
    float ArrowScale;

    ///File name of the image for the texture
    string ImageFilename;

    ///Whether to draw the texture in RGB or grayscale
    bool bColoredTexture;

	//Vector field to be loaded
	VectorField2 field;

	//Vector of critical points
	Vector2f lastCriticalPoint;
	vector<Point2D> critPts;

protected:
    GLGeometryViewer* viewer;
};


#endif
