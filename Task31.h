//---------------------------------------------------------------------------
#ifndef Task31H
#define Task31H
//---------------------------------------------------------------------------
#include "Experiment.h"
#include "LinearAlgebra.h"
#include "GLGeometryViewer.h"
#include "Field2.hpp"
//---------------------------------------------------------------------------


/// This is an example experiment.
///
/// The code is meant to demonstrate how
///  to use the GeoX framework
///
class Cell{
	public:
		float v1, v2, v3, v4;
		Vector2f p1, p2, p3, p4; 
};

class Task31 : public Experiment
{
    GEOX_CLASS(Task31)

//Constructor / Destructor
public:
    Task31();
    virtual ~Task31();

//Methods
public:
	void DrawGrid();
    void DrawIsoline();
	float calculateIntersection(float vMin, float vMax, float xMin, float xMax);
	void DrawIntersection(Cell cell);
	void LoadFile();
    virtual QWidget* createViewer();

//Attributes
public:
    ///File name of the scalar field
	vector<Cell> vectorCells;
	string ScalarfieldFilename;
	float isovalue;
	bool asymptotic;
	ScalarField2 field;

protected:
    GLGeometryViewer* viewer;
};


#endif
