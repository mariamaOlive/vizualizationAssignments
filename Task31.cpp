//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task31.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"

IMPLEMENT_GEOX_CLASS( Task31, 0)
{
    BEGIN_CLASS_INIT( Task31 );

    ADD_SEPARATOR("Scalarfield")
    ADD_STRING_PROP(ScalarfieldFilename, 0)
	ADD_FLOAT32_PROP(isovalue,0)
    ADD_NOARGS_METHOD(Task31::DrawScalarField)
}

QWidget* Task31::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}


Task31::Task31()
{
    viewer = NULL;
    ScalarfieldFilename = "SimpleGrid.am";
	isovalue = 1;
}

Task31::~Task31() {}

void Task31::DrawGrid(ScalarField2 field){
	Vector2f minBox= makeVector2f(field.boundMin()[0], field.boundMin()[1]);
	Vector2f maxBox= makeVector2f(field.boundMax()[0], field.boundMax()[1]);

	//Adding boundaries of the grid (boundBox)
	viewer->addLine(makeVector2f(minBox[0],minBox[1]), makeVector2f(maxBox[0],minBox[1]));
	viewer->addLine(makeVector2f(maxBox[0],minBox[1]), makeVector2f(maxBox[0],maxBox[1]));
	viewer->addLine(makeVector2f(maxBox[0],maxBox[1]), makeVector2f(minBox[0],maxBox[1]));
	viewer->addLine(makeVector2f(minBox[0],maxBox[1]), makeVector2f(minBox[0],minBox[1]));

	//Adding grid line horizontally
	for(int i=1;i<field.dims()[1]-1;i++){
		Vector2f p1= field.nodePosition(0,i);	
		Vector2f p2= field.nodePosition(field.dims()[0]-1,i);
	
		viewer->addLine(p1,p2);
	}

	//Adding grid line vertically
	for(int i=1;i<field.dims()[0]-1;i++){
		Vector2f p1= field.nodePosition(i, 0);	
		Vector2f p2= field.nodePosition(i,field.dims()[1]-1);
	
		viewer->addLine(p1,p2);
	}

    viewer->refresh();
}

void Task31::DrawScalarField()
{
    viewer->clear();

    //Load scalar field
    ScalarField2 field;
    if (!field.load(ScalarfieldFilename))
    {
        output << "Error loading field file " << ScalarfieldFilename << "\n";
        return;
    }

	DrawGrid(field);

    //Get the minimum/maximum value in that field
    /*float32 min = std::numeric_limits<float32>::max();
    float32 max = -std::numeric_limits<float32>::max();
    for(size_t j=0; j<field.dims()[1]; j++)
    {
        for(size_t i=0; i< field.dims()[0]; i++)
        {
            const float32 val = field.nodeScalar(i,j);
            min = val < min ? val : min;
            max = val > max ? val : max;
        }
    }

    //Draw a point for each grid vertex.
    for(size_t j=0; j<field.dims()[1]; j++)
    {
        for(size_t i=0; i<field.dims()[0]; i++)
        {
            const float32 val = field.nodeScalar(i, j);
            const float32 c = (val - min) / (max - min);

            Point2D p;
            p.position  = field.nodePosition(i, j);
            p.size = 5;
            //Use a grayscale depending on the actual value
            p.color[0] = c; p.color[1] = c; p.color[2] = c;
            viewer->addPoint(p);
        }
    }*/

	//Loading cell into a vector

	for(int i=0; i<field.dims()[0]-1;i++){		
		for(int j=0; j<field.dims()[1]-1; j++){
			Cell cell;
			//loading the values of each point
			cell.v1=field.nodeScalar(i,j);
			cell.v2=field.nodeScalar(i+1,j);
			cell.v3=field.nodeScalar(i+1,j+1);
			cell.v4=field.nodeScalar(i,j+1);
			//loading the position of each point
			cell.p1=field.nodePosition(i,j);
			cell.p2=field.nodePosition(i+1,j);
			cell.p3=field.nodePosition(i+1,j+1);
			cell.p4=field.nodePosition(i,j+1);
			vectorCells.push_back(cell);
		}
	}

	int k=0;

	//for each cell, get value for each of the four points and compare them to isovalue, (true/false smaller or bigger?)
	// if all are smaller/bigger do nothing, if one point is diffrent, if two point are diffrent (check for diagonal case)


}


